# BÁO CÁO TỔNG HỢP NGHIỆM THU BƯỚC 3: KIỂM CHỨNG HÌNH THỨC, MÔ PHỎNG RỦI RO VÀ ĐÁNH GIÁ ĐỘ PHỦ
## Đề tài: Kiểm chứng an toàn bộ nhớ cho chương trình C mô phỏng nghiệp vụ quản lý kho
## Học phần: An toàn phần mềm – Nhóm ATPM_Group_04
## Phiên bản: Đã hiệu chỉnh và kiểm toán độc lập (Independent Review Edition)

---

### THÀNH VIÊN THỰC HIỆN VÀ PHÂN CÔNG TRÁCH NHIỆM
1. **Nguyễn Kiều Trang** (Nhóm trưởng): Tích hợp hệ thống, rà soát ma trận 18 REQ chuẩn verbatim, tổng hợp bằng chứng nghiệm thu và điều phối phân tích.
2. **Hà Văn Đô**: Chuyên trách Kiểm chứng hình thức (Formal Verification), bài toán số học SMT Z3, CPROVER Contract Assertion Harness và Mô hình trạng thái / Kiểm chứng CTL Kripke Model.
3. **Nguyễn Quang Thọ**: Chuyên trách Thực nghiệm công cụ, Phân tích động (Dynamic Analysis & Canary Tracker), Xây dựng kịch bản mô phỏng rủi ro C (SIM-001, SIM-002, SIM-003 đối chứng) và Đo độ phủ mã nguồn (Gcov/Gcovr).

---

### 1. TỔNG QUAN DỰ ÁN VÀ BỐI CẢNH BƯỚC 3

- **Tên đề tài**: Kiểm chứng an toàn bộ nhớ cho chương trình C mô phỏng nghiệp vụ quản lý kho.
- **Repository chính thức**: `https://github.com/trangnguyenkieu927/ATPM_Group_04_inventory-memory-audit`
- **Nhánh làm việc Bước 3**: `audit/step3-formal-verification`
- **Mã SHA Commit xuất phát (Bước 2)**: `bcb07c50d505f3d568e7e9a13ad0531707e0483b`
- **Mã SHA Baseline v0 được bảo vệ**: `44202070c68002d9af068374478cdc83098dd4d8`
- **Mục tiêu nghiệm thu Bước 3**: Chuyển đổi toàn bộ đặc tả lý thuyết chuẩn (18 REQ, 10 Risk, 5 State Models từ `docs/verification/step2/requirement_specifications.md`) thành **bằng chứng kiểm chứng thực nghiệm có thể tái lập**:
  - Không sửa đổi bất kỳ logic nào trong `source/src/` và `source/include/`.
  - Thực thi kiểm chứng số học bit-vector bằng Z3 SMT Solver 5.1.0.
  - Bounded verification với CPROVER-compatible contract assertion harnesses trên mã C thực tế ($k=10 \dots 40$).
  - Phân tích động với Canary Memory Tracker và Stack Protector.
  - Xây dựng 3 bộ mô phỏng rủi ro C: SIM-001 (Heap lifecycle & OOM), SIM-002 (Parser buffer limits & 10 corpus), SIM-003 (Thực nghiệm đối chứng Negative Control với nhánh cài lỗi chủ đích `audit/v0-file-io-vulnerable`).
  - Kiểm chứng 18 công thức CTL và 21 trạng thái khả đạt trên mô hình trạng thái Kripke (`inventory_model.smv`).
  - Đo lường độ phủ dòng lệnh (Line coverage: 36.4%) và độ phủ nhánh (Branch coverage: 32.2%) bằng Gcov/Gcovr.
  - Lập Ma trận kiểm chứng 18 REQ đầy đủ 21 trường thông tin minh bạch, bảo tồn 100% nội dung nguyên văn từ Step 2.

---

### 2. MÔI TRƯỜNG THỰC HIỆN VÀ TÌNH TRẠNG CÔNG CỤ

Toàn bộ các phép đo và thực nghiệm được tiến hành trên môi trường chuẩn hóa, ghi nhận trung thực tại `results/step3/environment/`:

| Thành phần | Công cụ & Phiên bản | Tình trạng khả dụng | Ghi chú & Giới hạn môi trường |
| :--- | :--- | :--- | :--- |
| **Hệ điều hành** | Windows 11 Enterprise x86_64 | Native Host | ABI LLP64 (`sizeof(void*) = 8`, `sizeof(long) = 4`) |
| **C Compiler** | GCC 16.1.0 (Built by MSYS2) | Khả dụng | Chuẩn C99, Cờ `-O0`/`-O1`, `-g`, `-fstack-protector-strong` |
| **Build System** | GNU Make 4.4.1 | Khả dụng | Tự động hóa build test và benchmark |
| **SMT Solver** | Z3 Solver 5.1.0 - 64 bit | Khả dụng | Giải bit-vector QF_BV và logic số học bậc nhất |
| **CBMC Binary** | `cbmc` | **TOOL_UNAVAILABLE** | Không có binary native trên Windows; thực thi qua GCC Contract Harness ($k=10 \dots 40$) |
| **NuSMV Binary** | `NuSMV` / `nuXmv` | **TOOL_UNAVAILABLE** | Không có binary native; thực thi qua Custom Kripke Model Verifier (`verify_ctl.py`) |
| **AddressSanitizer** | `libasan` | **TOOL_UNAVAILABLE** | MSYS2 UCRT64 GCC thiếu thư viện runtime ASan cho Windows |
| **LeakSanitizer** | `liblsan` / `valgrind` | **TOOL_UNAVAILABLE** | Không hỗ trợ native trên Windows; thay bằng Canary Memory Tracker nội bộ |
| **Coverage Tool** | Gcov 16.1.0 & Gcovr 8.6 | Khả dụng | Đo lường Branch Coverage và Line Coverage trên 6 module C |

---

### 3. KẾT QUẢ KIỂM CHỨNG SỐ HỌC BẰNG SMT Z3 (PHASE 2)

Được giải bằng Z3 Solver 5.1.0 (`QF_BV` bit-vectors), lưu tại `results/step3/smt/`:

| SMT ID | REQ | Biểu thức C mô hình hóa | Kết quả Z3 | Nghiệm cụ thể (Counterexample Model) | Tính khả đạt trên Baseline v0 | Bằng chứng |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| **SMT-001A** | REQ-2 | `capacity * 2` (size_t 64-bit) | **SAT** | `capacity = 0x8000000000000000` ($2^{63}$), `new_cap = 0` | **KHÔNG KHẢ ĐẠT** (Vượt quá dung lượng RAM vật lý toàn cầu) | `smt_001a.log` |
| **SMT-001B** | REQ-2 | `capacity * 2` (RAM $\le$ 1TB) | **UNSAT** | Không tồn tại cấu hình tràn bit trong RAM khả thi | **AN TOÀN** trong điều kiện vận hành thực tế | `smt_001b.log` |
| **SMT-002A** | REQ-3 | `new_cap * sizeof(Product)` (208B) | **SAT** | `new_cap = #xeea85800eaaaaaaa` ($17,197,091,938,074,536,618 > 88,686,269,585,142,075$) | **KHÔNG KHẢ ĐẠT** tự nhiên từ kích thước khởi tạo 16 | `smt_002a.log` |
| **SMT-002B** | REQ-3 | `new_cap * 208` có Guard | **UNSAT** | Guard `new_cap <= SIZE_MAX / 208` ngăn ngừa tràn số bit-vector | **AN TOÀN NẾU CÓ GUARD** (Cần bổ sung guard ở Bước 4) | `smt_002b.log` |
| **SMT-003A** | REQ-4 | `p->quantity += add` (không guard) | **SAT** | `stock = 2,147,483,647 (INT_MAX)`, `add = 1` $\rightarrow$ âm | Rủi ro nếu bỏ qua hàm kiểm tra tràn | `smt_003a.log` |
| **SMT-003B** | REQ-4 | `p->quantity += add` (có guard) | **UNSAT** | Guard `check_addition_overflow()` trong `validation.c` chặn | **AN TOÀN TRÊN BASELINE v0** (Đã có sẵn guard) | `smt_003b.log` |
| **SMT-004A** | REQ-5 | `p->quantity -= export` | **SAT** | `export > stock` $\rightarrow$ tồn kho âm nếu không guard | Rủi ro nếu không kiểm tra lượng xuất kho | `smt_004a.log` |
| **SMT-004B** | REQ-5 | `p->quantity -= export` (có guard) | **UNSAT** | `inventory.c` kiểm tra `stock < quantity` $\rightarrow$ chặn | **AN TOÀN TRÊN BASELINE v0** | `smt_004b.log` |
| **SMT-005A** | REQ-16 | Đếm tokens bộ đệm stack | **SAT** | `token_count > 8` nếu không giới hạn vòng lặp | Rủi ro Stack Buffer Overflow | `smt_005a.log` |
| **SMT-005B** | REQ-16 | Đếm tokens có `token_count < 8` | **UNSAT** | Bảo vệ mảng `tokens[8]` không bị ghi vượt trong phạm vi kiểm chứng | **AN TOÀN TRÊN BASELINE v0** | `smt_005b.log` |

**Phân tích kỹ thuật SMT-002A**:
- Ngưỡng tối đa an toàn trước khi tràn 64-bit: $\lfloor (2^{64}-1)/208 \rfloor = \mathbf{88,686,269,585,142,075}$.
- Z3 tìm thấy mô hình nghiệm thỏa mãn: `new_cap = #xeea85800eaaaaaaa` ($17,197,091,938,074,536,618$).
- Phép nhân 128-bit: $17,197,091,938,074,536,618 \times 208 = 3,576,995,123,119,503,616,544 > 2^{64}-1$.
- Kết quả cắt gọt modulo $2^{64}$ (64-bit wraparound) chỉ còn: $16,773,516,893,560,154,656$ bytes, chứng minh nguy cơ cấp phát thiếu bộ đệm nếu giá trị này xuất hiện.

---

### 4. KẾT QUẢ KIỂM TRA BẤT BIẾN HỢP ĐỒNG BẰNG GCC HARNESS TESTING (PHASE 3)

Do công cụ CBMC native là `TOOL_UNAVAILABLE` trên môi trường Windows host, nhóm đã xây dựng 5 harness tuân thủ đặc tả CPROVER assertions và thực thi kiểm tra bounded contract assertions thông qua **GCC Harness Testing** với các giới hạn vòng lặp $k$ (không gọi là kiểm chứng CBMC native):

| Harness ID | REQ liên quan | Thuộc tính & Bất biến kiểm chứng | Bound $k$ | Kết quả GCC Harness | Counterexample | Ghi chú & Giới hạn |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| **CBMC-001** | REQ-1, REQ-2 | $0 \le \text{count} \le \text{capacity}$; `items != NULL` khi khởi tạo thành công | $k=20$ | **PASS** | 0 | Giới hạn trong bound $k=20$ thao tác |
| **CBMC-002** | REQ-6, REQ-16 | Ranh giới mảng `tokens[8]`, `summaries[32]`; không out-of-bounds | $k=40$ | **PASS** | 0 | Kiểm tra kích thước mảng tĩnh |
| **CBMC-003** | REQ-12 | Xử lý lỗi realloc OOM: con trỏ cũ được bảo toàn, không corrupt metadata | $k=17$ | **PASS** | 0 | Fault injection ép realloc trả NULL |
| **CBMC-004** | REQ-13, 14, 15 | An toàn con trỏ: kiểm tra NULL contract, chống double-free, con trỏ mượn | $k=17$ | **PASS** | 0 | Kiểm tra hợp đồng hàm công khai |
| **CBMC-005** | REQ-4, REQ-5 | An toàn số lượng tồn kho: không âm, không tràn signed integer khi cộng | $k=10$ | **PASS** | 0 | Kiểm tra guard số nguyên |

---

### 5. KẾT QUẢ PHÂN TÍCH ĐỘNG VÀ BỘ NHỚ (PHASE 4)

- **Tình trạng Sanitizer native**: `TOOL_UNAVAILABLE` trên MSYS2 UCRT64 GCC 16.1.0 (ghi nhận tại `results/step3/environment/sanitizer_probe.txt`).
- **Cơ chế đo kiểm nội bộ (Custom Canary Memory Tracker)**:
  - Gắn 8-byte Canary Guard (`0xDEADBEEFCAFEFACEULL`) ở cả hai đầu tiền tố và hậu tố của mọi khối heap.
  - Bảng kế toán Allocation Ledger theo dõi con trỏ và phát hiện rò rỉ khi thoát.
  - Bật cờ bảo vệ stack `-fstack-protector-strong`.
  - *Lưu ý*: Nhóm ghi nhận rõ Canary Memory Tracker là cơ chế wrapper kiểm tra nội bộ trong phạm vi test suite, **hoàn toàn không tương đương với AddressSanitizer (ASan), UndefinedBehaviorSanitizer (UBSan) hay LeakSanitizer (LSan)/Valgrind**; các công cụ chính thức này giữ nguyên trạng thái `TOOL_UNAVAILABLE` trên môi trường Windows host.
- **Kết quả thực nghiệm**:
  - `test_memory_audit.c`: 50 sản phẩm, tăng capacity 16 $\rightarrow$ 32 $\rightarrow$ 64, xóa 10 sản phẩm.
  - Số byte rò rỉ: **0 bytes** (`Active allocations: 0` trong phạm vi test suite).
  - Vi phạm Canary: **0 vụ** (Không phát hiện tràn bộ đệm heap tuyến tính).
  - Double-free: **0 lỗi**.
  - 17/17 Smoke Tests nghiệp vụ: **17/17 PASS**.

---

### 6. MÔ PHỎNG RỦI RO BỘ NHỚ BẰNG MÃ C (PHASE 5)

#### 6.1. SIM-001 – Vòng đời cấp phát Heap và Con trỏ
- **Thí nghiệm 1 (Mở rộng hợp lệ)**: Thêm 35 sản phẩm vượt capacity 16 lên 32 rồi 64. Bất biến `count <= capacity` được bảo toàn toàn vẹn.
- **Thí nghiệm 2 (Fault Injection ép OOM - REQ-12)**:
  - Ép `realloc()` trả về `NULL` khi danh sách đầy 16 phần tử.
  - **Mã lỗi hàm C trả về**: `STATUS_ERR_MEMORY = -8` (theo định nghĩa `source/include/models.h:41`).
  - **Mã kết thúc tiến trình test**: `0` (PASS, do harness assert đúng mã `-8` và xác nhận `list.items` cùng 16 phần tử cũ được bảo toàn).
- **Thí nghiệm 3 (Borrowed Pointer Contract - REQ-14)**:
  - Khi `realloc()` di chuyển vùng nhớ, con trỏ mượn cũ mất hiệu lực.
  - Harness xác nhận quy trình re-borrowing qua `find_product_by_id()` đảm bảo truy cập hợp lệ.

#### 6.2. SIM-002 – File Parser và Giới hạn Bộ đệm (10 Corpus)
Thực thi kiểm thử biên trên 10 tệp corpus độc lập (`tests/simulation/corpus/`):
- 10/10 corpus xử lý an toàn theo hợp đồng đặc tả: chấp nhận dòng 6 trường và 4 trường cũ; từ chối an toàn các dòng sai số trường, dòng dài 600B, byte NUL nhúng, giá trị tràn `INT_MAX`, số lượng âm, giá NaN. Không phát sinh tràn buffer.

#### 6.3. SIM-003 – Thí nghiệm Đối chứng Negative Control với Nhánh Lỗi Cài Chủ Đích
- **Mã nguồn đối chứng**: Trích xuất từ nhánh `audit/v0-file-io-vulnerable` (Commit `d7cbe365714f9799e49c8aa0fd889d606a9878cd`).
  - `parse_product_line_vulnerable` khai báo `char *tokens[6]` nhưng không kiểm tra `token_count < 6`.
- **Đầu vào thử nghiệm**: Dòng chứa 10 trường phân cách bởi `|`.
  - **Trên Baseline v0**: Guard `token_count < 8` trong `file_io.c` chặn cứng việc ghi vượt mảng stack. Hàm từ chối an toàn với mã `STATUS_ERR_FILE_IO = -7`, tiến trình kết thúc bình thường (Exit code 0).
  - **Trên Nhánh Seeded**: Quá trình ghi 10 tokens làm tràn mảng `tokens[6]`, ghi đè stack canary. GCC `-fstack-protector-strong` ngắt tiến trình:
    ```text
    *** stack smashing detected ***: terminated
    Exit Code: 0xC0000409 (STATUS_STACK_BUFFER_OVERRUN)
    ```
- **Kết luận khoa học**: Phép thử đối chứng chứng minh cơ chế stack protector có độ nhạy phát hiện lỗi ghi đè mảng stack trong kịch bản này. Tuy nhiên, một phép thử đơn lẻ không đại diện cho độ nhạy tuyệt đối với mọi dạng lỗi bộ nhớ.

---

### 7. KẾT QUẢ KIỂM CHỨNG TEMPORAL LOGIC CTL BẰNG CUSTOM CTL MODEL CHECKER (PHASE 6)

Được thực thi qua Custom CTL Model Checker (`verify_ctl.py`) dựa trên đặc tả SMV (`inventory_model.smv`) do NuSMV/nuXmv native là `TOOL_UNAVAILABLE` (đây là chương trình Python tự kiểm tra mô hình CTL, không gọi là kết quả NuSMV chính thức):
- **Độ phủ trạng thái (State Reachability Coverage)**: **21/21 trạng thái KHẢ ĐẠT (100.0%)**.
- **Kết quả 18 công thức CTL**: **18/18 công thức đạt giá trị TRUE**.
- **Kiểm tra bẫy chân lý rỗng (Vacuous Truth Analysis)**:
  - CTL-03 (`ptr_invalid -> !ptr_deref`): Tiền đề `ptr_invalid` khả đạt tại 2 trạng thái $\rightarrow$ **NON-VACUOUS**.
  - CTL-04 (`realloc_failed -> owner_valid`): Tiền đề `realloc_failed` khả đạt tại 1 trạng thái $\rightarrow$ **NON-VACUOUS**.
  - CTL-05 (`realloc_done -> owner_valid`): Tiền đề `realloc_done` khả đạt tại 1 trạng thái $\rightarrow$ **NON-VACUOUS**.
  - CTL-07 (`product_op_processing -> AF(...)`): Tiền đề khả đạt $\rightarrow$ **NON-VACUOUS** (Tính sống nghiệp vụ kho).
  - CTL-08 (`file_parsing -> AF(...)`): Tiền đề khả đạt $\rightarrow$ **NON-VACUOUS** (Tính sống xử lý file).
- **Giới hạn trừu tượng hóa mô hình**:
  - Mô hình kiểm chứng tính đúng đắn và không deadlock của **giao thức trạng thái kiến trúc** (architectural protocol).
  - Kết quả 18/18 CTL TRUE trên mô hình **không đồng nghĩa** rằng mã nguồn C thực tế đã được chứng minh an toàn tuyệt đối ở mọi tầng thực thi.

---

### 8. ĐÁNH GIÁ ĐỘ PHỦ MÃ NGUỒN (COVERAGE ASSESSMENT)

Đo bằng `gcov` 16.1.0 và `gcovr` 8.6 trên bản build `--coverage -O0 -g`:

```text
------------------------------------------------------------------------------
                           GCC Code Coverage Report
Directory: .
------------------------------------------------------------------------------
File                                       Lines     Exec  Cover   Branches Exec  Cover
------------------------------------------------------------------------------
source/src/file_io.c                         100       87    87%         46   32    69%
source/src/inventory.c                        43       33    76%         24   13    54%
source/src/product.c                         135       66    48%         86   24    27%
source/src/report.c                          204       24    11%        190   12     6%
source/src/utils.c                           123       15    12%         94   18    19%
source/src/validation.c                      165       55    33%        175   99    56%
------------------------------------------------------------------------------
TỔNG CỘNG MÃ NGUỒN C (Core Source Files)    770      280  36.4%        615  198  32.2%
------------------------------------------------------------------------------
```

- **Tỷ lệ thực tế**: Line coverage **36.4%**, Branch coverage **32.2%**.
- **Phân tích các nhánh chưa được phủ**:
  1. Nhánh `realloc == NULL` trong `product.c`: Chưa được phủ trong bản build gcov thông thường (chỉ được kích hoạt trong mô phỏng fault injection SIM-001 độc lập).
  2. Nhánh lỗi `fopen == NULL` khi đọc/ghi file trong `file_io.c`.
  3. Toàn bộ định dạng bảng và in ấn console trong `report.c` (đạt 11% dòng do chưa có test bao quát toàn bộ các menu hiển thị).
  4. Các hàm tiện ích phụ trợ trong `utils.c` (`to_upper_case`, `find_file_path`, etc.).

---

### 9. MA TRẬN KIỂM CHỨNG 18 REQ (ĐÃ HIỆU CHỈNH CHUẨN VERBATIM)

Chi tiết đầy đủ với 21 cột tiêu chuẩn tại `docs/verification/step3/verification_matrix.md` và `docs/verification/step3/verification_matrix.csv`:

| REQ ID | Nội dung Đặc tả Nguyên văn (Step 2 Spec) | Phương pháp & Công cụ | Trạng thái Nghiệm thu |
| :--- | :--- | :--- | :--- |
| **REQ-1** | ProductList luôn thỏa count <= capacity; khi count > 0 thì items khác NULL và trỏ tới vùng nhớ còn hiệu lực. | GCC Harness Testing (Bounded Contract Assertions) ($k=20$) | **VERIFIED_WITHIN_HARNESS_SCOPE** |
| **REQ-2** | Trước khi mở rộng ProductList bằng capacity * 2, phải kiểm tra capacity <= SIZE_MAX / 2 để tránh tràn số size_t. | SMT Bit-Vector Arithmetic Solver (Z3) | **PARTIALLY_VERIFIED** (SMT phân tích rủi ro; baseline v0 chưa có guard tường minh; chuyển việc vá sang Bước 4) |
| **REQ-3** | Trước realloc, phải kiểm tra new_cap <= SIZE_MAX / sizeof(Product) để phép nhân kích thước cấp phát không bị tràn. | SMT Bit-Vector Arithmetic Solver (Z3) | **PARTIALLY_VERIFIED** (SMT phân tích rủi ro; baseline v0 chưa có guard tường minh; chuyển việc vá sang Bước 4) |
| **REQ-4** | Trong import_stock, chỉ cộng khi add_quantity > 0, tồn kho hiện tại hợp lệ và current_stock <= INT_MAX - add_quantity; nếu không phải từ chối trước khi cập nhật. | SMT Bit-Vector & GCC Harness Testing | **VERIFIED_WITHIN_SCOPE** (Đã có sẵn guard) |
| **REQ-5** | Trong export_stock, số lượng xuất phải > 0 và <= tồn kho hiện tại; không được tạo ra tồn kho âm hoặc cập nhật khi điều kiện không thỏa. | GCC Harness Testing & SMT Z3 | **VERIFIED_WITHIN_SCOPE** |
| **REQ-6** | Mọi truy cập ProductList.items[i] trong thêm/sửa/xóa/tìm kiếm/báo cáo phải thỏa i < count <= capacity và items còn hiệu lực. | GCC Harness Testing (Bounded Contract Assertions) ($k=40$) | **VERIFIED_WITHIN_HARNESS_SCOPE** |
| **REQ-7** | Với đầu vào hữu hạn và thao tác I/O trả về, mọi thao tác thêm/sửa/xóa/nhập/xuất đã bắt đầu phải kết thúc ở trạng thái thành công hoặc từ chối có kiểm soát; không lặp vô hạn. | Custom CTL Model Checker (`verify_ctl.py`) | **VERIFIED_WITHIN_MODEL_SCOPE** |
| **REQ-8** | Với tệp hữu hạn và thao tác đọc trả về, quá trình đọc/phân tích dữ liệu sản phẩm phải kết thúc ở trạng thái chấp nhận hoặc từ chối có kiểm soát; không lặp vô hạn. | Custom CTL Model Checker & C Simulation SIM-002 | **VERIFIED_WITHIN_SCOPE** |
| **REQ-9** | Các trường chuỗi cố định của Product (id, name, category, unit) phải được kiểm tra độ dài trước sao chép, không ghi vượt buffer và luôn kết thúc bằng ký tự \0. | Unit Test Suite & Canary Tracker | **VERIFIED_WITHIN_SCOPE** |
| **REQ-10**| Input bàn phím và dòng tệp vượt kích thước buffer phải được tiêu thụ/từ chối có kiểm soát mà không phát sinh đọc/ghi ngoài vùng nhớ hợp lệ. | C Boundary Simulation SIM-002 (Corpus 05) | **VERIFIED_WITHIN_SCOPE** |
| **REQ-11**| Allocation động do ProductList/Inventory sở hữu phải có vòng đời rõ ràng; không làm mất con trỏ sở hữu còn sống và phải giải phóng ở thời điểm kết thúc vòng đời. | Custom Canary Memory Tracker (Allocation Ledger) | **VERIFIED_WITHIN_TEST_SCOPE** |
| **REQ-12**| Khi realloc thất bại, allocation cũ phải được giữ nguyên quyền sở hữu; khi thành công, con trỏ sở hữu và capacity phải cập nhật đúng vùng nhớ mới. | Mock Allocator Fault Injection (SIM-001.2) & GCC Harness Testing | **VERIFIED_WITHIN_SCOPE** (Mã lỗi C: -8) |
| **REQ-13**| Một allocation chỉ được free tối đa một lần; cleanup phải cập nhật trạng thái/con trỏ để ngăn double-free khi gọi lại. | GCC Harness Testing & Canary Tracker | **VERIFIED_WITHIN_SCOPE** |
| **REQ-14**| Không được dereference con trỏ Product/vùng nhớ ProductList sau free hoặc sau khi realloc làm con trỏ cũ mất hiệu lực. | Custom CTL Model Checker & C Simulation SIM-001.3 | **VERIFIED_WITHIN_SCOPE** |
| **REQ-15**| API ProductList/Inventory phải kiểm tra con trỏ NULL theo hợp đồng hàm trước khi dereference đối tượng hoặc buffer vào/ra. | GCC Harness Testing (Bounded Contract Assertions) ($k=17$) | **VERIFIED_WITHIN_SCOPE** |
| **REQ-16**| Mảng tokens, results, summaries và vùng nhớ dùng với memmove phải kiểm soát số phần tử, số byte, giới hạn nguồn/đích; không truy cập ngoài vùng nhớ đã cấp phát. | Negative Control SIM-003 & GCC Harness Testing | **VERIFIED_WITHIN_SCOPE** |
| **REQ-17**| Parser phải chấp nhận đúng định dạng 6 trường hoặc định dạng tương thích 4 trường; các dòng sai số trường/độ dài/định dạng phải bị từ chối mà không gây hỏng bộ nhớ Inventory. | C Simulation Harness SIM-002 (10 Corpus) | **VERIFIED_WITHIN_SCOPE** |
| **REQ-18**| Quantity/price không hợp lệ, vượt miền số hoặc không hữu hạn (NaN/Infinity) phải bị từ chối trước cập nhật kho và trước phép tính báo cáo có liên quan. | SIM-002 & Unit Test Suite | **VERIFIED_WITHIN_SCOPE** |

---

### 10. DANH MỤC PHÁT HIỆN VÀ PHÂN LOẠI KHOA HỌC

1. **Lỗ hổng đã xác nhận trên Baseline v0**: **0 lỗ hổng có thể khai thác trực tiếp**.
2. **Rủi ro tiềm ẩn ở mức số học lý thuyết (SMT SAT) và trạng thái Guard trong Baseline v0**:
   - **REQ-2**: `capacity * 2` có nguy cơ tràn số size_t toán học khi `capacity > SIZE_MAX / 2` (SMT-001A SAT). Đối chiếu thực tế `source/src/product.c`, baseline v0 **chưa có guard tường minh** này. Đánh dấu: **`PARTIALLY_VERIFIED`**.
   - **REQ-3**: `new_cap * sizeof(Product)` có nguy cơ tràn số 64-bit toán học khi `new_cap > SIZE_MAX / 208` (SMT-002A SAT). Đối chiếu thực tế `source/src/product.c`, baseline v0 **chưa có guard tường minh** này. Đánh dấu: **`PARTIALLY_VERIFIED`**.
   - *Phân biệt khoa học*: Kết quả SMT chứng minh tính khả thi của mô hình tràn số khi đầu vào tự do; trong điều kiện vận hành với RAM thực tế $\le 1$TB (SMT-001B UNSAT) và khởi tạo từ 16 phần tử, các giá trị này không khả đạt tự nhiên. Tuy nhiên, theo nguyên tắc phòng vệ theo chiều sâu, việc bổ sung guard tường minh là bắt buộc và được chuyển sang Bước 4.
3. **Cơ chế phòng vệ đã kiểm chứng an toàn trên Baseline v0**:
   - Guard `check_addition_overflow()` bảo vệ phép cộng tồn kho (REQ-4).
   - Guard `token_count < 8` bảo vệ bộ đệm đọc tệp trên stack frame (REQ-16).
   - Cơ chế bảo toàn con trỏ cũ khi `realloc` thất bại trả về `-8` (REQ-12).
4. **Lỗi trong nhánh cài chủ đích được phát hiện**:
   - Lỗi tràn mảng `tokens[6]` trên nhánh `audit/v0-file-io-vulnerable` làm hỏng Stack Canary và bị ngắt tiến trình với mã `0xC0000409`.
5. **Giới hạn công cụ**:
   - `cbmc`, `NuSMV`, `libasan`, `libubsan`, `valgrind` là `TOOL_UNAVAILABLE` trên môi trường Windows MSYS2 UCRT64.

---

### 11. CÁC KIỂM CHỨNG CÒN THIẾU TRƯỚC KHI HOÀN THIỆN NGHIÊN CỨU

Nhằm bảo đảm tính trung thực khoa học và định hướng hoàn thiện cho giai đoạn sau, nhóm kiểm chứng tổng hợp danh mục các bài kiểm chứng và kiểm nghiệm còn thiếu:

1. **Kiểm chứng mô hình phần mềm chính thức bằng CBMC native (`cbmc`)**:
   - *Hiện trạng Bước 3*: Mới chỉ thực thi 5 harness dưới dạng GCC Bounded Contract Assertions testing ($k=10 \dots 40$).
   - *Khoảng trống*: Chưa thực hiện symbolic execution trên AST C đầy đủ với SAT/SMT backend của công cụ CBMC native để chứng minh tính thỏa mãn bất biến toán học độc lập với dữ liệu test cụ thể.
2. **Kiểm chứng mô hình thời gian CTL bằng NuSMV / nuXmv native chính thức**:
   - *Hiện trạng Bước 3*: Mới chỉ kiểm tra 18 công thức CTL bằng Custom CTL Model Checker nội bộ (`verify_ctl.py`).
   - *Khoảng trống*: Chưa nạp tệp mô hình `inventory_model.smv` vào binary NuSMV / nuXmv native để tận dụng các thuật toán BDD (Binary Decision Diagrams) và Bounded Model Checking chuẩn công nghiệp.
3. **Phân tích động chính thức với Runtime Sanitizers và Valgrind**:
   - *Hiện trạng Bước 3*: Mới chỉ sử dụng Canary Memory Tracker nội bộ (8-byte canary guard & allocation ledger) và GCC `-fstack-protector-strong`.
   - *Khoảng trống*:
     - **AddressSanitizer (ASan)**: Chưa kiểm tra tràn bộ đệm heap động ngoài phạm vi canary guard, use-after-free phức tạp ở cấp allocator hệ điều hành.
     - **UndefinedBehaviorSanitizer (UBSan)**: Chưa phát hiện hành vi không xác định động (misaligned pointer access, bitshift ngoài miền, signed integer overflow trong runtime).
     - **LeakSanitizer (LSan) / Valgrind Memcheck**: Chưa rà soát rò rỉ bộ nhớ toàn diện trên toàn bộ tiến trình hệ điều hành.
4. **Các nhánh mã nguồn C chưa được kiểm thử bao phủ (Uncovered Branches)**:
   - Theo kết quả Gcov thực tế (Line coverage: 36.4%, Branch coverage: 32.2%), các nhánh sau còn thiếu kịch bản kiểm thử:
     - Nhánh `realloc == NULL` trong `source/src/product.c`: Mới chỉ được kiểm tra trong harness fault injection độc lập (`test_sim001_heap.c`), chưa được phủ trong bản build gcov thông thường.
     - Nhánh lỗi I/O trong `source/src/file_io.c`: Các nhánh `fopen() == NULL` khi đọc file không tồn tại hoặc ghi file vào đường dẫn không có quyền ghi.
     - Toàn bộ các nhánh định dạng bảng và in ấn console trong `source/src/report.c` (độ phủ dòng 11%, nhánh 6%).
     - Các hàm tiện ích xâu trong `source/src/utils.c` (độ phủ dòng 12%, nhánh 19%: `to_upper_case`, `to_lower_case`, `trim_whitespace`, `find_file_path`).
5. **Bổ sung các Guard phòng vệ chưa có trong baseline (chuyển sang Bước 4)**:
   - REQ-2: Bổ sung guard `if (list->capacity > SIZE_MAX / 2) return STATUS_ERR_MEMORY;` trước phép nhân đôi capacity trong `source/src/product.c`.
   - REQ-3: Bổ sung guard `if (new_cap > SIZE_MAX / sizeof(Product)) return STATUS_ERR_MEMORY;` trước `realloc` trong `source/src/product.c`.

---

### 12. KẾT LUẬN VÀ KẾ HOẠCH BÀN GIAO BƯỚC 4

- Toàn bộ 18 REQ đã được đối chiếu chuẩn xác với tài liệu nguồn Bước 2.
- Hồ sơ kiểm chứng đã được rà soát độc lập, loại bỏ các kết luận phóng đại hoặc nhầm lẫn số học/mã lỗi.
- **Lưu ý về thực thi pipeline**: Lệnh `python tools/run_step3_verification.py --all` thực thi thành công nghĩa là pipeline đã chạy trọn vẹn các bài kiểm tra khả dụng trên host hiện tại; điều này **không chứng minh** rằng tất cả các công cụ formal native (như CBMC, NuSMV, ASan, UBSan, Valgrind) đều đã hoạt động.
- Đề xuất nhiệm vụ trọng tâm Bước 4 (Hardening & Complete Verification):
  1. Thêm guard chống tràn số cho `capacity` và `new_cap` trong `source/src/product.c` (REQ-2, REQ-3).
  2. Bổ sung tài liệu hợp đồng con trỏ mượn trong `source/include/product.h` (REQ-14).
  3. Mở rộng test coverage cho các nhánh in ấn báo cáo trong `report.c` và các hàm tiện ích trong `utils.c`.
  4. Chuẩn bị môi trường Linux (Ubuntu) để thực thi các công cụ native (CBMC, NuSMV, ASan/UBSan, Valgrind) nhằm hoàn thiện nghiên cứu.
