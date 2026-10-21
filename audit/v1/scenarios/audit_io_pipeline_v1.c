/* =========================================================================
 * MODULE 1: I/O & RAW INPUT BUFFER INGESTION PIPELINE (v1 - DA VA DEFENSIVE GUARD)
 * Thanh vien phu trach: Nguyen Quang Tho
 * ========================================================================= */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* KICH BAN SC-01 (VÁ): CWE-120 / CWE-121
 * Guard: Kiem tra do dai chuoi nguon truoc khi copy. Neu >= cap thi tu choi ngay. */
int ingest_raw_line_v1(char *dst, int cap, const char *raw_line) {
    if (!dst || !raw_line || cap <= 0) {
        return -1; /* Loi con tro hoac kich thuoc cap khong hop le */
    }
    size_t len = strlen(raw_line);
    if (len >= (size_t)cap) {
        return -2; /* Tu choi: Chuoi vuot qua dung luong buffer (Chan Buffer Overflow) */
    }
    memcpy(dst, raw_line, len);
    dst[len] = '\0';
    return (int)len;
}

/* KICH BAN SC-02 (VÁ): CWE-125
 * Guard: Kiem tra chat che ranh gioi chi so truoc khi truy cap mang token. */
int get_token_field_v1(const int *token_offsets, int token_count, int field_idx) {
    if (!token_offsets || token_count <= 0) {
        return -1; /* Mang khong hop le */
    }
    if (field_idx < 0 || field_idx >= token_count) {
        return -2; /* Tu choi: Chi so ngoai bien (Chan Out-of-bounds Read) */
    }
    return token_offsets[field_idx];
}
