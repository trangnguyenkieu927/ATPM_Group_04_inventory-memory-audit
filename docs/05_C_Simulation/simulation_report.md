# BÁO CÁO MÔ PHỎNG AN TOÀN BỘ NHỚ C (C SIMULATION REPORT)

## 1. MỤC TIÊU VÀ NGUYÊN TẮC THIẾT KẾ C SIMULATOR
Đáp ứng chỉ đạo trọng tâm của Giảng viên:
> *"Xây dựng mã C để simulate lại rủi ro... Phải tạo được mã nguồn kiểm thử, mô hình, các tập dữ liệu và kết quả thực nghiệm thật. Không chỉ lập kế hoạch hoặc viết tài liệu mô tả."*

Nhóm ATPM_Group_04 đã xây dựng toàn bộ **6 bộ test driver C độc lập (SC-01 đến SC-06)** đặt tại `tests/simulation/SC-01/` đến `SC-06/`, liên kết trực tiếp với mã nguồn baseline `source/src/*.c`. Đồng thời, nhóm xây dựng **3 thí nghiệm đối chứng lỗi cài chủ đích (Seeded Vulnerabilities)** tại `tests/seeded/` để làm rõ cơ chế phòng vệ của phần mềm.

## 2. KẾT QUẢ THỰC THI 6 KỊCH BẢN MÔ PHỎNG CHÍNH

| Kịch bản | Tập tin Driver | Số ca kiểm thử | Kết quả Thực tế | Bằng chứng |
|---|---|---|---|---|
| **SC-01: Parser Input Validation** | `tests/simulation/SC-01/test_sc01_parser.c` | 7 ca | **7/7 PASS (100%)** | `results/simulation/sim_sc01.log` |
| **SC-02: Buffer Bounds & String Safety** | `tests/simulation/SC-02/test_sc02_buffer.c` | 5 ca | **5/5 PASS (100%)** | `results/simulation/sim_sc02.log` |
| **SC-03: Heap Allocation & Doubling** | `tests/simulation/SC-03/test_sc03_capacity.c` | 5 ca | **5/5 PASS (100%)** | `results/simulation/sim_sc03.log` |
| **SC-04: Allocation Failure & Leak Defense** | `tests/simulation/SC-04/test_sc04_alloc_failure.c` | 4 ca | **4/4 PASS (100%)** | `results/simulation/sim_sc04.log` |
| **SC-05: Pointer Lifetime & Borrowed Refs** | `tests/simulation/SC-05/test_sc05_pointer_lifetime.c` | 3 ca | **3/3 PASS (100%)** | `results/simulation/sim_sc05.log` |
| **SC-06: Stock Arithmetic & Conservation** | `tests/simulation/SC-06/test_sc06_stock_arithmetic.c` | 7 ca | **7/7 PASS (100%)** | `results/simulation/sim_sc06.log` |

## 3. PHÂN TÍCH KỸ THUẬT CÁC ĐIỂM SÁNG TRONG THỰC NGHIỆM
1. **Bảo toàn con trỏ khi OOM (SC-04):** Bằng kỹ thuật Fault Injection ép `mock_realloc` trả về `NULL`, test driver chứng minh con trỏ cũ `list->items` và toàn bộ 16 phần tử đã nạp trước đó được bảo toàn nguyên vẹn, hàm trả về `STATUS_ERR_MEMORY (-8)` mà không làm rò rỉ một byte bộ nhớ nào.
2. **Bất biến bảo toàn tồn kho (Conservation Invariant - SC-06):** Khi thực hiện giao dịch nhập kho tràn số (`INT_MAX + 50`) hoặc xuất kho vượt quá số lượng hiện có (`200 > 150`), hệ thống từ chối dứt khoát và số lượng tồn kho cũ được bảo toàn $100\%$ không suy suyển.
3. **Phòng vệ Double-Free Idempotent (SC-04):** Thử nghiệm gọi `product_list_free` hai lần liên tiếp và `product_list_free(NULL)` hoàn thành an toàn tuyệt đối do con trỏ được chủ động gán về `NULL` ngay sau khi giải phóng.

## 4. KẾT QUẢ ĐỐI CHỨNG VỚI MÃ LỖI CÀI CHỦ ĐÍCH (SEEDED VULNERABILITIES)
Để minh chứng giá trị bảo vệ của Baseline v0, nhóm đã thực thi đối chứng với 3 mẫu lỗi cài đặt chủ đích tại `tests/seeded/`:
- **SEED-01 (Stack Buffer Overflow):** Mô phỏng parser không kiểm tra `token_count < 6`. Khi nạp dòng 10 token, cơ chế bảo vệ stack canary của GCC (`-fstack-protector-strong`) lập tức phát hiện ghi đè stack và can thiệp. Trong khi đó, baseline v0 có guard cứng `token_count < 8` nên hoàn toàn miễn nhiễm.
- **SEED-02 (Use-After-Free):** Minh chứng nguy cơ khi con trỏ mượn bị dereference sau khi realloc làm thay đổi địa chỉ mảng.
- **SEED-03 (Memory Leak do realloc):** Minh họa thói quen xấu `ptr = realloc(ptr, ...)` trực tiếp làm mất dấu địa chỉ khi hết RAM.
- **Nguyên tắc phân loại:** Toàn bộ các phát hiện này được gắn nhãn minh bạch là `SEEDED_VULNERABILITY`, không gộp vào khiếm khuyết của baseline.

## 5. DANH MỤC TÀI LIỆU VÀ BẢNG BIỂU ĐÃ XUẤT
Tất cả các tài liệu được lưu tại `docs/05_C_Simulation/`:
1. `simulation_results.xlsx`: Kết quả thực thi chi tiết 31 ca kiểm thử mô phỏng.
2. `simulation_traceability.xlsx`: Ma trận truy vết từ C Test Driver đến REQ và Bất biến an toàn.
3. `seeded_vulnerability_register.xlsx`: Sổ đăng ký các thí nghiệm đối chứng lỗi cài chủ đích.
4. `simulation_experiments.json`: Dữ liệu JSON chuẩn hóa phục vụ tích hợp báo cáo tổng thể.
