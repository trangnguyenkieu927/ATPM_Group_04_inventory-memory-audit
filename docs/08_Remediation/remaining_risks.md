# BÁO CÁO RỦI RO CÒN LẠI VÀ PHÂN TÍCH AN TOÀN (REMAINING RISKS)

## 1. RÀ SOÁT CÁC RỦI RO CÒN LẠI SAU BẢN VÁ V1
Sau khi áp dụng bản vá phòng vệ v1 cho `source/src/product.c`:
1. **Rủi ro Tràn số Mở rộng Dung lượng (REQ-2 & REQ-3):** **ĐÃ ĐƯỢC LOẠI BỎ TRIỆT ĐỂ (RESOLVED)** nhờ guard trước phép nhân.
2. **Rủi ro Rò rỉ Bộ nhớ khi Cấp phát Thất bại (REQ-12):** **ĐÃ ĐƯỢC BẢO VỆ (GUARDED)** nhờ con trỏ tạm `new_items`.
3. **Rủi ro Double-Free (REQ-13):** **ĐÃ ĐƯỢC BẢO VỆ (GUARDED)** nhờ gán `NULL` sau giải phóng.
4. **Rủi ro Tràn số Nhập kho (REQ-4):** **ĐÃ ĐƯỢC BẢO VỆ (GUARDED)** trong mã nguồn gốc.
5. **Rủi ro Tràn Bộ đệm Parser (REQ-16):** **ĐÃ ĐƯỢC BẢO VỆ (GUARDED)** nhờ `token_count < 8` và kiểm tra độ dài.

## 2. CÁC ĐIỀU KIỆN TIỀN GIẢ ĐỊNH VÀ GIỚI HẠN VẬN HÀNH
- **Hợp đồng Con trỏ Mượn (REQ-14):** Con trỏ trả về từ `find_product_by_id` chỉ có giá trị đọc tức thời (borrowed reference). Ứng dụng gọi API không được lưu trữ con trỏ này qua các thao tác thêm mới sản phẩm làm kích hoạt `realloc`. Đây là quy ước API chuẩn mực trong lập trình C (tương tự như iterator invalidation trong C++).
- **Phần cứng và Bộ nhớ Vật lý:** Các bài kiểm tra mô phỏng đã kiểm chứng trong phạm vi bộ nhớ vật lý thực tế. Khi hệ điều hành cạn kiệt RAM hoàn toàn (OOM), phần mềm trả về `STATUS_ERR_MEMORY` an toàn mà không bị crash hay mất dữ liệu đã có.
