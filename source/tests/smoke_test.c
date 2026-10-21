#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>

#include "../include/models.h"
#include "../include/inventory.h"
#include "../include/product.h"
#include "../include/file_io.h"
#include "../include/validation.h"
#include "../include/utils.h"
#include "../include/report.h"

static int g_smoke_total = 0;
static int g_smoke_passed = 0;

typedef struct {
    int id;
    const char *operation;
    const char *input_desc;
    const char *expected;
    char actual[128];
    int exit_code;
    const char *status;
} SmokeRecord;

static void log_record(int id, const char *op, const char *input, const char *expected,
                       const char *actual, int exit_code, int pass) {
    g_smoke_total++;
    if (pass) g_smoke_passed++;
    printf("| %-2d | %-22s | %-30s | %-24s | %-24s | %-4d | %-4s |\n",
           id, op, input, expected, actual, exit_code, pass ? "PASS" : "FAIL");
}

int main(void) {
    printf("========================================================================================================================\n");
    printf("                                      SMOKE TEST CHUC NANG NEN TANG - BASELINE V0                                       \n");
    printf("========================================================================================================================\n");
    printf("| ID | Thao tac               | Input                          | Expected                 | Actual                   | Code | Pass |\n");
    printf("|----|------------------------|--------------------------------|--------------------------|--------------------------|------|------|\n");

    /* 1. Khoi tao danh sach san pham */
    Inventory inv;
    int st1 = inventory_init(&inv);
    log_record(1, "Khoi tao danh sach", "Inventory pointer", "STATUS_SUCCESS (0)",
               (st1 == STATUS_SUCCESS) ? "STATUS_SUCCESS (0)" : "ERROR",
               st1, st1 == STATUS_SUCCESS);

    /* 2. Them san pham moi (6 truong) */
    Product p1 = {"SP_TEST01", "Laptop Dell Precision", "Laptop", "Chiec", 10, 2500.0};
    int st2 = inventory_add_product(&inv, &p1);
    log_record(2, "Them san pham 6 truong", "SP_TEST01|Dell|Laptop|10", "STATUS_SUCCESS (0)",
               (st2 == STATUS_SUCCESS) ? "STATUS_SUCCESS (0)" : "ERROR",
               st2, st2 == STATUS_SUCCESS);

    /* 3. Chan trung ma san pham */
    int st3 = inventory_add_product(&inv, &p1);
    log_record(3, "Chan trung lap ID", "SP_TEST01 (da ton tai)", "STATUS_ERR_DUPLICATE_ID",
               (st3 == STATUS_ERR_DUPLICATE_ID) ? "STATUS_ERR_DUPLICATE_ID" : "ERROR",
               st3, st3 == STATUS_ERR_DUPLICATE_ID);

    /* 4. Tim kiem theo ID */
    const Product *found = find_product(&inv, "SP_TEST01");
    int pass4 = (found != NULL && strcmp(found->name, "Laptop Dell Precision") == 0);
    log_record(4, "Tim kiem theo ID", "ID = SP_TEST01", "Tim thay Dell Precision",
               pass4 ? "Tim thay Dell Precision" : "Khong tim thay",
               pass4 ? 0 : -1, pass4);

    /* 5. Tim kiem theo ten (khong phan biet hoa/thuong) */
    Product search_res[4];
    size_t found_cnt = 0;
    int st5 = search_products_by_name(&inv, "dell", search_res, 4, &found_cnt);
    int pass5 = (st5 == STATUS_SUCCESS && found_cnt == 1);
    log_record(5, "Tim kiem theo ten", "Keyword = 'dell'", "1 ket qua phu hop",
               pass5 ? "1 ket qua phu hop" : "Khong khop",
               st5, pass5);

    /* 6. Cap nhat san pham */
    int st6 = update_product(&inv, "SP_TEST01", "Laptop Dell Precision Gen2", NULL, NULL, 12, 2600.0);
    const Product *p_updated = find_product(&inv, "SP_TEST01");
    int pass6 = (st6 == STATUS_SUCCESS && p_updated != NULL && p_updated->quantity == 12 && p_updated->price == 2600.0);
    log_record(6, "Cap nhat san pham", "ID=SP_TEST01, SL=12, Gia=2600", "Cap nhat thanh cong",
               pass6 ? "Cap nhat thanh cong" : "That bai",
               st6, pass6);

    /* 7. Nhap kho & Chan tran so nguyen */
    int st7a = import_stock(&inv, "SP_TEST01", 8);
    int pass7a = (st7a == STATUS_SUCCESS && find_product(&inv, "SP_TEST01")->quantity == 20);
    log_record(7, "Nhap kho hop le", "ID=SP_TEST01, qty=+8", "Ton kho tang len 20",
               pass7a ? "Ton kho tang len 20" : "Loi nhap kho",
               st7a, pass7a);

    int st7b = import_stock(&inv, "SP_TEST01", INT_MAX);
    log_record(8, "Chan tran so nhap kho", "ID=SP_TEST01, qty=INT_MAX", "STATUS_ERR_OVERFLOW",
               (st7b == STATUS_ERR_OVERFLOW) ? "STATUS_ERR_OVERFLOW" : "ERROR",
               st7b, st7b == STATUS_ERR_OVERFLOW);

    /* 8. Xuat kho & Chan xuat qua ton */
    int st8a = export_stock(&inv, "SP_TEST01", 5);
    int pass8a = (st8a == STATUS_SUCCESS && find_product(&inv, "SP_TEST01")->quantity == 15);
    log_record(9, "Xuat kho hop le", "ID=SP_TEST01, qty=-5", "Ton kho giam con 15",
               pass8a ? "Ton kho giam con 15" : "Loi xuat kho",
               st8a, pass8a);

    int st8b = export_stock(&inv, "SP_TEST01", 100);
    log_record(10, "Chan xuat qua ton kho", "ID=SP_TEST01, qty=100 (ton=15)", "ERR_INSUFFICIENT_STOCK",
               (st8b == STATUS_ERR_INSUFFICIENT_STOCK) ? "ERR_INSUFFICIENT_STOCK" : "ERROR",
               st8b, st8b == STATUS_ERR_INSUFFICIENT_STOCK);

    /* 9. Ghi du lieu ra tep 6 truong */
    const char *test_out = "data/test_smoke_products.txt";
    int st9 = save_products_to_file(&inv, test_out);
    log_record(11, "Ghi tep du lieu 6 truong", "data/test_smoke_products.txt", "STATUS_SUCCESS (0)",
               (st9 == STATUS_SUCCESS) ? "STATUS_SUCCESS (0)" : "ERROR",
               st9, st9 == STATUS_SUCCESS);

    /* 10. Doc du lieu tu tep 6 truong */
    Inventory reloaded;
    inventory_init(&reloaded);
    int st10 = load_products_from_file(&reloaded, test_out);
    int pass10 = (st10 == STATUS_SUCCESS && reloaded.count == 1 && strcmp(reloaded.items[0].id, "SP_TEST01") == 0);
    log_record(12, "Doc tep du lieu 6 truong", "data/test_smoke_products.txt", "Nap thanh cong 1 san pham",
               pass10 ? "Nap thanh cong 1 san pham" : "Loi nap tep",
               st10, pass10);
    inventory_free(&reloaded);
    remove(test_out);

    /* 11. Kiem thu tuong thich tep 4 truong cu (Legacy Format) */
    const char *legacy_file = "data/test_legacy_4field.txt";
    FILE *fl = fopen(legacy_file, "w");
    assert(fl != NULL);
    fprintf(fl, "OLD01|Ban phim co|25|45.00\n");
    fclose(fl);

    Inventory legacy_inv;
    inventory_init(&legacy_inv);
    int st11 = load_products_from_file(&legacy_inv, legacy_file);
    int pass11 = (st11 == STATUS_SUCCESS && legacy_inv.count == 1 &&
                  strcmp(legacy_inv.items[0].category, "ChuaPhanLoai") == 0 &&
                  strcmp(legacy_inv.items[0].unit, "Cai") == 0 &&
                  legacy_inv.items[0].quantity == 25);
    log_record(13, "Tuong thich tep 4 truong", "OLD01|Ban phim|25|45.00", "Default cat/unit hop le",
               pass11 ? "Default cat/unit hop le" : "Loi doc 4 truong",
               st11, pass11);
    inventory_free(&legacy_inv);
    remove(legacy_file);

    /* 12. Bao cao & Thong ke */
    Product p2 = {"SP_TEST02", "Chuot khong day", "Phu kien", "Con", 0, 30.0};
    inventory_add_product(&inv, &p2);
    InventorySummary summary;
    int st12 = calculate_inventory_summary(&inv, 10, &summary);
    int pass12 = (st12 == STATUS_SUCCESS && summary.total_products == 2 && summary.out_of_stock_count == 1);
    log_record(14, "Thong ke tong quan kho", "2 SP (1 ton 15, 1 ton 0)", "Tong 2 SP, 1 het hang",
               pass12 ? "Tong 2 SP, 1 het hang" : "Loi thong ke",
               st12, pass12);

    /* 13. Xuat bao cao ra tep */
    const char *rep_file = "data/test_smoke_report.txt";
    int st13 = export_inventory_report_to_file(&inv, rep_file, 10);
    int pass13 = (st13 == STATUS_SUCCESS);
    log_record(15, "Xuat bao cao ra tep", "data/test_smoke_report.txt", "STATUS_SUCCESS (0)",
               pass13 ? "STATUS_SUCCESS (0)" : "ERROR",
               st13, pass13);
    remove(rep_file);

    /* 14. Xoa san pham */
    int st14 = delete_product(&inv, "SP_TEST01");
    int pass14 = (st14 == STATUS_SUCCESS && inv.count == 1 && find_product(&inv, "SP_TEST01") == NULL);
    log_record(16, "Xoa san pham", "ID = SP_TEST01", "Xoa thanh cong",
               pass14 ? "Xoa thanh cong" : "Loi xoa SP",
               st14, pass14);

    /* 15. Giai phong bo nho */
    inventory_free(&inv);
    int pass15 = (inv.items == NULL && inv.count == 0 && inv.capacity == 0);
    log_record(17, "Giai phong bo nho", "inventory_free(&inv)", "Con tro NULL, size=0",
               pass15 ? "Con tro NULL, size=0" : "Chua reset bo nho",
               0, pass15);

    printf("------------------------------------------------------------------------------------------------------------------------\n");
    printf("TONG KET SMOKE TEST: Tong so ca: %d | Dat: %d | Loi: %d | Ket qua: %s\n",
           g_smoke_total, g_smoke_passed, g_smoke_total - g_smoke_passed,
           (g_smoke_passed == g_smoke_total) ? "TAT CA DEU PASS (100%)" : "CO CA THAT BAI");
    printf("========================================================================================================================\n");

    return (g_smoke_passed == g_smoke_total) ? 0 : 1;
}
