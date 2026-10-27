/* =========================================================================
 * CHUONG TRINH MAIN DEMO KIEM TOAN AN TOAN BO NHO (v0 - BAN CAI LOI)
 * De tai: Kiem toan an toan bo nho chuong trinh C quan ly kho hang dien may
 * Nhom 04 (Group 04) - Nguyen Kieu Trang, Nguyen Quang Tho, Ha Van Do
 * ========================================================================= */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Khai bao nguyen mau ham sinh truc tiep tu dac ta JSON (SpecVerificationLab) */
void ingest_raw_line(const char *raw_line);
int get_token_field(int token_count, int field_idx);
void expand_stock_items(void);
void release_stock_items(void);
int update_stock_quantity(int current_stock, int added_quantity);
int scale_product_price(int base_price, int rate);

/* Khai bao nguyen mau ham mo phong nghiep vu mo rong */
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
        const char *normal_input = "SP001;GaoST25;100;25000";
        printf("    Goi ham sinh tu dac ta: ingest_raw_line(\"%s\"):\n    ", normal_input);
        ingest_raw_line(normal_input);

        int token_res = get_token_field(4, 2);
        printf("    Goi ham sinh tu dac ta: get_token_field(token_count=4, field_idx=2) = %d\n", token_res);

        if (mode == 1) {
            char short_buf[16];
            const char *long_input = "VOUCHER-ELEC-2026-LONG-DATA-OVERFLOW-STRING";
            printf("    Kich hoat loi: ingest_raw_line_v0() gay tran stack...\n");
            ingest_raw_line_v0(short_buf, 16, long_input);
        }
    }

    if (mode == 0 || mode == 2) {
        printf("\n[2] Kiem tra Module 2: Vong doi bo nho Heap (Memory Leak & Use-After-Free)\n");
        printf("    Goi ham sinh tu dac ta: expand_stock_items() (CWE-401 Memory Leak)...\n");
        expand_stock_items();
        printf("    -> Cung cap vung nho 16 ints thanh cong nhung thieu free (leak)\n");

        if (mode == 2) {
            printf("    Kich hoat release_stock_items() gay loi UAF...\n");
            release_stock_items();
        } else {
            printf("    (Chay an toan che do demo release)\n");
        }
    }

    if (mode == 0 || mode == 3) {
        printf("\n[3] Kiem tra Module 3: Tinh toan ton kho (Integer Overflow CWE-190)\n");
        int stock = 2147483640;
        int add = 20;
        int res = update_stock_quantity(stock, add);
        printf("    Goi ham sinh tu dac ta: update_stock_quantity(%d, %d) = %d (Tran so lat am!)\n", stock, add, res);
        int price = scale_product_price(2000000000, 200000000);
        printf("    Goi ham sinh tu dac ta: scale_product_price(2B, 200M) = %d\n", price);
    }

    printf("\nHoan tat kiem tra ban v0.\n");
    return 0;
}
