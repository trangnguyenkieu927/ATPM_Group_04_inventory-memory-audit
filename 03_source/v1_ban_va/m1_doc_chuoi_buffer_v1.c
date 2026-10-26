
/* =========================================================
 * MODULE 1: I/O PIPELINE - PHIEN BAN V1 (DA VA)
 * Thanh vien: Nguyen Quang Tho
 *
 * Muc tieu:
 *   Ngan chan trang thai tran bo dem trong SC-01.
 *
 * Quy tac:
 *   len < 32  : Cho phep sao chep
 *   len >= 32 : Tu choi truoc khi sao chep
 *
 * Kich ban bo sung SC-02:
 *   Kiem tra chi so truoc khi doc mang token.
 * ========================================================= */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define M1_BUFFER_SIZE 32

/* ---------------------------------------------------------
 * SC-01: NAP CHUOI AN TOAN - VA CWE-120 / CWE-121
 *
 * Trang thai Kripke:
 *   m1_nhan_chuoi
 *        |
 *   m1_kiem_tra_bien
 *        |
 *        +-- len < 32
 *        |     --> m1_chep_gioi_han_an_toan
 *        |
 *        +-- len >= 32
 *              --> m1_tu_choi_chuoi_dai
 *
 * Khong co duong di den trang thai violation
 * trong mo hinh voi cap = 32.
 * --------------------------------------------------------- */

int ingest_raw_line_v1(
    char *dst,
    int cap,
    const char *raw_line
) {
    /* GUARD 1: Kiem tra con tro va dung luong */
    if (dst == NULL || raw_line == NULL ||
        cap != M1_BUFFER_SIZE) {
        return -1;
    }

    /* Xac dinh do dai chuoi nguon */
    size_t len = strlen(raw_line);

    /* GUARD 2: Chan truoc khi sao chep.
     * Can 1 byte de luu ky tu '\0'.
     */
    if (len >= M1_BUFFER_SIZE) {
        return -2;
    }

    /* Chi sao chep khi len <= 31 */
    memcpy(dst, raw_line, len);

    /* Ket thuc chuoi C */
    dst[len] = '\0';

    return (int)len;
}

/* ---------------------------------------------------------
 * SC-02: DOC TRUONG DU LIEU AN TOAN - VA CWE-125
 *
 * Kiem tra:
 *   token_offsets != NULL
 *   token_count > 0
 *   0 <= field_idx < token_count
 * --------------------------------------------------------- */

int get_token_field_v1(
    const int *token_offsets,
    int token_count,
    int field_idx
) {
    /* GUARD 1: Kiem tra mang va so phan tu */
    if (token_offsets == NULL || token_count <= 0) {
        return -1;
    }

    /* GUARD 2: Kiem tra chi so */
    if (field_idx < 0 || field_idx >= token_count) {
        return -2;
    }

    /* Chi doc khi chi so hop le */
    return token_offsets[field_idx];
}
