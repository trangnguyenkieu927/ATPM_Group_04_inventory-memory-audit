#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>
#include "models.h"

/**
 * Module: Các hàm tiện ích dùng chung (Common Utilities)
 * Thiết kế an toàn bộ nhớ (Memory Safety) chống tràn bộ đệm và lỗi con trỏ.
 */

// 1. Kiểm tra chuỗi rỗng hoặc chỉ chứa khoảng trắng
int is_empty_or_whitespace(const char *str);

// 2. Cắt bỏ khoảng trắng ở đầu và cuối chuỗi (In-place trim)
char* trim_whitespace(char *str);

// 3. Sao chép chuỗi an toàn tuyệt đối chống tràn bộ đệm (Buffer Overflow)
// Luôn đảm bảo có ký tự kết thúc chuỗi '\0'
int safe_string_copy(char *dest, size_t dest_size, const char *src);

// 4. Đọc một dòng văn bản an toàn từ bàn phím (stdin)
// Tự động xóa '\n', '\r' và dọn sạch bộ đệm nhập nếu dòng quá dài
void safe_read_line(const char *prompt, char *buffer, size_t size);

// 5. Tách một dòng văn bản thành các token theo ký tự phân cách (CSV/Pipe)
int split_line(char *line, char delimiter, char *tokens[], int max_tokens);

// 6. Chuyển chuỗi sang số nguyên an toàn (kiểm tra định dạng và chống tràn số)
int safe_str_to_int(const char *str, int *out_val);

// 7. Chuyển chuỗi sang số thực double an toàn
int safe_str_to_double(const char *str, double *out_val);

// 8. Chuyển đổi chữ hoa / chữ thường in-place
void to_upper_case(char *str);
void to_lower_case(char *str);

// 9. Tìm đường dẫn tệp dữ liệu linh hoạt (cho phép chạy từ thư mục gốc hoặc source/)
const char* find_file_path(const char *filename);

#endif /* UTILS_H */
