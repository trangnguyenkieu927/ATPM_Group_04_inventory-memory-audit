#ifndef MODELS_H
#define MODELS_H

#include <stddef.h>

#define MAX_ID_LEN 20
#define MAX_NAME_LEN 100
#define INITIAL_CAPACITY 16

/* Cấu trúc sản phẩm */
typedef struct {
    char id[MAX_ID_LEN];
    char name[MAX_NAME_LEN];
    int quantity;
    double price;
} Product;

/* Bảng mã lỗi / trạng thái xử lý */
#define STATUS_SUCCESS                  0   /* Thao tác thành công, không có lỗi */
#define STATUS_ERR_NULL_PTR            -1   /* Con trỏ NULL truyền vào hàm */
#define STATUS_ERR_INVALID_ID          -2   /* Mã sản phẩm không hợp lệ (rỗng, quá dài, chứa ký tự cấm) */
#define STATUS_ERR_INVALID_QTY         -3   /* Số lượng không hợp lệ (<= 0) */
#define STATUS_ERR_NOT_FOUND           -4   /* Không tìm thấy sản phẩm trong kho */
#define STATUS_ERR_INSUFFICIENT_STOCK  -5   /* Tồn kho không đủ để xuất kho */
#define STATUS_ERR_OVERFLOW            -6   /* Nguy cơ tràn số nguyên (Integer Overflow) */
#define STATUS_ERR_FILE_IO             -7   /* Lỗi thao tác đọc/ghi tệp dữ liệu */
#define STATUS_ERR_MEMORY              -8   /* Lỗi cấp phát bộ nhớ động (malloc/realloc thất bại) */

#endif /* MODELS_H */
