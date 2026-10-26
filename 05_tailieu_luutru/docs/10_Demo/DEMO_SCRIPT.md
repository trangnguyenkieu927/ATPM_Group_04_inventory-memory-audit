# KỊCH BẢN THUYẾT MINH CHI TIẾT (DEMO SCRIPT)

**Người trình bày (Mở đầu):**
*"Em xin chào Thầy. Hôm nay nhóm ATPM_Group_04 xin phép báo cáo kết quả thực nghiệm bài tập lớn: Kiểm chứng an toàn bộ nhớ cho chương trình C mô phỏng quản lý kho. Thay vì chỉ viết tài liệu mô tả, nhóm xin được chứng minh trực tiếp chuỗi truy vết từ mã nguồn C gốc, các mô hình toán học, chương trình mô phỏng C, và kết quả kiểm chứng bản vá v1."*

**Bước 1: Điểm rủi ro trên mã nguồn C (Mở product.c)**
*"Xuất phát từ mã nguồn C tại hàm add_product dòng 118, khi mở rộng mảng động, chương trình thực hiện new_cap = capacity * 2 và realloc new_cap * sizeof(Product). Ở phiên bản baseline v0, hai phép tính này hoàn toàn thiếu guard kiểm tra tràn số nguyên size_t."*

**Bước 2: Giải bài toán SMT Z3 (Chạy terminal)**
*"Để chứng minh khả năng vi phạm về mặt toán học, nhóm đã mô hình hóa bài toán này bằng ngôn ngữ SMT-LIB2 bit-vector 64-bit. Khi chạy Z3 Solver, hệ thống tìm ra nghiệm SAT capacity = 0x8000000000000000 (tức 2 mũ 63). Phép nhân đôi sẽ bị wraparound về 0, dẫn đến cấp phát heap sai lệch."*

**Bước 3: Mô phỏng C và Kiểm toán động (Chạy demo runner)**
*"Nhóm đã xây dựng 6 bộ driver mô phỏng C tương ứng với 6 kịch bản SC-01 đến SC-06. Em xin phép chạy script run_demo.py. Thầy có thể thấy toàn bộ 31 ca kiểm thử mô phỏng đều PASS, và bộ Canary Memory Tracker xác nhận 0 bytes rò rỉ, 0 double-free trên chu trình chạy."*

**Bước 4: Đối chứng với bản có lỗi cài chủ đích**
*"Để kiểm chứng khả năng phát hiện lỗi, nhóm xây dựng bản đối chứng seeded stack overflow. Trình bảo vệ stack canary lập tức bắt được và dừng tiến trình an toàn, minh chứng cơ chế bảo vệ của môi trường hoạt động chuẩn xác."*

**Bước 5: Bản vá v1 và Kiểm chứng hồi quy**
*"Nhóm đã bổ sung 2 guard tường minh trước khi nhân đôi capacity và trước khi tính số byte cấp phát. Toàn bộ 17/17 ca smoke test, unit test và fuzzing 1000 executions đều PASS. Toàn bộ 18 REQs đã được bảo vệ trọn vẹn và thể hiện đầy đủ trên Master Workbook 17 sheets."*
