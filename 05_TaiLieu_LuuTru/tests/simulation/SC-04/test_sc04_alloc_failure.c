#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "models.h"

/* Biến cờ điều khiển fault injection */
static int g_inject_realloc_failure = 0;
static void *(*g_real_realloc)(void *, size_t) = realloc;

void *sc04_mock_realloc(void *ptr, size_t size) {
    if (g_inject_realloc_failure) {
        return NULL; /* Ép realloc trả về NULL */
    }
    return g_real_realloc(ptr, size);
}

#define realloc sc04_mock_realloc
#include "../../../source/src/product.c"
#undef realloc

int main(void) {
    printf("============================================================\n");
    printf(" SC-04 SIMULATION: ALLOCATION FAILURE & MEMORY LEAK DEFENSE \n");
    printf("============================================================\n");

    int total = 0, passed = 0;

    ProductList list;
    product_list_init(&list);

    // Điền dữ liệu tới đầy capacity ban đầu
    size_t init_cap = list.capacity;
    for (size_t i = 0; i < init_cap; ++i) {
        Product p;
        snprintf(p.id, sizeof(p.id), "P%04zu", i);
        snprintf(p.name, sizeof(p.name), "Item %zu", i);
        strcpy(p.category, "Category");
        strcpy(p.unit, "pcs");
        p.quantity = 10;
        p.price = 50.0;
        add_product(&list, &p);
    }

    Product *old_ptr = list.items;
    size_t old_cap = list.capacity;
    size_t old_count = list.count;

    // TC-SC04-01: Realloc failure fault injection (OOM)
    total++;
    g_inject_realloc_failure = 1;

    Product extra_prod;
    strcpy(extra_prod.id, "P_OOM");
    strcpy(extra_prod.name, "OOM Item");
    strcpy(extra_prod.category, "Category");
    strcpy(extra_prod.unit, "pcs");
    extra_prod.quantity = 1;
    extra_prod.price = 10.0;

    int rc = add_product(&list, &extra_prod);
    g_inject_realloc_failure = 0; // Tắt fault injection

    if (rc == STATUS_ERR_MEMORY) {
        printf("[PASS] TC-SC04-01: Realloc failure returned STATUS_ERR_MEMORY (%d)\n", rc);
        passed++;
    } else {
        printf("[FAIL] TC-SC04-01: Realloc failure did not return STATUS_ERR_MEMORY (rc=%d)\n", rc);
    }

    // TC-SC04-02: Old memory retention (Zero Memory Leak)
    total++;
    if (list.items == old_ptr && list.capacity == old_cap && list.count == old_count) {
        printf("[PASS] TC-SC04-02: Existing heap items pointer (%p) and count (%zu) preserved (NO LEAK)\n",
               (void*)list.items, list.count);
        passed++;
    } else {
        printf("[FAIL] TC-SC04-02: Existing heap items pointer lost or corrupted on realloc failure\n");
    }

    // TC-SC04-03: Normal cleanup after failure
    total++;
    product_list_free(&list);
    if (list.items == NULL && list.count == 0 && list.capacity == 0) {
        printf("[PASS] TC-SC04-03: Cleanup after failure zeroed list.items, count, capacity\n");
        passed++;
    } else {
        printf("[FAIL] TC-SC04-03: Cleanup after failure did not zero pointers\n");
    }

    // TC-SC04-04: Double-free defense (idempotent cleanup)
    total++;
    product_list_free(&list); // Gọi free lần thứ hai
    product_list_free(NULL);  // Gọi free với NULL pointer
    printf("[PASS] TC-SC04-04: Second free call and free(NULL) completed safely without crash\n");
    passed++;

    printf("------------------------------------------------------------\n");
    printf("SC-04 RESULT: %d/%d tests PASSED (%s)\n", passed, total, passed == total ? "SUCCESS" : "FAILURE");
    printf("============================================================\n");
    return passed == total ? 0 : 1;
}
