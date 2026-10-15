# Bảng Đăng ký Rủi ro An toàn Bộ nhớ C (Memory Risk Register - Step 2)

## 1. Giới thiệu và Nguyên tắc Phân loại
Tài liệu này tổng hợp toàn bộ các rủi ro an toàn bộ nhớ, trạng thái phòng thủ (Guards) và khả năng kích hoạt (Reachability) trên phiên bản mã nguồn **Baseline v0** (Commit `44202070c68002d9af068374478cdc83098dd4d8`).

> [!IMPORTANT]
> **Quy định trạng thái bằng chứng và khả đạt**:
> - `GUARDED`: Đoạn mã đã có cơ chế phòng vệ (guard clause) ngăn chặn vi phạm.
> - `CANDIDATE`: Ứng viên rủi ro trên mô hình lý thuyết hoặc toán học.
> - `REACHABILITY_UNPROVEN`: Điều kiện toán học có thể thỏa mãn (SAT), nhưng chưa tìm được đường thực thi (path) trong chương trình thực tế đạt tới trạng thái đó.
> - `CONFIRMED`: Trạng thái đã được kiểm chứng bằng thực nghiệm hoặc test suite.
> - `FALSE_POSITIVE`: Cảnh báo ban đầu được bác bỏ sau khi phân tích mã nguồn thực tế.

---

## 2. Bảng Tổng hợp Rủi ro An toàn Bộ nhớ (Risk Register Table)

| Risk ID | Tệp nguồn & Hàm | Đối tượng bộ nhớ | Phân loại rủi ro | CWE | Trạng thái phòng thủ (Existing Guards) | Reachability | Mức độ | REQ liên quan | Phương pháp kiểm chứng đề xuất |
|:---:|---|---|---|:---:|---|:---:|:---:|:---:|---|
| **RISK-001** | `source/src/product.c`<br>`add_product()` (L118) | list->capacity (size_t) | Integer Arithmetic / Heap Growth | `CWE-190` | None explicitly in product.c before multiplication (relies implicitly on system RAM limits) | `REACHABILITY_UNPROVEN` | **MEDIUM** | `REQ-2, CON-7` | SMT Bit-Vector Solver (Z3) / CBMC |
| **RISK-002** | `source/src/product.c`<br>`add_product()` (L119) | Heap allocation buffer size argument (size_t) | Integer Arithmetic / Heap Allocation | `CWE-190, CWE-680` | None explicitly checking new_cap <= SIZE_MAX / sizeof(Product) in product.c | `REACHABILITY_UNPROVEN` | **HIGH** | `REQ-3, CON-8` | SMT Bit-Vector Solver (Z3) / CBMC Bounded Model Checker |
| **RISK-003** | `source/src/product.c`<br>`find_product_by_id / add_product()` (L119, L223-L233) | Borrowed pointer Product* to element inside dynamic array list->items | Pointer Safety / Pointer Invalidation | `CWE-416` | No language-level borrow checker. Current v0 baseline callers in main.c do NOT hold pointers across add_product | `REACHABILITY_UNPROVEN` | **HIGH** | `REQ-14` | CBMC Pointer Invariant Check / ASan Simulation Harness |
| **RISK-004** | `source/src/utils.c`<br>`trim_whitespace()` (L21-L42) | String buffer passed via char *str | Pointer Safety / Memory Mutability | `CWE-787` | All existing callers in main.c and file_io.c pass writable stack buffers | `GUARDED` | **MEDIUM** | `REQ-9, CON-13` | CBMC pointer safety check / Unit Test with read-only probe |
| **RISK-005** | `source/src/report.c`<br>`calculate_category_summaries()` (L72-L74) | Array CategorySummary summaries[32] | Buffer & Array Bounds / Capacity Boundary | `CWE-122` | Explicit bounds guard: if (*out_count >= max_summaries) return STATUS_ERR_OVERFLOW; | `GUARDED` | **LOW** | `REQ-16, CON-12` | Boundary test with >32 categories |
| **RISK-006** | `source/src/file_io.c`<br>`load_products_from_file()` (L111-L114) | Stack buffer char line[512] | Input / Parser Safety | `CWE-120` | fgets bounds size to sizeof(line); newline check detects and rejects truncated line | `GUARDED` | **LOW** | `REQ-8, REQ-10, CON-9` | Fuzzing / Corpus with malformed 1024-byte lines |
| **RISK-007** | `source/src/inventory.c`<br>`import_stock()` (L41-L46) | product->quantity (signed 32-bit int) | Integer Arithmetic Safety | `CWE-190` | check_addition_overflow in validation.c guards current_stock > INT_MAX - add_quantity | `GUARDED` | **HIGH** | `REQ-4, CON-11` | Boundary Unit Test / UBSan / SMT Check |
| **RISK-008** | `source/src/inventory.c`<br>`export_stock()` (L60-L65) | product->quantity (signed 32-bit int) | Integer Arithmetic / Business Logic | `CWE-839` | validate_export_quantity rejects export_quantity > current_stock with STATUS_ERR_INSUFFICIENT_STOCK | `GUARDED` | **MEDIUM** | `REQ-5` | Boundary Unit Test |
| **RISK-009** | `source/src/product.c`<br>`delete_product()` (L211-L213) | Heap buffer list->items | Buffer & Array Bounds / Memory Operation | `CWE-122` | Loop bounds i < list->count, and guard if (i < list->count - 1) ensures positive count | `GUARDED` | **MEDIUM** | `REQ-6, REQ-16` | CBMC Array Bounds Check |
| **RISK-010** | `source/src/product.c`<br>`All Public APIs()` (Entry points of product_list_init, add_product, update_product, delete_product, find_product_by_id) | Pointer arguments (ProductList*, Product*, char*) | Pointer Safety / Defensive Programming | `CWE-476` | Defensive guards at top of all public functions returning STATUS_ERR_NULL_PTR or NULL | `GUARDED` | **HIGH** | `REQ-15` | CBMC NULL Pointer Analysis / ASan NULL Harness |

---

## 3. Phân tích Chi tiết Từng Rủi ro

### RISK-001: Integer Arithmetic / Heap Growth (CWE-190)
- **Vị trí**: `source/src/product.c` - Hàm `add_product()` (L118)
- **Biểu thức nguồn**: `size_t new_cap = (list->capacity == 0) ? INITIAL_CAPACITY : (list->capacity * 2);`
- **Tiền điều kiện (Preconditions)**: list->capacity > SIZE_MAX / 2
- **Điều kiện kích hoạt (Trigger)**: list->capacity reaches 2^63 items on 64-bit ABI and triggers growth
- **Trạng thái mất an toàn (Unsafe State)**: new_cap wraps around to 0 or smaller value modulo 2^64
- **Hậu quả tiềm ẩn**: Heap allocation of 0 bytes, leading to subsequent heap buffer overflow
- **Cơ chế phòng thủ hiện có**: None explicitly in product.c before multiplication (relies implicitly on system RAM limits)
- **Đánh giá khả đạt (Reachability)**: `REACHABILITY_UNPROVEN` | **Độ tin cậy**: `HIGH`
- **Yêu cầu liên quan**: `REQ-2, CON-7`
- **Phương pháp kiểm chứng đề xuất**: SMT Bit-Vector Solver (Z3) / CBMC
- **Ghi chú kỹ thuật**: Arithmetically satisfiable if capacity unbounded; unreachable in practice on 64-bit due to RAM exhaustion before 2^63 items.

### RISK-002: Integer Arithmetic / Heap Allocation (CWE-190, CWE-680)
- **Vị trí**: `source/src/product.c` - Hàm `add_product()` (L119)
- **Biểu thức nguồn**: `Product *new_items = (Product *)realloc(list->items, new_cap * sizeof(Product));`
- **Tiền điều kiện (Preconditions)**: new_cap > SIZE_MAX / sizeof(Product) (new_cap > 88,686,269,585,142,075 on 64-bit)
- **Điều kiện kích hoạt (Trigger)**: new_cap exceeds 8.86e16 items, causing multiplication overflow
- **Trạng thái mất an toàn (Unsafe State)**: Multiplication wraps around, realloc allocates smaller buffer than required
- **Hậu quả tiềm ẩn**: Heap buffer overflow when items[list->count] is written (CWE-122)
- **Cơ chế phòng thủ hiện có**: None explicitly checking new_cap <= SIZE_MAX / sizeof(Product) in product.c
- **Đánh giá khả đạt (Reachability)**: `REACHABILITY_UNPROVEN` | **Độ tin cậy**: `HIGH`
- **Yêu cầu liên quan**: `REQ-3, CON-8`
- **Phương pháp kiểm chứng đề xuất**: SMT Bit-Vector Solver (Z3) / CBMC Bounded Model Checker
- **Ghi chú kỹ thuật**: Critical formal verification target. On 32-bit systems, SIZE_MAX/208 = 20,647,663 items which is reachable.

### RISK-003: Pointer Safety / Pointer Invalidation (CWE-416)
- **Vị trí**: `source/src/product.c` - Hàm `find_product_by_id / add_product()` (L119, L223-L233)
- **Biểu thức nguồn**: `Product* find_product_by_id(...) -> return &list->items[i]; vs realloc(list->items, ...)`
- **Tiền điều kiện (Preconditions)**: Caller holds borrowed pointer returned by find_product*() and subsequently calls add_product()
- **Điều kiện kích hoạt (Trigger)**: add_product triggers realloc() which moves array to new heap address
- **Trạng thái mất an toàn (Unsafe State)**: Held pointer points to freed old heap memory (Dangling Pointer)
- **Hậu quả tiềm ẩn**: Use-after-free read or write when dereferencing old pointer
- **Cơ chế phòng thủ hiện có**: No language-level borrow checker. Current v0 baseline callers in main.c do NOT hold pointers across add_product
- **Đánh giá khả đạt (Reachability)**: `REACHABILITY_UNPROVEN` | **Độ tin cậy**: `HIGH`
- **Yêu cầu liên quan**: `REQ-14`
- **Phương pháp kiểm chứng đề xuất**: CBMC Pointer Invariant Check / ASan Simulation Harness
- **Ghi chú kỹ thuật**: Architectural contract hazard. Safe in current main.c execution, but unsafe API contract for composition.

### RISK-004: Pointer Safety / Memory Mutability (CWE-787)
- **Vị trí**: `source/src/utils.c` - Hàm `trim_whitespace()` (L21-L42)
- **Biểu thức nguồn**: `*end = '\0'; if (start != str) memmove(str, start, (size_t)(end - start + 1));`
- **Tiền điều kiện (Preconditions)**: Caller passes pointer to non-writable memory (string literal in .rdata)
- **Điều kiện kích hoạt (Trigger)**: Function attempts to write '\0' into read-only page
- **Trạng thái mất an toàn (Unsafe State)**: Segmentation fault (Access Violation on write to read-only memory)
- **Hậu quả tiềm ẩn**: Application crash / Denial of service
- **Cơ chế phòng thủ hiện có**: All existing callers in main.c and file_io.c pass writable stack buffers
- **Đánh giá khả đạt (Reachability)**: `GUARDED` | **Độ tin cậy**: `MEDIUM`
- **Yêu cầu liên quan**: `REQ-9, CON-13`
- **Phương pháp kiểm chứng đề xuất**: CBMC pointer safety check / Unit Test with read-only probe
- **Ghi chú kỹ thuật**: Function contract requires writable C string; guarded in all internal call sites.

### RISK-005: Buffer & Array Bounds / Capacity Boundary (CWE-122)
- **Vị trí**: `source/src/report.c` - Hàm `calculate_category_summaries()` (L72-L74)
- **Biểu thức nguồn**: `if (*out_count >= max_summaries) { return STATUS_ERR_OVERFLOW; }`
- **Tiền điều kiện (Preconditions)**: Inventory contains products spanning more than max_summaries (32) distinct categories
- **Điều kiện kích hoạt (Trigger)**: Discovery of 33rd distinct category
- **Trạng thái mất an toàn (Unsafe State)**: Function aborts with STATUS_ERR_OVERFLOW; no out-of-bounds write occurs
- **Hậu quả tiềm ẩn**: Functional report omission; no memory safety violation
- **Cơ chế phòng thủ hiện có**: Explicit bounds guard: if (*out_count >= max_summaries) return STATUS_ERR_OVERFLOW;
- **Đánh giá khả đạt (Reachability)**: `GUARDED` | **Độ tin cậy**: `HIGH`
- **Yêu cầu liên quan**: `REQ-16, CON-12`
- **Phương pháp kiểm chứng đề xuất**: Boundary test with >32 categories
- **Ghi chú kỹ thuật**: CWE-122 buffer overflow is REFUTED (FALSE_POSITIVE) because code guards array bounds before write. It is a capacity limit.

### RISK-006: Input / Parser Safety (CWE-120)
- **Vị trí**: `source/src/file_io.c` - Hàm `load_products_from_file()` (L111-L114)
- **Biểu thức nguồn**: `if (strchr(line, '\n') == NULL && !feof(file)) { status = STATUS_ERR_FILE_IO; break; }`
- **Tiền điều kiện (Preconditions)**: Input file contains record line longer than 511 characters
- **Điều kiện kích hoạt (Trigger)**: fgets fills 512 bytes without encountering newline
- **Trạng thái mất an toàn (Unsafe State)**: Line truncation detected; parsing aborts with STATUS_ERR_FILE_IO
- **Hậu quả tiềm ẩn**: Subsequent records in file ignored; no buffer overflow
- **Cơ chế phòng thủ hiện có**: fgets bounds size to sizeof(line); newline check detects and rejects truncated line
- **Đánh giá khả đạt (Reachability)**: `GUARDED` | **Độ tin cậy**: `HIGH`
- **Yêu cầu liên quan**: `REQ-8, REQ-10, CON-9`
- **Phương pháp kiểm chứng đề xuất**: Fuzzing / Corpus with malformed 1024-byte lines
- **Ghi chú kỹ thuật**: CWE-120 buffer overflow is REFUTED (FALSE_POSITIVE). Line truncation is safely trapped and rejected.

### RISK-007: Integer Arithmetic Safety (CWE-190)
- **Vị trí**: `source/src/inventory.c` - Hàm `import_stock()` (L41-L46)
- **Biểu thức nguồn**: `status = check_addition_overflow(product->quantity, quantity); product->quantity += quantity;`
- **Tiền điều kiện (Preconditions)**: current_stock + quantity > INT_MAX
- **Điều kiện kích hoạt (Trigger)**: Import quantity causing sum to exceed 2,147,483,647
- **Trạng thái mất an toàn (Unsafe State)**: Signed integer overflow would wrap to negative quantity (Undefined Behavior)
- **Hậu quả tiềm ẩn**: Negative inventory count, inventory inconsistency
- **Cơ chế phòng thủ hiện có**: check_addition_overflow in validation.c guards current_stock > INT_MAX - add_quantity
- **Đánh giá khả đạt (Reachability)**: `GUARDED` | **Độ tin cậy**: `HIGH`
- **Yêu cầu liên quan**: `REQ-4, CON-11`
- **Phương pháp kiểm chứng đề xuất**: Boundary Unit Test / UBSan / SMT Check
- **Ghi chú kỹ thuật**: Confirmed defended by guard in baseline v0. Guarded against overflow.

### RISK-008: Integer Arithmetic / Business Logic (CWE-839)
- **Vị trí**: `source/src/inventory.c` - Hàm `export_stock()` (L60-L65)
- **Biểu thức nguồn**: `status = validate_export_quantity(product->quantity, quantity); product->quantity -= quantity;`
- **Tiền điều kiện (Preconditions)**: export_quantity > current_stock or export_quantity <= 0
- **Điều kiện kích hoạt (Trigger)**: Exporting more items than currently in stock
- **Trạng thái mất an toàn (Unsafe State)**: Inventory quantity becomes negative
- **Hậu quả tiềm ẩn**: Negative inventory corruption
- **Cơ chế phòng thủ hiện có**: validate_export_quantity rejects export_quantity > current_stock with STATUS_ERR_INSUFFICIENT_STOCK
- **Đánh giá khả đạt (Reachability)**: `GUARDED` | **Độ tin cậy**: `HIGH`
- **Yêu cầu liên quan**: `REQ-5`
- **Phương pháp kiểm chứng đề xuất**: Boundary Unit Test
- **Ghi chú kỹ thuật**: Confirmed defended by validation guard in baseline v0.

### RISK-009: Buffer & Array Bounds / Memory Operation (CWE-122)
- **Vị trí**: `source/src/product.c` - Hàm `delete_product()` (L211-L213)
- **Biểu thức nguồn**: `if (i < list->count - 1) { memmove(&list->items[i], &list->items[i + 1], (list->count - i - 1) * sizeof(Product)); }`
- **Tiền điều kiện (Preconditions)**: Index i >= list->count
- **Điều kiện kích hoạt (Trigger)**: list->count - i - 1 wraps around to huge size_t value
- **Trạng thái mất an toàn (Unsafe State)**: memmove writes past end of allocated buffer
- **Hậu quả tiềm ẩn**: Heap corruption and crash
- **Cơ chế phòng thủ hiện có**: Loop bounds i < list->count, and guard if (i < list->count - 1) ensures positive count
- **Đánh giá khả đạt (Reachability)**: `GUARDED` | **Độ tin cậy**: `HIGH`
- **Yêu cầu liên quan**: `REQ-6, REQ-16`
- **Phương pháp kiểm chứng đề xuất**: CBMC Array Bounds Check
- **Ghi chú kỹ thuật**: Properly guarded by index checks in delete_product.

### RISK-010: Pointer Safety / Defensive Programming (CWE-476)
- **Vị trí**: `source/src/product.c` - Hàm `All Public APIs()` (Entry points of product_list_init, add_product, update_product, delete_product, find_product_by_id)
- **Biểu thức nguồn**: `if (list == NULL) return STATUS_ERR_NULL_PTR;`
- **Tiền điều kiện (Preconditions)**: Caller invokes public API with NULL pointer
- **Điều kiện kích hoạt (Trigger)**: Dereferencing NULL pointer
- **Trạng thái mất an toàn (Unsafe State)**: Hardware segmentation fault / Crash
- **Hậu quả tiềm ẩn**: Application crash, Denial of Service
- **Cơ chế phòng thủ hiện có**: Defensive guards at top of all public functions returning STATUS_ERR_NULL_PTR or NULL
- **Đánh giá khả đạt (Reachability)**: `GUARDED` | **Độ tin cậy**: `HIGH`
- **Yêu cầu liên quan**: `REQ-15`
- **Phương pháp kiểm chứng đề xuất**: CBMC NULL Pointer Analysis / ASan NULL Harness
- **Ghi chú kỹ thuật**: Guarded by explicit null pointer checks in all public APIs.

