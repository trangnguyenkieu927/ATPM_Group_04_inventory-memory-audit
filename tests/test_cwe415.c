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
 * 1. KIỂM THỬ CWE-415: MODULE PRODUCTLIST (QUẢN LÝ BỘ NHỚ DANH SÁCH SP)
 * ========================================================================= */
static void test_product_list_double_free(void) {
    printf("=== 1. KIEM THU PHONG THU DOUBLE FREE TAI PRODUCTLIST (product_list_free) ===\n");

    /* 1.1 Khoi tao danh sach va giai phong 2 lan lien tiep */
    ProductList list;
    product_list_init(&list);
    Product p1 = {"SP001", "Laptop Dell XPS", "Laptop", "Chiec", 10, 1500.0};
    add_product(&list, &p1);

    /* Lan free thu nhat */
    product_list_free(&list);
    TEST_ASSERT(list.items == NULL,
                "Lan free 1: Con tro items duoc gan ve NULL de triet tieu dia chi Heap cu");
    TEST_ASSERT(list.count == 0 && list.capacity == 0,
                "Lan free 1: Reset count = 0 va capacity = 0 dong bo");

    /* Lan free thu hai: Khong duoc gay crash (chong Double Free) */
    product_list_free(&list);
    TEST_ASSERT(list.items == NULL,
                "Lan free 2 (Giai phong lai): Ham phong thu thanh cong, 0 bi crash [Chong CWE-415]");

    /* 1.2 Stress Test: Goi free lien tuc 5 lan tren cung mot bien */
    for (int i = 0; i < 5; ++i) {
        product_list_free(&list);
    }
    TEST_ASSERT(list.items == NULL,
                "Giai phong lien tuc 5 lan: Tinh chat Idempotent hoat dong on dinh tuyet doi");

    /* 1.3 Giai phong danh sach chua tung duoc cap phat bo nho (items = NULL) */
    ProductList unallocated_list = {NULL, 0, 0};
    product_list_free(&unallocated_list);
    TEST_ASSERT(unallocated_list.items == NULL,
                "Giai phong danh sach chua cap phat (items=NULL): Xu ly an toan, khong dereference");

    /* 1.4 Chu trinh Tai cap phat va Giai phong nhieu lan (Init -> Add -> Free -> Free) */
    product_list_init(&list);
    Product p2 = {"SP002", "Chuot Logitech", "Phu kien", "Con", 50, 99.0};
    add_product(&list, &p2);
    product_list_free(&list);
    product_list_free(&list);
    TEST_ASSERT(list.items == NULL,
                "Chu trinh tai cap phat va giai phong kep: Bo nho sach se, 0 loi Heap Metadata");
}

/* =========================================================================
 * 2. KIỂM THỬ CWE-415: MODULE INVENTORY (QUẢN LÝ BỘ NHỚ KHO HÀNG)
 * ========================================================================= */
static void test_inventory_double_free(void) {
    printf("\n=== 2. KIEM THU PHONG THU DOUBLE FREE TAI INVENTORY (inventory_free) ===\n");

    /* 2.1 Khoi tao Inventory va giai phong 2 lan lien tiep */
    Inventory inv;
    inventory_init(&inv);
    Product p = {"SP001", "Man hinh LG", "Man hinh", "Chiec", 15, 350.0};
    inventory_add_product(&inv, &p);

    /* Lan free thu nhat */
    inventory_free(&inv);
    TEST_ASSERT(inv.products == NULL,
                "Lan free 1: Con tro products duoc gan ve NULL sau khi giai phong");
    TEST_ASSERT(inv.count == 0 && inv.capacity == 0,
                "Lan free 1: Reset so luong va capacity ve 0");

    /* Lan free thu hai: Kiem tra khong double free */
    inventory_free(&inv);
    TEST_ASSERT(inv.products == NULL,
                "Lan free 2: inventory_free(inv) xu ly an toan khi products da NULL [Chong CWE-415]");

    /* 2.2 Stress Test: Goi free lien tuc 5 lan tren cung mot Inventory */
    for (int i = 0; i < 5; ++i) {
        inventory_free(&inv);
    }
    TEST_ASSERT(inv.products == NULL,
                "Giai phong Inventory lien tiep 5 lan: Hoan toan an toan va khong crash");

    /* 2.3 Giai phong Inventory chua tung khoi tao bo nho */
    Inventory uninit_inv = {NULL, 0, 0};
    inventory_free(&uninit_inv);
    TEST_ASSERT(uninit_inv.products == NULL,
                "Giai phong Inventory chua cap phat (products=NULL): Phong thu an toan");

    /* 2.4 Mo rong bo nho dong bang realloc (inventory_grow), sau do giai phong kep */
    inventory_init(&inv);
    for (int i = 0; i < 25; ++i) {
        char id_buf[MAX_ID_LEN];
        snprintf(id_buf, sizeof(id_buf), "TEST_%02d", i + 1);
        Product tmp = {"", "SP Test", "Linh kien", "Chiec", 5, 20.0};
        safe_string_copy(tmp.id, sizeof(tmp.id), id_buf);
        inventory_add_product(&inv, &tmp);
    }
    /* Giai phong sau realloc */
    inventory_free(&inv);
    inventory_free(&inv);
    TEST_ASSERT(inv.products == NULL,
                "Giai phong kep sau khi bo nho mo rong (realloc): Vung nho moi duoc don dep sach, 0 double free");
}

/* =========================================================================
 * 3. KIỂM THỬ CWE-415: DỌN DẸP BỘ NHỚ TRONG LUỒNG XỬ LÝ NGOẠI LỆ / FILE I/O
 * ========================================================================= */
static void test_error_cleanup_double_free(void) {
    printf("\n=== 3. KIEM THU CWE-415 TAI CAC LUONG XU LY NGOAI LE / ROLLBACK CLEANUP ===\n");

    Inventory inv;
    inventory_init(&inv);

    /* Mo tep chua du lieu bat thuong / bi loi dinh dang de kich hoat luong thoat som */
    const char *path = find_file_path("cwe415_double_free_input.txt");
    if (path == NULL) {
        path = "tests/abnormal/cwe415_double_free_input.txt";
    }

    int st = load_products_from_file(&inv, path);
    /* File chua dong loi nen load_products_from_file se dung giua chung va tra ve ma loi */
    TEST_ASSERT(st != STATUS_SUCCESS,
                "Phat hien du lieu loi trong tep cwe415_double_free_input.txt (Thoat luong an toan)");

    /* Mo phong rollback cleanup: Goi don dep trong khoi xu ly loi */
    inventory_free(&inv);
    TEST_ASSERT(inv.products == NULL,
                "Don dep bo nho tai diem bat loi (Rollback Cleanup) thanh cong");

    /* Mo phong don dep bo nho tiep tuc tai cuoi ham (Double Cleanup tai Finally Block) */
    inventory_free(&inv);
    TEST_ASSERT(inv.products == NULL,
                "Don dep bo nho lan 2 tai Finally block: Xu ly an toan, khong bi Double Free");
}

/* =========================================================================
 * 4. KIỂM THỬ CWE-415: STRESS TEST VÒNG ĐỜI BỘ NHỚ LẶP LẠI (50 CYCLES)
 * ========================================================================= */
static void test_memory_lifecycle_stress(void) {
    printf("\n=== 4. STRESS TEST VONG DOI BO NHO LAP LAI 50 CHU TRINH (LIFECYCLE STRESS) ===\n");

    int all_ok = 1;
    for (int cycle = 1; cycle <= 50; ++cycle) {
        ProductList list;
        if (product_list_init(&list) != STATUS_SUCCESS) {
            all_ok = 0;
            break;
        }

        Product p = {"CYCLE_SP", "SanPhamVongDoi", "Loai", "Cai", 10, 50.0};
        add_product(&list, &p);

        /* Thuc hien 2 lan free lien tiep moi chu trinh */
        product_list_free(&list);
        product_list_free(&list);

        if (list.items != NULL || list.count != 0 || list.capacity != 0) {
            all_ok = 0;
            break;
        }
    }

    TEST_ASSERT(all_ok,
                "Chay lien tuc 50 chu trinh Cap phat - Nap du lieu - Giai phong kep: Bo nho luon toan ven 100%");
}

/* =========================================================================
 * 5. HÀM MAIN CHẠY TOÀN BỘ SUITE KIỂM THỬ CWE-415
 * ========================================================================= */
int main(void) {
    printf("=========================================================================================\n");
    printf("                BO KIEM THU KIEM TOAN AN TOAN BO NHO: CWE-415 - DOUBLE FREE              \n");
    printf("=========================================================================================\n");

    test_product_list_double_free();
    test_inventory_double_free();
    test_error_cleanup_double_free();
    test_memory_lifecycle_stress();

    printf("\n=========================================================================================\n");
    printf("TONG KET KIEM TOAN CWE-415: Tong so ca: %d | Dat: %d | Loi: %d\n",
           g_tests_run, g_tests_passed, g_tests_run - g_tests_passed);
    printf("DANH GIA: %s\n",
           (g_tests_run == g_tests_passed)
               ? "HE THONG DAT CHUAN AN TOAN BO NHO - 100% PHONG THU THANH CONG LOI DOUBLE FREE [PASS]"
               : "PHAT HIEN NGUY CO CWE-415 CAN KHAC PHUC");
    printf("=========================================================================================\n\n");

    return (g_tests_run == g_tests_passed) ? 0 : 1;
}
