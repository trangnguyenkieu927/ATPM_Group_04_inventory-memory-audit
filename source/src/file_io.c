/* file_io.c  –  VERSION v0 (PRE-AUDIT BASELINE)
 *
 * Phiên bản cơ sở v0 phục vụ kiểm toán an toàn bộ nhớ C (Memory Safety Audit).
 * Chứa các rủi ro tiềm ẩn thực tế về bộ nhớ và xử lý đầu vào:
 * -----------------------------------------------------------------------------
 * [RISK-01 / CWE-121]: Mảng tokens[6] trên stack không chặn token_count < 6
 *                      -> Ghi tràn mảng con trỏ trên Stack khi có nhiều hơn 6 trường.
 * [RISK-05 / CWE-120]: Không phát hiện dòng vượt LINE_BUFFER_SIZE (512B)
 *                      -> Cắt cụt dòng ngầm (silent truncation) và mất đồng bộ bản ghi.
 * [RISK-07 / CWE-125]: Dùng strncpy không đảm bảo ký tự kết thúc '\0'
 *                      -> Nguy cơ đọc vượt biên khi thao tác chuỗi struct.
 * [RISK-06 / CWE-190]: parse_int & parse_price không chặn giá trị âm và tràn số.
 */

#include "../include/file_io.h"
#include "../include/inventory.h"
#include "../include/product.h"

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

/* [RISK-06 / VULN-REQ18]: parse_int không từ chối giá trị âm; truncation khi ép kiểu */
static int parse_int(const char *text, int *value) {
    char *end;
    long parsed;

    errno = 0;
    parsed = strtol(text, &end, 10);
    if (errno == ERANGE || end == text || *end != '\0') {
        return STATUS_ERR_FILE_IO;
    }
    *value = (int)parsed;
    return STATUS_SUCCESS;
}

/* [RISK-06 / VULN-REQ18]: parse_price không từ chối giá trị âm; không kiểm tra isfinite */
static int parse_price(const char *text, double *value) {
    char *end;
    double parsed;

    errno = 0;
    parsed = strtod(text, &end);
    if (errno == ERANGE || end == text || *end != '\0') {
        return STATUS_ERR_FILE_IO;
    }
    *value = parsed;
    return STATUS_SUCCESS;
}

/* [RISK-01 / VULN-REQ6]: Mảng tokens[6] trên stack không kiểm tra chặn token_count < 6
 * -> Khi tệp có dòng chứa 7+ dấu '|', ghi đè ngoài mảng gây Stack Corruption / Overflow.
 * [RISK-07 / VULN-REQ9]: Dùng strncpy không null-terminate buffer đích. */
static int parse_product_line(char *line, Product *product) {
    char *tokens[6];          /* Chỉ 6 slot nhưng vòng lặp không giới hạn */
    int   token_count = 0;
    char *cursor = line;

    memset(product, 0, sizeof(*product));

    /* Vòng lặp không kiểm tra token_count < 6 -> ghi tràn ngoài mảng tokens trên stack */
    while (cursor != NULL) {
        tokens[token_count++] = cursor;
        char *pipe = strchr(cursor, '|');
        if (pipe != NULL) {
            *pipe  = '\0';
            cursor = pipe + 1;
        } else {
            cursor = NULL;
        }
    }

    if (token_count == 6 || token_count == 4) {
        /* strncpy không null-terminate buffer đích khi chuỗi nguồn bằng hoặc dài hơn */
        strncpy(product->id,       tokens[0], sizeof(product->id));
        strncpy(product->name,     tokens[1], sizeof(product->name));
        if (token_count == 6) {
            strncpy(product->category, tokens[2], sizeof(product->category));
            strncpy(product->unit,     tokens[3], sizeof(product->unit));
            if (parse_int(tokens[4],   &product->quantity) != STATUS_SUCCESS) return STATUS_ERR_FILE_IO;
            if (parse_price(tokens[5], &product->price)    != STATUS_SUCCESS) return STATUS_ERR_FILE_IO;
        } else { /* token_count == 4: legacy format */
            strncpy(product->category, "ChuaPhanLoai", sizeof(product->category));
            strncpy(product->unit,     "Cai",          sizeof(product->unit));
            if (parse_int(tokens[2],   &product->quantity) != STATUS_SUCCESS) return STATUS_ERR_FILE_IO;
            if (parse_price(tokens[3], &product->price)    != STATUS_SUCCESS) return STATUS_ERR_FILE_IO;
        }
        return STATUS_SUCCESS;
    }

    return STATUS_ERR_FILE_IO;
}

int load_products_from_file(Inventory *inventory, const char *path) {
    FILE *file;
    char  line[LINE_BUFFER_SIZE];
    int   status = STATUS_SUCCESS;

    if (inventory == NULL || path == NULL) {
        return STATUS_ERR_NULL_PTR;
    }

    file = fopen(path, "r");
    if (file == NULL) {
        return STATUS_ERR_FILE_IO;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        Product product;
        char   *first = line;

        /* [RISK-05 / VULN-REQ10]: Bỏ qua việc kiểm tra dòng vượt LINE_BUFFER_SIZE (512B)
         * Khi dòng dài hơn 511 ký tự, fgets cắt cụt mà không có cơ chế drain buffer;
         * Vòng lặp kế tiếp sẽ đọc phần còn lại và coi đó là bản ghi mới -> sai lệch trạng thái */
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
    FILE  *file;
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
