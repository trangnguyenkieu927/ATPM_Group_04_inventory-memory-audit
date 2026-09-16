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
        case STATUS_ERR_INVALID_QTY: return "LOI SO LUONG";
        case STATUS_ERR_INVALID_PRICE: return "LOI DON GIA";
        case STATUS_ERR_OVERFLOW: return "TRAN SO NGUYEN";
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
    printf("                  KIEM TRA GIA VA SO LUONG SAN PHAM \n");
    printf("=========================================================================================================================\n");
    printf("%-8s | %-32s | %-12s | %-8s | %-8s | %-9s | %-18s\n",
           "MA SP", "TEN SAN PHAM", "LOAI SP", "DON VI", "SO LUONG", "GIA ($)", "K.TRA GIA & SL");
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

            // Kiểm tra giá và số lượng bằng validate_price_quantity
            int pq_st = validate_price_quantity(price_str, qty_str);

            char status_detail[64];
            if (pq_st == STATUS_SUCCESS) {
                snprintf(status_detail, sizeof(status_detail), "[HOP LE]");
                valid_products++;
            } else {
                snprintf(status_detail, sizeof(status_detail), "[%s]", get_status_desc(pq_st));
            }

            printf("%-8s | %-32s | %-12s | %-8s | %-8s | %-9s | %-18s\n",
                   id, name, category, unit, qty_str, price_str, status_detail);
        }
    }

    printf("-------------------------------------------------------------------------------------------------------------------------\n");
    printf("Tong so ban ghi mau: %d | Gia & SL hop le: %d | Gia & SL loi: %d\n",
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

        if (num_tokens >= 6 && strcmp(tokens[0], search_id) == 0) {
            const char *id = tokens[0];
            const char *name = tokens[1];
            const char *category = tokens[2];
            const char *unit = tokens[3];
            const char *qty_str = tokens[4];
            const char *price_str = tokens[5];

            printf("\n KET QUA TRA CUU: %s\n", id);
            printf("----------------------------------------------------------------------\n");
            printf("  - Ma san pham (ID)     : %s\n", id);
            printf("  - Ten san pham (Name)  : %s\n", name);
            printf("  - Loai san pham (Cat)  : %s\n", category);
            printf("  - Don vi tinh (Unit)   : %s\n", unit);
            printf("  - So luong ton kho     : %s\n", qty_str);
            printf("  - Don gia niem yet     : %s $\n", price_str);
            printf("----------------------------------------------------------------------\n");

            int price_st = validate_price_str(price_str);
            int qty_st = validate_quantity_str(qty_str);
            int pq_st = validate_price_quantity(price_str, qty_str);

            printf("  * Kiem tra Don gia (validate_price_str)   : %s\n",
                   price_st == STATUS_SUCCESS ? "HOP LE [OK]" : "KHONG HOP LE (AM/SAI DINH DANG) [FAIL]");
            printf("  * Kiem tra So luong (validate_quantity_str): %s\n",
                   qty_st == STATUS_SUCCESS ? "HOP LE [OK]" : "KHONG HOP LE (AM/SAI DINH DANG) [FAIL]");
            printf("  * validate_price_quantity(\"%s\", \"%s\") : %s (Ma loi: %d)\n",
                   price_str, qty_str,
                   pq_st == STATUS_SUCCESS ? "HOP LE [OK]" : "CHAN THANH CONG [CHAN GIA TRI SAI]",
                   pq_st);
            printf("----------------------------------------------------------------------\n\n");
            fclose(f);
            return 1;
        }
    }

    fclose(f);
    return 0;
}

static void run_price_quantity_tests() {
    printf("--- Chay cac ca kiem thu tu dong: Tinh nang kiem tra Gia & So luong ---\n");
    printf("1. So luong am (-5)                     : %s\n",
           validate_quantity_str("-5") == STATUS_ERR_INVALID_QTY ? "[PASS] (Chan so am thanh cong)" : "[FAIL]");
    printf("2. So luong sai dinh dang chu ('qưqw')   : %s\n",
           validate_quantity_str("qưqw") == STATUS_ERR_INVALID_QTY ? "[PASS] (Chan chu thanh cong)" : "[FAIL]");
    printf("3. So luong sai dinh dang chu ('aaa')    : %s\n",
           validate_quantity_str("aaa") == STATUS_ERR_INVALID_QTY ? "[PASS] (Chan chu thanh cong)" : "[FAIL]");
    printf("4. So luong chua chu va so ('15a')       : %s\n",
           validate_quantity_str("15a") == STATUS_ERR_INVALID_QTY ? "[PASS] (Chan ky tu la thanh cong)" : "[FAIL]");
    printf("5. So luong hop le (35)                  : %s\n",
           validate_quantity_str("35") == STATUS_SUCCESS ? "[PASS]" : "[FAIL]");
    printf("6. Gia am (-99.50)                       : %s\n",
           validate_price_str("-99.50") == STATUS_ERR_INVALID_PRICE ? "[PASS] (Chan gia am thanh cong)" : "[FAIL]");
    printf("7. Gia sai dinh dang chu ('abc')         : %s\n",
           validate_price_str("abc") == STATUS_ERR_INVALID_PRICE ? "[PASS] (Chan chu thanh cong)" : "[FAIL]");
    printf("8. Gia sai dinh dang ('12.3.4')          : %s\n",
           validate_price_str("12.3.4") == STATUS_ERR_INVALID_PRICE ? "[PASS] (Chan 2 dau cham thanh cong)" : "[FAIL]");
    printf("9. Gia hop le (1499.99)                  : %s\n",
           validate_price_str("1499.99") == STATUS_SUCCESS ? "[PASS]" : "[FAIL]");
    printf("10. validate_price_quantity('1499.99', '35'): %s\n",
           validate_price_quantity("1499.99", "35") == STATUS_SUCCESS ? "[PASS]" : "[FAIL]");
    printf("11. validate_price_quantity('1499.99', 'qưqw'): %s\n",
           validate_price_quantity("1499.99", "qưqw") == STATUS_ERR_INVALID_QTY ? "[PASS] (Chan so luong sai dinh dang)" : "[FAIL]");
    printf("12. validate_price_quantity('-25.5', '10') : %s\n",
           validate_price_quantity("-25.5", "10") == STATUS_ERR_INVALID_PRICE ? "[PASS] (Chan gia am thanh cong)" : "[FAIL]");
    printf("-------------------------------------------------------------------------------------------------------------------------\n\n");
}

int main() {
    run_price_quantity_tests();

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
