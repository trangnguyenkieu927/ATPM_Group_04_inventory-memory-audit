/* =========================================================================
 * MODULE 3: DATA INGESTION ARITHMETIC & STATE UPDATE VERIFICATION
 * PHIÊN BẢN V1: BẢN VÁ AN TOÀN PHÒNG THỦ (DEFENSIVE GUARDS & SAFE ARITHMETIC)
 * -------------------------------------------------------------------------
 * Thành viên phụ trách: Hà Văn Đô - MSV: 23010406 - Nhóm 04
 * ========================================================================= */
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define STOCK_ERR_NEGATIVE   (-1)
#define STOCK_ERR_OVERFLOW   (-2)
#define PRICE_ERR_INVALID    (-1)
#define PRICE_ERR_OVERFLOW   (-2)

int update_stock_quantity_v1(int current_stock, int added_quantity) {
    if (current_stock < 0 || added_quantity < 0) {
        return STOCK_ERR_NEGATIVE;
    }
    if (added_quantity > INT_MAX - current_stock) {
        return STOCK_ERR_OVERFLOW;
    }
    return current_stock + added_quantity;
}

int scale_product_price_v1(int base_price, int rate) {
    if (base_price <= 0 || rate <= 0) {
        return PRICE_ERR_INVALID;
    }
    if (base_price > INT_MAX / rate) {
        return PRICE_ERR_OVERFLOW;
    }
    return base_price * rate;
}

int update_stock_quantity(int current_stock, int added_quantity) {
    return update_stock_quantity_v1(current_stock, added_quantity);
}

int scale_product_price(int base_price, int rate) {
    return scale_product_price_v1(base_price, rate);
}
