#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "../source/include/models.h"
#include "../source/include/validation.h"

static int g_total_tests = 0;
static int g_passed_tests = 0;

#define TEST_ASSERT(expr, desc) do { \
    g_total_tests++; \
    if (expr) { \
        g_passed_tests++; \
        printf("  [PASS] %s\n", desc); \
    } else { \
        printf("  [FAIL] %s (Line %d)\n", desc, __LINE__); \
    } \
} while (0)

/* =========================================================================
 * 1. KIỂM THỬ NHÓM 1: KIỂM TRA MÃ SẢN PHẨM (ID)
 * ========================================================================= */
static void test_id_validation(void) {
    printf("\n=== 1. KIEM THU NHOM 1: MA SAN PHAM (ID) ===\n");

    /* Hợp lệ */
    TEST_ASSERT(validate_product_id("SP001") == STATUS_SUCCESS, "REQ-V2 [Valid]: Ma san pham tieu chuan 'SP001'");
    TEST_ASSERT(validate_id("A") == STATUS_SUCCESS, "REQ-V2 [Boundary Min]: Do dai = 1 ky tu");
    
    char max_valid_id[MAX_ID_LEN]; // 19 ky tu
    memset(max_valid_id, 'A', MAX_ID_LEN - 1);
    max_valid_id[MAX_ID_LEN - 1] = '\0';
    TEST_ASSERT(validate_product_id(max_valid_id) == STATUS_SUCCESS, "REQ-V2 [Boundary Max]: Do dai = 19 ky tu (MAX_ID_LEN - 1)");

    /* Biên bất thường: Vượt quá giới hạn */
    char overflow_id[MAX_ID_LEN + 1]; // 20 ky tu
    memset(overflow_id, 'A', MAX_ID_LEN);
    overflow_id[MAX_ID_LEN] = '\0';
    TEST_ASSERT(validate_product_id(overflow_id) == STATUS_ERR_INVALID_ID, "REQ-V2 [Boundary Overflow]: Do dai = 20 ky tu (>= MAX_ID_LEN) bi chan");

    /* Bất thường: Rỗng hoặc NULL */
    TEST_ASSERT(validate_product_id("") == STATUS_ERR_INVALID_ID, "REQ-V2 [Abnormal]: Chuoi rong");
    TEST_ASSERT(validate_product_id(NULL) == STATUS_ERR_NULL_PTR, "REQ-V1 [Abnormal]: Con tro NULL");

    /* Bất thường: Chứa ký tự cấm (khoảng trắng, ký tự phân cách, control char) */
    TEST_ASSERT(validate_product_id("SP 001") == STATUS_ERR_INVALID_ID, "REQ-V3 [Abnormal]: Chanh chua khoang trang");
    TEST_ASSERT(validate_product_id("SP\t01") == STATUS_ERR_INVALID_ID, "REQ-V3 [Abnormal]: Chanh chua ky tu tab/control");
    TEST_ASSERT(validate_product_id("SP|001") == STATUS_ERR_INVALID_ID, "REQ-V3 [Abnormal]: Chanh chua dau phan cach '|'");
    TEST_ASSERT(validate_product_id("SP,001") == STATUS_ERR_INVALID_ID, "REQ-V3 [Abnormal]: Chanh chua dau phan cach ','");
    TEST_ASSERT(validate_product_id("SP;001") == STATUS_ERR_INVALID_ID, "REQ-V3 [Abnormal]: Chanh chua dau phan cach ';'");
}

/* =========================================================================
 * 2. KIỂM THỬ NHÓM 2: TÊN, DANH MỤC, ĐƠN VỊ TÍNH (NAME, CATEGORY, UNIT)
 * ========================================================================= */
static void test_product_info_validation(void) {
    printf("\n=== 2. KIEM THU NHOM 2: THONG TIN SAN PHAM (NAME, CATEGORY, UNIT) ===\n");

    /* Name Validation */
    TEST_ASSERT(validate_product_name("Laptop Dell XPS 15") == STATUS_SUCCESS, "REQ-V4 [Valid]: Ten hop le");
    TEST_ASSERT(validate_product_name(NULL) == STATUS_ERR_NULL_PTR, "REQ-V4 [Abnormal]: Name NULL");
    TEST_ASSERT(validate_product_name("") == STATUS_ERR_INVALID_NAME, "REQ-V4 [Abnormal]: Name chuoi rong");
    TEST_ASSERT(validate_product_name("     ") == STATUS_ERR_INVALID_NAME, "REQ-V4 [Abnormal]: Name toan khoang trang");
    TEST_ASSERT(validate_product_name("Laptop|Dell") == STATUS_ERR_INVALID_NAME, "REQ-V4 [Abnormal]: Name chua dau phan cach '|'");
    TEST_ASSERT(validate_product_name("Laptop\nDell") == STATUS_ERR_INVALID_NAME, "REQ-V4 [Abnormal]: Name chua ky tu xuong dong");

    char max_valid_name[MAX_NAME_LEN];
    memset(max_valid_name, 'N', MAX_NAME_LEN - 1);
    max_valid_name[MAX_NAME_LEN - 1] = '\0';
    TEST_ASSERT(validate_product_name(max_valid_name) == STATUS_SUCCESS, "REQ-V4 [Boundary Max]: Name 99 ky tu hop le");

    char overflow_name[MAX_NAME_LEN + 1];
    memset(overflow_name, 'N', MAX_NAME_LEN);
    overflow_name[MAX_NAME_LEN] = '\0';
    TEST_ASSERT(validate_product_name(overflow_name) == STATUS_ERR_INVALID_NAME, "REQ-V4 [Boundary Overflow]: Name 100 ky tu bi chan");

    /* Category Validation */
    TEST_ASSERT(validate_category("Dien tu") == STATUS_SUCCESS, "REQ-V5 [Valid]: Category hop le");
    TEST_ASSERT(validate_category(NULL) == STATUS_ERR_NULL_PTR, "REQ-V5 [Abnormal]: Category NULL");
    TEST_ASSERT(validate_category("   ") == STATUS_ERR_INVALID_CATEGORY, "REQ-V5 [Abnormal]: Category toan khoang trang");
    TEST_ASSERT(validate_category("Dien|tu") == STATUS_ERR_INVALID_CATEGORY, "REQ-V5 [Abnormal]: Category chua '|'");

    /* Unit Validation */
    TEST_ASSERT(validate_unit("Chiec") == STATUS_SUCCESS, "REQ-V6 [Valid]: Unit hop le");
    TEST_ASSERT(validate_unit(NULL) == STATUS_ERR_NULL_PTR, "REQ-V6 [Abnormal]: Unit NULL");
    TEST_ASSERT(validate_unit("   ") == STATUS_ERR_INVALID_UNIT, "REQ-V6 [Abnormal]: Unit toan khoang trang");
    TEST_ASSERT(validate_unit("Chiec|Cai") == STATUS_ERR_INVALID_UNIT, "REQ-V6 [Abnormal]: Unit chua '|'");

    /* Tong hop validate_product_info */
    TEST_ASSERT(validate_product_info("Chuot Logitech", "Phu kien", "Cai") == STATUS_SUCCESS, "REQ-V7 [Valid]: Bo thong tin san pham hop le");
    TEST_ASSERT(validate_product_info("", "Phu kien", "Cai") == STATUS_ERR_INVALID_NAME, "REQ-V7 [Abnormal]: Name loi trong bo thong tin");
    TEST_ASSERT(validate_product_info("Chuot", "Phu|kien", "Cai") == STATUS_ERR_INVALID_CATEGORY, "REQ-V7 [Abnormal]: Category loi trong bo thong tin");
    TEST_ASSERT(validate_product_info("Chuot", "Phu kien", "") == STATUS_ERR_INVALID_UNIT, "REQ-V7 [Abnormal]: Unit loi trong bo thong tin");
}

/* =========================================================================
 * 3. KIỂM THỬ NHÓM 3: ĐƠN GIÁ VÀ SỐ LƯỢNG (PRICE & QUANTITY)
 * ========================================================================= */
static void test_price_quantity_validation(void) {
    printf("\n=== 3. KIEM THU NHOM 3: GIA VA SO LUONG (PRICE & QUANTITY) ===\n");

    /* Giá số thực */
    TEST_ASSERT(validate_price(1500.50) == STATUS_SUCCESS, "REQ-V8 [Valid]: Don gia 1500.50 hop le");
    TEST_ASSERT(validate_price(0.0) == STATUS_SUCCESS, "REQ-V8 [Boundary Min]: Don gia 0.0 hop le");
    TEST_ASSERT(validate_price(-0.01) == STATUS_ERR_INVALID_PRICE, "REQ-V8 [Abnormal Negative]: Don gia am bi chan");

    /* Giá dạng chuỗi */
    TEST_ASSERT(validate_price_str("1250.75") == STATUS_SUCCESS, "REQ-V9 [Valid]: Chuoi gia hop le");
    TEST_ASSERT(validate_price_str("  99.0  ") == STATUS_SUCCESS, "REQ-V9 [Valid]: Chuoi gia co khoang trang dau/cuoi duoc parse dung");
    TEST_ASSERT(validate_price_str("0") == STATUS_SUCCESS, "REQ-V9 [Boundary Min]: Chuoi '0'");
    TEST_ASSERT(validate_price_str("-10.5") == STATUS_ERR_INVALID_PRICE, "REQ-V9 [Abnormal]: Chuoi gia am bi chan");
    TEST_ASSERT(validate_price_str("abc") == STATUS_ERR_INVALID_PRICE, "REQ-V9 [Abnormal]: Chuoi gia chua chu cai bi chan");
    TEST_ASSERT(validate_price_str("12.34.56") == STATUS_ERR_INVALID_PRICE, "REQ-V9 [Abnormal]: Chuoi gia nhieu dau cham thap phan bi chan");
    TEST_ASSERT(validate_price_str(NULL) == STATUS_ERR_NULL_PTR, "REQ-V9 [Abnormal]: Chuoi gia NULL bi chan");

    /* Số lượng số nguyên */
    TEST_ASSERT(validate_quantity(100) == STATUS_SUCCESS, "REQ-V10 [Valid]: So luong 100 hop le");
    TEST_ASSERT(validate_quantity(0) == STATUS_SUCCESS, "REQ-V10 [Boundary Min]: So luong 0 hop le");
    TEST_ASSERT(validate_quantity(-1) == STATUS_ERR_INVALID_QTY, "REQ-V10 [Abnormal Negative]: So luong am bi chan");

    /* Số lượng dạng chuỗi & phòng thủ tràn số nguyên */
    TEST_ASSERT(validate_quantity_str("500") == STATUS_SUCCESS, "REQ-V11 [Valid]: Chuoi so luong hop le");
    TEST_ASSERT(validate_quantity_str("0") == STATUS_SUCCESS, "REQ-V11 [Boundary Min]: Chuoi so luong '0'");
    TEST_ASSERT(validate_quantity_str("2147483647") == STATUS_SUCCESS, "REQ-V11 [Boundary Max]: Chuoi so luong INT_MAX hop le");
    TEST_ASSERT(validate_quantity_str("2147483648") == STATUS_ERR_OVERFLOW, "REQ-V11 [Safety Overflow]: Vuot INT_MAX phat hien tran so nguyen");
    TEST_ASSERT(validate_quantity_str("-5") == STATUS_ERR_INVALID_QTY, "REQ-V11 [Abnormal]: Chuoi so luong am bi chan");
    TEST_ASSERT(validate_quantity_str("12.5") == STATUS_ERR_INVALID_QTY, "REQ-V11 [Abnormal]: So thap phan truyen vao so luong nguyen bi chan");
    TEST_ASSERT(validate_quantity_str("abc") == STATUS_ERR_INVALID_QTY, "REQ-V11 [Abnormal]: Chuoi so luong ky tu chu bi chan");
    TEST_ASSERT(validate_quantity_str(NULL) == STATUS_ERR_NULL_PTR, "REQ-V11 [Abnormal]: Chuoi so luong NULL bi chan");
}

/* =========================================================================
 * 4. KIỂM THỬ NHÓM 4: LOGIC KHO & AN TOÀN TRÀN SỐ NGUYÊN (KHO & OVERFLOW)
 * ========================================================================= */
static void test_inventory_logic_validation(void) {
    printf("\n=== 4. KIEM THU NHOM 4: LOGIC KHO & AN TOAN TRAN SO NGUYEN ===\n");

    /* Xuất kho (validate_export_quantity) */
    TEST_ASSERT(validate_export_quantity(100, 50) == STATUS_SUCCESS, "REQ-V12 [Valid]: Xuat kho binh thuong (ton 100, xuat 50)");
    TEST_ASSERT(validate_export_quantity(50, 50) == STATUS_SUCCESS, "REQ-V12 [Boundary Max]: Xuat kho bang dung ton kho (ton 50, xuat 50)");
    TEST_ASSERT(validate_export_quantity(50, 0) == STATUS_ERR_INVALID_QTY, "REQ-V12 [Abnormal]: Xuat kho bang 0 bi chan");
    TEST_ASSERT(validate_export_quantity(50, -5) == STATUS_ERR_INVALID_QTY, "REQ-V12 [Abnormal]: Xuat kho am bi chan");
    TEST_ASSERT(validate_export_quantity(50, 51) == STATUS_ERR_INSUFFICIENT_STOCK, "REQ-V12 [Abnormal]: Xuat kho vuot ton kho (ton 50, xuat 51) bao loi khong du hang");
    TEST_ASSERT(validate_export_quantity(-10, 5) == STATUS_ERR_INSUFFICIENT_STOCK, "REQ-V12 [Abnormal]: Ton kho am bi chan");

    /* Nhập kho & Phòng thủ tràn số (check_addition_overflow) */
    TEST_ASSERT(check_addition_overflow(100, 200) == STATUS_SUCCESS, "REQ-V13 [Valid]: Cong binh thuong (100 + 200)");
    TEST_ASSERT(check_addition_overflow(INT_MAX - 100, 100) == STATUS_SUCCESS, "REQ-V13 [Boundary Max]: Cong cham dung nguong INT_MAX");
    TEST_ASSERT(check_addition_overflow(INT_MAX - 100, 101) == STATUS_ERR_OVERFLOW, "REQ-V13 [Safety Overflow]: Cong vuot nguong INT_MAX bi chan triet de");
    TEST_ASSERT(check_addition_overflow(100, 0) == STATUS_ERR_INVALID_QTY, "REQ-V13 [Abnormal]: So luong nhap = 0 bi chan");
    TEST_ASSERT(check_addition_overflow(100, -10) == STATUS_ERR_INVALID_QTY, "REQ-V13 [Abnormal]: So luong nhap am bi chan");
    TEST_ASSERT(check_addition_overflow(-5, 10) == STATUS_ERR_INSUFFICIENT_STOCK, "REQ-V13 [Abnormal]: Ton kho hien tai am bi chan");
}

int main(void) {
    printf("====================================================================\n");
    printf("     BO KIEM THU VALIDATION SPEC & BOUNDARY/ABNORMAL TEST SUITE     \n");
    printf("     Nguoi thuc hien: Ha Van Do (Task 7 - P0)                       \n");
    printf("====================================================================\n");

    test_id_validation();
    test_product_info_validation();
    test_price_quantity_validation();
    test_inventory_logic_validation();

    printf("\n--------------------------------------------------------------------\n");
    printf("KET QUA KIEM THU: Passed %d / %d tests (%.2f%%)\n",
           g_passed_tests, g_total_tests, (float)g_passed_tests / g_total_tests * 100.0);
    printf("--------------------------------------------------------------------\n");

    return (g_passed_tests == g_total_tests) ? 0 : 1;
}
