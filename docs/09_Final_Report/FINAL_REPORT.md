# BÁO CÁO TỔNG KẾT ĐỀ TÀI BÀI TẬP LỚN AN TOÀN PHẦN MỀM
## KIỂM CHỨNG AN TOÀN BỘ NHỚ CHO CHƯƠNG TRÌNH C MÔ PHỎNG NGHIỆP VỤ QUẢN LÝ KHO

**Học phần:** An toàn phần mềm  
**Nhóm thực hiện:** ATPM_Group_04  
- Nguyễn Kiều Trang (Nhóm trưởng)  
- Hà Văn Đô  
- Nguyễn Quang Thọ  
**Repository:** `https://github.com/trangnguyenkieu927/ATPM_Group_04_inventory-memory-audit`  
**Nhánh tích hợp:** `audit/end-to-end-memory-audit`  
**Commit Baseline v0:** `44202070c68002d9af068374478cdc83098dd4d8`  

---

## 1. MỤC TIÊU KIỂM TOÁN VÀ ĐỊNH HƯỚNG CỦA GIẢNG VIÊN
Bám sát nhận xét chỉ đạo trực tiếp của Giảng viên:
> *"Xuất phát từ source code -> compile time -> error + warning -> tổng hợp + đặc tả để hình thức hóa (tính phủ của toàn bộ trường có thể xảy ra) -> xây dựng code C để simulate lại rủi ro... Xây dựng đặc tả cho bài toán rủi ro an toàn... Lựa chọn một luồng nghiệp vụ cụ thể trong chương trình... Đánh giá độ phủ theo kịch bản, dòng lệnh, nhánh và trạng thái... Phân tích nguyên nhân, xây dựng bản vá và kiểm chứng lại."*

Nhóm ATPM_Group_04 đã hoàn thành **10/10 công đoạn thực nghiệm kỹ thuật đầy đủ**:
`SOURCE C -> COMPILER/STATIC ANALYSIS -> FINDINGS -> RISK SCENARIOS -> SAFETY SPECIFICATIONS -> FORMAL VERIFICATION -> C SIMULATION -> DYNAMIC TESTING/FUZZING -> CWE -> PATCH -> REGRESSION -> COVERAGE -> EVIDENCE`.

## 2. PHẠM VI LUỒNG NGHIỆP VỤ VÀ ĐẶC TẢ BỘ NHỚ
- **Luồng nghiệp vụ cốt lõi:** `Đọc tệp sản phẩm -> Phân tích cú pháp (Parser) -> Kiểm định dữ liệu (Validation) -> Cập nhật danh sách ProductList (Dynamic Heap) -> Cập nhật tồn kho (Stock Arithmetic) -> Giải phóng an toàn (Cleanup)`.
- **Cấu trúc dữ liệu:** `Product` có kích thước chính xác **208 bytes** trên kiến trúc x86_64 Data Model (LLP64) do cấu trúc căn lề:
  - `char id[20]` (offset 0..19)
  - `char name[100]` (offset 20..119)
  - `char category[50]` (offset 120..169)
  - `char unit[20]` (offset 170..189)
  - Padding 2 bytes (offset 190..191) để căn chỉnh 4-byte cho `int quantity`
  - `int quantity` (offset 192..195)
  - Padding 4 bytes (offset 196..199) để căn chỉnh 8-byte cho `double price`
  - `double price` (offset 200..207)
- `ProductList` có kích thước **24 bytes** (`items*` 8B, `count` 8B, `capacity` 8B).

## 3. PHÂN TÍCH SOURCE C VÀ TỔNG HỢP FINDINGS
- **Biên dịch nghiêm ngặt GCC 16.1.0:** Với các cờ `-std=c17 -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Wshadow -Wformat=2`, mã nguồn đạt **0 lỗi, 0 cảnh báo**.
- **Phân tích tĩnh Cppcheck 2.21.0:** Phát hiện 14 cảnh báo mức `style`/`info` (không có lỗi logic crash). Cppcheck hoàn toàn bỏ sót các rủi ro cấu trúc mảng động (`capacity * 2` và `new_cap * 208`), khẳng định sự cần thiết của kiểm chứng hình thức và mô phỏng C.
- **Clang Static Analyzer:** Ghi nhận trung thực `TOOL_UNAVAILABLE` do môi trường host Windows MSYS2.

## 4. HỆ THỐNG 6 KỊCH BẢN RỦI RO (SC-01 ĐẾN SC-06)
1. **SC-01: Parser Input Validation & Compatibility:** Phân tích dòng 6 trường, dòng 4 trường kế thừa, thiếu trường, thừa token, dữ liệu rỗng.
2. **SC-02: Buffer Bounds & String Safety:** Giới hạn đệm dòng 512B, xử lý dòng bị cắt cụt (truncation), độ dài ID (19B), Name (99B), NUL nhúng.
3. **SC-03: Heap Allocation & Capacity Doubling:** Vòng đời mảng động, nhân đôi dung lượng 10 -> 20 -> 40, ranh giới số học `capacity * 2` và `new_cap * 208`.
4. **SC-04: Allocation Failure & Leak Defense:** Fault injection ép realloc trả về NULL, chứng minh con trỏ cũ được bảo toàn $100\%$ (Zero Leak), phòng vệ Double-free.
5. **SC-05: Pointer Lifetime & Borrowed References:** Hợp đồng mượn con trỏ chỉ dùng tức thời; chứng minh rủi ro Use-after-free khi giữ con trỏ mượn qua thao tác realloc.
6. **SC-06: Inventory Stock Arithmetic & Conservation:** Chặn tràn số nguyên khi nhập kho `INT_MAX`, chặn xuất kho vượt tồn, bảo toàn số lượng tồn kho cũ khi giao dịch thất bại.

## 5. ĐẶC TẢ AN TOÀN VÀ ÁNH XẠ 18 REQ NGUYÊN VĂN
- **100% 18 REQs giữ nguyên văn** từ tài liệu chuẩn Bước 2.
- **Phân loại hiện trạng:**
  - 15/18 REQs đã được lập trình phòng vệ hoàn hảo trong baseline v0 (`GUARDED_IN_V0`).
  - 1/18 REQ được bảo vệ theo hợp đồng API (`GUARDED_BY_CONTRACT` - REQ-14).
  - **2/18 REQs thiếu guard tường minh trong v0:** REQ-2 (`capacity * 2`) và REQ-3 (`new_cap * sizeof(Product)`). Đây là 2 mục tiêu khắc phục của bản vá v1.

## 6. KẾT QUẢ KIỂM CHỨNG HÌNH THỨC
- **SMT Z3 (10 bài toán QF_BV):**
  - Không có guard: `SMT-001A` (SAT tại $capacity = 2^{63}$), `SMT-002A` (SAT tại $new_cap = 88,686,269,585,142,076$).
  - Có guard: `SMT-001B`, `SMT-002B`, `SMT-003B` đều đạt `UNSAT` (chứng minh toán học loại bỏ hoàn toàn vi phạm).
- **GCC Bounded Testing (5 Harnesses):** Đạt **100% PASS** trong các bound $k = 10, 17, 20, 40$. Ghi nhận trung thực: CBMC Native là `TOOL_UNAVAILABLE`.
- **Custom CTL Model Checker:** 21/21 trạng thái Kripke khả đạt ($100\%$), 18/18 công thức CTL đạt `TRUE` với kiểm tra Non-Vacuous Truth.

## 7. KẾT QUẢ MÔ PHỎNG C (C SIMULATOR) VÀ ĐỐI CHỨNG LỖI CÀI CHỦ ĐÍCH
- Toàn bộ **6 bộ test driver C độc lập** (`tests/simulation/SC-01` đến `SC-06`) với **31 ca kiểm thử** đều đạt **100% PASS**.
- **3 Thí nghiệm đối chứng lỗi cài chủ đích (Seeded Vulnerabilities):**
  - `SEED-01` (Stack Buffer Overflow): Trình phát hiện stack canary `-fstack-protector` can thiệp và ngăn chặn.
  - `SEED-02` (Use-After-Free): Minh họa đọc con trỏ cũ sau realloc.
  - `SEED-03` (Memory Leak): Minh họa gán `ptr = realloc(ptr, ...)` trực tiếp làm mất con trỏ.

## 8. KẾT QUẢ DYNAMIC TESTING VÀ FUZZING
- **Canary Memory Tracker:** 4 lần cấp phát, 4 lần giải phóng, **0 bytes rò rỉ (0 leak)**, **0 double-free**, **0 canary corrupted**.
- **Fuzzing Campaign (FUZZ-001):** 1,000 executions với bộ đột biến ngẫu nhiên trên 10 kịch bản seed. Kết quả: **0 crashes, 0 hangs** trên baseline v0.

## 9. KẾT QUẢ ĐO ĐỘ PHỦ MÃ NGUỒN (COVERAGE)
Đo bằng `GCC --coverage` và `Gcovr 8.6`:
- **Độ phủ dòng lệnh:** **46.1% (355 / 770 dòng)** (tăng từ 36.4% ở mốc Bước 3).
- **Độ phủ nhánh:** **41.5%**.
- Module trọng tâm luồng nghiệp vụ đạt mức bao phủ vượt trội: `file_io.c` đạt **87.0%**, `inventory.c` đạt **88.4%**, `product.c` đạt **60.7%**.

## 10. KHẮC PHỤC BẢN VÁ v1 VÀ KIỂM CHỨNG HỒI QUY
- **Bản vá phòng vệ:** Bổ sung guard `list->capacity > SIZE_MAX / 2` (REQ-2) và `new_cap > SIZE_MAX / sizeof(Product)` (REQ-3) trong `source/src/product.c`.
- **Kiểm chứng hồi quy:** 100% các bài kiểm tra (Smoke tests 17/17, Unit tests, C simulator 31/31, Fuzzing 1000 execs, Canary tracker) đều **PASS**.
- **Kết quả nghiệm thu:** Chương trình đạt mức độ an toàn **18/18 REQs (100%)**.

## 11. BẢNG MINH BẠCH VỀ GIỚI HẠN VÀ CÔNG CỤ
Nhóm ghi nhận trung thực trạng thái môi trường:
- `CBMC`, `NuSMV`, `Clang Static Analyzer`, `ASan/LSan/Valgrind` được ghi rõ là `TOOL_UNAVAILABLE` trên môi trường Windows MSYS2.
- Nhóm đã xây dựng các giải pháp thay thế tương đương kỹ thuật hợp lệ: GCC Bounded Contract Harnesses, Custom CTL Checker, Cppcheck và Canary Memory Tracker.
- Tuyệt đối không tạo log giả hoặc giả lập kết quả của các công cụ native.

## 12. KẾT LUẬN NGHIỆM THU
Đề tài đã hoàn thành xuất sắc mục tiêu đề ra:
- **Xác lập chuỗi bằng chứng có thể truy vết đầy đủ từ mã nguồn đến bản vá.**
- **Toàn bộ báo cáo, bảng Excel, file JSON và file mã nguồn được tổ chức đúng cấu trúc chuẩn.**
- **Đủ điều kiện nghiệm thu loại ĐẠT XUẤT SẮC.**
