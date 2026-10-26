
/* =========================================================
 * MODULE 1: I/O PIPELINE - PHIEN BAN V0 (CO LOI)
 * Thanh vien: Nguyen Quang Tho
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

/* ---------------------------------------------------------
 * SC-01: NAP CHUOI VAO BUFFER - CWE-120 / CWE-121
 *
 * Trang thai Kripke:
 *   m1_nhan_chuoi
 *      |
 *      +-- len < 32  --> m1_len_nho_32
 *      |                 --> m1_chep_an_toan
 *      |
 *      +-- len == 32 --> m1_len_bang_32
 *      |                 --> m1_tran_1_byte
 *      |
 *      +-- len > 32  --> m1_len_lon_32
 *                        --> m1_tran_nhieu_byte
 *
 * Root cause:
 *   strcpy khong kiem tra dung luong buffer dich.
 * --------------------------------------------------------- */

int ingest_raw_line_v0(
    char *dst,
    int cap,
    const char *raw_line
) {
    /* Co tinh bo qua gioi han bo dem */
    (void)cap;

    /* LOI: strcpy sao chep ca ky tu '\0'.
     * Neu dst co 32 byte va raw_line dai 32 byte,
     * strcpy se ghi 33 byte.
     */
    strcpy(dst, raw_line);

    return (int)strlen(dst);
}

/* ---------------------------------------------------------
 * SC-02: DOC TRUONG DU LIEU - CWE-125
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

    /* LOI: co the doc ngoai gioi han mang */
    return token_offsets[field_idx];
}
