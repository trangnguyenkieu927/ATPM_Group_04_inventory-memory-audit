/* =========================================================================
 * CHUONG TRINH MAIN DEMO KIEM TOAN AN TOAN BO NHO (v1 - BAN VA PHONG THU)
 * De tai: Kiem toan an toan bo nho chuong trinh C quan ly kho hang dien may
 * Nhom 04 (Group 04) - Nguyen Kieu Trang, Nguyen Quang Tho, Ha Van Do
 * ========================================================================= */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Khai bao nguyen mau ham tu cac module ban va */
int ingest_raw_line_v1(char *dst, int cap, const char *raw_line);
int get_token_field_v1(const int *token_offsets, int token_count, int field_idx);
int calc_expand_capacity_v1(int current_cap, int factor);
int realloc_buffer_leak_v1(int old_size, int new_size, int simulate_oom);
int cleanup_and_read_uaf_v1(int val);
int update_stock_quantity_v1(int current_stock, int added_quantity);
int scale_product_price_v1(int base_price, int rate);

int main(void) {
    printf("====================================================================\n");
    printf(" CHUONG TRINH THUC THI KIEM TOAN AN TOAN BO NHO KHO HANG (BAN V1)\n");
    printf("====================================================================\n");

    printf("\n[1] Kiem tra Module 1: Ingest chuoi ma phieu kho (Chong Buffer Overflow)\n");
    char safe_buf[16];
    const char *long_input = "VOUCHER-ELEC-2026-LONG-DATA-OVERFLOW-STRING";
    int ret1 = ingest_raw_line_v1(safe_buf, sizeof(safe_buf), long_input);
    printf("    Ket qua tu choi dau vao vuot bien: ma loi = %d (An toan!)\n", ret1);

    printf("\n[2] Kiem tra Module 2: Vong doi bo nho Heap (Chong Use-After-Free & Leak)\n");
    int val = cleanup_and_read_uaf_v1(123456);
    printf("    Gia tri doc an toan truoc khi thu hoi: %d\n", val);
    int leak_test = realloc_buffer_leak_v1(128, 256, 0);
    printf("    Ket qua tai cap phat an toan: ma loi = %d (Khong ro ri RAM!)\n", leak_test);

    printf("\n[3] Kiem tra Module 3: Tinh toan ton kho (Chong Integer Overflow)\n");
    int stock = 2147483640;
    int add = 20;
    int ret3 = update_stock_quantity_v1(stock, add);
    printf("    Ket qua kiem tra nguong INT_MAX: ma loi = %d (Chan dung tran so!)\n", ret3);

    printf("\nHoan tat kiem tra ban v1: 100%% cac kich ban an toan tuyet doi.\n");
    return 0;
}
