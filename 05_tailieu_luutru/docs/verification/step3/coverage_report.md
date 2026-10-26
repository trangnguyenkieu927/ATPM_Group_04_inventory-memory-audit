# BÁO CÁO ĐO LƯỜNG ĐỘ PHỦ MÃ NGUỒN VÀ ĐỘ PHỦ NHÁNH (CODE & BRANCH COVERAGE)

**Học phần:** An toàn phần mềm (CSE703093)  
**Đề tài:** Kiểm chứng an toàn bộ nhớ cho chương trình C mô phỏng nghiệp vụ quản lý kho  
**Phụ trách chính:** Nguyễn Quang Thọ – Kỹ sư Công cụ & Thực nghiệm C  
**Công cụ đo lường:** GCC 16.1.0 (`--coverage -O0 -g`), Gcov 16.1.0, Gcovr 8.6  
**Thư mục báo cáo dữ liệu:** `results/step3/coverage/`  
**Ngày thực nghiệm:** 15/10/2026  

---

## 1. Phương pháp đo lường và cấu hình biên dịch

Độ phủ mã nguồn động được đo lường bằng cách kích hoạt trình đo độ phủ chính thức của GCC thông qua các cờ:
```bash
gcc --coverage -O0 -g -Isource/include ...
```
- Mức tối ưu `-O0` được bắt buộc sử dụng để ngăn chặn trình biên dịch tự động gộp lệnh, loại bỏ nhánh rẽ hoặc inline hàm, bảo đảm mọi nhánh logic điều kiện (`if`, `while`, `for`) được ánh xạ 1-1 với mã máy.
- Tập kiểm thử chạy đo độ phủ bao gồm toàn bộ các kịch bản thực thi hợp lệ:
  1. 17 ca kiểm thử nền tảng (Smoke Test Suite - `source/tests/smoke_test.c`).
  2. Bộ kiểm thử đơn vị kho hàng (Unit Tests - `source/tests/test_inventory.c`).
  3. Mô phỏng vòng đời bộ nhớ heap và tái cấp phát (SIM-001 - `tests/simulation/test_sim001_heap.c`).
  4. Mô phỏng phân tích cú pháp tệp với 10 corpus biên (SIM-002 - `tests/simulation/test_sim002_parser.c`).
  5. Bộ kiểm tra rò rỉ bộ nhớ và canary (Memory Audit - `verification/step3/sanitizers/test_memory_audit.c`).
  6. Các harness kiểm chứng bất biến bộ nhớ (CBMC-001, CBMC-002, CBMC-004, CBMC-005).
- Các phép thử cố ý làm crash tiến trình (như tiến trình con trong SIM-003 kích hoạt Stack Smashing) được cách ly độc lập để không làm gián đoạn việc flush file `.gcda`.

---

## 2. Bảng tổng hợp độ phủ dòng (Line Coverage) và độ phủ nhánh (Branch Coverage)

| Module nguồn C | Tổng số dòng | Số dòng đã chạy | Line Coverage (%) | Tổng số nhánh | Số nhánh đã rẽ | Branch Coverage (%) | Chức năng chính |
| :--- | :---: | :---: | :---: | :---: | :---: | :---: | :--- |
| `source/src/file_io.c` | 100 | 87 | **87.0%** | 92 | 64 | **69.6%** | Đọc/ghi tệp, phân tích dòng CSV |
| `source/src/inventory.c` | 43 | 33 | **76.7%** | 24 | 13 | **54.2%** | Nghiệp vụ nhập, xuất, cập nhật kho |
| `source/src/product.c` | 135 | 66 | **48.9%** | 121 | 45 | **37.2%** | Quản lý mảng động ProductList, realloc |
| `source/src/validation.c`| 165 | 55 | **33.3%** | 156 | 51 | **32.7%** | Kiểm tra hợp lệ chuỗi, số lượng, giá |
| `source/src/report.c` | 204 | 24 | **11.8%** | 112 | 15 | **13.4%** | Thống kê danh mục, in ấn báo cáo |
| `source/src/utils.c` | 123 | 15 | **12.2%** | 110 | 10 | **9.1%** | Tiện ích chuỗi, thời gian, console |
| **TỔNG HỢP TOÀN DỰ ÁN** | **770** | **280** | **36.4%** | **615** | **198** | **32.2%** | **6 module nghiệp vụ cốt lõi** |

*(Ghi chú: `source/src/main.c` chứa hàm `main()` tương tác console CLI với người dùng qua bàn phím, được phân tách khỏi mã thư viện nghiệp vụ để đo lường độc lập).*

---

## 3. Phân tích chi tiết các dòng và nhánh chưa được phủ (Uncovered Analysis)

Tuân thủ nguyên tắc trung thực khoa học, nhóm phân tích nguyên nhân kỹ thuật của các vùng mã nguồn chưa được thực thi:

### 3.1. Các nhánh xử lý lỗi cấp phát bộ nhớ (`malloc`/`realloc` thất bại)
- **Vị trí tiêu biểu:**
  - `product.c:15-18`: Nhánh `if (list->items == NULL)` trong `product_list_init`.
  - `product.c:120`: Nhánh `if (new_items == NULL)` trong `add_product`.
- **Nguyên nhân:** Trong môi trường máy phát triển có đủ RAM (16GB - 64GB), hệ điều hành Windows luôn phân bổ bộ nhớ thành công trong luồng bình thường. Do đó, các nhánh xử lý OOM trong mã C nguyên bản không thể được kích hoạt tự nhiên bởi unit test thông thường.
- **Biện pháp đối chứng:** Nhánh lỗi này đã được chứng minh an toàn độc lập thông qua mô phỏng Fault Injection trong SIM-001 (Thí nghiệm 2) và kiểm chứng hình thức CBMC-003.

### 3.2. Các hàm định dạng in ấn báo cáo (`report.c`)
- **Vị trí:** `print_inventory_summary_report`, `print_low_stock_report`, `print_out_of_stock_report`, `print_category_summary_report` (dòng 58-78).
- **Nguyên nhân:** Đây là các hàm thuần túy thực hiện `printf` định dạng bảng ra màn hình console người dùng, không tác động trực tiếp đến an toàn bộ nhớ heap. Bộ kiểm thử tự động ưu tiên đo lường hàm tính toán cốt lõi `calculate_category_summaries` (đạt kiểm chứng REQ-6, REQ-16).

### 3.3. Các hàm tiện ích phụ trợ (`utils.c`)
- **Vị trí:** Các hàm xử lý thời gian, đọc input an toàn từ bàn phím `read_line_from_stdin`.
- **Nguyên nhân:** Các hàm đọc bàn phím tương tác trực tiếp với người dùng nên không được gọi trong luồng kiểm thử tự động không tương tác (headless automated testing).

### 3.4. Các nhánh kiểm tra ký tự đặc biệt (`validation.c`)
- **Vị trí:** Các nhánh kiểm tra ký tự điều khiển cấm, ký tự không in được trong `validate_name`, `validate_category`.
- **Nguyên nhân:** Tập test hiện tại tập trung vào các rủi ro tràn số nguyên, tràn chuỗi dài quá buffer và chuỗi rỗng. Các ký tự điều khiển ASCII đặc biệt sẽ được bổ sung vào bộ corpus fuzzing ở Bước 4.

---

## 4. Phân biệt giữa Độ phủ mã nguồn (Code Coverage) và Độ phủ kiểm chứng (Verification Coverage)

Một điểm mấu chốt được nhấn mạnh trong hướng dẫn của giảng viên: **"Không coi 100% độ phủ dòng là 100% an toàn bộ nhớ"**.
- **Code Coverage (36.4% lines, 32.2% branches):** Đo lường tỷ lệ các câu lệnh C thực tế đã được CPU duyệt qua ít nhất 1 lần trong phiên chạy test. Một dòng lệnh có thể được phủ 100% nhưng vẫn tiềm ẩn lỗ hổng tràn số nếu giá trị đầu vào chưa chạm tới điểm biên.
- **Verification Coverage (Độ phủ kiểm chứng đặc tả):**
  - **SMT Arithmetic Coverage:** Đã kiểm chứng 100% các biểu thức số học nhạy cảm (SMT-001 đến SMT-005) trên toàn bộ không gian số nguyên 32-bit và 64-bit.
  - **CBMC Bounded Safety Coverage:** Đã kiểm chứng 5/5 thuộc tính an toàn bộ nhớ cốt lõi (bất biến ProductList, biên mảng, realloc failure, con trỏ NULL/double-free, và bảo toàn tồn kho).
  - **CTL State Space Coverage:** Đạt **100% (21/21 trạng thái khả đạt)** và **100% (18/18 công thức CTL TRUE)** trên toàn bộ cây tính toán của mô hình Kripke.

---

## 5. Đề xuất các ca kiểm thử bổ sung cho Bước 4 (Bảo trì & Nâng cấp)

Để nâng cao độ phủ nhánh trong giai đoạn tiếp theo, nhóm đề xuất bổ sung:
1. **Mock Allocator Framework:** Tích hợp cơ chế can thiệp `malloc`/`realloc` có thể cấu hình số lần thành công trước khi trả NULL trực tiếp vào test suite chính.
2. **Interactive CLI Automation:** Xây dựng kịch bản mô phỏng bàn phím (`stdin piping`) để đo lường độ phủ cho `main.c` và các hàm nhập liệu trong `utils.c`.
3. **Fuzzing Target Mở rộng:** Áp dụng công cụ kiểm thử mờ (Fuzzing) tự động sinh chuỗi ký tự điều khiển ngẫu nhiên để phủ 100% các nhánh rẽ trong `validation.c`.
