#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "models.h"
#include "inventory.h"
#include "product.h"
#include "validation.h"

#ifndef __CPROVER__
#define __CPROVER_assert(cond, msg) do { \
    if (!(cond)) { \
        fprintf(stderr, "[CBMC-005 VIOLATION] %s (line %d)\n", msg, __LINE__); \
        return 1; \
    } \
} while(0)
#define __CPROVER_assume(cond) do { if (!(cond)) return 0; } while(0)
#endif

/*
 * CBMC-005: Stock Arithmetic & Conservation Safety Verification
 * Yêu cầu: REQ-4, REQ-5
 * Thuộc tính:
 *   1. import_stock không bao giờ để xảy ra tràn số nguyên có dấu 32-bit (INT_MAX).
 *   2. export_stock từ chối xuất khi quantity > stock hoặc quantity <= 0, không bao giờ để tồn kho bị âm.
 *   3. Khi thao tác bị từ chối, giá trị stock cũ được bảo toàn 100% không bị thay đổi.
 *   4. update_stock từ chối giá trị âm (< 0).
 */

int main(void) {
    Inventory inventory;
    Product p;
    int status;
    const Product *check_prod;

    status = inventory_init(&inventory);
    __CPROVER_assert(status == STATUS_SUCCESS, "Inventory init must succeed");

    /* Khởi tạo sản phẩm có tồn kho ban đầu là 100 */
    memset(&p, 0, sizeof(p));
    snprintf(p.id, sizeof(p.id), "P001");
    snprintf(p.name, sizeof(p.name), "Keyboard");
    snprintf(p.category, sizeof(p.category), "IT");
    snprintf(p.unit, sizeof(p.unit), "Cai");
    p.quantity = 100;
    p.price = 50.0;
    inventory_add_product(&inventory, &p);

    /* 1. Kiểm tra export_stock: Số lượng xuất > tồn kho (101 > 100) */
    status = export_stock(&inventory, "P001", 101);
    __CPROVER_assert(status != STATUS_SUCCESS, "Export exceeding current stock MUST be rejected");
    check_prod = find_product(&inventory, "P001");
    __CPROVER_assert(check_prod->quantity == 100, "Stock MUST remain 100 after rejected export");

    /* 2. Kiểm tra export_stock: Số lượng xuất <= 0 */
    status = export_stock(&inventory, "P001", 0);
    __CPROVER_assert(status != STATUS_SUCCESS, "Export quantity <= 0 MUST be rejected");
    status = export_stock(&inventory, "P001", -5);
    __CPROVER_assert(status != STATUS_SUCCESS, "Export negative quantity MUST be rejected");
    __CPROVER_assert(check_prod->quantity == 100, "Stock unchanged after invalid export calls");

    /* 3. Xuất kho hợp lệ: 40 sản phẩm -> tồn kho còn 60 */
    status = export_stock(&inventory, "P001", 40);
    __CPROVER_assert(status == STATUS_SUCCESS, "Valid export must succeed");
    __CPROVER_assert(check_prod->quantity == 60, "Stock must decrement to exactly 60");
    __CPROVER_assert(check_prod->quantity >= 0, "Stock invariant: quantity >= 0");

    /* 4. Kiểm tra import_stock: Kích hoạt điều kiện biên tràn số INT_MAX */
    /* Hiện tại stock = 60. Thử cộng thêm INT_MAX -> 60 + INT_MAX > INT_MAX -> Phải từ chối */
    status = import_stock(&inventory, "P001", INT_MAX);
    __CPROVER_assert(status == STATUS_ERR_OVERFLOW, "Import causing signed overflow MUST return STATUS_ERR_OVERFLOW");
    __CPROVER_assert(check_prod->quantity == 60, "Stock MUST remain 60 after overflow rejection");

    /* Thử cộng thêm (INT_MAX - 59) -> 60 + (INT_MAX - 59) = INT_MAX + 1 -> Vượt INT_MAX */
    status = import_stock(&inventory, "P001", INT_MAX - 59);
    __CPROVER_assert(status == STATUS_ERR_OVERFLOW, "Import exceeding INT_MAX by 1 MUST return OVERFLOW");
    __CPROVER_assert(check_prod->quantity == 60, "Stock unchanged");

    /* 5. Nhập kho hợp lệ tới đúng sát ngưỡng INT_MAX */
    /* Cộng thêm (INT_MAX - 60) -> 60 + INT_MAX - 60 = INT_MAX */
    status = import_stock(&inventory, "P001", INT_MAX - 60);
    __CPROVER_assert(status == STATUS_SUCCESS, "Import up to exactly INT_MAX must succeed");
    __CPROVER_assert(check_prod->quantity == INT_MAX, "Stock must equal INT_MAX");

    /* Khi đang ở INT_MAX, cộng thêm 1 sản phẩm phải bị chặn ngay */
    status = import_stock(&inventory, "P001", 1);
    __CPROVER_assert(status == STATUS_ERR_OVERFLOW, "Import at INT_MAX + 1 MUST return OVERFLOW");
    __CPROVER_assert(check_prod->quantity == INT_MAX, "Stock must remain INT_MAX");

    /* 6. Kiểm tra update_stock từ chối giá trị âm */
    status = update_stock(&inventory, "P001", -10);
    __CPROVER_assert(status == STATUS_ERR_INVALID_QTY, "update_stock negative must return ERR_INVALID_QTY");
    __CPROVER_assert(check_prod->quantity == INT_MAX, "Stock untouched");

    inventory_free(&inventory);
    printf("[CBMC-005 PASS] Stock arithmetic overflow guard and conservation invariants verified\n");
    return 0;
}
