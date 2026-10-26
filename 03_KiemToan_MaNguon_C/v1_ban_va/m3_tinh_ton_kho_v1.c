/* =========================================================================
 * MODULE 3: DATA INGESTION ARITHMETIC & STATE UPDATE VERIFICATION
 * PHIÊN BẢN V1: BẢN VÁ AN TOÀN PHÒNG THỦ (DEFENSIVE GUARDS & SAFE ARITHMETIC)
 * -------------------------------------------------------------------------
 * Thành viên phụ trách: Hà Văn Đô - MSV: 23010406 - Nhóm 04
 * Hành vi hệ thống: 
 *   - H4: Cập nhật tồn kho an toàn (update_stock_quantity)
 *   - H5: Quy đổi đơn giá & giá trị hàng hóa (scale_product_price)
 * Lỗ hổng khắc phục: 
 *   - CWE-190: Signed Integer Overflow (Tràn số nguyên có dấu 32-bit)
 *   - Business Logic Flaw: Negative Quantity Ingestion (Nhập số lượng âm)
 * Ánh xạ yêu cầu đặc tả (dacta_M3_tinh_ton_kho.json):
 *   - REQ-1: Tính toán tiến đến trạng thái hoàn tất thành công (Liveness)
 *   - REQ-2: Không rơi vào deadlock tại bất kỳ trạng thái nào
 *   - REQ-3: Bất biến an toàn AG(!violation) được thỏa mãn tuyệt đối
 *   - REQ-4: Ngăn chặn tràn số nguyên 32-bit trong phép cộng dồn tồn kho
 *   - REQ-5: Từ chối số lượng nhập hàng là số âm để bảo vệ tính toàn vẹn kho
 *   - REQ-6: Ngăn chặn tràn số nguyên trong phép nhân quy đổi giá trị hàng hóa
 * Ánh xạ trạng thái mô hình Kripke M3-v1:
 *   - m3_nhan_so_luong     : Tiếp nhận tham số đầu vào (receiving_quantity)
 *   - m3_kiem_tra_rao_chan : Kích hoạt trạm kiểm soát rào chắn (guard_checking)
 *   - m3_tu_choi_so_am     : Từ chối an toàn khi số âm (negative_rejected, safe_exit, done)
 *   - m3_tu_choi_tran_so   : Từ chối an toàn khi nguy cơ tràn số (overflow_prevented, safe_exit, done)
 *   - m3_cong_don_an_toan  : Phép cộng hoàn tất an toàn (sum_safe, done)
 *   - m3_nhan_gia_an_toan  : Phép nhân hoàn tất an toàn (mul_safe, done)
 * ========================================================================= */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

/* =========================================================================
 * ĐỊNH NGHĨA HẰNG SỐ MÃ LỖI QUY ƯỚC AN TOÀN (SAFE ERROR CODES)
 * ========================================================================= */
#define STOCK_ERR_NEGATIVE   (-1)  /* Lỗi: Giá trị âm không hợp lệ trong quản lý kho (REQ-5) */
#define STOCK_ERR_OVERFLOW   (-2)  /* Lỗi: Từ chối vì tràn số nguyên có dấu 32-bit (REQ-4, CWE-190) */

#define PRICE_ERR_INVALID    (-1)  /* Lỗi: Đơn giá hoặc hệ số tỉ giá <= 0 không hợp lệ */
#define PRICE_ERR_OVERFLOW   (-2)  /* Lỗi: Từ chối vì tràn số phép nhân giá (REQ-6, CWE-190) */

/* =========================================================================
 * HÀM 1: CẬP NHẬT TỒN KHO AN TOÀN PHÒNG THỦ (H4 - REQ-1, REQ-3, REQ-4, REQ-5)
 * =========================================================================
 * @param current_stock  : Số lượng tồn kho hiện tại (phải >= 0)
 * @param added_quantity : Số lượng hàng mới nhập vào kho (phải >= 0)
 * @return:
 *    - Giá trị tồn kho mới (>= 0) nếu tính toán an toàn
 *    - STOCK_ERR_NEGATIVE (-1)   nếu phát hiện số âm (vi phạm logic nghiệp vụ)
 *    - STOCK_ERR_OVERFLOW (-2)   nếu phát hiện nguy cơ tràn số nguyên có dấu
 * ========================================================================= */
int update_stock_quantity_v1(int current_stock, int added_quantity) {
    /* RÀO CHẮN 1 (REQ-5): Kiểm tra tính hợp lệ về dấu của dữ liệu đầu vào.
     * Chuyển trạng thái: m3_kiem_tra_rao_chan -> m3_tu_choi_so_am */
    if (current_stock < 0 || added_quantity < 0) {
        return STOCK_ERR_NEGATIVE;
    }

    /* RÀO CHẮN 2 (REQ-3, REQ-4 - CWE-190): Kiểm tra trần số học INT_MAX trước khi cộng.
     * Sử dụng bất đẳng thức chuyển vế an toàn: added_quantity > INT_MAX - current_stock.
     * Đảm bảo tuyệt đối không để CPU thực hiện phép cộng gây tràn số (Undefined Behavior).
     * Chuyển trạng thái: m3_kiem_tra_rao_chan -> m3_tu_choi_tran_so */
    if (added_quantity > INT_MAX - current_stock) {
        return STOCK_ERR_OVERFLOW;
    }

    /* TÍNH TOÁN AN TOÀN (REQ-1): Vượt qua toàn bộ rào chắn phòng thủ.
     * Chuyển trạng thái: m3_kiem_tra_rao_chan -> m3_cong_don_an_toan (sum_safe, done) */
    return current_stock + added_quantity;
}

/* =========================================================================
 * HÀM 2: QUY ĐỔI GIÁ TRỊ HÀNG HÓA AN TOÀN PHÒNG THỦ (H5 - REQ-6)
 * =========================================================================
 * @param base_price : Đơn giá cơ sở của sản phẩm (phải > 0)
 * @param rate       : Hệ số tỉ giá / số lượng quy đổi (phải > 0)
 * @return:
 *    - Tổng giá trị sản phẩm quy đổi (> 0) nếu an toàn
 *    - PRICE_ERR_INVALID (-1)  nếu đơn giá hoặc tỉ giá <= 0
 *    - PRICE_ERR_OVERFLOW (-2) nếu phép nhân vượt trần INT_MAX
 * ========================================================================= */
int scale_product_price_v1(int base_price, int rate) {
    /* RÀO CHẮN 1: Kiểm tra tính hợp lệ của giá trị tiền tệ và tỉ giá */
    if (base_price <= 0 || rate <= 0) {
        return PRICE_ERR_INVALID;
    }

    /* RÀO CHẮN 2 (REQ-6 - CWE-190): Kiểm tra tràn số phép nhân trước khi tính toán.
     * Biến đổi bất đẳng thức an toàn: base_price > INT_MAX / rate.
     * Ngăn chặn việc nhân tràn số làm lật âm giá trị hàng hóa kho. */
    if (base_price > INT_MAX / rate) {
        return PRICE_ERR_OVERFLOW;
    }

    /* TÍNH TOÁN AN TOÀN: Chuyển trạng thái m3_nhan_gia_an_toan (mul_safe, done) */
    return base_price * rate;
}

/* =========================================================================
 * CÁC HÀM WRAPPER / ALIAS TƯƠNG THÍCH ĐẶC TẢ VÀ BỘ TEST
 * =========================================================================
 * Giúp mã nguồn tương thích với cả các công cụ kiểm chứng hình thức
 * không có hậu tố _v1 và các framework kiểm thử tích hợp.
 * ========================================================================= */
int update_stock_quantity(int current_stock, int added_quantity) {
    return update_stock_quantity_v1(current_stock, added_quantity);
}

int scale_product_price(int base_price, int rate) {
    return scale_product_price_v1(base_price, rate);
}
