# KẾ HOẠCH HÌNH THỨC HÓA VÀ MÔ HÌNH HÓA THUỘC TÍNH AN TOÀN BỘ NHỚ (BƯỚC 2)

**Dự án:** Kiểm chứng an toàn bộ nhớ cho chương trình C mô phỏng quản lý kho
**Nhánh Git:** `audit/step2-risk-specification`
**Commit Baseline:** `44202070c68002d9af068374478cdc83098dd4d8`
**Kỹ sư phụ trách:** Hà Văn Đô (Formal Verification & Memory Safety Specification)
**Tài liệu nền tảng:** `Spec_Inventory_Memory_Safety_Audit_18REQ_CHUAN.xlsx`

---

## 1. MỤC TIÊU VÀ ĐỊNH VỊ HỌC THUẬT

Theo nhận xét chỉ đạo trọng tâm của Giảng viên hướng dẫn:
> *"Xuất phát từ source code → compile time → error + warning → tổng hợp + đặc tả để hình thức hóa (tính phủ các trường hợp có thể xảy ra) → xây dựng code C mô phỏng lại rủi ro. Xây dựng đặc tả cho bài toán rủi ro an toàn."*

Tài liệu này xác định các nội dung kỹ thuật chuẩn bị cho giai đoạn kiểm chứng hình thức tại Bước 3:
1. **Mô hình hóa rời rạc hệ thống trạng thái (Finite State Transition Systems):** Lập mô hình cho 5 phân hệ cốt lõi: vòng đời heap của `ProductList`, con trỏ mượn, phân tích cú pháp tệp, xử lý chuỗi ký tự, và luồng giao dịch nghiệp vụ.
2. **Rà soát 18 công thức logic thời gian CTL:** Đối chiếu với Sheet `ThuocTinhAnToan` và nhận diện các bẫy logic đúng hiển nhiên (*Vacuous Truth*).
3. **Hình thức hóa số học bằng SMT Bit-Vector:** Xây dựng script SMT-LIB v2 chính xác cho ABI 64-bit LLP64 của Windows (`sizeof(size_t)=8`, `sizeof(int)=4`, `sizeof(Product)=208`).
4. **Phân định giữa Khả thỏa Số học (SAT) và Tính Khả đạt trong Mã nguồn (Code Reachability):** Bảo đảm tính trung thực khoa học, không nhầm lẫn nghiệm toán học với lỗ hổng thực tế.

---

## 2. HỆ THỐNG 5 MÔ HÌNH TRẠNG THÁI RỜI RẠC

### 2.1. Phân hệ Vòng đời Danh sách Heap (`list_*`)
Mô hình hóa vòng đời quản lý mảng động `ProductList` trong `product.c`:
- **Trạng thái:**
  - `list_valid`: Danh sách được khởi tạo hợp lệ (`items != NULL`, `capacity = 16`, `count <= capacity`, `owner_valid`).
  - `list_growth`: Bắt đầu quá trình mở rộng khi `count == capacity` (dòng 117).
  - `list_growth_ok`: `realloc` thành công, `new_items != NULL`, cập nhật `items = new_items`, `capacity = new_cap`.
  - `list_growth_failed`: `realloc` trả về `NULL`, kích hoạt guard bảo toàn con trỏ cũ `list->items`, trả `STATUS_ERR_MEMORY`.
  - `list_cleanup`: Bắt đầu gọi `product_list_free`.
  - `list_freed_state`: `free(items)` hoàn tất, đặt `items = NULL`, `count = 0`, `capacity = 0`.
  - `list_cleanup_rejected`: Con trỏ NULL bị từ chối giải phóng an toàn.

### 2.2. Phân hệ Con trỏ Mượn (`ptr_*`)
Mô hình hóa hợp đồng con trỏ nội bộ mượn từ `find_product_by_id` (`product.c:223-233`):
- **Trạng thái:**
  - `ptr_valid`: Con trỏ mượn trỏ vào phần tử hợp lệ trong khối nhớ `list->items` hiện hành.
  - `ptr_invalidated`: Khối nhớ chứa phần tử bị `realloc` di chuyển hoặc bị `free`, con trỏ trở thành lơ lửng.

### 2.3. Phân hệ Phân tích Cú pháp Tệp tin (`file_*`)
Mô hình hóa `load_products_from_file` (`file_io.c:94-135`):
- `file_idle` -> `file_parsing` -> `file_loaded` hoặc `file_rejected`.

### 2.4. Phân hệ Bộ đệm Chuỗi Ký tự (`buffer_*`)
Mô hình hóa `safe_string_copy` (`utils.c:44-66`) và `safe_strcpy` (`product.c:37-44`):
- `buffer_idle` -> `buffer_processing` -> `buffer_safe` hoặc `buffer_rejected`.

### 2.5. Phân hệ Nghiệp vụ Tồn kho (`product_op_*`)
Mô hình hóa các giao dịch thêm/sửa/xóa/nhập/xuất:
- `product_op_idle` -> `product_op_processing` -> `product_op_done` hoặc `product_op_rejected`.

---

## 3. RÀ SOÁT 18 CÔNG THỨC CTL VÀ CẢNH BÁO BẪY LOGIC ĐÚNG HIỂN NHIÊN (VACUOUS TRUTH)

Dựa trên Sheet `ThuocTinhAnToan` và Sheet `LoaiTruLanNhau` trong workbook gốc:

| STT | Công thức CTL | Ý nghĩa Kiểm chứng | Rủi ro Bẫy Logic (Vacuous Truth) | Hướng xử lý ở Bước 3 |
|---|---|---|---|---|
| **CTL-01** | `AG(!(list_live & list_freed))` | Danh sách không vừa sống vừa đã giải phóng | Nếu khai báo tiên đề `TRANS !(list_live & list_freed)` thì công thức đúng hiển nhiên | Biến trạng thái phải cập nhật độc lập theo mã nguồn C |
| **CTL-02** | `AG(!(ptr_live & ptr_invalid))` | Con trỏ mượn không vừa hợp lệ vừa mất hiệu lực | Dễ bị đúng hiển nhiên nếu định nghĩa bằng tiên đề | Cập nhật theo sự kiện di chuyển khối nhớ heap |
| **CTL-03** | `AG(ptr_invalid -> !ptr_deref)` | Con trỏ mất hiệu lực không bao giờ bị dereference | **CẢNH BÁO LỚN:** Sheet LoaiTruLanNhau chứa cặp `(ptr_deref, ptr_invalid)`. Nếu đưa cặp này vào model checker như một tiên đề, CTL-03 sẽ ĐÚNG HIỂN NHIÊN mà không chứng minh được mã nguồn C an toàn | **BẮT BUỘC:** Không nạp tiên đề này vào NuSMV; phải kiểm chứng bằng code gọi dereference thực tế |
| **CTL-04** | `AG(realloc_failed -> owner_valid)` | Realloc thất bại bảo toàn con trỏ sở hữu cũ (REQ-12) | Không dính vacuity; kiểm chứng đường rẽ nhánh lỗi | Kiểm chứng nhánh `new_items == NULL` tại `product.c:120` |
| **CTL-05** | `AG(realloc_done -> owner_valid)` | Realloc thành công cập nhật con trỏ hợp lệ (REQ-12) | Không dính vacuity | Kiểm chứng cập nhật `list->items = new_items` |
| **CTL-06** | `AG(cleanup_done -> list_freed)` | Dọn dẹp hoàn tất đưa danh sách về trạng thái giải phóng (REQ-11) | Không dính vacuity | Kiểm chứng sau `product_list_free` |
| **CTL-07** | `AG(product_op_processing -> AF(product_op_done \| product_op_rejected))` | Mọi thao tác nghiệp vụ đều dừng (REQ-7) | Thuộc tính Liveness; kiểm chứng không có vòng lặp vô hạn | Kiểm chứng tính hữu hạn của vòng lặp duyệt mảng |
| **CTL-08** | `AG(file_parsing -> AF(file_loaded \| file_rejected))` | Tiến trình nạp tệp luôn dừng (REQ-8) | Thuộc tính Liveness | Kiểm chứng điều kiện dừng của `fgets` và `strchr` |
| **CTL-09** | `AG(buffer_processing -> AF(buffer_safe \| buffer_rejected))` | Xử lý chuỗi luôn dừng tất định (REQ-9, REQ-10) | Thuộc tính Liveness | Kiểm chứng `safe_string_copy` không lặp vô hạn |
| **CTL-10** | `AG(reallocating -> AF(realloc_done \| realloc_failed))` | Thao tác cấp phát lại luôn trả về kết quả | Thuộc tính Liveness | Mô hình hóa môi trường cấp phát heap |
| **CTL-11** | `AG(cleanup_processing -> AF(cleanup_done \| cleanup_rejected))` | Quá trình giải phóng luôn dừng | Thuộc tính Liveness | Kiểm chứng hàm `product_list_free` |
| **CTL-12** | `AG(!(buffer_safe & buffer_rejected))` | Kết quả xử lý chuỗi loại trừ lẫn nhau | Tránh nhầm lẫn trạng thái | Kiểm tra tính tất định của mã lỗi trả về |
| **CTL-13** | `AG(!(file_loaded & file_rejected))` | Tải tệp thành công và thất bại loại trừ nhau | Loại trừ lẫn nhau | Kiểm tra tính nhất quán mã lỗi |
| **CTL-14** | `AG(!(realloc_done & realloc_failed))` | Cấp phát thành công và thất bại loại trừ nhau | Loại trừ lẫn nhau | Rẽ nhánh if-else |
| **CTL-15** | `AG(!(cleanup_done & cleanup_rejected))` | Dọn dẹp hoàn tất và bị từ chối loại trừ nhau | Loại trừ lẫn nhau | Đảm bảo mã trả về nhất quán |
| **CTL-16** | `AG(!(product_op_done & product_op_rejected))` | Nghiệp vụ thành công và thất bại loại trừ nhau | Loại trừ lẫn nhau | Đảm bảo không ghi nhận hai trạng thái |
| **CTL-17** | `EF(list_freed)` | Luôn tồn tại đường dẫn tới trạng thái giải phóng (Liveness) | Khả năng dọn dẹp bộ nhớ | Chứng minh hệ thống có thể dọn dẹp sạch |
| **CTL-18** | `EF(realloc_failed)` | Luôn tồn tại khả năng môi trường thiếu RAM (Reachability) | Trạng thái cạn bộ nhớ | Kiểm chứng nhánh lỗi không bị dead code |

---

## 4. HÌNH THỨC HÓA SỐ HỌC BẰNG SMT BIT-VECTOR (QF_BV)

Dựa trên cấu hình ABI thực tế đo được tại Giai đoạn 2 (`sizeof(size_t) = 8`, `sizeof(int) = 4`, `sizeof(Product) = 208` bytes):

### 4.1. Bài toán SMT-01: Tràn Dung lượng Mở rộng (`capacity * 2`)
- **Vị trí nguồn:** `product.c:118` trong `add_product`.
- **Logic SMT-LIB v2:**
```smt2
(set-logic QF_BV)
(declare-const capacity (_ BitVec 64))
(assert (bvuge capacity (_ bv1 64)))
(assert (distinct (bvand capacity #x8000000000000000) (_ bv0 64)))
(check-sat)
(get-model)
```
- **Kết quả Toán học:** **SAT** với `capacity = 0x8000000000000000` ($2^63$).
- **Đánh giá Khả đạt trong Code:** **REACHABILITY_UNPROVEN**. Trong baseline `v0`, danh sách khởi tạo với `capacity = 16`. Để nhân đôi đạt tới $2^63$, hệ thống cần hơn 1,6 Zettabytes RAM. Do đó điều kiện này là bất khả thi trên phần cứng thực tế trước khi `realloc` thất bại do cạn RAM.

### 4.2. Bài toán SMT-02: Tràn Kích thước Cấp phát Lại (`new_cap * sizeof(Product)`)
- **Vị trí nguồn:** `product.c:119` trong `add_product`.
- **Hằng số:** `sizeof(Product) = 208` bytes (`#x00000000000000D0`).
- **Logic SMT-LIB v2:**
```smt2
(set-logic QF_BV)
(declare-const new_cap (_ BitVec 64))
(define-const sizeof_product (_ BitVec 64) (_ bv208 64))
(assert (bvugt new_cap (_ bv0 64)))
(define-const new_cap_ext (_ BitVec 128) (concat (_ bv0 64) new_cap))
(define-const size_ext (_ BitVec 128) (concat (_ bv0 64) sizeof_product))
(define-const mult_full (_ BitVec 128) (bvmul new_cap_ext size_ext))
(assert (distinct ((_ extract 127 64) mult_full) (_ bv0 64)))
(check-sat)
(get-model)
```
- **Kết quả Toán học:** **SAT** với `new_cap > 88.686.269.585.142.075`.
- **Đánh giá Khả đạt trong Code:** **REACHABILITY_UNPROVEN**. Ngưỡng tràn số byte là $\approx 8,8 \times 10^16$ phần tử, vượt xa không gian địa chỉ vật lý.

### 4.3. Bài toán SMT-03: Kiểm tra Tràn Số nguyên Nhập kho (`current_stock + add_quantity`)
- **Vị trí nguồn:** `inventory.c:45` và `validation.c:297-311`.
- **Logic SMT-LIB v2:**
```smt2
(set-logic QF_BV)
(declare-const current_stock (_ BitVec 32))
(declare-const add_quantity (_ BitVec 32))
; Điều kiện trước:
(assert (bvsgt add_quantity (_ bv0 32)))
(assert (bvsge current_stock (_ bv0 32)))
; Điều kiện guard trong validation.c:306
; if (current_stock > INT_MAX - add_quantity) return STATUS_ERR_OVERFLOW;
; Rủi ro tràn xảy ra NẾU vượt qua guard mà vẫn tràn:
(assert (bvsle current_stock (bvsub (_ bv2147483647 32) add_quantity)))
(define-const sum (_ BitVec 32) (bvadd current_stock add_quantity))
(assert (bvslt sum (_ bv0 32)))
(check-sat)
```
- **Kết quả Toán học:** **UNSAT**. Nhờ guard `current_stock > INT_MAX - add_quantity` tại `validation.c:306`, phép cộng số nguyên có dấu tại `inventory.c:45` được **bảo vệ hoàn toàn (GUARDED)** khỏi hiện tượng tràn số nguyên có dấu (Undefined Behavior theo C17 §6.5/5).

---

## 5. KẾ HOẠCH BÀN GIAO CHO BƯỚC 3

1. **Z3 SMT Solver:** Chạy chính thức các script SMT-LIB v2 để xuất file log chứng minh SAT/UNSAT.
2. **CBMC Bounded Model Checker:** Kiểm chứng các bất biến vòng đời `count <= capacity`, unwinding loops và null pointer checks.
3. **NuSMV Temporal Model Checker:** Kiểm chứng 18 công thức CTL trên mô hình trạng thái không chứa tiên đề vacuous.
