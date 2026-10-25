# BÁO CÁO PHÂN TÍCH MÃ NGUỒN TĨNH (STATIC CODE ANALYSIS REPORT)

## 1. CÔNG CỤ VÀ CẤU HÌNH PHÂN TÍCH
- **Công cụ chính thức:** `Cppcheck 2.21.0`.
- **Cấu hình thực thi:**
  ```bash
  cppcheck --enable=all --inconclusive --std=c17 \
           --xml --xml-version=2 --suppress=missingIncludeSystem \
           -I include src/
  ```
- **Công cụ Clang Static Analyzer:** `TOOL_UNAVAILABLE`.
  - *Giải thích nguyên nhân:* Môi trường thực nghiệm Windows MSYS2 hiện tại chưa cài đặt bộ công cụ `clang` và script `scan-build`. Nhóm ghi nhận trung thực trạng thái này theo Mục V Quy chế kiểm chứng và không giả lập log kết quả của Clang.

## 2. TỔNG HỢP PHÁT HIỆN CPPCHECK
Cppcheck đã kiểm tra toàn bộ 7 file mã nguồn C trong `source/src/`:
- **Tổng số cảnh báo phát hiện:** 31 cảnh báo.
- **Phân loại theo mức độ nghiêm trọng:**
  - `error`: 0 phát hiện (Không có lỗi cú pháp hoặc vi phạm bộ nhớ hiển nhiên phát hiện bởi rule tĩnh cơ bản).
  - `style`: 12 phát hiện (Chủ yếu liên quan đến khai báo con trỏ `const`, hàm nội bộ có thể đặt `static`, và thứ tự kiểm tra mảng).
  - `information`: 2 phát hiện (Giới hạn nhánh phân tích mặc định).

## 3. CÁC PHÁT HIỆN ĐÁNG CHÚ Ý CẦN RÀ SOÁT KỸ
1. **`STATIC-CPP-001` (`src/file_io.c:26:82`) - `compareValueOutOfTypeRangeError`:**
   - *Nội dung:* `Comparing expression of type 'signed long' against value 2147483647. Condition is always false.`
   - *Phân tích kỹ thuật:* Trong Windows x86_64 Data Model (LLP64), kiểu `long` có kích thước 32-bit (`LONG_MAX = 2147483647 = INT_MAX`). Do đó biến `parsed` (trả về từ `strtol`) khi ép kiểu hoặc so sánh với `INT_MAX` sẽ không bao giờ lớn hơn `INT_MAX`. Đây là vấn đề tương thích nền tảng (portability) khi chuyển giữa LP64 (Linux) và LLP64 (Windows).
2. **`STATIC-CPP-003` & `004` (`src/product.c:55 & 61`) - `arrayIndexThenCheck`:**
   - *Nội dung:* `Array index 'i' is used before limits check.` trong vòng lặp `for (; haystack[i] != '\0' && i < sizeof(h_lower) - 1; ++i)`.
   - *Phân tích kỹ thuật:* Mặc dù `haystack[i]` được duyệt tuần tự, việc kiểm tra `haystack[i] != '\0'` trước điều kiện biên `i < sizeof(h_lower) - 1` là một rủi ro style/logic tiềm ẩn nếu con trỏ chuỗi không có null-terminator trước kích thước đệm.

## 4. GIỚI HẠN CỦA PHÂN TÍCH TĨNH VÀ KẾT NỐI VỚI BƯỚC TIẾP THEO
Cppcheck hoàn toàn bỏ sót các rủi ro cấu trúc bộ nhớ heap:
- Không cảnh báo việc thiếu guard tràn số ở `capacity * 2` và `new_cap * sizeof(Product)`.
- Không phát hiện được nguy cơ dangling pointer khi borrowed pointer trỏ vào mảng động sau khi realloc.
Điều này khẳng định sự cần thiết của **Công đoạn 3 (Xây dựng kịch bản rủi ro)** và **Công đoạn 4 & 5 (Đặc tả an toàn & Kiểm chứng hình thức SMT/BMC/CTL)**.
