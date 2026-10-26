/* =========================================================================
 * CHUONG TRINH MAIN DEMO KIEM TOAN AN TOAN BO NHO (v0 - BAN CAI LOI)
 * De tai: Kiem toan an toan bo nho chuong trinh C quan ly kho hang dien may
 * Nhom 04 (Group 04) - Nguyen Kieu Trang, Nguyen Quang Tho, Ha Van Do
 * ========================================================================= */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Khai bao nguyen mau ham tu cac module */
int ingest_raw_line_v0(char *dst, int cap, const char *raw_line);
int get_token_field_v0(const int *token_offsets, int token_count, int field_idx);
int calc_expand_capacity_v0(int current_cap, int factor);
int realloc_buffer_leak_v0(int old_size, int new_size, int simulate_oom);
int cleanup_and_read_uaf_v0(int val);
int update_stock_quantity_v0(int current_stock, int added_quantity);
int scale_product_price_v0(int base_price, int rate);

int main(int argc, char *argv[]) {
    printf("====================================================================\n");
    printf(" CHUONG TRINH THUC THI KIEM TOAN AN TOAN BO NHO KHO HANG (BAN V0)\n");
    printf("====================================================================\n");

    int mode = 0;
    if (argc > 1) {
        mode = atoi(argv[1]);
    }

    if (mode == 0 || mode == 1) {
        printf("\n[1] Kiem tra Module 1: Ingest chuoi ma phieu kho (Buffer Overflow)\n");
        char short_buf[16];
        const char *long_input = "VOUCHER-ELEC-2026-LONG-DATA-OVERFLOW-STRING";
        printf("    Buffer capacity: 16 bytes | Do dai chuoi dau vao: %zu bytes\n", strlen(long_input));
        if (mode == 1) {
            printf("    Dang thuc hien ingest_raw_line_v0() gay tran stack...\n");
            ingest_raw_line_v0(short_buf, 16, long_input);
        } else {
            printf("    (Chay an toan che do demo)\n");
        }
    }

    if (mode == 0 || mode == 2) {
        printf("\n[2] Kiem tra Module 2: Vong doi bo nho Heap (Use-After-Free CWE-416)\n");
        if (mode == 2) {
            printf("    Kich hoat doc con tro treo sau khi giai phong...\n");
            int val = cleanup_and_read_uaf_v0(123456);
            printf("    Gia tri doc duoc tu con tro da free: %d\n", val);
        } else {
            printf("    (Chay an toan che do demo)\n");
        }
    }

    if (mode == 0 || mode == 3) {
        printf("\n[3] Kiem tra Module 3: Tinh toan ton kho (Integer Overflow CWE-190)\n");
        int stock = 2147483640;
        int add = 20;
        int res = update_stock_quantity_v0(stock, add);
        printf("    Phep tinh: %d + %d = %d (Trang thai tran so nguyen am!)\n", stock, add, res);
    }

    printf("\nHoan tat kiem tra ban v0.\n");
    return 0;
}
