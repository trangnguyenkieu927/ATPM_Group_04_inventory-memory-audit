# ĐẶC TẢ KỸ THUẬT VÀ TRUY VẾT 18 YÊU CẦU AN TOÀN BỘ NHỚ (18 REQs - STEP 2)

**Dự án:** Kiểm chứng an toàn bộ nhớ cho chương trình C mô phỏng quản lý kho
**Tài liệu nguồn chuẩn:** `Spec_Inventory_Memory_Safety_Audit_18REQ_CHUAN.xlsx`
**Commit Baseline:** `44202070c68002d9af068374478cdc83098dd4d8`
**Kỹ sư phụ trách:** Hà Văn Đô (Formal Specification & Verification)

---

## 1. NGUYÊN TẮC ĐẶC TẢ HÌNH THỨC HÓA

1. **Sử dụng nguyên văn (Verbatim):** 100% nội dung yêu cầu được trích xuất nguyên văn từ Sheet `YeuCauChucNang` trong workbook gốc của đề tài; không suy đoán hoặc tự đặt lại tên yêu cầu.
2. **Định vị mã nguồn chuẩn xác:** Mỗi yêu cầu được định vị tới đúng tập tin C, hàm C và số dòng mã tại commit baseline `4420207`.
3. **Đặc tả đa chiều:** Mỗi yêu cầu có đầy đủ Tiền điều kiện (Preconditions), Hậu điều kiện (Postconditions), Bất biến an toàn (Safety Invariant), Điều kiện vi phạm (Unsafe Condition), Cơ chế guard hiện tại và Phương pháp kiểm chứng dự kiến cho Bước 3.

---

## 2. MA TRẬN TRUY VẾT 18 YÊU CẦU (18 REQs TRACEABILITY MATRIX)

| ID | Danh mục | Nội dung Nguyên văn | Module & Hàm C | Dòng mã | Rủi ro Liên quan | Kỹ thuật Kiểm chứng |
|---|---|---|---|---|---|---|
| **REQ-1** | `safety` | ProductList luôn thỏa count <= capacity; khi count > 0 thì items khác NULL và trỏ tới vùng nhớ còn hiệu lực. | `product_list_init, add_product, delete_product` | `9-24, 116-138, 203-220` | `RISK-001, RISK-002` | CBMC Invariant Check / Temporal Logic Model Checking |
| **REQ-2** | `safety` | Trước khi mở rộng ProductList bằng capacity * 2, phải kiểm tra capacity <= SIZE_MAX / 2 để tránh tràn số size_t. | `add_product` | `118` | `RISK-001` | SMT Bit-Vector (Z3 QF_BV) / CBMC |
| **REQ-3** | `safety` | Trước realloc, phải kiểm tra new_cap <= SIZE_MAX / sizeof(Product) để phép nhân kích thước cấp phát không bị tràn. | `add_product` | `119` | `RISK-002` | SMT Bit-Vector (Z3 QF_BV) / Harness SIM-001 |
| **REQ-4** | `safety` | Trong import_stock, chỉ cộng khi add_quantity > 0, tồn kho hiện tại hợp lệ và current_stock <= INT_MAX - add_quantity; nếu không phải từ chối trước khi cập nhật. | `import_stock` | `30-47` | `None (Đã được guard hoàn toàn trong baseline v0)` | SMT Bit-Vector (Z3) / Smoke Test Case 8 |
| **REQ-5** | `safety` | Trong export_stock, số lượng xuất phải > 0 và <= tồn kho hiện tại; không được tạo ra tồn kho âm hoặc cập nhật khi điều kiện không thỏa. | `export_stock` | `49-66` | `None (Đã được phòng vệ an toàn)` | CBMC / Unit Test / Smoke Test Case 10 |
| **REQ-6** | `safety` | Mọi truy cập ProductList.items[i] trong thêm/sửa/xóa/tìm kiếm/báo cáo phải thỏa i < count <= capacity và items còn hiệu lực. | `find_product_by_id, update_product, delete_product` | `128, 147, 207-215, 227-230` | `None (Đã được kiểm tra giới hạn trong v0)` | CBMC Loop Unwinding / AddressSanitizer |
| **REQ-7** | `liveness` | Với đầu vào hữu hạn và thao tác I/O trả về, mọi thao tác thêm/sửa/xóa/nhập/xuất đã bắt đầu phải kết thúc ở trạng thái thành công hoặc từ chối có kiểm soát; không lặp vô hạn. | `add_product, update_product, delete_product, import_stock, export_stock` | `70-138, 141-198, 200-221, 30-47, 49-66` | `None` | CTL Model Checking (NuSMV) / Termination Analysis |
| **REQ-8** | `liveness` | Với tệp hữu hạn và thao tác đọc trả về, quá trình đọc/phân tích dữ liệu sản phẩm phải kết thúc ở trạng thái chấp nhận hoặc từ chối có kiểm soát; không lặp vô hạn. | `load_products_from_file` | `94-135` | `None` | CTL Model Checking (NuSMV) |
| **REQ-9** | `memory` | Các trường chuỗi cố định của Product (id, name, category, unit) phải được kiểm tra độ dài trước sao chép, không ghi vượt buffer và luôn kết thúc bằng ký tự \0. | `safe_string_copy, safe_strcpy` | `44-66 (utils.c), 37-44 (product.c)` | `RISK-006` | CBMC Bounds Check / Unit Test test_utils |
| **REQ-10** | `memory` | Input bàn phím và dòng tệp vượt kích thước buffer phải được tiêu thụ/từ chối có kiểm soát mà không phát sinh đọc/ghi ngoài vùng nhớ hợp lệ. | `load_products_from_file` | `96, 107-114` | `RISK-007` | AddressSanitizer / Harness SIM-002 |
| **REQ-11** | `memory` | Allocation động do ProductList/Inventory sở hữu phải có vòng đời rõ ràng; không làm mất con trỏ sở hữu còn sống và phải giải phóng ở thời điểm kết thúc vòng đời. | `product_list_init, product_list_free, inventory_free` | `9-24, 27-34` | `RISK-003, RISK-005` | LeakSanitizer (LSan) / Valgrind / CBMC |
| **REQ-12** | `memory` | Khi realloc thất bại, allocation cũ phải được giữ nguyên quyền sở hữu; khi thành công, con trỏ sở hữu và capacity phải cập nhật đúng vùng nhớ mới. | `add_product` | `119-125` | `RISK-003` | CTL Model Checking / CBMC / Fault Injection SIM-001 |
| **REQ-13** | `memory` | Một allocation chỉ được free tối đa một lần; cleanup phải cập nhật trạng thái/con trỏ để ngăn double-free khi gọi lại. | `product_list_free` | `27-34` | `RISK-005` | AddressSanitizer / CTL Model Checking |
| **REQ-14** | `memory` | Không được dereference con trỏ Product/vùng nhớ ProductList sau free hoặc sau khi realloc làm con trỏ cũ mất hiệu lực. | `find_product_by_id, add_product` | `223-233, 119` | `RISK-004` | Harness SIM-001 / Dynamic Contract Checking |
| **REQ-15** | `memory` | API ProductList/Inventory phải kiểm tra con trỏ NULL theo hợp đồng hàm trước khi dereference đối tượng hoặc buffer vào/ra. | `product_list_init, add_product, update_product, delete_product, find_product_by_id` | `10, 71, 142, 201, 224` | `None (Đã được guard đầy đủ trên toàn bộ API)` | CBMC Pointer Check / Unit Tests |
| **REQ-16** | `memory` | Mảng tokens, results, summaries và vùng nhớ dùng với memmove phải kiểm soát số phần tử, số byte, giới hạn nguồn/đích; không truy cập ngoài vùng nhớ đã cấp phát. | `parse_product_line, calculate_category_summaries, delete_product` | `47, 52 (file_io.c); 72-74 (report.c); 212 (product.c)` | `RISK-008, RISK-010` | AddressSanitizer / CBMC Array Bounds |
| **REQ-17** | `functional` | Parser phải chấp nhận đúng định dạng 6 trường hoặc định dạng tương thích 4 trường; các dòng sai số trường/độ dài/định dạng phải bị từ chối mà không gây hỏng bộ nhớ Inventory. | `parse_product_line` | `46-92` | `RISK-007, RISK-008, RISK-009` | Fuzzing / Test Corpus Regression (Smoke Test 12, 13) |
| **REQ-18** | `functional` | Quantity/price không hợp lệ, vượt miền số hoặc không hữu hạn (NaN/Infinity) phải bị từ chối trước cập nhật kho và trước phép tính báo cáo có liên quan. | `validate_quantity, validate_price, parse_int, parse_price` | `150-170, 180-210 (validation.c); 20-44 (file_io.c)` | `RISK-009` | Unit Tests / CBMC IEEE-754 Validation |

---

## 3. ĐẶC TẢ CHI TIẾT TỪNG YÊU CẦU AN TOÀN BỘ NHỚ

### REQ-1: ProductList luôn thỏa count <= capacity; khi count > 0 thì items khác NULL và trỏ tới vùng nhớ còn hiệu lực.
- **Danh mục:** `safety` | **Mức ưu tiên:** `HIGH` | **Phạm vi:** Quản lý mảng động ProductList
- **Tập tin nguồn:** `source/src/product.c` | **Hàm C:** `product_list_init, add_product, delete_product` | **Dòng mã:** `9-24, 116-138, 203-220`
- **Kiểu dữ liệu:** `ProductList, Product*, size_t` | **Đối tượng bộ nhớ:** `list->items, list->count, list->capacity`
- **Tiền điều kiện (Preconditions):** list != NULL
- **Hậu điều kiện (Postconditions):** list->count <= list->capacity, items != NULL khi count > 0
- **Bất biến an toàn (Safety Invariant):** `AG(list_valid -> (list->count <= list->capacity))`
- **Điều kiện rủi ro vi phạm (Unsafe Condition):** `count > capacity || (count > 0 && items == NULL)`
- **Cơ chế phòng vệ hiện tại (Existing Guards):** Khởi tạo count=0, capacity=INITIAL_CAPACITY; add_product mở rộng capacity khi count >= capacity
- **Kỹ thuật kiểm chứng Bước 3:** CBMC Invariant Check / Temporal Logic Model Checking
- **Rủi ro liên quan:** `RISK-001, RISK-002`
- **Bằng chứng cần tạo ở Bước 3:** CBMC harness invariant_check

### REQ-2: Trước khi mở rộng ProductList bằng capacity * 2, phải kiểm tra capacity <= SIZE_MAX / 2 để tránh tràn số size_t.
- **Danh mục:** `safety` | **Mức ưu tiên:** `HIGH` | **Phạm vi:** Tính toán dung lượng mở rộng mảng động
- **Tập tin nguồn:** `source/src/product.c` | **Hàm C:** `add_product` | **Dòng mã:** `118`
- **Kiểu dữ liệu:** `size_t (64-bit unsigned)` | **Đối tượng bộ nhớ:** `list->capacity, new_cap`
- **Tiền điều kiện (Preconditions):** list != NULL, list->count >= list->capacity
- **Hậu điều kiện (Postconditions):** new_cap không bị tràn bit (new_cap >= list->capacity)
- **Bất biến an toàn (Safety Invariant):** `capacity <= SIZE_MAX / 2`
- **Điều kiện rủi ro vi phạm (Unsafe Condition):** `capacity > SIZE_MAX / 2 (0x8000000000000000)`
- **Cơ chế phòng vệ hiện tại (Existing Guards):** Chưa có guard tường minh; phụ thuộc vào trần bộ nhớ RAM
- **Kỹ thuật kiểm chứng Bước 3:** SMT Bit-Vector (Z3 QF_BV) / CBMC
- **Rủi ro liên quan:** `RISK-001`
- **Bằng chứng cần tạo ở Bước 3:** Script SMT-LIB v2 scratch/smt_capacity_growth.smt2

### REQ-3: Trước realloc, phải kiểm tra new_cap <= SIZE_MAX / sizeof(Product) để phép nhân kích thước cấp phát không bị tràn.
- **Danh mục:** `safety` | **Mức ưu tiên:** `HIGH` | **Phạm vi:** Tính toán kích thước byte truyền vào realloc
- **Tập tin nguồn:** `source/src/product.c` | **Hàm C:** `add_product` | **Dòng mã:** `119`
- **Kiểu dữ liệu:** `size_t, Product (sizeof=208 bytes)` | **Đối tượng bộ nhớ:** `new_cap * sizeof(Product)`
- **Tiền điều kiện (Preconditions):** new_cap > 0
- **Hậu điều kiện (Postconditions):** new_cap * sizeof(Product) <= SIZE_MAX
- **Bất biến an toàn (Safety Invariant):** `new_cap <= SIZE_MAX / sizeof(Product)`
- **Điều kiện rủi ro vi phạm (Unsafe Condition):** `new_cap > SIZE_MAX / 208 (88.686.269.585.142.075)`
- **Cơ chế phòng vệ hiện tại (Existing Guards):** Chưa có guard tường minh; realloc sẽ thất bại do thiếu RAM trước khi tràn
- **Kỹ thuật kiểm chứng Bước 3:** SMT Bit-Vector (Z3 QF_BV) / Harness SIM-001
- **Rủi ro liên quan:** `RISK-002`
- **Bằng chứng cần tạo ở Bước 3:** Script SMT-LIB v2 scratch/smt_allocation_size.smt2

### REQ-4: Trong import_stock, chỉ cộng khi add_quantity > 0, tồn kho hiện tại hợp lệ và current_stock <= INT_MAX - add_quantity; nếu không phải từ chối trước khi cập nhật.
- **Danh mục:** `safety` | **Mức ưu tiên:** `HIGH` | **Phạm vi:** Nghiệp vụ nhập kho và phép cộng số lượng tồn kho
- **Tập tin nguồn:** `source/src/inventory.c` | **Hàm C:** `import_stock` | **Dòng mã:** `30-47`
- **Kiểu dữ liệu:** `int (32-bit signed)` | **Đối tượng bộ nhớ:** `product->quantity`
- **Tiền điều kiện (Preconditions):** inventory != NULL, product_id != NULL, product tồn tại trong kho
- **Hậu điều kiện (Postconditions):** product->quantity mới = product->quantity cũ + add_quantity
- **Bất biến an toàn (Safety Invariant):** `current_stock <= INT_MAX - add_quantity && add_quantity > 0 && current_stock >= 0`
- **Điều kiện rủi ro vi phạm (Unsafe Condition):** `current_stock > INT_MAX - add_quantity (Tràn số nguyên có dấu)`
- **Cơ chế phòng vệ hiện tại (Existing Guards):** Gọi check_addition_overflow tại validation.c:297-311 kiểm tra current_stock > INT_MAX - add_quantity và trả về STATUS_ERR_OVERFLOW
- **Kỹ thuật kiểm chứng Bước 3:** SMT Bit-Vector (Z3) / Smoke Test Case 8
- **Rủi ro liên quan:** `None (Đã được guard hoàn toàn trong baseline v0)`
- **Bằng chứng cần tạo ở Bước 3:** SMT-03 Bit-Vector Script & Smoke Test 8 log

### REQ-5: Trong export_stock, số lượng xuất phải > 0 và <= tồn kho hiện tại; không được tạo ra tồn kho âm hoặc cập nhật khi điều kiện không thỏa.
- **Danh mục:** `safety` | **Mức ưu tiên:** `HIGH` | **Phạm vi:** Nghiệp vụ xuất kho
- **Tập tin nguồn:** `source/src/inventory.c` | **Hàm C:** `export_stock` | **Dòng mã:** `49-66`
- **Kiểu dữ liệu:** `int (32-bit signed)` | **Đối tượng bộ nhớ:** `product->quantity`
- **Tiền điều kiện (Preconditions):** inventory != NULL, product_id != NULL, product tồn tại trong kho
- **Hậu điều kiện (Postconditions):** product->quantity mới >= 0
- **Bất biến an toàn (Safety Invariant):** `export_quantity > 0 && export_quantity <= current_stock`
- **Điều kiện rủi ro vi phạm (Unsafe Condition):** `export_quantity > current_stock || export_quantity <= 0`
- **Cơ chế phòng vệ hiện tại (Existing Guards):** Gọi validate_export_quantity tại validation.c:286-295 trả về STATUS_ERR_INSUFFICIENT_STOCK
- **Kỹ thuật kiểm chứng Bước 3:** CBMC / Unit Test / Smoke Test Case 10
- **Rủi ro liên quan:** `None (Đã được phòng vệ an toàn)`
- **Bằng chứng cần tạo ở Bước 3:** Smoke Test Case 10 pass report

### REQ-6: Mọi truy cập ProductList.items[i] trong thêm/sửa/xóa/tìm kiếm/báo cáo phải thỏa i < count <= capacity và items còn hiệu lực.
- **Danh mục:** `safety` | **Mức ưu tiên:** `HIGH` | **Phạm vi:** Truy cập phần tử mảng động items[i]
- **Tập tin nguồn:** `source/src/product.c` | **Hàm C:** `find_product_by_id, update_product, delete_product` | **Dòng mã:** `128, 147, 207-215, 227-230`
- **Kiểu dữ liệu:** `size_t i, ProductList` | **Đối tượng bộ nhớ:** `list->items[i]`
- **Tiền điều kiện (Preconditions):** list != NULL, list->items != NULL
- **Hậu điều kiện (Postconditions):** Không phát sinh Out-of-bounds Read/Write (CWE-125/CWE-787)
- **Bất biến an toàn (Safety Invariant):** `forall i: 0 <= i < list->count <= list->capacity`
- **Điều kiện rủi ro vi phạm (Unsafe Condition):** `i >= list->count || list->items == NULL`
- **Cơ chế phòng vệ hiện tại (Existing Guards):** Vòng lặp kiểm tra i < list->count trước khi truy cập
- **Kỹ thuật kiểm chứng Bước 3:** CBMC Loop Unwinding / AddressSanitizer
- **Rủi ro liên quan:** `None (Đã được kiểm tra giới hạn trong v0)`
- **Bằng chứng cần tạo ở Bước 3:** CBMC unwinding report & ASan run

### REQ-7: Với đầu vào hữu hạn và thao tác I/O trả về, mọi thao tác thêm/sửa/xóa/nhập/xuất đã bắt đầu phải kết thúc ở trạng thái thành công hoặc từ chối có kiểm soát; không lặp vô hạn.
- **Danh mục:** `liveness` | **Mức ưu tiên:** `HIGH` | **Phạm vi:** Tính dừng của các tiến trình nghiệp vụ kho
- **Tập tin nguồn:** `source/src/product.c` | **Hàm C:** `add_product, update_product, delete_product, import_stock, export_stock` | **Dòng mã:** `70-138, 141-198, 200-221, 30-47, 49-66`
- **Kiểu dữ liệu:** `StatusCode, Inventory, ProductList` | **Đối tượng bộ nhớ:** `Trạng thái máy trạng thái nghiệp vụ`
- **Tiền điều kiện (Preconditions):** Đầu vào hữu hạn
- **Hậu điều kiện (Postconditions):** Hàm trả về mã trạng thái trong miền xác định, không rơi vào vòng lặp vô hạn
- **Bất biến an toàn (Safety Invariant):** `AG(product_op_processing -> AF(product_op_done | product_op_rejected))`
- **Điều kiện rủi ro vi phạm (Unsafe Condition):** `EG(product_op_processing) (Vòng lặp vô hạn / treo hệ thống)`
- **Cơ chế phòng vệ hiện tại (Existing Guards):** Các vòng lặp đều có điều kiện dừng hữu hạn trên biến đếm mảng
- **Kỹ thuật kiểm chứng Bước 3:** CTL Model Checking (NuSMV) / Termination Analysis
- **Rủi ro liên quan:** `None`
- **Bằng chứng cần tạo ở Bước 3:** Mô hình NuSMV verification result

### REQ-8: Với tệp hữu hạn và thao tác đọc trả về, quá trình đọc/phân tích dữ liệu sản phẩm phải kết thúc ở trạng thái chấp nhận hoặc từ chối có kiểm soát; không lặp vô hạn.
- **Danh mục:** `liveness` | **Mức ưu tiên:** `HIGH` | **Phạm vi:** Tính dừng của quá trình đọc và phân tích tệp tin
- **Tập tin nguồn:** `source/src/file_io.c` | **Hàm C:** `load_products_from_file` | **Dòng mã:** `94-135`
- **Kiểu dữ liệu:** `FILE*, char line[512]` | **Đối tượng bộ nhớ:** `file pointer, line buffer`
- **Tiền điều kiện (Preconditions):** Tệp tin mở thành công ở chế độ đọc
- **Hậu điều kiện (Postconditions):** Đóng tệp tin, trả về trạng thái nạp tệp thành công hoặc từ chối an toàn
- **Bất biến an toàn (Safety Invariant):** `AG(file_parsing -> AF(file_loaded | file_rejected))`
- **Điều kiện rủi ro vi phạm (Unsafe Condition):** `Vòng lặp fgets không tiến triển khi gặp ký tự lạ hoặc lỗi I/O`
- **Cơ chế phòng vệ hiện tại (Existing Guards):** while (fgets(...) != NULL) kết hợp kiểm tra ferror(file) và feof(file)
- **Kỹ thuật kiểm chứng Bước 3:** CTL Model Checking (NuSMV)
- **Rủi ro liên quan:** `None`
- **Bằng chứng cần tạo ở Bước 3:** NuSMV CTL-08 model check

### REQ-9: Các trường chuỗi cố định của Product (id, name, category, unit) phải được kiểm tra độ dài trước sao chép, không ghi vượt buffer và luôn kết thúc bằng ký tự \0.
- **Danh mục:** `memory` | **Mức ưu tiên:** `HIGH` | **Phạm vi:** Bảo đảm kích thước chuỗi cố định và ký tự kết thúc NUL
- **Tập tin nguồn:** `source/src/utils.c` | **Hàm C:** `safe_string_copy, safe_strcpy` | **Dòng mã:** `44-66 (utils.c), 37-44 (product.c)`
- **Kiểu dữ liệu:** `char id[20], char name[100], char category[50], char unit[20]` | **Đối tượng bộ nhớ:** `Product struct string fields`
- **Tiền điều kiện (Preconditions):** dest != NULL, dest_size > 0
- **Hậu điều kiện (Postconditions):** dest[dest_size - 1] == '\0', không ghi vượt dest_size bytes
- **Bất biến an toàn (Safety Invariant):** `forall str in {id, name, category, unit}: str[sizeof(str)-1] == '\0'`
- **Điều kiện rủi ro vi phạm (Unsafe Condition):** `Độ dài chuỗi sao chép >= dest_size mà không có ký tự NUL kết thúc`
- **Cơ chế phòng vệ hiện tại (Existing Guards):** safe_string_copy kiểm tra biên và luôn gán dest[i] = '\0'; safe_strcpy dùng snprintf
- **Kỹ thuật kiểm chứng Bước 3:** CBMC Bounds Check / Unit Test test_utils
- **Rủi ro liên quan:** `RISK-006`
- **Bằng chứng cần tạo ở Bước 3:** CBMC string bounds proof / tests/test_utils.c execution log

### REQ-10: Input bàn phím và dòng tệp vượt kích thước buffer phải được tiêu thụ/từ chối có kiểm soát mà không phát sinh đọc/ghi ngoài vùng nhớ hợp lệ.
- **Danh mục:** `memory` | **Mức ưu tiên:** `HIGH` | **Phạm vi:** Bộ đệm dòng đầu vào tệp và bàn phím (512 bytes)
- **Tập tin nguồn:** `source/src/file_io.c` | **Hàm C:** `load_products_from_file` | **Dòng mã:** `96, 107-114`
- **Kiểu dữ liệu:** `char line[LINE_BUFFER_SIZE]` | **Đối tượng bộ nhớ:** `line buffer trên stack`
- **Tiền điều kiện (Preconditions):** Tệp tin chứa dòng vượt quá 511 bytes
- **Hậu điều kiện (Postconditions):** Từ chối dòng bị cắt cụt một cách an toàn, không phát sinh tràn bộ đệm
- **Bất biến an toàn (Safety Invariant):** `AG(buffer_processing -> AF(buffer_safe | buffer_rejected))`
- **Điều kiện rủi ro vi phạm (Unsafe Condition):** `Đọc ghi vượt quá 512 bytes của line buffer (CWE-120)`
- **Cơ chế phòng vệ hiện tại (Existing Guards):** fgets(line, sizeof(line), file) giới hạn 512 bytes; kiểm tra strchr(line, '\n') trả STATUS_ERR_FILE_IO
- **Kỹ thuật kiểm chứng Bước 3:** AddressSanitizer / Harness SIM-002
- **Rủi ro liên quan:** `RISK-007`
- **Bằng chứng cần tạo ở Bước 3:** Harness SIM-002 oversized line execution log

### REQ-11: Allocation động do ProductList/Inventory sở hữu phải có vòng đời rõ ràng; không làm mất con trỏ sở hữu còn sống và phải giải phóng ở thời điểm kết thúc vòng đời.
- **Danh mục:** `memory` | **Mức ưu tiên:** `HIGH` | **Phạm vi:** Quản lý quyền sở hữu bộ nhớ heap và giải phóng tài nguyên
- **Tập tin nguồn:** `source/src/product.c` | **Hàm C:** `product_list_init, product_list_free, inventory_free` | **Dòng mã:** `9-24, 27-34`
- **Kiểu dữ liệu:** `ProductList, Inventory` | **Đối tượng bộ nhớ:** `list->items`
- **Tiền điều kiện (Preconditions):** Danh sách được khởi tạo hợp lệ
- **Hậu điều kiện (Postconditions):** Tại thời điểm kết thúc, free(items) được gọi và con trỏ được đặt về NULL; 0 byte rò rỉ
- **Bất biến an toàn (Safety Invariant):** `AG(cleanup_done -> list_freed)`
- **Điều kiện rủi ro vi phạm (Unsafe Condition):** `Mất con trỏ sở hữu trước khi free (Memory Leak - CWE-401)`
- **Cơ chế phòng vệ hiện tại (Existing Guards):** Mô hình Single Ownership: chỉ có ProductList sở hữu items; dọn dẹp tại product_list_free
- **Kỹ thuật kiểm chứng Bước 3:** LeakSanitizer (LSan) / Valgrind / CBMC
- **Rủi ro liên quan:** `RISK-003, RISK-005`
- **Bằng chứng cần tạo ở Bước 3:** LSan zero-leak report sau 17 smoke tests

### REQ-12: Khi realloc thất bại, allocation cũ phải được giữ nguyên quyền sở hữu; khi thành công, con trỏ sở hữu và capacity phải cập nhật đúng vùng nhớ mới.
- **Danh mục:** `memory` | **Mức ưu tiên:** `HIGH` | **Phạm vi:** An toàn cấp phát lại bộ nhớ heap khi gặp lỗi
- **Tập tin nguồn:** `source/src/product.c` | **Hàm C:** `add_product` | **Dòng mã:** `119-125`
- **Kiểu dữ liệu:** `Product*, size_t` | **Đối tượng bộ nhớ:** `new_items, list->items, list->capacity`
- **Tiền điều kiện (Preconditions):** list != NULL, count == capacity
- **Hậu điều kiện (Postconditions):** Nếu realloc NULL: list->items cũ còn nguyên; nếu thành công: list->items trỏ vùng mới, capacity = new_cap
- **Bất biến an toàn (Safety Invariant):** `AG(realloc_failed -> owner_valid) && AG(realloc_done -> owner_valid)`
- **Điều kiện rủi ro vi phạm (Unsafe Condition):** `list->items bị gán thành NULL khi realloc thất bại (Mất dấu vùng nhớ cũ)`
- **Cơ chế phòng vệ hiện tại (Existing Guards):** Sử dụng biến con trỏ tạm new_items hứng kết quả realloc trước khi gán đè
- **Kỹ thuật kiểm chứng Bước 3:** CTL Model Checking / CBMC / Fault Injection SIM-001
- **Rủi ro liên quan:** `RISK-003`
- **Bằng chứng cần tạo ở Bước 3:** NuSMV CTL-04/05 check & CBMC proof

### REQ-13: Một allocation chỉ được free tối đa một lần; cleanup phải cập nhật trạng thái/con trỏ để ngăn double-free khi gọi lại.
- **Danh mục:** `memory` | **Mức ưu tiên:** `HIGH` | **Phạm vi:** Ngăn chặn giải phóng kép vùng nhớ
- **Tập tin nguồn:** `source/src/product.c` | **Hàm C:** `product_list_free` | **Dòng mã:** `27-34`
- **Kiểu dữ liệu:** `ProductList` | **Đối tượng bộ nhớ:** `list->items`
- **Tiền điều kiện (Preconditions):** product_list_free được gọi lần 1 hoặc lần 2
- **Hậu điều kiện (Postconditions):** list->items == NULL, list->count == 0, list->capacity == 0
- **Bất biến an toàn (Safety Invariant):** `AG(!(list_live & list_freed)) && AG(list_freed -> !free_called)`
- **Điều kiện rủi ro vi phạm (Unsafe Condition):** `Gọi free(items) khi items đã được free trước đó (Double Free - CWE-415)`
- **Cơ chế phòng vệ hiện tại (Existing Guards):** Kiểm tra if (list != NULL && list->items != NULL) và gán list->items = NULL ngay sau free
- **Kỹ thuật kiểm chứng Bước 3:** AddressSanitizer / CTL Model Checking
- **Rủi ro liên quan:** `RISK-005`
- **Bằng chứng cần tạo ở Bước 3:** ASan double-free regression test log

### REQ-14: Không được dereference con trỏ Product/vùng nhớ ProductList sau free hoặc sau khi realloc làm con trỏ cũ mất hiệu lực.
- **Danh mục:** `memory` | **Mức ưu tiên:** `HIGH` | **Phạm vi:** Vòng đời con trỏ mượn và ngăn chặn Use-After-Free
- **Tập tin nguồn:** `source/src/product.c` | **Hàm C:** `find_product_by_id, add_product` | **Dòng mã:** `223-233, 119`
- **Kiểu dữ liệu:** `Product*` | **Đối tượng bộ nhớ:** `Con trỏ mượn trỏ vào mảng items`
- **Tiền điều kiện (Preconditions):** Lấy con trỏ phần tử qua find_product_by_id
- **Hậu điều kiện (Postconditions):** Không dereference con trỏ sau khi danh sách mở rộng hoặc bị giải phóng
- **Bất biến an toàn (Safety Invariant):** `AG(ptr_invalid -> !ptr_deref)`
- **Điều kiện rủi ro vi phạm (Unsafe Condition):** `Dereference con trỏ trỏ vào vùng nhớ heap cũ đã bị realloc giải phóng (CWE-416)`
- **Cơ chế phòng vệ hiện tại (Existing Guards):** Kiến trúc v0 không lưu trữ con trỏ mượn qua các lời gọi add_product
- **Kỹ thuật kiểm chứng Bước 3:** Harness SIM-001 / Dynamic Contract Checking
- **Rủi ro liên quan:** `RISK-004`
- **Bằng chứng cần tạo ở Bước 3:** Harness SIM-001 pointer invalidation log

### REQ-15: API ProductList/Inventory phải kiểm tra con trỏ NULL theo hợp đồng hàm trước khi dereference đối tượng hoặc buffer vào/ra.
- **Danh mục:** `memory` | **Mức ưu tiên:** `HIGH` | **Phạm vi:** Bảo vệ biên API trước con trỏ NULL
- **Tập tin nguồn:** `source/src/product.c` | **Hàm C:** `product_list_init, add_product, update_product, delete_product, find_product_by_id` | **Dòng mã:** `10, 71, 142, 201, 224`
- **Kiểu dữ liệu:** `const ProductList*, Product*, const char*` | **Đối tượng bộ nhớ:** `Con trỏ tham số vào`
- **Tiền điều kiện (Preconditions):** Tham số đầu vào là con trỏ
- **Hậu điều kiện (Postconditions):** Nếu tham số == NULL, trả về mã lỗi STATUS_ERR_NULL_PTR lập tức
- **Bất biến an toàn (Safety Invariant):** `ptr == NULL -> !dereference(ptr)`
- **Điều kiện rủi ro vi phạm (Unsafe Condition):** `Dereference con trỏ NULL gây crash SIGSEGV (CWE-476)`
- **Cơ chế phòng vệ hiện tại (Existing Guards):** 100% các hàm API đều có if (list == NULL || ...) return STATUS_ERR_NULL_PTR; ở đầu hàm
- **Kỹ thuật kiểm chứng Bước 3:** CBMC Pointer Check / Unit Tests
- **Rủi ro liên quan:** `None (Đã được guard đầy đủ trên toàn bộ API)`
- **Bằng chứng cần tạo ở Bước 3:** Unit tests null pointer checks report

### REQ-16: Mảng tokens, results, summaries và vùng nhớ dùng với memmove phải kiểm soát số phần tử, số byte, giới hạn nguồn/đích; không truy cập ngoài vùng nhớ đã cấp phát.
- **Danh mục:** `memory` | **Mức ưu tiên:** `HIGH` | **Phạm vi:** Giới hạn các mảng con trỏ, mảng kết quả, mảng báo cáo và memmove
- **Tập tin nguồn:** `source/src/file_io.c` | **Hàm C:** `parse_product_line, calculate_category_summaries, delete_product` | **Dòng mã:** `47, 52 (file_io.c); 72-74 (report.c); 212 (product.c)`
- **Kiểu dữ liệu:** `char *tokens[8], CategorySummary summaries[32], memmove` | **Đối tượng bộ nhớ:** `tokens, summaries, list->items`
- **Tiền điều kiện (Preconditions):** Thao tác trên mảng cố định hoặc dồn mảng bằng memmove
- **Hậu điều kiện (Postconditions):** Chỉ số truy cập < kích thước tối đa đã cấp phát
- **Bất biến an toàn (Safety Invariant):** `token_count < 8 && out_count < max_summaries && (list->count - i - 1) >= 0`
- **Điều kiện rủi ro vi phạm (Unsafe Condition):** `Truy cập vượt quá biên mảng (CWE-121 / CWE-122)`
- **Cơ chế phòng vệ hiện tại (Existing Guards):** file_io.c có token_count < 8; report.c có if (*out_count >= max_summaries) return STATUS_ERR_OVERFLOW; product.c tính đúng byte memmove
- **Kỹ thuật kiểm chứng Bước 3:** AddressSanitizer / CBMC Array Bounds
- **Rủi ro liên quan:** `RISK-008, RISK-010`
- **Bằng chứng cần tạo ở Bước 3:** Harness SIM-002 token bounds & CBMC array proof

### REQ-17: Parser phải chấp nhận đúng định dạng 6 trường hoặc định dạng tương thích 4 trường; các dòng sai số trường/độ dài/định dạng phải bị từ chối mà không gây hỏng bộ nhớ Inventory.
- **Danh mục:** `functional` | **Mức ưu tiên:** `HIGH` | **Phạm vi:** Bộ phân tích cú pháp tệp dữ liệu kho
- **Tập tin nguồn:** `source/src/file_io.c` | **Hàm C:** `parse_product_line` | **Dòng mã:** `46-92`
- **Kiểu dữ liệu:** `char *line, Product *product` | **Đối tượng bộ nhớ:** `product struct sau khi parse`
- **Tiền điều kiện (Preconditions):** Dòng dữ liệu đầu vào từ tệp
- **Hậu điều kiện (Postconditions):** Nếu dòng hợp lệ (4 hoặc 6 trường): trả STATUS_SUCCESS; nếu sai: trả STATUS_ERR_FILE_IO mà không làm hỏng dữ liệu
- **Bất biến an toàn (Safety Invariant):** `valid_format(line) <-> parse_success`
- **Điều kiện rủi ro vi phạm (Unsafe Condition):** `Dòng sai định dạng gây crash hoặc tạo sản phẩm có trường rác trong kho`
- **Cơ chế phòng vệ hiện tại (Existing Guards):** Rẽ nhánh tường minh token_count == 6 và token_count == 4; kiểm tra strlen từng trường; fallback an toàn
- **Kỹ thuật kiểm chứng Bước 3:** Fuzzing / Test Corpus Regression (Smoke Test 12, 13)
- **Rủi ro liên quan:** `RISK-007, RISK-008, RISK-009`
- **Bằng chứng cần tạo ở Bước 3:** Smoke Test Case 12 & 13 pass logs

### REQ-18: Quantity/price không hợp lệ, vượt miền số hoặc không hữu hạn (NaN/Infinity) phải bị từ chối trước cập nhật kho và trước phép tính báo cáo có liên quan.
- **Danh mục:** `functional` | **Mức ưu tiên:** `HIGH` | **Phạm vi:** Xác thực dữ liệu số học đầu vào
- **Tập tin nguồn:** `source/src/validation.c` | **Hàm C:** `validate_quantity, validate_price, parse_int, parse_price` | **Dòng mã:** `150-170, 180-210 (validation.c); 20-44 (file_io.c)`
- **Kiểu dữ liệu:** `int quantity, double price` | **Đối tượng bộ nhớ:** `product->quantity, product->price`
- **Tiền điều kiện (Preconditions):** Giá trị số đọc từ tệp hoặc người dùng nhập
- **Hậu điều kiện (Postconditions):** quantity >= 0, price >= 0.0, isfinite(price) == true
- **Bất biến an toàn (Safety Invariant):** `quantity >= 0 && price >= 0.0 && !isnan(price) && !isinf(price)`
- **Điều kiện rủi ro vi phạm (Unsafe Condition):** `Số âm, giá trị NaN hoặc Inf lọt vào kho dữ liệu`
- **Cơ chế phòng vệ hiện tại (Existing Guards):** parse_int dùng strtol kiểm tra ERANGE và INT_MAX; parse_price dùng strtod kiểm tra !isfinite(parsed)
- **Kỹ thuật kiểm chứng Bước 3:** Unit Tests / CBMC IEEE-754 Validation
- **Rủi ro liên quan:** `RISK-009`
- **Bằng chứng cần tạo ở Bước 3:** Unit tests validation results

