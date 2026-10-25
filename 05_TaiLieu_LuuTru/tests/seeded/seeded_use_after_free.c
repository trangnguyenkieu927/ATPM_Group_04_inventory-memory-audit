/*
 * SEEDED_VULNERABILITY: Use-After-Free (CWE-416)
 * Mục đích: Mô phỏng hành vi truy cập con trỏ mượn cũ sau khi bộ nhớ heap đã bị realloc di chuyển.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char id[20];
    char name[100];
    int quantity;
} SimpleProduct;

int main(void) {
    printf("============================================================\n");
    printf(" [SEEDED_VULNERABILITY] USE-AFTER-FREE DEMO (CWE-416)       \n");
    printf("============================================================\n");

    size_t cap = 2;
    SimpleProduct *items = (SimpleProduct *)malloc(cap * sizeof(SimpleProduct));
    strcpy(items[0].id, "P01");
    strcpy(items[0].name, "Item 1");
    items[0].quantity = 10;

    // Con trỏ mượn trỏ trực tiếp vào phần tử 0
    SimpleProduct *borrowed_ptr = &items[0];
    printf("Con trỏ mượn trỏ tới: %p (Tên: %s)\n", (void*)borrowed_ptr, borrowed_ptr->name);

    // LỖ HỔNG CÀI CHỦ ĐÍCH: Mở rộng bộ đệm bằng realloc làm thay đổi địa chỉ
    // Ép cấp phát kích thước lớn để allocator phải chuyển sang block mới
    SimpleProduct *new_items = (SimpleProduct *)realloc(items, 100000 * sizeof(SimpleProduct));
    printf("Địa chỉ mảng mới sau realloc: %p (Khối nhớ cũ đã bị free)\n", (void*)new_items);

    if (new_items != items) {
        printf("[NGUY HIỂM] Khối nhớ cũ đã bị giải phóng! Con trỏ borrowed_ptr (%p) là Dangling Pointer!\n", (void*)borrowed_ptr);
        printf("Đọc từ con trỏ mượn đã bị vô hiệu hóa (Use-After-Free read)...\n");
        // Đây là hành vi vi phạm UAF
        printf("Giá trị đọc: ID = %s\n", borrowed_ptr->id);
    } else {
        printf("[THÔNG BÁO] Allocator mở rộng tại chỗ (in-place expansion).\n");
    }

    free(new_items);
    return 0;
}
