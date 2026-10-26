/*
 * SEEDED_VULNERABILITY: Double Free Defect (CWE-415)
 * Mục đích: Mô phỏng hành vi giải phóng hai lần trên cùng một con trỏ heap
 *           khi thiếu cơ chế gán con trỏ về NULL sau khi free.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    printf("============================================================\n");
    printf(" [SEEDED_VULNERABILITY] DOUBLE FREE DEMONSTRATION (CWE-415) \n");
    printf("============================================================\n");

    void *ptr = malloc(208);
    printf("Cấp phát vùng nhớ heap ban đầu tại: %p\n", ptr);

    printf("Thực hiện free(ptr) lần 1...\n");
    free(ptr);
    // LỖ HỔNG CÀI CHỦ ĐÍCH: Không gán ptr = NULL!
    
    printf("Thực hiện free(ptr) lần 2 (Double Free)...");
    fflush(stdout);
    
    // Gọi free lần 2 trên cùng một địa chỉ đã giải phóng
    free(ptr);
    printf(" [CẢNH BÁO] Hệ thống không bắt được double free!\n");
    return 0;
}
