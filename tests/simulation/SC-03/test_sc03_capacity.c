#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "models.h"
#include "product.h"

int main(void) {
    printf("============================================================\n");
    printf(" SC-03 SIMULATION: HEAP ALLOCATION & CAPACITY DOUBLING      \n");
    printf("============================================================\n");

    int total = 0, passed = 0;

    ProductList list;
    int rc = product_list_init(&list);
    assert(rc == STATUS_SUCCESS);

    // TC-SC03-01: Initial capacity inspection
    total++;
    printf("Initial list state: count = %zu, capacity = %zu, items = %p\n", list.count, list.capacity, (void*)list.items);
    if (list.count == 0 && list.capacity == INITIAL_CAPACITY && list.items != NULL) {
        printf("[PASS] TC-SC03-01: Initial capacity correctly initialized (cap = %zu)\n", list.capacity);
        passed++;
    } else {
        printf("[FAIL] TC-SC03-01: Initial capacity unexpected\n");
    }

    // TC-SC03-02: Fill up to capacity without resizing
    total++;
    size_t initial_cap = list.capacity;
    int fill_ok = 1;
    for (size_t i = 0; i < initial_cap; ++i) {
        Product p;
        snprintf(p.id, sizeof(p.id), "P%04zu", i);
        snprintf(p.name, sizeof(p.name), "Item %zu", i);
        strcpy(p.category, "Category");
        strcpy(p.unit, "pcs");
        p.quantity = (int)(i + 1);
        p.price = 10.0 + (double)i;

        if (add_product(&list, &p) != STATUS_SUCCESS) fill_ok = 0;
        if (list.count > list.capacity) fill_ok = 0;
    }

    if (fill_ok && list.count == initial_cap && list.capacity == initial_cap) {
        printf("[PASS] TC-SC03-02: Filled to capacity (%zu items) with count == capacity\n", initial_cap);
        passed++;
    } else {
        printf("[FAIL] TC-SC03-02: Fill to capacity failed\n");
    }

    // TC-SC03-03: First resize triggered by adding (initial_cap + 1)-th item
    total++;
    {
        Product p;
        snprintf(p.id, sizeof(p.id), "P%04zu", initial_cap);
        strcpy(p.name, "Overflow Item");
        strcpy(p.category, "Category");
        strcpy(p.unit, "pcs");
        p.quantity = 100;
        p.price = 99.0;

        rc = add_product(&list, &p);
        if (rc == STATUS_SUCCESS && list.capacity == initial_cap * 2 && list.count == initial_cap + 1) {
            printf("[PASS] TC-SC03-03: First capacity doubling triggered: %zu -> %zu (count=%zu)\n",
                   initial_cap, list.capacity, list.count);
            passed++;
        } else {
            printf("[FAIL] TC-SC03-03: First capacity doubling failed (cap=%zu, count=%zu)\n", list.capacity, list.count);
        }
    }

    // TC-SC03-04: Multiple resizes (add up to 45 items)
    total++;
    size_t prev_cap = list.capacity;
    int multi_ok = 1;
    for (size_t i = list.count; i < 45; ++i) {
        Product p;
        snprintf(p.id, sizeof(p.id), "P%04zu", i);
        snprintf(p.name, sizeof(p.name), "Item %zu", i);
        strcpy(p.category, "Category");
        strcpy(p.unit, "pcs");
        p.quantity = 1;
        p.price = 5.0;

        if (add_product(&list, &p) != STATUS_SUCCESS) multi_ok = 0;
        if (list.count > list.capacity) multi_ok = 0;
    }

    if (multi_ok && list.count == 45 && list.capacity >= 45) {
        printf("[PASS] TC-SC03-04: Multiple capacity doublings successful: final cap=%zu, count=%zu\n",
               list.capacity, list.count);
        passed++;
    } else {
        printf("[FAIL] TC-SC03-04: Multiple resizes failed\n");
    }

    // TC-SC03-05: Invariant verification: count <= capacity && items != NULL
    total++;
    if (list.count <= list.capacity && list.items != NULL) {
        printf("[PASS] TC-SC03-05: Safety Invariant count (%zu) <= capacity (%zu) maintained\n",
               list.count, list.capacity);
        passed++;
    } else {
        printf("[FAIL] TC-SC03-05: Safety Invariant violated\n");
    }

    product_list_free(&list);

    printf("------------------------------------------------------------\n");
    printf("SC-03 RESULT: %d/%d tests PASSED (%s)\n", passed, total, passed == total ? "SUCCESS" : "FAILURE");
    printf("============================================================\n");
    return passed == total ? 0 : 1;
}
