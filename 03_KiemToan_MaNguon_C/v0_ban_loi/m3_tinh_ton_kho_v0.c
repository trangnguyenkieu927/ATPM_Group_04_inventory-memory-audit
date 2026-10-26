/* =========================================================================
 * MODULE 3: DATA INGESTION ARITHMETIC & STATE UPDATE VERIFICATION (v0 - CO LOI)
 * Thanh vien phu trach: Ha Van Do - MSV: 23010406
 * Ma C sinh tu dac ta: 'quanly_khohang_m3_tinh_ton_kho'
 * ========================================================================= */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

/* sinh từ đặc tả 'quanly_khohang_m3_tinh_ton_kho', hàm 'update_stock_quantity', kind='arithmetic' */
/* KICH BAN SC-06: CWE-190 (Signed Integer Overflow khi cong don ton kho) */
int update_stock_quantity(int current_stock, int added_quantity) {
    return current_stock + added_quantity;
}

/* sinh từ đặc tả 'quanly_khohang_m3_tinh_ton_kho', hàm 'scale_product_price', kind='arithmetic' */
/* KICH BAN PHU: CWE-190 (Arithmetic Overflow khi quy doi gia san pham) */
int scale_product_price(int base_price, int rate) {
    return base_price + rate;
}

/* Alias tuong thich cho bo test kien truc v0 */
int update_stock_quantity_v0(int current_stock, int added_quantity) {
    return update_stock_quantity(current_stock, added_quantity);
}

int scale_product_price_v0(int base_price, int rate) {
    return scale_product_price(base_price, rate);
}
