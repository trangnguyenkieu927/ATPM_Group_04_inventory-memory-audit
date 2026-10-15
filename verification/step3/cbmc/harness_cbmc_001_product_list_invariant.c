#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "models.h"
#include "product.h"

#ifndef __CPROVER__
#define __CPROVER_assert(cond, msg) do { \
    if (!(cond)) { \
        fprintf(stderr, "[CBMC-001 VIOLATION] %s (line %d)\n", msg, __LINE__); \
        return 1; \
    } \
} while(0)
#define __CPROVER_assume(cond) do { if (!(cond)) return 0; } while(0)
#endif

/*
 * CBMC-001: ProductList Invariant Verification
 * Yêu cầu: REQ-1, REQ-2
 * Thuộc tính:
 *   1. count <= capacity tại mọi thời điểm.
 *   2. Nếu capacity > 0 thì items != NULL.
 *   3. Sau product_list_init: count == 0, capacity == INITIAL_CAPACITY (16), items != NULL.
 *   4. Sau thêm lặp N sản phẩm (unwind bound k=20, vượt qua INITIAL_CAPACITY để kích hoạt realloc), bất biến luôn duy trì.
 */

int main(void) {
    ProductList list;
    int status;
    size_t i;
    const size_t UNWIND_BOUND = 20;

    /* 1. Kiểm chứng trạng thái khởi tạo */
    status = product_list_init(&list);
    __CPROVER_assert(status == STATUS_SUCCESS, "Init must return SUCCESS");
    __CPROVER_assert(list.count == 0, "Initial count must be 0");
    __CPROVER_assert(list.capacity == INITIAL_CAPACITY, "Initial capacity must be INITIAL_CAPACITY");
    __CPROVER_assert(list.items != NULL, "Initial items must not be NULL");
    __CPROVER_assert(list.count <= list.capacity, "Invariant: count <= capacity after init");

    /* 2. Thêm lặp k=20 sản phẩm (kích hoạt realloc khi vượt 16) */
    for (i = 0; i < UNWIND_BOUND; ++i) {
        Product p;
        memset(&p, 0, sizeof(p));
        snprintf(p.id, sizeof(p.id), "P%03zu", i + 1);
        snprintf(p.name, sizeof(p.name), "Product_%zu", i + 1);
        snprintf(p.category, sizeof(p.category), "Electronics");
        snprintf(p.unit, sizeof(p.unit), "Cai");
        p.quantity = (int)(i * 10 + 5);
        p.price = 100.0 + (double)i;

        status = add_product(&list, &p);
        __CPROVER_assert(status == STATUS_SUCCESS, "Add must return SUCCESS");
        __CPROVER_assert(list.count <= list.capacity, "Invariant: count <= capacity after add");
        __CPROVER_assert(list.items != NULL, "Invariant: items must not be NULL when capacity > 0");
        __CPROVER_assert(list.count == i + 1, "Count must increment by 1");
    }

    /* Kiểm tra dung lượng đã mở rộng lên 32 khi count = 20 */
    __CPROVER_assert(list.capacity == 32, "Capacity must be expanded to 32 after adding 20 items");
    __CPROVER_assert(strcmp(list.items[0].id, "P001") == 0, "First element id preserved");
    __CPROVER_assert(strcmp(list.items[UNWIND_BOUND - 1].id, "P020") == 0, "Last element id preserved");

    /* 3. Xóa phần tử và kiểm tra duy trì bất biến */
    status = delete_product(&list, "P005");
    __CPROVER_assert(status == STATUS_SUCCESS, "Delete existing must return SUCCESS");
    __CPROVER_assert(list.count == UNWIND_BOUND - 1, "Count must decrement by 1");
    __CPROVER_assert(list.count <= list.capacity, "Invariant: count <= capacity after delete");

    /* 4. Dọn dẹp tài nguyên */
    product_list_free(&list);
    __CPROVER_assert(list.count == 0, "Count must be 0 after free");
    __CPROVER_assert(list.capacity == 0, "Capacity must be 0 after free");
    __CPROVER_assert(list.items == NULL, "Items must be NULL after free");

    printf("[CBMC-001 PASS] ProductList invariant verified within bound k=%zu\n", UNWIND_BOUND);
    return 0;
}
