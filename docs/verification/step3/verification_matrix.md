# MA TRẬN KẾT QUẢ KIỂM CHỨNG TOÀN DIỆN 18 YÊU CẦU AN TOÀN BỘ NHỚ (18 REQs)

**Dự án:** Kiểm chứng an toàn bộ nhớ cho chương trình C mô phỏng quản lý kho
**Nguồn đặc tả chuẩn:** `docs/verification/step2/requirement_specifications.md` và `docs/verification/step2/req_traceability.csv`
**Commit Baseline:** `44202070c68002d9af068374478cdc83098dd4d8`
**Trạng thái:** Đã hiệu chỉnh nghiêm ngặt theo kết luận kiểm toán độc lập

---

## 1. BẢNG TỔNG HỢP MA TRẬN KIỂM CHỨNG 18 REQs

| REQ ID | Nội dung Đặc tả Nguyên văn | Risk ID | Module & Hàm C | Phương pháp & Công cụ | Trạng thái Kết quả | Bằng chứng kiểm chứng |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| **REQ-1** | ProductList luôn thỏa count <= capacity; khi count > 0 thì items khác NULL và trỏ tới vùng nhớ còn hiệu lực. | `RISK-001, RISK-002` | `product_list_init, add_product, delete_product` | GCC Harness Testing (Bounded Contract Assertions) (GCC Harness Testing (CBMC is TOOL_UNAVAILABLE)) | **VERIFIED_WITHIN_HARNESS_SCOPE** | [`results/step3/cbmc/cbmc_001_product_list_invariant.log`](file:///C:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/results/step3/cbmc/cbmc_001_product_list_invariant.log) |
| **REQ-2** | Trước khi mở rộng ProductList bằng capacity * 2, phải kiểm tra capacity <= SIZE_MAX / 2 để tránh tràn số size_t. | `RISK-001` | `add_product` | SMT Bit-Vector Arithmetic Solver (Z3 Solver) | **PARTIALLY_VERIFIED** | [`results/step3/smt/smt_001a_capacity_growth.log, results/step3/smt/smt_001b_capacity_growth_reach.log`](file:///C:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/results/step3/smt/smt_001a_capacity_growth.log) |
| **REQ-3** | Trước realloc, phải kiểm tra new_cap <= SIZE_MAX / sizeof(Product) để phép nhân kích thước cấp phát không bị tràn. | `RISK-002` | `add_product` | SMT Bit-Vector Arithmetic Solver (Z3 Solver) | **PARTIALLY_VERIFIED** | [`results/step3/smt/smt_002a_realloc_size.log, results/step3/smt/smt_002b_realloc_size_guard.log`](file:///C:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/results/step3/smt/smt_002a_realloc_size.log) |
| **REQ-4** | Trong import_stock, chỉ cộng khi add_quantity > 0, tồn kho hiện tại hợp lệ và current_stock <= INT_MAX - add_quantity; nếu không phải từ chối trước khi cập nhật. | `None (Đã được guard hoàn toàn trong baseline v0)` | `import_stock` | SMT Bit-Vector & GCC Harness Testing (Z3 Solver & GCC Harness Testing) | **VERIFIED_WITHIN_SCOPE** | [`results/step3/smt/smt_003b_stock_addition_guard.log, results/step3/cbmc/cbmc_005_stock_safety.log`](file:///C:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/results/step3/smt/smt_003b_stock_addition_guard.log) |
| **REQ-5** | Trong export_stock, số lượng xuất phải > 0 và <= tồn kho hiện tại; không được tạo ra tồn kho âm hoặc cập nhật khi điều kiện không thỏa. | `None (Đã được phòng vệ an toàn)` | `export_stock` | GCC Harness Testing & SMT Z3 (GCC Harness Testing (CBMC is TOOL_UNAVAILABLE) & Z3) | **VERIFIED_WITHIN_SCOPE** | [`results/step3/cbmc/cbmc_005_stock_safety.log`](file:///C:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/results/step3/cbmc/cbmc_005_stock_safety.log) |
| **REQ-6** | Mọi truy cập ProductList.items[i] trong thêm/sửa/xóa/tìm kiếm/báo cáo phải thỏa i < count <= capacity và items còn hiệu lực. | `None (Đã được kiểm tra giới hạn trong v0)` | `find_product_by_id, update_product, delete_product` | GCC Harness Testing (Bounded Contract Assertions) (GCC Harness Testing (CBMC is TOOL_UNAVAILABLE)) | **VERIFIED_WITHIN_HARNESS_SCOPE** | [`results/step3/cbmc/cbmc_002_array_bounds.log`](file:///C:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/results/step3/cbmc/cbmc_002_array_bounds.log) |
| **REQ-7** | Với đầu vào hữu hạn và thao tác I/O trả về, mọi thao tác thêm/sửa/xóa/nhập/xuất đã bắt đầu phải kết thúc ở trạng thái thành công hoặc từ chối có kiểm soát; không lặp vô hạn. | `None` | `add_product, update_product, delete_product, import_stock, export_stock` | Custom CTL Model Checker (Custom CTL Model Checker (NuSMV is TOOL_UNAVAILABLE)) | **VERIFIED_WITHIN_MODEL_SCOPE** | [`results/step3/ctl/ctl_verification_results.json`](file:///C:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/results/step3/ctl/ctl_verification_results.json) |
| **REQ-8** | Với tệp hữu hạn và thao tác đọc trả về, quá trình đọc/phân tích dữ liệu sản phẩm phải kết thúc ở trạng thái chấp nhận hoặc từ chối có kiểm soát; không lặp vô hạn. | `None` | `load_products_from_file` | Custom CTL Model Checker & C Simulation SIM-002 (Custom CTL Model Checker (NuSMV is TOOL_UNAVAILABLE) & GCC) | **VERIFIED_WITHIN_SCOPE** | [`results/step3/ctl/ctl_verification_results.json, results/step3/simulation/sim002_parser_limits.log`](file:///C:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/results/step3/ctl/ctl_verification_results.json) |
| **REQ-9** | Các trường chuỗi cố định của Product (id, name, category, unit) phải được kiểm tra độ dài trước sao chép, không ghi vượt buffer và luôn kết thúc bằng ký tự \0. | `RISK-006` | `safe_string_copy, safe_strcpy` | Unit Test Suite & Dynamic Canary Check (GCC Unit Test Runner & Canary Tracker) | **VERIFIED_WITHIN_SCOPE** | [`results/step3/leaks/unit_test_run.log`](file:///C:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/results/step3/leaks/unit_test_run.log) |
| **REQ-10** | Input bàn phím và dòng tệp vượt kích thước buffer phải được tiêu thụ/từ chối có kiểm soát mà không phát sinh đọc/ghi ngoài vùng nhớ hợp lệ. | `RISK-007` | `load_products_from_file` | C Boundary Simulation SIM-002 (GCC C Simulation Harness) | **VERIFIED_WITHIN_SCOPE** | [`results/step3/simulation/sim002_parser_limits.log`](file:///C:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/results/step3/simulation/sim002_parser_limits.log) |
| **REQ-11** | Allocation động do ProductList/Inventory sở hữu phải có vòng đời rõ ràng; không làm mất con trỏ sở hữu còn sống và phải giải phóng ở thời điểm kết thúc vòng đời. | `RISK-003, RISK-005` | `product_list_init, product_list_free, inventory_free` | Custom Canary Memory Tracker (Allocation Ledger) (Canary Memory Tracker (Valgrind/LSan is TOOL_UNAVAILABLE)) | **VERIFIED_WITHIN_TEST_SCOPE** | [`results/step3/leaks/memory_leak_report.log`](file:///C:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/results/step3/leaks/memory_leak_report.log) |
| **REQ-12** | Khi realloc thất bại, allocation cũ phải được giữ nguyên quyền sở hữu; khi thành công, con trỏ sở hữu và capacity phải cập nhật đúng vùng nhớ mới. | `RISK-003` | `add_product` | Mock Allocator Fault Injection (SIM-001.2) & GCC Harness Testing (GCC C Simulation Harness) | **VERIFIED_WITHIN_SCOPE** | [`results/step3/simulation/sim001_heap_lifecycle.log, results/step3/cbmc/cbmc_003_realloc_failure.log`](file:///C:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/results/step3/simulation/sim001_heap_lifecycle.log) |
| **REQ-13** | Một allocation chỉ được free tối đa một lần; cleanup phải cập nhật trạng thái/con trỏ để ngăn double-free khi gọi lại. | `RISK-005` | `product_list_free` | GCC Harness Testing (Bounded Contract Assertions) (GCC Harness Testing (CBMC is TOOL_UNAVAILABLE) & Canary Tracker) | **VERIFIED_WITHIN_SCOPE** | [`results/step3/cbmc/cbmc_004_pointer_safety.log, results/step3/leaks/memory_leak_report.log`](file:///C:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/results/step3/cbmc/cbmc_004_pointer_safety.log) |
| **REQ-14** | Không được dereference con trỏ Product/vùng nhớ ProductList sau free hoặc sau khi realloc làm con trỏ cũ mất hiệu lực. | `RISK-004` | `find_product_by_id, add_product` | Custom CTL Model Checker & C Simulation SIM-001.3 (Custom CTL Model Checker (NuSMV is TOOL_UNAVAILABLE) & GCC) | **VERIFIED_WITHIN_SCOPE** | [`results/step3/ctl/ctl_verification_results.json, results/step3/simulation/sim001_heap_lifecycle.log`](file:///C:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/results/step3/ctl/ctl_verification_results.json) |
| **REQ-15** | API ProductList/Inventory phải kiểm tra con trỏ NULL theo hợp đồng hàm trước khi dereference đối tượng hoặc buffer vào/ra. | `None (Đã được guard đầy đủ trên toàn bộ API)` | `product_list_init, add_product, update_product, delete_product, find_product_by_id` | GCC Harness Testing (Bounded Contract Assertions) (GCC Harness Testing (CBMC is TOOL_UNAVAILABLE)) | **VERIFIED_WITHIN_SCOPE** | [`results/step3/cbmc/cbmc_004_pointer_safety.log`](file:///C:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/results/step3/cbmc/cbmc_004_pointer_safety.log) |
| **REQ-16** | Mảng tokens, results, summaries và vùng nhớ dùng với memmove phải kiểm soát số phần tử, số byte, giới hạn nguồn/đích; không truy cập ngoài vùng nhớ đã cấp phát. | `RISK-008, RISK-010` | `parse_product_line, calculate_category_summaries, delete_product` | Negative Control SIM-003 & GCC Harness Testing (GCC Stack Protector & GCC Harness Testing) | **VERIFIED_WITHIN_SCOPE** | [`results/step3/simulation/sim003_negative_control.log, results/step3/cbmc/cbmc_002_array_bounds.log`](file:///C:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/results/step3/simulation/sim003_negative_control.log) |
| **REQ-17** | Parser phải chấp nhận đúng định dạng 6 trường hoặc định dạng tương thích 4 trường; các dòng sai số trường/độ dài/định dạng phải bị từ chối mà không gây hỏng bộ nhớ Inventory. | `RISK-007, RISK-008, RISK-009` | `parse_product_line` | C Simulation Harness SIM-002 (10 Corpus Files) (GCC C Simulation Harness) | **VERIFIED_WITHIN_SCOPE** | [`results/step3/simulation/sim002_parser_limits.log`](file:///C:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/results/step3/simulation/sim002_parser_limits.log) |
| **REQ-18** | Quantity/price không hợp lệ, vượt miền số hoặc không hữu hạn (NaN/Infinity) phải bị từ chối trước cập nhật kho và trước phép tính báo cáo có liên quan. | `RISK-009` | `validate_quantity, validate_price, parse_int, parse_price` | SIM-002 & Unit Test Suite (GCC C Simulation & Unit Test Runner) | **VERIFIED_WITHIN_SCOPE** | [`results/step3/simulation/sim002_parser_limits.log`](file:///C:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/results/step3/simulation/sim002_parser_limits.log) |

---

## 2. CHI TIẾT ĐẦY ĐỦ 21 THUỘC TÍNH CỦA MA TRẬN KIỂM CHỨNG

### REQ-1: ProductList luôn thỏa count <= capacity; khi count > 0 thì items khác NULL và trỏ tới vùng nhớ còn hiệu lực.

- **Risk ID liên quan:** `RISK-001, RISK-002`
- **Tập tin nguồn C:** `source/src/product.c` | **Hàm C:** `product_list_init, add_product, delete_product`
- **Bất biến / Thuộc tính an toàn (Property):** `AG(list_valid -> (list->count <= list->capacity && (list->count == 0 || list->items != NULL)))`
- **Tiền điều kiện (Preconditions):** list != NULL
- **Điều kiện vi phạm (Unsafe Condition):** `count > capacity || (count > 0 && items == NULL)`
- **Phương pháp kiểm chứng:** GCC Harness Testing (Bounded Contract Assertions)
- **Công cụ & Phiên bản:** GCC Harness Testing (CBMC is TOOL_UNAVAILABLE) (GCC 16.1.0)
- **Harness / Tệp mô hình:** `verification/step3/cbmc/harness_cbmc_001_product_list_invariant.c`
- **Đầu vào thử nghiệm (Test Input):** Khởi tạo, thêm 18 phần tử, xóa 5 phần tử
- **Giả định mô hình (Assumptions):** Allocation ban đầu và realloc thành công trong phạm vi heap mô phỏng
- **Giới hạn kiểm chứng (Bound):** `k = 20`
- **Kết quả nghiệm thu:** **`VERIFIED_WITHIN_HARNESS_SCOPE`**
- **Phản ví dụ (Counterexample):** 0 (Không phát hiện vi phạm trong phạm vi harness)
- **Đường dẫn bằng chứng (Evidence Path):** [`results/step3/cbmc/cbmc_001_product_list_invariant.log`](file:///C:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/results/step3/cbmc/cbmc_001_product_list_invariant.log)
- **Trạng thái độ phủ (Coverage Status):** `COVERED_IN_BOUND`
- **Giới hạn kiểm chứng (Limitation):** Kiểm tra bounded contract assertions qua GCC, không phải kiểm chứng mô hình biểu tượng bằng CBMC native.
- **Kết luận kỹ thuật:** Bất biến container count <= capacity được duy trì trong phạm vi các kịch bản thêm/xóa của harness.

### REQ-2: Trước khi mở rộng ProductList bằng capacity * 2, phải kiểm tra capacity <= SIZE_MAX / 2 để tránh tràn số size_t.

- **Risk ID liên quan:** `RISK-001`
- **Tập tin nguồn C:** `source/src/product.c` | **Hàm C:** `add_product`
- **Bất biến / Thuộc tính an toàn (Property):** `capacity <= SIZE_MAX / 2`
- **Tiền điều kiện (Preconditions):** list != NULL, list->count >= list->capacity
- **Điều kiện vi phạm (Unsafe Condition):** `capacity > SIZE_MAX / 2 (0x8000000000000000)`
- **Phương pháp kiểm chứng:** SMT Bit-Vector Arithmetic Solver
- **Công cụ & Phiên bản:** Z3 Solver (5.1.0 - 64 bit)
- **Harness / Tệp mô hình:** `verification/step3/smt/smt_001a_capacity_growth.smt2, smt_001b_capacity_growth_reach.smt2`
- **Đầu vào thử nghiệm (Test Input):** Mô hình bit-vector 64-bit tự do và mô hình ràng buộc RAM <= 1TB
- **Giả định mô hình (Assumptions):** Biểu thức C: capacity * 2 sử dụng size_t 64-bit unsigned
- **Giới hạn kiểm chứng (Bound):** `Không giới hạn (Unbounded SMT BV-64)`
- **Kết quả nghiệm thu:** **`PARTIALLY_VERIFIED`**
- **Phản ví dụ (Counterexample):** SMT-001A: capacity = 0x8000000000000000 (Toán học); SMT-001B: UNSAT (Không khả đạt trong RAM vật lý)
- **Đường dẫn bằng chứng (Evidence Path):** [`results/step3/smt/smt_001a_capacity_growth.log, results/step3/smt/smt_001b_capacity_growth_reach.log`](file:///C:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/results/step3/smt/smt_001a_capacity_growth.log)
- **Trạng thái độ phủ (Coverage Status):** `COVERED_SMT`
- **Giới hạn kiểm chứng (Limitation):** Baseline v0 CHƯA CÓ guard tường minh capacity <= SIZE_MAX / 2 trong mã C; rủi ro được phân tích qua SMT và chuyển việc vá sang Bước 4.
- **Kết luận kỹ thuật:** SMT chứng minh mô hình toán học có nghiệm tràn; trên mã C baseline v0 yêu cầu chưa được cài đặt guard tường minh.

### REQ-3: Trước realloc, phải kiểm tra new_cap <= SIZE_MAX / sizeof(Product) để phép nhân kích thước cấp phát không bị tràn.

- **Risk ID liên quan:** `RISK-002`
- **Tập tin nguồn C:** `source/src/product.c` | **Hàm C:** `add_product`
- **Bất biến / Thuộc tính an toàn (Property):** `new_cap <= SIZE_MAX / sizeof(Product)`
- **Tiền điều kiện (Preconditions):** new_cap > 0, sizeof(Product) = 208 bytes
- **Điều kiện vi phạm (Unsafe Condition):** `new_cap > SIZE_MAX / 208 (88,686,269,585,142,075)`
- **Phương pháp kiểm chứng:** SMT Bit-Vector Arithmetic Solver
- **Công cụ & Phiên bản:** Z3 Solver (5.1.0 - 64 bit)
- **Harness / Tệp mô hình:** `verification/step3/smt/smt_002a_realloc_size.smt2, smt_002b_realloc_size_guard.smt2`
- **Đầu vào thử nghiệm (Test Input):** Mô hình nhân 128-bit vs trần 64-bit SIZE_MAX
- **Giả định mô hình (Assumptions):** LLP64 ABI, sizeof(Product) = 208 bytes
- **Giới hạn kiểm chứng (Bound):** `Không giới hạn (SMT QF_BV)`
- **Kết quả nghiệm thu:** **`PARTIALLY_VERIFIED`**
- **Phản ví dụ (Counterexample):** SMT-002A: new_cap = #xeea85800eaaaaaaa (17,197,091,938,074,536,618 > 88,686,269,585,142,075; tích 128-bit tràn)
- **Đường dẫn bằng chứng (Evidence Path):** [`results/step3/smt/smt_002a_realloc_size.log, results/step3/smt/smt_002b_realloc_size_guard.log`](file:///C:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/results/step3/smt/smt_002a_realloc_size.log)
- **Trạng thái độ phủ (Coverage Status):** `COVERED_SMT`
- **Giới hạn kiểm chứng (Limitation):** Baseline v0 CHƯA CÓ guard tường minh new_cap <= SIZE_MAX / 208 trong mã C; chuyển việc bổ sung guard phòng vệ sang Bước 4.
- **Kết luận kỹ thuật:** Z3 xác nhận mô hình toán học tràn khi thiếu guard (SMT-002A SAT) và an toàn khi có guard (SMT-002B UNSAT); code baseline v0 chưa có guard.

### REQ-4: Trong import_stock, chỉ cộng khi add_quantity > 0, tồn kho hiện tại hợp lệ và current_stock <= INT_MAX - add_quantity; nếu không phải từ chối trước khi cập nhật.

- **Risk ID liên quan:** `None (Đã được guard hoàn toàn trong baseline v0)`
- **Tập tin nguồn C:** `source/src/inventory.c` | **Hàm C:** `import_stock`
- **Bất biến / Thuộc tính an toàn (Property):** `current_stock <= INT_MAX - add_quantity && add_quantity > 0 && current_stock >= 0`
- **Tiền điều kiện (Preconditions):** inventory != NULL, product tồn tại trong kho
- **Điều kiện vi phạm (Unsafe Condition):** `current_stock > INT_MAX - add_quantity (Tràn số nguyên có dấu)`
- **Phương pháp kiểm chứng:** SMT Bit-Vector & GCC Harness Testing
- **Công cụ & Phiên bản:** Z3 Solver & GCC Harness Testing (Z3 5.1.0, GCC 16.1.0)
- **Harness / Tệp mô hình:** `verification/step3/smt/smt_003b_stock_addition_guard.smt2, verification/step3/cbmc/harness_cbmc_005_stock_safety.c`
- **Đầu vào thử nghiệm (Test Input):** current_stock = INT_MAX, add = 1; và các giá trị biên
- **Giả định mô hình (Assumptions):** Sử dụng hàm check_addition_overflow() trong validation.c
- **Giới hạn kiểm chứng (Bound):** `k = 10`
- **Kết quả nghiệm thu:** **`VERIFIED_WITHIN_SCOPE`**
- **Phản ví dụ (Counterexample):** 0 khi có guard (SMT-003B UNSAT; Harness PASS)
- **Đường dẫn bằng chứng (Evidence Path):** [`results/step3/smt/smt_003b_stock_addition_guard.log, results/step3/cbmc/cbmc_005_stock_safety.log`](file:///C:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/results/step3/smt/smt_003b_stock_addition_guard.log)
- **Trạng thái độ phủ (Coverage Status):** `COVERED`
- **Giới hạn kiểm chứng (Limitation):** Nếu bỏ guard sẽ có model tràn (SMT-003A SAT: stock=INT_MAX, add=1); baseline v0 đã có guard này.
- **Kết luận kỹ thuật:** Hàm check_addition_overflow() trong baseline v0 ngăn chặn thành công nguy cơ tràn số nguyên có dấu.

### REQ-5: Trong export_stock, số lượng xuất phải > 0 và <= tồn kho hiện tại; không được tạo ra tồn kho âm hoặc cập nhật khi điều kiện không thỏa.

- **Risk ID liên quan:** `None (Đã được phòng vệ an toàn)`
- **Tập tin nguồn C:** `source/src/inventory.c` | **Hàm C:** `export_stock`
- **Bất biến / Thuộc tính an toàn (Property):** `export_quantity > 0 && export_quantity <= current_stock`
- **Tiền điều kiện (Preconditions):** inventory != NULL, product tồn tại trong kho
- **Điều kiện vi phạm (Unsafe Condition):** `export_quantity <= 0 || export_quantity > current_stock || new_stock < 0`
- **Phương pháp kiểm chứng:** GCC Harness Testing & SMT Z3
- **Công cụ & Phiên bản:** GCC Harness Testing (CBMC is TOOL_UNAVAILABLE) & Z3 (GCC 16.1.0, Z3 5.1.0)
- **Harness / Tệp mô hình:** `verification/step3/cbmc/harness_cbmc_005_stock_safety.c, verification/step3/smt/smt_004b_category_total_guard.smt2`
- **Đầu vào thử nghiệm (Test Input):** Xuất 15 cái từ kho 10 cái (từ chối); Xuất -5 cái (từ chối); Xuất 5 cái (thành công)
- **Giả định mô hình (Assumptions):** Hàm export_stock kiểm tra điều kiện trước khi trừ
- **Giới hạn kiểm chứng (Bound):** `k = 10`
- **Kết quả nghiệm thu:** **`VERIFIED_WITHIN_SCOPE`**
- **Phản ví dụ (Counterexample):** 0 (Không phát hiện vi phạm)
- **Đường dẫn bằng chứng (Evidence Path):** [`results/step3/cbmc/cbmc_005_stock_safety.log`](file:///C:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/results/step3/cbmc/cbmc_005_stock_safety.log)
- **Trạng thái độ phủ (Coverage Status):** `COVERED`
- **Giới hạn kiểm chứng (Limitation):** Kiểm chứng trong phạm vi nghiệp vụ kho đơn luồng.
- **Kết luận kỹ thuật:** Hàm từ chối khi số lượng xuất không hợp lệ; tồn kho không bị âm trong các kịch bản kiểm thử.

### REQ-6: Mọi truy cập ProductList.items[i] trong thêm/sửa/xóa/tìm kiếm/báo cáo phải thỏa i < count <= capacity và items còn hiệu lực.

- **Risk ID liên quan:** `None (Đã được kiểm tra giới hạn trong v0)`
- **Tập tin nguồn C:** `source/src/product.c` | **Hàm C:** `find_product_by_id, update_product, delete_product`
- **Bất biến / Thuộc tính an toàn (Property):** `forall i in [0, count): &items[i] nằm trong phạm vi cấp phát hợp lệ`
- **Tiền điều kiện (Preconditions):** list != NULL, count <= capacity
- **Điều kiện vi phạm (Unsafe Condition):** `Truy cập items[i] với i >= count hoặc items == NULL khi count > 0`
- **Phương pháp kiểm chứng:** GCC Harness Testing (Bounded Contract Assertions)
- **Công cụ & Phiên bản:** GCC Harness Testing (CBMC is TOOL_UNAVAILABLE) (GCC 16.1.0)
- **Harness / Tệp mô hình:** `verification/step3/cbmc/harness_cbmc_002_array_bounds.c`
- **Đầu vào thử nghiệm (Test Input):** Truy cập duyệt mảng trong find_product_by_id và calculate_category_summaries
- **Giả định mô hình (Assumptions):** Vòng lặp duyệt tuần tự có điều kiện dừng i < list->count
- **Giới hạn kiểm chứng (Bound):** `k = 40`
- **Kết quả nghiệm thu:** **`VERIFIED_WITHIN_HARNESS_SCOPE`**
- **Phản ví dụ (Counterexample):** 0 (Không phát hiện truy cập ngoài ranh giới)
- **Đường dẫn bằng chứng (Evidence Path):** [`results/step3/cbmc/cbmc_002_array_bounds.log`](file:///C:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/results/step3/cbmc/cbmc_002_array_bounds.log)
- **Trạng thái độ phủ (Coverage Status):** `COVERED_IN_BOUND`
- **Giới hạn kiểm chứng (Limitation):** AddressSanitizer native chưa khả dụng trên host; kiểm tra dựa trên assertions và bounds checks.
- **Kết luận kỹ thuật:** Các vòng lặp duyệt mảng items[i] trong baseline v0 đều có guard i < count trong phạm vi harness.

### REQ-7: Với đầu vào hữu hạn và thao tác I/O trả về, mọi thao tác thêm/sửa/xóa/nhập/xuất đã bắt đầu phải kết thúc ở trạng thái thành công hoặc từ chối có kiểm soát; không lặp vô hạn.

- **Risk ID liên quan:** `None`
- **Tập tin nguồn C:** `source/src/product.c, source/src/inventory.c` | **Hàm C:** `add_product, update_product, delete_product, import_stock, export_stock`
- **Bất biến / Thuộc tính an toàn (Property):** `AG(product_op_processing -> AF(product_op_done | product_op_rejected))`
- **Tiền điều kiện (Preconditions):** Đầu vào hữu hạn, hàm được gọi
- **Điều kiện vi phạm (Unsafe Condition):** `EG(!product_op_done && !product_op_rejected) (Treo lặp vô hạn / Deadlock)`
- **Phương pháp kiểm chứng:** Custom CTL Model Checker
- **Công cụ & Phiên bản:** Custom CTL Model Checker (NuSMV is TOOL_UNAVAILABLE) (Step3 Embedded Model Checker (verify_ctl.py))
- **Harness / Tệp mô hình:** `verification/step3/ctl/inventory_model.smv, verification/step3/ctl/verify_ctl.py`
- **Đầu vào thử nghiệm (Test Input):** Mô hình chuyển tiếp trạng thái hệ thống con Product / Inventory
- **Giả định mô hình (Assumptions):** Mô hình hóa các nhánh rẽ thành công và từ chối là hữu hạn
- **Giới hạn kiểm chứng (Bound):** `Toàn bộ không gian 21 trạng thái`
- **Kết quả nghiệm thu:** **`VERIFIED_WITHIN_MODEL_SCOPE`**
- **Phản ví dụ (Counterexample):** 0 (CTL-07 đạt TRUE, Non-vacuous)
- **Đường dẫn bằng chứng (Evidence Path):** [`results/step3/ctl/ctl_verification_results.json`](file:///C:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/results/step3/ctl/ctl_verification_results.json)
- **Trạng thái độ phủ (Coverage Status):** `MODEL_COVERED_100%`
- **Giới hạn kiểm chứng (Limitation):** Kiểm chứng trên mô hình trạng thái trừu tượng; chưa bao hàm kiểm chứng dừng trên mã máy/CPU cycle.
- **Kết luận kỹ thuật:** Mô hình giao thức nghiệp vụ kho bảo đảm tính kết thúc (Liveness) và không có chu trình deadlock.

### REQ-8: Với tệp hữu hạn và thao tác đọc trả về, quá trình đọc/phân tích dữ liệu sản phẩm phải kết thúc ở trạng thái chấp nhận hoặc từ chối có kiểm soát; không lặp vô hạn.

- **Risk ID liên quan:** `None`
- **Tập tin nguồn C:** `source/src/file_io.c` | **Hàm C:** `load_products_from_file`
- **Bất biến / Thuộc tính an toàn (Property):** `AG(file_parsing -> AF(file_loaded | file_rejected))`
- **Tiền điều kiện (Preconditions):** Tệp dữ liệu hữu hạn, handle tệp hợp lệ
- **Điều kiện vi phạm (Unsafe Condition):** `EG(file_parsing) (Vòng lặp đọc tệp không dừng)`
- **Phương pháp kiểm chứng:** Custom CTL Model Checker & C Simulation SIM-002
- **Công cụ & Phiên bản:** Custom CTL Model Checker (NuSMV is TOOL_UNAVAILABLE) & GCC (verify_ctl.py, GCC 16.1.0)
- **Harness / Tệp mô hình:** `verification/step3/ctl/inventory_model.smv, tests/simulation/test_sim002_parser.c`
- **Đầu vào thử nghiệm (Test Input):** 10 tệp corpus kiểm thử (chuẩn, dị dạng, dài 600B, thiếu newline)
- **Giả định mô hình (Assumptions):** fgets gặp EOF hoặc newline sẽ trả về theo chuẩn C
- **Giới hạn kiểm chứng (Bound):** `21 trạng thái mô hình & 10 corpus tệp thực tế`
- **Kết quả nghiệm thu:** **`VERIFIED_WITHIN_SCOPE`**
- **Phản ví dụ (Counterexample):** 0 (CTL-08 TRUE; 10/10 corpus dừng và trả mã trạng thái)
- **Đường dẫn bằng chứng (Evidence Path):** [`results/step3/ctl/ctl_verification_results.json, results/step3/simulation/sim002_parser_limits.log`](file:///C:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/results/step3/ctl/ctl_verification_results.json)
- **Trạng thái độ phủ (Coverage Status):** `COVERED`
- **Giới hạn kiểm chứng (Limitation):** Giả định thao tác I/O của hệ điều hành không bị treo vĩnh viễn ở tầng driver phần cứng.
- **Kết luận kỹ thuật:** Quá trình đọc tệp kết thúc tất định ở trạng thái thành công hoặc từ chối có kiểm soát.

### REQ-9: Các trường chuỗi cố định của Product (id, name, category, unit) phải được kiểm tra độ dài trước sao chép, không ghi vượt buffer và luôn kết thúc bằng ký tự \0.

- **Risk ID liên quan:** `RISK-006`
- **Tập tin nguồn C:** `source/src/utils.c, source/src/product.c` | **Hàm C:** `safe_string_copy, safe_strcpy`
- **Bất biến / Thuộc tính an toàn (Property):** `dst[size - 1] == '\0' && strlen(dst) < size`
- **Tiền điều kiện (Preconditions):** dst != NULL, src != NULL, size > 0
- **Điều kiện vi phạm (Unsafe Condition):** `Ghi đè quá size byte hoặc thiếu ký tự null terminator`
- **Phương pháp kiểm chứng:** Unit Test Suite & Dynamic Canary Check
- **Công cụ & Phiên bản:** GCC Unit Test Runner & Canary Tracker (GCC 16.1.0)
- **Harness / Tệp mô hình:** `tests/test_utils.c, verification/step3/sanitizers/test_memory_audit.c`
- **Đầu vào thử nghiệm (Test Input):** Chuỗi dài hơn kích thước đệm đích; chuỗi rỗng; chuỗi có ký tự trắng
- **Giả định mô hình (Assumptions):** safe_strcpy cắt ngắn và gán null terminator cuối
- **Giới hạn kiểm chứng (Bound):** `Dữ liệu kiểm thử hữu hạn`
- **Kết quả nghiệm thu:** **`VERIFIED_WITHIN_SCOPE`**
- **Phản ví dụ (Counterexample):** 0 (Không phát hiện xâu không kết thúc bằng \0)
- **Đường dẫn bằng chứng (Evidence Path):** [`results/step3/leaks/unit_test_run.log`](file:///C:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/results/step3/leaks/unit_test_run.log)
- **Trạng thái độ phủ (Coverage Status):** `COVERED`
- **Giới hạn kiểm chứng (Limitation):** Cơ chế hiện tại thực hiện cắt ngắn chuỗi (truncation); Bước 4 xem xét việc trả mã lỗi tường minh.
- **Kết luận kỹ thuật:** Các thao tác sao chép xâu vào Product bảo đảm an toàn kích thước đệm và luôn có ký tự kết thúc \0.

### REQ-10: Input bàn phím và dòng tệp vượt kích thước buffer phải được tiêu thụ/từ chối có kiểm soát mà không phát sinh đọc/ghi ngoài vùng nhớ hợp lệ.

- **Risk ID liên quan:** `RISK-007`
- **Tập tin nguồn C:** `source/src/file_io.c` | **Hàm C:** `load_products_from_file`
- **Bất biến / Thuộc tính an toàn (Property):** `Dòng vượt quá LINE_BUFFER_SIZE (512) không làm tràn ngăn xếp`
- **Tiền điều kiện (Preconditions):** Đường dẫn tệp hợp lệ
- **Điều kiện vi phạm (Unsafe Condition):** `Ghi vượt 512 byte trên mảng buffer stack`
- **Phương pháp kiểm chứng:** C Boundary Simulation SIM-002
- **Công cụ & Phiên bản:** GCC C Simulation Harness (GCC 16.1.0)
- **Harness / Tệp mô hình:** `tests/simulation/test_sim002_parser.c`
- **Đầu vào thử nghiệm (Test Input):** tests/simulation/corpus/corpus_05_line_over_512.txt (dòng dài 600 byte)
- **Giả định mô hình (Assumptions):** fgets(buffer, sizeof(buffer), fp) giới hạn đọc đúng 512 byte
- **Giới hạn kiểm chứng (Bound):** `Kích thước tệp 600 bytes`
- **Kết quả nghiệm thu:** **`VERIFIED_WITHIN_SCOPE`**
- **Phản ví dụ (Counterexample):** 0 (Không có hiện tượng tràn bộ đệm stack)
- **Đường dẫn bằng chứng (Evidence Path):** [`results/step3/simulation/sim002_parser_limits.log`](file:///C:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/results/step3/simulation/sim002_parser_limits.log)
- **Trạng thái độ phủ (Coverage Status):** `COVERED`
- **Giới hạn kiểm chứng (Limitation):** Dòng dài hơn 512 byte bị cắt đoạn đọc; cần hoàn thiện cơ chế nuốt hết dòng thừa ở Bước 4.
- **Kết luận kỹ thuật:** Bộ đệm đọc tệp được giới hạn bởi sizeof(line_buffer) = 512, không xảy ra tràn ngăn xếp.

### REQ-11: Allocation động do ProductList/Inventory sở hữu phải có vòng đời rõ ràng; không làm mất con trỏ sở hữu còn sống và phải giải phóng ở thời điểm kết thúc vòng đời.

- **Risk ID liên quan:** `RISK-003, RISK-005`
- **Tập tin nguồn C:** `source/src/product.c, source/src/inventory.c` | **Hàm C:** `product_list_init, product_list_free, inventory_free`
- **Bất biến / Thuộc tính an toàn (Property):** `Active allocations at exit == 0 && Leak bytes == 0 trong kịch bản kiểm thử`
- **Tiền điều kiện (Preconditions):** ProductList / Inventory được khởi tạo hợp lệ
- **Điều kiện vi phạm (Unsafe Condition):** `Mất con trỏ sở hữu khi chưa free (Rò rỉ bộ nhớ động - Memory Leak)`
- **Phương pháp kiểm chứng:** Custom Canary Memory Tracker (Allocation Ledger)
- **Công cụ & Phiên bản:** Canary Memory Tracker (Valgrind/LSan is TOOL_UNAVAILABLE) (Step3 Native Canary Guard)
- **Harness / Tệp mô hình:** `verification/step3/sanitizers/test_memory_audit.c, verification/step3/sanitizers/memory_tracker.c`
- **Đầu vào thử nghiệm (Test Input):** Cấp phát 50 phần tử, mở rộng capacity 16->32->64, xóa 10 phần tử, giải phóng toàn bộ
- **Giả định mô hình (Assumptions):** Mọi lệnh malloc/realloc/free trong test đều đi qua memory tracker wrapper
- **Giới hạn kiểm chứng (Bound):** `Kịch bản kiểm thử 50 phần tử`
- **Kết quả nghiệm thu:** **`VERIFIED_WITHIN_TEST_SCOPE`**
- **Phản ví dụ (Counterexample):** 0 (0 bytes rò rỉ được ghi nhận trong kịch bản test_memory_audit)
- **Đường dẫn bằng chứng (Evidence Path):** [`results/step3/leaks/memory_leak_report.log`](file:///C:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/results/step3/leaks/memory_leak_report.log)
- **Trạng thái độ phủ (Coverage Status):** `COVERED_IN_TEST`
- **Giới hạn kiểm chứng (Limitation):** Valgrind và LeakSanitizer native là TOOL_UNAVAILABLE; kiểm tra dựa trên ledger đo kiểm nội bộ của test suite.
- **Kết luận kỹ thuật:** Vòng đời cấp phát động được giải phóng sạch sẽ trong kịch bản kiểm thử; không rò rỉ con trỏ.

### REQ-12: Khi realloc thất bại, allocation cũ phải được giữ nguyên quyền sở hữu; khi thành công, con trỏ sở hữu và capacity phải cập nhật đúng vùng nhớ mới.

- **Risk ID liên quan:** `RISK-003`
- **Tập tin nguồn C:** `source/src/product.c` | **Hàm C:** `add_product`
- **Bất biến / Thuộc tính an toàn (Property):** `(new_items == NULL) -> (list->items == old_items && list->capacity == old_cap && return == STATUS_ERR_MEMORY)`
- **Tiền điều kiện (Preconditions):** list != NULL, count >= capacity
- **Điều kiện vi phạm (Unsafe Condition):** `list->items bị gán thành NULL hoặc vùng nhớ cũ bị giải phóng khi realloc thất bại`
- **Phương pháp kiểm chứng:** Mock Allocator Fault Injection (SIM-001.2) & GCC Harness Testing
- **Công cụ & Phiên bản:** GCC C Simulation Harness (GCC 16.1.0)
- **Harness / Tệp mô hình:** `tests/simulation/test_sim001_heap.c, verification/step3/cbmc/harness_cbmc_003_realloc_failure.c`
- **Đầu vào thử nghiệm (Test Input):** Nạp 16 phần tử, kích hoạt Fault Injection ép realloc() trả NULL tại phần tử thứ 17
- **Giả định mô hình (Assumptions):** Mock allocator mô phỏng tình huống OOM cục bộ của hệ điều hành
- **Giới hạn kiểm chứng (Bound):** `k = 17`
- **Kết quả nghiệm thu:** **`VERIFIED_WITHIN_SCOPE`**
- **Phản ví dụ (Counterexample):** 0 (Hàm trả về STATUS_ERR_MEMORY = -8; con trỏ cũ và 16 phần tử nguyên vẹn)
- **Đường dẫn bằng chứng (Evidence Path):** [`results/step3/simulation/sim001_heap_lifecycle.log, results/step3/cbmc/cbmc_003_realloc_failure.log`](file:///C:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/results/step3/simulation/sim001_heap_lifecycle.log)
- **Trạng thái độ phủ (Coverage Status):** `COVERED`
- **Giới hạn kiểm chứng (Limitation):** Mô phỏng qua fault injection trong môi trường test; chưa kiểm tra trường hợp phân mảnh heap phức tạp.
- **Kết luận kỹ thuật:** Hàm add_product bảo toàn con trỏ cũ và dữ liệu khi realloc thất bại, trả về đúng mã lỗi -8.

### REQ-13: Một allocation chỉ được free tối đa một lần; cleanup phải cập nhật trạng thái/con trỏ để ngăn double-free khi gọi lại.

- **Risk ID liên quan:** `RISK-005`
- **Tập tin nguồn C:** `source/src/product.c` | **Hàm C:** `product_list_free`
- **Bất biến / Thuộc tính an toàn (Property):** `list->items == NULL && list->count == 0 && list->capacity == 0 sau free; free lần 2 là no-op`
- **Tiền điều kiện (Preconditions):** list != NULL hoặc list == NULL
- **Điều kiện vi phạm (Unsafe Condition):** `free() trên cùng một con trỏ 2 lần (Double-Free CWE-415)`
- **Phương pháp kiểm chứng:** GCC Harness Testing (Bounded Contract Assertions)
- **Công cụ & Phiên bản:** GCC Harness Testing (CBMC is TOOL_UNAVAILABLE) & Canary Tracker (GCC 16.1.0)
- **Harness / Tệp mô hình:** `verification/step3/cbmc/harness_cbmc_004_pointer_safety.c, verification/step3/sanitizers/test_memory_audit.c`
- **Đầu vào thử nghiệm (Test Input):** Gọi product_list_free(NULL); gọi product_list_free(&list) 2 lần liên tiếp
- **Giả định mô hình (Assumptions):** product_list_free kiểm tra if (list && list->items) trước khi free và gán items = NULL
- **Giới hạn kiểm chứng (Bound):** `k = 17`
- **Kết quả nghiệm thu:** **`VERIFIED_WITHIN_SCOPE`**
- **Phản ví dụ (Counterexample):** 0 (Không phát sinh crash, không có double-free)
- **Đường dẫn bằng chứng (Evidence Path):** [`results/step3/cbmc/cbmc_004_pointer_safety.log, results/step3/leaks/memory_leak_report.log`](file:///C:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/results/step3/cbmc/cbmc_004_pointer_safety.log)
- **Trạng thái độ phủ (Coverage Status):** `COVERED`
- **Giới hạn kiểm chứng (Limitation):** Kiểm tra trên đối tượng ProductList đơn nhất.
- **Kết luận kỹ thuật:** Hàm product_list_free() có tính lũy đẳng (Idempotent cleanup); ngăn ngừa double-free khi gọi lặp lại.

### REQ-14: Không được dereference con trỏ Product/vùng nhớ ProductList sau free hoặc sau khi realloc làm con trỏ cũ mất hiệu lực.

- **Risk ID liên quan:** `RISK-004`
- **Tập tin nguồn C:** `source/src/product.c` | **Hàm C:** `find_product_by_id, add_product`
- **Bất biến / Thuộc tính an toàn (Property):** `AG(ptr_invalid -> !ptr_deref)`
- **Tiền điều kiện (Preconditions):** Con trỏ mượn được lấy trước khi container thay đổi kích thước
- **Điều kiện vi phạm (Unsafe Condition):** `Dereference con trỏ mượn cũ sau khi realloc đã di chuyển mảng sang địa chỉ mới (Use-After-Free)`
- **Phương pháp kiểm chứng:** Custom CTL Model Checker & C Simulation SIM-001.3
- **Công cụ & Phiên bản:** Custom CTL Model Checker (NuSMV is TOOL_UNAVAILABLE) & GCC (verify_ctl.py, GCC 16.1.0)
- **Harness / Tệp mô hình:** `verification/step3/ctl/inventory_model.smv, tests/simulation/test_sim001_heap.c`
- **Đầu vào thử nghiệm (Test Input):** Mượn con trỏ &items[0], kích hoạt add_product mở rộng capacity 16->32, kiểm tra con trỏ mới
- **Giả định mô hình (Assumptions):** Re-borrowing contract: caller phải lấy lại con trỏ qua API sau khi sửa đổi container
- **Giới hạn kiểm chứng (Bound):** `21 trạng thái mô hình & kịch bản kiểm thử C`
- **Kết quả nghiệm thu:** **`VERIFIED_WITHIN_SCOPE`**
- **Phản ví dụ (Counterexample):** 0 (CTL-03 đạt TRUE, Non-vacuous: ptr_invalid khả đạt tại 2 trạng thái)
- **Đường dẫn bằng chứng (Evidence Path):** [`results/step3/ctl/ctl_verification_results.json, results/step3/simulation/sim001_heap_lifecycle.log`](file:///C:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/results/step3/ctl/ctl_verification_results.json)
- **Trạng thái độ phủ (Coverage Status):** `COVERED`
- **Giới hạn kiểm chứng (Limitation):** Ngôn ngữ C không có borrow checker ở compile-time; an toàn phụ thuộc vào việc tuân thủ quy ước gọi hàm.
- **Kết luận kỹ thuật:** Quy tắc hợp đồng con trỏ mượn được kiểm chứng trên mô hình; cần tài liệu hóa cảnh báo ở Bước 4.

### REQ-15: API ProductList/Inventory phải kiểm tra con trỏ NULL theo hợp đồng hàm trước khi dereference đối tượng hoặc buffer vào/ra.

- **Risk ID liên quan:** `None (Đã được guard đầy đủ trên toàn bộ API)`
- **Tập tin nguồn C:** `source/src/product.c` | **Hàm C:** `product_list_init, add_product, update_product, delete_product, find_product_by_id`
- **Bất biến / Thuộc tính an toàn (Property):** `ptr == NULL -> return STATUS_ERR_NULL_PTR`
- **Tiền điều kiện (Preconditions):** Gọi hàm với đối số con trỏ bất kỳ
- **Điều kiện vi phạm (Unsafe Condition):** `Dereference con trỏ NULL dẫn tới crash chương trình (Segmentation Fault / Access Violation)`
- **Phương pháp kiểm chứng:** GCC Harness Testing (Bounded Contract Assertions)
- **Công cụ & Phiên bản:** GCC Harness Testing (CBMC is TOOL_UNAVAILABLE) (GCC 16.1.0)
- **Harness / Tệp mô hình:** `verification/step3/cbmc/harness_cbmc_004_pointer_safety.c`
- **Đầu vào thử nghiệm (Test Input):** init(NULL), add(NULL, &p), add(&list, NULL), delete(NULL, id), delete(&list, NULL)
- **Giả định mô hình (Assumptions):** Mọi hàm API công khai đều có guard if (ptr == NULL) return STATUS_ERR_NULL_PTR
- **Giới hạn kiểm chứng (Bound):** `k = 17`
- **Kết quả nghiệm thu:** **`VERIFIED_WITHIN_SCOPE`**
- **Phản ví dụ (Counterexample):** 0 (Toàn bộ 5 lời gọi NULL đều bị từ chối với mã -1)
- **Đường dẫn bằng chứng (Evidence Path):** [`results/step3/cbmc/cbmc_004_pointer_safety.log`](file:///C:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/results/step3/cbmc/cbmc_004_pointer_safety.log)
- **Trạng thái độ phủ (Coverage Status):** `COVERED`
- **Giới hạn kiểm chứng (Limitation):** Kiểm tra trên các hàm công khai đã khai báo trong header.
- **Kết luận kỹ thuật:** Các API quản lý sản phẩm trong baseline v0 phòng vệ đối số con trỏ NULL an toàn.

### REQ-16: Mảng tokens, results, summaries và vùng nhớ dùng với memmove phải kiểm soát số phần tử, số byte, giới hạn nguồn/đích; không truy cập ngoài vùng nhớ đã cấp phát.

- **Risk ID liên quan:** `RISK-008, RISK-010`
- **Tập tin nguồn C:** `source/src/file_io.c, source/src/report.c, source/src/product.c` | **Hàm C:** `parse_product_line, calculate_category_summaries, delete_product`
- **Bất biến / Thuộc tính an toàn (Property):** `token_count < 8 && summary_count < 32 && memmove_bytes <= (count - index - 1) * sizeof(Product)`
- **Tiền điều kiện (Preconditions):** Buffer đầu vào có độ dài bất kỳ
- **Điều kiện vi phạm (Unsafe Condition):** `tokens[token_count++] khi token_count >= 8 (Stack Buffer Overflow) hoặc ghi đè summaries[32]`
- **Phương pháp kiểm chứng:** Negative Control SIM-003 & GCC Harness Testing
- **Công cụ & Phiên bản:** GCC Stack Protector & GCC Harness Testing (GCC 16.1.0 -fstack-protector-strong)
- **Harness / Tệp mô hình:** `tests/simulation/test_sim003_negative_control.c, verification/step3/cbmc/harness_cbmc_002_array_bounds.c`
- **Đầu vào thử nghiệm (Test Input):** Dòng 10 trường phân tách '|' đối chứng giữa baseline v0 và nhánh cài lỗi có chủ đích
- **Giả định mô hình (Assumptions):** Baseline v0 có guard token_count < 8; nhánh seeded cố tình bỏ guard
- **Giới hạn kiểm chứng (Bound):** `k = 40`
- **Kết quả nghiệm thu:** **`VERIFIED_WITHIN_SCOPE`**
- **Phản ví dụ (Counterexample):** Baseline v0: 0 vi phạm (từ chối an toàn); Nhánh Seeded: Bắt được Stack Smashing (0xC0000409)
- **Đường dẫn bằng chứng (Evidence Path):** [`results/step3/simulation/sim003_negative_control.log, results/step3/cbmc/cbmc_002_array_bounds.log`](file:///C:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/results/step3/simulation/sim003_negative_control.log)
- **Trạng thái độ phủ (Coverage Status):** `COVERED`
- **Giới hạn kiểm chứng (Limitation):** Phép thử đối chứng chứng minh phát hiện lỗi tràn stack cụ thể; không đại diện cho mọi dạng tràn heap khác.
- **Kết luận kỹ thuật:** Baseline v0 được bảo vệ bởi guard token_count < 8; thí nghiệm đối chứng bắt được stack smashing khi thiếu guard.

### REQ-17: Parser phải chấp nhận đúng định dạng 6 trường hoặc định dạng tương thích 4 trường; các dòng sai số trường/độ dài/định dạng phải bị từ chối mà không gây hỏng bộ nhớ Inventory.

- **Risk ID liên quan:** `RISK-007, RISK-008, RISK-009`
- **Tập tin nguồn C:** `source/src/file_io.c` | **Hàm C:** `parse_product_line`
- **Bất biến / Thuộc tính an toàn (Property):** `(token_count == 6 || token_count == 4) -> parse thành công; ngược lại -> return STATUS_ERR_FILE_IO`
- **Tiền điều kiện (Preconditions):** Dòng dữ liệu văn bản đọc từ tệp
- **Điều kiện vi phạm (Unsafe Condition):** `Chấp nhận dòng sai cấu trúc hoặc parse thiếu trường dẫn tới đọc con trỏ rác`
- **Phương pháp kiểm chứng:** C Simulation Harness SIM-002 (10 Corpus Files)
- **Công cụ & Phiên bản:** GCC C Simulation Harness (GCC 16.1.0)
- **Harness / Tệp mô hình:** `tests/simulation/test_sim002_parser.c`
- **Đầu vào thử nghiệm (Test Input):** Corpus 01 (6 trường), Corpus 02 (4 trường cũ), Corpus 03 (10 trường), Corpus 04 (thiếu trường), Corpus 07 (NUL byte)
- **Giả định mô hình (Assumptions):** Parser gán giá trị mặc định cho category='Chung' và unit='Cai' khi gặp tệp 4 trường cũ
- **Giới hạn kiểm chứng (Bound):** `10 tệp corpus`
- **Kết quả nghiệm thu:** **`VERIFIED_WITHIN_SCOPE`**
- **Phản ví dụ (Counterexample):** 0 (10/10 corpus xử lý an toàn theo đúng hợp đồng đặc tả)
- **Đường dẫn bằng chứng (Evidence Path):** [`results/step3/simulation/sim002_parser_limits.log`](file:///C:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/results/step3/simulation/sim002_parser_limits.log)
- **Trạng thái độ phủ (Coverage Status):** `COVERED`
- **Giới hạn kiểm chứng (Limitation):** Chưa kiểm tra tệp chứa encoding UTF-16 hoặc BOM đặc biệt.
- **Kết luận kỹ thuật:** Parser xử lý định dạng 6 trường và 4 trường tương thích; từ chối an toàn các dòng sai cấu trúc.

### REQ-18: Quantity/price không hợp lệ, vượt miền số hoặc không hữu hạn (NaN/Infinity) phải bị từ chối trước cập nhật kho và trước phép tính báo cáo có liên quan.

- **Risk ID liên quan:** `RISK-009`
- **Tập tin nguồn C:** `source/src/validation.c, source/src/file_io.c` | **Hàm C:** `validate_quantity, validate_price, parse_int, parse_price`
- **Bất biến / Thuộc tính an toàn (Property):** `quantity > 0 && quantity <= INT_MAX && price >= 0.0 && isfinite(price)`
- **Tiền điều kiện (Preconditions):** Chuỗi số hoặc giá trị số truyền vào hàm
- **Điều kiện vi phạm (Unsafe Condition):** `quantity âm, quantity > INT_MAX, price âm, price là NaN/Infinity được chấp nhận vào kho`
- **Phương pháp kiểm chứng:** SIM-002 & Unit Test Suite
- **Công cụ & Phiên bản:** GCC C Simulation & Unit Test Runner (GCC 16.1.0)
- **Harness / Tệp mô hình:** `tests/simulation/test_sim002_parser.c, tests/test_utils.c`
- **Đầu vào thử nghiệm (Test Input):** Corpus 08 (quantity=9999999999 > INT_MAX), Corpus 09 (quantity=-50), Corpus 10 (price=NaN)
- **Giả định mô hình (Assumptions):** Hàm strtol/strtod kiểm tra ERANGE và hàm validation kiểm tra isfinite()
- **Giới hạn kiểm chứng (Bound):** `Tập giá trị biên IEEE-754 và signed int32`
- **Kết quả nghiệm thu:** **`VERIFIED_WITHIN_SCOPE`**
- **Phản ví dụ (Counterexample):** 0 (Toàn bộ dữ liệu số sai phạm vi đều bị từ chối)
- **Đường dẫn bằng chứng (Evidence Path):** [`results/step3/simulation/sim002_parser_limits.log`](file:///C:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/results/step3/simulation/sim002_parser_limits.log)
- **Trạng thái độ phủ (Coverage Status):** `COVERED`
- **Giới hạn kiểm chứng (Limitation):** Chưa kiểm tra định dạng số mũ khoa học dạng 1e+308 trong file text.
- **Kết luận kỹ thuật:** Bộ validation loại bỏ các giá trị số lượng/đơn giá âm, tràn số hoặc không hữu hạn.

