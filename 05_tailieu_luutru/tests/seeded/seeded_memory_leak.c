/*
 * SEEDED_VULNERABILITY: Missing Release of Memory / Realloc Leak (CWE-401)
 * Mục đích: Mô phỏng thói quen lập trình nguy hiểm trong C:
 *           `ptr = realloc(ptr, new_size)` trực tiếp mà không dùng con trỏ tạm.
 *           Khi realloc trả về NULL, con trỏ cũ bị ghi đè thành NULL dẫn đến mất dấu và leak vĩnh viễn.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    printf("============================================================\n");
    printf(" [SEEDED_VULNERABILITY] REALLOC LEAK DEMONSTRATION (CWE-401)\n");
    printf("============================================================\n");

    void *ptr = malloc(1024);
    printf("Cấp phát khối nhớ 1024 bytes ban đầu: %p\n", ptr);

    // LỖ HỔNG CÀI CHỦ ĐÍCH: Gán trực tiếp ptr = realloc(...) khi yêu cầu kích thước bất khả thi
    printf("Gọi ptr = realloc(ptr, SIZE_MAX) trực tiếp không qua biến tạm...\n");
    void *old_address = ptr;
    
    // SIZE_MAX chắc chắn trả về NULL trên hệ thống 64-bit
    ptr = realloc(ptr, (size_t)-1);

    if (ptr == NULL) {
        printf("[XÁC NHẬN LỖ HỔNG] realloc trả về NULL!\n");
        printf("Biến 'ptr' hiện tại là NULL. Địa chỉ khối nhớ cũ (%p) bị mất vĩnh viễn!\n", old_address);
        printf("Hậu quả: Không thể free() khối nhớ cũ -> RÒ RỈ BỘ NHỚ (Memory Leak - CWE-401).\n");
    }

    return 0;
}
