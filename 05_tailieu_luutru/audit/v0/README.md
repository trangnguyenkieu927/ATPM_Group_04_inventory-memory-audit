# Báo cáo Nghiệm thu Baseline v0 (Pre-Audit Baseline)

## 1. Mục tiêu Phiên bản v0
Phiên bản **v0** là bản mã nguồn cơ sở (Baseline) của chương trình C quản lý kho, được chuẩn hóa và đóng băng làm đối tượng nghiên cứu cho các công tác kiểm chứng an toàn bộ nhớ (C Memory Safety Audit & Verification).

Mục tiêu cụ thể của Bước 1:
1. Đảm bảo chương trình có một kiến trúc dữ liệu và API hoàn toàn thống nhất.
2. Đảm bảo khả năng biên dịch thành công từ bản checkout sạch (Clean Checkout) với chuẩn C17 và cờ cảnh báo nghiêm ngặt (`-Wall -Wextra -Wpedantic`).
3. Khắc phục các lỗi cấu hình build/link trong Makefile để toàn bộ các bộ kiểm thử nền tảng có thể thực thi tự động.
4. Đầy đủ tài liệu kỹ thuật về kiến trúc, quyền sở hữu bộ nhớ (ownership) và bề mặt tấn công (attack surface).
5. Thu thập log thực nghiệm thật và hướng dẫn tái lập minh bạch.

---

## 2. Phạm vi Mã nguồn v0

Toàn bộ mã nguồn thuộc phạm vi v0 nằm trong thư mục `source/`:
- **Headers (`source/include/`)**:
  - `models.h`: Mô hình dữ liệu `Product`, `ProductList`, `Inventory` và mã lỗi `STATUS_*`.
  - `inventory.h`: Facade nghiệp vụ kho hàng.
  - `product.h`: Quản lý danh sách động `ProductList` trên heap.
  - `file_io.h`: Đọc/ghi tệp dữ liệu kho.
  - `validation.h`: Xác thực dữ liệu đầu vào và kiểm tra an toàn số học.
  - `utils.h`: Tiện ích chuỗi và chuyển đổi kiểu an toàn.
  - `report.h`: Báo cáo thống kê và xuất dữ liệu tổng hợp.
- **Sources (`source/src/`)**:
  - `inventory.c`, `product.c`, `file_io.c`, `validation.c`, `utils.c`, `report.c`, `main.c`.
- **Tests (`source/tests/` và `tests/`)**:
  - `source/tests/test_inventory.c`: Kiểm thử tích hợp luồng nghiệp vụ kho và lưu tệp.
  - `source/tests/smoke_test.c`: Bộ smoke test tự động kiểm tra 17 ca nghiệp vụ và an toàn bộ nhớ.
  - `tests/test_utils.c`: Kiểm thử 25 ca tiện ích chuỗi, chống tràn bộ đệm và chuyển đổi số.

---

## 3. Kiến trúc Dữ liệu Thống nhất

Hệ thống thống nhất sử dụng cấu trúc `ProductList` làm container duy nhất:
```c
typedef struct {
    Product *items;
    size_t count;
    size_t capacity;
} ProductList;

typedef ProductList Inventory;
```
- **Xác nhận không xung đột**: Mọi module đều sử dụng con trỏ `items`. Nhánh `feature/audit-do` trước đây tự ý sửa thành `products` dẫn tới gãy tương thích API với `file_io.c`, `main.c`, `test_inventory.c`. Trong v0, cấu trúc `items` được khẳng định là chuẩn kiến trúc duy nhất.

---

## 4. Phân biệt Baseline Gốc và Nhánh Lỗi Cài có Chủ đích

Dự án có sự phân định nghiêm ngặt giữa các nhánh Git:
- **`audit/prepare-v0` (Baseline gốc)**:
  - Là phiên bản chương trình nghiệp vụ C sạch, có đầy đủ các biện pháp phòng vệ hợp lý ban đầu (`safe_strcpy`, `snprintf`, `realloc` guard, kiểm tra NULL, kiểm tra tràn số).
  - Đây là đối tượng baseline để kiểm chứng các đặc tả đúng đắn và đánh giá tỷ lệ False Positive của công cụ.
- **`audit/v0-file-io-vulnerable` (Nhánh cài lỗi chủ đích)**:
  - Nhánh này cố tình giữ lại/cài cắm các lỗ hổng bộ nhớ nghiêm trọng trong `source/src/file_io.c` (đọc không kiểm tra độ dài, thiếu kiểm tra con trỏ, nguy cơ tràn bộ đệm) nhằm phục vụ kiểm thử độ nhạy của các công cụ kiểm toán (Cppcheck, ASan, UBSan, Valgrind, CBMC).
  - **Quy tắc**: Tuyệt đối KHÔNG merge nhánh `v0-file-io-vulnerable` vào baseline `v0`.

---

## 5. Môi trường Yêu cầu

- **Hệ điều hành**: Windows 10/11 (MSYS2 / UCRT64 / MinGW-w64) hoặc Linux (Ubuntu 20.04+).
- **Trình biên dịch**: GCC 10+ (Đã nghiệm thu trên `gcc (Rev5, Built by MSYS2 project) 16.1.0`).
- **Trình điều khiển build**: GNU Make 4.0+ (Đã nghiệm thu trên `GNU Make 4.4.1`).
- **Tiêu chuẩn C**: C17 (`-std=c17`).

---

## 6. Hướng dẫn Biên dịch và Chạy Kiểm thử (Build & Execution Guide)

Tất cả các thao tác đều được thực hiện từ thư mục `source/`:

### 6.1. Dọn dẹp bản build cũ
```powershell
make clean
```

### 6.2. Biên dịch ứng dụng chuẩn
```powershell
make
```
Lệnh này tạo ra tệp thực thi `inventory_app` (hoặc `inventory_app.exe` trên Windows) với các cờ chuẩn:
`-std=c17 -Wall -Wextra -Wpedantic -I./include`

### 6.3. Biên dịch chế độ Debug (phục vụ GDB / CBMC)
```powershell
make debug
```
Lệnh này bật cờ `-g -O0`.

### 6.4. Chạy kiểm thử tích hợp (Integration Test)
```powershell
make test
```
Đầu ra mong đợi: `inventory tests passed` (Exit code: 0).

### 6.5. Chạy Smoke Test toàn diện (17 test cases)
```powershell
make smoke-test
```
Đầu ra mong đợi: Bảng kết quả 17/17 ca PASS (Exit code: 0).

### 6.6. Chạy các chế độ CLI của ứng dụng
```powershell
# Chạy bộ unit tests tích hợp trong main.c
./inventory_app --test

# Hiển thị bảng danh sách sản phẩm hiện có
./inventory_app --list

# Kiểm toán dữ liệu tệp products.txt
./inventory_app --validate

# Tạo báo cáo tổng quan và cơ cấu ngành hàng
./inventory_app --report
```

---

## 7. Bằng chứng Thực nghiệm (Evidence List)

Toàn bộ log thực nghiệm thực tế được lưu trữ tại `results/baseline/`:
- [environment.txt](file:///c:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/results/baseline/environment.txt): Thông tin chi tiết phiên bản OS, CPU, GCC, Make và Git SHA.
- [build.log](file:///c:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/results/baseline/build.log): Nhật ký biên dịch sạch `make clean`, `make` và `make debug` không phát sinh warning nào.
- [build-exit-code.txt](file:///c:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/results/baseline/build-exit-code.txt): Mã thoát 0 của tiến trình build.
- [test.log](file:///c:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/results/baseline/test.log): Nhật ký chạy `make test` và `inventory_app --test`.
- [smoke-test.log](file:///c:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/results/baseline/smoke-test.log): Bảng chi tiết kết quả 17 ca smoke test chức năng nền tảng.
- [reproduction.log](file:///c:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/results/baseline/reproduction.log): Nhật ký tái lập từ môi trường checkout độc lập.

---

## 8. Các Hạn chế và Nguy cơ Bộ nhớ Đã biết trong v0 (Audit Targets)

Mặc dù baseline v0 được thiết kế phòng thủ cơ bản, các hạn chế kiến trúc sau được ghi nhận để kiểm chứng ở các bước tiếp theo:
1. **Con trỏ mượn bị vô hiệu hóa khi realloc**: Các con trỏ trả về từ `find_product()` không được theo dõi vòng đời; nếu hàm `add_product()` gây di dời heap qua `realloc`, các con trỏ này biến thành Dangling Pointer (CWE-416).
2. **Thiếu kiểm tra tràn số phép nhân trong realloc**: Biểu thức `new_cap * sizeof(Product)` chưa có cơ chế so sánh với `SIZE_MAX / sizeof(Product)`.
3. **Giới hạn số danh mục trong báo cáo**: Mảng `CategorySummary summaries[32]` trong `calculate_category_summaries()` có thể tràn nếu kho có trên 32 danh mục.
4. **Giới hạn bộ đệm tệp tin**: Dòng tệp vượt quá 512 bytes bị từ chối đọc nhưng cần kiểm chứng hành vi với các công cụ Fuzzing.

---

## 9. Trạng thái Nghiệm thu Bước 1
- **Kiến trúc thống nhất**: ĐẠT (100% khớp `ProductList.items`).
- **Khả năng biên dịch**: ĐẠT (C17, 0 warnings với `-Wall -Wextra -Wpedantic`).
- **Khả năng kiểm thử**: ĐẠT (Tất cả unit test và smoke test đều thực thi thành công, exit code 0).
- **Tài liệu kỹ thuật**: ĐẠT (Đã hoàn thiện tài liệu kiến trúc, ownership và attack surface).
- **Tính tái lập**: ĐẠT (Đã kiểm chứng độc lập).
