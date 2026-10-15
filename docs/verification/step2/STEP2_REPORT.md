# BÁO CÁO TỔNG KẾT BƯỚC 2: PHÂN TÍCH VÀ ĐẶC TẢ RỦI RO AN TOÀN BỘ NHỚ C

**Đề tài môn học:** An toàn phần mềm  
**Dự án:** Kiểm chứng an toàn bộ nhớ cho chương trình C mô phỏng nghiệp vụ quản lý kho  
**Repository:** [ATPM_Group_04_inventory-memory-audit](https://github.com/trangnguyenkieu927/ATPM_Group_04_inventory-memory-audit)  
**Nhóm thực hiện (Nhóm 04):**  
- **Nguyễn Kiều Trang** – Nhóm trưởng, phụ trách tích hợp hệ thống, kiến trúc và QA  
- **Hà Văn Đô** – Phụ trách đặc tả, mô hình bộ nhớ, thuộc tính an toàn và kiểm chứng hình thức  
- **Nguyễn Quang Thọ** – Phụ trách công cụ, môi trường thực nghiệm, build/test và bằng chứng kiểm chứng  
**Thời điểm thực hiện:** Học kỳ I / 2026-10-15  
**Nhánh Git làm việc:** `audit/step2-risk-specification`  
**Commit Baseline (Đã xác minh):** `44202070c68002d9af068374478cdc83098dd4d8`  

---

## 1. GIỚI THIỆU VÀ ĐỊNH VỊ HỌC THUẬT

### 1.1. Nhận xét Trọng tâm của Giảng viên Hướng dẫn
Báo cáo này được xây dựng bám sát chỉ đạo trực tiếp của Giảng viên hướng dẫn:
> *"Xuất phát từ source code → compile time → error + warning → tổng hợp + đặc tả để hình thức hóa (tính phủ các trường hợp có thể xảy ra) → xây dựng code C mô phỏng lại rủi ro. Xây dựng đặc tả cho bài toán rủi ro an toàn."*

### 1.2. Mục tiêu của Bước 2
Bước 2 là bước chuyển tiếp cốt lõi từ mã nguồn baseline đã chuẩn hóa ở Bước 1 sang giai đoạn kiểm chứng hình thức và thực nghiệm ở Bước 3. Mục tiêu 100% của Bước 2 bao gồm:
1. **Rà soát toàn diện mã nguồn C v0:** Thống kê chi tiết từng module, hàm, đối tượng bộ nhớ và luồng con trỏ.
2. **Thu thập và phân loại kết quả Compile Time:** Khám phá cảnh báo với cờ biên dịch tăng cường của GCC.
3. **Thực thi phân tích tĩnh (Static Code Analysis):** Đánh giá hiện trạng công cụ Cppcheck/Clang Analyzer trên môi trường thực tế.
4. **Rà soát chuyên sâu 5 nhóm rủi ro an toàn bộ nhớ C:** Heap allocation, Pointer lifetime, Buffer & array bounds, Integer safety, và Input parser.
5. **Chuẩn hóa Danh mục Rủi ro (Risk Register):** Phân loại rủi ro dựa trên khả năng đạt tới thực tế (*Reachability*), không suy đoán cảm tính.
6. **Đặc tả hình thức toàn diện 18 REQ:** Chuyển đổi toàn bộ 18 yêu cầu từ workbook học thuật gốc thành đặc tả tiền điều kiện, hậu điều kiện, bất biến CTL và điều kiện vi phạm.
7. **Chuẩn bị mô hình hình thức hóa:** Xây dựng hệ thống 5 mô hình trạng thái rời rạc, nhận diện bẫy logic đúng hiển nhiên (*Vacuous Truth*) từ các tiên đề loại trừ lẫn nhau (*LoaiTruLanNhau*), và thiết lập các bài toán SMT Bit-Vector (Z3/SMT-LIB v2).
8. **Đánh giá tính phủ đa chiều (Coverage Assessment):** Đo lường độ phủ đặc tả, độ phủ rà soát mã nguồn, và tuyên bố minh bạch các chỉ số chưa đo (*NOT_MEASURED*).
9. **Lập kế hoạch mô phỏng rủi ro bằng C Test Harness (SIM-001, SIM-002):** Sẵn sàng cho Bước 3.
10. **Lưu trữ bằng chứng có thể kiểm chứng độc lập:** Tạo Evidence Manifest với mã băm SHA-256 thực tế.

---

## 2. PHIÊN BẢN MÃ NGUỒN VÀ MÔI TRƯỜNG THỰC NGHIỆM

### 2.1. Mã nguồn Baseline v0
- **Repository:** `ATPM_Group_04_inventory-memory-audit`
- **Commit Baseline:** `44202070c68002d9af068374478cdc83098dd4d8` (Nhánh `audit/prepare-v0`)
- **Tình trạng:** Baseline đã nghiệm thu 100% ở Bước 1, không chứa code tạm, sạch build.
- **Quy mô mã nguồn:**
  - **7 module C nghiệp vụ:** `models.h`, `product.c`, `inventory.c`, `file_io.c`, `validation.c`, `utils.c`, `report.c`, `main.c`.
  - **Quy mô dòng mã thực tế:**
    - C Source Files (`source/src/*.c`): **1.809 Code LOC** (2.181 Total Lines).
    - C Header Files (`source/include/*.h`): **139 Code LOC** (329 Total Lines).
    - C Test Harnesses (`tests/` & `source/tests/`): **281 Code LOC** (357 Total Lines).
    - **Tổng cộng dự án:** **2.229 Code LOC** (2.867 Total Lines).
  - **Tổng số hàm C được rà soát:** **70 hàm** (chi tiết tại [source_inventory.md](file:///c:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/docs/verification/step2/source_inventory.md)).

### 2.2. Khám phá Môi trường Thực nghiệm và ABI
Thực nghiệm biên dịch và chạy file thăm dò [probe_env.c](file:///c:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/results/step2/compiler/environment.txt) với GCC:
- **Hệ điều hành:** Windows 11 Enterprise (Build 10.0.26100)
- **Môi trường:** MSYS2 / UCRT64
- **Trình biên dịch:** GCC 16.1.0 (`x86_64-w64-mingw32`, C17 Standard `__STDC_VERSION__ = 201710L`)
- **Công cụ Build:** GNU Make 4.4.1
- **Mô hình Dữ liệu (Data Model):** LLP64 (Chuẩn 64-bit Windows)
  - `sizeof(int) = 4` bytes (32-bit, `INT_MAX = 2,147,483,647`)
  - `sizeof(long) = 4` bytes (32-bit trên Windows LLP64)
  - `sizeof(long long) = 8` bytes (64-bit)
  - `sizeof(size_t) = 8` bytes (64-bit unsigned, `SIZE_MAX = 18,446,744,073,709,551,615`)
  - `sizeof(void*) = 8` bytes (64-bit pointer)
  - `sizeof(Product) = 208` bytes (Gồm 190 bytes chuỗi + 4 bytes `int quantity` + 6 bytes padding căn lề 8-byte + 8 bytes `double price`).
  - `sizeof(ProductList) = 24` bytes
  - `sizeof(CategorySummary) = 80` bytes
  - `sizeof(InventorySummary) = 456` bytes

> [!NOTE]
> **PHÁT HIỆN QUAN TRỌNG VỀ ABI:**  
> Kích thước thực tế của `Product` đo được là **208 bytes**, khác với một số tài liệu cũ ước lượng 184 bytes do compiler căn lề struct theo `double price` (8-byte alignment). Điều này làm thay đổi ngưỡng tràn số học lý thuyết của `new_cap * sizeof(Product)` sang $\lfloor 2^{64}-1 / 208 \rfloor = 88.686.269.585.142.075$ phần tử.

---

## 3. PHÂN TÍCH COMPILE TIME (GCC COMPILER ANALYSIS)

### 3.1. Biên dịch Baseline Chuẩn
Thực hiện chuỗi lệnh build chuẩn trong thư mục `source/`:
```bash
make clean && make && make debug && make test && make smoke-test
```
- **Kết quả:**
  - Build chính (`app.exe`): **Thành công** (Exit code: 0).
  - Build gỡ lỗi (`app_debug.exe`): **Thành công** (Exit code: 0).
  - Chạy Unit Test (`tests/test_inventory.exe`, `tests/test_utils.exe`): **PASS** (Exit code: 0).
  - Chạy Smoke Test (17 kịch bản tự động): **PASS 17/17** (Exit code: 0).
  - Bằng chứng lưu tại: [build.log](file:///c:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/results/step2/compiler/build.log) và [build-exit-code.txt](file:///c:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/results/step2/compiler/build-exit-code.txt).

### 3.2. Lượt Phân tích Cảnh báo Tăng cường (Enhanced Diagnostics Run)
Thực hiện lệnh phân tích chuyên sâu độc lập với bộ cờ nghiêm ngặt nhất của GCC:
```bash
gcc -std=c17 -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Wshadow -Wformat=2 -Wnull-dereference -Iinclude -c src/*.c
```
- **Số lượng Compiler Errors:** **0**
- **Số lượng Compiler Warnings:** **0**
- **Exit Code:** **0**
- **Bằng chứng lưu tại:** [diagnostics.log](file:///c:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/results/step2/compiler/diagnostics.log) và [compiler-findings.csv](file:///c:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/results/step2/compiler/compiler-findings.csv).

> [!IMPORTANT]
> **KẾT LUẬN VỀ KẾT QUẢ 0 WARNING:**  
> Việc GCC không phát hiện cảnh báo nào ngay cả dưới `-Wconversion` và `-Wsign-conversion` chứng minh mã nguồn baseline `v0` đã được chuẩn hóa rất tốt về mặt cú pháp kiểu dữ liệu ở Bước 1. **Tuy nhiên, theo nguyên tắc kiểm toán an toàn phần mềm, 0 compiler warning TUYỆT ĐỐI KHÔNG ĐỒNG NGHĨA VỚI CHƯƠNG TRÌNH KHÔNG CÓ LỖ HỔNG AN TOÀN BỘ NHỚ**. Compiler không thể phát hiện các lỗi logic ngữ cảnh như mất hiệu lực con trỏ mượn sau `realloc`, tràn số học cộng dồn vượt `INT_MAX`, hoặc các kịch bản cạn kiệt tài nguyên hệ thống.

---

## 4. PHÂN TÍCH TĨNH MÃ NGUỒN (STATIC CODE ANALYSIS)

### 4.1. Khám phá và Ghi nhận Công cụ
- **Cppcheck:** Kiểm tra trong môi trường hệ thống cho thấy lệnh `cppcheck` chưa được cài đặt trong biến môi trường PATH (Exit code: 127).
- **Trạng thái ghi nhận:** Đánh dấu chính thức **`TOOL_UNAVAILABLE`** theo đúng chỉ đạo tại Mục V.2 của Master Prompt.
- Nhóm đã lưu trữ bằng chứng và cung cấp hướng dẫn cài đặt cụ thể cho các môi trường tại [results/step2/cppcheck/](file:///c:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/results/step2/cppcheck/):
  - MSYS2 UCRT64: `pacman -S mingw-w64-ucrt-x86_64-cppcheck`
  - Windows: `winget install Cppcheck.Cppcheck`
  - Linux: `sudo apt-get install cppcheck`
- **Clang Static Analyzer / Clang-tidy:** Tương tự, `clang` chưa có sẵn trong môi trường Windows hiện tại (`TOOL_UNAVAILABLE`).

### 4.2. Rà soát Mã nguồn Thủ công Kết hợp Phân tích Luồng Dữ liệu (Code Review)
Để bù đắp việc công cụ tự động chưa khả dụng, nhóm kỹ sư đã tiến hành rà soát thủ công 100% 70 hàm C dựa trên mô hình đối tượng bộ nhớ (Memory Ownership & Control Flow Analysis). Toàn bộ 10 phát hiện đánh giá (`CR-001` tới `CR-010`) được lưu trữ tại [code-review-findings.csv](file:///c:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/results/step2/review/code-review-findings.csv).

---

## 5. RÀ SOÁT CHUYÊN SÂU 5 NHÓM RỦI RO BỘ NHỚ

### 5.1. Nhóm A – Cấp phát Vùng nhớ Động Heap (Heap Allocation Safety)
- **Hàm trọng tâm:** `product_list_init()`, `add_product()`, `product_list_free()`.
- **Thao tác:** `malloc`, `realloc`, `free`.
- **Đánh giá cơ chế phòng vệ:**
  - Trong `add_product()` (`product.c:44`), chương trình sử dụng biến con trỏ tạm:
    ```c
    Product *new_items = realloc(list->items, new_cap * sizeof(Product));
    if (!new_items) return STATUS_ERR_NO_MEMORY;
    list->items = new_items;
    ```
  - **Kết luận:** Mã nguồn xử lý đúng mẫu an toàn cấp phát lại; nếu `realloc` trả về `NULL`, khối nhớ gốc `list->items` **không bị rò rỉ (No Memory Leak)**. Rủi ro này được xếp loại **`GUARDED`**.

### 5.2. Nhóm B – An toàn Con trỏ và Vòng đời Truy cập (Pointer Safety)
- **Hàm trọng tâm:** `find_product_by_id()`, `find_product()`.
- **Mô hình hợp đồng con trỏ:** Các hàm tra cứu trả về con trỏ mượn trực tiếp `Product*` trỏ vào phần tử nội bộ của mảng `list->items`.
- **Rủi ro Mất hiệu lực Con trỏ (Pointer Invalidation):**
  - Nếu một hàm gọi giữ con trỏ `Product *p` trong khi thực hiện một thao tác khác làm danh sách mở rộng (`add_product`), `realloc` có thể cấp phát vùng nhớ ở địa chỉ mới và giải phóng khối nhớ cũ. Khi đó `p` trở thành con trỏ lơ lửng (*dangling pointer*).
  - **Kết quả rà soát Baseline v0:** Trong toàn bộ `main.c` và `inventory.c`, không có bất kỳ luồng thực thi nào lưu giữ con trỏ mượn qua lời gọi `add_product()`. Sau khi tìm kiếm, mã nguồn đọc/ghi ngay hoặc sao chép dữ liệu.
  - **Kết luận:** Rủi ro Use-After-Free sau `realloc` là một **Rủi ro Hợp đồng Kiến trúc (Architectural Contract Risk)** chứ **KHÔNG PHẢI LỖ HỔNG ĐÃ XÁC NHẬN (NOT A CONFIRMED DEFECT)** trong baseline v0. Xếp loại: **`REACHABILITY_UNPROVEN`**.

### 5.3. Nhóm C – Giới hạn Mảng và Bộ đệm Chuỗi (Buffer & Array Bounds)
- **Hàm trọng tâm:** `safe_string_copy()`, `trim_whitespace()`, `calculate_category_summaries()`.
- **Đánh giá `calculate_category_summaries()` (Xác minh CANDIDATE-03):**
  - Tài liệu sơ bộ trước đây từng nghi ngờ khả năng tràn mảng khi vượt quá 32 danh mục.
  - Kiểm tra thực tế tại `report.c:72-76`:
    ```c
    if (*out_count >= max_summaries) {
        return STATUS_ERR_OVERFLOW;
    }
    ```
  - Mã nguồn đã chủ động kiểm tra giới hạn mảng trước khi ghi bản ghi mới vào `summaries[*out_count]`.
  - **Kết luận:** Nhận định lỗi Heap Buffer Overflow (CWE-122) là **`FALSE_POSITIVE`**. Đây là cơ chế phòng vệ có chủ đích (**`DEFENSIVE_BEHAVIOR`**) và giới hạn dung lượng nghiệp vụ.
- **Đánh giá `safe_string_copy()`:** Sử dụng `strncpy` kết hợp ép `dest[dest_size - 1] = ' '`. An toàn, xếp loại **`GUARDED`**.

### 5.4. Nhóm D – An toàn Số học Số nguyên (Integer Safety)
- **Hàm trọng tâm:** `add_product()`, `import_stock()`, `export_stock()`.
- **Phép toán `capacity * 2` và `new_cap * sizeof(Product)`:**
  - Về mặt lý thuyết số học, phép nhân unsigned 64-bit có thể tràn nếu `capacity >= 2^{63}` hoặc `new_cap >= 88.686.269.585.142.076`.
  - Tuy nhiên, trong môi trường thực tế, hệ thống không thể chứa đủ dữ liệu để đạt tới dung lượng này. Xếp loại: **`REACHABILITY_UNPROVEN`**.
- **Phép toán `p->quantity += quantity` tại `inventory.c:48`:**
  - Kiểu dữ liệu `int` (32-bit signed).
  - Hàm `validate_quantity` chỉ kiểm tra `quantity > 0`, không kiểm tra trần `p->quantity + quantity <= INT_MAX`.
  - Nếu nhập kho liên tiếp vượt $2.147.483.647$, phép cộng số nguyên có dấu sẽ bị tràn (*Signed Integer Overflow*), dẫn tới hành vi bất định (Undefined Behavior theo C17).
  - Xếp loại: **`POTENTIALLY_REACHABLE`** (Ưu tiên kiểm chứng ở Bước 3).

### 5.5. Nhóm E – Xử lý Dữ liệu Đầu vào Tệp tin (Input / Parser Safety)
- **Hàm trọng tâm:** `load_products_from_file()`, `parse_product_line()`.
- **Bộ đệm 512 bytes và Dòng Quá Dài:**
  - `fgets(line, 512, fp)` đọc tối đa 511 ký tự.
  - Mã nguồn kiểm tra: `if (!strchr(line, '
') && !feof(fp))` $\implies$ phát hiện dòng bị cắt cụt do vượt quá 512 bytes và trả về `STATUS_ERR_INVALID_DATA`.
  - Không xảy ra tràn bộ đệm ghi đè ngăn xếp (*Stack Buffer Overflow*). Xếp loại: **`GUARDED`**.
- **Số Lượng Token Phân tách:**
  - Mảng cố định `char *tokens[8]`. Vòng lặp `strtok_r` có guard `token_count < 8`. An toàn, xếp loại **`GUARDED`**.
- **Xử lý Dữ liệu Sai Định dạng (Legacy 4 trường vs 6 trường):**
  - Dòng chỉ có 4 trường được xử lý an toàn bằng cách gán giá trị mặc định cho `quantity` và `price`.
  - Dòng thiếu trường hoặc chứa ký tự không hợp lệ bị từ chối an toàn.

---

## 6. DANH MỤC RỦI RO CHUẨN HÓA (RISK REGISTER)

10 rủi ro được tổng hợp và phân loại độc lập tại [risk_register.csv](file:///c:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/docs/verification/step2/risk_register.csv) và [risk_register.md](file:///c:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/docs/verification/step2/risk_register.md):

| Risk ID | Tập tin / Hàm | Dòng mã | Bản chất Rủi ro | CWE | Cơ chế Guard Hiện tại | Trạng thái Khả đạt | Yêu cầu REQ | Phương pháp Kiểm chứng Đề xuất |
|---|---|---|---|---|---|---|---|---|
| **RISK-001** | `product.c` / `add_product` | 42 | Tràn số nhân đôi capacity (`capacity * 2`) | CWE-190 | Không có guard tường minh; phụ thuộc ngưỡng RAM | **REACHABILITY_UNPROVEN** | REQ-2, REQ-3 | SMT Bit-Vector / CBMC |
| **RISK-002** | `product.c` / `add_product` | 44 | Tràn số byte cấp phát (`new_cap * sizeof(Product)`) | CWE-190 | Không có guard; realloc thất bại trước khi đạt ngưỡng | **REACHABILITY_UNPROVEN** | REQ-3 | SMT Bit-Vector / CBMC |
| **RISK-003** | `product.c` / `add_product` | 44-48 | Rò rỉ bộ nhớ khi `realloc` thất bại | CWE-401 | Guard con trỏ tạm `new_items` bảo toàn `list->items` | **GUARDED** | REQ-4 | CBMC / Error Injection Harness |
| **RISK-004** | `product.c` / `find_product_by_id` | 57-80 | Con trỏ mượn mất hiệu lực sau khi realloc dời mảng | CWE-416 | Hợp đồng kiến trúc; callers không lưu con trỏ qua realloc | **REACHABILITY_UNPROVEN** | REQ-14 | Contract Checking / C Harness SIM-001 |
| **RISK-005** | `product.c` / `product_list_free` | 16-25 | Giải phóng kép hoặc giải phóng con trỏ rác | CWE-415 | Gán `list->items = NULL`, `count = 0`, `capacity = 0` | **GUARDED** | REQ-13, REQ-15 | CTL Verification / ASan |
| **RISK-006** | `utils.c` / `safe_string_copy` | 14-25 | Tràn bộ đệm đích khi sao chép chuỗi | CWE-120 | Kiểm tra `dest_size > 0`, ép `dest[dest_size - 1] = 0` | **GUARDED** | REQ-5, REQ-6 | Unit Bounds Tests / CBMC |
| **RISK-007** | `file_io.c` / `load_products_from_file` | 31-48 | Tràn bộ đệm đọc dòng tệp 512 bytes | CWE-120 | `fgets(512)` + kiểm tra thiếu newline trả về lỗi | **GUARDED** | REQ-16, REQ-17 | Fuzzing Harness SIM-002 |
| **RISK-008** | `file_io.c` / `parse_product_line` | 86-105 | Tràn mảng con trỏ token khi dòng có quá nhiều dấu phẩy | CWE-121 | Guard `token_count < 8` trong vòng lặp `strtok_r` | **GUARDED** | REQ-17 | Token Fuzzing SIM-002 |
| **RISK-009** | `file_io.c` / `parse_product_line` | 80-140 | Lỗi xử lý dữ liệu sai định dạng (4 trường vs 6 trường) | CWE-20 | Kiểm tra số token >= 4, parse an toàn qua `safe_str_to_*` | **POTENTIALLY_REACHABLE** | REQ-16, REQ-18 | Boundary Parsing Harness |
| **RISK-010** | `report.c` / `calculate_category_summaries` | 72-76 | Tràn mảng tổng hợp danh mục vượt quá 32 phần tử | CWE-122 | Guard `*out_count >= max_summaries` trả về mã lỗi | **FALSE_POSITIVE** | REQ-11 | Model Checking Invariant |

---

## 7. ĐẶC TẢ TOÀN BỘ 18 YÊU CẦU AN TOÀN BỘ NHỚ (18 REQs)

Toàn bộ 18 REQ từ `Spec_Inventory_Memory_Safety_Audit_18REQ_CHUAN.xlsx` đã được chuyển hóa thành bản đặc tả kỹ thuật đầy đủ tại [requirement_specifications.md](file:///c:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/docs/verification/step2/requirement_specifications.md) và ma trận truy vết hai chiều tại [req_traceability.csv](file:///c:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/docs/verification/step2/req_traceability.csv).

### Tóm tắt Phân nhóm 18 REQ:
1. **Nhóm Cấp phát và Quản lý Danh sách (REQ-1 đến REQ-4):**
   - `REQ-1`: Khởi tạo danh sách động hợp lệ (`capacity = 16`, `items != NULL`).
   - `REQ-2`: Mở rộng dung lượng nhân đôi không tràn số (`capacity * 2 <= SIZE_MAX`).
   - `REQ-3`: Cấp phát lại kích thước byte an toàn (`new_cap * sizeof(Product) <= SIZE_MAX`).
   - `REQ-4`: Xử lý cạn kiệt bộ nhớ không làm rò rỉ khối nhớ cũ (`realloc_failed -> owner_valid`).
2. **Nhóm Vùng đệm và Chuỗi Ký tự (REQ-5 đến REQ-9):**
   - `REQ-5`: Bảo đảm kích thước trường Mã sản phẩm (`id[20]`).
   - `REQ-6`: Bảo đảm kích thước trường Tên sản phẩm (`name[100]`).
   - `REQ-7`: Bảo đảm kích thước trường Danh mục (`category[50]`).
   - `REQ-8`: Bảo đảm kích thước trường Đơn vị tính (`unit[20]`).
   - `REQ-9`: Bất biến luôn đóng NUL (`str[sizeof(str)-1] == ' '`) cho mọi chuỗi.
3. **Nhóm Số học và Tính toán Kho (REQ-10 đến REQ-12):**
   - `REQ-10`: Phép cộng số lượng nhập kho không tràn số nguyên (`current + add <= INT_MAX`).
   - `REQ-11`: Giới hạn mảng tổng hợp danh mục (`out_count <= max_summaries`).
   - `REQ-12`: Thống kê tổng giá trị và số lượng kho không tràn số nguyên 64-bit.
4. **Nhóm Con trỏ và Quyền Sở hữu (REQ-13 đến REQ-15):**
   - `REQ-13`: Giải phóng toàn bộ tài nguyên heap khi dọn dẹp danh sách (`items = NULL`).
   - `REQ-14`: Không dereference con trỏ mượn sau khi khối nhớ bị realloc di chuyển.
   - `REQ-15`: Ngăn chặn giải phóng kép danh sách (`AG(!(list_live & list_freed))`).
5. **Nhóm Tệp tin và Bộ Phân tích Cú pháp (REQ-16 đến REQ-18):**
   - `REQ-16`: Đọc dòng tệp tin an toàn với bộ đệm 512 bytes (`line[512]`).
   - `REQ-17`: Tách token phân cách phẩy không vượt quá sức chứa mảng `tokens[8]`.
   - `REQ-18`: Phân tích dữ liệu số nguyên và số thực an toàn từ chuỗi đầu vào.

---

## 8. MÔ HÌNH HÓA VÀ CHUẨN BỊ KIỂM CHỨNG HÌNH THỨC

Chi tiết tại [formalization_plan.md](file:///c:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/docs/verification/step2/formalization_plan.md):

### 8.1. Hệ Thống 5 Mô Hình Trạng Thái Rời Rạc
- **Phân hệ Danh sách Heap (`list_*`):** 7 trạng thái (`list_valid`, `list_growth`, `list_growth_ok`, `list_growth_failed`, `list_cleanup`, `list_freed_state`, `list_cleanup_rejected`).
- **Phân hệ Con trỏ Mượn (`ptr_*`):** 2 trạng thái (`ptr_valid`, `ptr_invalidated`).
- **Phân hệ Tệp tin (`file_*`):** 4 trạng thái (`file_idle`, `file_parsing`, `file_loaded`, `file_rejected`).
- **Phân hệ Bộ đệm (`buffer_*`):** 4 trạng thái (`buffer_idle`, `buffer_processing`, `buffer_safe`, `buffer_rejected`).
- **Phân hệ Nghiệp vụ (`product_op_*`):** 4 trạng thái (`product_op_idle`, `product_op_processing`, `product_op_done`, `product_op_rejected`).

### 8.2. Nhận Diện Bẫy Logic "Đúng Hiển Nhiên" (Vacuous Truth)
- Sheet `LoaiTruLanNhau` định nghĩa 8 cặp mệnh đề loại trừ lẫn nhau (ví dụ: `(ptr_deref, ptr_invalid)`).
- **Phát hiện học thuật:** Nếu người kiểm chứng khai báo tiên đề `TRANS !(ptr_deref & ptr_invalid)` trong NuSMV, công thức CTL-03 `AG(ptr_invalid -> !ptr_deref)` sẽ đúng hiển nhiên do cấu trúc logic chứ không chứng minh được mã nguồn C an toàn.
- **Giải pháp chuyển giao cho Bước 3:** Trong mô hình kiểm chứng Bước 3, các biến trạng thái phải được kích hoạt độc lập theo đường chạy của mã C; không được biến thuộc tính an toàn cần kiểm chứng thành tiên đề tiên nghiệm.

### 8.3. SMT Bit-Vector (Z3 Formulation)
Đã thiết lập 3 script SMT-LIB v2 chính xác cho kiến trúc LLP64:
- `SMT-01`: Tràn dung lượng `capacity * 2` trên `(_ BitVec 64)` $\implies$ SAT lý thuyết, Unreachable runtime.
- `SMT-02`: Tràn kích thước cấp phát `new_cap * 208` trên `(_ BitVec 64)` $\implies$ SAT lý thuyết, Unreachable runtime.
- `SMT-03`: Tràn số nguyên có dấu `current_stock + add_quantity` trên `(_ BitVec 32)` $\implies$ **SAT & POTENTIALLY_REACHABLE**.

---

## 9. ĐÁNH GIÁ TÍNH PHỦ (COVERAGE ASSESSMENT)

Chi tiết tại [coverage_matrix.csv](file:///c:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/docs/verification/step2/coverage_matrix.csv) và [coverage_report.md](file:///c:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/docs/verification/step2/coverage_report.md):

1. **Requirement Mapping Coverage:** **18 / 18 (100.0%)** – 100% REQ trong đề tài được ánh xạ đầy đủ.
2. **Source Review Coverage:** **7 / 7 modules (100.0%)**, **70 / 70 hàm (100.0%)**, **2.229 Code LOC (100.0%)**.
3. **Risk Specification Coverage:** **10 / 10 rủi ro (100.0%)** được đặc tả chi tiết.
4. **Planned State Transition Coverage:** **21 / 21 trạng thái (100.0% đặc tả)**.
5. **Dynamic Code/Branch Coverage:** **`NOT_MEASURED`** (Giai đoạn 2 chưa tích hợp gcov).
6. **Formal State Space Exploration Coverage:** **`NOT_MEASURED`** (Chờ chạy Model Checker ở Bước 3).

---

## 10. KẾ HOẠCH MÔ PHỎNG RỦI RO BẰNG C (SIMULATION PLAN)

Chi tiết tại [c_simulation_plan.md](file:///c:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/docs/verification/step2/c_simulation_plan.md):
- **SIM-001 (Heap Reallocation & Pointer Invalidation):** Thiết kế C harness cô lập cưỡng bức dung lượng cận biên $2^{62}$ để quan sát phản ứng của `realloc` và đo đạc hành vi truy cập con trỏ mượn cũ.
- **SIM-002 (Parser Buffer & Token Boundary Fuzzing):** Xây dựng bộ test corpus gồm 7 trường hợp dị dạng (dòng > 512 bytes, > 8 tokens, < 4 tokens, ký tự NUL nhúng ` `, số liệu NaN/Overflow) để kích hoạt toàn diện các nhánh phòng vệ của `file_io.c`.
- **Vai trò của nhánh `audit/v0-file-io-vulnerable`:** Được lưu giữ làm mẫu đối chứng lỗi (Benchmark Baseline) để kiểm thử độ nhạy của công cụ phân tích tĩnh trong Bước 3, tuyệt đối không merge vào baseline chính.

---

## 11. HỒ SƠ BẰNG CHỨNG (EVIDENCE MANIFEST)

Toàn bộ 22 bằng chứng thực nghiệm đã được lập chỉ mục và băm mã SHA-256 thực tế tại [evidence_manifest.csv](file:///c:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/results/step2/evidence_manifest.csv). Bất kỳ thành viên nào cũng có thể kiểm chứng tính toàn vẹn của kết quả bằng cách so sánh mã băm của từng tập tin log.

---

## 12. KẾT LUẬN VÀ SẴN SÀNG CHUYỂN GIAO BƯỚC 3

### 12.1. Trả lời Trọng tâm Nhận xét của Giảng viên
1. **Xuất phát từ Source C:** Đã rà soát chi tiết 100% 70 hàm và 2.229 dòng lệnh.
2. **Compile Time & Errors/Warnings:** Thu được 0 lỗi và 0 cảnh báo dưới bộ cờ nghiêm ngặt nhất của GCC C17; hiểu rõ bản chất cảnh báo 0 không đồng nghĩa với không có rủi ro logic.
3. **Tổng hợp & Đặc tả Hình thức:** Hoàn thành 18 REQ, 18 CTL properties, 5 mô hình trạng thái và 3 bài toán SMT.
4. **Tính Phủ:** Đạt 100% độ phủ đặc tả và rà soát mã nguồn; phân định minh bạch với độ phủ thực thi runtime (`NOT_MEASURED`).
5. **Mô phỏng Rủi ro bằng C:** Đã lập kế hoạch chi tiết hai bài toán then chốt `SIM-001` và `SIM-002`.

### 12.2. Trạng thái Sẵn sàng cho Bước 3
Dự án đã hoàn thành **100% Bước 2** trên nhánh làm việc `audit/step2-risk-specification` và sẵn sàng bước vào **Bước 3: Thực thi Kiểm chứng Hình thức, Phân tích Động và Mô phỏng C**.
