/* =========================================================
 * MODULE 1: I/O PIPELINE - PHIEN BAN V0 (CO LOI)
 * Thanh vien: Nguyen Quang Tho - MSV: 23010322
 *
 * Kich ban chinh SC-01:
 *   Doc ma phieu kho vao bo dem Stack 32 byte.
 *
 * Gioi han:
 *   0 - 31 byte : Hop le
 *   32 byte     : Tran 1 byte (ky tu '\0')
 *   >= 33 byte  : Tran nhieu byte
 *
 * Loi chu dich:
 *   CWE-120 / CWE-121: Stack Buffer Overflow
 *   CWE-193: Off-by-one
 *
 * Kich ban bo sung SC-02:
 *   CWE-125: Out-of-bounds Read
 * ========================================================= */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define M1_BUFFER_SIZE 32

/* =========================================================
 * CAC HAM SINH MA CHUAN TU DAC TA JSON (SpecVerificationLab)
 * ========================================================= */

/* sinh từ đặc tả 'quanly_khohang_m1_doc_chuoi_buffer', hàm 'ingest_raw_line', kind='buffer_copy' */
void ingest_raw_line(const char *raw_line) {
    char buffer[32];
    strcpy(buffer, raw_line);   /* sinh từ đặc tả: buffer-overflow */
    printf("%s\n", buffer);
}

/* sinh từ đặc tả 'quanly_khohang_m1_doc_chuoi_buffer', hàm 'get_token_field', kind='arithmetic' */
int get_token_field(int token_count, int field_idx) {
    return token_count + field_idx;
}

/* ---------------------------------------------------------
 * SC-01: NAP CHUOI VAO BUFFER - CWE-120 / CWE-121 / CWE-193
 *
 * Trang thai Kripke:
 *   m1_nhan_dong_dulieu -> m1_kiem_tra_chieu_dai -> m1_tran_stack_buffer
 *
 * Root cause:
 *   strcpy khong kiem tra dung luong buffer dich (32 byte).
 * --------------------------------------------------------- */
int ingest_raw_line_v0(
    char *dst,
    int cap,
    const char *raw_line
) {
    /* Co tinh bo qua gioi han bo dem */
    (void)cap;

    /* LOI: strcpy sao chep ca ky tu '\0'.
     * Neu dst co 32 byte va raw_line dai >= 32 byte,
     * strcpy se ghi de khung stack frame.
     */
    strcpy(dst, raw_line);

    return (int)strlen(dst);
}

/* ---------------------------------------------------------
 * SC-02: DOC TRUONG DU LIEU - CWE-125
 *
 * Trang thai Kripke:
 *   m1_tach_token_hang_hoa -> m1_truy_cap_ngoai_bien
 *
 * Root cause:
 *   Khong kiem tra field_idx co nam trong
 *   khoang [0, token_count - 1] hay khong.
 * --------------------------------------------------------- */
int get_token_field_v0(
    const int *token_offsets,
    int token_count,
    int field_idx
) {
    /* Co tinh bo qua so luong token */
    (void)token_count;

    /* LOI: co the doc ngoai gioi han mang (chi so am hoac vuot tran) */
    return token_offsets[field_idx];
}
