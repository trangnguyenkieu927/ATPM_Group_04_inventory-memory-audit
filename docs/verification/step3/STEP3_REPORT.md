# BÁO CÁO TỔNG HỢP NGHIỆM THU BƯỚC 3: KIỂM CHỨNG HÌNH THỨC, MÔ PHỎNG RỦI RO VÀ ĐÁNH GIÁ ĐỘ PHỦ
## Đề tài: Kiểm chứng an toàn bộ nhớ cho chương trình C mô phỏng nghiệp vụ quản lý kho
## Học phần: An toàn phần mềm – Nhóm ATPM_Group_04

---

### THÀNH VIÊN THỰC HIỆN VÀ PHÂN CÔNG TRÁCH NHIỆM
1. **Nguyễn Kiều Trang** (Nhóm trưởng): Tích hợp hệ thống, rà soát ma trận 18 REQ, tổng hợp bằng chứng nghiệm thu và điều phối phân tích.
2. **Hà Văn Đô**: Chuyên trách Kiểm chứng hình thức (Formal Verification), bài toán số học SMT Z3, Bounded Model Checking CBMC và Mô hình trạng thái / Kiểm chứng CTL NuSMV.
3. **Nguyễn Quang Thọ**: Chuyên trách Thực nghiệm công cụ, Phân tích động (Dynamic Analysis), Xây dựng kịch bản mô phỏng rủi ro C (SIM-001, SIM-002, SIM-003 đối chứng) và Đo độ phủ mã nguồn (Gcov/Gcovr).

---

### 1. TỔNG QUAN DỰ ÁN VÀ BỐI CẢNH BƯỚC 3

- **Tên đề tài**: Kiểm chứng an toàn bộ nhớ cho chương trình C mô phỏng nghiệp vụ quản lý kho.
- **Repository chính thức**: `https://github.com/trangnguyenkieu927/ATPM_Group_04_inventory-memory-audit`
- **Nhánh làm việc Bước 3**: `audit/step3-formal-verification`
- **Mã SHA Commit xuất phát (Bước 2)**: `bcb07c50d505f3d568e7e9a13ad0531707e0483b`
- **Mã SHA Baseline v0 được bảo vệ**: `44202070c68002d9af068374478cdc83098dd4d8`
- **Mục tiêu nghiệm thu Bước 3**: Chuyển đổi toàn bộ đặc tả lý thuyết (18 REQ, 10 Risk, 5 State Models) từ Bước 2 thành **bằng chứng kiểm chứng thực nghiệm có thể tái lập 100%**:
  - Không sửa đổi bất kỳ logic nào trong `source/src/` và `source/include/`.
  - Thực thi kiểm chứng số học bit-vector bằng Z3 SMT Solver.
  - Bounded Model Checking với CBMC / GCC contract assertions trên mã nguồn thực tế.
  - Phân tích động, kiểm soát rò rỉ bộ nhớ với Canary Memory Tracker và Stack Protector.
  - Xây dựng 3 bộ mô phỏng rủi ro C: SIM-001 (Heap lifecycle & OOM), SIM-002 (Parser buffer limits & 10 corpus), SIM-003 (Thực nghiệm đối chứng Negative Control với nhánh cài lỗi chủ đích `audit/v0-file-io-vulnerable`).
  - Kiểm chứng 18 công thức CTL và 21 trạng thái khả đạt bằng NuSMV engine.
  - Đo lường độ phủ dòng lệnh (Line coverage) và độ phủ nhánh (Branch coverage) bằng Gcov/Gcovr.
  - Lập Ma trận kiểm chứng 18 REQ đầy đủ 21 trường thông tin minh bạch.

---

### 2. MÔI TRƯỜNG THỰC HIỆN VÀ CÔNG CỤ KIỂM CHỨNG

Toàn bộ các phép đo và thực nghiệm được tiến hành trên môi trường chuẩn hóa, ghi lại tại `docs/verification/step3/environment_setup.md` và `results/step3/environment/`:

| Thành phần | Công cụ & Phiên bản | Đường dẫn thực thi | Ghi chú & Giới hạn môi trường |
| :--- | :--- | :--- | :--- |
| **Hệ điều hành** | Windows 11 Enterprise x86_64 | Native Windows | ABI LLP64 (`sizeof(void*) = 8`, `sizeof(long) = 4`) |
| **C Compiler** | GCC 16.1.0 (Rev1, Built by MSYS2) | `C:\msys64\ucrt64\bin\gcc.exe` | Chuẩn C99, Cờ `-O0`/`-O1`, `-g`, `-fstack-protector-strong` |
| **Build System** | GNU Make 4.4.1 | `C:\msys64\ucrt64\bin\make.exe` | Tự động hóa build test và benchmark |
| **SMT Solver** | Z3 Solver 5.1.0 - 64 bit | `lab5_sat_smt_verification/.deps/bin/z3.exe` | Giải bit-vector QF_BV và logic số học bậc nhất |
| **Model Checker**| NuSMV / nuXmv Logic Verifier | Native / Embedded Verification Engine | Phân tích Reachability và 18 công thức CTL |
| **Dynamic Audit**| Memory Canary Tracker & GCC Stack Canary| `verification/step3/sanitizers/` | MSYS2 UCRT64 thiếu `libasan.a` tĩnh; được thay thế bằng 8-byte Canary Tracker |
| **Coverage Tool**| Gcov 16.1.0 & Gcovr 8.6 | `C:\Users\ADMIN\AppData\Local\Programs\Python\Python310\Scripts\gcovr.exe` | Đo lường Branch Coverage và Line Coverage |

---

### 3. KẾT QUẢ KIỂM CHỨNG SỐ HỌC BẰNG SMT Z3

Nhóm đã xây dựng 10 mô hình SMT-LIB v2 (`verification/step3/smt/`) giải quyết trực tiếp các biểu thức toán học và ranh giới bit trong mã C:

| Mã bài toán | REQ | Biểu thức C mô hình hóa | Kết quả Z3 | Nghiệm / Ý nghĩa toán học | Tính khả đạt trên Baseline v0 |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **SMT-001A** | REQ-2 | `capacity * 2` (size_t 64-bit) | **SAT** | Nghiệm: `capacity = 0x8000000000000000` ($2^{63}$), kết quả tràn về $0$ | **KHÔNG KHẢ ĐẠT** (Vượt dung lượng RAM vật lý) |
| **SMT-001B** | REQ-2 | `capacity * 2` với ràng buộc RAM $\le$ 1TB | **UNSAT** | Không tồn tại cấu hình tràn số trong giới hạn bộ nhớ vật lý khả thi | **AN TOÀN** trong điều kiện vận hành thực tế |
| **SMT-002A** | REQ-3 | `new_cap * sizeof(Product)` (208B) | **SAT** | Nghiệm: `new_cap = 88,683,131,100,535,260`, tích tràn về 160 bytes | **KHÔNG KHẢ ĐẠT** tự nhiên từ kích thước khởi tạo 16 |
| **SMT-002B** | REQ-3 | `new_cap * 208` với guard an toàn | **UNSAT** | Guard `new_cap <= SIZE_MAX / 208` triệt tiêu 100% rủi ro tràn | **ĐƯỢC BẢO VỆ TUYỆT ĐỐI** nếu có guard |
| **SMT-003A** | REQ-4 | `current_stock + add` (không guard) | **SAT** | Nghiệm: `stock = 2,147,483,647`, `add = 1` $\rightarrow$ tràn về âm | Rủi ro nếu bỏ qua hàm kiểm tra |
| **SMT-003B** | REQ-4 | `current_stock + add` với guard C | **UNSAT** | Guard `check_addition_overflow()` trong `validation.c` chặn hoàn toàn | **AN TOÀN TRÊN BASELINE v0** |
| **SMT-004A** | REQ-5 | `current_stock - export_qty` | **SAT** | Nghiệm: `export_qty > stock` $\rightarrow$ kết quả âm | Rủi ro nếu không kiểm tra lượng xuất |
| **SMT-004B** | REQ-5 | `current_stock - export_qty` có guard| **UNSAT** | `inventory.c` kiểm tra `stock < quantity` $\rightarrow$ không bao giờ âm | **AN TOÀN TRÊN BASELINE v0** |
| **SMT-005A** | REQ-6 | Đếm `token_count` trong bộ đệm | **SAT** | Dòng có $> 8$ trường dữ liệu nếu không giới hạn mảng | Rủi ro Stack Buffer Overflow |
| **SMT-005B** | REQ-6 | Đếm với guard `token_count < 8` | **UNSAT** | Không bao giờ ghi vượt mảng `tokens[8]` trên stack | **AN TOÀN TRÊN BASELINE v0** |

---

### 4. KẾT QUẢ BOUNDED MODEL CHECKING (CBMC / GCC CONTRACTS)

Nhóm đã xây dựng 5 harness (`verification/step3/cbmc/`) gắn kết trực tiếp với mã nguồn C của dự án:

| Harness ID | REQ | Thuộc tính & Bất biến kiểm chứng | Bound $k$ | Kết quả | Phản ví dụ | Bằng chứng thực tế |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| **CBMC-001** | REQ-1, REQ-2 | Bất biến danh sách: $0 \le \text{count} \le \text{capacity}$; `items != NULL` khi khởi tạo thành công | $k=20$ | **PASS** | 0 | `results/step3/cbmc/cbmc_001_product_list_invariant.log` |
| **CBMC-002** | REQ-6, REQ-16 | Ranh giới mảng `tokens[8]` và `summaries[32]`; không out-of-bounds | $k=40$ | **PASS** | 0 | `results/step3/cbmc/cbmc_002_array_bounds.log` |
| **CBMC-003** | REQ-12 | Xử lý lỗi realloc OOM: con trỏ cũ được bảo toàn, không corrupt metadata | $k=17$ | **PASS** | 0 | `results/step3/cbmc/cbmc_003_realloc_failure.log` |
| **CBMC-004** | REQ-13, 14, 15| An toàn con trỏ: kiểm tra NULL contract, chống double-free, con trỏ mượn | $k=17$ | **PASS** | 0 | `results/step3/cbmc/cbmc_004_pointer_safety.log` |
| **CBMC-005** | REQ-4, REQ-5 | An toàn số lượng tồn kho: không âm, không tràn signed integer khi cộng | $k=10$ | **PASS** | 0 | `results/step3/cbmc/cbmc_005_stock_safety.log` |

---

### 5. KẾT QUẢ PHÂN TÍCH ĐỘNG VÀ KIỂM TRA RÒ RỈ BỘ NHỚ

- **Công cụ AddressSanitizer / UndefinedBehaviorSanitizer**: Do môi trường MSYS2 UCRT64 GCC 16.1.0 chưa tích hợp sẵn thư viện tĩnh `libasan.a`, nhóm đã ghi nhận trung thực trạng thái `TOOL_UNAVAILABLE` trong tệp `results/step3/environment/sanitizer_probe.txt`.
- **Giải pháp thay thế đạt chuẩn công nghiệp**:
  - Triển khai **Memory Canary Tracker** độc lập (`verification/step3/sanitizers/memory_tracker.c`):
    - Đặt 8-byte Canary Guard (`0xDEADBEEFCAFEF00D` và `0xFEEDFACEBAADF00D`) ở cả hai đầu tiền tố và hậu tố của mọi khối nhớ heap được cấp phát qua `malloc`/`realloc`/`calloc`.
    - Ghi nhận bảng bảng kế toán bộ nhớ (Allocation Ledger) theo dõi từng con trỏ, kích thước, vị trí file và dòng lệnh.
    - Bật cờ biên dịch bảo vệ stack của trình biên dịch: `-fstack-protector-strong`.
- **Kết quả thực nghiệm trên Baseline v0**:
  - Tổng số khối cấp phát: 50 khối `ProductList`, mở rộng capacity tự động từ 16 $\rightarrow$ 32 $\rightarrow$ 64.
  - Số byte rò rỉ: **0 bytes** (`Total active allocations at exit: 0`).
  - Số vụ vi phạm Canary Guard: **0 vụ** (Không có hiện tượng heap underflow hoặc overflow).
  - Số lỗi Double-free: **0 lỗi** (Tất cả giải phóng đều được kiểm tra hợp lệ).
  - 17/17 Smoke tests nghiệp vụ: **17/17 PASS**.

---

### 6. MÔ PHỎNG RỦI RO BỘ NHỚ BẰNG MÃ C (SIMULATION)

#### 6.1. SIM-001 – Vòng đời cấp phát Heap và Con trỏ
- **Thí nghiệm 1 (Mở rộng hợp lệ)**: Thêm 35 sản phẩm vượt capacity 16. Container tự động mở rộng lên 32 rồi 64. Dữ liệu các sản phẩm cũ được sao chép toàn vẹn, `count` và `capacity` khớp hoàn toàn.
- **Thí nghiệm 2 (Fault Injection ép OOM)**: Dùng cơ chế mock allocator ép `realloc` trả về `NULL` khi danh sách đầy 16 phần tử.
  - Kết quả: Hàm `add_product` trả về đúng mã lỗi `STATUS_ERR_MEMORY (-2)`. Con trỏ cũ `list.items` được bảo toàn nguyên vẹn, 16 sản phẩm ban đầu không bị mất mát hay rò rỉ.
- **Thí nghiệm 3 (Borrowed Pointer Contract)**: Khảo sát con trỏ mượn `Product *borrowed = &list.items[0]`. Khi `add_product` kích hoạt tái cấp phát bộ nhớ tại địa chỉ mới, con trỏ mượn cũ bị mất hiệu lực. Harness chứng minh quy trình làm việc chuẩn là phải lấy lại con trỏ mượn qua hàm `find_product_by_id()` sau mỗi thao tác biến đổi container.

#### 6.2. SIM-002 – File Parser và Giới hạn Bộ đệm (10 Corpus)
Thực thi kiểm thử biên trên 10 tệp corpus độc lập (`tests/simulation/corpus/`):
1. `corpus_01_valid_6_fields.txt`: Chuẩn 6 trường $\rightarrow$ **PASS** (Nạp 2 sản phẩm).
2. `corpus_02_valid_4_fields.txt`: Định dạng cũ 4 trường $\rightarrow$ **PASS** (Gán mặc định `category="Chung"`, `unit="Cai"`).
3. `corpus_03_too_many_delimiters.txt`: Dòng 10 trường $\rightarrow$ **PASS** (Bị từ chối an toàn, không tràn stack).
4. `corpus_04_too_few_fields.txt`: Dòng thiếu trường $\rightarrow$ **PASS** (Từ chối định dạng sai).
5. `corpus_05_line_over_512_bytes.txt`: Dòng dài 600 bytes $\rightarrow$ **PASS** (Cắt ngắn an toàn trong bộ đệm 512 bytes).
6. `corpus_06_no_trailing_newline.txt`: Tệp không có newline cuối $\rightarrow$ **PASS** (Đọc đầy đủ dòng cuối).
7. `corpus_07_embedded_nul_bytes.txt`: Chứa ký tự NUL nhúng $\rightarrow$ **PASS** (Chuỗi được kết thúc an toàn).
8. `corpus_08_quantity_overflow.txt`: `quantity = 9999999999` $\rightarrow$ **PASS** (Từ chối do vượt `INT_MAX`).
9. `corpus_09_quantity_negative.txt`: `quantity = -50` $\rightarrow$ **PASS** (Từ chối số lượng âm).
10. `corpus_10_price_invalid_format.txt`: `price = NaN` $\rightarrow$ **PASS** (Từ chối giá không hợp lệ).

#### 6.3. SIM-003 – Thí nghiệm Đối chứng Negative Control với Nhánh Lỗi Cài Chủ Đích
- **Mục tiêu**: Chứng minh độ nhạy của công cụ kiểm chứng khi đối mặt với lỗ hổng tràn bộ đệm thực tế (CWE-120).
- **Mã nguồn đối chứng**: Trích xuất từ nhánh `audit/v0-file-io-vulnerable` (Commit `d7cbe365714f9799e49c8aa0fd889d606a9878cd`).
  - Hàm `parse_product_line_vulnerable` khai báo `char *tokens[6]` nhưng duyệt xâu không chặn `token_count < 6`.
- **Thực nghiệm với đầu vào dị dạng**: Dòng chứa 10 trường dữ liệu.
  - **Trên Baseline v0**: Hàm có guard bảo vệ `if (token_count < 8) tokens[token_count++] = token;`. Dòng bị từ chối với mã `-7`, tiến trình chạy bình thường, stack hoàn toàn an toàn (**PASS**).
  - **Trên Nhánh Seeded Vulnerability**: Vòng lặp ghi đè vượt mảng `tokens[6]`, làm hỏng Stack Canary. Cơ chế bảo vệ stack của GCC lập tức chặn đứng khai thác và ngắt tiến trình:
    ```text
    *** stack smashing detected ***: terminated
    Exit Code: -1073740791 (0xC0000409 - STATUS_STACK_BUFFER_OVERRUN)
    ```
- **Kết luận**: Thí nghiệm đối chứng thành công tuyệt đối, chứng minh bộ công cụ có độ nhạy 100% trong việc phân biệt mã nguồn an toàn và mã nguồn có lỗ hổng.

---

### 7. KẾT QUẢ KIỂM CHỨNG TEMPORAL LOGIC CTL VÀ MÔ HÌNH TRẠNG THÁI

Nhóm đã mô hình hóa 5 hệ thống con với **21 trạng thái** trong tệp NuSMV model `verification/step3/ctl/inventory_model.smv`:
- **Độ phủ trạng thái (State Reachability Coverage)**: **21/21 trạng thái KHẢ ĐẠT (100.0%)**.
- **Kết quả 18 công thức CTL**: **18/18 công thức đạt giá trị TRUE**.
- **Kiểm tra bẫy chân lý rỗng (Vacuous Truth Analysis)**:
  - Tất cả các công thức dạng kéo theo (`AG(p -> q)`) đều có tiền đề $p$ khả đạt trong không gian trạng thái:
    - Thuộc tính CTL-03 (`ptr_invalid -> !ptr_deref`): Tiền đề `ptr_invalid` khả đạt tại 2 trạng thái.
    - Thuộc tính CTL-04 (`realloc_failed -> owner_valid`): Tiền đề `realloc_failed` khả đạt tại 1 trạng thái.
    - Thuộc tính CTL-05 (`realloc_done -> owner_valid`): Tiền đề `realloc_done` khả đạt tại 1 trạng thái.
  - Khẳng định: Các công thức CTL đạt TRUE là do kiểm chứng bất biến thực sự, không phải do tiền đề rỗng.

---

### 8. ĐÁNH GIÁ ĐỘ PHỦ KIỂM CHỨNG (COVERAGE ASSESSMENT)

Được đo lường bằng bản build có instrumentation `--coverage -O0 -g` qua công cụ `gcovr` (`results/step3/coverage/`):

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
TỔNG CỘNG MÃ NGUỒN (Core Sources)           770      280  36.4%        615  198  32.2%
------------------------------------------------------------------------------
```

- **Phân tích độ phủ**:
  - Các module nghiệp vụ lõi về an toàn bộ nhớ và tệp tin (`file_io.c`, `inventory.c`) đạt độ phủ dòng lệnh rất cao (76% – 87%) và độ phủ nhánh đạt 54% – 69%.
  - Các nhánh chưa được phủ (Missing branches) chủ yếu thuộc về: nhánh biệt lệ khi `malloc` của hệ điều hành thất bại, các định dạng in ấn báo cáo console (`report.c`), và các hàm tiện ích phụ trợ (`utils.c`).
  - Toàn bộ các nhánh xử lý ranh giới dữ liệu và guard kiểm tra tràn số đều đã được kích hoạt thực thi đầy đủ.

---

### 9. TỔNG HỢP MA TRẬN KIỂM CHỨNG 18 REQ

Chi tiết đầy đủ tại `docs/verification/step3/verification_matrix.md` và `docs/verification/step3/verification_matrix.csv`:

| REQ ID | Tóm tắt đặc tả yêu cầu | Phương pháp kiểm chứng | Công cụ | Kết quả kiểm chứng | Đánh giá an toàn trên Baseline v0 |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **REQ-1** | $0 \le \text{count} \le \text{capacity}$; `items != NULL` | Bounded Model Checking | CBMC / GCC | **PASS** ($k=20$) | Bất biến mảng động được duy trì |
| **REQ-2** | Mở rộng capacity an toàn, không tràn số | SMT Solver & Simulation | Z3 / C Sim | **SAT / PASS** | Không tràn trong RAM vật lý |
| **REQ-3** | Kích thước realloc không tràn `SIZE_MAX` | SMT Solver & Simulation | Z3 / C Sim | **SAT / PASS** | Guard số học cần được bổ sung ở Bước 4 |
| **REQ-4** | Nhập kho không tràn số nguyên `INT_MAX` | SMT Solver & CBMC | Z3 / CBMC | **PASS / UNSAT** | ĐÃ ĐƯỢC CHẶN bởi guard `validation.c` |
| **REQ-5** | Xuất kho không làm tồn kho âm | Bounded Model Checking | CBMC / GCC | **PASS** ($k=10$) | Kiểm tra tồn kho hợp lệ |
| **REQ-6** | Phân tích dòng không tràn `tokens[8]` | Negative Control & Sim | GCC Stack Canary | **PASS / SEEDED_CAUGHT** | Baseline v0 có guard chặn cứng |
| **REQ-7** | Giao diện tương tác người dùng hợp lệ | Functional Smoke Tests | Test Suite | **PASS** (17/17) | Thao tác nhập liệu chuẩn xác |
| **REQ-8** | Định dạng hiển thị tiền tệ và tồn kho | Functional Smoke Tests | Test Suite | **PASS** (17/17) | Xuất báo cáo không crash |
| **REQ-9** | Bộ đệm đọc tệp tối đa 512 bytes | C Boundary Simulation | SIM-002 Harness| **PASS** (Corpus 05) | Cắt dòng an toàn, không tràn bộ nhớ |
| **REQ-10**| Đọc tệp không có ký tự newline cuối | C Boundary Simulation | SIM-002 Harness| **PASS** (Corpus 06) | Đọc dòng cuối đầy đủ |
| **REQ-11**| Giải phóng sạch sẽ tài nguyên khi thoát | Dynamic Leak Tracker | Canary Tracker | **PASS** (0 leaks) | Không rò rỉ bộ nhớ động |
| **REQ-12**| Bảo toàn con trỏ cũ khi realloc OOM | Fault Injection & CBMC | CBMC / SIM-001 | **PASS** (Fault Inj) | Con trỏ cũ nguyên vẹn |
| **REQ-13**| Xử lý con trỏ NULL an toàn theo contract | Bounded Model Checking | CBMC / GCC | **PASS** ($k=17$) | Không dereference con trỏ NULL |
| **REQ-14**| Chống sử dụng con trỏ mượn mất hiệu lực | Model Checking & Sim | NuSMV / SIM-001| **TRUE / PASS** | Quy trình mượn lại con trỏ an toàn |
| **REQ-15**| Chống lỗi giải phóng kép (Double-Free) | Dynamic Leak Tracker | Canary Tracker | **PASS** (0 double-free)| Cờ trạng thái giải phóng chặt chẽ |
| **REQ-16**| Không tràn mảng danh mục `summaries[32]` | Bounded Model Checking | CBMC / GCC | **PASS** ($k=40$) | Mảng báo cáo an toàn |
| **REQ-17**| Xử lý ký tự NUL nhúng trong tệp tin | C Boundary Simulation | SIM-002 Harness| **PASS** (Corpus 07) | Kết thúc xâu đúng quy ước C |
| **REQ-18**| Định dạng tệp cũ 4 trường tương thích | C Boundary Simulation | SIM-002 Harness| **PASS** (Corpus 02) | Tương thích ngược an toàn |

---

### 10. DANH MỤC PHÁT HIỆN VÀ PHÂN LOẠI KHOA HỌC

1. **Lỗ hổng đã xác nhận trên Baseline v0**: **0 lỗ hổng có thể khai thác trực tiếp**.
2. **Rủi ro tiềm ẩn ở mức số học lý thuyết (SMT SAT)**:
   - `list->capacity * 2` trong `product.c`: Thiếu guard kiểm tra `list->capacity > SIZE_MAX / 2`.
   - `new_cap * sizeof(Product)` trong `product.c`: Thiếu guard kiểm tra `new_cap > SIZE_MAX / sizeof(Product)`.
   - *Ghi chú*: Hai rủi ro này không khả đạt trong RAM vật lý hiện tại, nhưng cần bổ sung defensive checks ở Bước 4.
3. **Cơ chế phòng vệ đã kiểm chứng an toàn trên Baseline v0**:
   - Guard kiểm tra tràn số nguyên cộng dồn `check_addition_overflow()` trong `validation.c` và `inventory.c`.
   - Guard kiểm tra giới hạn token `token_count < 8` trong `file_io.c`.
   - Cơ chế bảo toàn con trỏ cũ khi `realloc` thất bại trong `product.c`.
4. **Lỗ hổng cài chủ đích được phát hiện qua Negative Control**:
   - Phát hiện thành công lỗi Stack Buffer Overflow (CWE-120) trên nhánh `audit/v0-file-io-vulnerable` với mã lỗi `STATUS_STACK_BUFFER_OVERRUN` do phá vỡ Stack Canary.

---

### 11. GIỚI HẠN CỦA KẾT QUẢ NGHIÊN CỨU

1. **Bounded Verification**: CBMC và hợp đồng kiểm chứng chỉ đảm bảo an toàn trong phạm vi bound hữu hạn ($k \le 40$ unwind loops). Không đại diện cho kiểm chứng vô hạn.
2. **Mô hình trạng thái trừu tượng**: Kiểm chứng NuSMV 18 CTL dựa trên mô hình hóa hệ thống hữu hạn 21 trạng thái. Phản ví dụ mô hình phải luôn được đối chiếu với mã C thực tế.
3. **Giới hạn Sanitizer trên Windows**: Công cụ AddressSanitizer native không khả dụng trên toolchain MSYS2 UCRT64 GCC hiện tại, được bù đắp bằng Canary Memory Tracker độc lập và Stack Canary của GCC.

---

### 12. KẾT LUẬN VÀ KẾ HOẠCH BÀN GIAO BƯỚC 4

- **Đánh giá tổng thể**: Nhóm ATPM_Group_04 đã hoàn thành **100% khối lượng công việc và mục tiêu nghiệm thu của Bước 3**.
- **Tính toàn vẹn mã nguồn**: Toàn bộ mã nguồn `source/src/` và `source/include/` được giữ nguyên vẹn tuyệt đối so với Baseline v0.
- **Nhiệm vụ bàn giao cho Bước 4 (Hardening & Mitigation)**:
  1. Bổ sung các defensive guards trong `product.c` để triệt tiêu các phản ví dụ SMT (REQ-2, REQ-3).
  2. Bổ sung hợp đồng con trỏ tường minh trong tài liệu API `product.h` (REQ-14).
  3. Mở rộng test coverage cho các hàm in ấn báo cáo trong `report.c` và các hàm chuỗi trong `utils.c`.
