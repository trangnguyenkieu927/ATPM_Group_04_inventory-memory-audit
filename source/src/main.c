#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/validation.h"
#include "../include/utils.h"

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
        int num_tokens = split_line(line, '|', tokens, 8);

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
        int num_tokens = split_line(line, '|', tokens, 8);

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

static void run_utils_tests() {
    printf("--- Chay cac ca kiem thu: Cac ham tien ich dung chung (utils.h) ---\n");
    printf("1. is_empty_or_whitespace('   ')           : %s\n",
           is_empty_or_whitespace("   ") ? "[PASS]" : "[FAIL]");
    printf("2. is_empty_or_whitespace(NULL)            : %s\n",
           is_empty_or_whitespace(NULL) ? "[PASS]" : "[FAIL]");

    char str_trim[32] = "  Laptop Dell  ";
    trim_whitespace(str_trim);
    printf("3. trim_whitespace('  Laptop Dell  ')      : %s\n",
           strcmp(str_trim, "Laptop Dell") == 0 ? "[PASS]" : "[FAIL]");

    char buf[10];
    int copy_st = safe_string_copy(buf, sizeof(buf), "VeryLongStringHere");
    printf("4. safe_string_copy (chong tran bo dem)    : %s\n",
           (copy_st == STATUS_ERR_OVERFLOW && buf[sizeof(buf) - 1] == '\0') ? "[PASS]" : "[FAIL]");

    int int_val = 0;
    int int_st = safe_str_to_int("qưqw", &int_val);
    printf("5. safe_str_to_int ('qưqw' chan chu)       : %s\n",
           int_st == STATUS_ERR_INVALID_QTY ? "[PASS]" : "[FAIL]");

    double dbl_val = 0.0;
    int dbl_st = safe_str_to_double("1499.99", &dbl_val);
    printf("6. safe_str_to_double ('1499.99' hop le)   : %s\n",
           (dbl_st == STATUS_SUCCESS && dbl_val > 1499.0) ? "[PASS]" : "[FAIL]");

    char case_str[16] = "sp001";
    to_upper_case(case_str);
    printf("7. to_upper_case ('sp001' -> 'SP001')      : %s\n",
           strcmp(case_str, "SP001") == 0 ? "[PASS]" : "[FAIL]");
    printf("-------------------------------------------------------------------------------------------------------------------------\n\n");
}

int main() {
    run_utils_tests();

    const char *filepath = find_file_path("products.txt");
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
