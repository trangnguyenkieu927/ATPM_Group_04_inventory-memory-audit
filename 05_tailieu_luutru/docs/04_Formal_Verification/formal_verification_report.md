# BÁO CÁO KẾT QUẢ KIỂM CHỨNG HÌNH THỨC (FORMAL VERIFICATION REPORT)

## 1. PHƯƠNG PHÁP HÌNH THỨC HÓA TOÀN DỰ ÁN
Để đáp ứng triệt để yêu cầu của Giảng viên về tính khoa học và trung thực, nhóm ATPM_Group_04 đã triển khai kiểm chứng hình thức trên 3 tầng phân tích bổ trợ:
1. **Tầng số học hữu hạn (Bit-Vector SMT via Z3):** Phân tích tính tràn số nguyên `size_t` và `int32_t` trên cấu trúc bit thực tế của kiến trúc x86_64 Data Model (LLP64 / LP64).
2. **Tầng bất biến hợp đồng (GCC Bounded Contract Testing):** Kiểm tra tính bất biến của mảng động `ProductList` trong phạm vi unwind $k$ có giới hạn. (Ghi nhận trung thực: CBMC Native là `TOOL_UNAVAILABLE`).
3. **Tầng mô hình hóa trạng thái (Temporal Logic / CTL Model Checking):** Mô hình hóa hệ thống bằng đồ thị Kripke 5 phân hệ gồm 21 trạng thái khả đạt, kiểm chứng 18 công thức CTL bằng Custom CTL Checker.

## 2. KẾT QUẢ KIỂM CHỨNG SỐ HỌC BẰNG Z3 SMT SOLVER
Nhóm đã chạy 10 bài toán SMT chuẩn hóa với kết quả xác thực độc lập:
- **`SMT-001A` (`capacity * 2` không guard):** Trả về `SAT` với nghiệm phản ví dụ $capacity = 2^{63} = 9223372036854775808$. Phép nhân tràn về $0$ trong số học modulo $2^{64}$.
- **`SMT-001B` (Tính khả đạt):** Trả về `UNSAT`, chứng minh rằng trong phạm vi sử dụng thông thường ($capacity \le 1,000,000$), trạng thái tràn số này là không khả đạt tại runtime.
- **`SMT-002A` (`new_cap * 208` không guard):** Trả về `SAT` với nghiệm $new_cap = 88686269585142076 > \lfloor (2^{64}-1)/208 floor$. Phép nhân bị wraparound làm kích thước cấp phát bé hơn thực tế.
- **`SMT-002B` (`new_cap * 208` có guard):** Trả về `UNSAT`, chứng minh guard phòng vệ `new_cap <= SIZE_MAX / sizeof(Product)` triệt tiêu hoàn toàn khả năng tràn số.
- **`SMT-003A` & `SMT-003B` (Nhập kho `INT_MAX`):** Khẳng định guard `INT_MAX - current_stock < add_quantity` trong baseline v0 là hoàn toàn an toàn (`UNSAT`).

## 3. KẾT QUẢ KIỂM CHỨNG BẤT BIẾN HỢP ĐỒNG (GCC BOUNDED TESTING)
- Cả 5 harness đều đạt kết quả `PASS` với các độ sâu unwind:
  - `HARNESS-001`: $k=20$, bảo toàn bất biến $count \le capacity$ và $items 
e NULL$.
  - `HARNESS-002`: $k=40$, bảo toàn không truy cập ngoài mảng.
  - `HARNESS-003`: $k=17$, mô phỏng Fault Injection ép realloc trả về NULL, con trỏ cũ được bảo toàn 100%.
  - `HARNESS-004`: $k=17$, kiểm tra 100% API từ chối con trỏ NULL và idempotent cleanup.
  - `HARNESS-005`: $k=10$, bảo toàn giới hạn tồn kho khi số lượng vượt `INT_MAX`.

## 4. KẾT QUẢ MÔ HÌNH HÓA TRẠNG THÁI VÀ CTL CHECKING
- **Độ phủ không gian trạng thái:** 21/21 trạng thái khả đạt ($100.0\%$).
- **Số lượng công thức CTL:** 18/18 công thức đạt giá trị `TRUE`.
- **Kiểm tra tính không rỗng (Non-Vacuous Truth):** Toàn bộ các tiền đề trong công thức an toàn và liveness đều đã được chứng minh khả đạt ít nhất một lần trong mô hình (ví dụ: `realloc_failed` khả đạt tại 1 trạng thái, `ptr_invalid` khả đạt tại 2 trạng thái).

## 5. TỔNG HỢP CÁC FILE BÁO CÁO VÀ WORKBOOK ĐÃ XUẤT
Tất cả các tài liệu được lưu trữ tại `docs/04_Formal_Verification/`:
1. `smt_results.xlsx`: Bảng tổng hợp chi tiết 10 bài toán SMT Z3.
2. `bmc_results.xlsx`: Bảng tổng hợp 5 bài kiểm tra GCC Bounded Testing.
3. `ctl_results.xlsx`: Bảng chi tiết 18 công thức CTL và kết quả kiểm chứng.
4. `state_transition_matrix.xlsx`: Ma trận chuyển trạng thái hệ thống Kripke 5 phân hệ.
5. `coverage_plan.xlsx`: Kế hoạch kiểm chứng phân bổ theo các lớp đầu vào và đối tượng bộ nhớ.
6. `formal_limitations.md`: Báo cáo các giới hạn trung thực của kiểm chứng hình thức.
