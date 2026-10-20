/* =========================================================================
 * MODULE 3: DATA INGESTION ARITHMETIC & STATE UPDATE VERIFICATION (v0 - CO LOI)
 * Thanh vien phu trach: Ha Van Do
 * Muc tieu: Kiem toan tinh toan so hoc ton kho, gia tien va Chung minh SMT Z3.
 * Cac loi chu dich:
 *   - update_stock_quantity_v0 : Cong don bi tran so nguyen co dau (CWE-190)
 *   - scale_product_price_v0   : Nhan gia tien voi ti gia bi tran so nguyen (CWE-190)
 * ========================================================================= */
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

/* KICH BAN SC-06: CWE-190 (Signed Integer Overflow khi cap nhat ton kho)
 * Giai doan RAM: Cong don so luong nhap moi vao ton kho hien tai.
 * Root cause: Thuc hien phep cong truc tiep ma khong kiem tra vuot nguong INT_MAX. */
int update_stock_quantity_v0(int current_stock, int added_quantity) {
    /* Khi current_stock gan INT_MAX va added_quantity duong,
     * tong se bi overflow thanh so am (UB theo chuan C99). */
    return current_stock + added_quantity;
}

/* KICH BAN PHU: CWE-190 (Arithmetic Overflow khi quy doi gia san pham)
 * Giai doan RAM: Nhan gia tien co ban voi he so nhan ti gia.
 * Root cause: Khong kiem tra phep nhan so nguyen. */
static int helper_scale_price(int price, int rate) {
    return price * rate;
}

int scale_product_price_v0(int base_price, int rate) {
    return helper_scale_price(base_price, rate);
}
