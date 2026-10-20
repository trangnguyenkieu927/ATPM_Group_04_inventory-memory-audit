/* =========================================================================
 * MODULE 1: I/O & RAW INPUT BUFFER INGESTION PIPELINE (v0 - CO LOI)
 * Thanh vien phu trach: Nguyen Quang Tho
 * Muc tieu: Kiem toan giai doan Cho input va Nap du lieu tho vao Buffer RAM.
 * Cac loi chu dich:
 *   - ingest_raw_line_v0  : Copy bo qua capacity -> Tran bo dem Stack (CWE-120/121)
 *   - get_token_field_v0  : Doc chi so cot khong kiem tra bien -> Doc ngoai bien (CWE-125)
 * ========================================================================= */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* KICH BAN SC-01: CWE-120 / CWE-121 (Stack Buffer Overflow)
 * Giai doan RAM: Cho chuoi input tu ban phim/file CSV, nap vao buffer dong.
 * Root cause: Bo qua tham so cap, dung ham khong gioi han strcpy gay tran buffer. */
int ingest_raw_line_v0(char *dst, int cap, const char *raw_line) {
    (void)cap; /* Bo qua cap -> Tran bo dem neu do dai raw_line >= cap */
    strcpy(dst, raw_line);
    return (int)strlen(dst);
}

/* KICH BAN SC-02: CWE-125 (Out-of-bounds Read)
 * Giai doan RAM: Phan tach cac truong (ID, Name, Price, Stock) tu chuoi buffer.
 * Root cause: Doc chi so truong idx nhung bo qua do dai token_count cua dong du lieu. */
int get_token_field_v0(const int *token_offsets, int token_count, int field_idx) {
    (void)token_count; /* Thieu kiem tra 0 <= field_idx < token_count */
    return token_offsets[field_idx];
}
