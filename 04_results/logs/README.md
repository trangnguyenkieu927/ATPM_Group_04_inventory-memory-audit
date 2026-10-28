# Thư Mục Logs Kết Quả Kiểm Toán An Toàn Bộ Nhớ (Nhóm 04)

Thư mục này lưu trữ các tệp nhật ký (logs) kết quả chính thức được đánh số thứ tự tuần tự từ 01 đến 09:

1. **`01_kiem_chung_mo_hinh_kripke.log`**: Kiểm chứng 22 công thức CTL trên mô hình Kripke toàn hệ thống (bản v0 vi phạm 18 rủi ro; bản v1 đạt 100% thỏa mãn).
2. **`02_kiem_chung_ctl_bmc.log`**: Bounded Model Checking (BMC) kiểm chứng thuộc tính an toàn $AG(!violation)$ qua từng bước chuyển trạng thái hữu hạn $k$.
3. **`03_kiem_tra_so_hoc_z3_smt.log`**: Kiểm tra rủi ro tràn số nguyên `int32_t` bằng Z3 SMT Solver (v0 SAT phát hiện lỗi, v1 UNSAT chứng minh an toàn).
4. **`04_kiem_chung_z3_bmc_memory.log`**: Kiểm chứng mô hình bộ nhớ con trỏ Heap và con trỏ treo bằng Z3 BMC.
5. **`05_kiem_tra_sat_unsat_m2.log`**: Kiểm chứng chuyên sâu 4 bài toán Module M2 (Heap Allocation, Capacity Doubling, Realloc Leak, Use-After-Free) trên Z3 SMT.
6. **`06_bien_ban_kiem_thu_pytest.log`**: Biên bản thực thi kiểm thử hồi quy tự động đối chứng 10 ca kiểm thử (Pass 10/10).
7. **`07_kiem_toan_asan_v0_bat_loi.log`**: Nhật ký Clang AddressSanitizer (ASan) phát hiện lỗi tràn bộ đệm stack và rò rỉ bộ nhớ trên bản v0.
8. **`08_kiem_toan_asan_v1_sach_loi.log`**: Nhật ký Clang AddressSanitizer (ASan) xác nhận bản vá v1 chạy sạch 100% 0 lỗi bộ nhớ.
9. **`09_bao_cao_pipeline_specverificationlab_v0.log`**: Báo cáo tổng hợp tự động từ pipeline của SpecVerificationLab / SecLabFramework.

*(Ghi chú: Toàn bộ logs thử nghiệm cũ đã được lưu trữ riêng tại `05_tailieu_luutru/logs_cu_afl_fuzzing/`).*
