# Bản Đặc tả Kỹ thuật Toàn diện 18 Yêu cầu An toàn Bộ nhớ (Requirement Specifications - Step 2)

## 1. Tổng quan và Căn cứ Học thuật
Tài liệu này chuẩn hóa và chi tiết hóa toàn bộ **18 Yêu cầu (REQ-1 đến REQ-18)** xuất phát từ hồ sơ đặc tả gốc của dự án (`Spec_Inventory_Memory_Safety_Audit_18REQ_CHUAN.xlsx`).
Mỗi yêu cầu được hình thức hóa thành các thành phần toán học và kỹ thuật phần mềm:
- **Tiền điều kiện (Preconditions)**
- **Hậu điều kiện (Postconditions)**
- **Bất biến an toàn (Safety Invariant) / Thuộc tính sống (Liveness Property)**
- **Điều kiện vi phạm (Unsafe Condition)**
- **Điều kiện biên (Boundary Conditions)**
- **Cơ chế phòng vệ hiện có (Existing Guards)**
- **Kỹ thuật kiểm chứng đề xuất (Verification Technique)**

---

## 2. Bảng Ma trận Truy vết Yêu cầu (18 REQ Traceability Matrix)

| REQ ID | Danh mục | Mức độ | Tiêu đề tóm tắt | Module C liên quan | Hàm C trọng yếu | Kỹ thuật kiểm chứng | Test Case | Risk ID |
|:---:|:---:|:---:|---|---|---|---|:---:|:---:|
| **REQ-1** | `safety` | **HIGH** | Bất biến cấu trúc danh sách sản phẩm (ProductList Structure Invariant) | `models.h, product.c, inventory.c` | `product_list_init, add_product, delete_product, product_list_free` | CBMC Invariant Assertion / ASan | `TC-1` | `RISK-001, RISK-010` |
| **REQ-2** | `safety` | **HIGH** | Chống tràn số khi nhân đôi sức chứa mảng (Capacity Doubling Overflow Guard) | `product.c` | `add_product` | SMT Bit-Vector Solver (Z3) / CBMC Arithmetic Check | `TC-2` | `RISK-001` |
| **REQ-3** | `safety` | **HIGH** | Chống tràn số kích thước cấp phát lại (Reallocation Size Overflow Guard) | `product.c` | `add_product` | SMT Bit-Vector Solver (Z3) / CBMC / C Simulation Harness (SIM-001) | `TC-3` | `RISK-002` |
| **REQ-4** | `safety` | **HIGH** | Chống tràn số nguyên khi nhập kho (Stock Import Addition Overflow Guard) | `inventory.c, validation.c` | `import_stock, check_addition_overflow` | SMT Bit-Vector / UBSan / Unit Boundary Test | `TC-4` | `RISK-007` |
| **REQ-5** | `safety` | **HIGH** | Chống âm kho khi xuất kho (Stock Export Underflow Guard) | `inventory.c, validation.c` | `export_stock, validate_export_quantity` | CBMC Boundary Invariant / Unit Test | `TC-5` | `RISK-008` |
| **REQ-6** | `safety` | **HIGH** | Bảo đảm chỉ số truy cập mảng sản phẩm (Array Index Boundary Safety) | `product.c, report.c, file_io.c, main.c` | `find_product_by_id, delete_product, print_product_list, calculate_inventory_summary` | CBMC Array Bounds Check / ASan | `TC-6` | `RISK-009` |
| **REQ-7** | `liveness` | **HIGH** | Tính dừng của các thao tác nghiệp vụ kho (Operation Termination Liveness) | `main.c, product.c, inventory.c` | `add_product, update_product, delete_product, import_stock, export_stock` | CTL Model Checking (NuSMV/CBMC) / Loop Termination Proof | `TC-7` | `NONE` |
| **REQ-8** | `liveness` | **HIGH** | Tính dừng của quá trình đọc và phân tích tệp (Parser Termination Liveness) | `file_io.c` | `load_products_from_file, parse_product_line` | CTL Model Checking / Corpus Testing with malformed EOF | `TC-8` | `RISK-006` |
| **REQ-9** | `memory` | **HIGH** | Giới hạn chuỗi và ký tự kết thúc NUL (Fixed-Length String Boundary & NUL-Termination) | `models.h, validation.c, utils.c, product.c` | `validate_product_id, validate_product_name, validate_category, validate_unit, safe_strcpy, safe_string_copy` | ASan Boundary Testing / CBMC String Invariants | `TC-9` | `RISK-004` |
| **REQ-10** | `memory` | **HIGH** | Kiểm soát an toàn bộ đệm nhập liệu dòng (Input Buffer Boundary and Truncation Guard) | `utils.c, file_io.c, main.c` | `safe_read_line, load_products_from_file` | ASan / Long Line Fuzzing | `TC-10` | `RISK-006` |
| **REQ-11** | `memory` | **HIGH** | Quản lý vòng đời và chống rò rỉ bộ nhớ (Allocation Lifecycle & Leak Freedom) | `product.c, inventory.c, main.c` | `product_list_init, product_list_free, inventory_init, inventory_free` | LSan (LeakSanitizer) / Valgrind / ASan | `TC-11` | `NONE` |
| **REQ-12** | `memory` | **HIGH** | Bảo toàn khối nhớ khi realloc thất bại (Realloc Failure Ownership Preservation) | `product.c` | `add_product` | Fault Injection Harness / CBMC | `TC-12` | `NONE` |
| **REQ-13** | `memory` | **HIGH** | Chống giải phóng bộ nhớ hai lần (Double Free Defense Invariant) | `product.c, inventory.c` | `product_list_free, inventory_free` | ASan Double Free Test / CBMC | `TC-13` | `NONE` |
| **REQ-14** | `memory` | **HIGH** | Chống sử dụng con trỏ mất hiệu lực sau realloc hoặc free (Dangling Pointer Safety) | `product.c, inventory.c` | `find_product_by_id, find_product, find_product_mutable, add_product` | ASan Dangling Pointer Harness / CBMC Pointer Invalidation Model | `TC-14` | `RISK-003` |
| **REQ-15** | `memory` | **HIGH** | Kiểm tra phòng thủ con trỏ NULL tại giao diện API (Defensive NULL Pointer Contract) | `product.h, inventory.h, file_io.h, validation.h, utils.h, report.h` | `Tất cả các hàm public nhận tham số con trỏ` | CBMC NULL Pointer Analysis / ASan NULL Harness (test_cwe476) | `TC-15` | `RISK-010` |
| **REQ-16** | `memory` | **HIGH** | Giới hạn kích thước mảng phụ trợ và dịch chuyển memmove (Auxiliary Buffer Bounds Safety) | `file_io.c, product.c, report.c, utils.c` | `parse_product_line, search_products_by_name, calculate_category_summaries, delete_product, split_line` | ASan / CBMC Array Bounds / Boundary Tests | `TC-6, TC-16` | `RISK-005, RISK-009` |
| **REQ-17** | `functional` | **HIGH** | Đặc tả cú pháp tệp dữ liệu kho và từ chối an toàn (File Format Parser Specification) | `file_io.c` | `load_products_from_file, parse_product_line` | Corpus Testing / Fuzzing / Round-trip Save-Load Test | `TC-16` | `NONE` |
| **REQ-18** | `functional` | **HIGH** | Xác thực số lượng và đơn giá hợp lệ (Numeric Range and Finite Float Invariant) | `validation.c, file_io.c, report.c` | `validate_price, validate_quantity, validate_price_str, validate_quantity_str, parse_int, parse_price` | UBSan / Numeric Boundary Unit Test | `TC-17` | `NONE` |

---

## 3. Đặc tả Chi tiết Từng Yêu cầu (REQ-1 đến REQ-18)

### REQ-1: Bất biến cấu trúc danh sách sản phẩm (ProductList Structure Invariant)
- **Nội dung nguyên văn (Verbatim)**:
  > *"ProductList luôn thỏa count <= capacity; khi count > 0 thì items khác NULL và trỏ tới vùng nhớ còn hiệu lực."*
- **Danh mục & Ưu tiên**: `SAFETY` | Mức độ: **HIGH**
- **Phạm vi áp dụng**: Mô hình mảng động quản lý bộ nhớ duy nhất
- **Module & Hàm C liên quan**: `models.h, product.c, inventory.c` - Các hàm: `product_list_init, add_product, delete_product, product_list_free`
- **Kiểu dữ liệu & Đối tượng bộ nhớ**: `ProductList, Product*, size_t` | `ProductList list, Heap buffer list.items`
- **Tiền điều kiện (Preconditions)**: `list != NULL`
- **Hậu điều kiện (Postconditions)**: `list->count <= list->capacity && (list->count == 0 || list->items != NULL)`
- **Bất biến an toàn / Liveness**: `count <= capacity && (count > 0 ==> items != NULL)`
- **Điều kiện vi phạm (Unsafe State)**: `count > capacity || (count > 0 && items == NULL)`
- **Điều kiện biên (Boundaries)**: count == 0, count == capacity, capacity == INITIAL_CAPACITY (16)
- **Giả định mô hình (Assumptions)**: Bộ nhớ heap được cấp phát và quản lý độc quyền qua ProductList
- **Cơ chế phòng thủ hiện có trong mã v0**: product_list_init kiểm tra malloc; add_product kiểm tra count >= capacity trước realloc
- **Kỹ thuật kiểm chứng đề xuất**: **CBMC Invariant Assertion / ASan** (Bounded k=32 thao tác)
- **Phản ví dụ kỳ vọng (Counterexample)**: Hàm can thiệp trực tiếp giảm capacity mà không giảm count
- **Test case liên kết**: `TC-1`
- **Mã rủi ro liên kết**: `RISK-001, RISK-010`
- **Bằng chứng cần tạo ở Bước 3**: `CBMC assertion verification log / ASan memory invariant harness`
- **Trạng thái đặc tả**: `SPECIFIED_COMPLETE`

### REQ-2: Chống tràn số khi nhân đôi sức chứa mảng (Capacity Doubling Overflow Guard)
- **Nội dung nguyên văn (Verbatim)**:
  > *"Trước khi mở rộng ProductList bằng capacity * 2, phải kiểm tra capacity <= SIZE_MAX / 2 để tránh tràn số size_t."*
- **Danh mục & Ưu tiên**: `SAFETY` | Mức độ: **HIGH**
- **Phạm vi áp dụng**: Cơ chế mở rộng mảng động (Heap Growth)
- **Module & Hàm C liên quan**: `product.c` - Các hàm: `add_product`
- **Kiểu dữ liệu & Đối tượng bộ nhớ**: `size_t` | `list->capacity`
- **Tiền điều kiện (Preconditions)**: `list != NULL && list->count >= list->capacity`
- **Hậu điều kiện (Postconditions)**: `new_cap == (list->capacity == 0 ? INITIAL_CAPACITY : list->capacity * 2) && new_cap >= list->capacity`
- **Bất biến an toàn / Liveness**: `capacity <= SIZE_MAX / 2`
- **Điều kiện vi phạm (Unsafe State)**: `capacity > SIZE_MAX / 2 (gây wrap-around thành số nhỏ hơn)`
- **Điều kiện biên (Boundaries)**: capacity == 0, capacity == SIZE_MAX / 2, capacity == SIZE_MAX
- **Giả định mô hình (Assumptions)**: ABI 64-bit (SIZE_MAX = 18446744073709551615)
- **Cơ chế phòng thủ hiện có trong mã v0**: Chưa có guard rõ ràng trước phép nhân capacity * 2; hiện tại dựa vào giới hạn RAM thực tế
- **Kỹ thuật kiểm chứng đề xuất**: **SMT Bit-Vector Solver (Z3) / CBMC Arithmetic Check** (Unbounded bit-vector 64-bit)
- **Phản ví dụ kỳ vọng (Counterexample)**: capacity = 0x8000000000000000 -> capacity * 2 = 0
- **Test case liên kết**: `TC-2`
- **Mã rủi ro liên kết**: `RISK-001`
- **Bằng chứng cần tạo ở Bước 3**: `Z3 proof script (capacity_growth.smt2) / CBMC overflow check`
- **Trạng thái đặc tả**: `SPECIFIED_COMPLETE`

### REQ-3: Chống tràn số kích thước cấp phát lại (Reallocation Size Overflow Guard)
- **Nội dung nguyên văn (Verbatim)**:
  > *"Trước realloc, phải kiểm tra new_cap <= SIZE_MAX / sizeof(Product) để phép nhân kích thước cấp phát không bị tràn."*
- **Danh mục & Ưu tiên**: `SAFETY` | Mức độ: **HIGH**
- **Phạm vi áp dụng**: Cấp phát lại bộ nhớ heap qua realloc
- **Module & Hàm C liên quan**: `product.c` - Các hàm: `add_product`
- **Kiểu dữ liệu & Đối tượng bộ nhớ**: `size_t, Product` | `Heap buffer list->items`
- **Tiền điều kiện (Preconditions)**: `new_cap > 0 && sizeof(Product) == 208`
- **Hậu điều kiện (Postconditions)**: `alloc_bytes == new_cap * sizeof(Product) && alloc_bytes / sizeof(Product) == new_cap`
- **Bất biến an toàn / Liveness**: `new_cap <= SIZE_MAX / sizeof(Product)`
- **Điều kiện vi phạm (Unsafe State)**: `new_cap > SIZE_MAX / sizeof(Product)`
- **Điều kiện biên (Boundaries)**: new_cap == SIZE_MAX / 208 (= 88686269585142075)
- **Giả định mô hình (Assumptions)**: sizeof(Product) = 208 bytes trên MSYS2/GCC 16.1.0 (LLP64)
- **Cơ chế phòng thủ hiện có trong mã v0**: Chưa có guard kiểm tra new_cap <= SIZE_MAX / sizeof(Product) trước realloc
- **Kỹ thuật kiểm chứng đề xuất**: **SMT Bit-Vector Solver (Z3) / CBMC / C Simulation Harness (SIM-001)** (Unbounded bit-vector 64-bit / 32-bit comparison)
- **Phản ví dụ kỳ vọng (Counterexample)**: new_cap = 88686269585142076 -> wrap-around thành kích thước nhỏ
- **Test case liên kết**: `TC-3`
- **Mã rủi ro liên kết**: `RISK-002`
- **Bằng chứng cần tạo ở Bước 3**: `Z3 proof script (allocation_size.smt2) / SIM-001 simulation log`
- **Trạng thái đặc tả**: `SPECIFIED_COMPLETE`

### REQ-4: Chống tràn số nguyên khi nhập kho (Stock Import Addition Overflow Guard)
- **Nội dung nguyên văn (Verbatim)**:
  > *"Trong import_stock, chỉ cộng khi add_quantity > 0, tồn kho hiện tại hợp lệ và current_stock <= INT_MAX - add_quantity; nếu không phải từ chối trước khi cập nhật."*
- **Danh mục & Ưu tiên**: `SAFETY` | Mức độ: **HIGH**
- **Phạm vi áp dụng**: Nghiệp vụ nhập kho hàng
- **Module & Hàm C liên quan**: `inventory.c, validation.c` - Các hàm: `import_stock, check_addition_overflow`
- **Kiểu dữ liệu & Đối tượng bộ nhớ**: `int` | `product->quantity`
- **Tiền điều kiện (Preconditions)**: `product != NULL && add_quantity > 0 && current_stock >= 0`
- **Hậu điều kiện (Postconditions)**: `product->quantity == current_stock + add_quantity`
- **Bất biến an toàn / Liveness**: `current_stock <= INT_MAX - add_quantity`
- **Điều kiện vi phạm (Unsafe State)**: `current_stock > INT_MAX - add_quantity`
- **Điều kiện biên (Boundaries)**: current_stock = INT_MAX - 1, add_quantity = 2; current_stock = INT_MAX
- **Giả định mô hình (Assumptions)**: int là kiểu bù 2 32-bit (INT_MAX = 2147483647)
- **Cơ chế phòng thủ hiện có trong mã v0**: check_addition_overflow kiểm tra current_stock > INT_MAX - add_quantity và trả STATUS_ERR_OVERFLOW
- **Kỹ thuật kiểm chứng đề xuất**: **SMT Bit-Vector / UBSan / Unit Boundary Test** (Signed 32-bit integer arithmetic)
- **Phản ví dụ kỳ vọng (Counterexample)**: Không có trong baseline vì guard đã chặn thành công
- **Test case liên kết**: `TC-4`
- **Mã rủi ro liên kết**: `RISK-007`
- **Bằng chứng cần tạo ở Bước 3**: `results/baseline/smoke-test.log (Test 8 PASS với mã -6) / SMT proof`
- **Trạng thái đặc tả**: `SPECIFIED_COMPLETE`

### REQ-5: Chống âm kho khi xuất kho (Stock Export Underflow Guard)
- **Nội dung nguyên văn (Verbatim)**:
  > *"Trong export_stock, số lượng xuất phải > 0 và <= tồn kho hiện tại; không được tạo ra tồn kho âm hoặc cập nhật khi điều kiện không thỏa."*
- **Danh mục & Ưu tiên**: `SAFETY` | Mức độ: **HIGH**
- **Phạm vi áp dụng**: Nghiệp vụ xuất kho hàng
- **Module & Hàm C liên quan**: `inventory.c, validation.c` - Các hàm: `export_stock, validate_export_quantity`
- **Kiểu dữ liệu & Đối tượng bộ nhớ**: `int` | `product->quantity`
- **Tiền điều kiện (Preconditions)**: `product != NULL && current_stock >= 0`
- **Hậu điều kiện (Postconditions)**: `product->quantity == current_stock - export_quantity >= 0`
- **Bất biến an toàn / Liveness**: `export_quantity > 0 && export_quantity <= current_stock`
- **Điều kiện vi phạm (Unsafe State)**: `export_quantity <= 0 || export_quantity > current_stock`
- **Điều kiện biên (Boundaries)**: export_quantity == current_stock; export_quantity == current_stock + 1
- **Giả định mô hình (Assumptions)**: Số lượng tồn kho là đại lượng không âm trong logic nghiệp vụ
- **Cơ chế phòng thủ hiện có trong mã v0**: validate_export_quantity kiểm tra export_quantity > current_stock trả STATUS_ERR_INSUFFICIENT_STOCK
- **Kỹ thuật kiểm chứng đề xuất**: **CBMC Boundary Invariant / Unit Test** (Mọi giá trị int)
- **Phản ví dụ kỳ vọng (Counterexample)**: Không có trong baseline vì guard đã chặn thành công
- **Test case liên kết**: `TC-5`
- **Mã rủi ro liên kết**: `RISK-008`
- **Bằng chứng cần tạo ở Bước 3**: `results/baseline/smoke-test.log (Test 10 PASS với mã -5)`
- **Trạng thái đặc tả**: `SPECIFIED_COMPLETE`

### REQ-6: Bảo đảm chỉ số truy cập mảng sản phẩm (Array Index Boundary Safety)
- **Nội dung nguyên văn (Verbatim)**:
  > *"Mọi truy cập ProductList.items[i] trong thêm/sửa/xóa/tìm kiếm/báo cáo phải thỏa i < count <= capacity và items còn hiệu lực."*
- **Danh mục & Ưu tiên**: `SAFETY` | Mức độ: **HIGH**
- **Phạm vi áp dụng**: Các thao tác lặp và đánh chỉ số trên mảng `items`
- **Module & Hàm C liên quan**: `product.c, report.c, file_io.c, main.c` - Các hàm: `find_product_by_id, delete_product, print_product_list, calculate_inventory_summary`
- **Kiểu dữ liệu & Đối tượng bộ nhớ**: `size_t, Product*` | `Heap array list->items[i]`
- **Tiền điều kiện (Preconditions)**: `list != NULL && list->items != NULL`
- **Hậu điều kiện (Postconditions)**: `Truy cập tại chỉ số i luôn nằm trong vùng nhớ heap hợp lệ`
- **Bất biến an toàn / Liveness**: `0 <= i < count <= capacity`
- **Điều kiện vi phạm (Unsafe State)**: `i >= count || i >= capacity`
- **Điều kiện biên (Boundaries)**: i == 0, i == count - 1, count == 0
- **Giả định mô hình (Assumptions)**: Vòng lặp chuẩn for (size_t i = 0; i < list->count; ++i)
- **Cơ chế phòng thủ hiện có trong mã v0**: Tất cả vòng lặp đều giới hạn bởi i < list->count
- **Kỹ thuật kiểm chứng đề xuất**: **CBMC Array Bounds Check / ASan** (Toàn bộ 7 vòng lặp duyệt `items`)
- **Phản ví dụ kỳ vọng (Counterexample)**: Thao tác xóa memmove với chỉ số vượt quá count
- **Test case liên kết**: `TC-6`
- **Mã rủi ro liên kết**: `RISK-009`
- **Bằng chứng cần tạo ở Bước 3**: `CBMC unwinding assertion bounds log / ASan execution log`
- **Trạng thái đặc tả**: `SPECIFIED_COMPLETE`

### REQ-7: Tính dừng của các thao tác nghiệp vụ kho (Operation Termination Liveness)
- **Nội dung nguyên văn (Verbatim)**:
  > *"Với đầu vào hữu hạn và thao tác I/O trả về, mọi thao tác thêm/sửa/xóa/nhập/xuất đã bắt đầu phải kết thúc ở trạng thái thành công hoặc từ chối có kiểm soát; không lặp vô hạn."*
- **Danh mục & Ưu tiên**: `LIVENESS` | Mức độ: **HIGH**
- **Phạm vi áp dụng**: Vòng đời xử lý nghiệp vụ đơn lẻ
- **Module & Hàm C liên quan**: `main.c, product.c, inventory.c` - Các hàm: `add_product, update_product, delete_product, import_stock, export_stock`
- **Kiểu dữ liệu & Đối tượng bộ nhớ**: `int (status code)` | `State machine transitions`
- **Tiền điều kiện (Preconditions)**: `product_op_processing == true`
- **Hậu điều kiện (Postconditions)**: `product_op_done || product_op_rejected`
- **Bất biến an toàn / Liveness**: `AG(product_op_processing -> AF(product_op_done | product_op_rejected))`
- **Điều kiện vi phạm (Unsafe State)**: `EG(product_op_processing && !product_op_done && !product_op_rejected)`
- **Điều kiện biên (Boundaries)**: Vòng lặp tuyến tính duyệt mảng N phần tử
- **Giả định mô hình (Assumptions)**: Hệ thống đơn luồng (Single-threaded), không có khóa chết (deadlock)
- **Cơ chế phòng thủ hiện có trong mã v0**: Mọi thuật toán đều là vòng lặp hữu hạn for/while có bước nhảy xác định
- **Kỹ thuật kiểm chứng đề xuất**: **CTL Model Checking (NuSMV/CBMC) / Loop Termination Proof** (State transition graph)
- **Phản ví dụ kỳ vọng (Counterexample)**: Vòng lặp while không tăng biến đếm
- **Test case liên kết**: `TC-7`
- **Mã rủi ro liên kết**: `NONE`
- **Bằng chứng cần tạo ở Bước 3**: `Model checking proof log / termination analysis`
- **Trạng thái đặc tả**: `SPECIFIED_COMPLETE`

### REQ-8: Tính dừng của quá trình đọc và phân tích tệp (Parser Termination Liveness)
- **Nội dung nguyên văn (Verbatim)**:
  > *"Với tệp hữu hạn và thao tác đọc trả về, quá trình đọc/phân tích dữ liệu sản phẩm phải kết thúc ở trạng thái chấp nhận hoặc từ chối có kiểm soát; không lặp vô hạn."*
- **Danh mục & Ưu tiên**: `LIVENESS` | Mức độ: **HIGH**
- **Phạm vi áp dụng**: Bộ đọc và nạp tệp dữ liệu kho
- **Module & Hàm C liên quan**: `file_io.c` - Các hàm: `load_products_from_file, parse_product_line`
- **Kiểu dữ liệu & Đối tượng bộ nhớ**: `FILE*, char line[512]` | `File stream, Stack line buffer`
- **Tiền điều kiện (Preconditions)**: `file_parsing == true`
- **Hậu điều kiện (Postconditions)**: `file_loaded || file_rejected`
- **Bất biến an toàn / Liveness**: `AG(file_parsing -> AF(file_loaded | file_rejected))`
- **Điều kiện vi phạm (Unsafe State)**: `Vòng lặp fgets không tiến tới EOF hoặc bị treo vô hạn`
- **Điều kiện biên (Boundaries)**: Tệp rỗng, tệp không có ký tự newline, tệp chỉ chứa khoảng trắng
- **Giả định mô hình (Assumptions)**: Hệ điều hành đảm bảo fread/fgets trả về EOF khi hết tệp
- **Cơ chế phòng thủ hiện có trong mã v0**: while (fgets(...) != NULL) kết thúc khi gặp EOF; kiểm tra ferror(file)
- **Kỹ thuật kiểm chứng đề xuất**: **CTL Model Checking / Corpus Testing with malformed EOF** (File reading loop)
- **Phản ví dụ kỳ vọng (Counterexample)**: Tệp không có newline và feof không kích hoạt
- **Test case liên kết**: `TC-8`
- **Mã rủi ro liên kết**: `RISK-006`
- **Bằng chứng cần tạo ở Bước 3**: `Model checking proof log / EOF test log`
- **Trạng thái đặc tả**: `SPECIFIED_COMPLETE`

### REQ-9: Giới hạn chuỗi và ký tự kết thúc NUL (Fixed-Length String Boundary & NUL-Termination)
- **Nội dung nguyên văn (Verbatim)**:
  > *"Các trường chuỗi cố định của Product (id, name, category, unit) phải được kiểm tra độ dài trước sao chép, không ghi vượt buffer và luôn kết thúc bằng ký tự \0."*
- **Danh mục & Ưu tiên**: `MEMORY` | Mức độ: **HIGH**
- **Phạm vi áp dụng**: Các trường dữ liệu chuỗi trong cấu trúc Product
- **Module & Hàm C liên quan**: `models.h, validation.c, utils.c, product.c` - Các hàm: `validate_product_id, validate_product_name, validate_category, validate_unit, safe_strcpy, safe_string_copy`
- **Kiểu dữ liệu & Đối tượng bộ nhớ**: `char[], size_t` | `product.id[20], product.name[100], product.category[50], product.unit[20]`
- **Tiền điều kiện (Preconditions)**: `Chuỗi đầu vào là chuỗi C hợp lệ có \0`
- **Hậu điều kiện (Postconditions)**: `strlen(field) < sizeof(field) && field[sizeof(field)-1] == '\0'`
- **Bất biến an toàn / Liveness**: `strlen(str) < MAX_LEN && field[MAX_LEN-1] == '\0'`
- **Điều kiện vi phạm (Unsafe State)**: `strlen(str) >= MAX_LEN hoặc buffer thiếu ký tự kết thúc '\0'`
- **Điều kiện biên (Boundaries)**: Độ dài chuỗi = MAX_LEN - 2, MAX_LEN - 1, MAX_LEN
- **Giả định mô hình (Assumptions)**: Hằng số kích thước: MAX_ID_LEN=20, MAX_NAME_LEN=100, MAX_CAT_LEN=50, MAX_UNIT_LEN=20
- **Cơ chế phòng thủ hiện có trong mã v0**: Validation module kiểm tra len >= MAX_*_LEN và trả STATUS_ERR_INVALID_*; safe_strcpy dùng snprintf
- **Kỹ thuật kiểm chứng đề xuất**: **ASan Boundary Testing / CBMC String Invariants** (Tất cả 4 trường chuỗi)
- **Phản ví dụ kỳ vọng (Counterexample)**: Sao chép bằng strcpy không kiểm tra độ dài
- **Test case liên kết**: `TC-9`
- **Mã rủi ro liên kết**: `RISK-004`
- **Bằng chứng cần tạo ở Bước 3**: `ASan test string boundary log / test_utils.c (Test safe_string_copy PASS)`
- **Trạng thái đặc tả**: `SPECIFIED_COMPLETE`

### REQ-10: Kiểm soát an toàn bộ đệm nhập liệu dòng (Input Buffer Boundary and Truncation Guard)
- **Nội dung nguyên văn (Verbatim)**:
  > *"Input bàn phím và dòng tệp vượt kích thước buffer phải được tiêu thụ/từ chối có kiểm soát mà không phát sinh đọc/ghi ngoài vùng nhớ hợp lệ."*
- **Danh mục & Ưu tiên**: `MEMORY` | Mức độ: **HIGH**
- **Phạm vi áp dụng**: Đọc dữ liệu dòng từ stdin và từ file
- **Module & Hàm C liên quan**: `utils.c, file_io.c, main.c` - Các hàm: `safe_read_line, load_products_from_file`
- **Kiểu dữ liệu & Đối tượng bộ nhớ**: `char buffer[], size_t size` | `char buffer[size], char line[512]`
- **Tiền điều kiện (Preconditions)**: `buffer != NULL && size > 0`
- **Hậu điều kiện (Postconditions)**: `Không có byte nào được ghi vượt quá size-1; luôn có null-terminator`
- **Bất biến an toàn / Liveness**: `bytes_written < size && buffer[bytes_written] == '\0'`
- **Điều kiện vi phạm (Unsafe State)**: `Đọc tràn bộ đệm stack (Stack Buffer Overflow) hoặc sót dữ liệu trong stdin làm sai lệch lệnh tiếp theo`
- **Điều kiện biên (Boundaries)**: Dòng nhập có độ dài đúng size - 1, size, size + 1
- **Giả định mô hình (Assumptions)**: fgets đảm bảo không đọc quá size-1 ký tự
- **Cơ chế phòng thủ hiện có trong mã v0**: safe_read_line dọn sạch bộ đệm getchar(); load_products_from_file kiểm tra strchr('\n')
- **Kỹ thuật kiểm chứng đề xuất**: **ASan / Long Line Fuzzing** (Console input và File input)
- **Phản ví dụ kỳ vọng (Counterexample)**: gets() hoặc scanf("%s") không giới hạn độ dài
- **Test case liên kết**: `TC-10`
- **Mã rủi ro liên kết**: `RISK-006`
- **Bằng chứng cần tạo ở Bước 3**: `ASan long line test log / SIM-002`
- **Trạng thái đặc tả**: `SPECIFIED_COMPLETE`

### REQ-11: Quản lý vòng đời và chống rò rỉ bộ nhớ (Allocation Lifecycle & Leak Freedom)
- **Nội dung nguyên văn (Verbatim)**:
  > *"Allocation động do ProductList/Inventory sở hữu phải có vòng đời rõ ràng; không làm mất con trỏ sở hữu còn sống và phải giải phóng ở thời điểm kết thúc vòng đời."*
- **Danh mục & Ưu tiên**: `MEMORY` | Mức độ: **HIGH**
- **Phạm vi áp dụng**: Vùng nhớ heap mảng items
- **Module & Hàm C liên quan**: `product.c, inventory.c, main.c` - Các hàm: `product_list_init, product_list_free, inventory_init, inventory_free`
- **Kiểu dữ liệu & Đối tượng bộ nhớ**: `ProductList*` | `list->items heap buffer`
- **Tiền điều kiện (Preconditions)**: `Khởi tạo thành công qua product_list_init`
- **Hậu điều kiện (Postconditions)**: `Mọi khối heap được cấp phát đều được giải phóng qua product_list_free trước khi kết thúc chương trình`
- **Bất biến an toàn / Liveness**: `allocated_blocks == freed_blocks at termination`
- **Điều kiện vi phạm (Unsafe State)**: `Gán đè con trỏ list->items mà không free, hoặc thoát chương trình mà không gọi product_list_free`
- **Điều kiện biên (Boundaries)**: Danh sách rỗng (count=0), danh sách có 1000 sản phẩm, thoát chương trình bình thường và bất thường
- **Giả định mô hình (Assumptions)**: Single owner: ProductList là đối tượng duy nhất sở hữu items
- **Cơ chế phòng thủ hiện có trong mã v0**: main.c gọi product_list_free(&list) tại mọi điểm thoát
- **Kỹ thuật kiểm chứng đề xuất**: **LSan (LeakSanitizer) / Valgrind / ASan** (Toàn bộ luồng thực thi)
- **Phản ví dụ kỳ vọng (Counterexample)**: list->items = NULL mà chưa gọi free(list->items)
- **Test case liên kết**: `TC-11`
- **Mã rủi ro liên kết**: `NONE`
- **Bằng chứng cần tạo ở Bước 3**: `Valgrind/LSan clean exit log (0 bytes in 0 blocks leaked)`
- **Trạng thái đặc tả**: `SPECIFIED_COMPLETE`

### REQ-12: Bảo toàn khối nhớ khi realloc thất bại (Realloc Failure Ownership Preservation)
- **Nội dung nguyên văn (Verbatim)**:
  > *"Khi realloc thất bại, allocation cũ phải được giữ nguyên quyền sở hữu; khi thành công, con trỏ sở hữu và capacity phải cập nhật đúng vùng nhớ mới."*
- **Danh mục & Ưu tiên**: `MEMORY` | Mức độ: **HIGH**
- **Phạm vi áp dụng**: Thao tác mở rộng bộ nhớ heap trong add_product
- **Module & Hàm C liên quan**: `product.c` - Các hàm: `add_product`
- **Kiểu dữ liệu & Đối tượng bộ nhớ**: `Product*` | `list->items, new_items pointer`
- **Tiền điều kiện (Preconditions)**: `list->items != NULL && new_cap > list->capacity`
- **Hậu điều kiện (Postconditions)**: `(realloc_ok ==> list->items == new_items) && (realloc_fail ==> list->items == old_items)`
- **Bất biến an toàn / Liveness**: `AG(realloc_failed -> owner_valid)`
- **Điều kiện vi phạm (Unsafe State)**: `list->items = realloc(list->items, ...) gây mất con trỏ cũ khi trả về NULL (CWE-401)`
- **Điều kiện biên (Boundaries)**: realloc trả về NULL do thiếu bộ nhớ
- **Giả định mô hình (Assumptions)**: Môi trường có thể giả lập lỗi cấp phát
- **Cơ chế phòng thủ hiện có trong mã v0**: add_product dùng con trỏ tạm new_items: if (new_items == NULL) return STATUS_ERR_MEMORY;
- **Kỹ thuật kiểm chứng đề xuất**: **Fault Injection Harness / CBMC** (Hàm add_product)
- **Phản ví dụ kỳ vọng (Counterexample)**: list->items = realloc(list->items, new_size) trực tiếp
- **Test case liên kết**: `TC-12`
- **Mã rủi ro liên kết**: `NONE`
- **Bằng chứng cần tạo ở Bước 3**: `Allocation failure injection test log / code audit`
- **Trạng thái đặc tả**: `SPECIFIED_COMPLETE`

### REQ-13: Chống giải phóng bộ nhớ hai lần (Double Free Defense Invariant)
- **Nội dung nguyên văn (Verbatim)**:
  > *"Một allocation chỉ được free tối đa một lần; cleanup phải cập nhật trạng thái/con trỏ để ngăn double-free khi gọi lại."*
- **Danh mục & Ưu tiên**: `MEMORY` | Mức độ: **HIGH**
- **Phạm vi áp dụng**: Thu hồi bộ nhớ heap
- **Module & Hàm C liên quan**: `product.c, inventory.c` - Các hàm: `product_list_free, inventory_free`
- **Kiểu dữ liệu & Đối tượng bộ nhớ**: `ProductList*` | `list->items`
- **Tiền điều kiện (Preconditions)**: `list != NULL`
- **Hậu điều kiện (Postconditions)**: `list->items == NULL && list->count == 0 && list->capacity == 0`
- **Bất biến an toàn / Liveness**: `AG(!(list_live & list_freed))`
- **Điều kiện vi phạm (Unsafe State)**: `Gọi free(list->items) khi items đã được giải phóng trước đó (CWE-415)`
- **Điều kiện biên (Boundaries)**: Gọi product_list_free 2 lần liên tiếp trên cùng một biến
- **Giả định mô hình (Assumptions)**: free(NULL) trong C chuẩn là thao tác an toàn no-op
- **Cơ chế phòng thủ hiện có trong mã v0**: product_list_free kiểm tra if (list->items != NULL) và gán list->items = NULL ngay sau free
- **Kỹ thuật kiểm chứng đề xuất**: **ASan Double Free Test / CBMC** (Tất cả hàm cleanup)
- **Phản ví dụ kỳ vọng (Counterexample)**: free(list->items) nhưng không gán list->items = NULL
- **Test case liên kết**: `TC-13`
- **Mã rủi ro liên kết**: `NONE`
- **Bằng chứng cần tạo ở Bước 3**: `ASan repeated free test log / smoke-test.log (Test 17 PASS)`
- **Trạng thái đặc tả**: `SPECIFIED_COMPLETE`

### REQ-14: Chống sử dụng con trỏ mất hiệu lực sau realloc hoặc free (Dangling Pointer Safety)
- **Nội dung nguyên văn (Verbatim)**:
  > *"Không được dereference con trỏ Product/vùng nhớ ProductList sau free hoặc sau khi realloc làm con trỏ cũ mất hiệu lực."*
- **Danh mục & Ưu tiên**: `MEMORY` | Mức độ: **HIGH**
- **Phạm vi áp dụng**: Con trỏ mượn (Borrowed Pointers) vào mảng items
- **Module & Hàm C liên quan**: `product.c, inventory.c` - Các hàm: `find_product_by_id, find_product, find_product_mutable, add_product`
- **Kiểu dữ liệu & Đối tượng bộ nhớ**: `Product*` | `Borrowed pointer Product* target`
- **Tiền điều kiện (Preconditions)**: `Con trỏ mượn được lấy trước khi container thay đổi`
- **Hậu điều kiện (Postconditions)**: `Không có hành vi giải tham chiếu (dereference) trên con trỏ cũ sau khi realloc di dời heap`
- **Bất biến an toàn / Liveness**: `AG(ptr_invalid -> !ptr_deref)`
- **Điều kiện vi phạm (Unsafe State)**: `Dereference con trỏ trỏ tới vùng heap đã bị thu hồi/di dời (CWE-416)`
- **Điều kiện biên (Boundaries)**: realloc di chuyển khối nhớ sang địa chỉ khác
- **Giả định mô hình (Assumptions)**: realloc có thể trả về địa chỉ mới hoặc tái sử dụng tại chỗ tùy thuộc allocator
- **Cơ chế phòng thủ hiện có trong mã v0**: Trong baseline v0, không có caller nào lưu trữ con trỏ mượn qua lời gọi add_product
- **Kỹ thuật kiểm chứng đề xuất**: **ASan Dangling Pointer Harness / CBMC Pointer Invalidation Model** (Inter-procedural pointer analysis)
- **Phản ví dụ kỳ vọng (Counterexample)**: Lưu ptr = find_product(), gọi add_product(), sau đó đọc ptr->quantity
- **Test case liên kết**: `TC-14`
- **Mã rủi ro liên kết**: `RISK-003`
- **Bằng chứng cần tạo ở Bước 3**: `ASan simulation log (SIM-001) / CBMC verification report`
- **Trạng thái đặc tả**: `SPECIFIED_COMPLETE`

### REQ-15: Kiểm tra phòng thủ con trỏ NULL tại giao diện API (Defensive NULL Pointer Contract)
- **Nội dung nguyên văn (Verbatim)**:
  > *"API ProductList/Inventory phải kiểm tra con trỏ NULL theo hợp đồng hàm trước khi dereference đối tượng hoặc buffer vào/ra."*
- **Danh mục & Ưu tiên**: `MEMORY` | Mức độ: **HIGH**
- **Phạm vi áp dụng**: Toàn bộ các API public trong các tệp header
- **Module & Hàm C liên quan**: `product.h, inventory.h, file_io.h, validation.h, utils.h, report.h` - Các hàm: `Tất cả các hàm public nhận tham số con trỏ`
- **Kiểu dữ liệu & Đối tượng bộ nhớ**: `Con trỏ các loại (const char*, ProductList*, Inventory*, double*, int*)` | `Pointer arguments`
- **Tiền điều kiện (Preconditions)**: `Lời gọi hàm với đối số con trỏ bất kỳ (hợp lệ hoặc NULL)`
- **Hậu điều kiện (Postconditions)**: `Nếu con trỏ NULL, hàm trả về mã lỗi STATUS_ERR_NULL_PTR hoặc NULL mà không crash`
- **Bất biến an toàn / Liveness**: `ptr == NULL ==> không dereference ptr`
- **Điều kiện vi phạm (Unsafe State)**: `Truy cập *ptr khi ptr == NULL dẫn tới Hardware Access Violation (CWE-476)`
- **Điều kiện biên (Boundaries)**: Truyền NULL cho mọi tham số con trỏ
- **Giả định mô hình (Assumptions)**: Hợp đồng API yêu cầu trả mã lỗi hoặc con trỏ rỗng khi nhận NULL
- **Cơ chế phòng thủ hiện có trong mã v0**: Tất cả 14 API chính đều có mệnh đề bảo vệ if (ptr == NULL) return ...
- **Kỹ thuật kiểm chứng đề xuất**: **CBMC NULL Pointer Analysis / ASan NULL Harness (test_cwe476)** (100% public APIs)
- **Phản ví dụ kỳ vọng (Counterexample)**: Hàm truy cập list->count ngay dòng đầu tiên mà không kiểm tra list != NULL
- **Test case liên kết**: `TC-15`
- **Mã rủi ro liên kết**: `RISK-010`
- **Bằng chứng cần tạo ở Bước 3**: `CBMC pointer checks log / Unit test NULL assertions`
- **Trạng thái đặc tả**: `SPECIFIED_COMPLETE`

### REQ-16: Giới hạn kích thước mảng phụ trợ và dịch chuyển memmove (Auxiliary Buffer Bounds Safety)
- **Nội dung nguyên văn (Verbatim)**:
  > *"Mảng tokens, results, summaries và vùng nhớ dùng với memmove phải kiểm soát số phần tử, số byte, giới hạn nguồn/đích; không truy cập ngoài vùng nhớ đã cấp phát."*
- **Danh mục & Ưu tiên**: `MEMORY` | Mức độ: **HIGH**
- **Phạm vi áp dụng**: Các mảng cục bộ và thao tác di chuyển vùng nhớ
- **Module & Hàm C liên quan**: `file_io.c, product.c, report.c, utils.c` - Các hàm: `parse_product_line, search_products_by_name, calculate_category_summaries, delete_product, split_line`
- **Kiểu dữ liệu & Đối tượng bộ nhớ**: `char* tokens[8], Product results[], CategorySummary summaries[32]` | `tokens, results, summaries, items memmove slice`
- **Tiền điều kiện (Preconditions)**: `Mảng phụ trợ có dung lượng tối đa xác định (8 tokens, max_results, max_summaries=32)`
- **Hậu điều kiện (Postconditions)**: `Chỉ số ghi luôn nhỏ hơn dung lượng tối đa được cấp phát`
- **Bất biến an toàn / Liveness**: `token_count <= 8 && out_count <= max_summaries && i < count`
- **Điều kiện vi phạm (Unsafe State)**: `Ghi vượt ra ngoài mảng tokens, summaries hoặc tính sai byte size trong memmove (CWE-122)`
- **Điều kiện biên (Boundaries)**: Dòng tệp có >8 tokens; kho có >32 danh mục; xóa phần tử đầu tiên / cuối cùng
- **Giả định mô hình (Assumptions)**: Kích thước mảng được định nghĩa qua hằng số hoặc tham số max_*
- **Cơ chế phòng thủ hiện có trong mã v0**: parse_product_line giới hạn token_count < 8; report giới hạn *out_count >= max_summaries
- **Kỹ thuật kiểm chứng đề xuất**: **ASan / CBMC Array Bounds / Boundary Tests** (4 hàm sử dụng mảng phụ trợ)
- **Phản ví dụ kỳ vọng (Counterexample)**: tokens[token_count++] trong vòng lặp không kiểm tra token_count < 8
- **Test case liên kết**: `TC-6, TC-16`
- **Mã rủi ro liên kết**: `RISK-005, RISK-009`
- **Bằng chứng cần tạo ở Bước 3**: `ASan bounds verification log`
- **Trạng thái đặc tả**: `SPECIFIED_COMPLETE`

### REQ-17: Đặc tả cú pháp tệp dữ liệu kho và từ chối an toàn (File Format Parser Specification)
- **Nội dung nguyên văn (Verbatim)**:
  > *"Parser phải chấp nhận đúng định dạng 6 trường hoặc định dạng tương thích 4 trường; các dòng sai số trường/độ dài/định dạng phải bị từ chối mà không gây hỏng bộ nhớ Inventory."*
- **Danh mục & Ưu tiên**: `FUNCTIONAL` | Mức độ: **HIGH**
- **Phạm vi áp dụng**: Phân tích cú pháp tệp products.txt
- **Module & Hàm C liên quan**: `file_io.c` - Các hàm: `load_products_from_file, parse_product_line`
- **Kiểu dữ liệu & Đối tượng bộ nhớ**: `char*, Product*` | `Product record, tokens`
- **Tiền điều kiện (Preconditions)**: `Tệp tin văn bản mở thành công chế độ đọc`
- **Hậu điều kiện (Postconditions)**: `Bản ghi hợp lệ (6 trường hoặc 4 trường) được nạp vào Inventory; bản ghi lỗi bị bỏ qua mà không làm hỏng dữ liệu trước đó`
- **Bất biến an toàn / Liveness**: `parse_error ==> Inventory state preserved & no corrupted Product inserted`
- **Điều kiện vi phạm (Unsafe State)**: `Parser chép dữ liệu sai định dạng hoặc chuỗi chưa kiểm tra vào kho gây hỏng bộ nhớ`
- **Điều kiện biên (Boundaries)**: Dòng có 3 trường, 5 trường, 7 trường, dòng có trường rỗng, dòng chứa pipe liên tiếp '||'
- **Giả định mô hình (Assumptions)**: Định dạng 6 trường: ID|Name|Category|Unit|Quantity|Price; 4 trường: ID|Name|Quantity|Price
- **Cơ chế phòng thủ hiện có trong mã v0**: if (token_count == 6) { ... } else if (token_count == 4) { ... } else return STATUS_ERR_FILE_IO;
- **Kỹ thuật kiểm chứng đề xuất**: **Corpus Testing / Fuzzing / Round-trip Save-Load Test** (Bộ nạp tệp file_io)
- **Phản ví dụ kỳ vọng (Counterexample)**: Dòng có 5 trường bị ép kiểu dẫn tới đọc con trỏ NULL
- **Test case liên kết**: `TC-16`
- **Mã rủi ro liên kết**: `NONE`
- **Bằng chứng cần tạo ở Bước 3**: `results/baseline/smoke-test.log (Test 11, 12, 13 PASS) / Fuzzing harness`
- **Trạng thái đặc tả**: `SPECIFIED_COMPLETE`

### REQ-18: Xác thực số lượng và đơn giá hợp lệ (Numeric Range and Finite Float Invariant)
- **Nội dung nguyên văn (Verbatim)**:
  > *"Quantity/price không hợp lệ, vượt miền số hoặc không hữu hạn (NaN/Infinity) phải bị từ chối trước cập nhật kho và trước phép tính báo cáo có liên quan."*
- **Danh mục & Ưu tiên**: `FUNCTIONAL` | Mức độ: **HIGH**
- **Phạm vi áp dụng**: Kiểm tra giá trị số của sản phẩm
- **Module & Hàm C liên quan**: `validation.c, file_io.c, report.c` - Các hàm: `validate_price, validate_quantity, validate_price_str, validate_quantity_str, parse_int, parse_price`
- **Kiểu dữ liệu & Đối tượng bộ nhớ**: `int, double` | `product.quantity, product.price`
- **Tiền điều kiện (Preconditions)**: `Dữ liệu chuỗi số từ bàn phím hoặc tệp tin`
- **Hậu điều kiện (Postconditions)**: `Chỉ chấp nhận số nguyên >= 0 và số thực >= 0.0 hữu hạn (isfinite)`
- **Bất biến an toàn / Liveness**: `quantity >= 0 && quantity <= INT_MAX && isfinite(price) && price >= 0.0`
- **Điều kiện vi phạm (Unsafe State)**: `Chấp nhận số âm, số vượt INT_MAX, NaN, Infinity làm sai lệch tổng giá trị kho hàng`
- **Điều kiện biên (Boundaries)**: price = -0.01, 0.0, NaN, Inf; quantity = -1, 0, INT_MAX, INT_MAX + 1
- **Giả định mô hình (Assumptions)**: Chuẩn IEEE 754 cho kiểu double; isfinite() trong math.h
- **Cơ chế phòng thủ hiện có trong mã v0**: parse_price kiểm tra !isfinite(parsed) || parsed < 0.0; validate_price_str kiểm tra dấu âm
- **Kỹ thuật kiểm chứng đề xuất**: **UBSan / Numeric Boundary Unit Test** (Toàn bộ chuyển đổi số)
- **Phản ví dụ kỳ vọng (Counterexample)**: parse_price cho phép nhập chuỗi 'nan' hoặc 'inf'
- **Test case liên kết**: `TC-17`
- **Mã rủi ro liên kết**: `NONE`
- **Bằng chứng cần tạo ở Bước 3**: `test_utils.c (Test safe_conversions PASS) / test_validation.c`
- **Trạng thái đặc tả**: `SPECIFIED_COMPLETE`

