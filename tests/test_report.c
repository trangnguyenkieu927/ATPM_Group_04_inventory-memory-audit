#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../source/include/models.h"
#include "../source/include/product.h"
#include "../source/include/report.h"
#include "../source/include/utils.h"

static int g_tests_run = 0;
static int g_tests_passed = 0;

#define TEST_ASSERT(expr, msg) do { \
    g_tests_run++; \
    if (expr) { \
        printf("  [PASS] %s\n", msg); \
        g_tests_passed++; \
    } else { \
        printf("  [FAIL] %s (Line %d)\n", msg, __LINE__); \
    } \
} while (0)

int main(void) {
    printf("=======================================================================\n");
    printf("                  UNIT TEST: MODULE BAO CAO (REPORT.H / REPORT.C)      \n");
    printf("=======================================================================\n\n");

    /* Khoi tao danh sach san pham de test */
    ProductList list;
    product_list_init(&list);

    Product p1 = {"SP001", "Laptop Dell", "Laptop", "Chiec", 10, 1000.0};
    Product p2 = {"SP002", "Chuot Logitech", "Phu kien", "Con", 0, 50.0};
    Product p3 = {"SP003", "Ban phim co", "Phu kien", "Chiec", 3, 80.0};
    Product p4 = {"SP004", "Man hinh Dell", "Man hinh", "Chiec", 25, 300.0};

    add_product(&list, &p1);
    add_product(&list, &p2);
    add_product(&list, &p3);
    add_product(&list, &p4);

    /* 1. Test tinh toan tong quan (calculate_inventory_summary) */
    printf("=== 1. Test calculate_inventory_summary ===\n");
    InventorySummary summary;
    int st1 = calculate_inventory_summary(NULL, 5, &summary);
    TEST_ASSERT(st1 == STATUS_ERR_NULL_PTR, "Chan con tro list NULL");

    int st2 = calculate_inventory_summary(&list, 5, NULL);
    TEST_ASSERT(st2 == STATUS_ERR_NULL_PTR, "Chan con tro summary NULL");

    int st3 = calculate_inventory_summary(&list, 5, &summary);
    TEST_ASSERT(st3 == STATUS_SUCCESS, "Tinh toan summary thanh cong");
    TEST_ASSERT(summary.total_products == 4, "Tong so mat hang = 4");
    TEST_ASSERT(summary.total_quantity == 38, "Tong so luong ton = 10 + 0 + 3 + 25 = 38");
    /* 10*1000 + 0*50 + 3*80 + 25*300 = 10000 + 0 + 240 + 7500 = 17740.0 */
    TEST_ASSERT(summary.total_inventory_value == 17740.0, "Tong gia tri ton = 17740.0 $");
    TEST_ASSERT(summary.out_of_stock_count == 1, "So mat hang het hang (SP002) = 1");
    TEST_ASSERT(summary.low_stock_count == 1, "So mat hang sap het hang (SP003, SL=3 <= 5) = 1");
    TEST_ASSERT(strcmp(summary.most_expensive_product.id, "SP001") == 0, "SP gia cao nhat la SP001");
    TEST_ASSERT(strcmp(summary.least_expensive_product.id, "SP002") == 0, "SP gia thap nhat la SP002");

    /* 2. Test co cau theo danh muc (calculate_category_summaries) */
    printf("\n=== 2. Test calculate_category_summaries ===\n");
    CategorySummary cats[16];
    size_t cat_count = 0;
    int c_st = calculate_category_summaries(&list, cats, 16, &cat_count);
    TEST_ASSERT(c_st == STATUS_SUCCESS, "Tinh toan category summary thanh cong");
    TEST_ASSERT(cat_count == 3, "So luong danh muc duy nhat = 3 (Laptop, Phu kien, Man hinh)");

    /* 3. Test xuat bao cao ra file */
    printf("\n=== 3. Test export_inventory_report_to_file ===\n");
    const char *test_report_file = "test_inventory_report.txt";
    int exp_st = export_inventory_report_to_file(&list, test_report_file, 5);
    TEST_ASSERT(exp_st == STATUS_SUCCESS, "Xuat bao cao ra file thanh cong");

    FILE *rf = fopen(test_report_file, "r");
    TEST_ASSERT(rf != NULL, "Mo duoc file bao cao vua xuat");
    if (rf != NULL) {
        fclose(rf);
        remove(test_report_file);
    }

    /* 4. Test report_statistics (xuat tong so luong va gia tri) */
    printf("\n=== 4. Test report_statistics ===\n");
    long long stat_qty = 0;
    double stat_val = 0.0;
    int s_null = (report_statistics)(NULL, &stat_qty, &stat_val);
    TEST_ASSERT(s_null == STATUS_ERR_NULL_PTR, "report_statistics chan list NULL");

    int s_ok = (report_statistics)(&list, &stat_qty, &stat_val);
    TEST_ASSERT(s_ok == STATUS_SUCCESS, "report_statistics thanh cong");
    TEST_ASSERT(stat_qty == 38, "report_statistics xuat tong so luong dung (38)");
    TEST_ASSERT(stat_val == 17740.0, "report_statistics xuat tong gia tri dung (17740.0 $)");

    int s_macro1 = report_statistics(&list);
    TEST_ASSERT(s_macro1 == STATUS_SUCCESS, "report_statistics goi voi 1 tham so (macro) thanh cong");

    product_list_free(&list);

    printf("\n=======================================================================\n");
    printf("KET QUA KIEM THU: Tong so ca: %d | Dat: %d | Loi: %d\n",
           g_tests_run, g_tests_passed, g_tests_run - g_tests_passed);
    printf("TRANG THAI: %s\n", (g_tests_run == g_tests_passed) ? "TAT CA DEU PASS [OK]" : "CO LOI XAY RA");
    printf("=======================================================================\n\n");

    return (g_tests_run == g_tests_passed) ? 0 : 1;
}
