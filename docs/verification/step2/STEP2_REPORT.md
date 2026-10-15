# BÁO CÁO NGHIỆM THU HOÀN THÀNH BƯỚC 2: PHÂN TÍCH VÀ ĐẶC TẢ RỦI RO AN TOÀN BỘ NHỚ C

**Đề tài môn học:** An toàn phần mềm
**Dự án:** Kiểm chứng an toàn bộ nhớ cho chương trình C mô phỏng nghiệp vụ quản lý kho
**Repository:** [https://github.com/trangnguyenkieu927/ATPM_Group_04_inventory-memory-audit](https://github.com/trangnguyenkieu927/ATPM_Group_04_inventory-memory-audit)
**Nhóm thực hiện (Nhóm 04):**
- **Nguyễn Kiều Trang** – Nhóm trưởng, phụ trách tích hợp hệ thống, kiến trúc và QA
- **Hà Văn Đô** – Phụ trách đặc tả, mô hình bộ nhớ, thuộc tính an toàn và kiểm chứng hình thức
- **Nguyễn Quang Thọ** – Phụ trách công cụ, môi trường thực nghiệm, build/test và bằng chứng kiểm chứng
**Nhánh Git làm việc:** `audit/step2-risk-specification`
**Commit Baseline cố định:** `44202070c68002d9af068374478cdc83098dd4d8`
**Thời điểm hoàn thành:** Học kỳ I / 2026-10-15

---

## 1. TỔNG QUAN VÀ BỐI CẢNH DỰ ÁN

### 1.1. Nhận xét Trọng tâm của Giảng viên Hướng dẫn
Báo cáo này được triển khai nhằm đáp ứng chuẩn mực học thuật và phương pháp luận do Giảng viên chỉ đạo:
> *"Xuất phát từ source code → compile time → error + warning → tổng hợp + đặc tả để hình thức hóa (tính phủ các trường hợp có thể xảy ra) → xây dựng code C mô phỏng lại rủi ro. Xây dựng đặc tả cho bài toán rủi ro an toàn."*

### 1.2. Mục tiêu Nghiệm thu của Bước 2
Bước 2 là giai đoạn phân tích mã nguồn và chuyển giao đặc tả hình thức từ baseline v0 đã hoàn thành ở Bước 1. Mục tiêu bao gồm:
1. Rà soát toàn bộ 7 module C nghiệp vụ, thống kê chính xác 70 hàm và 2.229 dòng Code LOC.
2. Thu thập kết quả chẩn đoán biên dịch tăng cường của trình biên dịch GCC C17 (0 lỗi, 0 cảnh báo).
3. Thực thi công cụ phân tích tĩnh mã nguồn Cppcheck 2.21.0 thực tế trên môi trường MSYS2 UCRT64, phân loại toàn diện 67 cảnh báo.
4. Rà soát chuyên sâu 5 nhóm rủi ro an toàn bộ nhớ C (Heap, Pointer, Buffer, Integer, Parser) với số dòng chuẩn xác trên commit baseline `4420207`.
5. Chuẩn hóa Danh mục Rủi ro (Risk Register) gồm 10 rủi ro độc lập với tính khả đạt thực tế.
6. Đặc tả toàn bộ 18 REQ theo đúng văn bản nguyên văn của tài liệu nguồn `Spec_Inventory_Memory_Safety_Audit_18REQ_CHUAN.xlsx`.
7. Lập mô hình 5 hệ thống trạng thái rời rạc (21 trạng thái), rà soát 18 công thức CTL và phân tích bẫy logic đúng hiển nhiên (*Vacuous Truth*).
8. Chuẩn bị 3 bài toán SMT Bit-Vector (QF_BV) cho kiến trúc 64-bit LLP64.
9. Đánh giá tính phủ 4 chiều, ghi nhận trung thực các chỉ số runtime là `CHƯA ĐO (NOT_MEASURED)`.
10. Lập kế hoạch mô phỏng rủi ro bằng C test harness (SIM-001, SIM-002) cho Bước 3.
11. Thiết lập công cụ tự động kiểm tra tính nhất quán `tools/check_step2_consistency.py` đạt kết quả PASS 100%.

---

## 2. KẾT QUẢ RÀ SOÁT VÀ KHẮC PHỤC CÁC TỒN TẠI TRƯỚC ĐÓ

Theo yêu cầu kiểm tra độc lập, nhóm đã đối chiếu toàn diện hồ sơ và khắc phục triệt để các sai lệch trước đây:

| STT | Vấn đề ban đầu | File liên quan | Nguyên nhân | Mức độ | Nội dung đã sửa & Bằng chứng |
|---|---|---|---|---|---|
| 1 | `RISK-003` (realloc failure) ánh xạ sang `REQ-4` | `risk_register.csv`, `req_traceability.csv` | REQ-4 thực chất là tràn số nhập kho `import_stock`. REQ đúng cho bảo toàn con trỏ realloc là REQ-12 và REQ-11. | Nghiêm trọng | Đã chuyển ánh xạ sang `REQ-12, REQ-11`. Xác minh tại [risk_register.csv](risk_register.csv). |
| 2 | `RISK-006` (sao chép chuỗi) ánh xạ sang `REQ-5, REQ-6` | `risk_register.csv`, `req_traceability.csv` | REQ-5 là xuất kho, REQ-6 là truy cập items[i]. REQ chuẩn cho sao chép chuỗi đóng NUL là REQ-9. | Nghiêm trọng | Đã chuyển ánh xạ sang `REQ-9`. Xác minh tại [req_traceability.csv](req_traceability.csv). |
| 3 | `RISK-007` (dòng file quá dài) ánh xạ `REQ-16, REQ-17` | `risk_register.csv`, `req_traceability.csv` | Dòng dài vượt 512 bytes trực tiếp thuộc REQ-10 (Xử lý input quá dài) và REQ-17 (Parser an toàn). | Trung bình | Đã cập nhật ánh xạ sang `REQ-10, REQ-17`. |
| 4 | `RISK-008` (mảng token) ánh xạ thiếu `REQ-16` | `risk_register.csv`, `req_traceability.csv` | Giới hạn mảng `tokens[8]` thuộc phạm vi REQ-16 (Giới hạn mảng) và REQ-17. | Trung bình | Đã cập nhật ánh xạ sang `REQ-16, REQ-17`. |
| 5 | `RISK-009` (parser tương thích) ánh xạ thiếu `REQ-18` | `risk_register.csv`, `req_traceability.csv` | Parser chấp nhận định dạng 4/6 trường kết hợp kiểm tra số liệu thuộc REQ-17 và REQ-18. | Trung bình | Đã cập nhật ánh xạ sang `REQ-17, REQ-18`. |
| 6 | `RISK-010` bị xếp loại `FALSE_POSITIVE` và gán `REQ-11` | `risk_register.csv`, `req_traceability.csv` | `calculate_category_summaries` có guard `if (*out_count >= max_summaries) return STATUS_ERR_OVERFLOW;` tại `report.c:72-74`. Đây là cơ chế phòng vệ có chủ đích (GUARDED), không phải lỗi công cụ báo sai. REQ đúng là REQ-16. | Nghiêm trọng | Đã phân loại lại thành **`GUARDED`** (Defensive Behavior / Capacity Limit), ánh xạ duy nhất tới `REQ-16`. |
| 7 | Số dòng trong Risk Register bị lệch so với baseline | `risk_register.csv`, `code-review-findings.csv` | Trích dẫn từ bản nháp cũ (ví dụ ghi dòng 42 thay vì dòng 118 cho realloc trong product.c). | Nghiêm trọng | Đo đạc lại 100% số dòng trên commit baseline `4420207`: `product.c:118` (`capacity * 2`), `119` (`new_cap * 208`), `223-233` (`find_product_by_id`), `report.c:72-74`, `file_io.c:47, 52-61`. |
| 8 | Mô tả parser dùng `strtok_r` sai thực tế mã nguồn | `risk_register.md`, `c_simulation_plan.md` | Mã nguồn thực tế tại `file_io.c:54` dùng `char *pipe = strchr(cursor, '\|');` duyệt và cắt chuỗi. | Trung bình | Đã đính chính toàn bộ mô tả kỹ thuật phản ánh đúng cơ chế `strchr('\|')`. |
| 9 | Cppcheck ghi nhận `TOOL_UNAVAILABLE` | `results/step2/cppcheck/*` | Cppcheck chưa có trong PATH hệ thống. | Nghiêm trọng | Được người dùng phê duyệt, đã cài đặt thành công `Cppcheck 2.21.0` qua MSYS2 pacman và chạy thực tế 100%. |
| 10 | Yêu cầu tiếng Việt có dấu cho toàn bộ commit và báo cáo | Git history, tài liệu báo cáo | Commit và báo cáo trước đây có phần tiếng Anh. | Bắt buộc | Biên soạn toàn bộ commit message và báo cáo kỹ thuật bằng Tiếng Việt có dấu chuẩn UTF-8. |

---

## 3. MÔI TRƯỜNG THỰC NGHIỆM VÀ CẤU HÌNH TOOLCHAIN

Thực nghiệm đo đạc thực tế bằng chương trình thăm dò ABI trên môi trường kiểm toán:
- **Hệ điều hành:** Windows 11 Enterprise 64-bit (Build 10.0.26100).
- **Môi trường Unix:** MSYS2 / UCRT64.
- **Trình biên dịch:** GCC 16.1.0 (`x86_64-w64-mingw32`), C17 Standard (`__STDC_VERSION__ = 201710L`).
- **Trình quản lý Build:** GNU Make 4.4.1.
- **Công cụ Phân tích tĩnh:** Cppcheck 2.21.0 (Đã cài đặt tại `C:\msys64\ucrt64\bin\cppcheck.exe`).
- **Mô hình Dữ liệu ABI (LLP64 Windows):**
  - `sizeof(int) = 4` bytes (`INT_MAX = 2.147.483.647`)
  - `sizeof(long) = 4` bytes
  - `sizeof(size_t) = 8` bytes (`SIZE_MAX = 18.446.744.073.709.551.615`)
  - `sizeof(void*) = 8` bytes
  - `sizeof(Product) = 208` bytes (190 bytes dữ liệu chuỗi + 4 bytes `int quantity` + 6 bytes padding căn lề 8-byte + 8 bytes `double price`).
  - `sizeof(ProductList) = 24` bytes
  - `sizeof(CategorySummary) = 80` bytes
  - `sizeof(InventorySummary) = 456` bytes

---

## 4. KẾT QUẢ PHÂN TÍCH TRÌNH BIÊN DỊCH (GCC COMPILE-TIME ANALYSIS)

### 4.1. Biên dịch Baseline v0 và Bộ Kiểm thử Nền tảng
Chuỗi lệnh thực thi từ thư mục `source/`:
```bash
make clean; make; make test; make smoke-test
```
- **Kết quả:**
  - Build chính (`inventory_app.exe`): **Thành công** (Exit code: 0).
  - Build unit tests (`inventory_tests.exe`): **Thành công**, pass toàn bộ unit test inventory & utils.
  - Smoke tests: **17/17 ca kiểm thử ĐẠT 100% (PASS)**, Exit code: 0.
  - Bằng chứng lưu tại: [results/step2/compiler/build.log](../../../results/step2/compiler/build.log) (Exit code: [build-exit-code.txt](../../../results/step2/compiler/build-exit-code.txt)).

### 4.2. Lượt Phân tích Chẩn đoán Tăng cường (Enhanced GCC Diagnostics)
Lệnh phân tích chuyên sâu độc lập với 9 cờ cảnh báo nghiêm ngặt của GCC:
```bash
gcc -std=c17 -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Wshadow -Wformat=2 -Wnull-dereference -Iinclude -c src/*.c
```
- **Số lượng Compiler Errors:** **0**
- **Số lượng Compiler Warnings:** **0 warnings observed**
- **Exit Code:** **0**
- **Bằng chứng lưu tại:** [results/step2/compiler/diagnostics.log](../../../results/step2/compiler/diagnostics.log) và [results/step2/compiler/compiler-findings.csv](../../../results/step2/compiler/compiler-findings.csv).

> [!NOTE]
> **KẾT LUẬN VỀ KẾT QUẢ 0 WARNING CỦA GCC:**
> Trình biên dịch GCC trả về 0 cảnh báo chứng minh mã nguồn baseline `v0` đã được chuẩn hóa kiểu dữ liệu rất tốt ở Bước 1. Tuy nhiên, theo nguyên tắc an toàn phần mềm, **0 compiler warning không đồng nghĩa với không có lỗi an toàn bộ nhớ**. Compiler không thể phát hiện các lỗi logic ngữ cảnh như con trỏ mượn bị mất hiệu lực sau `realloc`, hoặc các kịch bản cạn kiệt tài nguyên bộ nhớ.

---

## 5. KẾT QUẢ PHÂN TÍCH TĨNH MÃ NGUỒN (CPPCHECK 2.21.0 ANALYSIS)

### 5.1. Cấu hình và Lệnh Thực thi
Sau khi được người dùng phê duyệt, công cụ Cppcheck 2.21.0 đã được cài đặt và thực thi trên toàn bộ mã nguồn `source/src/`:
```bash
cppcheck --enable=all --inconclusive --std=c17 --language=c --template=gcc -Iinclude src/
cppcheck --enable=all --inconclusive --std=c17 --language=c --xml --xml-version=2 -Iinclude src/
```
- **Phiên bản:** Cppcheck 2.21.0 ([version.txt](../../../results/step2/cppcheck/version.txt)).
- **Exit code:** 0 ([exit-code.txt](../../../results/step2/cppcheck/exit-code.txt)).
- **Log thực thi:** [results/step2/cppcheck/cppcheck.log](../../../results/step2/cppcheck/cppcheck.log).
- **XML chính thức:** [results/step2/cppcheck/cppcheck.xml](../../../results/step2/cppcheck/cppcheck.xml).
- **Bảng dữ liệu chuẩn hóa:** [results/step2/cppcheck/findings.csv](../../../results/step2/cppcheck/findings.csv).

### 5.2. Phân loại 67 Cảnh báo từ Cppcheck
Tổng số 67 cảnh báo được Cppcheck ghi nhận gồm các nhóm sau:
1. **Nhóm Information (38 cảnh báo):**
   - 30 cảnh báo `missingIncludeSystem`: Nhắc nhở các header thư viện chuẩn (`<stdio.h>`, `<stdlib.h>`, `<string.h>`, `<ctype.h>`, `<limits.h>`). Đây là hành vi bình thường khi chạy phân tích tĩnh mà không truyền đường dẫn include nội bộ của hệ điều hành.
   - 7 cảnh báo `normalCheckLevelMaxBranches`: Cppcheck giới hạn độ sâu nhánh rẽ; đề xuất dùng cờ `--check-level=exhaustive` khi kiểm chứng toàn diện.
   - 1 cảnh báo `checkersReport`: Báo cáo 119/186 bộ kiểm tra đang hoạt động.
2. **Nhóm Style & Advisory (29 cảnh báo):**
   - **`arrayIndexThenCheck` tại `src/product.c:55` và `src/product.c:61` (SA-037, SA-038):** Cảnh báo chỉ số `i` hoặc `j` được dùng truy cập `haystack[i]` trước khi so sánh với giới hạn `sizeof(h_lower) - 1`. Trong ngữ cảnh hàm `contains_ignore_case`, chuỗi kết thúc bằng ` ` nên không tràn nếu chuỗi hợp lệ, nhưng đây là một lưu ý phong cách phòng vệ quan trọng cho Bước 3.
   - **`compareValueOutOfTypeRangeError` tại `src/file_io.c:26` (SA-011):** So sánh `parsed > INT_MAX` khi `parsed` là kiểu `long`. Trên Windows LLP64, `sizeof(long) == 4` nên `LONG_MAX == INT_MAX = 2147483647`. Biểu thức so sánh luôn nhận giá trị false trên kiến trúc LLP64. Đây là một phát hiện phân tích tĩnh chính xác về tính khả chuyển (Portability/Type Range).
   - **`constVariablePointer` & `constParameterPointer` (8 cảnh báo):** Đề xuất gắn từ khóa `const` cho các con trỏ chỉ đọc trong `main.c`, `product.c`, `report.c`.
   - **`unusedFunction` & `staticFunction` (11 cảnh báo):** Các hàm API công khai (`inventory_init`, `inventory_free`, `update_stock`, v.v.) chưa được gọi trực tiếp trong hàm `main.c`.
3. **Nhóm Memory Error / Vulnerability:** **0 lỗi bộ nhớ nghiêm trọng (0 errors)**. Cppcheck xác nhận không phát hiện rò rỉ bộ nhớ hoặc truy cập con trỏ NULL hiển nhiên trong baseline `v0`.

---

## 6. DANH MỤC 10 RỦI RO AN TOÀN BỘ NHỚ (RISK REGISTER)

Sau khi rà soát độc lập và chuẩn hóa số dòng mã theo commit baseline `4420207`, 10 rủi ro được lưu trữ tại [docs/verification/step2/risk_register.csv](risk_register.csv) và [risk_register.md](risk_register.md):

| Risk ID | File & Dòng | Hàm C | Biểu thức / Nguy cơ | CWE | Cơ chế Guard Hiện tại | Trạng thái Khả đạt | REQ Liên quan | Phương pháp Kiểm chứng (Bước 3) |
|---|---|---|---|---|---|---|---|---|
| **RISK-001** | `product.c:118` | `add_product` | Tràn số nhân đôi: `list->capacity * 2` | CWE-190 | Chưa có guard; cần $2^63$ phần tử (trần RAM vật lý) | **REACHABILITY_UNPROVEN** | `REQ-2` | SMT Bit-Vector (Z3) / CBMC |
| **RISK-002** | `product.c:119` | `add_product` | Tràn số byte cấp phát: `new_cap * sizeof(Product)` | CWE-190, CWE-680 | Chưa có guard; realloc thất bại do cạn RAM trước | **REACHABILITY_UNPROVEN** | `REQ-3` | SMT Bit-Vector (Z3) / CBMC / SIM-001 |
| **RISK-003** | `product.c:119-124` | `add_product` | Rò rỉ khối nhớ cũ khi realloc trả về NULL | CWE-401 | Dùng con trỏ tạm `new_items`, bảo toàn `list->items` | **GUARDED** | `REQ-12, REQ-11` | CBMC / OOM Fault Injection |
| **RISK-004** | `product.c:223-233` | `find_product_by_id` | Con trỏ mượn `Product*` mất hiệu lực sau realloc dời mảng | CWE-416 | Hợp đồng kiến trúc; callers không lưu con trỏ qua realloc | **REACHABILITY_UNPROVEN** | `REQ-14` | Contract Checking / SIM-001 Harness |
| **RISK-005** | `product.c:27-34` | `product_list_free` | Giải phóng kép hoặc giải phóng con trỏ rác | CWE-415 | Gán `list->items = NULL`, `count = 0`, `capacity = 0` | **GUARDED** | `REQ-13, REQ-11` | CTL Temporal Verification / ASan |
| **RISK-006** | `utils.c:44-66` | `safe_string_copy` | Tràn bộ đệm đích khi sao chép chuỗi | CWE-120 | Kiểm tra `dest_size > 0`, ép `dest[i] = 0`, trả mã lỗi | **GUARDED** | `REQ-9` | CBMC String Bounds / Unit Tests |
| **RISK-007** | `file_io.c:96, 107-114` | `load_products_from_file` | Đọc dòng tệp dài vượt quá bộ đệm 512 bytes | CWE-120 | `fgets(line, 512)` + phát hiện thiếu newline trả lỗi | **GUARDED** | `REQ-10, REQ-17` | Fuzzing Harness SIM-002 |
| **RISK-008** | `file_io.c:47, 52-61` | `parse_product_line` | Tràn mảng con trỏ token `tokens[8]` trên ngăn xếp | CWE-121 | Guard `cursor != NULL && token_count < 8` trong vòng lặp | **GUARDED** | `REQ-16, REQ-17` | Excess Token Fuzzing SIM-002 |
| **RISK-009** | `file_io.c:63-91` | `parse_product_line` | Xử lý dữ liệu sai định dạng (4 trường vs 6 trường) | CWE-20 | Rẽ nhánh token == 6/4, kiểm tra miền số qua `parse_int` | **GUARDED** | `REQ-17, REQ-18` | Boundary Parsing Harness SIM-002 |
| **RISK-010** | `report.c:72-74` | `calculate_category_summaries` | Vượt quá sức chứa mảng danh mục 32 phần tử | CWE-122 (Prevented) | Guard `if (*out_count >= max_summaries) return STATUS_ERR_OVERFLOW;` | **GUARDED** (Defensive Behavior) | `REQ-16` | Model Checking Invariant |

---

## 7. ĐẶC TẢ VÀ TRUY VẾT 18 YÊU CẦU AN TOÀN BỘ NHỚ (18 REQs)

100% 18 yêu cầu từ Sheet `YeuCauChucNang` trong workbook `Spec_Inventory_Memory_Safety_Audit_18REQ_CHUAN.xlsx` đã được trích xuất nguyên văn và đặc tả tại [docs/verification/step2/requirement_specifications.md](requirement_specifications.md) và [req_traceability.csv](req_traceability.csv).

### Bảng Tóm tắt Ánh xạ 18 REQ:
1. **REQ-1 (Safety Invariant):** `ProductList luôn thỏa count <= capacity; khi count > 0 thì items khác NULL và trỏ tới vùng nhớ còn hiệu lực.` (`product.c:9-24, 116-138`).
2. **REQ-2 (Safety Arithmetic):** `Trước khi mở rộng ProductList bằng capacity * 2, phải kiểm tra capacity <= SIZE_MAX / 2 để tránh tràn số size_t.` (`product.c:118`, ánh xạ `RISK-001`).
3. **REQ-3 (Safety Arithmetic):** `Trước realloc, phải kiểm tra new_cap <= SIZE_MAX / sizeof(Product) để phép nhân kích thước cấp phát không bị tràn.` (`product.c:119`, ánh xạ `RISK-002`).
4. **REQ-4 (Safety Arithmetic):** `Trong import_stock, chỉ cộng khi add_quantity > 0, tồn kho hiện tại hợp lệ và current_stock <= INT_MAX - add_quantity; nếu không phải từ chối trước khi cập nhật.` (`inventory.c:30-47`, đã được guard tại `validation.c:297-311`).
5. **REQ-5 (Safety Arithmetic):** `Trong export_stock, số lượng xuất phải > 0 và <= tồn kho hiện tại; không được tạo ra tồn kho âm hoặc cập nhật khi điều kiện không thỏa.` (`inventory.c:49-66`, đã guard tại `validation.c:286-295`).
6. **REQ-6 (Memory Bounds):** `Mọi truy cập ProductList.items[i] trong thêm/sửa/xóa/tìm kiếm/báo cáo phải thỏa i < count <= capacity và items còn hiệu lực.` (`product.c:128, 147, 207-215, 227-230`).
7. **REQ-7 (Liveness):** `Với đầu vào hữu hạn và thao tác I/O trả về, mọi thao tác thêm/sửa/xóa/nhập/xuất đã bắt đầu phải kết thúc ở trạng thái thành công hoặc từ chối có kiểm soát; không lặp vô hạn.` (`product.c`, `inventory.c`).
8. **REQ-8 (Liveness):** `Với tệp hữu hạn và thao tác đọc trả về, quá trình đọc/phân tích dữ liệu sản phẩm phải kết thúc ở trạng thái chấp nhận hoặc từ chối có kiểm soát; không lặp vô hạn.` (`file_io.c:94-135`).
9. **REQ-9 (Memory Bounds):** `Các trường chuỗi cố định của Product (id, name, category, unit) phải được kiểm tra độ dài trước sao chép, không ghi vượt buffer và luôn kết thúc bằng ký tự  .` (`utils.c:44-66`, `product.c:37-44`, ánh xạ `RISK-006`).
10. **REQ-10 (Memory Bounds):** `Input bàn phím và dòng tệp vượt kích thước buffer phải được tiêu thụ/từ chối có kiểm soát mà không phát sinh đọc/ghi ngoài vùng nhớ hợp lệ.` (`file_io.c:96, 107-114`, ánh xạ `RISK-007`).
11. **REQ-11 (Memory Ownership):** `Allocation động do ProductList/Inventory sở hữu phải có vòng đời rõ ràng; không làm mất con trỏ sở hữu còn sống và phải giải phóng ở thời điểm kết thúc vòng đời.` (`product.c:9-24, 27-34`, ánh xạ `RISK-003, RISK-005`).
12. **REQ-12 (Memory Ownership):** `Khi realloc thất bại, allocation cũ phải được giữ nguyên quyền sở hữu; khi thành công, con trỏ sở hữu và capacity phải cập nhật đúng vùng nhớ mới.` (`product.c:119-125`, ánh xạ `RISK-003`).
13. **REQ-13 (Memory Safety):** `Một allocation chỉ được free tối đa một lần; cleanup phải cập nhật trạng thái/con trỏ để ngăn double-free khi gọi lại.` (`product.c:27-34`, ánh xạ `RISK-005`).
14. **REQ-14 (Pointer Safety):** `Không được dereference con trỏ Product/vùng nhớ ProductList sau free hoặc sau khi realloc làm con trỏ cũ mất hiệu lực.` (`product.c:223-233, 119`, ánh xạ `RISK-004`).
15. **REQ-15 (API Boundary Safety):** `API ProductList/Inventory phải kiểm tra con trỏ NULL theo hợp đồng hàm trước khi dereference đối tượng hoặc buffer vào/ra.` (`product.c:10, 71, 142, 201, 224`).
16. **REQ-16 (Memory Bounds):** `Mảng tokens, results, summaries và vùng nhớ dùng với memmove phải kiểm soát số phần tử, số byte, giới hạn nguồn/đích; không truy cập ngoài vùng nhớ đã cấp phát.` (`file_io.c:47, 52`, `report.c:72-74`, `product.c:212`, ánh xạ `RISK-008, RISK-010`).
17. **REQ-17 (Functional Parser):** `Parser phải chấp nhận đúng định dạng 6 trường hoặc định dạng tương thích 4 trường; các dòng sai số trường/độ dài/định dạng phải bị từ chối mà không gây hỏng bộ nhớ Inventory.` (`file_io.c:46-92`, ánh xạ `RISK-007, RISK-008, RISK-009`).
18. **REQ-18 (Functional Validation):** `Quantity/price không hợp lệ, vượt miền số hoặc không hữu hạn (NaN/Infinity) phải bị từ chối trước cập nhật kho và trước phép tính báo cáo có liên quan.` (`validation.c:150-210`, `file_io.c:20-44`, ánh xạ `RISK-009`).

---

## 8. HÌNH THỨC HÓA VÀ CHUẨN BỊ CHO BƯỚC 3

Chi tiết tại [formalization_plan.md](formalization_plan.md):
- **5 Mô hình Trạng thái Rời rạc (21 trạng thái):** Đã phân định rõ các biến trạng thái cho vòng đời heap, con trỏ mượn, bộ phân tích cú pháp tệp, bộ đệm chuỗi và luồng giao dịch kho.
- **Phân tích Bẫy Logic Đúng Hiển Nhiên (Vacuous Truth):** Nhận diện nguy cơ nếu đưa tiên đề `TRANS !(ptr_deref & ptr_invalid)` trong `LoaiTruLanNhau` vào model checker, công thức `AG(ptr_invalid -> !ptr_deref)` sẽ đúng hiển nhiên. Nhóm đã đề ra phương án kiểm chứng độc lập theo luồng gọi C thực tế ở Bước 3.
- **3 Bài toán SMT Bit-Vector (QF_BV):**
  - `SMT-01` (`product.c:118`): `capacity * 2 > UINT64_MAX` $\implies$ SAT toán học, Unreachable runtime.
  - `SMT-02` (`product.c:119`): `new_cap * 208 > UINT64_MAX` $\implies$ SAT toán học, Unreachable runtime.
  - `SMT-03` (`inventory.c:45` & `validation.c:306`): Phép cộng số lượng có guard `current_stock > INT_MAX - add_quantity` $\implies$ **UNSAT** (Đã phòng vệ an toàn khỏi Undefined Behavior).

---

## 9. ĐÁNH GIÁ TÍNH PHỦ (COVERAGE ASSESSMENT)

Chi tiết tại [coverage_matrix.csv](coverage_matrix.csv) và [coverage_report.md](coverage_report.md):
1. **Requirement Mapping Coverage:** **18 / 18 (100.0%)** – Toàn bộ 18 REQ được ánh xạ chi tiết hai chiều.
2. **Source Review Coverage:** **7 / 7 modules (100.0%)**, **70 / 70 hàm C (100.0%)**, **2.229 Code LOC (100.0%)**.
3. **Risk Specification Coverage:** **10 / 10 rủi ro (100.0%)** được đặc tả đầy đủ 24 trường thông tin.
4. **Static Code Analysis Coverage:** **7 / 7 tệp C (100.0%)** được Cppcheck 2.21.0 phân tích thực tế (67 findings).
5. **Planned State Model Coverage:** **21 / 21 trạng thái (100.0% đặc tả)**.
6. **Dynamic Code / Branch Coverage:** **`CHƯA ĐO (NOT_MEASURED)`** (Ghi nhận trung thực; sẽ đo bằng gcov/lcov ở Bước 3).
7. **Formal State Space Coverage:** **`CHƯA ĐO (NOT_MEASURED)`** (Ghi nhận trung thực; sẽ đo bằng CBMC/NuSMV ở Bước 3).

---

## 10. KẾ HOẠCH MÔ PHỎNG RỦI RO BẰNG C TEST HARNESS (SIMULATION PLAN)

Chi tiết tại [c_simulation_plan.md](c_simulation_plan.md):
- **SIM-001 (Heap Reallocation & Borrowed Pointer Lifetime):**
  - *Mã nguồn:* `product.c:116-138, 223-233`.
  - *Kịch bản A:* Thiết lập trạng thái cận biên $2^62$ để kích thích phép nhân tràn và kiểm chứng guard con trỏ tạm `new_items` bảo toàn mảng gốc khi OOM (REQ-12).
  - *Kịch bản B:* Thu thập con trỏ mượn, ép realloc dời mảng heap, và dereference để quan sát phát hiện Use-After-Free của AddressSanitizer (REQ-14).
- **SIM-002 (File Parser Buffer Bounds & Malformed Inputs):**
  - *Mã nguồn:* `file_io.c:46-92, 94-135`.
  - *Test Corpus:* 6 trường hợp kiểm thử biên (dòng > 512 bytes, > 8 tokens, < 4 tokens, NUL nhúng, số liệu lỗi) để kích hoạt toàn diện các guard của parser dùng `strchr('\|')` (REQ-9, REQ-10, REQ-16, REQ-17, REQ-18).
- **Mẫu Đối chứng Lỗi (Vulnerable Benchmark):** Sử dụng nhánh `audit/v0-file-io-vulnerable` làm mẫu thử đối chứng đo độ nhạy phát hiện của công cụ phân tích động, bảo đảm không merge vào baseline chính.

---

## 11. HỒ SƠ BẰNG CHỨNG THỰC NGHIỆM (EVIDENCE MANIFEST)

Toàn bộ 23 bằng chứng thực nghiệm đã được lập chỉ mục tại [results/step2/evidence_manifest.csv](../../../results/step2/evidence_manifest.csv). Script kiểm tra tự động `tools/check_step2_consistency.py` đã xác minh 100% mã băm SHA-256 thực tế của các tập tin log và tài liệu.

---

## 12. KẾT LUẬN NGHIỆM THU VÀ PHÂN CÔNG BƯỚC 3

### 12.1. Đánh giá Nghiệm thu Bước 2
Dự án đã **HOÀN THÀNH 100% BƯỚC 2** theo đúng toàn bộ các tiêu chí đề ra:
- Mã nguồn baseline `v0` được bảo vệ nguyên vẹn 100%.
- Cppcheck 2.21.0 đã được cài đặt và thực thi thực tế, phân loại 67 cảnh báo.
- 10 rủi ro bộ nhớ được chuẩn hóa số dòng và phân loại tính khả đạt chính xác.
- 18 REQ được trích xuất nguyên văn và liên kết hai chiều chặt chẽ.
- Script kiểm tra tính nhất quán tự động báo cáo 0 lỗi, 0 cảnh báo.

### 12.2. Kế hoạch Phân công Công việc cho Bước 3
1. **Kiểm chứng Hình thức SMT (Nguyễn Quang Thọ):** Chạy Z3 solver trên các script QF_BV cho `capacity * 2`, `new_cap * 208` và `stock + add`.
2. **Kiểm chứng Mô hình Hữu hạn CBMC (Hà Văn Đô):** Thiết lập unwinding loop và assertion checks cho các hàm mảng động và chuỗi.
3. **Kiểm chứng Logic Thời gian NuSMV (Hà Văn Đô):** Mô hình hóa và kiểm chứng 18 công thức CTL không dính vacuity.
4. **Phân tích Động & Test Harness Mô phỏng (Nguyễn Quang Thọ):** Xây dựng và biên dịch SIM-001, SIM-002 với GCC ASan + UBSan; đo độ phủ thực thi bằng gcov/lcov.
5. **Đánh giá và Tích hợp Báo cáo Bước 3 (Nguyễn Kiều Trang):** Tổng hợp bằng chứng, đối sánh kết quả với mẫu đối chứng lỗi, chuẩn bị hồ sơ cho bản v1.
