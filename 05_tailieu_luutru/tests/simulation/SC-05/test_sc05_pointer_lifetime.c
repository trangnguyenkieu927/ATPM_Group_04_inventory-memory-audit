#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "models.h"
#include "product.h"

int main(void) {
    printf("============================================================\n");
    printf(" SC-05 SIMULATION: POINTER LIFETIME & BORROWED REFERENCES   \n");
    printf("============================================================\n");

    int total = 0, passed = 0;

    ProductList list;
    product_list_init(&list);

    Product p1;
    strcpy(p1.id, "TARGET01");
    strcpy(p1.name, "Original Target Item");
    strcpy(p1.category, "Category");
    strcpy(p1.unit, "pcs");
    p1.quantity = 10;
    p1.price = 100.0;
    add_product(&list, &p1);

    // TC-SC05-01: Borrowed pointer valid for immediate read
    total++;
    Product *borrowed = find_product_by_id(&list, "TARGET01");
    if (borrowed != NULL && strcmp(borrowed->name, "Original Target Item") == 0) {
        printf("[PASS] TC-SC05-01: Borrowed pointer retrieved and valid for immediate read (%p)\n", (void*)borrowed);
        passed++;
    } else {
        printf("[FAIL] TC-SC05-01: Borrowed pointer retrieval failed\n");
    }

    // Lưu lại địa chỉ cũ trước realloc
    Product *old_ptr_addr = borrowed;
    void *old_items_base = (void*)list.items;

    // TC-SC05-02: Trigger realloc by adding items exceeding capacity
    total++;
    size_t target_add = list.capacity + 5;
    for (size_t i = 0; i < target_add; ++i) {
        Product extra;
        snprintf(extra.id, sizeof(extra.id), "EXTRA_%04zu", i);
        snprintf(extra.name, sizeof(extra.name), "Extra Item %zu", i);
        strcpy(extra.category, "Category");
        strcpy(extra.unit, "pcs");
        extra.quantity = 1;
        extra.price = 1.0;
        add_product(&list, &extra);
    }

    void *new_items_base = (void*)list.items;
    printf("Heap base address: Old = %p -> New = %p\n", old_items_base, new_items_base);

    // Kể cả khi allocator giữ nguyên hay di chuyển địa chỉ, quy ước an toàn là:
    // Con trỏ mượn cũ KHÔNG ĐƯỢC tin cậy nữa. Phải tra cứu lại con trỏ mới.
    Product *refreshed_ptr = find_product_by_id(&list, "TARGET01");
    if (refreshed_ptr != NULL && strcmp(refreshed_ptr->name, "Original Target Item") == 0) {
        printf("[PASS] TC-SC05-02: Refreshed lookup after realloc succeeded at current address (%p)\n", (void*)refreshed_ptr);
        passed++;
    } else {
        printf("[FAIL] TC-SC05-02: Refreshed lookup failed\n");
    }

    // TC-SC05-03: NULL pointer defenses on lookup API
    total++;
    Product *null_search1 = find_product_by_id(NULL, "TARGET01");
    Product *null_search2 = find_product_by_id(&list, NULL);
    if (null_search1 == NULL && null_search2 == NULL) {
        printf("[PASS] TC-SC05-03: NULL container or NULL search key safely returned NULL without crash\n");
        passed++;
    } else {
        printf("[FAIL] TC-SC05-03: NULL defenses failed on lookup API\n");
    }

    product_list_free(&list);

    printf("------------------------------------------------------------\n");
    printf("SC-05 RESULT: %d/%d tests PASSED (%s)\n", passed, total, passed == total ? "SUCCESS" : "FAILURE");
    printf("============================================================\n");
    return passed == total ? 0 : 1;
}
