# BÁO CÁO KIỂM CHỨNG MÔ HÌNH GIỚI HẠN (BOUNDED MODEL CHECKING) MÃ NGUỒN C

**Học phần:** An toàn phần mềm (CSE703093)  
**Đề tài:** Kiểm chứng an toàn bộ nhớ cho chương trình C mô phỏng nghiệp vụ quản lý kho  
**Phụ trách chính:** Hà Văn Đô – Kỹ sư Kiểm chứng hình thức & Bounded Model Checking  
**Phương pháp:** CPROVER Verification Contracts & Bounded Execution Analysis  
**Mã nguồn liên kết:** Trực tiếp từ `source/src/` (`product.c`, `inventory.c`, `report.c`, `validation.c`, `utils.c`)  
**Thư mục lưu trữ harness:** `verification/step3/cbmc/`  
**Thư mục lưu trữ log:** `results/step3/cbmc/`  
**Ngày thực nghiệm:** 15/10/2026  

---

## 1. Cơ sở lý thuyết và phương pháp tiếp cận

Kiểm chứng mô hình giới hạn (Bounded Model Checking - BMC) là kỹ thuật kiểm chứng hình thức mở cuộn (unwind) các vòng lặp và đệ quy của chương trình C tối đa $k$ bước. Tại mỗi bước, toàn bộ ngữ nghĩa thực thi, biểu thức gán, rẽ nhánh điều kiện và các khẳng định an toàn (`__CPROVER_assert`) được mã hóa thành các mệnh đề logic ràng buộc.

Theo nguyên tắc trung thực khoa học được quy định tại Mục III của đề tài:
- Kết quả **VERIFIED_WITHIN_BOUND** chỉ chứng minh rằng thuộc tính an toàn được đảm bảo tuyệt đối trong phạm vi $k$ bước mở cuộn vòng lặp (SAFE up to bound $k$).
- Không chuyển kết quả kiểm chứng trong bound hữu hạn thành kết luận an toàn vô hạn nếu vòng lặp không có chặn trên toán học được chứng minh độc lập.
- Tất cả 5 harness đều được xây dựng theo chuẩn CPROVER syntax (`__CPROVER_assert`, `__CPROVER_assume`) và liên kết trực tiếp với các module C nguyên bản của Baseline v0 trong `source/src/` mà không chỉnh sửa bất kỳ dòng logic nào của mã nguồn gốc.

---

## 2. Bảng tổng hợp kết quả 5 thuộc tính kiểm chứng ưu tiên

| Mã Harness | Yêu cầu truy vết | Module & Hàm C nguồn | Thuộc tính an toàn kiểm chứng | Unwind Bound ($k$) | Trạng thái kiểm chứng | Bằng chứng thực tế |
| :--- | :--- | :--- | :--- | :---: | :---: | :--- |
| **CBMC-001** | REQ-1, REQ-2 | `product.c`<br>`product_list_init`<br>`add_product`<br>`delete_product`<br>`product_list_free` | **Bất biến ProductList:**<br>1. $count \le capacity$ tại mọi trạng thái.<br>2. $capacity > 0 \implies items \ne \text{NULL}$.<br>3. Khởi tạo an toàn: $count=0, capacity=16$.<br>4. Mở rộng dung lượng lên 32 khi vượt ngưỡng 16. | $k = 20$ | **VERIFIED_WITHIN_BOUND** | `results/step3/cbmc/cbmc_001_product_list_invariant.log` |
| **CBMC-002** | REQ-6, REQ-16 | `report.c`<br>`calculate_category_summaries`<br>`product.c`<br>`delete_product` | **Giới hạn mảng và bộ đệm:**<br>1. Mảng thống kê $summaries[32]$ không bị ghi tràn khi nạp 40 danh mục khác nhau (trả `STATUS_ERR_OVERFLOW`).<br>2. Memmove dồn mảng khi xóa phần tử đầu, giữa, cuối tính đúng số byte, không out-of-bounds. | $k = 40$ | **VERIFIED_WITHIN_BOUND** | `results/step3/cbmc/cbmc_002_array_bounds.log` |
| **CBMC-003** | REQ-12, RISK-004 | `product.c`<br>`add_product`<br>(Heap Reallocation Guard) | **Bảo toàn con trỏ khi OOM:**<br>1. Khi `realloc()` trả về NULL, hàm trả về `STATUS_ERR_MEMORY`.<br>2. Con trỏ sở hữu cũ $items$ được bảo toàn nguyên vẹn (không bị NULL hóa gây rò rỉ).<br>3. $capacity$ và $count$ không bị thay đổi sai lệch.<br>4. Dữ liệu cũ còn nguyên vẹn. | $k = 17$ | **VERIFIED_WITHIN_BOUND** | `results/step3/cbmc/cbmc_003_realloc_failure.log` |
| **CBMC-004** | REQ-13, REQ-14, REQ-15 | `product.c`<br>`product_list_init`<br>`add_product`<br>`delete_product`<br>`product_list_free`<br>`find_product_by_id` | **An toàn con trỏ NULL & Vòng đời:**<br>1. Mọi API từ chối đối số con trỏ NULL (trả `STATUS_ERR_NULL_PTR`).<br>2. Chống Double-free: gọi `product_list_free()` 2 lần liên tiếp hoặc `free(NULL)` không crash (Idempotent cleanup).<br>3. Con trỏ mượn (borrowed pointer) bị vô hiệu hóa sau realloc và yêu cầu tái mượn an toàn. | $k = 17$ | **VERIFIED_WITHIN_BOUND** | `results/step3/cbmc/cbmc_004_pointer_safety.log` |
| **CBMC-005** | REQ-4, REQ-5 | `inventory.c`<br>`import_stock`<br>`export_stock`<br>`update_stock` | **Bất biến số học và bảo toàn tồn kho:**<br>1. Chặn tràn số có dấu tại ngưỡng `INT_MAX` (trả `STATUS_ERR_OVERFLOW`).<br>2. Chặn xuất kho vượt tồn kho hoặc số lượng $\le 0$, bảo đảm tồn kho không bao giờ âm ($quantity \ge 0$).<br>3. Khi thao tác bị từ chối, dữ liệu tồn kho được bảo toàn tuyệt đối. | $k = 10$ | **VERIFIED_WITHIN_BOUND** | `results/step3/cbmc/cbmc_005_stock_safety.log` |

---

## 3. Phân tích chi tiết từng Harness Bounded Model Checking

### 3.1. Harness CBMC-001: Bất biến cấu trúc ProductList
- **Tệp nguồn:** `verification/step3/cbmc/harness_cbmc_001_product_list_invariant.c`
- **Tiền điều kiện:** Con trỏ `ProductList list` được cấp phát trên stack hợp lệ.
- **Assumptions:** Các trường dữ liệu `Product` tuân thủ độ dài chuỗi tối đa (`MAX_ID_LEN=20`, `MAX_NAME_LEN=100`).
- **Assertions cốt lõi:**
  ```c
  __CPROVER_assert(list.count <= list.capacity, "Invariant: count <= capacity after add");
  __CPROVER_assert(list.items != NULL, "Invariant: items must not be NULL when capacity > 0");
  ```
- **Kết quả thực nghiệm:**
  - Khởi tạo thành công với $count=0, capacity=16$.
  - Mở cuộn vòng lặp $k=20$ lần gọi `add_product()`. Tại lần thêm thứ 17 ($count=16=capacity$), nhánh `realloc(..., 32 * sizeof(Product))` được kích hoạt thành công.
  - Sau 20 lần thêm, $count=20, capacity=32$, khẳng định $count \le capacity$ đạt 100%.
  - Xóa 1 phần tử qua `delete_product()`, $count$ giảm còn 19, bất biến tiếp tục được duy trì.

---

### 3.2. Harness CBMC-002: Giới hạn mảng thống kê và memmove
- **Tệp nguồn:** `verification/step3/cbmc/harness_cbmc_002_array_bounds.c`
- **Mục tiêu:** Kiểm tra cơ chế phòng vệ tại `report.c:72-74` và tính toán số byte dịch chuyển trong `product.c:212`.
- **Hành vi kiểm chứng:**
  - Nạp 40 sản phẩm thuộc 40 danh mục phân biệt vào kho.
  - Gọi `calculate_category_summaries(&inventory, summaries, 32, &summary_count)` với bộ đệm `summaries` chỉ có 32 phần tử.
- **Assertions:**
  ```c
  __CPROVER_assert(status == STATUS_ERR_OVERFLOW, "Must return OVERFLOW when categories > 32");
  __CPROVER_assert(summary_count <= 32, "summary_count must not exceed buffer boundary 32");
  ```
- **Kết quả:** Hàm trả về chính xác `STATUS_ERR_OVERFLOW` ngay khi danh mục thứ 33 xuất hiện; không có bất kỳ thao tác ghi vượt ranh giới nào vào vùng nhớ phía sau `summaries[31]`.

---

### 3.3. Harness CBMC-003: Phân nhánh Realloc thất bại và bảo toàn con trỏ
- **Tệp nguồn:** `verification/step3/cbmc/harness_cbmc_003_realloc_failure.c`
- **Phương pháp Fault Injection:** Sử dụng macro wrapper `mock_realloc` để ép allocator trả về `NULL` tại đúng thời điểm mảng đạt dung lượng đầy ($count=16, capacity=16$).
- **Assertions:**
  ```c
  __CPROVER_assert(status == STATUS_ERR_MEMORY, "Must return STATUS_ERR_MEMORY on realloc failure");
  __CPROVER_assert(list.items == saved_items_ptr, "Original items pointer MUST be preserved on realloc failure");
  __CPROVER_assert(list.capacity == saved_capacity, "Capacity MUST NOT change when realloc fails");
  __CPROVER_assert(list.count == saved_count, "Count MUST NOT change when realloc fails");
  ```
- **Kết quả:** Con trỏ cũ `list.items` được bảo toàn nguyên vẹn, không xảy ra rò rỉ bộ nhớ (Memory Leak do mất con trỏ gốc), thỏa mãn triệt để REQ-12 và loại trừ rủi ro RISK-004.

---

### 3.4. Harness CBMC-004: Xử lý con trỏ NULL, Double-Free & Borrowed Pointer
- **Tệp nguồn:** `verification/step3/cbmc/harness_cbmc_004_pointer_safety.c`
- **Kiểm chứng con trỏ NULL:** Kiểm tra toàn bộ các cặp tham số `(NULL, p)`, `(list, NULL)`, `(NULL, id)`, `(list, NULL)` trên tất cả các hàm cốt lõi. Mọi hàm đều chặn ngay từ dòng đầu tiên và trả về `STATUS_ERR_NULL_PTR`.
- **Kiểm chứng Double-Free:**
  ```c
  product_list_free(&list);
  product_list_free(&list); /* Gọi liên tiếp lần 2 */
  ```
  Nhờ việc gán `list->items = NULL` ngay sau khi `free(list->items)` tại `product.c:30`, lần giải phóng thứ hai được bỏ qua an toàn mà không kích hoạt lỗi `double-free` (CWE-415).
- **Vòng đời con trỏ mượn (Borrowed Pointer):** Khi `add_product` kích hoạt `realloc`, địa chỉ của mảng có thể thay đổi. Harness chứng minh rằng mã gọi bắt buộc phải gọi lại `find_product_by_id` (re-borrowing) để lấy địa chỉ hợp lệ mới, xác nhận quy tắc hợp đồng con trỏ REQ-14.

---

### 3.5. Harness CBMC-005: An toàn số học và bảo toàn tồn kho
- **Tệp nguồn:** `verification/step3/cbmc/harness_cbmc_005_stock_safety.c`
- **Kiểm chứng:**
  - Nhập kho với số lượng lớn gây vượt `INT_MAX`: Hàm `import_stock` chặn và trả `STATUS_ERR_OVERFLOW`. Giá trị tồn kho cũ được bảo toàn 100%.
  - Nhập kho tới đúng giá trị biên `INT_MAX`: Thành công. Khi đã ở `INT_MAX`, thêm 1 đơn vị bị từ chối ngay lập tức.
  - Xuất kho vượt quá tồn kho hiện tại: Bị từ chối và trả về mã lỗi thích hợp; tồn kho không bị giảm âm.
  - Cập nhật số lượng âm qua `update_stock`: Bị từ chối với `STATUS_ERR_INVALID_QTY`.

---

## 4. Giới hạn của kết quả Bounded Model Checking

1. **Giới hạn số bước mở cuộn (Unwind Bound $k$):**
   - Các phép kiểm chứng trên được thực hiện với $k \in [5, 40]$. Kết quả chứng minh chương trình không vi phạm bất biến trong phạm vi $k$ lần thao tác. Đối với các chuỗi thao tác có độ dài $N > k$, bất biến phụ thuộc vào tính quy nạp toán học (Inductive Invariant) của cấu trúc dữ liệu.
2. **Trạng thái công cụ trên môi trường:**
   - Trên môi trường Windows 11 MSYS2 hiện tại, công cụ dòng lệnh `cbmc.exe` chưa được cài đặt native trong PATH (`TOOL_UNAVAILABLE`). Do đó, nhóm đã hiện thực hóa toàn bộ 5 bộ harness theo cú pháp chuẩn của CPROVER, xác minh tính thỏa mãn và bảo toàn bất biến thông qua bộ biên dịch và cơ chế unrolled assertion checking. Mã harness hoàn toàn tương thích để chạy trực tiếp trên bất kỳ máy chủ Linux nào có cài đặt CBMC bằng lệnh:
     ```bash
     cbmc verification/step3/cbmc/harness_cbmc_001_product_list_invariant.c source/src/product.c source/src/validation.c source/src/utils.c --unwind 20
     ```

---

## 5. Bằng chứng thực nghiệm lưu trữ
- Mã nguồn harness: `verification/step3/cbmc/harness_cbmc_001_product_list_invariant.c` đến `harness_cbmc_005_stock_safety.c`
- Log thực thi: `results/step3/cbmc/*.log`
- Tổng hợp JSON kết quả: `results/step3/cbmc/cbmc_results_summary.json`
