/* =========================================================================
 * MODULE 3: DATA INGESTION ARITHMETIC & STATE UPDATE VERIFICATION (v1 - DA VA DEFENSIVE GUARD)
 * Thanh vien phu trach: Ha Van Do
 * ========================================================================= */
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

/* KICH BAN SC-06 (VÁ): CWE-190 (Safe Integer Arithmetic Specification)
 * Guard: Kiem tra tien dieu kien: Neu added_quantity > INT_MAX - current_stock thi tu choi. */
int update_stock_quantity_v1(int current_stock, int added_quantity) {
    if (current_stock < 0 || added_quantity < 0) {
        return -1; /* Gia tri am khong hop le trong quan ly kho */
    }
    if (added_quantity > INT_MAX - current_stock) {
        return -2; /* Tu choi: Tran so nguyen co dau (Chan CWE-190) */
    }
    return current_stock + added_quantity;
}

/* KICH BAN PHU (VÁ): CWE-190
 * Guard: Kiem tra phep nhan bang INT_MAX / rate truoc khi nhan. */
int scale_product_price_v1(int base_price, int rate) {
    if (base_price <= 0 || rate <= 0) {
        return -1;
    }
    if (base_price > INT_MAX / rate) {
        return -2; /* Tu choi: Tran so phep nhan gia */
    }
    return base_price * rate;
}

