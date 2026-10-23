/* =========================================================================
 * MODULE 2: DYNAMIC MEMORY & HEAP LIFECYCLE MANAGEMENT (v1 - BẢN ĐÃ VÁ CHUẨN HOÁ)
 * Thanh vien phu trach: Nguyen Kieu Trang
 * ========================================================================= */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX_SAFE_CAPACITY 10000000 /* Giới hạn an toàn 10MB tránh cạn kiệt RAM */

/* KỊCH BẢN SC-03 (ĐÃ VÁ): Ngăn chặn tràn số nguyên và giá trị âm từ Fuzzer */
int calc_expand_capacity_v1(int current_cap, int factor) {
    if (current_cap <= 0 || factor <= 0) {
        return -1; 
    }
    if (current_cap > MAX_SAFE_CAPACITY / factor) {
        return -2; /* Từ chối giá trị vượt ngưỡng an toàn */
    }
    return current_cap * factor;
}

/* KỊCH BẢN SC-04 (ĐÃ VÁ): Safe Reallocation Pattern ngăn chặn Leak CWE-401 & TIMEOUT */
int realloc_buffer_leak_v1(int old_size, int new_size, int simulate_oom) {
    /* Guard 1: Chặn kích thước âm hoặc vượt quá 10MB (Ngăn chặn Fuzzer gây TIMEOUT 2GB) */
    if (old_size <= 0 || old_size > MAX_SAFE_CAPACITY || new_size <= 0 || new_size > MAX_SAFE_CAPACITY) {
        return -1;
    }

    /* Guard 2: Xử lý mô phỏng OOM riêng biệt, không cấp phát lãng phí */
    if (simulate_oom) {
        return -2; /* Mô phỏng từ chối cấp phát an toàn, không tạo leak */
    }

    char *buf = (char *)malloc(old_size);
    if (!buf) return -1;
    memset(buf, 'A', old_size - 1);

    /* Guard 3: Dùng con trỏ tạm tmp để tái cấp phát an toàn */
    char *tmp = (char *)realloc(buf, new_size);
    if (!tmp) {
        free(buf);   /* Nếu realloc thất bại vẫn giải phóng được con trỏ cũ */
        return -3;
    }

    free(tmp);       /* Thu hồi hoàn toàn bộ nhớ sau khi hoàn thành */
    return 0;
}

/* KỊCH BẢN SC-05 (ĐÃ VÁ): Pointer Nullification ngăn chặn Use-After-Free CWE-416 */
int cleanup_and_read_uaf_v1(int val) {
    int *ptr = (int *)malloc(sizeof(int));
    if (!ptr) return -1;
    *ptr = val;

    /* Guard: Đọc và lưu giá trị ra biến cục bộ trước khi giải phóng */
    int cached_val = *ptr;

    free(ptr);
    ptr = NULL; /* XÓA CON TRỎ TRÔI NỔI -> TRIỆT TIÊU HOÀN TOÀN CWE-416 UAF */

    return cached_val; /* Trả về giá trị an toàn */
}

