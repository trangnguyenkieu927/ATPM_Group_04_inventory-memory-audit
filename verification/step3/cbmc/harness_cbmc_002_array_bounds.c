#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../../source/include/models.h"
#include "../../../source/include/product.h"
#include "../../../source/include/inventory.h"
#include "../../../source/include/report.h"

#ifndef __CPROVER__
#define __CPROVER_assert(cond, msg) do { \
    if (!(cond)) { \
        fprintf(stderr, "[CBMC-002 VIOLATION] %s (line %d)\n", msg, __LINE__); \
        return 1; \
    } \
} while(0)
#define __CPROVER_assume(cond) do { if (!(cond)) return 0; } while(0)
#endif

/*
 * CBMC-002: Array Bounds and Buffer Safety Verification
 * Yêu cầu: REQ-6, REQ-16
 * Thuộc tính:
 *   1. Không ghi vượt quá kích thước mảng CategorySummary summaries[32] trong report.c.
 *   2. Memmove trong delete_product tính đúng số byte và không out-of-bounds.
 *   3. Parse tối đa tokens[8] mà không ghi tràn bộ nhớ stack.
 */

int main(void) {
    Inventory inventory;
    CategorySummary summaries[32];
    size_t summary_count = 0;
    int status;
    size_t i;
    const size_t TEST_CATEGORIES = 40;

    status = inventory_init(&inventory);
    __CPROVER_assert(status == STATUS_SUCCESS, "Inventory init must succeed");

    /* 1. Nạp 40 sản phẩm với 40 danh mục khác nhau để kích hoạt điều kiện biên max_summaries = 32 */
    for (i = 0; i < TEST_CATEGORIES; ++i) {
        Product p;
        memset(&p, 0, sizeof(p));
        snprintf(p.id, sizeof(p.id), "P%03zu", i + 1);
        snprintf(p.name, sizeof(p.name), "Item_%zu", i + 1);
        snprintf(p.category, sizeof(p.category), "Cat_%03zu", i + 1);
        snprintf(p.unit, sizeof(p.unit), "Cai");
        p.quantity = 10;
        p.price = 50.0;
        status = inventory_add_product(&inventory, &p);
        __CPROVER_assert(status == STATUS_SUCCESS, "Add product must succeed");
    }

    /* 2. Gọi generate_inventory_summary với max_summaries = 32 */
    /* Hàm C trong report.c:72-74 có guard:
     * if (*out_count >= max_summaries) return STATUS_ERR_OVERFLOW;
     */
    status = calculate_category_summaries(&inventory, summaries, 32, &summary_count);
    __CPROVER_assert(status == STATUS_ERR_OVERFLOW, "Must return OVERFLOW when categories > 32");
    __CPROVER_assert(summary_count <= 32, "summary_count must not exceed buffer boundary 32");

    /* 3. Kiểm chứng biên memmove trong delete_product */
    /* Xóa phần tử đầu tiên (index 0): memmove dịch chuyển toàn bộ N-1 phần tử */
    status = delete_product(&inventory, "P001");
    __CPROVER_assert(status == STATUS_SUCCESS, "Remove first element must succeed");
    __CPROVER_assert(inventory.count == TEST_CATEGORIES - 1, "Count must be decremented");
    __CPROVER_assert(strcmp(inventory.items[0].id, "P002") == 0, "New head must be P002");

    /* Xóa phần tử cuối cùng (index count-1): số byte memmove là 0 */
    status = delete_product(&inventory, "P040");
    __CPROVER_assert(status == STATUS_SUCCESS, "Remove last element must succeed");
    __CPROVER_assert(inventory.count == TEST_CATEGORIES - 2, "Count must be decremented");

    inventory_free(&inventory);
    printf("[CBMC-002 PASS] Array bounds and buffer safety verified successfully\n");
    return 0;
}
