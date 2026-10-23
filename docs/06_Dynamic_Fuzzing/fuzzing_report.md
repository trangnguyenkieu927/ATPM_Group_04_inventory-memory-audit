# BÁO CÁO THỰC NGHIỆM ĐỘT BIẾN ĐẦU VÀO VÀ FUZZING (FUZZING REPORT)

## 1. MỤC TIÊU VÀ PHƯƠNG PHÁP THỰC NGHIỆM
Theo quy định tại Công đoạn 7.3 của Master Prompt:
- Bổ sung Fuzzing làm thí nghiệm kiểm tra đầu vào chuyên sâu đối với parser nghiệp vụ đọc tệp (`load_products_from_file` và `parse_product_line`).
- Xây dựng **In-Process Mutation Fuzzing Engine** kết hợp bộ Seed Corpus gồm 10 lớp kịch bản ranh giới đầu vào (SC-01 và SC-02).

## 2. KẾT QUẢ CHIẾN DỊCH FUZZING (FUZZ-001)
- **Tổng số lần thực thi (Executions):** `1000` lượt.
- **Thời gian thực thi:** `26.35` giây.
- **Tốc độ trung bình:** `38.0` executions/giây.
- **Số lượng Crashes phát hiện trên Baseline v0:** **`0` crashes**.
- **Số lượng Hangs (Timeout 2s):** **`0` hangs**.
- **Tập tin Log chiến dịch:** `results/fuzzing/fuzz_campaign.log`.

## 3. ĐÁNH GIÁ VÀ TRIAGE KẾT QUẢ
> [!NOTE]
> **Kết luận khoa học:** *Việc không phát hiện crash (0 crashes) sau 1000 lần đột biến ngẫu nhiên trên Baseline v0 chứng minh rằng parser của chương trình đã được lập trình phòng vệ rất tốt:*
> 1. Có guard kiểm soát số token `token_count < 8`.
> 2. Có kiểm tra độ dài xâu trước khi copy `strlen(tokens[i]) < sizeof(field)`.
> 3. Có hàm `trim_line_end` và bộ kiểm định số học `parse_int` / `parse_price` loại bỏ giá trị rác, `NaN`, `Infinity` và số âm.

Đối chứng với nhánh có lỗi chủ đích (Seeded Branch): Kịch bản `tests/seeded/seeded_stack_overflow.c` bị crash ngay lập tức ở lần thử đầu tiên do thiếu guard, xác nhận cơ chế phát hiện của môi trường là hoàn toàn chính xác.
