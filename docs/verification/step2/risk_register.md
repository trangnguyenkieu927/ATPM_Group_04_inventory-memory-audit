# DANH MỤC RỦI RO AN TOÀN BỘ NHỚ C (MEMORY SAFETY RISK REGISTER - STEP 2)

**Dự án:** Kiểm chứng an toàn bộ nhớ cho chương trình C mô phỏng quản lý kho
**Nhánh Git:** `audit/step2-risk-specification`
**Commit Baseline:** `44202070c68002d9af068374478cdc83098dd4d8`
**Quy mô rà soát:** 7 modules, 70 hàm C, 2.229 Code LOC

---

## 1. NGUYÊN TẮC PHÂN LOẠI VÀ ĐÁNH GIÁ RỦI RO

Tuân thủ nghiêm ngặt chỉ đạo kiểm toán và nguyên tắc trung thực khoa học:
- **Không đánh đồng nguy cơ lý thuyết với lỗ hổng đã xác nhận (Confirmed Defect):** Một biểu thức toán học có khả năng tràn bit trên lý thuyết (SMT SAT) chỉ trở thành lỗ hổng nếu tồn tại đường thực thi đạt tới trạng thái đó trong chương trình thực tế.
- **Phân loại chính xác cơ chế phòng vệ (GUARDED):** Khi một hàm đã có câu lệnh kiểm tra biên và từ chối an toàn (ví dụ: `if (*out_count >= max_summaries) return STATUS_ERR_OVERFLOW;`), đây là cơ chế phòng vệ có chủ đích (**GUARDED**), không được gán nhầm thành lỗi tràn bộ nhớ hoặc False Positive của công cụ khi công cụ không hề báo lỗi.
- **Chuẩn hóa số dòng theo commit baseline:** 100% số dòng mã được đối chiếu trực tiếp trên cây mã nguồn của commit baseline `4420207`.

---

## 2. BẢNG TỔNG HỢP 10 RỦI RO BỘ NHỚ ĐỘC LẬP

| Risk ID | Tập tin & Dòng | Hàm C | Biểu thức / Nguy cơ | CWE | Cơ chế Guard Hiện tại | Trạng thái Khả đạt | REQ Liên quan | Phương pháp Kiểm chứng |
|---|---|---|---|---|---|---|---|---|
| **RISK-001** | `source/src/product.c:118` | `add_product` | Integer Arithmetic Safety | CWE-190 | Không có guard tường minh; trong v0 list khởi... | **REACHABILITY_UNPROVEN** | `REQ-2` | SMT Bit-Vector (Z3) / CBMC Bounded Check |
| **RISK-002** | `source/src/product.c:119` | `add_product` | Heap Allocation Size Arithmetic | CWE-190, CWE-680 | Không có guard if (new_cap > SIZE_MAX / sizeo... | **REACHABILITY_UNPROVEN** | `REQ-3` | SMT Bit-Vector (Z3) / CBMC / Harness SIM-001 |
| **RISK-003** | `source/src/product.c:119-124` | `add_product` | Heap Allocation Failure & Ownership | CWE-401 | Mã nguồn sử dụng con trỏ tạm new_items; nếu N... | **GUARDED** | `REQ-12, REQ-11` | CBMC Fault Injection / OOM Simulation |
| **RISK-004** | `source/src/product.c:223-233` | `find_product_by_id` | Pointer Lifetime & Invalidation | CWE-416 | Hợp đồng kiến trúc: các caller trong main.c v... | **REACHABILITY_UNPROVEN** | `REQ-14` | Harness SIM-001 / Dynamic Contract Checking |
| **RISK-005** | `source/src/product.c:27-34` | `product_list_free` | Heap Cleanup & Lifecycle Safety | CWE-415 | Kiểm tra if (list != NULL && list->items != N... | **GUARDED** | `REQ-13, REQ-11` | CTL Formula AG(!(list_live & list_freed)) / ASan |
| **RISK-006** | `source/src/utils.c:44-66` | `safe_string_copy` | Buffer Bounds & String Safety | CWE-120 | Kiểm tra dest_size > 0, vòng lặp dừng ở i + 1... | **GUARDED** | `REQ-9` | CBMC String Bounds Check / Unit Test |
| **RISK-007** | `source/src/file_io.c:96, 107-114` | `load_products_from_file` | Input Stream & Buffer Handling | CWE-120 | fgets giới hạn đúng 512 bytes; guard strchr(l... | **GUARDED** | `REQ-10, REQ-17` | Fuzzing Harness SIM-002 (Oversized Lines) |
| **RISK-008** | `source/src/file_io.c:47, 52-61` | `parse_product_line` | Stack Array Bounds Safety | CWE-121 | Điều kiện lặp cursor != NULL && token_count <... | **GUARDED** | `REQ-16, REQ-17` | Fuzzing Harness SIM-002 (Excess Delimiters) |
| **RISK-009** | `source/src/file_io.c:63-91` | `parse_product_line` | Input Validation & Backward Compatibility | CWE-20 | Rẽ nhánh rõ ràng: token_count == 6 gán đủ trư... | **GUARDED** | `REQ-17, REQ-18` | Test Corpus Regression & CBMC |
| **RISK-010** | `source/src/report.c:72-74` | `calculate_category_summaries` | Array Bounds & Functional Limitation | CWE-122 (Prevented) / CWE-400 | Mã nguồn kiểm tra rõ ràng if (*out_count >= m... | **GUARDED** | `REQ-16` | Model Checking Invariant / Boundary Test |

---

## 3. CHI TIẾT TỪNG RỦI RO AN TOÀN BỘ NHỚ

### RISK-001: Integer Arithmetic Safety tại `add_product` (`source/src/product.c:118`)
- **Biểu thức nguồn:** `size_t new_cap = (list->capacity == 0) ? INITIAL_CAPACITY : (list->capacity * 2);`
- **Đối tượng bộ nhớ:** list->capacity (biến thành viên quản lý sức chứa mảng heap)
- **CWE ứng viên:** `CWE-190`
- **Tiền điều kiện:** list != NULL, count == capacity, capacity >= 2^63 (trên kiến trúc 64-bit unsigned)
- **Điều kiện kích hoạt:** Số lượng phần tử đạt tới ngưỡng cận biên bit cao nhất của size_t (0x8000000000000000)
- **Trạng thái mất an toàn:** Phép nhân capacity * 2 bị tràn bọc vòng về 0 hoặc giá trị nhỏ hơn capacity
- **Hậu quả tiềm tàng:** Cấp phát kích thước sai lệch, có thể dẫn tới tràn bộ đệm heap nếu tiếp tục ghi
- **Cơ chế bảo vệ hiện tại (Existing Guards):** Không có guard tường minh; trong v0 list khởi tạo bằng 16, RAM vật lý cạn kiệt trước khi đạt 2^63
- **Đánh giá tính khả đạt (Reachability Status):** **REACHABILITY_UNPROVEN**
- **Mức độ nghiêm trọng:** `LOW` | **Độ tin cậy:** `HIGH (Toán học) / LOW (Khả đạt thực tế)`
- **Ánh xạ yêu cầu:** `REQ-2`
- **Phương pháp kiểm chứng Bước 3:** SMT Bit-Vector (Z3) / CBMC Bounded Check
- **Ghi chú rà soát:** Đã sửa số dòng từ 42 thành 118; ánh xạ chính xác duy nhất tới REQ-2 theo văn bản gốc.

### RISK-002: Heap Allocation Size Arithmetic tại `add_product` (`source/src/product.c:119`)
- **Biểu thức nguồn:** `Product *new_items = (Product *)realloc(list->items, new_cap * sizeof(Product));`
- **Đối tượng bộ nhớ:** new_cap * sizeof(Product) (tổng số byte cấp phát lại mảng động)
- **CWE ứng viên:** `CWE-190, CWE-680`
- **Tiền điều kiện:** new_cap > SIZE_MAX / sizeof(Product) (với sizeof(Product) = 208 bytes, ngưỡng là 88.686.269.585.142.075)
- **Điều kiện kích hoạt:** Dung lượng yêu cầu new_cap vượt quá ngưỡng phân chia SIZE_MAX / 208
- **Trạng thái mất an toàn:** Phép nhân new_cap * 208 bị tràn số byte truyền vào realloc thành giá trị nhỏ
- **Hậu quả tiềm tàng:** Heap Buffer Overflow (CWE-122) nếu realloc cấp phát thành công vùng nhớ nhỏ hơn số lượng phần tử
- **Cơ chế bảo vệ hiện tại (Existing Guards):** Không có guard if (new_cap > SIZE_MAX / sizeof(Product)); realloc sẽ thất bại do OOM trước
- **Đánh giá tính khả đạt (Reachability Status):** **REACHABILITY_UNPROVEN**
- **Mức độ nghiêm trọng:** `MEDIUM` | **Độ tin cậy:** `HIGH (Toán học) / LOW (Khả đạt runtime)`
- **Ánh xạ yêu cầu:** `REQ-3`
- **Phương pháp kiểm chứng Bước 3:** SMT Bit-Vector (Z3) / CBMC / Harness SIM-001
- **Ghi chú rà soát:** Đã sửa số dòng từ 44 thành 119; ánh xạ chuẩn xác tới REQ-3.

### RISK-003: Heap Allocation Failure & Ownership tại `add_product` (`source/src/product.c:119-124`)
- **Biểu thức nguồn:** `Product *new_items = (Product *)realloc(...); if (new_items == NULL) { return STATUS_ERR_MEMORY; }`
- **Đối tượng bộ nhớ:** list->items (vùng nhớ heap mảng sản phẩm gốc)
- **CWE ứng viên:** `CWE-401`
- **Tiền điều kiện:** realloc trả về NULL do cạn kiệt bộ nhớ hệ thống (OOM)
- **Điều kiện kích hoạt:** Môi trường cạn bộ nhớ; nếu gán trực tiếp list->items = realloc(...) thì con trỏ cũ bị mất
- **Trạng thái mất an toàn:** Rò rỉ vùng nhớ cũ nếu con trỏ sở hữu bị ghi đè bởi NULL
- **Hậu quả tiềm tàng:** Rò rỉ bộ nhớ nghiêm trọng (Memory Leak)
- **Cơ chế bảo vệ hiện tại (Existing Guards):** Mã nguồn sử dụng con trỏ tạm new_items; nếu NULL thì return ngay, bảo toàn list->items
- **Đánh giá tính khả đạt (Reachability Status):** **GUARDED**
- **Mức độ nghiêm trọng:** `HIGH (Nếu không có guard) / LOW (Thực tế đã được bảo vệ)` | **Độ tin cậy:** `CERTAIN`
- **Ánh xạ yêu cầu:** `REQ-12, REQ-11`
- **Phương pháp kiểm chứng Bước 3:** CBMC Fault Injection / OOM Simulation
- **Ghi chú rà soát:** Đã sửa ánh xạ từ REQ-4 sang REQ-12 và REQ-11 (Bảo toàn con trỏ và quyền sở hữu khi realloc thất bại).

### RISK-004: Pointer Lifetime & Invalidation tại `find_product_by_id` (`source/src/product.c:223-233`)
- **Biểu thức nguồn:** `return &list->items[i];`
- **Đối tượng bộ nhớ:** Product* (con trỏ mượn trỏ trực tiếp vào phần tử trong mảng heap)
- **CWE ứng viên:** `CWE-416`
- **Tiền điều kiện:** Hàm gọi giữ con trỏ mượn `Product *p = find_product_by_id(...)`, sau đó gọi add_product làm realloc dời khối nhớ
- **Điều kiện kích hoạt:** realloc di chuyển mảng sang khối nhớ mới và giải phóng khối nhớ cũ, sau đó hàm gọi dereference con trỏ p
- **Trạng thái mất an toàn:** Dereference con trỏ lơ lửng đã bị giải phóng (Use-After-Free)
- **Hậu quả tiềm tàng:** Đọc sai dữ liệu, crash SIGSEGV, hoặc ghi đè bộ nhớ bất hợp pháp
- **Cơ chế bảo vệ hiện tại (Existing Guards):** Hợp đồng kiến trúc: các caller trong main.c và inventory.c không giữ con trỏ qua add_product
- **Đánh giá tính khả đạt (Reachability Status):** **REACHABILITY_UNPROVEN**
- **Mức độ nghiêm trọng:** `MEDIUM` | **Độ tin cậy:** `MEDIUM`
- **Ánh xạ yêu cầu:** `REQ-14`
- **Phương pháp kiểm chứng Bước 3:** Harness SIM-001 / Dynamic Contract Checking
- **Ghi chú rà soát:** Đã sửa số dòng thành 223-233; ánh xạ chính xác duy nhất tới REQ-14 (Không dereference con trỏ sau realloc).

### RISK-005: Heap Cleanup & Lifecycle Safety tại `product_list_free` (`source/src/product.c:27-34`)
- **Biểu thức nguồn:** `free(list->items); list->items = NULL; list->count = 0; list->capacity = 0;`
- **Đối tượng bộ nhớ:** list->items (khối nhớ heap)
- **CWE ứng viên:** `CWE-415`
- **Tiền điều kiện:** Gọi product_list_free nhiều lần trên cùng một đối tượng danh sách
- **Điều kiện kích hoạt:** Gọi free trên con trỏ đã giải phóng
- **Trạng thái mất an toàn:** Giải phóng kép vùng nhớ heap (Double Free)
- **Hậu quả tiềm tàng:** Crash tiến trình hoặc phá hỏng cấu trúc heap allocator
- **Cơ chế bảo vệ hiện tại (Existing Guards):** Kiểm tra if (list != NULL && list->items != NULL) và gán ngay list->items = NULL
- **Đánh giá tính khả đạt (Reachability Status):** **GUARDED**
- **Mức độ nghiêm trọng:** `HIGH (Nếu không có guard) / LOW (Đã được phòng vệ)` | **Độ tin cậy:** `CERTAIN`
- **Ánh xạ yêu cầu:** `REQ-13, REQ-11`
- **Phương pháp kiểm chứng Bước 3:** CTL Formula AG(!(list_live & list_freed)) / ASan
- **Ghi chú rà soát:** Đã sửa số dòng thành 27-34; ánh xạ chuẩn xác tới REQ-13 (Single Free) và REQ-11.

### RISK-006: Buffer Bounds & String Safety tại `safe_string_copy` (`source/src/utils.c:44-66`)
- **Biểu thức nguồn:** `while (i + 1 < dest_size && src[i] != '\0') { dest[i] = src[i]; i++; } dest[i] = '\0';`
- **Đối tượng bộ nhớ:** dest (bộ đệm chuỗi ký tự đích)
- **CWE ứng viên:** `CWE-120`
- **Tiền điều kiện:** Chuỗi nguồn src có độ dài lớn hơn hoặc bằng dest_size
- **Điều kiện kích hoạt:** Sao chép chuỗi không kiểm tra độ dài hoặc không đóng NUL
- **Trạng thái mất an toàn:** Ghi đè ngoài biên bộ đệm đích hoặc chuỗi không kết thúc bằng '\0'
- **Hậu quả tiềm tàng:** Tràn bộ đệm, đọc vượt biên khi gọi strlen/strcmp tiếp theo
- **Cơ chế bảo vệ hiện tại (Existing Guards):** Kiểm tra dest_size > 0, vòng lặp dừng ở i + 1 < dest_size, luôn gán dest[i] = '\0', trả STATUS_ERR_OVERFLOW
- **Đánh giá tính khả đạt (Reachability Status):** **GUARDED**
- **Mức độ nghiêm trọng:** `MEDIUM (Nếu không có guard) / LOW (Đã phòng vệ an toàn)` | **Độ tin cậy:** `CERTAIN`
- **Ánh xạ yêu cầu:** `REQ-9`
- **Phương pháp kiểm chứng Bước 3:** CBMC String Bounds Check / Unit Test
- **Ghi chú rà soát:** Đã sửa số dòng thành 44-66; sửa ánh xạ từ REQ-5, REQ-6 sang REQ-9 (Đặc tả chuẩn cho sao chép chuỗi và NUL).

### RISK-007: Input Stream & Buffer Handling tại `load_products_from_file` (`source/src/file_io.c:96, 107-114`)
- **Biểu thức nguồn:** `while (fgets(line, sizeof(line), file) != NULL) { if (strchr(line, '\n') == NULL && !feof(file)) { ... } }`
- **Đối tượng bộ nhớ:** char line[LINE_BUFFER_SIZE] (bộ đệm đọc dòng tệp 512 bytes trên stack)
- **CWE ứng viên:** `CWE-120`
- **Tiền điều kiện:** Tệp đầu vào chứa dòng dữ liệu dài hơn 511 bytes không có ký tự newline '\n'
- **Điều kiện kích hoạt:** Đọc tiếp nửa sau của dòng bị cắt cụt mà coi đó là một bản ghi mới hợp lệ
- **Trạng thái mất an toàn:** Dữ liệu bị cắt cụt, tràn bộ nhớ phân tích cú pháp hoặc xử lý sai lệch
- **Hậu quả tiềm tàng:** Hỏng tính toàn vẹn dữ liệu kho
- **Cơ chế bảo vệ hiện tại (Existing Guards):** fgets giới hạn đúng 512 bytes; guard strchr(line, '\n') == NULL trả về STATUS_ERR_FILE_IO và break
- **Đánh giá tính khả đạt (Reachability Status):** **GUARDED**
- **Mức độ nghiêm trọng:** `MEDIUM (Đã phòng vệ có kiểm soát)` | **Độ tin cậy:** `CERTAIN`
- **Ánh xạ yêu cầu:** `REQ-10, REQ-17`
- **Phương pháp kiểm chứng Bước 3:** Fuzzing Harness SIM-002 (Oversized Lines)
- **Ghi chú rà soát:** Đã sửa số dòng thành 96, 107-114; sửa ánh xạ sang REQ-10 (Xử lý input quá dài) và REQ-17.

### RISK-008: Stack Array Bounds Safety tại `parse_product_line` (`source/src/file_io.c:47, 52-61`)
- **Biểu thức nguồn:** `char *tokens[8]; while (cursor != NULL && token_count < 8) { tokens[token_count++] = cursor; ... }`
- **Đối tượng bộ nhớ:** tokens[8] (mảng con trỏ token cố định trên stack)
- **CWE ứng viên:** `CWE-121`
- **Tiền điều kiện:** Dòng đầu vào chứa nhiều hơn 8 trường phân cách bởi dấu gạch đứng '|'
- **Điều kiện kích hoạt:** Vòng lặp tách token tiếp tục ghi vào tokens[token_count++] khi token_count >= 8
- **Trạng thái mất an toàn:** Ghi đè ngoài biên mảng tokens trên stack (Stack Buffer Overflow)
- **Hậu quả tiềm tàng:** Ghi đè biến cục bộ, hỏng con trỏ khung ngăn xếp
- **Cơ chế bảo vệ hiện tại (Existing Guards):** Điều kiện lặp cursor != NULL && token_count < 8 chặn cứng việc ghi vượt quá 8 con trỏ
- **Đánh giá tính khả đạt (Reachability Status):** **GUARDED**
- **Mức độ nghiêm trọng:** `HIGH (Nếu không có guard) / LOW (Đã có guard chặn chặt)` | **Độ tin cậy:** `CERTAIN`
- **Ánh xạ yêu cầu:** `REQ-16, REQ-17`
- **Phương pháp kiểm chứng Bước 3:** Fuzzing Harness SIM-002 (Excess Delimiters)
- **Ghi chú rà soát:** Đã sửa số dòng thành 47, 52-61; đính chính parser dùng strchr('|') duyệt cursor; sửa ánh xạ sang REQ-16 và REQ-17.

### RISK-009: Input Validation & Backward Compatibility tại `parse_product_line` (`source/src/file_io.c:63-91`)
- **Biểu thức nguồn:** `if (token_count == 6) { ... } else if (token_count == 4) { ... } else { return STATUS_ERR_FILE_IO; }`
- **Đối tượng bộ nhớ:** Dữ liệu cấu trúc Product sau khi parse
- **CWE ứng viên:** `CWE-20`
- **Tiền điều kiện:** Tệp dữ liệu đầu vào chứa định dạng 4 trường (legacy) hoặc số trường khác 4 và 6
- **Điều kiện kích hoạt:** Đọc tệp định dạng cũ hoặc dữ liệu hỏng trường
- **Trạng thái mất an toàn:** Đọc sai trường dữ liệu, ép kiểu số học không hợp lệ
- **Hậu quả tiềm tàng:** Dữ liệu kho không toàn vẹn
- **Cơ chế bảo vệ hiện tại (Existing Guards):** Rẽ nhánh rõ ràng: token_count == 6 gán đủ trường; token_count == 4 gán giá trị mặc định; khác trả lỗi
- **Đánh giá tính khả đạt (Reachability Status):** **GUARDED**
- **Mức độ nghiêm trọng:** `LOW` | **Độ tin cậy:** `CERTAIN`
- **Ánh xạ yêu cầu:** `REQ-17, REQ-18`
- **Phương pháp kiểm chứng Bước 3:** Test Corpus Regression & CBMC
- **Ghi chú rà soát:** Đã sửa số dòng thành 63-91; ánh xạ chuẩn xác tới REQ-17 (Parser 4/6 trường) và REQ-18 (Kiểm tra số liệu).

### RISK-010: Array Bounds & Functional Limitation tại `calculate_category_summaries` (`source/src/report.c:72-74`)
- **Biểu thức nguồn:** `if (*out_count >= max_summaries) { return STATUS_ERR_OVERFLOW; }`
- **Đối tượng bộ nhớ:** summaries (mảng lưu trữ thống kê danh mục truyền từ caller)
- **CWE ứng viên:** `CWE-122 (Prevented) / CWE-400`
- **Tiền điều kiện:** Số lượng danh mục sản phẩm duy nhất trong kho vượt quá max_summaries (32 danh mục)
- **Điều kiện kích hoạt:** Thêm danh mục mới khi mảng summaries đã đầy
- **Trạng thái mất an toàn:** Ghi đè phần tử thứ 33 vào mảng chỉ có sức chứa 32
- **Hậu quả tiềm tàng:** Heap/Stack Buffer Overflow nếu không kiểm tra giới hạn
- **Cơ chế bảo vệ hiện tại (Existing Guards):** Mã nguồn kiểm tra rõ ràng if (*out_count >= max_summaries) return STATUS_ERR_OVERFLOW; trước khi ghi
- **Đánh giá tính khả đạt (Reachability Status):** **GUARDED**
- **Mức độ nghiêm trọng:** `LOW (Cơ chế phòng vệ và giới hạn nghiệp vụ, không phải lỗ hổng)` | **Độ tin cậy:** `CERTAIN`
- **Ánh xạ yêu cầu:** `REQ-16`
- **Phương pháp kiểm chứng Bước 3:** Model Checking Invariant / Boundary Test
- **Ghi chú rà soát:** Đã sửa số dòng thành 72-74; phân loại lại thành GUARDED (Defensive Behavior); sửa ánh xạ thành REQ-16 (Giới hạn mảng).

