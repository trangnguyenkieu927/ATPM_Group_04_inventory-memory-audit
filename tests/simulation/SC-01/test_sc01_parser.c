#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "models.h"
#include "product.h"
#include "validation.h"
#include "../../../source/src/file_io.c"

int main(void) {
    printf("============================================================\n");
    printf(" SC-01 SIMULATION: PARSER INPUT VALIDATION & COMPATIBILITY  \n");
    printf("============================================================\n");

    int total = 0, passed = 0;

    // TC-SC01-01: Valid 6 fields
    total++;
    {
        Product p;
        char line[] = "SP01|Dell XPS 15|Laptop|Chiec|10|1500.50\n";
        trim_line_end(line);
        int rc = parse_product_line(line, &p);
        if (rc == STATUS_SUCCESS && strcmp(p.id, "SP01") == 0 && p.quantity == 10 && p.price > 1500.0) {
            printf("[PASS] TC-SC01-01: Valid 6 fields correctly parsed\n");
            passed++;
        } else {
            printf("[FAIL] TC-SC01-01: Valid 6 fields parsing failed (rc=%d)\n", rc);
        }
    }

    // TC-SC01-02: Valid 4 fields legacy format
    total++;
    {
        Product p;
        char line[] = "SP02|Ban phim co|25|45.00\n";
        trim_line_end(line);
        int rc = parse_product_line(line, &p);
        if (rc == STATUS_SUCCESS && strcmp(p.id, "SP02") == 0 && strcmp(p.category, "ChuaPhanLoai") == 0 && strcmp(p.unit, "Cai") == 0) {
            printf("[PASS] TC-SC01-02: Legacy 4 fields parsed with default category/unit\n");
            passed++;
        } else {
            printf("[FAIL] TC-SC01-02: Legacy 4 fields parsing failed (rc=%d, cat=%s, unit=%s)\n", rc, p.category, p.unit);
        }
    }

    // TC-SC01-03: Missing fields (< 4)
    total++;
    {
        Product p;
        char line[] = "SP03|Chuot quang\n";
        int rc = parse_product_line(line, &p);
        if (rc < 0) {
            printf("[PASS] TC-SC01-03: Missing fields rejected safely (rc=%d)\n", rc);
            passed++;
        } else {
            printf("[FAIL] TC-SC01-03: Missing fields accepted unexpectedly\n");
        }
    }

    // TC-SC01-04: Excess tokens (> 7)
    total++;
    {
        Product p;
        char line[] = "SP04|Tai nghe|Am thanh|Chiec|10|50.0|Extra1|Extra2|Extra3\n";
        int rc = parse_product_line(line, &p);
        if (rc < 0) {
            printf("[PASS] TC-SC01-04: Excess tokens rejected safely (rc=%d)\n", rc);
            passed++;
        } else {
            printf("[FAIL] TC-SC01-04: Excess tokens accepted unexpectedly\n");
        }
    }

    // TC-SC01-05: Empty field
    total++;
    {
        Product p;
        char line[] = "SP05||Laptop|Chiec|10|50.0\n";
        int rc = parse_product_line(line, &p);
        if (rc < 0) {
            printf("[PASS] TC-SC01-05: Empty name field rejected safely (rc=%d)\n", rc);
            passed++;
        } else {
            printf("[FAIL] TC-SC01-05: Empty name field accepted unexpectedly\n");
        }
    }

    // TC-SC01-06: Malformed numeric values
    total++;
    {
        Product p;
        char line[] = "SP06|Man hinh|Dien tu|Chiec|not_a_number|100.0\n";
        int rc = parse_product_line(line, &p);
        if (rc < 0) {
            printf("[PASS] TC-SC01-06: Malformed integer quantity rejected safely (rc=%d)\n", rc);
            passed++;
        } else {
            printf("[FAIL] TC-SC01-06: Malformed quantity accepted unexpectedly\n");
        }
    }

    // TC-SC01-07: Duplicate ID prevention in ProductList
    total++;
    {
        ProductList list;
        product_list_init(&list);
        Product p1 = {.quantity = 5, .price = 100.0};
        strcpy(p1.id, "DUP01"); strcpy(p1.name, "Item 1"); strcpy(p1.category, "Cat"); strcpy(p1.unit, "pcs");
        add_product(&list, &p1);

        Product p2 = p1; // duplicate ID
        int rc = add_product(&list, &p2);
        if (rc == STATUS_ERR_DUPLICATE_ID) {
            printf("[PASS] TC-SC01-07: Duplicate ID correctly rejected (STATUS_ERR_DUPLICATE_ID)\n");
            passed++;
        } else {
            printf("[FAIL] TC-SC01-07: Duplicate ID not rejected properly (rc=%d)\n", rc);
        }
        product_list_free(&list);
    }

    printf("------------------------------------------------------------\n");
    printf("SC-01 RESULT: %d/%d tests PASSED (%s)\n", passed, total, passed == total ? "SUCCESS" : "FAILURE");
    printf("============================================================\n");
    return passed == total ? 0 : 1;
}
