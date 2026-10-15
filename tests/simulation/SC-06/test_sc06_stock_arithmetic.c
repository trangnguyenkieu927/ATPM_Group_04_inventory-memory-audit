#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include "models.h"
#include "product.h"
#include "inventory.h"

int main(void) {
    printf("============================================================\n");
    printf(" SC-06 SIMULATION: INVENTORY STOCK ARITHMETIC & CONSERVATION\n");
    printf("============================================================\n");

    int total = 0, passed = 0;

    Inventory inv;
    inventory_init(&inv);

    Product p;
    strcpy(p.id, "STOCK01");
    strcpy(p.name, "Stock Test Item");
    strcpy(p.category, "Category");
    strcpy(p.unit, "pcs");
    p.quantity = 100;
    p.price = 50.0;
    inventory_add_product(&inv, &p);

    // TC-SC06-01: Negative import quantity rejected
    total++;
    int rc = import_stock(&inv, "STOCK01", -10);
    if (rc < 0) {
        printf("[PASS] TC-SC06-01: Negative import quantity rejected (rc=%d)\n", rc);
        passed++;
    } else {
        printf("[FAIL] TC-SC06-01: Negative import quantity accepted unexpectedly\n");
    }

    // TC-SC06-02: Normal import success
    total++;
    rc = import_stock(&inv, "STOCK01", 50);
    const Product *cur = find_product(&inv, "STOCK01");
    if (rc == STATUS_SUCCESS && cur != NULL && cur->quantity == 150) {
        printf("[PASS] TC-SC06-02: Normal import succeeded (100 + 50 = %d)\n", cur->quantity);
        passed++;
    } else {
        printf("[FAIL] TC-SC06-02: Normal import failed\n");
    }

    // TC-SC06-03: Integer overflow prevention (quantity near INT_MAX)
    total++;
    // cur->quantity = 150. Delta = INT_MAX - 100. Sum = INT_MAX + 50 (OVERFLOW!)
    int delta_overflow = INT_MAX - 100;
    rc = import_stock(&inv, "STOCK01", delta_overflow);
    if (rc == STATUS_ERR_OVERFLOW) {
        printf("[PASS] TC-SC06-03: Integer overflow blocked by guard STATUS_ERR_OVERFLOW (%d)\n", rc);
        passed++;
    } else {
        printf("[FAIL] TC-SC06-03: Integer overflow not blocked (rc=%d)\n", rc);
    }

    // TC-SC06-04: Conservation invariant check after rejected import
    total++;
    cur = find_product(&inv, "STOCK01");
    if (cur != NULL && cur->quantity == 150) {
        printf("[PASS] TC-SC06-04: Conservation Invariant: Stock remained exactly 150 after rejected overflow\n");
        passed++;
    } else {
        printf("[FAIL] TC-SC06-04: Conservation Invariant violated\n");
    }

    // TC-SC06-05: Export exceeding current stock
    total++;
    rc = export_stock(&inv, "STOCK01", 200); // 200 > 150
    if (rc == STATUS_ERR_INSUFFICIENT_STOCK) {
        printf("[PASS] TC-SC06-05: Excessive export blocked by STATUS_ERR_INSUFFICIENT_STOCK (%d)\n", rc);
        passed++;
    } else {
        printf("[FAIL] TC-SC06-05: Excessive export not blocked (rc=%d)\n", rc);
    }

    // TC-SC06-06: Conservation invariant check after rejected export
    total++;
    cur = find_product(&inv, "STOCK01");
    if (cur != NULL && cur->quantity == 150) {
        printf("[PASS] TC-SC06-06: Conservation Invariant: Stock remained exactly 150 after rejected export\n");
        passed++;
    } else {
        printf("[FAIL] TC-SC06-06: Conservation Invariant violated\n");
    }

    // TC-SC06-07: Valid export
    total++;
    rc = export_stock(&inv, "STOCK01", 50);
    cur = find_product(&inv, "STOCK01");
    if (rc == STATUS_SUCCESS && cur != NULL && cur->quantity == 100) {
        printf("[PASS] TC-SC06-07: Valid export succeeded (150 - 50 = %d)\n", cur->quantity);
        passed++;
    } else {
        printf("[FAIL] TC-SC06-07: Valid export failed\n");
    }

    inventory_free(&inv);

    printf("------------------------------------------------------------\n");
    printf("SC-06 RESULT: %d/%d tests PASSED (%s)\n", passed, total, passed == total ? "SUCCESS" : "FAILURE");
    printf("============================================================\n");
    return passed == total ? 0 : 1;
}
