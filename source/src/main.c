#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/validation.h"

static const char* find_products_file() {
    const char *paths[] = {
        "data/products.txt",
        "source/data/products.txt",
        "../source/data/products.txt",
        NULL
    };
    for (int i = 0; paths[i] != NULL; ++i) {
        FILE *f = fopen(paths[i], "r");
        if (f != NULL) {
            fclose(f);
            return paths[i];
        }
    }
    return "data/products.txt";
}

static void safe_read_line(const char *prompt, char *buffer, size_t size) {
    printf("%s", prompt);
    if (fgets(buffer, (int)size, stdin) != NULL) {
        size_t len = strlen(buffer);
        while (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r')) {
            buffer[len - 1] = '\0';
            len--;
        }
    } else {
        buffer[0] = '\0';
    }
}

static int split_csv_line(char *line, char *tokens[], int max_tokens) {
    int count = 0;
    char *p = line;
    while (*p && count < max_tokens) {
        tokens[count++] = p;
        char *sep = strchr(p, '|');
        if (sep != NULL) {
            *sep = '\0';
            p = sep + 1;
        } else {
            break;
        }
    }
    return count;
}

static const char* get_status_desc(int status) {
    switch (status) {
        case STATUS_SUCCESS: return "HOP LE";
        case STATUS_ERR_INVALID_NAME: return "LOI TEN SP";
        case STATUS_ERR_INVALID_CATEGORY: return "LOI LOAI SP";
        case STATUS_ERR_INVALID_UNIT: return "LOI DON VI";
        case STATUS_ERR_INVALID_ID: return "LOI MA SP";
        default: return "KHONG HOP LE";
    }
}

static void display_and_validate_file(const char *filepath) {
    FILE *f = fopen(filepath, "r");
    if (f == NULL) {
        printf("[LOI] Khong the mo tap tin du lieu mau: %s\n\n", filepath);
        return;
    }

    printf("=========================================================================================================================\n");
    printf("                  KIEM TRA THONG TIN SAN PHAM TU TEP DU LIEU MAU\n");
    printf("=========================================================================================================================\n");
    printf("%-8s | %-32s | %-12s | %-8s | %-8s | %-9s | %-15s\n",
           "MA SP", "TEN SAN PHAM", "LOAI SP", "DON VI", "SO LUONG", "GIA ($)", "K.TRA THONG TIN");
    printf("-------------------------------------------------------------------------------------------------------------------------\n");

    char line[512];
    int total_products = 0;
    int valid_products = 0;

    while (fgets(line, sizeof(line), f) != NULL) {
        size_t len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[len - 1] = '\0';
            len--;
        }

        if (len == 0 || line[0] == '#') {
            continue;
        }

        char *tokens[8];
        int num_tokens = split_csv_line(line, tokens, 8);

        if (num_tokens >= 6) {
            total_products++;
            const char *id = tokens[0];
            const char *name = tokens[1];
            const char *category = tokens[2];
            const char *unit = tokens[3];
            const char *qty_str = tokens[4];
            const char *price_str = tokens[5];

            int name_st = validate_product_name(name);
            int cat_st = validate_category(category);
            int unit_st = validate_unit(unit);
            int name_unit_st = validate_name_unit(name, unit);
            int info_st = validate_product_info(name, category, unit);

            char status_detail[64];
            if (info_st == STATUS_SUCCESS && name_unit_st == STATUS_SUCCESS) {
                snprintf(status_detail, sizeof(status_detail), "[HOP LE]");
                valid_products++;
            } else if (name_st != STATUS_SUCCESS) {
                snprintf(status_detail, sizeof(status_detail), "[%s]", get_status_desc(name_st));
            } else if (cat_st != STATUS_SUCCESS) {
                snprintf(status_detail, sizeof(status_detail), "[%s]", get_status_desc(cat_st));
            } else if (unit_st != STATUS_SUCCESS) {
                snprintf(status_detail, sizeof(status_detail), "[%s]", get_status_desc(unit_st));
            } else {
                snprintf(status_detail, sizeof(status_detail), "[LOI DU LIEU]");
            }

            printf("%-8s | %-32s | %-12s | %-8s | %-8s | %-9s | %-15s\n",
                   id, name, category, unit, qty_str, price_str, status_detail);
        }
    }

    printf("-------------------------------------------------------------------------------------------------------------------------\n");
    printf("Tong so ban ghi mau: %d | Thong tin hop le: %d | Thong tin loi: %d\n",
           total_products, valid_products, total_products - valid_products);
    printf("=========================================================================================================================\n\n");

    fclose(f);
}

static int lookup_product_in_file(const char *filepath, const char *search_id) {
    FILE *f = fopen(filepath, "r");
    if (f == NULL) {
        return 0;
    }

    char line[512];
    while (fgets(line, sizeof(line), f) != NULL) {
        size_t len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[len - 1] = '\0';
            len--;
        }

        if (len == 0 || line[0] == '#') {
            continue;
        }

        char *tokens[8];
        int num_tokens = split_csv_line(line, tokens, 8);

        if (num_tokens >= 4 && strcmp(tokens[0], search_id) == 0) {
            const char *id = tokens[0];
            const char *name = tokens[1];
            const char *category = tokens[2];
            const char *unit = tokens[3];
            const char *qty_str = (num_tokens > 4) ? tokens[4] : "N/A";
            const char *price_str = (num_tokens > 5) ? tokens[5] : "N/A";

            printf("\n KET QUA TRA CUU DU LIEU MAU: %s\n", id);
            printf("----------------------------------------------------------------------\n");
            printf("  - Ma san pham (ID)     : %s\n", id);
            printf("  - Ten san pham (Name)  : %s\n", name);
            printf("  - Loai san pham (Cat)  : %s\n", category);
            printf("  - Don vi tinh (Unit)   : %s\n", unit);
            printf("  - So luong ton kho     : %s\n", qty_str);
            printf("  - Don gia niem yet     : %s $\n", price_str);
            printf("----------------------------------------------------------------------\n");
            printf("  * Kiem tra Ten (validate_product_name) : %s\n",
                   validate_product_name(name) == STATUS_SUCCESS ? "HOP LE [OK]" : "KHONG HOP LE [FAIL]");
            printf("  * Kiem tra Loai (validate_category)    : %s\n",
                   validate_category(category) == STATUS_SUCCESS ? "HOP LE [OK]" : "KHONG HOP LE [FAIL]");
            printf("  * Kiem tra Don vi (validate_unit)      : %s\n",
                   validate_unit(unit) == STATUS_SUCCESS ? "HOP LE [OK]" : "KHONG HOP LE [FAIL]");
            printf("  * Kiem tra Ten + Don vi (name_unit)    : %s\n",
                   validate_name_unit(name, unit) == STATUS_SUCCESS ? "HOP LE [OK]" : "KHONG HOP LE [FAIL]");
            printf("  * Kiem tra Toan bo thong tin san pham  : %s\n",
                   validate_product_info(name, category, unit) == STATUS_SUCCESS ? "HOP LE [OK]" : "KHONG HOP LE [FAIL]");
            printf("----------------------------------------------------------------------\n\n");
            fclose(f);
            return 1;
        }
    }

    fclose(f);
    return 0;
}

int main() {
    const char *filepath = find_products_file();
    display_and_validate_file(filepath);

    printf("TRA CUU VA KIEM TRA CHI TIET THONG TIN SAN PHAM\n");
    char id[256];
    safe_read_line("Nhap ma san pham: ", id, sizeof(id));

    if (strlen(id) > 0) {
        if (!lookup_product_in_file(filepath, id)) {
            printf("\n[THONG BAO] Khong tim thay ma '%s'!\n\n", id);
        }
    }

    return 0;
}
