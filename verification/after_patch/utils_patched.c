#include "../include/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

int is_empty_or_whitespace(const char *str) {
    if (str == NULL) {
        return 1;
    }
    while (*str) {
        if (!isspace((unsigned char)*str)) {
            return 0;
        }
        str++;
    }
    return 1;
}

char* trim_whitespace(char *str) {
    if (str == NULL) {
        return NULL;
    }

    char *start = str;
    while (*start && isspace((unsigned char)*start)) {
        start++;
    }

    char *end = start + strlen(start);
    while (end > start && isspace((unsigned char)*(end - 1))) {
        end--;
    }
    *end = '\0';

    if (start != str) {
        memmove(str, start, (size_t)(end - start + 1));
    }

    return str;
}

int safe_string_copy(char *dest, size_t dest_size, const char *src) {
    if (dest == NULL || dest_size == 0) {
        return STATUS_ERR_NULL_PTR;
    }

    if (src == NULL) {
        dest[0] = '\0';
        return STATUS_ERR_NULL_PTR;
    }

    size_t i = 0;
    while (i + 1 < dest_size && src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';

    if (src[i] != '\0') {
        return STATUS_ERR_OVERFLOW; /* Bị cắt ngắn do buffer đích nhỏ hơn chuỗi nguồn */
    }

    return STATUS_SUCCESS;
}

void safe_read_line(const char *prompt, char *buffer, size_t size) {
    if (buffer == NULL || size == 0) {
        return;
    }

    if (prompt != NULL) {
        printf("%s", prompt);
        fflush(stdout);
    }

    if (fgets(buffer, (int)size, stdin) != NULL) {
        size_t len = strlen(buffer);
        int has_newline = 0;
        while (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r')) {
            buffer[len - 1] = '\0';
            len--;
            has_newline = 1;
        }

        /* Nếu dòng nhập vượt quá kích thước buffer, xả sạch phần còn lại */
        if (!has_newline && len == size - 1) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF) {
                /* Dọn bộ đệm stdin */
            }
        }
    } else {
        buffer[0] = '\0';
    }
}

int split_line(char *line, char delimiter, char *tokens[], int max_tokens) {
    if (line == NULL || tokens == NULL || max_tokens <= 0) {
        return 0;
    }

    int count = 0;
    char *p = line;

    while (*p && count < max_tokens) {
        tokens[count++] = p;
        char *sep = strchr(p, delimiter);
        if (sep != NULL) {
            *sep = '\0';
            p = sep + 1;
        } else {
            break;
        }
    }

    return count;
}

int safe_str_to_int(const char *str, int *out_val) {
    if (str == NULL || out_val == NULL) {
        return STATUS_ERR_NULL_PTR;
    }

    while (isspace((unsigned char)*str)) {
        str++;
    }

    if (*str == '\0') {
        return STATUS_ERR_INVALID_QTY;
    }

    char *endptr = NULL;
    long long val = strtoll(str, &endptr, 10);

    /* Không đọc được số nào */
    if (endptr == str) {
        return STATUS_ERR_INVALID_QTY;
    }

    /* Kiểm tra phần đuôi chỉ chứa khoảng trắng */
    while (*endptr) {
        if (!isspace((unsigned char)*endptr)) {
            return STATUS_ERR_INVALID_QTY;
        }
        endptr++;
    }

    if (val > INT_MAX || val < INT_MIN) {
        return STATUS_ERR_OVERFLOW;
    }

    *out_val = (int)val;
    return STATUS_SUCCESS;
}

int safe_str_to_double(const char *str, double *out_val) {
    if (str == NULL || out_val == NULL) {
        return STATUS_ERR_NULL_PTR;
    }

    while (isspace((unsigned char)*str)) {
        str++;
    }

    if (*str == '\0') {
        return STATUS_ERR_INVALID_PRICE;
    }

    char *endptr = NULL;
    double val = strtod(str, &endptr);

    if (endptr == str) {
        return STATUS_ERR_INVALID_PRICE;
    }

    while (*endptr) {
        if (!isspace((unsigned char)*endptr)) {
            return STATUS_ERR_INVALID_PRICE;
        }
        endptr++;
    }

    *out_val = val;
    return STATUS_SUCCESS;
}

void to_upper_case(char *str) {
    if (str == NULL) {
        return;
    }
    while (*str) {
        *str = (char)toupper((unsigned char)*str);
        str++;
    }
}

void to_lower_case(char *str) {
    if (str == NULL) {
        return;
    }
    while (*str) {
        *str = (char)tolower((unsigned char)*str);
        str++;
    }
}

const char* find_file_path(const char *filename) {
    static char found_path[256];
    if (filename == NULL) {
        return "data/products.txt";
    }

    const char *prefixes[] = {
        "",
        "data/",
        "source/data/",
        "../source/data/",
        NULL
    };

    for (int i = 0; prefixes[i] != NULL; ++i) {
        snprintf(found_path, sizeof(found_path), "%s%s", prefixes[i], filename);
        FILE *f = fopen(found_path, "r");
        if (f != NULL) {
            fclose(f);
            return found_path;
        }
    }

    return filename;
}
