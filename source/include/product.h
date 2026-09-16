#ifndef PRODUCT_H
#define PRODUCT_H

#include "models.h"

/**
 * Khởi tạo danh sách sản phẩm động.
 * @param list Con trỏ tới danh sách ProductList cần khởi tạo.
 * @return STATUS_SUCCESS nếu thành công, STATUS_ERR_NULL_PTR hoặc STATUS_ERR_MEMORY nếu lỗi.
 */
int product_list_init(ProductList *list);

/**
 * Giải phóng bộ nhớ động đã cấp phát cho danh sách sản phẩm.
 * @param list Con trỏ tới danh sách ProductList cần giải phóng.
 */
void product_list_free(ProductList *list);

/**
 * Thêm một sản phẩm mới vào danh sách kho.
 * @param list Con trỏ tới danh sách ProductList.
 * @param prod Con trỏ tới thông tin sản phẩm cần thêm.
 * @return STATUS_SUCCESS nếu thành công, hoặc mã lỗi STATUS_ERR_* tương ứng.
 */
int add_product(ProductList *list, const Product *prod);

/**
 * Sửa thông tin sản phẩm hiện có theo mã ID.
 * @param list Con trỏ tới danh sách ProductList.
 * @param id Mã sản phẩm cần sửa.
 * @param new_name Tên mới (truyền NULL nếu không muốn sửa tên).
 * @param new_category Loại sản phẩm mới (truyền NULL nếu không sửa loại).
 * @param new_quantity Số lượng mới (-1 nếu không thay đổi).
 * @param new_price Đơn giá mới (-1.0 nếu không thay đổi).
 * @return STATUS_SUCCESS nếu sửa thành công, hoặc mã lỗi STATUS_ERR_*.
 */
int update_product(ProductList *list, const char *id, const char *new_name, const char *new_category, int new_quantity, double new_price);

/**
 * Xóa một sản phẩm khỏi danh sách theo mã ID.
 * @param list Con trỏ tới danh sách ProductList.
 * @param id Mã sản phẩm cần xóa.
 * @return STATUS_SUCCESS nếu xóa thành công, STATUS_ERR_NOT_FOUND nếu không tìm thấy.
 */
int delete_product(ProductList *list, const char *id);

/**
 * Tìm kiếm sản phẩm chính xác theo mã ID.
 * @param list Con trỏ tới danh sách ProductList.
 * @param id Mã sản phẩm cần tìm.
 * @return Con trỏ tới sản phẩm tìm thấy trong danh sách, hoặc NULL nếu không tìm thấy.
 */
Product* find_product_by_id(const ProductList *list, const char *id);

/**
 * Tìm kiếm sản phẩm theo tên (tìm chuỗi con, không phân biệt hoa thường).
 * @param list Con trỏ tới danh sách ProductList.
 * @param keyword Từ khóa tìm kiếm.
 * @param results Mảng chứa danh sách kết quả tìm được.
 * @param max_results Kích thước tối đa của mảng results.
 * @param out_count Con trỏ nhận số lượng kết quả tìm thấy thực tế.
 * @return STATUS_SUCCESS nếu thành công, mã lỗi STATUS_ERR_* nếu thất bại.
 */
int search_products_by_name(const ProductList *list, const char *keyword, Product *results, size_t max_results, size_t *out_count);

#endif /* PRODUCT_H */
