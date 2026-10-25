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

## 4. KẾT QUẢ THỰC NGHIỆM MÔ PHỎNG VÀ KÍCH HOẠT CÁC RỦI RO TIỀM ẨN
Để kích hoạt và cô lập chính xác các rủi ro bộ nhớ tiềm ẩn phát hiện được trên Baseline v0, nhóm đã thực thi các module kiểm nghiệm cô lập (Simulation Harness Drivers) tại `tests/seeded/`:
- **EXP-01 (Stack Buffer Overflow - CWE-121):** Mô phỏng khi parser không có guard chặn giới hạn số token `token_count < 6`. Khi nạp dòng CSV vượt quá 6 token, cơ chế bảo vệ stack canary của GCC (`-fstack-protector-strong`) lập tức phát hiện ghi đè stack và kích hoạt abort (exit code 3221226505 / 0xC0000409).
- **EXP-02 (Use-After-Free - CWE-416):** Chứng minh rủi ro khi con trỏ mượn từ `find_product_by_id` bị dereference sau khi danh sách heap bị realloc sang địa chỉ mới.
- **EXP-03 (Memory Leak do realloc - CWE-401):** Minh chứng nguy cơ khi realloc thất bại nếu không dùng con trỏ tạm, dẫn tới mất địa chỉ vùng nhớ cũ gây rò rỉ heap.
- **Ý nghĩa kiểm toán:** Các thực nghiệm này chứng minh rõ ràng cơ chế phát sinh lỗi vi phạm an toàn bộ nhớ trong môi trường C, làm căn cứ vững chắc để thiết kế và kiểm chứng bản vá bảo vệ triệt để trong bản v1.

## 5. DANH MỤC TÀI LIỆU VÀ BẢNG BIỂU ĐÃ XUẤT
Tất cả các tài liệu được lưu tại `docs/05_C_Simulation/`:
1. `simulation_results.xlsx`: Kết quả thực thi chi tiết 31 ca kiểm thử mô phỏng.
2. `simulation_traceability.xlsx`: Ma trận truy vết từ C Test Driver đến REQ và Bất biến an toàn.
3. `seeded_vulnerability_register.xlsx`: Sổ đăng ký các thí nghiệm đối chứng lỗi cài chủ đích.
4. `simulation_experiments.json`: Dữ liệu JSON chuẩn hóa phục vụ tích hợp báo cáo tổng thể.
