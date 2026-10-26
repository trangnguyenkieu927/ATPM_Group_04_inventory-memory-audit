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
    printf(" SC-02 SIMULATION: BUFFER BOUNDS & STRING SAFETY             \n");
    printf("============================================================\n");

    int total = 0, passed = 0;

    // TC-SC02-01: Line exactly 511 chars + '\n'
    total++;
    {
        char buffer[1024];
        memset(buffer, 'A', 480);
        buffer[480] = '\0';
        char line[1024];
        snprintf(line, sizeof(line), "SP02_01|%s|Cat|pcs|10|100.0\n", buffer);
        Product p;
        // In baseline, name validator rejects names > 99 chars, ensuring memory bounds
        int rc = parse_product_line(line, &p);
        if (rc < 0) {
            printf("[PASS] TC-SC02-01: Oversized field in line rejected safely by bounds check\n");
            passed++;
        } else {
            printf("[FAIL] TC-SC02-01: Oversized field was not rejected\n");
        }
    }

    // TC-SC02-02: String safety in validate_product_id: ID length boundary
    total++;
    {
        char valid_id[20]; // 19 chars + '\0'
        memset(valid_id, 'A', 19);
        valid_id[19] = '\0';
        int rc = validate_product_id(valid_id);
        if (rc == STATUS_SUCCESS) {
            printf("[PASS] TC-SC02-02: ID length 19 chars (N-1 boundary) accepted\n");
            passed++;
        } else {
            printf("[FAIL] TC-SC02-02: ID length 19 chars rejected unexpectedly\n");
        }
    }

    // TC-SC02-03: ID length 20 chars (overflowing MAX_ID buffer)
    total++;
    {
        char invalid_id[30];
        memset(invalid_id, 'A', 20);
        invalid_id[20] = '\0';
        int rc = validate_product_id(invalid_id);
        if (rc < 0) {
            printf("[PASS] TC-SC02-03: ID length 20 chars (N boundary) rejected safely\n");
            passed++;
        } else {
            printf("[FAIL] TC-SC02-03: ID length 20 chars accepted (buffer overflow hazard)\n");
        }
    }

    // TC-SC02-04: Name length boundary (99 chars accepted, 100 chars rejected)
    total++;
    {
        char name_99[100];
        memset(name_99, 'B', 99);
        name_99[99] = '\0';
        int rc_valid = validate_product_name(name_99);

        char name_100[110];
        memset(name_100, 'B', 100);
        name_100[100] = '\0';
        int rc_invalid = validate_product_name(name_100);

        if (rc_valid == STATUS_SUCCESS && rc_invalid < 0) {
            printf("[PASS] TC-SC02-04: Name boundary N-1 (99B) accepted, N (100B) rejected\n");
            passed++;
        } else {
            printf("[FAIL] TC-SC02-04: Name boundary check failed (valid=%d, invalid=%d)\n", rc_valid, rc_invalid);
        }
    }

    // TC-SC02-05: Embedded NUL byte handling
    total++;
    {
        char line[] = "SP02_05|Item\0Name|Cat|pcs|5|50.0\n";
        Product p;
        int rc = parse_product_line(line, &p);
        // strtok_r treats '\0' as string termination, so it sees too few fields
        if (rc < 0) {
            printf("[PASS] TC-SC02-05: Embedded NUL byte terminates stream safely without crash\n");
            passed++;
        } else {
            printf("[FAIL] TC-SC02-05: Embedded NUL byte not handled properly\n");
        }
    }

    printf("------------------------------------------------------------\n");
    printf("SC-02 RESULT: %d/%d tests PASSED (%s)\n", passed, total, passed == total ? "SUCCESS" : "FAILURE");
    printf("============================================================\n");
    return passed == total ? 0 : 1;
}
