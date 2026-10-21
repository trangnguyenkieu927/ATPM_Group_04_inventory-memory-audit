# BÁO CÁO THỰC NGHIỆM ĐỘT BIẾN ĐẦU VÀO VÀ FUZZING (FUZZING REPORT)

## 1. MỤC TIÊU VÀ PHƯƠNG PHÁP THỰC NGHIỆM
Theo quy định tại Công đoạn 7.3 của Master Prompt:
- Bổ sung Fuzzing làm thí nghiệm kiểm tra đầu vào chuyên sâu đối với parser nghiệp vụ đọc tệp (`load_products_from_file` và `parse_product_line`).
- Xây dựng **In-Process Mutation Fuzzing Engine** kết hợp bộ Seed Corpus gồm 10 lớp kịch bản ranh giới đầu vào (SC-01 và SC-02).

## 2. KẾT QUẢ CHIẾN DỊCH FUZZING (FUZZ-001)
- **Tổng số lần thực thi (Executions):** `1000` lượt.
- **Thời gian thực thi:** `19.00` giây.
- **Tốc độ trung bình:** `52.6` executions/giây.
- **Số lượng Crashes phát hiện trên Baseline v0:** **`0` crashes**.
- **Số lượng Hangs (Timeout 2s):** **`0` hangs**.
- **Tập tin Log chiến dịch:** `results/fuzzing/fuzz_campaign.log`.

## 3. ĐÁNH GIÁ VÀ TRIAGE KẾT QUẢ
> [!NOTE]
> **Phân định rõ ràng giữa Fuzzing ngẫu nhiên và Chứng minh lỗi tiềm ẩn:**
> 1. **Fuzzing ngẫu nhiên (Robustness Testing - 1,000 runs, 0 crash):** Thử nghiệm kiểm tra độ bền của vòng lặp đọc file trước các đột biến byte/bit ngẫu nhiên thông thường (không làm treo/hang chương trình).
> 2. **Kích hoạt và Bằng chứng lỗi tiềm ẩn (C Simulation & Seeded Harness):**
>    - **CWE-121 (Stack Buffer Overflow):** Kích hoạt qua `tests/seeded/seeded_stack_overflow.c` và `tests/simulation/SC-01` khi dòng CSV vượt quá 6 token, kích hoạt Stack Canary Trap lập tức làm crash tiến trình (exit code 3221226505 / 0xC0000409).
>    - **CWE-190 (Integer Overflow in Heap Allocation):** Mô hình hóa và chứng minh bằng SMT Z3 Bit-vector 64-bit (`tools/verification/verify_smt_z3.py`), tìm ra nghiệm phản ví dụ SAT gây tràn số `capacity * 2`.
>    - **CWE-401 (Memory Leak upon Realloc OOM):** Mô phỏng và chứng minh qua `tests/simulation/SC-03` kết hợp bộ theo dõi Canary Memory Tracker phát hiện mất con trỏ heap khi `realloc` thất bại.
>    - **CWE-416 (Use-After-Free):** Mô phỏng và chứng minh qua `tests/simulation/SC-04` khi con trỏ mượn từ `find_product_by_id` bị treo sau khi danh sách mở rộng dung lượng.
>    - **CWE-120 / CWE-125 (Buffer Boundary & Silent Truncation):** Mô phỏng qua `tests/simulation/SC-02` khi đọc dòng vượt quá kích thước 512B mà không có cơ chế drain.
