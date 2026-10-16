#include "../include/file_io.h"

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_BUFFER_SIZE 512

static void trim_line_end(char *line) {
    size_t length = strlen(line);
    while (length > 0 && (line[length - 1] == '\n' || line[length - 1] == '\r')) {
        line[--length] = '\0';
    }
}

static int parse_int(const char *text, int *value) {
    char *end;
    long parsed;

    errno = 0;
    parsed = strtol(text, &end, 10);
    if (errno == ERANGE || end == text || *end != '\0' || parsed < 0 || parsed > INT_MAX) {
        return STATUS_ERR_FILE_IO;
    }
    *value = (int)parsed;
    return STATUS_SUCCESS;
}

static int parse_price(const char *text, double *value) {
    char *end;
    double parsed;

    errno = 0;
    parsed = strtod(text, &end);
    if (errno == ERANGE || end == text || *end != '\0' || parsed < 0.0 || !isfinite(parsed)) {
        return STATUS_ERR_FILE_IO;
    }
    *value = parsed;
    return STATUS_SUCCESS;
}

static int parse_product_line(char *line, Product *product) {
    char *tokens[8];
    size_t token_count = 0;
    char *cursor = line;

    memset(product, 0, sizeof(*product));
    while (cursor != NULL && token_count < 8) {
        tokens[token_count++] = cursor;
        char *pipe = strchr(cursor, '|');
        if (pipe != NULL) {
            *pipe = '\0';
            cursor = pipe + 1;
        } else {
            cursor = NULL;
        }
    }

    if (token_count == 6) {
        if (strlen(tokens[0]) >= sizeof(product->id) ||
            strlen(tokens[1]) >= sizeof(product->name) ||
            strlen(tokens[2]) >= sizeof(product->category) ||
            strlen(tokens[3]) >= sizeof(product->unit)) {
            return STATUS_ERR_FILE_IO;
        }
        snprintf(product->id, sizeof(product->id), "%s", tokens[0]);
        snprintf(product->name, sizeof(product->name), "%s", tokens[1]);
        snprintf(product->category, sizeof(product->category), "%s", tokens[2]);
        snprintf(product->unit, sizeof(product->unit), "%s", tokens[3]);
        if (parse_int(tokens[4], &product->quantity) != STATUS_SUCCESS) return STATUS_ERR_FILE_IO;
        if (parse_price(tokens[5], &product->price) != STATUS_SUCCESS) return STATUS_ERR_FILE_IO;
        return STATUS_SUCCESS;
    } else if (token_count == 4) {
        if (strlen(tokens[0]) >= sizeof(product->id) ||
            strlen(tokens[1]) >= sizeof(product->name)) {
            return STATUS_ERR_FILE_IO;
        }
        snprintf(product->id, sizeof(product->id), "%s", tokens[0]);
        snprintf(product->name, sizeof(product->name), "%s", tokens[1]);
        snprintf(product->category, sizeof(product->category), "ChuaPhanLoai");
        snprintf(product->unit, sizeof(product->unit), "Cai");
        if (parse_int(tokens[2], &product->quantity) != STATUS_SUCCESS) return STATUS_ERR_FILE_IO;
        if (parse_price(tokens[3], &product->price) != STATUS_SUCCESS) return STATUS_ERR_FILE_IO;
        return STATUS_SUCCESS;
    }

    return STATUS_ERR_FILE_IO;
}

int load_products_from_file(Inventory *inventory, const char *path) {
    FILE *file;
    char line[LINE_BUFFER_SIZE];
    int status = STATUS_SUCCESS;

    if (inventory == NULL || path == NULL) {
        return STATUS_ERR_NULL_PTR;
    }
    file = fopen(path, "r");
    if (file == NULL) {
        return STATUS_ERR_FILE_IO;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        Product product;
        char *first = line;

        if (strchr(line, '\n') == NULL && !feof(file)) {
            status = STATUS_ERR_FILE_IO;
            break;
        }
        trim_line_end(line);
        while (isspace((unsigned char)*first)) {
            ++first;
        }
        if (*first == '\0' || *first == '#') {
            continue;
        }
        status = parse_product_line(first, &product);
        if (status == STATUS_SUCCESS) {
            status = inventory_add_product(inventory, &product);
        }
        if (status != STATUS_SUCCESS) {
            break;
        }
    }
    if (ferror(file)) {
        status = STATUS_ERR_FILE_IO;
    }
    fclose(file);
    return status;
}

int save_products_to_file(const Inventory *inventory, const char *path) {
    FILE *file;
    size_t i;

    if (inventory == NULL || path == NULL) {
        return STATUS_ERR_NULL_PTR;
    }
    file = fopen(path, "w");
    if (file == NULL) {
        return STATUS_ERR_FILE_IO;
    }
    if (fprintf(file, "# ID|Name|Category|Unit|Quantity|Price\n") < 0) {
        fclose(file);
        return STATUS_ERR_FILE_IO;
    }
    for (i = 0; i < inventory->count; ++i) {
        const Product *product = &inventory->items[i];
        if (fprintf(file, "%s|%s|%s|%s|%d|%.2f\n",
                    product->id, product->name, product->category, product->unit,
                    product->quantity, product->price) < 0) {
            fclose(file);
            return STATUS_ERR_FILE_IO;
        }
    }
    if (fclose(file) != 0) {
        return STATUS_ERR_FILE_IO;
    }
    return STATUS_SUCCESS;
}
