# KẾ HOẠCH MÔ PHỎNG RỦI RO AN TOÀN BỘ NHỚ BẰNG C TEST HARNESS (BƯỚC 2)

**Dự án:** Kiểm chứng an toàn bộ nhớ cho chương trình C mô phỏng quản lý kho
**Nhánh Git:** `audit/step2-risk-specification`
**Commit Baseline:** `44202070c68002d9af068374478cdc83098dd4d8`
**Kỹ sư phụ trách:** Nguyễn Quang Thọ (Toolchain & Testing) & Hà Văn Đô (Specification)

---

## 1. NGUYÊN TẮC THIẾT KẾ MÔ PHỎNG RỦI RO

Theo nhận xét chỉ đạo của Giảng viên:
> *"Xây dựng code C mô phỏng lại rủi ro. Xây dựng đặc tả cho bài toán rủi ro an toàn."*

1. **Bảo vệ Baseline v0:** Toàn bộ test harness mô phỏng sẽ được tạo độc lập tại thư mục `tests/simulation/` trong Bước 3, tuyệt đối không chỉnh sửa mã nguồn nghiệp vụ `source/src/`.
2. **Không merge nhánh lỗi cài chủ đích:** Nhánh `audit/v0-file-io-vulnerable` chỉ được sử dụng làm mẫu đối chứng lỗi (*Vulnerable Benchmark*) trong quá trình đánh giá độ nhạy của công cụ phân tích động ở Bước 3.
3. **Phân biệt rạch ròi giữa Kiểm chứng Số học và Bằng chứng Runtime:** Đo đạc chính xác mã lỗi trả về (`STATUS_ERR_*`), tín hiệu hệ thống, hoặc báo cáo AddressSanitizer/UBSan.

---

## 2. BÀI TOÁN MÔ PHỎNG ƯU TIÊN 1 (SIM-001): HEAP REALLOCATION SIZE & POINTER LIFETIME

### 2.1. Định danh và Phạm vi
- **Mã Rủi ro:** `RISK-001`, `RISK-002`, `RISK-003`, `RISK-004`.
- **Yêu cầu liên quan:** `REQ-2`, `REQ-3`, `REQ-12`, `REQ-14`.
- **Tập tin Nguồn:** `source/src/product.c` (`add_product` dòng 116-138, `find_product_by_id` dòng 223-233).

### 2.2. Kịch bản Mô phỏng A – Cận biên Tràn Kích thước Cấp phát
- **Trạng thái kích hoạt:** Cố tình khởi tạo `ProductList` với `capacity = 0x4000000000000000` ($2^62$) trong test harness cô lập.
- **Hành vi kích hoạt:** Gọi `add_product` để ép chương trình tính toán `new_cap = 2^63` và `new_cap * 208`.
- **Kỳ vọng:** Đo đạc việc `realloc` trả về `NULL` (OOM) hoặc mã lỗi `STATUS_ERR_MEMORY`. Chứng minh cơ chế guard con trỏ tạm `new_items` tại dòng 119-122 bảo toàn vùng nhớ gốc thành công (REQ-12).

### 2.3. Kịch bản Mô phỏng B – Con trỏ Mượn Bị Mất Hiệu lực (Pointer Invalidation)
- **Trạng thái kích hoạt:** Lấy con trỏ phần tử `Product *p = find_product_by_id(list, "SP001")`.
- **Hành vi kích hoạt:** Gọi `add_product` liên tiếp để ép `realloc` di chuyển khối nhớ heap sang địa chỉ mới; sau đó truy cập đọc/ghi qua `p->quantity`.
- **Kỳ vọng:** AddressSanitizer phát hiện lỗi `heap-use-after-free` (CWE-416) trên test harness mô phỏng hợp đồng kiến trúc này.

---

## 3. BÀI TOÁN MÔ PHỎNG ƯU TIÊN 2 (SIM-002): FILE PARSER BOUNDS & MALFORMED INPUTS

### 3.1. Định danh và Phạm vi
- **Mã Rủi ro:** `RISK-007`, `RISK-008`, `RISK-009`.
- **Yêu cầu liên quan:** `REQ-9`, `REQ-10`, `REQ-16`, `REQ-17`, `REQ-18`.
- **Tập tin Nguồn:** `source/src/file_io.c` (`load_products_from_file` dòng 94-135, `parse_product_line` dòng 46-92).
- **Cơ chế thực tế trong mã nguồn:** Sử dụng `strchr(cursor, '|')` duyệt và cắt chuỗi trực tiếp; bộ đệm dòng `char line[512]`; mảng token cố định `char *tokens[8]`.

### 3.2. Bộ Dữ liệu Thử nghiệm Mô phỏng (Test Corpus)
1. `corpus_valid_6col.txt`: Dòng chuẩn 6 trường phân cách bằng `|`. Kỳ vọng: Nạp thành công.
2. `corpus_legacy_4col.txt`: Dòng cũ 4 trường. Kỳ vọng: Gán mặc định danh mục và đơn vị tính (REQ-17).
3. `corpus_excess_tokens.txt`: Dòng chứa > 10 dấu `|`. Kỳ vọng: Guard `token_count < 8` chặn việc ghi tràn stack mảng `tokens[8]` (REQ-16).
4. `corpus_oversized_line.txt`: Dòng dài 1.024 bytes không có newline `\n`. Kỳ vọng: Guard `strchr(line, '\n') == NULL` từ chối an toàn và trả `STATUS_ERR_FILE_IO` (REQ-10).
5. `corpus_embedded_nul.txt`: Dòng chứa byte `\0` ở giữa trường tên. Kỳ vọng: Chuỗi bị ngắt an toàn, không đọc vượt biên bộ nhớ (REQ-9).
6. `corpus_numeric_malformed.txt`: Dòng chứa `quantity = 9999999999999999` hoặc `price = NaN`. Kỳ vọng: `parse_int` và `parse_price` từ chối an toàn (REQ-18).

---

## 4. CẤU HÌNH THỰC NGHIỆM VÀ CÔNG CỤ CHO BƯỚC 3

- **Trình biên dịch:** GCC với `-fsanitize=address,undefined -g`.
- **Môi trường kiểm thử:** MSYS2 / UCRT64 trên Windows 11.
- **Đầu ra cần thu thập:** Báo cáo crash dump ASan, exit code, và log kiểm tra rò rỉ LeakSanitizer.
