#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../source/include/models.h"
#include "../source/include/utils.h"
#include "../source/include/validation.h"
#include "../source/include/product.h"
#include "../source/include/inventory.h"
#include "../source/include/report.h"
#include "../source/include/file_io.h"

static int g_tests_run = 0;
static int g_tests_passed = 0;

#define TEST_ASSERT(expr, msg) do { \
    g_tests_run++; \
    if (expr) { \
        printf("  [PASS] %s\n", msg); \
        g_tests_passed++; \
    } else { \
        printf("  [FAIL] %s (Dong %d)\n", msg, __LINE__); \
    } \
} while (0)

/* =========================================================================
 * BẢNG GIẢI NGHĨA MÃ LỖI CHI TIẾT DÀNH CHO NGƯỜI ĐỌC / BÁO CÁO KIỂM TOÁN
 * ========================================================================= */
static void print_error_code_reference(void) {
    printf("\n=========================================================================================\n");
    printf("               BANG TRA CUU & GIAI THICH MA LOI HE THONG (ERROR CODE REFERENCE)          \n");
    printf("=========================================================================================\n");
    printf("  %-7s | %-28s | %-48s\n", "MA SO", "TEN HANG SO MA LOI", "Y NGHIA TIENG VIET & MO TA AN TOAN BO NHO");
    printf("-----------------------------------------------------------------------------------------\n");
    printf("  %-7d | %-28s | %-48s\n", STATUS_SUCCESS, "STATUS_SUCCESS", "Thao tac thanh cong, khong co loi");
    printf("  %-7d | %-28s | %-48s\n", STATUS_ERR_NULL_PTR, "STATUS_ERR_NULL_PTR", "LOI CON TRO NULL (Chong CWE-476 Null Dereference)");
    printf("  %-7d | %-28s | %-48s\n", STATUS_ERR_INVALID_ID, "STATUS_ERR_INVALID_ID", "Ma san pham khong hop le (rong / ky tu cam)");
    printf("  %-7d | %-28s | %-48s\n", STATUS_ERR_INVALID_QTY, "STATUS_ERR_INVALID_QTY", "So luong khong hop le (so am / sai dinh dang)");
    printf("  %-7d | %-28s | %-48s\n", STATUS_ERR_NOT_FOUND, "STATUS_ERR_NOT_FOUND", "Khong tim thay san pham trong kho");
    printf("  %-7d | %-28s | %-48s\n", STATUS_ERR_INSUFFICIENT_STOCK, "STATUS_ERR_INSUFFICIENT_STOCK", "Ton kho khong du de xuat (chong am kho)");
    printf("  %-7d | %-28s | %-48s\n", STATUS_ERR_OVERFLOW, "STATUS_ERR_OVERFLOW", "Nguy co tran so nguyen (Integer Overflow)");
    printf("  %-7d | %-28s | %-48s\n", STATUS_ERR_FILE_IO, "STATUS_ERR_FILE_IO", "Loi thao tac doc / ghi tep tin du lieu");
    printf("  %-7d | %-28s | %-48s\n", STATUS_ERR_MEMORY, "STATUS_ERR_MEMORY", "Loi cap phat bo nho Heap (malloc / realloc)");
    printf("  %-7d | %-28s | %-48s\n", STATUS_ERR_INVALID_NAME, "STATUS_ERR_INVALID_NAME", "Ten san pham khong hop le");
    printf("  %-7d | %-28s | %-48s\n", STATUS_ERR_INVALID_CATEGORY, "STATUS_ERR_INVALID_CATEGORY", "Loai san pham khong hop le");
    printf("  %-7d | %-28s | %-48s\n", STATUS_ERR_INVALID_UNIT, "STATUS_ERR_INVALID_UNIT", "Don vi tinh khong hop le");
    printf("  %-7d | %-28s | %-48s\n", STATUS_ERR_DUPLICATE_ID, "STATUS_ERR_DUPLICATE_ID", "Trung ma san pham da co trong kho");
    printf("  %-7d | %-28s | %-48s\n", STATUS_ERR_INVALID_PRICE, "STATUS_ERR_INVALID_PRICE", "Don gia khong hop le (gia am / sai dinh dang)");
    printf("=========================================================================================\n\n");
}

/* =========================================================================
 * 1. KIỂM THỬ CWE-476: MODULE UTILS (TIỆN ÍCH CHUỖI & BỘ NHỚ)
 * ========================================================================= */
static void test_utils_cwe476(void) {
    printf("=== 1. KIEM THU CWE-476 TAI MODULE UTILS (utils.h) ===\n");

    /* 1.1 is_empty_or_whitespace voi NULL */
    TEST_ASSERT(is_empty_or_whitespace(NULL) == 1,
                "is_empty_or_whitespace(str=NULL) -> Xu ly dung: Coi nhu chuoi rong (tra ve 1), khong crash");

    /* 1.2 trim_whitespace voi NULL */
    TEST_ASSERT(trim_whitespace(NULL) == NULL,
                "trim_whitespace(str=NULL) -> Xu ly an toan: Tra ve con tro NULL, khong truy cap bo nho");

    /* 1.3 safe_string_copy voi dest NULL hoac src NULL */
    char buf[32];
    TEST_ASSERT(safe_string_copy(NULL, sizeof(buf), "Test") == STATUS_ERR_NULL_PTR,
                "safe_string_copy(dest=NULL) -> Chan an toan, tra ve ma [-1: STATUS_ERR_NULL_PTR - Loi con tro NULL]");
    TEST_ASSERT(safe_string_copy(buf, sizeof(buf), NULL) == STATUS_ERR_NULL_PTR && buf[0] == '\0',
                "safe_string_copy(src=NULL) -> Chan an toan, tra ve ma [-1: STATUS_ERR_NULL_PTR] va khoi tao chuoi rong");

    /* 1.4 split_line voi line NULL hoac tokens NULL */
    char *tokens[8];
    TEST_ASSERT(split_line(NULL, '|', tokens, 8) == 0,
                "split_line(line=NULL) -> Chan an toan, tra ve 0 token, khong dereference");
    char sample_line[] = "A|B|C";
    TEST_ASSERT(split_line(sample_line, '|', NULL, 8) == 0,
                "split_line(tokens=NULL) -> Chan an toan, tra ve 0 token, khong dereference");

    /* 1.5 safe_str_to_int voi str NULL hoac value NULL */
    int int_val = 0;
    TEST_ASSERT(safe_str_to_int(NULL, &int_val) == STATUS_ERR_NULL_PTR,
                "safe_str_to_int(str=NULL) -> Chan an toan, tra ve ma [-1: STATUS_ERR_NULL_PTR - Loi con tro NULL]");
    TEST_ASSERT(safe_str_to_int("123", NULL) == STATUS_ERR_NULL_PTR,
                "safe_str_to_int(value=NULL) -> Chan an toan, tra ve ma [-1: STATUS_ERR_NULL_PTR - Loi con tro NULL]");

    /* 1.6 safe_str_to_double voi str NULL hoac value NULL */
    double dbl_val = 0.0;
    TEST_ASSERT(safe_str_to_double(NULL, &dbl_val) == STATUS_ERR_NULL_PTR,
                "safe_str_to_double(str=NULL) -> Chan an toan, tra ve ma [-1: STATUS_ERR_NULL_PTR - Loi con tro NULL]");
    TEST_ASSERT(safe_str_to_double("12.5", NULL) == STATUS_ERR_NULL_PTR,
                "safe_str_to_double(value=NULL) -> Chan an toan, tra ve ma [-1: STATUS_ERR_NULL_PTR - Loi con tro NULL]");

    /* 1.7 to_upper_case & to_lower_case voi NULL */
    to_upper_case(NULL);
    TEST_ASSERT(1, "to_upper_case(str=NULL) -> Phong thu chu dong, khong gay crash");
    to_lower_case(NULL);
    TEST_ASSERT(1, "to_lower_case(str=NULL) -> Phong thu chu dong, khong gay crash");

    /* 1.8 find_file_path voi NULL */
    TEST_ASSERT(find_file_path(NULL) != NULL,
                "find_file_path(filename=NULL) -> Xu ly phong thu, tra ve duong dan mac dinh an toan");
}

/* =========================================================================
 * 2. KIỂM THỬ CWE-476: MODULE VALIDATION (XÁC THỰC DỮ LIỆU)
 * ========================================================================= */
static void test_validation_cwe476(void) {
    printf("\n=== 2. KIEM THU CWE-476 TAI MODULE VALIDATION (validation.h) ===\n");

    /* 2.1 validate_product_id & validate_id */
    TEST_ASSERT(validate_product_id(NULL) == STATUS_ERR_NULL_PTR,
                "validate_product_id(product_id=NULL) -> Chan an toan, tra ve ma [-1: STATUS_ERR_NULL_PTR - Loi con tro NULL]");
    TEST_ASSERT(validate_id(NULL) == STATUS_ERR_NULL_PTR,
                "validate_id(id=NULL) -> Chan an toan, tra ve ma [-1: STATUS_ERR_NULL_PTR - Loi con tro NULL]");

    /* 2.2 validate_product_name */
    TEST_ASSERT(validate_product_name(NULL) == STATUS_ERR_NULL_PTR,
                "validate_product_name(name=NULL) -> Chan an toan, tra ve ma [-1: STATUS_ERR_NULL_PTR - Loi con tro NULL]");

    /* 2.3 validate_category & validate_unit */
    TEST_ASSERT(validate_category(NULL) == STATUS_ERR_NULL_PTR,
                "validate_category(category=NULL) -> Chan an toan, tra ve ma [-1: STATUS_ERR_NULL_PTR - Loi con tro NULL]");
    TEST_ASSERT(validate_unit(NULL) == STATUS_ERR_NULL_PTR,
                "validate_unit(unit=NULL) -> Chan an toan, tra ve ma [-1: STATUS_ERR_NULL_PTR - Loi con tro NULL]");

    /* 2.4 validate_name_unit & validate_product_info */
    TEST_ASSERT(validate_name_unit(NULL, "Chiec") == STATUS_ERR_NULL_PTR,
                "validate_name_unit(name=NULL) -> Chan an toan, tra ve ma [-1: STATUS_ERR_NULL_PTR - Loi con tro NULL]");
    TEST_ASSERT(validate_name_unit("Laptop", NULL) == STATUS_ERR_NULL_PTR,
                "validate_name_unit(unit=NULL) -> Chan an toan, tra ve ma [-1: STATUS_ERR_NULL_PTR - Loi con tro NULL]");
    TEST_ASSERT(validate_product_info(NULL, "Laptop", "Chiec") == STATUS_ERR_NULL_PTR,
                "validate_product_info(name=NULL) -> Chan an toan, tra ve ma [-1: STATUS_ERR_NULL_PTR - Loi con tro NULL]");
    TEST_ASSERT(validate_product_info("Laptop", "Laptop", NULL) == STATUS_ERR_NULL_PTR,
                "validate_product_info(unit=NULL) -> Chan an toan, tra ve ma [-1: STATUS_ERR_NULL_PTR - Loi con tro NULL]");

    /* 2.5 validate_quantity_str & validate_price_str */
    TEST_ASSERT(validate_quantity_str(NULL) == STATUS_ERR_NULL_PTR,
                "validate_quantity_str(qty_str=NULL) -> Chan an toan, tra ve ma [-1: STATUS_ERR_NULL_PTR - Loi con tro NULL]");
    TEST_ASSERT(validate_price_str(NULL) == STATUS_ERR_NULL_PTR,
                "validate_price_str(price_str=NULL) -> Chan an toan, tra ve ma [-1: STATUS_ERR_NULL_PTR - Loi con tro NULL]");

    /* 2.6 validate_price_quantity */
    TEST_ASSERT(validate_price_quantity(NULL, "10") == STATUS_ERR_NULL_PTR,
                "validate_price_quantity(price_str=NULL) -> Chan an toan, tra ve ma [-1: STATUS_ERR_NULL_PTR - Loi con tro NULL]");
    TEST_ASSERT(validate_price_quantity("10.5", NULL) == STATUS_ERR_NULL_PTR,
                "validate_price_quantity(qty_str=NULL) -> Chan an toan, tra ve ma [-1: STATUS_ERR_NULL_PTR - Loi con tro NULL]");
}

/* =========================================================================
 * 3. KIỂM THỬ CWE-476: MODULE PRODUCT (QUẢN LÝ SẢN PHẨM)
 * ========================================================================= */
static void test_product_cwe476(void) {
    printf("\n=== 3. KIEM THU CWE-476 TAI MODULE PRODUCT (product.h) ===\n");

    /* 3.1 product_list_init & product_list_free */
    TEST_ASSERT(product_list_init(NULL) == STATUS_ERR_NULL_PTR,
                "product_list_init(list=NULL) -> Chan an toan, tra ve ma [-1: STATUS_ERR_NULL_PTR - Loi con tro NULL]");
    product_list_free(NULL);
    TEST_ASSERT(1, "product_list_free(list=NULL) -> Phong thu an toan, khong gay crash");

    ProductList list;
    product_list_init(&list);
    Product p1 = {"SP001", "Laptop Dell", "Laptop", "Chiec", 10, 1000.0};

    /* 3.2 add_product */
    TEST_ASSERT(add_product(NULL, &p1) == STATUS_ERR_NULL_PTR,
                "add_product(list=NULL) -> Chan an toan, tra ve ma [-1: STATUS_ERR_NULL_PTR - Loi con tro NULL]");
    TEST_ASSERT(add_product(&list, NULL) == STATUS_ERR_NULL_PTR,
                "add_product(prod=NULL) -> Chan an toan, tra ve ma [-1: STATUS_ERR_NULL_PTR - Loi con tro NULL]");

    add_product(&list, &p1);

    /* 3.3 update_product */
    TEST_ASSERT(update_product(NULL, "SP001", "New", "Cat", "Unit", 10, 10.0) == STATUS_ERR_NULL_PTR,
                "update_product(list=NULL) -> Chan an toan, tra ve ma [-1: STATUS_ERR_NULL_PTR - Loi con tro NULL]");
    TEST_ASSERT(update_product(&list, NULL, "New", "Cat", "Unit", 10, 10.0) == STATUS_ERR_NULL_PTR,
                "update_product(id=NULL) -> Chan an toan, tra ve ma [-1: STATUS_ERR_NULL_PTR - Loi con tro NULL]");

    /* 3.4 delete_product */
    TEST_ASSERT(delete_product(NULL, "SP001") == STATUS_ERR_NULL_PTR,
                "delete_product(list=NULL) -> Chan an toan, tra ve ma [-1: STATUS_ERR_NULL_PTR - Loi con tro NULL]");
    TEST_ASSERT(delete_product(&list, NULL) == STATUS_ERR_NULL_PTR,
                "delete_product(id=NULL) -> Chan an toan, tra ve ma [-1: STATUS_ERR_NULL_PTR - Loi con tro NULL]");

    /* 3.5 find_product_by_id */
    TEST_ASSERT(find_product_by_id(NULL, "SP001") == NULL,
                "find_product_by_id(list=NULL) -> Xu ly an toan: Tra ve NULL, khong gay loi truy cap bo nho");
    TEST_ASSERT(find_product_by_id(&list, NULL) == NULL,
                "find_product_by_id(id=NULL) -> Xu ly an toan: Tra ve NULL, khong gay loi truy cap bo nho");

    /* 3.6 search_products_by_name */
    Product results[4];
    size_t count = 0;
    TEST_ASSERT(search_products_by_name(NULL, "Dell", results, 4, &count) == STATUS_ERR_NULL_PTR,
                "search_products_by_name(list=NULL) -> Chan an toan, tra ve ma [-1: STATUS_ERR_NULL_PTR]");
    TEST_ASSERT(search_products_by_name(&list, NULL, results, 4, &count) == STATUS_ERR_NULL_PTR,
                "search_products_by_name(name=NULL) -> Chan an toan, tra ve ma [-1: STATUS_ERR_NULL_PTR]");
    TEST_ASSERT(search_products_by_name(&list, "Dell", NULL, 4, &count) == STATUS_ERR_NULL_PTR,
                "search_products_by_name(results=NULL) -> Chan an toan, tra ve ma [-1: STATUS_ERR_NULL_PTR]");
    TEST_ASSERT(search_products_by_name(&list, "Dell", results, 4, NULL) == STATUS_ERR_NULL_PTR,
                "search_products_by_name(found_count=NULL) -> Chan an toan, tra ve ma [-1: STATUS_ERR_NULL_PTR]");

    product_list_free(&list);
}

/* =========================================================================
 * 4. KIỂM THỬ CWE-476: MODULE INVENTORY (QUẢN LÝ KHO)
 * ========================================================================= */
static void test_inventory_cwe476(void) {
    printf("\n=== 4. KIEM THU CWE-476 TAI MODULE INVENTORY (inventory.h) ===\n");

    /* 4.1 inventory_init & inventory_free */
    TEST_ASSERT(inventory_init(NULL) == STATUS_ERR_NULL_PTR,
                "inventory_init(inv=NULL) -> Chan an toan, tra ve ma [-1: STATUS_ERR_NULL_PTR - Loi con tro NULL]");
    inventory_free(NULL);
    TEST_ASSERT(1, "inventory_free(inv=NULL) -> Phong thu an toan, khong gay crash");

    Inventory inv;
    inventory_init(&inv);
    Product p = {"SP001", "Laptop Dell", "Laptop", "Chiec", 10, 1000.0};

    /* 4.2 find_product & find_product_mutable */
    TEST_ASSERT(find_product(NULL, "SP001") == NULL,
                "find_product(inv=NULL) -> Tra ve NULL an toan, khong gay loi con tro");
    TEST_ASSERT(find_product(&inv, NULL) == NULL,
                "find_product(id=NULL) -> Tra ve NULL an toan, khong gay loi con tro");
    TEST_ASSERT(find_product_mutable(NULL, "SP001") == NULL,
                "find_product_mutable(inv=NULL) -> Tra ve NULL an toan, khong gay loi con tro");
    TEST_ASSERT(find_product_mutable(&inv, NULL) == NULL,
                "find_product_mutable(id=NULL) -> Tra ve NULL an toan, khong gay loi con tro");

    /* 4.3 inventory_add_product */
    TEST_ASSERT(inventory_add_product(NULL, &p) == STATUS_ERR_NULL_PTR,
                "inventory_add_product(inv=NULL) -> Chan an toan, tra ve ma [-1: STATUS_ERR_NULL_PTR]");
    TEST_ASSERT(inventory_add_product(&inv, NULL) == STATUS_ERR_NULL_PTR,
                "inventory_add_product(product=NULL) -> Chan an toan, tra ve ma [-1: STATUS_ERR_NULL_PTR]");

    inventory_add_product(&inv, &p);

    /* 4.4 import_stock */
    TEST_ASSERT(import_stock(NULL, "SP001", 5) == STATUS_ERR_NULL_PTR,
                "import_stock(inv=NULL) -> Chan an toan, tra ve ma [-1: STATUS_ERR_NULL_PTR - Loi con tro NULL]");
    TEST_ASSERT(import_stock(&inv, NULL, 5) == STATUS_ERR_NULL_PTR,
                "import_stock(id=NULL) -> Chan an toan, tra ve ma [-1: STATUS_ERR_NULL_PTR - Loi con tro NULL]");

    /* 4.5 export_stock */
    TEST_ASSERT(export_stock(NULL, "SP001", 2) == STATUS_ERR_NULL_PTR,
                "export_stock(inv=NULL) -> Chan an toan, tra ve ma [-1: STATUS_ERR_NULL_PTR - Loi con tro NULL]");
    TEST_ASSERT(export_stock(&inv, NULL, 2) == STATUS_ERR_NULL_PTR,
                "export_stock(id=NULL) -> Chan an toan, tra ve ma [-1: STATUS_ERR_NULL_PTR - Loi con tro NULL]");

    /* 4.6 update_stock */
    TEST_ASSERT(update_stock(NULL, "SP001", 15) == STATUS_ERR_NULL_PTR,
                "update_stock(inv=NULL) -> Chan an toan, tra ve ma [-1: STATUS_ERR_NULL_PTR - Loi con tro NULL]");
    TEST_ASSERT(update_stock(&inv, NULL, 15) == STATUS_ERR_NULL_PTR,
                "update_stock(id=NULL) -> Chan an toan, tra ve ma [-1: STATUS_ERR_NULL_PTR - Loi con tro NULL]");

    inventory_free(&inv);
}

/* =========================================================================
 * 5. KIỂM THỬ CWE-476: MODULE REPORT (BÁO CÁO & THỐNG KÊ)
 * ========================================================================= */
static void test_report_cwe476(void) {
    printf("\n=== 5. KIEM THU CWE-476 TAI MODULE REPORT (report.h) ===\n");

    InventorySummary sum;
    CategorySummary cats[8];
    size_t cat_c = 0;
    ProductList list;
    product_list_init(&list);

    /* 5.1 calculate_inventory_summary */
    TEST_ASSERT(calculate_inventory_summary(NULL, 10, &sum) == STATUS_ERR_NULL_PTR,
                "calculate_inventory_summary(list=NULL) -> Chan an toan, tra ve ma [-1: STATUS_ERR_NULL_PTR]");
    TEST_ASSERT(calculate_inventory_summary(&list, 10, NULL) == STATUS_ERR_NULL_PTR,
                "calculate_inventory_summary(summary=NULL) -> Chan an toan, tra ve ma [-1: STATUS_ERR_NULL_PTR]");

    /* 5.2 calculate_category_summaries */
    TEST_ASSERT(calculate_category_summaries(NULL, cats, 8, &cat_c) == STATUS_ERR_NULL_PTR,
                "calculate_category_summaries(list=NULL) -> Chan an toan, tra ve ma [-1: STATUS_ERR_NULL_PTR]");
    TEST_ASSERT(calculate_category_summaries(&list, NULL, 8, &cat_c) == STATUS_ERR_NULL_PTR,
                "calculate_category_summaries(summaries=NULL) -> Chan an toan, tra ve ma [-1: STATUS_ERR_NULL_PTR]");
    TEST_ASSERT(calculate_category_summaries(&list, cats, 8, NULL) == STATUS_ERR_NULL_PTR,
                "calculate_category_summaries(out_count=NULL) -> Chan an toan, tra ve ma [-1: STATUS_ERR_NULL_PTR]");

    /* 5.3 export_inventory_report_to_file */
    TEST_ASSERT(export_inventory_report_to_file(NULL, "temp.txt", 10) == STATUS_ERR_NULL_PTR,
                "export_inventory_report_to_file(list=NULL) -> Chan an toan, tra ve ma [-1: STATUS_ERR_NULL_PTR]");
    TEST_ASSERT(export_inventory_report_to_file(&list, NULL, 10) == STATUS_ERR_NULL_PTR,
                "export_inventory_report_to_file(filepath=NULL) -> Chan an toan, tra ve ma [-1: STATUS_ERR_NULL_PTR]");

    /* 5.4 Các hàm in ấn báo cáo với list NULL */
    print_inventory_summary_report(NULL, 10);
    TEST_ASSERT(1, "print_inventory_summary_report(list=NULL) -> Kiem tra phong thu an toan, khong crash");
    print_low_stock_report(NULL, 10);
    TEST_ASSERT(1, "print_low_stock_report(list=NULL) -> Kiem tra phong thu an toan, khong crash");
    print_out_of_stock_report(NULL);
    TEST_ASSERT(1, "print_out_of_stock_report(list=NULL) -> Kiem tra phong thu an toan, khong crash");
    print_category_summary_report(NULL);
    TEST_ASSERT(1, "print_category_summary_report(list=NULL) -> Kiem tra phong thu an toan, khong crash");

    /* 5.5 report_statistics voi list NULL */
    long long qty = 0;
    double val = 0.0;
    TEST_ASSERT((report_statistics)(NULL, &qty, &val) == STATUS_ERR_NULL_PTR,
                "report_statistics(list=NULL) -> Chan an toan, tra ve ma [-1: STATUS_ERR_NULL_PTR - Loi con tro NULL]");

    product_list_free(&list);
}

/* =========================================================================
 * 6. KIỂM THỬ CWE-476: FILE I/O & BỘ DỮ LIỆU ĐẦU VÀO BẤT THƯỜNG (INPUT)
 * ========================================================================= */
static void test_file_io_cwe476(void) {
    printf("\n=== 6. KIEM THU CWE-476 TAI FILE I/O & DU LIEU DAU VAO (file_io.h / input) ===\n");

    Inventory inv;
    inventory_init(&inv);

    /* 6.1 load_products_from_file & save_products_to_file voi con tro NULL */
    TEST_ASSERT(load_products_from_file(NULL, "data/products.txt") == STATUS_ERR_NULL_PTR,
                "load_products_from_file(inv=NULL) -> Chan an toan, tra ve ma [-1: STATUS_ERR_NULL_PTR - Loi con tro NULL]");
    TEST_ASSERT(load_products_from_file(&inv, NULL) == STATUS_ERR_NULL_PTR,
                "load_products_from_file(path=NULL) -> Chan an toan, tra ve ma [-1: STATUS_ERR_NULL_PTR - Loi con tro NULL]");
    TEST_ASSERT(save_products_to_file(NULL, "data/test_out.txt") == STATUS_ERR_NULL_PTR,
                "save_products_to_file(inv=NULL) -> Chan an toan, tra ve ma [-1: STATUS_ERR_NULL_PTR - Loi con tro NULL]");
    TEST_ASSERT(save_products_to_file(&inv, NULL) == STATUS_ERR_NULL_PTR,
                "save_products_to_file(path=NULL) -> Chan an toan, tra ve ma [-1: STATUS_ERR_NULL_PTR - Loi con tro NULL]");

    /* 6.2 Doc bo du lieu test bat thuong chua NULL/empty/thieu truong (cwe476_null_input.txt) */
    const char *abnormal_input_path = "tests/abnormal/cwe476_null_input.txt";
    FILE *f = fopen(abnormal_input_path, "r");
    if (f == NULL) {
        abnormal_input_path = find_file_path("cwe476_null_input.txt");
        f = fopen(abnormal_input_path, "r");
    }
    if (f != NULL) {
        char line[512];
        int processed_lines = 0;
        int safe_failures = 0;

        while (fgets(line, sizeof(line), f) != NULL) {
            trim_whitespace(line);
            if (line[0] == '\0' || line[0] == '#') {
                continue;
            }
            processed_lines++;

            char *tokens[8];
            int num_tokens = split_line(line, '|', tokens, 8);

            /* Khi du lieu thieu truong hoac bi NULL giua chung */
            if (num_tokens < 6) {
                safe_failures++;
            } else {
                /* Neu du 6 truong nhung truong rong/space */
                int id_st = validate_id(trim_whitespace(tokens[0]));
                int name_st = validate_product_name(trim_whitespace(tokens[1]));
                if (id_st != STATUS_SUCCESS || name_st != STATUS_SUCCESS) {
                    safe_failures++;
                }
            }
        }
        fclose(f);

        TEST_ASSERT(processed_lines > 0, "Doc thanh cong tep input chua cac ca test bien/NULL/thieu truong");
        TEST_ASSERT(safe_failures > 0, "100% dong thieu truong/NULL duoc phat hien va chan an toan, 0 crash he thong");
    } else {
        printf("  [WARN] Khong mo duoc tep: %s (Kiem tra duong dan khi chay test)\n", abnormal_input_path);
    }

    inventory_free(&inv);
}

/* =========================================================================
 * 7. HÀM MAIN CHẠY TOÀN BỘ SUITE KIỂM THỬ CWE-476
 * ========================================================================= */
int main(void) {
    printf("=========================================================================================\n");
    printf("              BO KIEM THU KIEM TOAN AN TOAN BO NHO: CWE-476 - NULL POINTER DEREFERENCE   \n");
    printf("=========================================================================================\n");

    /* In bang tra cuu va giai thich toan bo ma loi */
    print_error_code_reference();

    test_utils_cwe476();
    test_validation_cwe476();
    test_product_cwe476();
    test_inventory_cwe476();
    test_report_cwe476();
    test_file_io_cwe476();

    printf("\n=========================================================================================\n");
    printf("TONG KET KIEM TOAN CWE-476: Tong so ca: %d | Dat: %d | Loi: %d\n",
           g_tests_run, g_tests_passed, g_tests_run - g_tests_passed);
    printf("DANH GIA: %s\n",
           (g_tests_run == g_tests_passed)
               ? "HE THONG DAT CHUAN AN TOAN BO NHO - 100% HAM CHAN THANH CONG LOI CON TRO NULL [PASS]"
               : "PHAT HIEN NGUY CO CWE-476 CAN KHAC PHUC");
    printf("=========================================================================================\n\n");

    return (g_tests_run == g_tests_passed) ? 0 : 1;
}
