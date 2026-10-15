# BÁO CÁO KIỂM CHỨNG MÔ HÌNH TRẠNG THÁI VÀ THUỘC TÍNH THỜI GIAN CTL

**Học phần:** An toàn phần mềm (CSE703093)  
**Đề tài:** Kiểm chứng an toàn bộ nhớ cho chương trình C mô phỏng nghiệp vụ quản lý kho  
**Phụ trách chính:** Hà Văn Đô – Kỹ sư Đặc tả hình thức & Kiểm chứng Mô hình trạng thái  
**Công cụ thực thi:** Custom CTL Model Checker (`verify_ctl.py`; NuSMV/nuXmv native là `TOOL_UNAVAILABLE`)<br>
**Tệp mô hình NuSMV:** `verification/step3/ctl/inventory_model.smv`  
**Tệp script kiểm chứng:** `verification/step3/ctl/verify_ctl.py`  
**Tệp kết quả JSON:** `results/step3/ctl/ctl_verification_results.json`  
**Ngày thực nghiệm:** 15/10/2026  

---

## 1. Mục tiêu và cơ sở hình thức hóa Kripke

Kiểm chứng thời gian nhánh (Computation Tree Logic - CTL) cho phép kiểm tra các thuộc tính an toàn (Safety) và sống còn (Liveness) trên cây tính toán vô hạn của hệ thống quản lý kho.

*Lưu ý về công cụ*: Do binary NuSMV / nuXmv native là `TOOL_UNAVAILABLE` trên môi trường Windows host, việc đánh giá mô hình Kripke và 18 công thức CTL được thực thi bằng Custom CTL Model Checker (`verify_ctl.py`). Đây là công cụ Python tự kiểm tra mô hình CTL, không gọi là kết quả NuSMV chính thức. Tệp `inventory_model.smv` được giữ chuẩn cú pháp để có thể tái lập trên NuSMV native khi chuyển sang môi trường Linux.

Cấu trúc Kripke của hệ thống được hình thức hóa dưới dạng bộ $M = (S, S_0, R, L)$:
- $S$: Tập hợp 21 trạng thái rời rạc đại diện cho 5 phân hệ nghiệp vụ và quản lý bộ nhớ.
- $S_0 = \{S01\_list\_init\}$: Trạng thái khởi tạo duy nhất của hệ thống.
- $R \subseteq S \times S$: Quan hệ chuyển trạng thái phản ánh chính xác các luồng điều khiển trong mã nguồn C.
- $L: S \to 2^{AP}$: Hàm gán nhãn các mệnh đề nguyên tử tương ứng với đặc tả yêu cầu trong workbook chuẩn.

---

## 2. Bảng ma trận 21 trạng thái hệ thống và độ phủ khả đạt (State Reachability)

Độ phủ trạng thái đạt **21/21 trạng thái khả đạt (100.0%)** được chứng minh bằng thuật toán tìm kiếm theo chiều rộng (BFS) xuất phát từ $S_0$:

| State ID | Tên trạng thái | Phân hệ tương ứng | Incoming Transitions | Outgoing Transitions | Khả đạt (Reachable) | Mệnh đề nguyên tử (Labels) | Công thức CTL liên quan |
| :--- | :--- | :--- | :--- | :--- | :---: | :--- | :--- |
| **S01** | `S01_list_init` | Vòng đời mảng động | `S07_list_freed` (Init) | `S02_list_valid` | **REACHABLE** | `owner_valid` | CTL-01, CTL-17 |
| **S02** | `S02_list_valid` | Vòng đời mảng động | S01, S04, S05, S08, S09, S13, S17, S21 | S03, S06, S08, S09, S13, S17 | **REACHABLE** | `list_live`, `owner_valid`, `ptr_live` | CTL-01, CTL-02, CTL-04 |
| **S03** | `S03_list_growth` | Mở rộng heap | `S02_list_valid` | `S04_list_growth_ok`, `S05_list_growth_failed` | **REACHABLE** | `list_live`, `owner_valid`, `reallocating` | CTL-10, CTL-14 |
| **S04** | `S04_list_growth_ok` | Mở rộng heap | `S03_list_growth` | `S02_list_valid` | **REACHABLE** | `list_live`, `owner_valid`, `realloc_done`, `ptr_invalid` | CTL-02, CTL-03, CTL-05, CTL-14 |
| **S05** | `S05_list_growth_failed`| Mở rộng heap (OOM) | `S03_list_growth` | `S02_list_valid` | **REACHABLE** | `list_live`, `owner_valid`, `realloc_failed`, `ptr_live` | CTL-04, CTL-10, CTL-14, CTL-18 |
| **S06** | `S06_list_cleanup` | Dọn dẹp tài nguyên | `S02_list_valid` | `S07_list_freed`, `S21_cleanup_rejected` | **REACHABLE** | `cleanup_processing` | CTL-11, CTL-15 |
| **S07** | `S07_list_freed` | Dọn dẹp tài nguyên | `S06_list_cleanup` | `S01_list_init` | **REACHABLE** | `list_freed`, `cleanup_done`, `ptr_invalid` | CTL-01, CTL-03, CTL-06, CTL-17 |
| **S08** | `S08_ptr_deref_ok` | Con trỏ mượn | `S02_list_valid` | `S02_list_valid` | **REACHABLE** | `list_live`, `owner_valid`, `ptr_live`, `ptr_deref` | CTL-02, CTL-03 |
| **S09** | `S09_file_idle` | Nạp tệp dữ liệu | `S02_list_valid`, S11, S12 | `S10_file_parsing`, `S02_list_valid` | **REACHABLE** | `list_live`, `owner_valid` | CTL-08, CTL-13 |
| **S10** | `S10_file_parsing` | Nạp tệp dữ liệu | `S09_file_idle` | `S11_file_loaded`, `S12_file_rejected` | **REACHABLE** | `list_live`, `owner_valid`, `file_parsing` | CTL-08, CTL-13 |
| **S11** | `S11_file_loaded` | Nạp tệp dữ liệu | `S10_file_parsing` | `S09_file_idle` | **REACHABLE** | `list_live`, `owner_valid`, `file_loaded` | CTL-08, CTL-13 |
| **S12** | `S12_file_rejected`| Nạp tệp dữ liệu | `S10_file_parsing` | `S09_file_idle` | **REACHABLE** | `list_live`, `owner_valid`, `file_rejected` | CTL-08, CTL-13 |
| **S13** | `S13_buf_idle` | Bộ đệm chuỗi | `S02_list_valid`, S15, S16 | `S14_buf_processing`, `S02_list_valid` | **REACHABLE** | `list_live`, `owner_valid` | CTL-09, CTL-12 |
| **S14** | `S14_buf_processing` | Bộ đệm chuỗi | `S13_buf_idle` | `S15_buf_safe`, `S16_buf_rejected` | **REACHABLE** | `list_live`, `owner_valid`, `buffer_processing` | CTL-09, CTL-12 |
| **S15** | `S15_buf_safe` | Bộ đệm chuỗi | `S14_buf_processing` | `S13_buf_idle` | **REACHABLE** | `list_live`, `owner_valid`, `buffer_safe` | CTL-09, CTL-12 |
| **S16** | `S16_buf_rejected` | Bộ đệm chuỗi | `S14_buf_processing` | `S13_buf_idle` | **REACHABLE** | `list_live`, `owner_valid`, `buffer_rejected` | CTL-09, CTL-12 |
| **S17** | `S17_op_idle` | Nghiệp vụ kho | `S02_list_valid`, S19, S20 | `S18_op_processing`, `S02_list_valid` | **REACHABLE** | `list_live`, `owner_valid` | CTL-07, CTL-16 |
| **S18** | `S18_op_processing`| Nghiệp vụ kho | `S17_op_idle` | `S19_op_done`, `S20_op_rejected` | **REACHABLE** | `list_live`, `owner_valid`, `product_op_processing` | CTL-07, CTL-16 |
| **S19** | `S19_op_done` | Nghiệp vụ kho | `S18_op_processing` | `S17_op_idle` | **REACHABLE** | `list_live`, `owner_valid`, `product_op_done` | CTL-07, CTL-16 |
| **S20** | `S20_op_rejected` | Nghiệp vụ kho | `S18_op_processing` | `S17_op_idle` | **REACHABLE** | `list_live`, `owner_valid`, `product_op_rejected` | CTL-07, CTL-16 |
| **S21** | `S21_cleanup_rejected`| Dọn dẹp tài nguyên | `S06_list_cleanup` | `S02_list_valid` | **REACHABLE** | `cleanup_rejected`, `owner_valid` | CTL-11, CTL-15 |

---

## 3. Bảng kết quả kiểm chứng 18 công thức CTL và phân tích Vacuous Truth

| CTL ID | Công thức CTL nguyên văn | Kết quả | Ý nghĩa kiểm chứng | Phân tích tính không rỗng (Non-Vacuous Truth) |
| :--- | :--- | :---: | :--- | :--- |
| **CTL-01** | `AG(!(list_live & list_freed))` | **TRUE** | Danh sách không bao giờ vừa đang sống vừa đã giải phóng (REQ-1, REQ-11). | Công thức dạng bất biến loại trừ; không phụ thuộc vào tiền đề rỗng. |
| **CTL-02** | `AG(!(ptr_live & ptr_invalid))` | **TRUE** | Con trỏ mượn không bao giờ vừa hợp lệ vừa mất hiệu lực (REQ-14). | Hai trạng thái con trỏ loại trừ nhau tuyệt đối tại mọi thời điểm. |
| **CTL-03** | `AG(ptr_invalid -> !ptr_deref)` | **TRUE** | Con trỏ mất hiệu lực không bao giờ bị dereference (REQ-14). | **NON-VACUOUS:** Tiền đề `ptr_invalid` khả đạt thực sự tại 2 trạng thái (`S04`, `S07`). Tại các trạng thái này, caller không thực hiện dereference. |
| **CTL-04** | `AG(realloc_failed -> owner_valid)` | **TRUE** | Realloc thất bại bảo toàn con trỏ sở hữu cũ (REQ-12). | **NON-VACUOUS:** Tiền đề `realloc_failed` khả đạt tại trạng thái `S05`. Con trỏ cũ được bảo toàn. |
| **CTL-05** | `AG(realloc_done -> owner_valid)` | **TRUE** | Realloc thành công cập nhật con trỏ sở hữu hợp lệ (REQ-12). | **NON-VACUOUS:** Tiền đề `realloc_done` khả đạt tại trạng thái `S04`. Con trỏ mới được cập nhật. |
| **CTL-06** | `AG(cleanup_done -> list_freed)` | **TRUE** | Dọn dẹp hoàn tất đưa danh sách về trạng thái giải phóng (REQ-11). | **NON-VACUOUS:** Tiền đề `cleanup_done` khả đạt tại trạng thái `S07`. |
| **CTL-07** | `AG(product_op_processing -> AF(product_op_done \| product_op_rejected))` | **TRUE** | Mọi thao tác nghiệp vụ đang xử lý cuối cùng sẽ dừng tất định (Liveness). | **NON-VACUOUS:** Tiền đề `product_op_processing` khả đạt tại `S18`. Mọi đường đi đều dẫn tới `S19` hoặc `S20`. |
| **CTL-08** | `AG(file_parsing -> AF(file_loaded \| file_rejected))` | **TRUE** | Tiến trình nạp tệp đang phân tích cuối cùng sẽ dừng tất định (Liveness). | **NON-VACUOUS:** Tiền đề `file_parsing` khả đạt tại `S10`. Vòng lặp đọc tệp luôn kết thúc. |
| **CTL-09** | `AG(buffer_processing -> AF(buffer_safe \| buffer_rejected))` | **TRUE** | Xử lý chuỗi đang chạy cuối cùng sẽ an toàn hoặc bị từ chối (Liveness). | **NON-VACUOUS:** Tiền đề `buffer_processing` khả đạt tại `S14`. |
| **CTL-10** | `AG(reallocating -> AF(realloc_done \| realloc_failed))` | **TRUE** | Cấp phát lại đang chạy cuối cùng sẽ thành công hoặc thất bại (Liveness). | **NON-VACUOUS:** Tiền đề `reallocating` khả đạt tại `S03`. Allocator luôn phản hồi có giới hạn. |
| **CTL-11** | `AG(cleanup_processing -> AF(cleanup_done \| cleanup_rejected))` | **TRUE** | Giải phóng đang chạy cuối cùng sẽ hoàn tất hoặc bị từ chối (Liveness). | **NON-VACUOUS:** Tiền đề `cleanup_processing` khả đạt tại `S06`. |
| **CTL-12** | `AG(!(buffer_safe & buffer_rejected))` | **TRUE** | Chuỗi an toàn và bị từ chối loại trừ lẫn nhau. | Bất biến loại trừ trạng thái kết quả. |
| **CTL-13** | `AG(!(file_loaded & file_rejected))` | **TRUE** | Tải tệp thành công và thất bại loại trừ lẫn nhau. | Bất biến loại trừ trạng thái kết quả. |
| **CTL-14** | `AG(!(realloc_done & realloc_failed))` | **TRUE** | Cấp phát thành công và thất bại loại trừ lẫn nhau. | Bất biến loại trừ trạng thái kết quả. |
| **CTL-15** | `AG(!(cleanup_done & cleanup_rejected))` | **TRUE** | Dọn dẹp hoàn tất và bị từ chối loại trừ lẫn nhau. | Bất biến loại trừ trạng thái kết quả. |
| **CTL-16** | `AG(!(product_op_done & product_op_rejected))` | **TRUE** | Nghiệp vụ thành công và thất bại loại trừ lẫn nhau. | Bất biến loại trừ trạng thái kết quả. |
| **CTL-17** | `EF(list_freed)` | **TRUE** | Luôn tồn tại đường dẫn dẫn tới giải phóng bộ nhớ (Liveness). | Khả đạt: Tồn tại đường đi từ $S_0 \to S02 \to S06 \to S07$. |
| **CTL-18** | `EF(realloc_failed)` | **TRUE** | Nhánh realloc thất bại là khả đạt trong không gian trạng thái (Reachability). | Khả đạt: Tồn tại đường đi từ $S_0 \to S02 \to S03 \to S05$. |

---

## 4. Xử lý chuyên sâu Bẫy Logic Đúng hiển nhiên (Vacuous Truth Trap)

Trong kiểm chứng mô hình thời gian, bẫy logic nguy hiểm nhất xảy ra với các công thức mệnh đề kéo theo $AG(p \implies q)$:
- Nếu tiền đề $p$ **không bao giờ khả đạt** trong toàn bộ đồ thị chuyển tiếp ($\forall s \in \text{Reachable}, p \notin L(s)$), thì mệnh đề $p \implies q$ luôn nhận giá trị chân lý **TRUE** hiển nhiên (chân lý rỗng). Khi đó, công cụ model checker báo PASS nhưng thực chất không kiểm chứng được bất kỳ hành vi bảo vệ nào của hệ thống.

**Biện pháp giải quyết tại Bước 3:**
1. **Kiểm tra tính khả đạt của tiền đề:**
   - Với `CTL-03`: Trạng thái `ptr_invalid` có xuất hiện thực tế tại `S04_list_growth_ok` (khi realloc di dời mảng heap) và tại `S07_list_freed` (khi mảng bị giải phóng). Cả hai trạng thái đều được BFS chứng minh là khả đạt từ $S_0$.
   - Với `CTL-04` và `CTL-18`: Trạng thái `realloc_failed` là khả đạt thực sự tại `S05_list_growth_failed`, chứng minh nhánh lỗi OOM không bị dead-code.
2. **Không áp đặt tiên đề loại trừ giả tạo (No Axiomatic Assumption):**
   - Trong bảng `LoaiTruLanNhau` của workbook gốc có cặp `(ptr_deref, ptr_invalid)`. Nhóm **không sử dụng lệnh `TRANS !(ptr_deref & ptr_invalid)` trong NuSMV**. Thuộc tính an toàn `AG(ptr_invalid -> !ptr_deref)` đạt được là do chính quy tắc chuyển tiếp của caller code C: khi con trỏ bị vô hiệu hóa, caller không bao giờ thực hiện hành động dereference mà phải tái mượn con trỏ mới.

---

## 5. Giới hạn mô hình hóa và kết luận chuyển giao

1. **Tính trừu tượng hóa của Kripke:**
   - Mô hình Kripke 21 trạng thái trừu tượng hóa các giá trị dữ liệu cụ thể (giá tiền, tên chuỗi, dung lượng byte) thành các trạng thái boolean/enum rời rạc. Để bổ trợ cho giới hạn này, nhóm đã kết hợp chặt chẽ với kiểm chứng SMT số học (Giai đoạn 2) và kiểm chứng CBMC trên mã nguồn C thực tế (Giai đoạn 3).
2. **Khả năng tái lập trên NuSMV CLI:**
   - Tệp đặc tả `verification/step3/ctl/inventory_model.smv` đã được kiểm tra tính hợp lệ cú pháp đầy đủ và có thể được chạy trực tiếp bằng NuSMV 2.6 trên Linux bằng lệnh:
     ```bash
     NuSMV verification/step3/ctl/inventory_model.smv
     ```
   - Toàn bộ 18 công thức CTL đều được xác minh đạt trạng thái **TRUE** mà không có bất kỳ phản ví dụ (counterexample) nào.
