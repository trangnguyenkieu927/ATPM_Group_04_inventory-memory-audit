# BÁO CÁO RÀ SOÁT VÀ KIỂM TOÁN ĐỘC LẬP BƯỚC 3 (STEP 3 INDEPENDENT REVIEW)
## Dự án: Kiểm chứng an toàn bộ nhớ cho chương trình C mô phỏng nghiệp vụ quản lý kho
## Học phần: An toàn phần mềm – Nhóm ATPM_Group_04
## Nhánh: `audit/step3-formal-verification`

---

### 1. TỔNG QUAN VỀ ĐỢT KIỂM TOÁN ĐỘC LẬP

Thực hiện theo chỉ đạo kiểm toán nghiêm ngặt trước khi thực hiện bàn giao hoặc đẩy mã nguồn (push) lên GitHub, nhóm kiểm chứng đã tiến hành một đợt rà soát độc lập toàn diện trên nhánh `audit/step3-formal-verification` nhằm:
1. Xác minh tính chính xác của các bài toán toán học và mô hình SMT Solver.
2. Đối chiếu 18 REQ với đặc tả chuẩn gốc trong Step 2 (`docs/verification/step2/requirement_specifications.md`).
3. Xác minh tính nhất quán giữa log thực tế của các harness với diễn giải trong báo cáo.
4. Đánh giá tính khả dụng thực tế của hệ thống công cụ (CBMC, NuSMV, Sanitizers).
5. Đánh giá độ phủ mã nguồn thực tế và rà soát các nhánh chưa được phủ.
6. Tuân thủ nguyên tắc **Bảo vệ mã nguồn Baseline v0** và **Trung thực khoa học**.

---

### 2. NHỮNG KẾT QUẢ ĐÃ XÁC MINH ĐƯỢC (VERIFIED RESULTS)

1. **Kiểm chứng số học SMT Z3**:
   - Bài toán SMT-001A (Tràn số khi nhân đôi capacity 64-bit): Z3 5.1.0 tìm thấy mô hình nghiệm thỏa mãn `capacity = 0x8000000000000000` ($2^{63}$) làm phép nhân $2 \times capacity$ quay vòng về $0$ (**SAT**).
   - Bài toán SMT-001B (Khả đạt trong RAM vật lý $\le 1$TB): **UNSAT** (Chứng minh bất khả đạt trong điều kiện vận hành thực tế).
   - Bài toán SMT-003B (Nhập kho có guard `check_addition_overflow()`): **UNSAT** (Chứng minh toán học guard loại bỏ hoàn toàn nguy cơ tràn số nguyên có dấu).
   - Bài toán SMT-004B (Xuất kho có guard tồn kho): **UNSAT** (Chứng minh tồn kho không bị âm khi thỏa guard).
   - Bài toán SMT-005B (Giới hạn token `token_count < 8`): **UNSAT** (Chứng minh loại bỏ nguy cơ ghi vượt mảng `tokens[8]` khi có guard `token_count < 8`).

2. **GCC Harness Testing (Bounded Contract Assertions)**:
   - 5 harness kiểm tra bất biến bộ nhớ (`harness_cbmc_001` đến `005`) liên kết trực tiếp với mã nguồn C gốc, thực thi với các bound $k=10 \dots 40$ bằng trình biên dịch GCC đều đạt kết quả **PASS**:
     - `count <= capacity` được bảo toàn sau mọi thao tác thêm/xóa.
     - Vòng lặp duyệt mảng không vượt quá `count`.
     - Gọi `product_list_free()` liên tiếp không gây double-free (Idempotent cleanup).
     - Toàn bộ 5 lời gọi API với con trỏ `NULL` đều bị từ chối với mã `STATUS_ERR_NULL_PTR = -1`.

3. **Mô phỏng rủi ro C (SIM-001, SIM-002, SIM-003)**:
   - **SIM-001.1**: Tăng trưởng mảng động tự động mở rộng 16 $\rightarrow$ 32 $\rightarrow$ 64 hợp lệ, dữ liệu cũ giữ nguyên.
   - **SIM-001.2**: Fault injection ép allocator trả về `NULL`, hàm `add_product()` trả về đúng mã lỗi `STATUS_ERR_MEMORY = -8`, con trỏ cũ và 16 phần tử được bảo toàn nguyên vẹn.
   - **SIM-001.3**: Kiểm chứng quy tắc hợp đồng con trỏ mượn (Borrowed Pointer Contract), chứng minh tính cần thiết của việc mượn lại con trỏ sau khi container thay đổi kích thước.
   - **SIM-002**: 10/10 corpus tệp tin (chuẩn 6 trường, cũ 4 trường, dài 600B, thiếu newline, ký tự NUL, overflow `INT_MAX`, số lượng âm, giá NaN) đều được xử lý an toàn theo hợp đồng đặc tả.
   - **SIM-003 (Negative Control)**: So sánh đối chứng tệp 10 trường dữ liệu:
     - Baseline v0: Guard `token_count < 8` chặn cứng, từ chối an toàn với mã `-7`.
     - Nhánh cài lỗi chủ đích (`audit/v0-file-io-vulnerable`): Tràn mảng stack `tokens[6]`, phá vỡ Stack Canary và bị GCC runtime ngắt tiến trình với lỗi `*** stack smashing detected ***` (`0xC0000409`).

4. **Kiểm chứng mô hình trạng thái CTL**:
   - 21/21 trạng thái trong mô hình Kripke `inventory_model.smv` đều **KHẢ ĐẠT** (100.0% State Reachability).
   - 18/18 công thức CTL đạt giá trị **TRUE**.
   - Kiểm tra Vacuous Truth xác nhận các công thức kéo theo (`AG(p -> q)`) đều có tiền đề $p$ xuất hiện thực sự trong không gian trạng thái.

5. **Đo độ phủ mã nguồn thực tế**:
   - Đo bằng bản build `--coverage -O0 -g` với `gcov`/`gcovr`:
     - Line coverage: **36.4%** (280 / 770 dòng).
     - Branch coverage: **32.2%** (198 / 615 nhánh).
     - Module lõi: `file_io.c` đạt 87% dòng / 69.6% nhánh; `inventory.c` đạt 76.7% dòng / 54.2% nhánh.

---

### 3. NHỮNG SAI LỆCH ĐÃ ĐƯỢC PHÁT HIỆN VÀ HIỆU CHỈNH (CORRECTIONS)

Qua rà soát kỹ thuật độc lập, nhóm đã phát hiện và khắc phục 4 điểm sai lệch trong hồ sơ trước:

#### 3.1. Sai lệch số liệu và diễn giải toán học trong bài toán SMT-002A
- **Sai lệch cũ**: Trong một số đoạn diễn giải narrative, văn bản ghi rằng nghiệm `new_cap = 88683131100535260` làm tràn phép nhân `new_cap * 208`.
  - Trên thực tế: $\lfloor (2^{64}-1)/208 \rfloor = \mathbf{88,686,269,585,142,075}$.
  - Con số $88,683,131,100,535,260 < 88,686,269,585,142,075$, do đó phép nhân này **chưa vượt quá $2^{64}-1$**!
- **Dữ liệu thực tế từ Z3 Solver**:
  - File SMT-LIB `smt_002a_realloc_size.smt2` đã assert `alloc_bytes_128 > SIZE_MAX`.
  - Z3 Solver 5.1.0 thực tế trả về mô hình nghiệm:
    $$\text{new\_cap} = \text{\#xeea85800eaaaaaaa} = \mathbf{17,197,091,938,074,536,618}$$
  - Phép nhân 128-bit:
    $$17,197,091,938,074,536,618 \times 208 = 3,576,995,123,119,503,616,544 > 2^{64} - 1$$
  - Giá trị cắt gọt 64-bit (wraparound):
    $$3,576,995,123,119,503,616,544 \pmod{2^{64}} = 16,773,516,893,560,154,656 \text{ bytes}$$
- **Hiệu chỉnh**: Toàn bộ tài liệu `counterexample_analysis.md`, `STEP3_REPORT.md` và `verification_matrix.md` đã được cập nhật lại theo đúng số liệu thực tế của Z3.

#### 3.2. Khôi phục 100% nội dung nguyên văn (Verbatim) của 18 REQ
- **Sai lệch cũ**: Ma trận `verification_matrix.csv` trước đây đã diễn đạt lại (paraphrase) một số yêu cầu, dẫn tới việc REQ-6 bị nhầm sang parser token bounds, REQ-7 và REQ-8 bị gán sai phạm vi, REQ-13 và REQ-15 bị đảo lộn.
- **Hiệu chỉnh**: Đã đối chiếu và khôi phục 100% nội dung nguyên văn từ `docs/verification/step2/requirement_specifications.md` và `docs/verification/step2/req_traceability.csv`:
  - **REQ-6**: Khôi phục về: "Mọi truy cập ProductList.items[i] trong thêm/sửa/xóa/tìm kiếm/báo cáo phải thỏa i < count <= capacity và items còn hiệu lực."
  - **REQ-7**: Khôi phục về: "Với đầu vào hữu hạn và thao tác I/O trả về, mọi thao tác thêm/sửa/xóa/nhập/xuất đã bắt đầu phải kết thúc ở trạng thái thành công hoặc từ chối có kiểm soát; không lặp vô hạn." (Ánh xạ chuẩn tới CTL-07).
  - **REQ-8**: Khôi phục về: "Với tệp hữu hạn và thao tác đọc trả về, quá trình đọc/phân tích dữ liệu sản phẩm phải kết thúc ở trạng thái chấp nhận hoặc từ chối có kiểm soát; không lặp vô hạn." (Ánh xạ chuẩn tới CTL-08 và SIM-002).
  - **REQ-9**: Khôi phục về an toàn chuỗi cố định của Product.
  - **REQ-13**: Khôi phục về quy tắc chống Double-Free.
  - **REQ-15**: Khôi phục về quy tắc kiểm tra con trỏ NULL theo hợp đồng API.
  - **REQ-16**: Khôi phục về ranh giới mảng tokens, results, summaries và memmove.

#### 3.3. Sửa nhầm lẫn mã lỗi trong thí nghiệm SIM-001.2
- **Sai lệch cũ**: Trong một đoạn báo cáo, văn bản ghi hàm `add_product` khi OOM trả về `-2`.
- **Thực tế**: Theo định nghĩa chuẩn trong `source/include/models.h:41`, `STATUS_ERR_MEMORY` có giá trị là **`-8`** (mã `-2` là `STATUS_ERR_INVALID_ID`). Log thực tế `sim001_heap_lifecycle.log:16` ghi nhận hàm trả về `-8`.
- **Hiệu chỉnh**: Đã sửa lại chính xác: Mã lỗi hàm C trả về là **`-8`** (`STATUS_ERR_MEMORY`); mã kết thúc của tiến trình test (Process exit code) là **`0`** (PASS).

#### 3.4. Chuẩn hóa nhận định về độ nhạy của công cụ từ thí nghiệm SIM-003
- **Sai lệch cũ**: Từng có câu diễn đạt rằng "công cụ có độ nhạy tuyệt đối 100% đối với mọi lỗi an toàn bộ nhớ".
- **Hiệu chỉnh**: Nhóm đã sửa đổi thành kết luận khách quan: "Thí nghiệm SIM-003 chứng minh cơ chế `-fstack-protector-strong` có khả năng phát hiện lỗi tràn mảng stack buffer (CWE-120) trong kịch bản đối chứng này; nhưng không xem đây là bằng chứng cho thấy công cụ có thể phát hiện 100% mọi dạng lỗi bộ nhớ khác".

---

### 4. DANH SÁCH CÔNG CỤ CHƯA KHẢ DỤNG VÀ BIỆN PHÁP THAY THẾ (TOOL STATUS)

Nhóm ghi nhận trung thực hiện trạng công cụ trên môi trường Windows 11 Enterprise / MSYS2 UCRT64:

| Công cụ | Tình trạng | Phương pháp thay thế thực tế đã sử dụng | Giới hạn so với công cụ chuẩn |
| :--- | :--- | :--- | :--- |
| **CBMC Native Binary** | `TOOL_UNAVAILABLE` | GCC Harness Testing (Bounded Contract Assertions) | Kiểm tra bất biến trong bound hữu hạn ($k=10 \dots 40$), không phải symbolic execution vô hạn; không gọi là kiểm chứng CBMC chính thức. |
| **NuSMV / nuXmv Binary** | `TOOL_UNAVAILABLE` | Custom CTL Model Checker (`verify_ctl.py`) | Đánh giá 18 CTL trên mô hình 21 trạng thái trừu tượng; là script Python nội bộ, không gọi là kết quả NuSMV chính thức. |
| **AddressSanitizer (ASan)** | `TOOL_UNAVAILABLE` | Canary Memory Tracker & GCC `-fstack-protector-strong` | Bắt được tràn mảng stack và heap boundary tuyến tính; không bắt được use-after-free ngoài canary. Hoàn toàn không tương đương ASan. |
| **UndefinedBehaviorSanitizer** | `TOOL_UNAVAILABLE` | SMT Bit-Vector Integer Overflow Guards | Chứng minh toán học guard loại bỏ UB; không phát hiện runtime UB động. Hoàn toàn không tương đương UBSan. |
| **LeakSanitizer / Valgrind** | `TOOL_UNAVAILABLE` | Canary Memory Allocation Ledger nội bộ | Đo lượng cấp phát/giải phóng trong test suite; không can thiệp sâu vào allocator hệ thống. Hoàn toàn không tương đương LSan/Valgrind. |

---

### 5. MA TRẬN 18 REQ SAU KHI HIỆU CHỈNH

Toàn bộ 18 REQ đã được phân loại trạng thái nghiệm thu minh bạch và đối chiếu với mã nguồn C:
- **`VERIFIED_WITHIN_SCOPE`**: 12 REQ (REQ-4, REQ-5, REQ-8, REQ-9, REQ-10, REQ-12, REQ-13, REQ-14, REQ-15, REQ-16, REQ-17, REQ-18).
- **`VERIFIED_WITHIN_HARNESS_SCOPE`**: 2 REQ (REQ-1, REQ-6 – Kiểm chứng đạt trong phạm vi bound hữu hạn của GCC Harness Testing; CBMC native là `TOOL_UNAVAILABLE`).
- **`VERIFIED_WITHIN_MODEL_SCOPE`**: 1 REQ (REQ-7 – Kiểm chứng đạt trên mô hình trạng thái giao thức qua Custom CTL Model Checker; NuSMV native là `TOOL_UNAVAILABLE`).
- **`VERIFIED_WITHIN_TEST_SCOPE`**: 1 REQ (REQ-11 – Đạt 0 bytes leak trên kịch bản kiểm thử của Canary Tracker; LSan/Valgrind native là `TOOL_UNAVAILABLE`).
- **`PARTIALLY_VERIFIED`**: 2 REQ:
  - **REQ-2**: Đã phân tích rủi ro số học toán học bằng SMT Z3 (SMT-001A SAT, SMT-001B UNSAT với RAM $\le 1$TB); đối chiếu mã nguồn `source/src/product.c` cho thấy baseline v0 **chưa có guard tường minh** `capacity <= SIZE_MAX / 2`. Yêu cầu chưa được đáp ứng đầy đủ trong baseline; chuyển việc bổ sung guard sang Bước 4.
  - **REQ-3**: Đã phân tích rủi ro số học toán học bằng SMT Z3 (SMT-002A SAT khi thiếu guard, SMT-002B UNSAT khi có guard); đối chiếu mã nguồn `source/src/product.c` cho thấy baseline v0 **chưa có guard tường minh** `new_cap <= SIZE_MAX / sizeof(Product)`. Yêu cầu chưa được đáp ứng đầy đủ trong baseline; chuyển việc bổ sung guard sang Bước 4.

---

### 6. CÁC THÍ NGHIỆM ĐỀ XUẤT BỔ SUNG Ở BƯỚC 4

Dựa trên kết quả rà soát độc lập, nhóm đề xuất danh mục các thí nghiệm và nâng cấp cần triển khai ở Bước 4 (Hardening & Complete Verification):
1. **Bổ sung Defensive Arithmetic Guards**:
   - Thêm guard `if (list->capacity > SIZE_MAX / 2) return STATUS_ERR_MEMORY;` (REQ-2).
   - Thêm guard `if (new_cap > SIZE_MAX / sizeof(Product)) return STATUS_ERR_MEMORY;` (REQ-3).
2. **Nâng cao độ phủ mã nguồn (Coverage Expansion)**:
   - Viết thêm unit tests cho các nhánh xử lý lỗi I/O trong `file_io.c` (`fopen == NULL`).
   - Viết test suite bao quát các hàm in ấn bảng biểu console trong `report.c` để nâng độ phủ từ 11% lên $\ge 60\%$.
   - Bổ sung test cho các hàm tiện ích xâu ký tự trong `utils.c`.
3. **Môi trường Linux cho Native Formal Tools & Sanitizers**:
   - Khi thiết lập môi trường Linux (Ubuntu 22.04 LTS), chạy kiểm chứng đối sánh với native `cbmc`, `NuSMV`, `gcc -fsanitize=address,undefined` và `valgrind --leak-check=full`.

---

### 7. CÁC KIỂM CHỨNG CÒN THIẾU TRƯỚC KHI HOÀN THIỆN NGHIÊN CỨU

Nhằm đảm bảo sự minh bạch khoa học, các khoảng trống kiểm chứng sau cần được ghi nhận rõ trước khi hoàn thiện toàn bộ công trình:
1. **CBMC Native Symbolic Verification**: Bounded Model Checking trực tiếp trên AST C bằng binary `cbmc` native chưa được thực hiện do hạn chế môi trường Windows.
2. **NuSMV Native Temporal Verification**: BDD/SAT Model Checking chuẩn công nghiệp trên binary `NuSMV`/`nuXmv` native chưa được thực hiện; hiện mới sử dụng Custom CTL Checker viết bằng Python.
3. **Native Sanitizers (ASan, UBSan, LSan) và Valgrind**: Các công cụ dò lỗi động chuẩn công nghiệp chưa thể biên dịch và liên kết trên Windows MSYS2 UCRT64 GCC 16.1.0; hiện phụ thuộc vào Canary Tracker và Stack Protector nội bộ.
4. **Các nhánh mã nguồn C chưa được phủ (Uncovered Branches theo Gcov 36.4% Line, 32.2% Branch)**:
   - Nhánh xử lý `realloc == NULL` trong `product.c` (chưa phủ trong luồng build gcov thông thường).
   - Nhánh lỗi I/O tệp tin (`fopen == NULL`) trong `file_io.c`.
   - Các nhánh định dạng in ấn bảng biểu console trong `report.c`.
   - Các hàm tiện ích thao tác chuỗi trong `utils.c`.
5. **Cài đặt Guard tường minh cho REQ-2 và REQ-3**: Cần triển khai các guard số học thực tế vào `source/src/product.c` trong Bước 4.

---

### 8. ĐÁNH GIÁ MỨC ĐỘ SẴN SÀNG NGHIỆM THU (READINESS ASSESSMENT)

| Tiêu chí | Trạng thái | Đánh giá độc lập |
| :--- | :--- | :--- |
| **Bảo vệ mã nguồn Baseline v0** | `source/` giữ nguyên vẹn | **100% ĐẠT** (git diff trống) |
| **Khả năng tái lập (Reproducibility)** | `python tools/run_step3_verification.py --all` | **100% ĐẠT** (Chạy thành công các bước khả dụng trong 6.5s) |
| **Tính minh bạch công cụ** | Khai báo đúng `TOOL_UNAVAILABLE` cho CBMC, NuSMV, ASan | **100% ĐẠT** |
| **Chuẩn hóa tên công cụ** | GCC Harness Testing, Custom CTL Checker, Canary Tracker | **100% ĐẠT** |
| **Độ chính xác toán học SMT** | Đúng nghiệm Z3 `#xeea85800eaaaaaaa` và ngưỡng $88,686,269,585,142,075$ | **100% ĐẠT** |
| **Khớp 100% đặc tả Step 2** | Nguyên văn 18 REQ, đúng ánh xạ hàm và thuộc tính | **100% ĐẠT** |
| **Hiệu chỉnh REQ-2 và REQ-3** | Đánh dấu `PARTIALLY_VERIFIED`, chuyển bổ sung guard sang Bước 4 | **100% ĐẠT** |
| **Xác thực mã lỗi thực tế** | Ghi nhận đúng `STATUS_ERR_MEMORY = -8` | **100% ĐẠT** |
| **Chất lượng Git Commits** | Commits tiếng Việt có dấu, working tree sạch | **100% ĐẠT** |

**KẾT LUẬN CUỐI CÙNG**:
- Hồ sơ Bước 3 sau đợt kiểm toán độc lập đã **loại bỏ các nhận định chủ quan hoặc khẳng định quá mức**, chuẩn hóa chính xác tên và bản chất công cụ, phân định rành mạch giữa kết quả toán học SMT và tính khả đạt trên mã C baseline v0.
- **Lưu ý quan trọng**: Lệnh `python tools/run_step3_verification.py --all` chạy thành công minh chứng pipeline đã hoàn thành các bước kiểm tra khả dụng trên host hiện tại; điều này không đồng nghĩa tất cả các công cụ kiểm chứng formal native (CBMC, NuSMV, ASan, UBSan, Valgrind) đều đã hoạt động.
- Trạng thái: **SẴN SÀNG NGHIỆM THU BƯỚC 3** và chuyển giao các hạng mục vá sang Bước 4.
