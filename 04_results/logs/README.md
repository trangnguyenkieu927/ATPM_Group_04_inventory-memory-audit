# Thư Mục Logs Kết Quả Kiểm Toán An Toàn Bộ Nhớ (Nhóm 04)

Thư mục này chỉ lưu trữ các tệp nhật ký (logs) kết quả chính thức phục vụ báo cáo và bảo vệ đề tài:

1. **`01_kiem_chung_mo_hinh_kripke.log`**: 
   - Kiểm chứng 18 công thức CTL trên mô hình trạng thái Kripke.
   - Thể hiện sự vi phạm của bản `v0` (kèm phản ví dụ) và sự thỏa mãn 100% của bản `v1`.

2. **`02_kiem_tra_so_hoc_z3_smt.log`**: 
   - Kiểm tra rủi ro tràn số nguyên `int32_t` bằng Z3 SMT Solver.
   - Kết quả bản `v0`: **SAT** (tìm ra bộ số sinh lỗi tràn).
   - Kết quả bản `v1`: **UNSAT** (chứng minh toán học không thể tràn số).

3. **`03_bien_ban_kiem_thu_pytest.log`**: 
   - Nhật ký thực thi 10 ca kiểm thử đối chứng tự động (Pass 10/10).

4. **`04_kiem_toan_asan_v0_bat_loi.log`**:
   - Nhật ký AddressSanitizer (ASan) phát hiện lỗi tràn bộ đệm stack (CWE-120/121) và rò rỉ bộ nhớ heap (CWE-401) trên bản `v0`.

5. **`05_kiem_toan_asan_v1_sach_loi.log`**:
   - Nhật ký AddressSanitizer (ASan) xác nhận bản vá `v1` chạy sạch hoàn toàn 0 lỗi bộ nhớ.

*(Ghi chú: Toàn bộ 80 logs thử nghiệm cũ đã được lưu trữ riêng tại `05_TaiLieu_LuuTru/logs_cu_afl_fuzzing`).*
