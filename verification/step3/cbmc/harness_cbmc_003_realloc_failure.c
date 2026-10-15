#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../../source/include/models.h"
#include "../../../source/include/validation.h"

/* Cơ chế Fault Injection để ép realloc trả NULL có kiểm soát */
static int g_fail_realloc = 0;
static void *(*real_realloc)(void *, size_t) = realloc;

void *mock_realloc(void *ptr, size_t size) {
    if (g_fail_realloc) {
        return NULL; /* Ép allocator trả về NULL để mô phỏng OOM */
    }
    return real_realloc(ptr, size);
}

#define realloc mock_realloc

/* Include trực tiếp logic product.c trong môi trường harness để kiểm chứng */
#include "../../source/src/product.c"

#undef realloc

#ifndef __CPROVER__
#define __CPROVER_assert(cond, msg) do { \
    if (!(cond)) { \
        fprintf(stderr, "[CBMC-003 VIOLATION] %s (line %d)\n", msg, __LINE__); \
        return 1; \
    } \
} while(0)
#define __CPROVER_assume(cond) do { if (!(cond)) return 0; } while(0)
#endif

/*
 * CBMC-003: Realloc Failure & Pointer Preservation Verification
 * Yêu cầu: REQ-12, RISK-004
 * Thuộc tính:
 *   1. Khi realloc() trả về NULL (OOM), hàm C trả về STATUS_ERR_MEMORY.
 *   2. Con trỏ sở hữu cũ list->items được bảo toàn nguyên vẹn (không bị ghi đè thành NULL).
 *   3. list->capacity và list->count không bị cập nhật sai lệch.
 *   4. Toàn bộ dữ liệu sản phẩm trước khi realloc vẫn hợp lệ và có thể truy cập được.
 */

int main(void) {
    ProductList list;
    int status;
    Product *saved_items_ptr;
    size_t saved_capacity;
    size_t saved_count;
    Product p;

    status = product_list_init(&list);
    __CPROVER_assert(status == STATUS_SUCCESS, "Init must succeed");
    __CPROVER_assert(list.capacity == INITIAL_CAPACITY, "Capacity starts at INITIAL_CAPACITY (16)");

    /* Nạp 16 sản phẩm để đạt đầy capacity = 16 */
    for (int i = 0; i < (int)INITIAL_CAPACITY; ++i) {
        memset(&p, 0, sizeof(p));
        snprintf(p.id, sizeof(p.id), "P%03d", i + 1);
        snprintf(p.name, sizeof(p.name), "Item_%d", i + 1);
        snprintf(p.category, sizeof(p.category), "General");
        snprintf(p.unit, sizeof(p.unit), "Cai");
        p.quantity = 10;
        p.price = 20.0;
        status = add_product(&list, &p);
        __CPROVER_assert(status == STATUS_SUCCESS, "Add 16 items must succeed");
    }

    __CPROVER_assert(list.count == INITIAL_CAPACITY, "Count must be 16");
    __CPROVER_assert(list.capacity == INITIAL_CAPACITY, "Capacity must be 16 (đầy mảng)");

    /* Lưu lại con trỏ và trạng thái trước khi mở rộng mảng */
    saved_items_ptr = list.items;
    saved_capacity = list.capacity;
    saved_count = list.count;

    /* BẬT FAULT INJECTION: Ép lần gọi realloc tiếp theo trả về NULL */
    g_fail_realloc = 1;

    memset(&p, 0, sizeof(p));
    snprintf(p.id, sizeof(p.id), "P017");
    snprintf(p.name, sizeof(p.name), "Item_17");
    snprintf(p.category, sizeof(p.category), "General");
    snprintf(p.unit, sizeof(p.unit), "Cai");
    p.quantity = 15;
    p.price = 30.0;

    /* Gọi thêm sản phẩm thứ 17 - phải kích hoạt realloc và thất bại */
    status = add_product(&list, &p);

    /* TẮT FAULT INJECTION để dọn dẹp an toàn */
    g_fail_realloc = 0;

    /* KIỂM CHỨNG BẤT BIẾN REQ-12 */
    __CPROVER_assert(status == STATUS_ERR_MEMORY, "Must return STATUS_ERR_MEMORY on realloc failure");
    __CPROVER_assert(list.items == saved_items_ptr, "Original items pointer MUST be preserved on realloc failure");
    __CPROVER_assert(list.capacity == saved_capacity, "Capacity MUST NOT change when realloc fails");
    __CPROVER_assert(list.count == saved_count, "Count MUST NOT change when realloc fails");

    /* Dữ liệu cũ còn nguyên vẹn */
    __CPROVER_assert(strcmp(list.items[0].id, "P001") == 0, "Item 0 data must be intact");
    __CPROVER_assert(strcmp(list.items[INITIAL_CAPACITY - 1].id, "P016") == 0, "Item 15 data must be intact");

    /* Dọn dẹp con trỏ cũ an toàn, không bị rò rỉ */
    product_list_free(&list);
    __CPROVER_assert(list.items == NULL, "Items must be NULL after cleanup");

    printf("[CBMC-003 PASS] Realloc failure and pointer preservation contract verified\n");
    return 0;
}
