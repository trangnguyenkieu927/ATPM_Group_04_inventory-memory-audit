#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../../source/include/models.h"
#include "../../../source/include/product.h"

#ifndef __CPROVER__
#define __CPROVER_assert(cond, msg) do { \
    if (!(cond)) { \
        fprintf(stderr, "[CBMC-004 VIOLATION] %s (line %d)\n", msg, __LINE__); \
        return 1; \
    } \
} while(0)
#define __CPROVER_assume(cond) do { if (!(cond)) return 0; } while(0)
#endif

/*
 * CBMC-004: Null Pointer, Double-Free & Borrowed Pointer Safety
 * Yêu cầu: REQ-13, REQ-14, REQ-15
 * Thuộc tính:
 *   1. Mọi API container từ chối đối số con trỏ NULL và trả STATUS_ERR_NULL_PTR.
 *   2. Gọi product_list_free(NULL) hoặc gọi product_list_free() 2 lần liên tiếp không gây crash hoặc double-free (Idempotent cleanup).
 *   3. Phân biệt rõ ranh giới con trỏ mượn (borrowed pointer) còn hiệu lực và bị vô hiệu hóa sau realloc.
 */

int main(void) {
    ProductList list;
    Product p;
    int status;
    Product *borrowed_ptr;

    /* 1. Kiểm tra phòng vệ con trỏ NULL theo hợp đồng REQ-13 */
    status = product_list_init(NULL);
    __CPROVER_assert(status == STATUS_ERR_NULL_PTR, "init(NULL) must return ERR_NULL_PTR");

    status = add_product(NULL, &p);
    __CPROVER_assert(status == STATUS_ERR_NULL_PTR, "add(NULL, p) must return ERR_NULL_PTR");

    status = product_list_init(&list);
    __CPROVER_assert(status == STATUS_SUCCESS, "Init list must succeed");

    status = add_product(&list, NULL);
    __CPROVER_assert(status == STATUS_ERR_NULL_PTR, "add(list, NULL) must return ERR_NULL_PTR");

    status = delete_product(NULL, "P001");
    __CPROVER_assert(status == STATUS_ERR_NULL_PTR, "remove(NULL, id) must return ERR_NULL_PTR");

    status = delete_product(&list, NULL);
    __CPROVER_assert(status == STATUS_ERR_NULL_PTR, "remove(list, NULL) must return ERR_NULL_PTR");

    /* 2. Kiểm chứng phòng vệ Double-Free và Idempotent Cleanup theo REQ-15 */
    /* Trường hợp A: free(NULL) không được crash */
    product_list_free(NULL);

    /* Thêm 2 sản phẩm */
    memset(&p, 0, sizeof(p));
    snprintf(p.id, sizeof(p.id), "P001");
    snprintf(p.name, sizeof(p.name), "Item_1");
    snprintf(p.category, sizeof(p.category), "General");
    snprintf(p.unit, sizeof(p.unit), "Cai");
    p.quantity = 10;
    p.price = 15.0;
    add_product(&list, &p);

    /* Trường hợp B: Gọi free lần 1 */
    product_list_free(&list);
    __CPROVER_assert(list.items == NULL, "Items pointer must be NULL after first free");
    __CPROVER_assert(list.count == 0, "Count must be 0 after first free");
    __CPROVER_assert(list.capacity == 0, "Capacity must be 0 after first free");

    /* Trường hợp C: Gọi free lần 2 trên cùng một biến (Idempotent Defense) */
    product_list_free(&list);
    __CPROVER_assert(list.items == NULL, "Items pointer must remain NULL after second free");

    /* 3. Kiểm chứng quy tắc hợp đồng Con trỏ Mượn (Borrowed Pointer Contract) theo REQ-14 */
    status = product_list_init(&list);
    __CPROVER_assert(status == STATUS_SUCCESS, "Re-init list must succeed");

    /* Nạp 16 sản phẩm để đầy capacity = 16 */
    for (int i = 0; i < (int)INITIAL_CAPACITY; ++i) {
        memset(&p, 0, sizeof(p));
        snprintf(p.id, sizeof(p.id), "P%03d", i + 1);
        snprintf(p.name, sizeof(p.name), "Item_%d", i + 1);
        snprintf(p.category, sizeof(p.category), "General");
        snprintf(p.unit, sizeof(p.unit), "Cai");
        p.quantity = 10;
        p.price = 20.0;
        add_product(&list, &p);
    }

    /* Mượn con trỏ trỏ tới phần tử thứ nhất */
    borrowed_ptr = find_product_by_id(&list, "P001");
    __CPROVER_assert(borrowed_ptr != NULL, "Borrowed pointer must be valid before realloc");
    __CPROVER_assert(borrowed_ptr == &list.items[0], "Borrowed pointer references element 0");

    /* Thêm sản phẩm thứ 17 -> Kích hoạt realloc tăng capacity từ 16 lên 32 */
    memset(&p, 0, sizeof(p));
    snprintf(p.id, sizeof(p.id), "P017");
    snprintf(p.name, sizeof(p.name), "Item_17");
    snprintf(p.category, sizeof(p.category), "General");
    snprintf(p.unit, sizeof(p.unit), "Cai");
    p.quantity = 5;
    p.price = 25.0;
    add_product(&list, &p);

    /* Hợp đồng an toàn: Khi capacity tăng, list.items có thể đã được di dời sang địa chỉ heap mới.
     * Con trỏ mượn cũ KHÔNG được giả định là còn trỏ vào container nếu địa chỉ items thay đổi.
     * Muốn truy cập an toàn, mã C bắt buộc phải tái mượn con trỏ (re-borrowing) qua find_product_by_id() */
    Product *re_borrowed = find_product_by_id(&list, "P001");
    __CPROVER_assert(re_borrowed != NULL, "Re-borrowed pointer must be valid");
    __CPROVER_assert(re_borrowed == &list.items[0], "Re-borrowed pointer matches new items base");
    __CPROVER_assert(strcmp(re_borrowed->name, "Item_1") == 0, "Data is correct");

    product_list_free(&list);
    printf("[CBMC-004 PASS] Null safety, double-free defense and borrowed pointer lifecycle verified\n");
    return 0;
}
