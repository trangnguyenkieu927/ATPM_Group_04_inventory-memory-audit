# BÁO CÁO KẾT QUẢ MINH CHỨNG THỰC NGHIỆM HỆ THỐNG
## DỰ ÁN: KIỂM TOÁN AN TOÀN BỘ NHỚ C (INVENTORY MEMORY SAFETY AUDIT)
* **Thời điểm thực thi:** 23/10/2026 - Môi trường: GCC 16.1.0 / Python 3.12 / Z3 SMT Solver / SecLabFramework
* **Nhóm thực hiện:** ATPM_Group_04 (Nguyễn Kiều Trang, Nguyễn Quang Thọ, Hà Văn Đô)

---

### PHẦN I. MINH CHỨNG KIỂM CHỨNG HÌNH THỨC (CHƯƠNG 1)

#### 1. Kết quả kiểm chứng mô hình Kripke và 18 Thuộc tính CTL (NuSMV / Custom Model Checker)
* **Độ phủ không gian trạng thái:** 21/21 trạng thái khả đạt (**100.0%**).
* **Kết quả 18 công thức CTL:**
  * `[TRUE] CTL-01: AG(!(list_live & list_freed))` - Không bao giờ xảy ra tình trạng vừa hoạt động vừa đã giải phóng (REQ-1, REQ-11).
  * `[TRUE] CTL-02: AG(!(ptr_live & ptr_invalid))` - Con trỏ mượn không bao giờ vừa hợp lệ vừa mất hiệu lực (REQ-14).
  * `[TRUE] CTL-03: AG(ptr_invalid -> !ptr_deref)` - Con trỏ mất hiệu lực không bao giờ bị dereference (Chống Use-After-Free, Non-Vacuous tại 2 trạng thái).
  * `[TRUE] CTL-04: AG(realloc_failed -> owner_valid)` - Realloc thất bại vẫn bảo toàn con trỏ cũ, không bị leak (Chống Memory Leak, Non-Vacuous tại 1 trạng thái).
  * `[TRUE] CTL-05: AG(realloc_done -> owner_valid)` - Realloc thành công cập nhật con trỏ hợp lệ.
  * `[TRUE] CTL-06: AG(cleanup_done -> list_freed)` - Dọn dẹp hoàn tất đưa danh sách về trạng thái giải phóng sạch.
  * `[TRUE] CTL-07: AG(product_op_processing -> AF(product_op_done | product_op_rejected))` - Nghiệp vụ số học luôn kết thúc hoặc bị từ chối an toàn.
  * `[TRUE] CTL-08: AG(file_parsing -> AF(file_loaded | file_rejected))` - Đọc phân tích tệp luôn nạp xong hoặc từ chối rõ ràng.
  * `[TRUE] CTL-09: AG(buffer_processing -> AF(buffer_safe | buffer_rejected))` - Xử lý chuỗi stack buffer luôn an toàn hoặc bị chặn đứng.
  * `[TRUE] CTL-10: AG(reallocating -> AF(realloc_done | realloc_failed))` - Tái cấp phát luôn thành công hoặc thất bại an toàn.
  * `[TRUE] CTL-11: AG(cleanup_processing -> AF(cleanup_done | cleanup_rejected))` - Tiến trình thu hồi RAM luôn hoàn tất hoặc từ chối hợp lệ.
  * `[TRUE] CTL-12: AG(!(buffer_safe & buffer_rejected))` - Hai trạng thái an toàn và từ chối loại trừ lẫn nhau.
  * `[TRUE] CTL-13: AG(!(file_loaded & file_rejected))` - Hai trạng thái nạp thành công và từ chối loại trừ lẫn nhau.
  * `[TRUE] CTL-14: AG(!(realloc_done & realloc_failed))` - Hai trạng thái realloc xong và thất bại loại trừ lẫn nhau.
  * `[TRUE] CTL-15: AG(!(cleanup_done & cleanup_rejected))` - Hai trạng thái dọn dẹp xong và từ chối loại trừ lẫn nhau.
  * `[TRUE] CTL-16: AG(!(product_op_done & product_op_rejected))` - Hai trạng thái nghiệp vụ xong và từ chối loại trừ lẫn nhau.
  * `[TRUE] CTL-17: EF(list_freed)` - Khả đạt: Luôn tồn tại đường dẫn dẫn tới giải phóng 100% RAM.
  * `[TRUE] CTL-18: EF(realloc_failed)` - Khả đạt: Nhánh xử lý realloc thất bại là khả đạt trong không gian trạng thái.

#### 2. Kết quả kiểm chứng Số học SMT Bit-Vector (Z3 Solver)
* **SMT-001A (Nhân `capacity * 2` trên v0 không guard):** Trả về **`SAT`** $\rightarrow$ Tìm ra nghiệm phản ví dụ $capacity = 2^{63} = 9,223,372,036,854,775,808$, phép nhân tràn bit về 0!
* **SMT-001B (Nhân `capacity * 2` trên v1 có guard):** Trả về **`UNSAT`** $\rightarrow$ Đã chứng minh triệt tiêu hoàn toàn khả năng tràn số!
* **SMT-002A (Nhân `new_cap * 208` trên v0 không guard):** Trả về **`SAT`** $\rightarrow$ Nghiệm $new_cap = 88,686,269,585,142,076$, tràn số làm cấp phát thiếu dung lượng!
* **SMT-002B (Nhân `new_cap * 208` trên v1 có guard):** Trả về **`UNSAT`** $\rightarrow$ Bảo vệ an toàn tuyệt đối cấp phát Heap!
* **SMT-003 & SMT-004 (Cộng trừ tồn kho):** Trả về **`UNSAT`** $\rightarrow$ Bất biến tồn kho $0 \le quantity \le INT\_MAX$ được bảo toàn.

---

### PHẦN II. MINH CHỨNG ĐỐI CHỨNG MÃ NGUỒN C (CHƯƠNG 2)

Dưới đây là kết quả thực thi runtime độc lập được ghi nhận trực tiếp trên hệ thống:

```text
========================================================================================
MODULE 1: I/O RAW INPUT PIPELINE (Phụ trách: Nguyễn Quang Thọ)
----------------------------------------------------------------------------------------
Hàm 1: ingest_raw_line (CWE-120/121 Stack Buffer Overflow)
  - Case [Valid string 12B into 32B buffer]:
    + v0 = 12 (Thành công)
    + v1 = 12 (Thành công)
  - Case [Long string 64B into 32B buffer]:
    + v0 = OVERFLOW CORRUPT (Ghi đè phá hủy Stack Frame / Return Address)
    + v1 = -2 (Guard chặn đứng, trả về -2 an toàn, bảo toàn Stack)

Hàm 2: get_token_field (CWE-125 Out-of-bounds Read)
  - Case [Field index 2 of 4 (Hợp lệ)]:
    + v0 = 25 (Đọc đúng offset)
    + v1 = 25 (Đọc đúng offset)
  - Case [Field index 10 of 4 (Ngoài biên OOB)]:
    + v0 = 0 (Đọc rác RAM / Nguy cơ Segmentation Fault)
    + v1 = -2 (Guard chặn đứng, trả về -2 an toàn)

========================================================================================
MODULE 2: DYNAMIC MEMORY & HEAP LIFECYCLE (Phụ trách: Nguyễn Kiều Trang)
----------------------------------------------------------------------------------------
Hàm 3: calc_expand_capacity (CWE-190/787 Integer Wraparound -> Heap Overflow)
  - Case [Normal expand 2x: cap=10, factor=2]:
    + v0 = 20 (Tính đúng)
    + v1 = 20 (Tính đúng)
  - Case [Extreme capacity: cap=1,500,000,000, factor=2]:
    + v0 = -1,294,967,296 (Tràn bit dấu thành số âm -> Nguy cơ Heap Overflow)
    + v1 = -2 (Guard chặn đứng, trả về -2 an toàn)
  - Case [Negative cap: cap=-5]:
    + v0 = -10 (Sai logic)
    + v1 = -1 (Từ chối giá trị âm)

Hàm 4: realloc_buffer_leak (CWE-401 Memory Leak khi OOM & Timeout 2GB)
  - Case [Normal realloc: 16B -> 32B]:
    + v0 = 0 (Thành công)
    + v1 = 0 (Thành công)
  - Case [Simulated OOM failure]:
    + v0 = -2 (Gán buf = NULL làm mất con trỏ cũ -> RÒ RỈ RAM)
    + v1 = -2 (Dùng con trỏ tạm, free(buf) trước khi thoát -> 0 LEAK)
  - Case [Extreme size 2GB (2,147,483,646 bytes)]:
    + v0 = TIMEOUT / KẸT HỆ THỐNG
    + v1 = -1 (Guard phát hiện vượt ngưỡng, thoát ngay lập tức trong 0ms)

Hàm 5: cleanup_and_read_uaf (CWE-416 Use-After-Free)
  - Case [Free con trỏ sau đó đọc dữ liệu *ptr]:
    + v0 = 42 (Đọc con trỏ trôi nổi Dangling Pointer sau free)
    + v1 = 42 (Lưu biến tạm, gọi free(ptr), gán ptr = NULL, đọc an toàn)

========================================================================================
MODULE 3: ARITHMETIC & STATE UPDATE (Phụ trách: Hà Văn Đô)
----------------------------------------------------------------------------------------
Hàm 6: update_stock_quantity (CWE-190 Signed Integer Overflow Tồn kho)
  - Case [Normal addition: 100 + 50]:
    + v0 = 150 (Tính đúng)
    + v1 = 150 (Tính đúng)
  - Case [Overflow INT_MAX: 2,000,000,000 + 200,000,000]:
    + v0 = -2,094,967,296 (Tràn bit thành số âm -> Undefined Behavior C99/C17)
    + v1 = -2 (Guard chặn đứng, bảo toàn tồn kho cũ)
  - Case [Negative added quantity: -50]:
    + v0 = 50 (Sai logic nghiệp vụ kho)
    + v1 = -1 (Từ chối giá trị âm)

Hàm 7: scale_product_price (CWE-190 Arithmetic Overflow Nhân tỉ giá)
  - Case [Normal scale: 100 * 2]:
    + v0 = 200 (Tính đúng)
    + v1 = 200 (Tính đúng)
  - Case [Multiply Overflow INT_MAX: 1,500,000,000 * 3]:
    + v0 = 205,032,704 (Sai lệch giá trị số học nghiêm trọng)
    + v1 = -2 (Guard chặn đứng, trả về -2 an toàn)
```

---

### PHẦN III. KẾT LUẬN MINH CHỨNG
1. **Chương 1:** Đã chứng minh toàn diện bằng toán học hình thức: Kripke phủ kín 100% không gian trạng thái khả đạt, 18 công thức CTL đạt `TRUE`, Z3 SMT chứng minh chuyển đổi từ `SAT` (v0 có lỗ hổng) sang `UNSAT` (v1 an toàn tuyệt đối).
2. **Chương 2:** Toàn bộ 7 hàm kiểm toán trên bản vá `v1` đã triệt tiêu hoàn toàn các lỗi bộ nhớ của bản `v0`, đạt **100% PASS** trên tất cả các ca kiểm thử biên.
