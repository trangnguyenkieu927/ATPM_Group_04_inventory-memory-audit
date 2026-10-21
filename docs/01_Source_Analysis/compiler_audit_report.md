# BÁO CÁO PHÂN TÍCH BIÊN DỊCH VÀ CẢNH BÁO (COMPILER AUDIT REPORT)

## 1. MÔI TRƯỜNG BIÊN DỊCH THỰC NGHIỆM
- **Trình biên dịch:** GCC 16.1.0 (`Rev5, Built by MSYS2 project`).
- **Mục tiêu phần cứng:** `x86_64-w64-mingw32`.
- **Hệ số chuẩn C:** ISO C17 (`-std=c17`).
- **Mã SHA Baseline v0 kiểm tra:** `44202070c68002d9af068374478cdc83098dd4d8`.
- **Thư mục mã nguồn:** `source/src/*.c`, `source/include/*.h`.

## 2. TỔ HỢP CỜ BIÊN DỊCH BẮT BUỘC
Để thực hiện kiểm tra an toàn nghiêm ngặt ở mức compile-time theo yêu cầu của Giảng viên, nhóm đã cấu hình tập cờ cảnh báo tối đa:
```bash
gcc -std=c17 -Wall -Wextra -Wpedantic \
    -Wconversion -Wsign-conversion -Wshadow \
    -Wformat=2 -Wnull-dereference \
    -I./include -c src/*.c
```

## 3. KẾT QUẢ THỰC TẾ
- **Exit Code:** `0` (SUCCESS).
- **Số lượng Error:** `0`.
- **Số lượng Warning:** `0`.
- **Chi tiết stdout:** Rỗng (không có cảnh báo).
- **Chi tiết stderr:** Rỗng (không có cảnh báo).

## 4. KẾT QUẢ UNIT TESTS VÀ SMOKE TESTS
- **Unit Tests:** `inventory_tests.exe` hoàn thành với thông báo `inventory tests passed` (Exit code: 0).
- **Smoke Tests:** `smoke_tests.exe` thực thi toàn diện 17/17 ca kiểm thử luồng nền tảng (khởi tạo, thêm, chặn trùng ID, tìm kiếm, nhập kho, chặn tràn số `INT_MAX`, xuất kho, đọc/ghi tệp 6 trường và 4 trường, xóa sản phẩm, giải phóng bộ nhớ). Kết quả: **17/17 PASS (100%)**.

## 5. NHẬN ĐỊNH VÀ BÀI HỌC AN TOÀN
> [!IMPORTANT]
> **Nguyên tắc kiểm chứng trung thực:** *Compiler không có warning KHÔNG CÓ NGHĨA là chương trình tuyệt đối an toàn bộ nhớ.*
> Mặc dù Baseline v0 được thiết kế cẩn thận và đạt 0 warning ngay cả với `-Wconversion` và `-Wsign-conversion`, các rủi ro tiềm ẩn cấp thiết kế vẫn tồn tại:
> 1. Thiếu guard kiểm tra tràn số khi nhân đôi `capacity * 2` tại `product.c:116` (REQ-2).
> 2. Thiếu guard kiểm tra tràn số khi tính `new_cap * sizeof(Product)` tại `product.c:120` (REQ-3).
> 3. Con trỏ mượn (borrowed pointer) từ `find_product_by_id` có thể bị biến thành dangling pointer nếu sau đó xảy ra `realloc` (REQ-14).
> Do đó, bắt buộc phải sử dụng các phương pháp phân tích sâu hơn: Phân tích tĩnh (Static Analysis), Kiểm chứng hình thức (Formal Verification) và Mô phỏng C (C Simulation).
