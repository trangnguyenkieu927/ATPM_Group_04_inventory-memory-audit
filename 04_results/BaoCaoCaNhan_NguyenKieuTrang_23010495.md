# ĐẠI HỌC PHENIKAA
## TRƯỜNG CÔNG NGHỆ THÔNG TIN
### BÀI TẬP LỚN HỌC PHẦN AN TOÀN PHẦN MỀM (CSE703093)

---

# BÁO CÁO CÁ NHÂN
## CHỦ ĐỀ: KIỂM TOÁN AN TOÀN BỘ NHỚ CHO CHƯƠNG TRÌNH C MÔ PHỎNG NGHIỆP VỤ QUẢN LÝ KHO HÀNG
### Phần việc phụ trách: Module 2 — Quản lý vòng đời bộ nhớ Heap (Heap Lifecycle Management): Mô hình hóa Kripke, Đặc tả CTL, Chứng minh SMT Z3, Phân tích tĩnh, Fuzzing và Kiểm toán động AddressSanitizer

* **Học phần:** CSE703093 – An toàn phần mềm
* **Lớp:** An toàn phần mềm (N02)
* **Nhóm thực hiện:** Nhóm 04 (ATPM_Group_04)
* **Sinh viên thực hiện:** Nguyễn Kiều Trang – 23010495
* **Vai trò:** Trưởng nhóm kiêm Phụ trách Module M2
* **Giảng viên hướng dẫn:** ThS. Vũ Quang Dũng
* **Học kỳ:** Học kỳ I, năm học 2026–2027
* **Hà Nội, 10/2026**

---

## MỤC LỤC

- **DANH MỤC BẢNG**
- **DANH MỤC HÌNH ẢNH**
- **CHƯƠNG 1. GIỚI THIỆU**
  - 1.1. Đề tài của nhóm
  - 1.2. Vị trí phần việc trong nhóm
  - 1.3. Mục tiêu phần việc
  - 1.4. Cấu trúc báo cáo
- **CHƯƠNG 2. NHIỆM VỤ ĐƯỢC GIAO**
  - 2.1. Nhiệm vụ phụ trách chính
  - 2.2. Phần việc cùng làm và rà soát
  - 2.3. Đầu vào, đầu ra và tiêu chí hoàn thành
- **CHƯƠNG 3. CƠ SỞ LÝ THUYẾT**
  - 3.1. Cấu trúc Kripke và mô hình hóa vòng đời bộ nhớ Heap
  - 3.2. Logic thời gian CTL và bài toán kiểm chứng an toàn
  - 3.3. Định lý SMT và bộ giải Z3 cho kiểm toán tràn số và con trỏ
  - 3.4. Kiểm toán an toàn bộ nhớ C, AddressSanitizer và cơ chế Shadow Memory
  - 3.5. Kiểm thử Fuzzing và phân tích tĩnh cấu trúc mã nguồn (AST / McCabe)
- **CHƯƠNG 4. QUÁ TRÌNH THỰC HIỆN**
  - 4.1. Từ kịch bản rủi ro Heap đến không gian trạng thái và nhãn nguyên tử
  - 4.2. Các quyết định thiết kế mô hình M2
  - 4.3. Quan hệ chuyển trạng thái giữa bản v0 và bản v1
  - 4.4. Xây dựng các công thức CTL và thuộc tính loại trừ lẫn nhau
  - 4.5. Xây dựng bốn bài toán kiểm tra số học và an toàn con trỏ cho Z3
  - 4.6. Thực nghiệm trên SecLabFramework tab Ch1 (Kripke & CTL)
  - 4.7. Thực nghiệm trên SecLabFramework tab Ch2 (Kiểm toán mã C)
  - 4.8. Thực nghiệm kiểm toán động với Clang AddressSanitizer
  - 4.9. Thực nghiệm bộ giải SMT Z3 độc lập trên Python
  - 4.10. Đối chiếu kết quả giữa các công cụ kiểm toán
- **CHƯƠNG 5. KẾT QUẢ VÀ PHÂN TÍCH**
  - 5.1. Tám trạng thái M2-v0 và sáu trạng thái M2-v1 của mô hình Heap
  - 5.2. Sơ đồ trạng thái Kripke của mô hình M2 (v0 và v1)
  - 5.3. Quan hệ chuyển: Các cạnh bị cắt và cơ chế phòng thủ bản vá
  - 5.4. Kết quả kiểm chứng sáu công thức CTL trên SecLabFramework
  - 5.5. Thuộc tính loại trừ lẫn nhau và kiểm chứng bất biến an toàn
  - 5.6. Phân tích kịch bản phản ví dụ thành tình huống thực tế
  - 5.7. Kết quả bốn bài toán kiểm tra SMT Z3 (SAT trên v0 và UNSAT trên v1)
  - 5.8. Kết quả phân tích tĩnh mã nguồn C trên SecLab Ch2
  - 5.9. Kết quả chiến dịch Fuzzing và phân tích AST độ phức tạp McCabe
  - 5.10. Kết quả kiểm toán động với AddressSanitizer (Shadow Bytes)
  - 5.11. Đối chiếu kết quả toàn diện giữa các nguồn kiểm chứng
- **CHƯƠNG 6. PHỐI HỢP TRONG NHÓM**
  - 6.1. Nhận đầu vào từ Module M1 (Nguyễn Quang Thọ)
  - 6.2. Bàn giao đầu ra cho Module M3 (Hà Văn Đô)
  - 6.3. Rà soát chéo ma trận ca kiểm thử và mã nguồn
  - 6.4. Phương thức quản lý dự án và điều hành nhóm của Trưởng nhóm
- **CHƯƠNG 7. KHÓ KHĂN VÀ HẠN CHẾ**
  - 7.1. Các vấn đề kỹ thuật gặp phải và giải pháp xử lý
  - 7.2. Khó khăn cá nhân trong quá trình nghiên cứu và thực nghiệm
  - 7.3. Hạn chế của phần việc và phạm vi nghiên cứu
- **CHƯƠNG 8. ĐÁNH GIÁ KẾT QUẢ VÀ BÀI HỌC KINH NGHIỆM**
  - 8.1. Đối chiếu kết quả với các mục tiêu đề ra
  - 8.2. Bài học rút ra về kiểm toán an toàn phần mềm
  - 8.3. Hướng phát triển và hoàn thiện tiếp theo
  - 8.4. Tự đánh giá mức độ đóng góp cá nhân
- **CAM KẾT VÀ GHI CHÚ CÔNG CỤ HỖ TRỢ**
- **TÀI LIỆU THAM KHẢO**
- **PHỤ LỤC**
  - Phụ lục A. Danh mục file minh chứng thực nghiệm
  - Phụ lục B. Trích xuất log AddressSanitizer và SMT Z3
  - Phụ lục C. Đặc tả bài toán số học trong sheet KiemTraSoHoc

---

## DANH MỤC BẢNG

* **Bảng 2.1.** Nhiệm vụ phụ trách chính của sinh viên Nguyễn Kiều Trang
* **Bảng 2.2.** Phần việc cùng làm hoặc rà soát trong Nhóm 04
* **Bảng 2.3.** Đầu vào, đầu ra và tiêu chí hoàn thành của phần việc Module M2
* **Bảng 3.1.** Các dạng công thức CTL sử dụng trong kiểm toán Module M2
* **Bảng 4.1.** Các nhóm nhãn nguyên tử trong mô hình Kripke M2
* **Bảng 4.2.** Các quyết định thiết kế mô hình vòng đời Heap M2
* **Bảng 5.1.** Danh mục trạng thái, nhãn nguyên tử và ý nghĩa nghiệp vụ của Module M2
* **Bảng 5.2.** Khác biệt quan hệ chuyển giữa bản lỗi M2-v0 (|R| = 9) và bản vá M2-v1 (|R| = 7)
* **Bảng 5.3.** Kết quả kiểm chứng 6 công thức CTL trên mô hình M2
* **Bảng 5.4.** Bảng tổng hợp đối chứng hình thức SMT Z3 trên 4 bài toán kiểm toán M2
* **Bảng 5.5.** Đối chiếu kết quả kiểm toán giữa SecLabFramework, Terminal Z3 và Clang AddressSanitizer
* **Bảng 6.1.** Đầu vào nhận từ các thành viên khác trong nhóm
* **Bảng 6.2.** Đầu ra bàn giao cho các thành viên khác trong nhóm
* **Bảng 7.1.** Tổng hợp các vấn đề kỹ thuật phát sinh và giải pháp xử lý
* **Bảng 8.1.** Đối chiếu kết quả thực nghiệm với các mục tiêu đề ra
* **Bảng A.1.** Danh sách 21 tệp ảnh minh chứng của phần việc
* **Bảng A.2.** Danh mục 9 tệp nhật ký kiểm toán (Audit Execution Logs) trong thư mục `04_results/logs/`
* **Bảng C.1.** Ràng buộc toán học của 4 bài toán kiểm tra số học và con trỏ ở bản v0 và v1

---

## DANH MỤC HÌNH ẢNH

* **Hình 1.1.** Chuỗi quy trình nghiệp vụ kho hàng 6 hành vi và vị trí quản lý Heap (H3, H6) của Module M2
* **Hình 4.1.** Sơ đồ Kripke bản lỗi M2-v0 hiển thị trên SecLabFramework tab Ch1
* **Hình 4.2.** Kết quả kiểm chứng CTL bản lỗi M2-v0 trên Console tool Thầy
* **Hình 4.3.** Kiểm tra thuộc tính loại trừ lẫn nhau (Mutual Exclusion) M2-v0 trên SecLabFramework
* **Hình 4.4.** Sơ đồ Kripke bản vá an toàn M2-v1 hiển thị trên SecLabFramework tab Ch1
* **Hình 4.5.** Kết quả kiểm chứng CTL bản vá M2-v1 trên Console tool Thầy
* **Hình 4.6.** Kiểm tra thuộc tính loại trừ lẫn nhau M2-v1 đạt an toàn trên SecLabFramework
* **Hình 5.1.** Kết quả phân tích tĩnh mã C M2-v0 trên SecLabFramework tab Ch2
* **Hình 5.2.** Chi tiết cảnh báo phân tích tĩnh CWE-401 (Memory Leak) và CWE-416 (Use-After-Free) trên M2-v0
* **Hình 5.3.** Phân tích tĩnh bản vá M2-v1 đạt an toàn, quét lỗi con trỏ 0 lỗi trên SecLab Ch2
* **Hình 5.4.** Phân tích cấu trúc Struct Alignment cho M2-v1 trên SecLab Ch2
* **Hình 5.5.** Chiến dịch Fuzzing hàm `expand_stock_items` trên SecLab Ch2
* **Hình 5.6.** Chiến dịch Fuzzing hàm `release_stock_items` trên SecLab Ch2
* **Hình 5.7.** Fuzzing phát hiện lỗi tràn số nguyên CWE-190 trong hàm `calc_expand_capacity_v0`
* **Hình 5.8.** Phân tích cây cú pháp trừu tượng AST và độ phức tạp chu trình McCabe = 7 cho M2-v0
* **Hình 5.9.** Giải bài toán SMT Z3 Heap Alloc Overflow trên giao diện SecLab Ch2
* **Hình 5.10.** Giải bài toán SMT Z3 tràn số nguyên khi nhân đôi Capacity trên SecLab Ch2
* **Hình 5.11.** Chi tiết kết quả chạy 4 kịch bản SMT Z3 SAT/UNSAT trên Terminal bằng Python
* **Hình 5.12.** Bảng tổng hợp đối chiếu kết quả SAT trên v0 và UNSAT trên v1 trên Terminal
* **Hình 5.13.** AddressSanitizer bắt trọn lỗi Heap-Use-After-Free tại thời gian chạy trên bản M2-v0
* **Hình 5.14.** AddressSanitizer chạy bản vá M2-v1 thành công tuyệt đối, 0 crash, clean memory

---

## CHƯƠNG 1. GIỚI THIỆU

### 1.1. Đề tài của nhóm
Nhóm 04 thực hiện đề tài **“Kiểm toán an toàn bộ nhớ cho chương trình C mô phỏng nghiệp vụ quản lý kho hàng”**. Hệ thống phần mềm quản lý kho đóng vai trò xương sống trong các dây chuyền phân phối logistics, tiếp nhận hàng ngàn mã kiện hàng, tính toán lượng tồn kho và giải phóng tài nguyên phiên làm việc với tốc độ cao. Do hạt nhân nghiệp vụ được viết bằng ngôn ngữ lập trình C để tối ưu hóa hiệu năng và truy cập trực tiếp phần cứng, hệ thống phải đối mặt với nguy cơ vi phạm an toàn bộ nhớ nghiêm trọng: ghi tràn bộ đệm Stack (CWE-121), tràn bộ đệm Heap (CWE-122), rò rỉ bộ nhớ khi cạn kiệt RAM (CWE-401), con trỏ lơ lửng Use-After-Free (CWE-416) và tràn số nguyên (CWE-190).

Toàn bộ quy trình nhập kho được Nhóm 04 chuẩn hóa thành sáu hành vi nghiệp vụ tuần tự:
* **H1 (Nhận mã phiếu nhập kho):** Tiếp nhận chuỗi ký tự thô từ mạng hoặc thiết bị quét mã vạch vào bộ đệm Stack.
* **H2 (Tách trường dữ liệu và xác thực định dạng):** Phân tích cú pháp chuỗi, kiểm tra ký tự hợp lệ và lưu trữ vào các biến cấu trúc.
* **H3 (Cấp phát và mở rộng danh mục hàng hóa):** Quản lý mảng động các sản phẩm `Product` trên Heap; nhân đôi dung lượng khi kho đầy.
* **H4 (Cập nhật số lượng tồn kho):** Thực hiện phép cộng dồn số lượng hàng hóa vào kho, đối mặt rủi ro tràn số nguyên.
* **H5 (Tính toán giá trị tài sản kho):** Nhân số lượng với đơn giá hàng hóa để tính tổng giá trị tài sản lưu kho.
* **H6 (Đóng phiên làm việc và thu hồi tài nguyên):** Giải phóng toàn bộ mảng động trên Heap, dọn dẹp bộ đệm và đóng kết nối.

Nhóm xây dựng hai phiên bản đối chứng: Bản lỗi v0 (cài cắm có chủ đích 6 lỗ hổng bảo mật đặc trưng) và Bản vá an toàn v1 (áp dụng các mẫu thiết kế phòng thủ - Defensive Programming Patterns). Kết quả chung của nhóm được kiểm chứng toàn diện thông qua hai công cụ chính: phần mềm của học phần (`SecLabFramework` / `SpecVerificationLab`) và bộ công cụ kiểm toán độc lập (SMT Solver Z3 trên Python, trình biên dịch Clang với AddressSanitizer, Fuzzing và phân tích AST).

### 1.2. Vị trí phần việc trong nhóm
Theo nguyên tắc tổ chức và phân công công việc của Nhóm 04, khối lượng đề tài được **chia đều 1/3 (33.33%) cho mỗi thành viên**. Nhóm không phân chia theo kiểu một người làm toàn bộ mô hình hay một người làm toàn bộ mã nguồn, mà chia theo kiến trúc ba tầng quản trị bộ nhớ độc lập của chương trình C. Mỗi thành viên chịu trách nhiệm trọn vẹn từ đầu đến cuối (Full Lifecycle) đối với module của mình: từ khảo sát rủi ro, mô hình hóa Kripke, viết công thức CTL, giải SMT Z3 đến lập trình mã C đối chứng và thực nghiệm với AddressSanitizer:
* **Nguyễn Quang Thọ (33.33% khối lượng):** Phụ trách trọn vẹn **Module M1 (Quản lý Stack & Chuỗi I/O – Hành vi H1, H2)**. Trọng tâm là kiểm soát tràn bộ đệm Stack (CWE-121) khi nạp mã phiếu và đọc ngoài biên mảng token (CWE-125).
* **Nguyễn Kiều Trang (33.33% khối lượng - Báo cáo này):** Phụ trách trọn vẹn **Module M2 (Quản lý Vòng đời Heap – Hành vi H3, H6)**. Trọng tâm là kiểm soát rò rỉ bộ nhớ khi realloc thất bại OOM (CWE-401) và con trỏ lơ lửng Use-After-Free (CWE-416). Đồng thời với vai trò Trưởng nhóm, em chủ trì việc điều phối tiến độ, chuẩn hóa giao thức dữ liệu giữa các module và tổng hợp báo cáo.
* **Hà Văn Đô (33.33% khối lượng):** Phụ trách trọn vẹn **Module M3 (Tính toán Số học & Cập nhật Kho – Hành vi H4, H5)**. Trọng tâm là kiểm soát tràn số nguyên có dấu (CWE-190) khi cộng dồn số lượng tồn kho và nhân đơn giá tính tài sản.
* **Giai đoạn Tích hợp & Đóng gói:** Cả ba thành viên cùng phối hợp ghép nối 3 module thành hệ thống hoàn chỉnh (`main_khohang_v0.c` và `main_khohang_v1.c`), chạy kiểm toán động toàn cục và đối chiếu chéo kết quả.

![Hình 1.1: Chuỗi công việc của nhóm và vị trí phần việc phụ trách](../04_results/anh_chay_tool_thay/Hinh_1_1_Chuoi_Cong_Viec_Nhom_04.png)
*Hình 1.1. Chuỗi công việc của nhóm và vị trí phần việc phụ trách*

Hình 1.1 biểu diễn tường minh chuỗi chuyển giao dữ liệu và các vòng phản hồi kiểm chứng giữa ba thành viên:
1. **Luồng dữ liệu xuôi (Mũi tên đen 1, 2, 3):**
   * *(Mũi tên 1):* Module M1 (Thọ) sau khi nhận mã phiếu và xác thực an toàn trên Stack (H1, H2) sẽ bàn giao danh sách token sản phẩm hợp lệ sang cho Module M2 (Trang) để cấp phát bộ nhớ động trên Heap (H3).
   * *(Mũi tên 2):* Module M2 (Trang) sau khi cấp phát và mở rộng mảng động `Product *items` an toàn sẽ bàn giao con trỏ và kích thước mảng sang cho Module M3 (Đô) để thực hiện tính toán số lượng tồn kho và giá trị tài sản (H4, H5).
   * *(Mũi tên 3):* Module M3 (Đô) sau khi tính toán xong sẽ bàn giao kết quả và trạng thái kiểm kê vào luồng tích hợp toàn hệ thống `main_khohang`.
2. **Vòng hoàn trả con trỏ giải phóng Heap (Mũi tên xanh nét đứt 4 - Phía dưới):**
   * Sau khi Module M3 (Đô) hoàn tất các phép tính trên các phần tử của mảng, quyền sở hữu con trỏ được hoàn trả về lại cho Module M2 (Trang) để thực hiện hành vi H6 (Đóng phiên làm việc và giải phóng an toàn mảng Heap bằng lệnh `free()` kết hợp gán ngay `NULL`). Cơ chế này đảm bảo tính đóng gói tài nguyên: Module nào cấp phát Heap (M2) thì Module đó chịu trách nhiệm thu hồi Heap (M2), ngăn chặn triệt để lỗ hổng Use-After-Free và rò rỉ bộ nhớ.
3. **Vòng phản hồi kiểm chứng phòng thủ (Mũi tên đỏ nét đứt 5 - Phía trên):**
   * Khi thực hiện kiểm toán động với AddressSanitizer, Fuzzing và bộ giải SMT Z3 trên toàn hệ thống phát hiện các lỗ hổng (như tràn số, cạn RAM OOM), kết quả phản ví dụ được phản hồi ngược về pha thiết kế đặc tả của cả ba module M1, M2, M3 để lập trình viên bổ sung các rào chắn phòng thủ (Defensive Guards: `MAX_SAFE_CAPACITY`, kiểm tra `tmp != NULL`, kiểm tra cận trên số học).

#### Phương pháp luận kiểm toán an toàn bộ nhớ C (Quy trình 5 bước cốt lõi của Nhóm 04)
Tuân thủ chặt chẽ phương pháp luận khoa học được ThS. Vũ Quang Dũng giảng dạy trong học phần An toàn phần mềm, quy trình kiểm toán của Nhóm 04 được triển khai bài bản qua 5 bước liên hoàn:
1. **Xuất phát từ Mã nguồn C & Quá trình Biên dịch (Compile-time Errors & Warnings):**
   Không xuất phát từ suy diễn trừu tượng, cả ba thành viên bắt đầu từ việc phân tích mã nguồn C và biên dịch nghiêm ngặt với các cờ rà soát (`gcc/clang -Wall -Wextra -Wconversion -Wformat-security`). Toàn bộ các cảnh báo biên dịch (`warning`) và thông báo lỗi (`error`) được thu thập và phân loại:
   * *Module M1 (Quản lý Stack):* Cảnh báo thao tác chuỗi thiếu kiểm soát kích thước (`gets()`, `strcpy()`, `sprintf()`), nguy cơ ghi tràn bộ đệm `stack-buffer-overflow` (CWE-121).
   * *Module M2 (Vòng đời Heap):* Cảnh báo gán đè con trỏ `list = realloc(list, ...)` làm mất dấu vùng nhớ cũ khi cạn RAM (CWE-401); cảnh báo giải phóng con trỏ nhưng không gán `NULL`, dẫn đến đọc ghi ô nhớ lơ lửng `heap-use-after-free` (CWE-416).
   * *Module M3 (Tính toán Số học):* Cảnh báo phép cộng dồn hoặc nhân dữ liệu lớn vượt quá giá trị cực đại có dấu `INT_MAX` dẫn đến tràn số nguyên (CWE-190).
2. **Hình thức hóa & Xây dựng Đặc tả (Formal Specification Coverage):**
   Từ các nhóm cảnh báo biên dịch, nhóm tổng hợp và đặc tả hình thức hóa hành vi hệ thống, đảm bảo **tính phủ (Coverage) của toàn bộ không gian trạng thái** có thể xảy ra trong chu trình xử lý nhập liệu và quản lý bộ nhớ:
   * **`WAIT` (Chờ dữ liệu):** Hệ thống ở trạng thái rảnh rỗi chờ tín hiệu / mã vạch / dữ liệu từ mạng (`idle`, `wait_input`).
   * **`INPUT` (Tiếp nhận nhập liệu):** Tiếp nhận luồng dữ liệu thô vào bộ nhớ tạm (`raw_buffer`).
   * **`VERIFY` (Xác thực dữ liệu):** Kiểm tra kiểu dữ liệu, định dạng cú pháp (mã phiếu nhập `NK-YYYY-XXXX`, đơn giá dương, không chứa ký tự điều khiển lạ).
   * **`BUFFER / CAPACITY CHECK` (Kiểm tra ranh giới bộ đệm & tài nguyên):** Kiểm tra độ dài chuỗi có vượt kích thước `MAX_BUFFER`? Mảng động Heap đã đầy chưa? Phép nhân kích thước `new_cap * sizeof(Product)` có vượt `INT_MAX`? Bộ nhớ RAM có bị cạn kiệt (OOM)?
   * **`BRANCHING` (Rẽ nhánh An toàn / Kích hoạt Rủi ro):**
     - Nhánh An toàn (`Safe`): Thao tác thành công, tài nguyên được bảo toàn, giải phóng sạch sẽ và đóng phiên làm việc.
     - Nhánh Rủi ro (`Violation`): Kích hoạt các trạng thái bẫy lỗi như tràn bộ đệm Stack (CWE-121), tràn số nguyên (CWE-190), rò rỉ bộ nhớ Heap (CWE-401) hoặc Use-After-Free (CWE-416).
3. **Cài đặt Mã C Simulate lại rủi ro (v0) & Bản vá phòng thủ (v1):**
   * *Bản v0:* Cài cắm có chủ đích để mô phỏng chính xác các nhánh rủi ro theo đúng mô hình Kripke.
   * *Bản v1:* Cài đặt các cơ chế phòng ngự (Defensive Programming Guards) triệt tiêu hoàn toàn các cạnh dẫn vào trạng thái lỗi.
4. **Sinh Test Case (Test Generation):**
   * Sinh các ca kiểm thử biên (Boundary Value Testing), kiểm thử mờ (Fuzzing) với 14–15 ca kiểm thử ngẫu nhiên, dữ liệu dị dạng, chuỗi vượt độ dài, số lượng âm, số lượng cực đại `INT_MAX` để kích hoạt các đường rẽ nhánh lỗi.
5. **Kiểm tra động & LƯU LẠI TOÀN BỘ NHẬT KÝ LOG (Audit Testing & Logging):**
   * Chạy kiểm toán với AddressSanitizer, Z3 SMT Solver, Pytest và SpecVerificationLab; thu thập và lưu giữ đầy đủ **10 tệp log thực nghiệm** tại `04_results/logs/` làm minh chứng nghiệm thu không thể bác bỏ.

### 1.3. Mục tiêu phần việc
Để đảm bảo tính nghiêm ngặt và khoa học, em đặt ra 5 mục tiêu cụ thể:
1. **Mô hình hóa hình thức Kripke:** Chuyển đổi các kịch bản rủi ro Heap thành cấu trúc Kripke 2 phiên bản: M2-v0 gồm 8 trạng thái (chứa 2 trạng thái bẫy vi phạm màu đỏ) và M2-v1 gồm 6 trạng thái (triệt tiêu hoàn toàn trạng thái bẫy).
2. **Đặc tả và kiểm chứng logic CTL:** Xây dựng hệ thống 6 công thức an toàn và bất biến (Safety & Liveness), kiểm chứng trên tab Ch1 của `SecLabFramework`, chứng minh v0 vi phạm và v1 đạt chuẩn an toàn 100%.
3. **Chứng minh toán học SMT Z3 SAT/UNSAT:** Xây dựng script Python độc lập (`kiem_tra_sat_unsat_m2.py`) mô hình hóa 4 bài toán kiểm toán của M2; chứng minh toán học đạt 100% SAT trên v0 (tìm ra Witness Model phản ví dụ cụ thể) và 100% UNSAT trên v1 (chứng minh rào chắn toán học bảo vệ tuyệt đối).
4. **Kiểm toán mã C thực tế & Dynamic Testing:** Viết mã C thực tế (`m2_cap_phat_heap_v0.c` và `m2_cap_phat_heap_v1.c`); thực hiện quét phân tích tĩnh, Fuzzing và phân tích AST McCabe trên tab Ch2 của `SecLabFramework`; biên dịch Clang `-fsanitize=address` để kích hoạt và phân tích lỗi `heap-use-after-free` thông qua cơ chế Shadow Memory.
5. **Đối chiếu và đánh giá đa nguồn:** Đặt kết quả của `SecLabFramework` (Ch1, Ch2), bộ giải Z3 Python và `AddressSanitizer` cạnh nhau để đối chiếu chéo, giải thích bản chất từng lỗi và chứng minh tính nhất quán của phương pháp luận.

### 1.4. Cấu trúc báo cáo
Báo cáo cá nhân gồm 8 chương: Chương 2 làm rõ nhiệm vụ và tiêu chí hoàn thành. Chương 3 tóm lược cơ sở lý thuyết Kripke, CTL, SMT Z3 và AddressSanitizer. Chương 4 trình bày chi tiết quá trình triển khai thực nghiệm. Chương 5 phân tích chuyên sâu các kết quả và hình ảnh minh chứng. Chương 6 trình bày sự phối hợp trong nhóm. Chương 7 nêu các khó khăn kỹ thuật và giải pháp xử lý. Chương 8 đánh giá kết quả, rút ra bài học kinh nghiệm và định hướng phát triển. Phần cuối là cam kết liêm chính, tài liệu tham khảo và phụ lục minh chứng.

---

## CHƯƠNG 2. NHIỆM VỤ ĐƯỢC GIAO

### 2.1. Nhiệm vụ phụ trách chính
Theo phân công của Nhóm 04 trong báo cáo nhóm [1], em phụ trách Module M2 – quản lý vòng đời bộ nhớ Heap, gắn với hành vi cấp phát, mở rộng danh mục hàng hóa (H3) và đóng phiên làm việc, thu hồi tài nguyên (H6). Các nhiệm vụ cụ thể được trình bày trong Bảng 2.1. “Phụ trách chính” nghĩa là em chịu trách nhiệm thực hiện, lưu minh chứng và giải thích kết quả của phần việc đó; các thành viên còn lại tham gia phối hợp và rà soát.

Đối với H3, em xem xét quá trình mở rộng vùng nhớ lưu danh mục hàng hóa, tập trung vào cách xử lý khi `realloc()` thất bại. Đối với H6, em xem xét thứ tự giải phóng và sử dụng vùng nhớ, tập trung vào trường hợp chương trình tiếp tục truy cập vùng nhớ đã được giải phóng. Hai tình huống này được theo dõi xuyên suốt từ đặc tả, mô hình trạng thái đến mã C và các bài kiểm tra của phiên bản v0, v1.

*Bảng 2.1. Nhiệm vụ phụ trách chính của sinh viên Nguyễn Kiều Trang*

| Nhiệm vụ | Sản phẩm | Minh chứng |
| :--- | :--- | :--- |
| **Phân tích các tình huống rủi ro của H3 và H6; xác định điều kiện đầu vào, điều kiện an toàn và điều kiện vi phạm** | Đặc tả các tình huống thuộc M2, gắn với thao tác và trạng thái bộ nhớ tương ứng (CWE-401 Memory Leak khi realloc OOM, CWE-416 Use-After-Free, CWE-190 Integer Overflow khi nhân đôi Capacity) | `01_spec/dacta_M2_cap_phat_heap.xlsx`<br>`01_spec/m2_cap_phat_heap.json` |
| **Xây dựng mô hình Kripke cho M2 ở v0 và v1; xác định trạng thái, nhãn và quan hệ chuyển** | Mô hình thể hiện các đường xử lý có lỗi (v0: 8 trạng thái, 2 nút bẫy lỗi đỏ) và cách xử lý sau khi sửa (v1: 6 trạng thái, 0 nút lỗi) | `01_spec/m2_cap_phat_heap.json`<br>`04_results/anh_chay_tool_thay/M2_V0_Ch1_SecLab_SoDo_Kripke_ToanManHinh_V0.png`<br>`04_results/anh_chay_tool_thay/M2_V1_Ch1_SecLab_SoDo_Kripke_ToanManHinh_V1.png` |
| **Viết công thức CTL và thực hiện kiểm chứng các thuộc tính của M2** | Danh sách 6 thuộc tính CTL ($AG(!violation)$, $AG(!memory\_leak)$, $AG(!use\_after\_free)$, $EF(realloc\_ok)$, $AF(done)$, $AG(list\_full 	o AF(done))$), kết quả kiểm chứng (v0 vi phạm 4/6, v1 đạt 6/6) và phản ví dụ | `04_results/anh_chay_tool_thay/M2_V0_Ch1_SecLab_Console_CTL_KiemChung.png`<br>`04_results/anh_chay_tool_thay/M2_V1_Ch1_SecLab_Console_CTL_KiemChung.png`<br>`04_results/anh_chay_tool_thay/M2_V0_Ch1_SecLab_Console_Mutual_Exclusion_1.png`<br>`04_results/anh_chay_tool_thay/M2_V1_Ch1_SecLab_Console_Mutual_Exclusion_1.png` |
| **Sử dụng, hoàn thiện mã C v0 để mô phỏng các tình huống đã đặc tả** | Mã C v0 (`m2_cap_phat_heap_v0.c`) có thể chạy lại để khảo sát điều kiện gây lỗi rò rỉ khi realloc OOM và Use-After-Free | `03_source/v0_ban_loi/m2_cap_phat_heap_v0.c`<br>`03_source/v0_ban_loi/main_khohang_v0.c` |
| **Phân tích tĩnh mã nguồn M2; đối chiếu cảnh báo với đoạn mã và tình huống liên quan** | Kết quả quét phân tích tĩnh trên SecLabFramework Ch2; phân tích vị trí dòng lệnh, nguyên nhân của cảnh báo CWE-401, CWE-416, CWE-190; cây AST và độ phức tạp McCabe = 7 | `04_results/anh_chay_tool_thay/M2_V0_Ch2_SecLab_Static_Scan_KetQua.png`<br>`04_results/anh_chay_tool_thay/M2_V0_Ch2_SecLab_Static_CWE401_CWE416_ChiTiet.png`<br>`04_results/anh_chay_tool_thay/M2_V1_Ch2_SecLab_QuetLoiConTro_0Loi_AnToan.png`<br>`04_results/anh_chay_tool_thay/M2_V0_Ch2_SecLab_AST_DoPhucTap_McCabe_7.png` |
| **Xây dựng và thực hiện các bài kiểm tra SMT/Z3 cho điều kiện rủi ro của M2** | 4 bài kiểm tra SMT/Z3 (`heap_alloc_bytes`, `calc_expand_capacity`, `realloc_buffer_leak`, `cleanup_and_read_uaf`); kết quả 4/4 SAT trên v0 (kèm mô hình phản ví dụ) và 4/4 UNSAT trên v1 | `02_kiemchung_mohinh/kiem_tra_sat_unsat_m2.py`<br>`04_results/anh_chay_tool_thay/M2_V0_Ch2_SecLab_SMT_Z3_SAT_HeapAlloc_Overflow.png`<br>`04_results/anh_chay_tool_thay/M2_V0_V1_Terminal_SMT_Z3_SAT_UNSAT_ChiTiet.png`<br>`04_results/anh_chay_tool_thay/M2_V0_V1_Terminal_SMT_Z3_SAT_UNSAT_BangTongHop.png` |
| **Chuẩn bị dữ liệu kiểm thử, chạy mã C với AddressSanitizer và thu thập kết quả** | Dữ liệu kiểm thử Fuzzing 14 ca kiểm thử biên; lệnh biên dịch Clang ASan và log chẩn đoán bắt trọn lỗi Heap-Use-After-Free với Shadow Bytes `[fd]` | `04_results/anh_chay_tool_thay/M2_V0_Ch2_SecLab_Fuzzing_ExpandStockItems.png`<br>`04_results/anh_chay_tool_thay/M2_V0_Terminal_ASan_Heap_Use_After_Free.png` |
| **Xây dựng, hoàn thiện bản vá v1 và chạy lại các bài kiểm tra tương ứng** | Mã C v1 (`m2_cap_phat_heap_v1.c`) áp dụng rào chắn phòng thủ và bảng đối chiếu kết quả toàn diện trước, sau sửa lỗi (sạch 0 lỗi, ASan clean memory) | `03_source/v1_ban_va/m2_cap_phat_heap_v1.c`<br>`03_source/v1_ban_va/main_khohang_v1.c`<br>`04_results/anh_chay_tool_thay/TongHop_V1_Terminal_ASan_Safe_Clean_Run.png` |

### 2.2. Phần việc cùng làm và rà soát

Ngoài nhiệm vụ chính tại M2, em tham gia trao đổi và rà soát các nội dung liên quan đến M1 và M3 để thống nhất cách mô tả tình huống, dữ liệu trao đổi và kết quả tổng hợp của nhóm. Phần phối hợp được phân biệt với phần phụ trách chính: em chịu trách nhiệm về nội dung mình đóng góp, còn người phụ trách module chịu trách nhiệm về kết quả của module đó.

Bảng 2.2 ghi nhận các phần việc phối hợp. Nội dung cụ thể cần được đối chiếu với quá trình làm việc và minh chứng của nhóm trước khi hoàn thiện báo cáo.

*Bảng 2.2. Phần việc cùng làm và rà soát trong Nhóm 04*

| Phần việc | Người phụ trách chính | Mức tham gia của em | Việc làm cụ thể |
| :--- | :--- | :--- | :--- |
| **Nội dung liên quan đến Module M1** | Nguyễn Quang Thọ | Cùng làm và rà soát | Rà soát giao thức truyền chuỗi token từ H2 (M1) sang tham số nạp mảng tại H3 (M2); kiểm tra kiểu dữ liệu chuỗi ký tự và kích thước bộ đệm Stack để đảm bảo token bàn giao sang Heap không bị cắt ngắn hoặc chứa ký tự kết thúc chuỗi ` ` sai quy cách; cùng rà soát tệp `01_spec/m1_doc_chuoi_buffer.json` và mã nguồn `m1_doc_chuoi_buffer_v0.c`. |
| **Nội dung liên quan đến Module M3** | Hà Văn Đô | Cùng làm và rà soát | Thống nhất con trỏ mảng động `Product*` và biến số lượng `items_count` bàn giao từ H3 (M2) sang H4, H5 (M3); quy định rõ điều kiện tiên quyết M3 chỉ thực hiện tính toán khi con trỏ khác `NULL`; thống nhất cơ chế hoàn trả con trỏ về cho M2 thu hồi tại H6; cùng rà soát tệp `01_spec/m3_tinh_so_hoc.json` và mã nguồn `m3_tinh_so_hoc_v0.c`. |
| **Đối chiếu và tổng hợp kết quả giữa các module** | Nguyễn Kiều Trang | Chủ trì thực hiện | Thiết kế ma trận đối chiếu kết quả Kripke, CTL, SMT Z3 và AddressSanitizer trên toàn hệ thống; ghép nối 3 module thành pipeline hoàn chỉnh trong `main_khohang_v0.c` và `main_khohang_v1.c`; tổng hợp 10 tệp log kiểm toán chính thức tại thư mục `04_results/logs/` và xây dựng sơ đồ chuỗi công việc Hình 1.1. |
| **Hoàn thiện báo cáo và thư mục minh chứng chung** | Nguyễn Kiều Trang | Chủ trì tổng hợp kiêm viết phần phụ trách M2 | Soạn thảo toàn bộ Chương 4, Chương 5 cho phần Module M2; tổng hợp tài liệu báo cáo chung của Nhóm 04; chuẩn hóa cấu trúc thư mục dự án trên GitHub; thu thập và tổ chức 21 tệp ảnh chụp minh chứng cùng 10 tệp nhật ký log thực nghiệm. |

Quá trình phối hợp cụ thể, những điểm cần thống nhất và cách nhóm xử lý các khác biệt được trình bày ở Chương 6.

### 2.3. Đầu vào, đầu ra và tiêu chí hoàn thành

Đầu vào của phần việc gồm mã nguồn, mô tả nghiệp vụ và các quy ước chung mà em sử dụng để thực hiện M2. Đầu ra là những sản phẩm em cần hoàn thành và bàn giao để nhóm rà soát, tổng hợp. Bảng 2.3 trình bày các thành phần này cùng tiêu chí đánh giá phần việc.

*Bảng 2.3. Đầu vào, đầu ra và tiêu chí hoàn thành của phần việc*

| Hạng mục | Nội dung chi tiết |
| :--- | :--- |
| **Đầu vào** | Mô tả hành vi H3 (Cấp phát, mở rộng danh mục) và H6 (Thu hồi tài nguyên) cùng các kịch bản rủi ro liên quan (CWE-190, CWE-401, CWE-416); mã C ban đầu của M2 (`m2_cap_phat_heap_v0.c`); cấu trúc dữ liệu `Product` và quy ước phối hợp giữa các module (con trỏ `items`, số lượng `count`); tài liệu, hướng dẫn sử dụng công cụ của học phần (`SecLabFramework` / `SpecVerificationLab`). |
| **Đầu ra** | Đặc tả M2 (`dacta_M2_cap_phat_heap.xlsx`, `m2_cap_phat_heap.json`); mô hình Kripke v0, v1; 6 công thức CTL và kết quả kiểm chứng; mã C v0, v1 (`m2_cap_phat_heap_v0.c`, `m2_cap_phat_heap_v1.c`); 4 bài kiểm tra SMT/Z3 (`kiem_tra_sat_unsat_m2.py`); dữ liệu kiểm thử Fuzzing (14 test cases); kết quả phân tích tĩnh và AddressSanitizer (Shadow Bytes `[fd]`); bảng đối chiếu trước, sau sửa lỗi (Bảng 5.5); nội dung báo cáo và hướng dẫn chạy lại phần phụ trách. |
| **Tiêu chí hoàn thành** | Các tình huống của M2 có điều kiện an toàn và điều kiện vi phạm rõ ràng; mô hình, mã C và bài kiểm tra có thể đối chiếu với nhau; kết quả được giải thích và lưu minh chứng; bản vá được kiểm tra lại với các trường hợp tương ứng; có đủ thông tin để chạy lại và kiểm tra kết quả. |

Các tiêu chí trên được cụ thể hóa như sau:

* **Đối với đặc tả và mô hình:** Mỗi tình huống cần chỉ ra trạng thái bộ nhớ liên quan, thao tác có thể gây vi phạm và yêu cầu xử lý. Thuộc tính CTL phải gắn với yêu cầu cần kiểm tra (ví dụ: $AG(!violation)$, $AG(!memory\_leak)$, $AG(!use\_after\_free)$); kết quả khác nhau giữa v0 và v1 phải được giải thích từ quan hệ chuyển hoặc nhãn trạng thái (v0 vi phạm do có cạnh chuyển sang trạng thái bẫy `m2_mat_con_tro_leak` và `m2_doc_con_tro_treo_uaf`; v1 an toàn do đã cắt bỏ 2 cạnh này).
* **Đối với mã C và kết quả kiểm toán:** Xác định được đoạn mã liên quan, điều kiện kích hoạt và nguyên nhân lỗi (hàm `expand_stock_items_v0` gán đè con trỏ trực tiếp khi `realloc()` trả về `NULL` gây CWE-401; hàm `cleanup_and_read_uaf_v0` đọc con trỏ sau khi `free()` gây CWE-416). Cảnh báo hoặc log được đối chiếu trực tiếp với mã nguồn; trường hợp công cụ phân tích tĩnh không bắt được tình huống số học phức tạp cũng cần ghi rõ để chuyển giao sang Z3 SMT giải quyết.
* **Đối với SMT/Z3 và kiểm thử động:** Nêu được điều kiện đưa vào bài kiểm tra, dữ liệu sử dụng và ý nghĩa của kết quả. Với tình huống phụ thuộc vào cấp phát thất bại, mô phỏng rõ điều kiện thất bại (`list_ptr == NULL`) để có thể chạy lại và quan sát trạng thái mất dấu vùng nhớ cũ.
* **Đối với bản vá:** Giải thích được thay đổi đã xử lý nguyên nhân nào (sử dụng con trỏ tạm `tmp`, kiểm tra `tmp != NULL`, gán `ptr = NULL` ngay sau khi `free`), chạy lại các bài kiểm tra tương ứng và ghi nhận kết quả trước, sau sửa. Những điều kiện chưa được kiểm tra phải được nêu trong phần hạn chế (Chương 7).
* **Đối với minh chứng:** Lưu mã nguồn, cấu hình hoặc lệnh chạy, dữ liệu đầu vào và kết quả theo tên gọi thống nhất trong thư mục `04_results/` để người đọc có thể tìm và đối chiếu nhanh chóng.

Chương 4 trình bày cách em thực hiện các nhiệm vụ này; Chương 5 trình bày kết quả và phân tích. Việc đối chiếu kết quả với mục tiêu đã đặt ra ở Chương 1 được thực hiện tại Chương 8.

---

## CHƯƠNG 3. CƠ SỞ LÝ THUYẾT

### 3.1. Cấu trúc Kripke và mô hình hóa vòng đời bộ nhớ Heap
Cấu trúc Kripke là một hệ thống chuyển đổi trạng thái hình thức được định nghĩa bởi bộ bốn:
$$K = (S, S_0, R, L)$$
Trong đó:
* $S$ là tập hữu hạn các trạng thái của hệ thống.
* $S_0 \subseteq S$ là tập các trạng thái khởi tạo ban đầu.
* $R \subseteq S 	imes S$ là quan hệ chuyển trạng thái giữa các bước thực thi. Để đảm bảo tính toán trên các đường đi vô hạn, quan hệ $R$ phải là toàn phần (total relation), nghĩa là với mọi $s \in S$, luôn tồn tại $s' \in S$ sao cho $(s, s') \in R$. Nếu một trạng thái không có cạnh đi ra, hệ thống sẽ rơi vào lỗi treo (Deadlock).
* $L: S 	o 2^{AP}$ là hàm gán nhãn, ánh xạ mỗi trạng thái $s$ tới một tập các mệnh đề nguyên tử (Atomic Propositions - $AP$) đúng tại trạng thái đó.

Trong Module M2, cấu trúc Kripke được dùng để mô hình hóa vòng đời cấp phát, tái cấp phát và giải phóng của một con trỏ Heap: từ trạng thái khởi tạo `m2_khoi_tao`, qua trạng thái danh sách đầy `m2_danh_sach_day`, gọi hàm mở rộng `m2_realloc_goi`, rẽ nhánh sang sự cố cạn RAM OOM `m2_oom_null` hoặc mở rộng thành công `m2_realloc_thanh_cong`, và cuối cùng là giải phóng tài nguyên. Điểm mấu chốt là hàm gán nhãn $L$: các trạng thái vi phạm được gắn các nhãn cảnh báo như `memory_leak`, `use_after_free`, `cwe_401`, `cwe_416` và nhãn bao quát `violation`.

### 3.2. Logic thời gian CTL và bài toán kiểm chứng an toàn
Logic thời gian cây tính toán (Computation Tree Logic - CTL) là logic thời gian phân nhánh cho phép biểu diễn các thuộc tính hành vi trên cây thực thi bắt nguồn từ một trạng thái. Cú pháp CTL kết hợp giữa lượng từ đường đi ($A$ - với mọi đường đi, $E$ - tồn tại ít nhất một đường đi) và toán tử thời gian ($G$ - luôn luôn, $F$ - tương lai cuối cùng, $X$ - bước kế tiếp, $U$ - cho đến khi).

*Bảng 3.1. Các dạng công thức CTL sử dụng trong kiểm toán Module M2*

| Dạng công thức | Tên gọi ngữ nghĩa | Điều kiện đúng tại trạng thái $s$ | Ứng dụng kiểm toán trong Module M2 |
| :---: | :--- | :--- | :--- |
| **$AG(\phi)$** | Luôn luôn an toàn (Safety Invariant) | $\phi$ đúng tại mọi trạng thái đạt tới được trên mọi đường đi từ $s$ | $AG(!violation)$: Không bao giờ rơi vào bất kỳ trạng thái vi phạm bộ nhớ nào. |
| **$AG(!x)$** | Ngăn chặn lỗ hổng cụ thể | Mệnh đề rủi ro $x$ không bao giờ xảy ra trên mọi nhánh | $AG(!memory_leak)$: Ngăn chặn rò rỉ bộ nhớ khi OOM.<br>$AG(!use_after_free)$: Cấm đọc ghi con trỏ sau khi free. |
| **$EF(\phi)$** | Khả năng đạt tới (Reachability) | Tồn tại ít nhất một đường đi tương lai chạm tới trạng thái thỏa mãn $\phi$ | $EF(realloc_ok)$: Hệ thống luôn có khả năng mở rộng bộ nhớ thành công. |
| **$AF(\phi)$** | Tính sống / Không treo (Liveness) | Mọi đường đi thực thi cuối cùng đều chạm tới trạng thái thỏa mãn $\phi$ | $AF(done)$: Hệ thống không bao giờ bị kẹt lại vô hạn trong chu trình cấp phát Heap. |

Thuật toán Model Checking CTL đánh giá tính thỏa mãn dựa trên phép tính điểm bất động (Fixed-point computation): $EF(\phi)$ là điểm bất động nhỏ nhất của phương trình $Z = \phi \lor EX(Z)$, trong khi $EG(\phi)$ là điểm bất động lớn nhất của $Z = \phi \land EX(Z)$. Công thức $AG(\phi)$ được kiểm tra thông qua công thức tương đương kép: $AG(\phi) \equiv 
eg EF(
eg \phi)$.

### 3.3. Định lý SMT và bộ giải Z3 cho kiểm toán tràn số và con trỏ
Bài toán thỏa mãn modulo lý thuyết (Satisfiability Modulo Theories - SMT) mở rộng bài toán SAT Boolean truyền thống bằng cách tích hợp các lý thuyết toán học chuyên biệt như số học nguyên (Linear/Non-linear Integer Arithmetic), mảng (Arrays) và véctơ bít (Bit-vectors).

Bộ giải Z3 (Microsoft Research) là công cụ giải SMT tiêu chuẩn công nghiệp. Để kiểm toán một thuộc tính an toàn, bài toán được thiết lập dưới dạng tìm phản ví dụ (Proof by Refutation):
$$\Phi = 	ext{Tiền điều kiện (Preconditions)} \land 	ext{Điều kiện vi phạm (Bad Conditions)}$$
* Nếu Z3 trả về **SAT (Satisfiable):** Tồn tại một bộ giá trị gán cho các biến làm cho $\Phi$ đúng. Bộ giá trị này chính là **mô hình phản ví dụ (Witness Counterexample)** chứng minh lỗ hổng bảo mật hoàn toàn có thể bị kích hoạt trong thực tế.
* Nếu Z3 trả về **UNSAT (Unsatisfiable):** Không tồn tại bất kỳ bộ giá trị nào thỏa mãn điều kiện vi phạm trong không gian tìm kiếm. Điều này mang ý nghĩa **chứng minh toán học tuyệt đối** rằng đoạn mã đã được bảo vệ an toàn 100% trước lớp lỗ hổng đó.

### 3.4. Kiểm toán an toàn bộ nhớ C, AddressSanitizer và cơ chế Shadow Memory
Trong môi trường thực thi ngôn ngữ C, bộ nhớ Heap được quản lý thủ công qua `malloc()`, `realloc()` và `free()`. Các lỗi phổ biến gồm:
1. **Integer Overflow dẫn tới Heap Overflow (CWE-190/CWE-122):** Phép tính kích thước `new_capacity * sizeof(Item)` bị tràn số nguyên 32-bit có dấu khi giá trị vượt quá $2.147.483.647$ (`INT_MAX`), lật về số âm hoặc số rất nhỏ. `malloc()` cấp phát một khối nhớ nhỏ, nhưng vòng lặp ghi chép hàng triệu phần tử sẽ ghi đè lên siêu dữ liệu của Heap (Heap Metadata) gây crash hoặc cho phép kẻ tấn công chiếm quyền điều khiển.
2. **Memory Leak khi OOM (CWE-401):** Lời gọi `buf = realloc(buf, new_size)` nếu gặp tình trạng hết RAM sẽ trả về `NULL`. Việc gán trực tiếp làm mất địa chỉ của vùng nhớ cũ `buf`, khiến vùng nhớ này trôi nổi vĩnh viễn trên Heap mà không thể thu hồi.
3. **Use-After-Free (CWE-416):** Lệnh `free(ptr)` thông báo cho bộ cấp phát rằng ô nhớ đã sẵn sàng để tái sử dụng, nhưng không xóa giá trị trong con trỏ `ptr` (Dangling Pointer). Việc tiếp tục đọc `*ptr` có thể đọc phải dữ liệu rác, hoặc nếu ô nhớ đã được tái cấp phát cho mục đích khác, việc ghi vào `*ptr` sẽ phá hoại tính toàn vẹn của cấu trúc dữ liệu mới.

**Cơ chế hoạt động của AddressSanitizer (ASan):**
ASan sử dụng kỹ thuật ánh xạ bộ nhớ bóng (**Shadow Memory**). Cứ mỗi 8 byte trong không gian địa chỉ ứng dụng sẽ được ánh xạ tới 1 byte trong Shadow Memory. Giá trị của byte bóng phản ánh trạng thái của 8 byte ứng dụng:
* `00`: Cả 8 byte đều hợp lệ, cho phép đọc ghi.
* `01` đến `07`: Chỉ có $k$ byte đầu tiên hợp lệ.
* `fa`: Vùng đệm bảo vệ Heap (Heap Redzone).
* `fd`: Vùng nhớ Heap đã bị giải phóng (**Freed Heap Region**).

Khi biên dịch với `-fsanitize=address`, trình biên dịch chèn mã kiểm tra vào trước mỗi lệnh truy cập bộ nhớ. Nếu một lệnh đọc con trỏ truy cập vào ô nhớ có byte bóng là `[fd]`, ASan sẽ chặn đứng tiến trình ngay lập tức tại thời gian chạy và in ra báo cáo chẩn đoán chi tiết: địa chỉ ô nhớ vi phạm, ngăn xếp lời gọi giải phóng ô nhớ trước đó (Freed stack trace), và ngăn xếp lời gọi cấp phát ban đầu (Allocated stack trace).

### 3.5. Kiểm thử Fuzzing và phân tích tĩnh cấu trúc mã nguồn (AST / McCabe)
* **Fuzzing (Kiểm thử mờ):** Kỹ thuật kiểm thử động đưa hàng loạt dữ liệu đầu vào ngẫu nhiên, dị dạng hoặc ở các giá trị biên đặc biệt (Boundary values) vào các hàm C nhằm phát hiện lỗi crash, tràn số hoặc rò rỉ tài nguyên. Tab Ch2 của `SecLabFramework` hỗ trợ Fuzzing theo 5 chiến lược: Biên (Boundary), Hộp đen (Black-box), Đột biến (Mutation), Hướng độ bao phủ (Coverage-guided) và Concolic (White-box kết hợp SMT).
* **Cây cú pháp trừu tượng (AST) và Độ phức tạp McCabe:** Trình phân tích tĩnh phân tích mã nguồn C thành cây AST để phát hiện các mẫu gọi hàm nguy hiểm (`strcpy`, `malloc` thiếu kiểm tra `NULL`, `free` thiếu gán `NULL`). Độ phức tạp chu trình McCabe ($M = E - N + 2P$) đo lường số lượng đường đi độc lập qua mã nguồn, giúp đánh giá rủi ro tiềm ẩn của các khối lệnh điều khiển phức tạp.

---

## CHƯƠNG 4. QUÁ TRÌNH THỰC HIỆN

### 4.1. Từ kịch bản rủi ro Heap đến không gian trạng thái và nhãn nguyên tử
Xuất phát từ phân tích nghiệp vụ kho hàng, em xác định hai giai đoạn rủi ro lớn nhất thuộc Module M2:
1. **Giai đoạn mở rộng danh mục hàng hóa (Hành vi H3):** Khi kho đã chứa đủ 16 sản phẩm mặc định, việc tiếp nhận thêm sản phẩm mới buộc hệ thống phải mở rộng mảng động. Rủi ro nảy sinh khi phép nhân đôi dung lượng bị tràn số nguyên (CWE-190) hoặc lời gọi `realloc()` thất bại do hệ thống cạn RAM nhưng con trỏ bị ghi đè gây mất dấu vùng nhớ cũ (CWE-401).
2. **Giai đoạn đóng phiên kho và thu hồi tài nguyên (Hành vi H6):** Sau khi hoàn tất kiểm kê, toàn bộ mảng động phải được thu hồi. Rủi ro nảy sinh khi lập trình viên thực hiện `free(list)` nhưng quên gán `list = NULL`, sau đó một hàm kiểm kê khác đọc lại dữ liệu `list[0]` gây ra lỗ hổng Use-After-Free (CWE-416).

Để chuyển đổi các kịch bản này sang mô hình Kripke, em áp dụng nguyên tắc chuẩn hóa: Mỗi trạng thái đại diện cho một mốc logic rõ ràng trong vòng đời bộ nhớ; các nhãn nguyên tử được chia thành 4 nhóm chức năng (Bảng 4.1).

*Bảng 4.1. Các nhóm nhãn nguyên tử trong mô hình Kripke M2*

| Nhóm nhãn | Tên nhãn nguyên tử cụ thể | Mục đích và ý nghĩa kỹ thuật |
| :--- | :--- | :--- |
| **Nhãn khởi tạo** | `initialized` | Đánh dấu mảng động đã được khởi tạo thành công với kích thước mặc định ban đầu |
| **Nhãn tiến trình** | `list_full`, `reallocating`, `realloc_temp` | Mô tả các bước chuyển tiếp: danh sách chạm ngưỡng, đang gọi hàm tái cấp phát |
| **Nhãn rủi ro & lỗi** | `memory_leak`, `use_after_free`, `cwe_401`, `cwe_416`, `violation` | Đánh dấu trạng thái bẫy nguy hiểm; nhãn `violation` dùng làm cờ chung cho $AG(!violation)$ |
| **Nhãn an toàn & kết thúc** | `realloc_ok`, `owner_preserved`, `pointer_nullified`, `zero_leak`, `safe_exit`, `done` | Đánh dấu các mốc an toàn, giải phóng sạch sẽ và kết thúc chu trình hợp lệ |

### 4.2. Các quyết định thiết kế mô hình M2
Trong quá trình xây dựng đặc tả, em đưa ra các quyết định thiết kế có tính hệ thống nhằm đảm bảo mô hình phản ánh trung thực bản chất mã nguồn C (Bảng 4.2).

*Bảng 4.2. Các quyết định thiết kế mô hình vòng đời Heap M2*

| Câu hỏi thiết kế | Lựa chọn kỹ thuật | Lý do thiết kế và cơ sở khoa học |
| :--- | :--- | :--- |
| Trạng thái lỗi đi về đâu? | Tự lặp tại chỗ (Self-loop) đối với trạng thái bẫy | Ngữ nghĩa CTL yêu cầu quan hệ $R$ phải toàn phần. Việc tự lặp tạo ra nhánh thực thi vô hạn giúp $AF(done)$ phát hiện nhánh bị kẹt lại trong lỗi. |
| Bản vá v1 có xóa nút lỗi không? | Thay thế các nút lỗi bằng các nút xử lý an toàn | Bản v0 có 8 nút; Bản v1 chuẩn hóa còn 6 nút. Thay vì để trạng thái trôi nổi, bản v1 điều hướng luồng OOM sang `m2_oom_bao_toan_ram` và luồng dọn dẹp sang `m2_giai_phong_an_toan`. |
| Số lượng trạng thái khởi tạo? | Đúng 1 trạng thái duy nhất: `m2_khoi_tao` | Tránh hiện tượng định giá sai các công thức tồn tại $EF(\phi)$ khi công cụ của Thầy duyệt qua tập $S_0$. |
| Đồng bộ Excel và JSON? | Nguồn chân lý duy nhất (Single Source of Truth) | Tệp Excel `dacta_M2_cap_phat_heap.xlsx` và JSON `m2_cap_phat_heap.json` được sinh đồng bộ tự động, khớp 100% về tên trạng thái, nhãn và danh sách cạnh chuyển. |

### 4.3. Quan hệ chuyển trạng thái giữa bản v0 và bản v1
* **Bản lỗi M2-v0 (8 trạng thái, 9 cạnh chuyển):**
  - Luồng 1: `m2_khoi_tao` ➔ `m2_danh_sach_day` ➔ `m2_realloc_goi`.
  - Luồng lỗi OOM (CWE-401): `m2_realloc_goi` ➔ `m2_oom_null` ➔ `m2_mat_con_tro_leak` (Tự lặp).
  - Luồng mở rộng thành công: `m2_realloc_goi` ➔ `m2_realloc_thanh_cong` ➔ `m2_giai_phong_xong`.
  - Luồng lỗi UAF (CWE-416): `m2_giai_phong_xong` ➔ `m2_doc_con_tro_treo_uaf` (Tự lặp).
* **Bản vá an toàn M2-v1 (6 trạng thái, 7 cạnh chuyển):**
  - Cắt bỏ hoàn toàn 2 cạnh dẫn vào trạng thái bẫy lỗi đỏ: cắt cạnh `m2_oom_null ➔ m2_mat_con_tro_leak` và cắt cạnh `m2_giai_phong_xong ➔ m2_doc_con_tro_treo_uaf`.
  - Bổ sung cạnh rẽ nhánh an toàn OOM: `m2_realloc_tam` ➔ `m2_oom_bao_toan_ram` (mang nhãn `owner_preserved`, `safe_exit`, `done`).
  - Bổ sung cạnh giải phóng chuẩn phòng thủ: `m2_realloc_thanh_cong` ➔ `m2_giai_phong_an_toan` (mang nhãn `pointer_nullified`, `zero_leak`, `done`).

### 4.4. Xây dựng các công thức CTL và thuộc tính loại trừ lẫn nhau
Em đặc tả hệ thống 6 công thức CTL phản ánh đầy đủ các yêu cầu an toàn phần mềm:
1. `AG(!violation)`: Bất biến an toàn tuyệt đối – Không bao giờ chạm tới bất kỳ trạng thái vi phạm bộ nhớ nào.
2. `AG(!memory_leak)`: Ngăn chặn rò rỉ bộ nhớ – Cấm rơi vào trạng thái mất con trỏ gốc khi `realloc()` thất bại.
3. `AG(!use_after_free)`: Ngăn chặn Use-After-Free – Cấm hành vi đọc dữ liệu từ con trỏ lơ lửng sau khi giải phóng.
4. `EF(realloc_ok)`: Tính khả đạt – Tồn tại đường đi thực thi cho phép tái cấp phát mảng động thành công.
5. `AF(done)`: Tính sống – Mọi đường đi thực thi hợp lệ cuối cùng đều phải hoàn thành và thoát phiên an toàn.
6. `AG(!(cwe_401 & cwe_416))`: Loại trừ lẫn nhau (Mutual Exclusion) – Kiểm tra tính phân tách độc lập giữa lỗi rò rỉ OOM và lỗi con trỏ lơ lửng UAF.

### 4.5. Xây dựng bốn bài toán kiểm tra số học và an toàn con trỏ cho Z3
Trong sheet `KiemTraSoHoc` và script Python `kiem_tra_sat_unsat_m2.py`, em thiết lập 4 bài toán kiểm toán hình thức:
* **M2-CHECK-01 (heap_alloc_bytes):** Kiểm tra tràn số nguyên khi tính toán kích thước cấp phát `new_capacity * item_size > 2147483647` (với `item_size = 48` bytes của struct `Product`). Bản v0 không chặn trần $	o$ Kỳ vọng SAT. Bản v1 thêm rào chắn `new_capacity <= 44739242` $	o$ Kỳ vọng UNSAT.
* **M2-CHECK-02 (calc_expand_capacity):** Kiểm tra tràn số nguyên khi nhân đôi dung lượng kho `current_capacity * factor > 2147483647` (`factor = 2`). Bản v0 cho phép số lớn $	o$ Kỳ vọng SAT. Bản v1 thêm rào chắn `current_capacity <= INT32_MAX / 2` $	o$ Kỳ vọng UNSAT.
* **M2-CHECK-03 (realloc_buffer_leak):** Kiểm tra tính bảo toàn con trỏ khi OOM: `old_ptr > 0 and list_ptr == 0`. Bản v0 gán thẳng `list_ptr = 0` $	o$ Kỳ vọng SAT. Bản v1 dùng con trỏ tạm, nếu OOM thì `list_ptr == old_ptr` $	o$ Kỳ vọng UNSAT.
* **M2-CHECK-04 (cleanup_and_read_uaf):** Kiểm tra truy xuất con trỏ lơ lửng: `ptr_state == FREED (2) and read_op == 1`. Bản v0 giữ nguyên trạng thái sau free $	o$ Kỳ vọng SAT. Bản v1 gán `ptr_state = NULL (0)` và chỉ cho phép đọc khi `ptr_state == ALLOCATED (1)` $	o$ Kỳ vọng UNSAT.

### 4.6. Thực nghiệm trên SecLabFramework tab Ch1 (Kripke & CTL)
Em tiến hành nạp các file đặc tả `dacta_M2_cap_phat_heap.xlsx` và `m2_cap_phat_heap.json` vào tab Ch1 của công cụ `SecLabFramework`:
1. Nhấn nút **"Sinh Kripke + kiểm chứng"** để công cụ phân tích không gian trạng thái, kiểm tra Deadlock và tính toán giá trị chân lý cho các công thức CTL.
2. Kiểm tra thuộc tính **Mutual Exclusion** giữa các cặp nhãn nguy hiểm.
3. Xuất đồ thị trạng thái trực quan để đối chiếu màu sắc của các nút (xanh an toàn vs đỏ vi phạm).

### 4.7. Thực nghiệm trên SecLabFramework tab Ch2 (Kiểm toán mã C)
Em chuyển sang tab Ch2 để thực hiện kiểm toán toàn diện trên mã nguồn C thực tế:
1. **Phân tích tĩnh (Static Scan):** Quét lỗi cú pháp AST, kiểm tra các lời gọi hàm phân bổ bộ nhớ thiếu giải phóng hoặc sử dụng sau giải phóng.
2. **Chiến dịch Fuzzing:** Chạy Fuzzer tích hợp trên các hàm nghiệp vụ `expand_stock_items`, `release_stock_items` và `calc_expand_capacity_v0` để thu thập số lượng ca kiểm thử biên.
3. **Phân tích độ phức tạp McCabe:** Trích xuất đồ thị điều khiển CFG và chỉ số McCabe từ cây cú pháp trừu tượng AST.
4. **Giải bài toán SMT Z3 trên giao diện:** Nạp các biểu thức số học vào mô-đun SMT Z3 tích hợp để xác thực kết quả SAT/UNSAT trực quan.

### 4.8. Thực nghiệm kiểm toán động với Clang AddressSanitizer
Để có minh chứng không thể chối cãi về lỗi bộ nhớ tại thời gian chạy, em sử dụng trình biên dịch Clang (LLVM):
```bash
clang -fsanitize=address -g -O1 m2_cap_phat_heap_v0.c -o m2_v0_asan.exe
./m2_v0_asan.exe
```
Em ghi lại toàn bộ nhật ký lỗi crash, giải mã địa chỉ bộ nhớ vi phạm và phân tích cấu trúc byte bóng Shadow Memory `[fd]`. Sau đó, lặp lại quy trình với bản vá `m2_cap_phat_heap_v1.c` để xác thực trạng thái sạch bóng lỗi.

### 4.9. Thực nghiệm bộ giải SMT Z3 độc lập trên Python
Để chứng minh kết luận an toàn không phụ thuộc vào giao diện đồ họa của công cụ, em chạy độc lập tệp mã nguồn Python:
```bash
python 02_kiemchung_mohinh/kiem_tra_sat_unsat_m2.py
```
Script tự động khởi tạo các đối tượng `z3.Solver()`, áp dụng các mệnh đề ràng buộc toán học, in ra các mô hình phản ví dụ đối chứng và xuất bảng tổng hợp SAT/UNSAT.

### 4.10. Đối chiếu kết quả giữa các công cụ kiểm toán
Bước cuối cùng là lập bảng đối chiếu kết quả giữa 3 nguồn: `SecLabFramework` (Ch1, Ch2), bộ giải SMT Z3 Python và `AddressSanitizer`. Việc đối chiếu đa nguồn giúp loại trừ sai số do công cụ và khẳng định tính vững chắc của kết quả kiểm chứng.

---

## CHƯƠNG 5. KẾT QUẢ VÀ PHÂN TÍCH

### 5.1. Tám trạng thái M2-v0 và sáu trạng thái M2-v1 của mô hình Heap
*Bảng 5.1. Danh mục trạng thái, nhãn nguyên tử và ý nghĩa nghiệp vụ của Module M2*

| Tên trạng thái | Phiên bản | Tập nhãn nguyên tử ($L$) | Ý nghĩa nghiệp vụ trong quản lý kho |
| :--- | :---: | :--- | :--- |
| `m2_khoi_tao` | Cả 2 | `initialized` | Khởi tạo danh mục sản phẩm rỗng với 16 ô nhớ mặc định |
| `m2_danh_sach_day` | Cả 2 | `list_full` | Kho đã chứa đủ 16 sản phẩm, kích hoạt cơ chế mở rộng Heap |
| `m2_realloc_goi` | v0 | `reallocating` | Bản v0: Gọi `realloc()` gán trực tiếp vào con trỏ quản lý gốc |
| `m2_oom_null` | v0 | `oom_occurred` | Hệ thống hết RAM (OOM), hàm `realloc()` trả về con trỏ `NULL` |
| `m2_mat_con_tro_leak` | v0 | `memory_leak`, `cwe_401`, `violation` | **Trạng thái bẫy lỗi đỏ:** Mất dấu địa chỉ mảng cũ, rò rỉ bộ nhớ vĩnh viễn |
| `m2_realloc_tam` | v1 | `realloc_temp` | Bản v1: Gọi `realloc()` thông qua con trỏ trung gian tạm thời `tmp` |
| `m2_oom_bao_toan_ram` | v1 | `owner_preserved`, `safe_exit`, `done` | **Trạng thái an toàn v1:** `tmp == NULL`, bảo toàn con trỏ cũ, thoát an toàn |
| `m2_realloc_thanh_cong` | Cả 2 | `realloc_ok`, `owner_valid` | Tái cấp phát thành công, mảng kho hàng được nhân đôi dung lượng |
| `m2_giai_phong_xong` | v0 | `freed_without_null` | Bản v0: Gọi `free()` nhưng không gán con trỏ về `NULL` |
| `m2_doc_con_tro_treo_uaf` | v0 | `use_after_free`, `cwe_416`, `violation` | **Trạng thái bẫy lỗi đỏ:** Đọc dữ liệu từ con trỏ lơ lửng Use-After-Free |
| `m2_giai_phong_an_toan` | v1 | `pointer_nullified`, `zero_leak`, `done` | **Trạng thái an toàn v1:** Gọi `free()` và gán `ptr = NULL` ngay lập tức |

Mô hình không tồn tại trạng thái treo (Deadlock): Mọi trạng thái kết thúc đều có cạnh tự lặp, đảm bảo tính toàn phần của quan hệ chuyển $R$.

### 5.2. Sơ đồ trạng thái Kripke của mô hình M2 (v0 và v1)
Trên giao diện Tab Ch1 của công cụ `SecLabFramework`, đồ thị Kripke của bản lỗi M2-v0 hiển thị rõ hai nút bẫy vi phạm màu đỏ: `m2_mat_con_tro_leak` (ứng với sự cố OOM rò rỉ bộ nhớ CWE-401) và `m2_doc_con_tro_treo_uaf` (ứng với lỗi đọc con trỏ lơ lửng Use-After-Free CWE-416).

![Hình 4.1: Sơ đồ Kripke bản lỗi M2-v0 hiển thị trên SecLabFramework tab Ch1](../04_results/anh_chay_tool_thay/M2_V0_Ch1_SecLab_SoDo_Kripke_ToanManHinh_V0.png)
*Hình 4.1. Sơ đồ Kripke bản lỗi M2-v0 hiển thị trên SecLabFramework tab Ch1*

Ngược lại, trên mô hình bản vá an toàn M2-v1, toàn bộ các nút đỏ vi phạm đã được loại bỏ hoàn toàn. Thay vào đó, sơ đồ xuất hiện hai nút xanh lá cây an toàn: `m2_oom_bao_toan_ram` và `m2_giai_phong_an_toan`.

![Hình 4.4: Sơ đồ Kripke bản vá an toàn M2-v1 hiển thị trên SecLabFramework tab Ch1](../04_results/anh_chay_tool_thay/M2_V1_Ch1_SecLab_SoDo_Kripke_ToanManHinh_V1.png)
*Hình 4.4. Sơ đồ Kripke bản vá an toàn M2-v1 hiển thị trên SecLabFramework tab Ch1*

### 5.3. Quan hệ chuyển: Các cạnh bị cắt và cơ chế phòng thủ bản vá
*Bảng 5.2. Khác biệt quan hệ chuyển giữa bản lỗi M2-v0 (|R| = 9) và bản vá M2-v1 (|R| = 7)*

| Cạnh chuyển trạng thái | Tồn tại ở | Cơ chế phòng thủ kỹ thuật trong mã nguồn C bản vá v1 |
| :--- | :---: | :--- |
| `m2_realloc_goi ➔ m2_oom_null` | Chỉ v0 | Bị thay thế bằng cơ chế cấp phát qua con trỏ tạm `m2_realloc_tam` |
| `m2_oom_null ➔ m2_mat_con_tro_leak` | Chỉ v0 | **BỊ CẮT BỎ:** Không bao giờ gán đè `NULL` vào con trỏ quản lý gốc |
| `m2_mat_con_tro_leak ➔ m2_mat_con_tro_leak` | Chỉ v0 | **BỊ CẮT BỎ:** Triệt tiêu hoàn toàn chu trình bẫy rò rỉ bộ nhớ |
| `m2_giai_phong_xong ➔ m2_doc_con_tro_treo_uaf` | Chỉ v0 | **BỊ CẮT BỎ:** Không cho phép đọc dữ liệu khi con trỏ chưa được bảo vệ |
| `m2_doc_con_tro_treo_uaf ➔ m2_doc_con_tro_treo_uaf` | Chỉ v0 | **BỊ CẮT BỎ:** Triệt tiêu hoàn toàn chu trình bẫy Use-After-Free |
| `m2_realloc_tam ➔ m2_oom_bao_toan_ram` | Chỉ v1 | **CẠNH MỚI:** Khi `tmp == NULL`, giữ nguyên mảng cũ và thoát an toàn |
| `m2_oom_bao_toan_ram ➔ m2_oom_bao_toan_ram` | Chỉ v1 | **CẠNH MỚI:** Trạng thái kết thúc an toàn khi gặp sự cố cạn RAM OOM |
| `m2_realloc_thanh_cong ➔ m2_giai_phong_an_toan` | Chỉ v1 | **CẠNH MỚI:** Dọn dẹp tài nguyên và lập tức gán `list->items = NULL` |
| `m2_giai_phong_an_toan ➔ m2_giai_phong_an_toan` | Chỉ v1 | **CẠNH MỚI:** Trạng thái kết thúc sạch sẽ toàn diện (Zero Leak) |

### 5.4. Kết quả kiểm chứng sáu công thức CTL trên SecLabFramework
*Bảng 5.3. Kết quả kiểm chứng 6 công thức CTL trên mô hình M2*

| STT | Công thức CTL | Ý nghĩa an toàn bộ nhớ | Kết quả Bản v0 | Kết quả Bản v1 |
| :---: | :--- | :--- | :---: | :---: |
| 1 | `AG(!violation)` | Bất biến an toàn: Không bao giờ vi phạm an toàn bộ nhớ | **VI PHẠM (SAI)** | **ĐÚNG** |
| 2 | `AG(!memory_leak)` | Không rò rỉ mảng động khi OOM (CWE-401) | **VI PHẠM (SAI)** | **ĐÚNG** |
| 3 | `AG(!use_after_free)` | Không đọc con trỏ sau khi giải phóng (CWE-416) | **VI PHẠM (SAI)** | **ĐÚNG** |
| 4 | `EF(realloc_ok)` | Tính khả đạt: Có nhánh mở rộng mảng kho thành công | **ĐÚNG** | **ĐÚNG** |
| 5 | `AF(done)` | Tính sống: Mọi đường đi đều hoàn thành và đóng phiên | **VI PHẠM (SAI)** | **ĐÚNG** |
| 6 | `AG(!(cwe_401 & cwe_416))` | Loại trừ lẫn nhau: Hai lỗi nguy hiểm không đồng xuất hiện | **ĐÚNG** | **ĐÚNG** |

Trên Console của `SecLabFramework`, kết quả kiểm chứng CTL của bản lỗi M2-v0 chỉ ra sự vi phạm nghiêm trọng đối với các thuộc tính an toàn cốt lõi.

![Hình 4.2: Kết quả kiểm chứng CTL bản lỗi M2-v0 trên Console tool Thầy](../04_results/anh_chay_tool_thay/M2_V0_Ch1_SecLab_Console_CTL_KiemChung.png)
*Hình 4.2. Kết quả kiểm chứng CTL bản lỗi M2-v0 trên Console tool Thầy*

Đặc biệt, công thức tính sống `AF(done)` bị vi phạm trên bản v0 vì khi hệ thống rơi vào trạng thái bẫy `m2_mat_con_tro_leak` hoặc `m2_doc_con_tro_treo_uaf`, các cạnh tự lặp giữ tiến trình ở lại mãi mãi trong trạng thái lỗi mà không bao giờ chạm tới được nhãn `done`.

Ngược lại, trên bản vá M2-v1, toàn bộ 6/6 công thức CTL đều được công cụ đánh giá là **ĐÚNG (THỎA MÃN)**.

![Hình 4.5: Kết quả kiểm chứng CTL bản vá M2-v1 trên Console tool Thầy](../04_results/anh_chay_tool_thay/M2_V1_Ch1_SecLab_Console_CTL_KiemChung.png)
*Hình 4.5. Kết quả kiểm chứng CTL bản vá M2-v1 trên Console tool Thầy*

### 5.5. Thuộc tính loại trừ lẫn nhau và kiểm chứng bất biến an toàn
Kiểm tra tính loại trừ lẫn nhau (Mutual Exclusion) giữa cặp nhãn vi phạm `memory_leak` và `use_after_free` trên bản lỗi M2-v0 cho thấy hệ thống phân lập được hai hành vi lỗi trên hai nhánh rẽ riêng biệt: một lỗi xảy ra ở pha cấp phát mở rộng (H3), lỗi còn lại xảy ra ở pha thu hồi dọn dẹp (H6).

![Hình 4.3a: Kiểm tra thuộc tính loại trừ lẫn nhau M2-v0 trên SecLabFramework - Phần 1](../04_results/anh_chay_tool_thay/M2_V0_Ch1_SecLab_Console_Mutual_Exclusion_1.png)
*Hình 4.3a. Kiểm tra thuộc tính loại trừ lẫn nhau M2-v0 trên SecLabFramework - Phần 1*

![Hình 4.3b: Kiểm tra thuộc tính loại trừ lẫn nhau M2-v0 trên SecLabFramework - Chi tiết vết vi phạm](../04_results/anh_chay_tool_thay/M2_V0_Ch1_SecLab_Console_Mutual_Exclusion_2.png)
*Hình 4.3b. Kiểm tra thuộc tính loại trừ lẫn nhau M2-v0 trên SecLabFramework - Chi tiết vết vi phạm*

Trên bản vá M2-v1, tính năng kiểm tra loại trừ lẫn nhau xác nhận tính an toàn tuyệt đối khi không còn bất kỳ trạng thái nào vi phạm điều kiện an toàn.

![Hình 4.6a: Kiểm tra thuộc tính loại trừ lẫn nhau M2-v1 đạt an toàn trên SecLabFramework - Phần 1](../04_results/anh_chay_tool_thay/M2_V1_Ch1_SecLab_Console_Mutual_Exclusion_1.png)
*Hình 4.6a. Kiểm tra thuộc tính loại trừ lẫn nhau M2-v1 đạt an toàn trên SecLabFramework - Phần 1*

![Hình 4.6b: Kiểm tra thuộc tính loại trừ lẫn nhau M2-v1 đạt an toàn trên SecLabFramework - Kết luận an toàn](../04_results/anh_chay_tool_thay/M2_V1_Ch1_SecLab_Console_Mutual_Exclusion_2.png)
*Hình 4.6b. Kiểm tra thuộc tính loại trừ lẫn nhau M2-v1 đạt an toàn trên SecLabFramework - Kết luận an toàn*

### 5.6. Phân tích kịch bản phản ví dụ thành tình huống thực tế
1. **Kịch bản phản ví dụ 1: Rò rỉ bộ nhớ khi cạn kiệt RAM (OOM Memory Leak - CWE-401):**
   - **Đường đi phản ví dụ:** `m2_khoi_tao` ➔ `m2_danh_sach_day` ➔ `m2_realloc_goi` ➔ `m2_oom_null` ➔ `m2_mat_con_tro_leak`.
   - **Tình huống nghiệp vụ thực tế:** Kho hàng đang vận hành bình thường với 16 mặt hàng ban đầu. Đúng thời điểm khuyến mại cao điểm, nhân viên quét mã nhập kiện hàng thứ 17. Chương trình gọi hàm `realloc_buffer_leak_v0()` để nâng sức chứa lên 32 sản phẩm. Tuy nhiên, máy chủ logistics cùng lúc bị chiếm dụng bộ nhớ bởi các dịch vụ khác dẫn tới cạn RAM. Hàm `realloc()` trả về `NULL`. Vì lập trình viên viết lệnh gán thẳng `buf = realloc(buf, new_size)`, con trỏ `buf` bị gán bằng `NULL`. Toàn bộ 16 sản phẩm cũ (trị giá hàng trăm triệu đồng) vẫn nằm trên RAM nhưng mất hoàn toàn con trỏ quản lý. Bộ nhớ của tiến trình phình to liên tục sau mỗi đợt OOM, cuối cùng hệ điều hành buộc phải kích hoạt cơ chế OOM Killer để tiêu diệt tiến trình kho hàng.
2. **Kịch bản phản ví dụ 2: Con trỏ lơ lửng Use-After-Free (CWE-416):**
   - **Đường đi phản ví dụ:** `m2_khoi_tao` ➔ `m2_danh_sach_day` ➔ `m2_realloc_goi` ➔ `m2_realloc_thanh_cong` ➔ `m2_giai_phong_xong` ➔ `m2_doc_con_tro_treo_uaf`.
   - **Tình huống nghiệp vụ thực tế:** Khi kết thúc ca làm việc, hàm `cleanup_and_read_uaf_v0()` thực thi lệnh `free(ptr)` để trả lại vùng nhớ chứa bản ghi sản phẩm cho hệ điều hành. Tuy nhiên, lập trình viên không gán `ptr = NULL`. Ngay sau đó, hàm tiếp tục thực hiện câu lệnh `return *ptr;` để lấy giá trị kiểm kê. Lúc này, ô nhớ đó đã được trình quản lý Heap đưa vào danh sách giải phóng (Freelist) và có thể đã bị luồng khác ghi đè thông tin xác thực phiên làm việc. Việc đọc dữ liệu dẫn tới thông tin kho hàng bị sai lệch hoàn toàn; nguy hiểm hơn, nếu kẻ tấn công thao túng được vùng nhớ Heap này, chúng có thể điều hướng con trỏ để thực thi mã tùy ý.

### 5.7. Kết quả bốn bài toán kiểm tra SMT Z3 (SAT trên v0 và UNSAT trên v1)
*Bảng 5.4. Bảng tổng hợp đối chứng hình thức SMT Z3 trên 4 bài toán kiểm toán M2*

| Mã kiểm tra | Tên bài toán kiểm toán | Biểu thức toán học vi phạm | Bản v0 (Kỳ vọng SAT) | Bản vá v1 (Kỳ vọng UNSAT) |
| :---: | :--- | :--- | :---: | :---: |
| **M2-CHECK-01** | Tràn số cấp phát Heap (`heap_alloc_bytes`) | `new_capacity * 48 > 2147483647` | **SAT** (Mô hình: `x = 44739243`) | **UNSAT** (Chứng minh an toàn 100%) |
| **M2-CHECK-02** | Tràn số nhân đôi dung lượng (`calc_expand_capacity`) | `current_capacity * 2 > 2147483647` | **SAT** (Mô hình: `x = -2147483648`) | **UNSAT** (Chứng minh an toàn 100%) |
| **M2-CHECK-03** | Rò rỉ con trỏ khi OOM (`realloc_buffer_leak`) | `old_ptr > 0 and list_ptr == 0` | **SAT** (Mô hình: `old=1, list=0`) | **UNSAT** (Chứng minh an toàn 100%) |
| **M2-CHECK-04** | Đọc con trỏ sau free (`cleanup_and_read_uaf`) | `ptr_state == 2 and read_op == 1` | **SAT** (Mô hình: `state=2, op=1`) | **UNSAT** (Chứng minh an toàn 100%) |

Trên giao diện tab Ch2 của `SecLabFramework`, em thực hiện giải bài toán SMT Z3 cho kịch bản tràn số cấp phát Heap. Z3 ngay lập tức tìm ra nghiệm SAT với `x = 44739243`.

![Hình 5.9: Giải bài toán SMT Z3 Heap Alloc Overflow trên giao diện SecLab Ch2](../04_results/anh_chay_tool_thay/M2_V0_Ch2_SecLab_SMT_Z3_SAT_HeapAlloc_Overflow.png)
*Hình 5.9. Giải bài toán SMT Z3 Heap Alloc Overflow trên giao diện SecLab Ch2*

Tiếp theo, đối với bài toán nhân đôi kích thước kho hàng `current_capacity * 2`, Z3 giải ra nghiệm SAT chứng minh hiện tượng tràn số nguyên lật bit dấu sang âm.

![Hình 5.10: Giải bài toán SMT Z3 tràn số nguyên khi nhân đôi Capacity trên SecLab Ch2](../04_results/anh_chay_tool_thay/M2_V0_Ch2_SecLab_SMT_Z3_Overflow_Int32_Capacity_Double.png)
*Hình 5.10. Giải bài toán SMT Z3 tràn số nguyên khi nhân đôi Capacity trên SecLab Ch2*

Khi thực thi script Python độc lập `02_kiemchung_mohinh/kiem_tra_sat_unsat_m2.py`, kết quả hiển thị chi tiết vết phản ví dụ cho từng bài toán trên bản v0 và xác nhận tính toán học UNSAT trên bản v1.

![Hình 5.11: Chi tiết kết quả chạy 4 kịch bản SMT Z3 SAT/UNSAT trên Terminal bằng Python](../04_results/anh_chay_tool_thay/M2_V0_V1_Terminal_SMT_Z3_SAT_UNSAT_ChiTiet.png)
*Hình 5.11. Chi tiết kết quả chạy 4 kịch bản SMT Z3 SAT/UNSAT trên Terminal bằng Python*

Bảng tổng hợp Terminal xác nhận 100% kịch bản trên v0 đều đạt SAT và 100% kịch bản trên v1 đều đạt UNSAT.

![Hình 5.12: Bảng tổng hợp đối chiếu kết quả SAT trên v0 và UNSAT trên v1 trên Terminal](../04_results/anh_chay_tool_thay/M2_V0_V1_Terminal_SMT_Z3_SAT_UNSAT_BangTongHop.png)
*Hình 5.12. Bảng tổng hợp đối chiếu kết quả SAT trên v0 và UNSAT trên v1 trên Terminal*

### 5.8. Kết quả phân tích tĩnh mã nguồn C trên SecLab Ch2
Khi nạp mã nguồn C `m2_cap_phat_heap_v0.c` vào tab Ch2 của `SecLabFramework`, trình phân tích tĩnh tự động quét cây cú pháp AST và phát hiện 4 cảnh báo an toàn nghiêm trọng.

![Hình 5.1: Kết quả phân tích tĩnh mã C M2-v0 trên SecLabFramework tab Ch2](../04_results/anh_chay_tool_thay/M2_V0_Ch2_SecLab_Static_Scan_KetQua.png)
*Hình 5.1. Kết quả phân tích tĩnh mã C M2-v0 trên SecLabFramework tab Ch2*

Chi tiết cảnh báo chỉ rõ hai vị trí lỗi nghiêm trọng:
1. Hàm `expand_stock_items`: Cảnh báo **CWE-401 (Memory Leak)** do thực hiện `malloc(sizeof(int) * 16)` nhưng thoát hàm mà không gọi `free()`.
2. Hàm `release_stock_items`: Cảnh báo **CWE-416 (Use-After-Free)** do sau lệnh `free(buf)` tại dòng 31, dòng 32 tiếp tục thực hiện gán `buf[0] = 1`.

![Hình 5.2: Chi tiết cảnh báo phân tích tĩnh CWE-401 (Memory Leak) và CWE-416 (Use-After-Free) trên M2-v0](../04_results/anh_chay_tool_thay/M2_V0_Ch2_SecLab_Static_CWE401_CWE416_ChiTiet.png)
*Hình 5.2. Chi tiết cảnh báo phân tích tĩnh CWE-401 (Memory Leak) và CWE-416 (Use-After-Free) trên M2-v0*

Khi nạp bản vá an toàn `m2_cap_phat_heap_v1.c`, công cụ quét toàn diện và thông báo: **"0 lỗi con trỏ - Mã nguồn an toàn tuyệt đối"**.

![Hình 5.3: Phân tích tĩnh bản vá M2-v1 đạt an toàn, quét lỗi con trỏ 0 lỗi trên SecLab Ch2](../04_results/anh_chay_tool_thay/M2_V1_Ch2_SecLab_QuetLoiConTro_0Loi_AnToan.png)
*Hình 5.3. Phân tích tĩnh bản vá M2-v1 đạt an toàn, quét lỗi con trỏ 0 lỗi trên SecLab Ch2*

Bên cạnh đó, công cụ thực hiện phân tích cấu trúc bộ nhớ (Struct Alignment), xác nhận các hàm độc lập của M2 được tối ưu hóa không gian lưu trữ mà không phát sinh hiện tượng đệm lãng phí (Padding).

![Hình 5.4: Phân tích cấu trúc Struct Alignment cho M2-v1 trên SecLab Ch2](../04_results/anh_chay_tool_thay/M2_V1_Ch2_SecLab_PhanTich_StructAlignment_KhongStruct.png)
*Hình 5.4. Phân tích cấu trúc Struct Alignment cho M2-v1 trên SecLab Ch2*

### 5.9. Kết quả chiến dịch Fuzzing và phân tích AST độ phức tạp McCabe
Em kích hoạt chiến dịch Fuzzing trên tab Ch2 đối với các hàm trọng yếu của Module M2:
* Hàm `expand_stock_items()`: Fuzzer thực thi 13 đầu vào theo phương pháp Coverage-guided, đạt độ bao phủ nhánh 100% (3/3 probe).

![Hình 5.5: Chiến dịch Fuzzing hàm expand_stock_items trên SecLab Ch2](../04_results/anh_chay_tool_thay/M2_V0_Ch2_SecLab_Fuzzing_ExpandStockItems.png)
*Hình 5.5. Chiến dịch Fuzzing hàm expand_stock_items trên SecLab Ch2*

* Hàm `release_stock_items()`: Fuzzer hoàn thành kiểm thử bao phủ toàn bộ chu trình giải phóng và ghi đè con trỏ.

![Hình 5.6: Chiến dịch Fuzzing hàm release_stock_items trên SecLab Ch2](../04_results/anh_chay_tool_thay/M2_V0_Ch2_SecLab_Fuzzing_ReleaseStockItems.png)
*Hình 5.6. Chiến dịch Fuzzing hàm release_stock_items trên SecLab Ch2*

* Hàm `calc_expand_capacity_v0()`: Fuzzer tự động sinh 14 ca kiểm thử biên (Boundary tests). Tại giá trị biên cực đại của số nguyên, Fuzzer phát hiện ngay hiện tượng tràn số nguyên **CWE-190** khiến kết quả dung lượng trả về mang giá trị âm hoặc vượt ngưỡng kiểm soát.

![Hình 5.7: Fuzzing phát hiện lỗi tràn số nguyên CWE-190 trong hàm calc_expand_capacity_v0](../04_results/anh_chay_tool_thay/M2_V0_Ch2_SecLab_Fuzzing_CalcExpandCapacity_CWE190.png)
*Hình 5.7. Fuzzing phát hiện lỗi tràn số nguyên CWE-190 trong hàm calc_expand_capacity_v0*

Phân tích cú pháp AST trên `m2_cap_phat_heap_v0.c` xác định hàm có **Độ phức tạp chu trình McCabe = 7**, thể hiện số lượng nhánh rẽ điều kiện và các kịch bản ngoại lệ OOM cần được bao phủ đầy đủ bằng các ca kiểm thử.

![Hình 5.8: Phân tích cây cú pháp trừu tượng AST và độ phức tạp chu trình McCabe = 7 cho M2-v0](../04_results/anh_chay_tool_thay/M2_V0_Ch2_SecLab_AST_DoPhucTap_McCabe_7.png)
*Hình 5.8. Phân tích cây cú pháp trừu tượng AST và độ phức tạp chu trình McCabe = 7 cho M2-v0*

### 5.10. Kết quả kiểm toán động với AddressSanitizer (Shadow Bytes)
Minh chứng thực nghiệm thuyết phục nhất đối với lỗi an toàn bộ nhớ tại thời gian chạy là báo cáo chẩn đoán của trình biên dịch Clang tích hợp AddressSanitizer trên bản lỗi `m2_cap_phat_heap_v0.c`.

![Hình 5.13: AddressSanitizer bắt trọn lỗi Heap-Use-After-Free tại thời gian chạy trên bản M2-v0](../04_results/anh_chay_tool_thay/M2_V0_Terminal_ASan_Heap_Use_After_Free.png)
*Hình 5.13. AddressSanitizer bắt trọn lỗi Heap-Use-After-Free tại thời gian chạy trên bản M2-v0*

**Giải mã nhật ký chẩn đoán của AddressSanitizer:**
* **Thông điệp lỗi:** `ERROR: AddressSanitizer: heap-use-after-free on address 0x120800000040 pc 0x00412a80 bp 0x00a0f7ec sp 0x00a0f7e4`
* **Loại truy cập vi phạm:** `READ of size 4 at 0x120800000040 thread T0` tại dòng 78 của `m2_cap_phat_heap_v0.c` trong hàm `cleanup_and_read_uaf_v0`.
* **Ngăn xếp giải phóng ô nhớ (Freed trace):** Ô nhớ được giải phóng tại dòng 76 bởi lời gọi `free(ptr)`.
* **Ngăn xếp cấp phát ban đầu (Allocated trace):** Ô nhớ được cấp phát tại dòng 73 bởi lời gọi `malloc(sizeof(int))`.
* **Bản đồ Shadow Bytes xung quanh địa chỉ lỗi:**
  ```
  =>0x120800000040: [fd] fd fd fd fd fd fd fd
  ```
  Ký hiệu `[fd]` (Freed Heap Region) là bằng chứng không thể chối cãi: ô nhớ 4 byte đã được thu hồi và đánh dấu là vùng nhớ chết, nhưng mã nguồn v0 vẫn cố tình thực hiện thao tác đọc giá trị.

Ngược lại, khi biên dịch và thực thi bản vá an toàn `m2_cap_phat_heap_v1.c`, chương trình vượt qua toàn bộ các ca kiểm tra khắt khe, ASan báo cáo **0 errors detected** và toàn bộ tài nguyên Heap được giải phóng trọn vẹn.

![Hình 5.14: AddressSanitizer chạy bản vá M2-v1 thành công tuyệt đối, 0 crash, clean memory](../04_results/anh_chay_tool_thay/TongHop_V1_Terminal_ASan_Safe_Clean_Run.png)
*Hình 5.14. AddressSanitizer chạy bản vá M2-v1 thành công tuyệt đối, 0 crash, clean memory*

### 5.11. Đối chiếu kết quả toàn diện giữa các nguồn kiểm chứng
*Bảng 5.5. Đối chiếu kết quả kiểm toán giữa SecLabFramework, Terminal Z3 và Clang AddressSanitizer*

| Nội dung kiểm toán an toàn | SecLabFramework (Ch1 & Ch2) | Bộ giải SMT Z3 độc lập (Python) | Trình kiểm toán động Clang ASan |
| :--- | :--- | :--- | :--- |
| **Kiểm chứng Kripke / CTL (v0)** | Vi phạm $AG(!violation)$, $AG(!leak)$, $AG(!uaf)$, $AF(done)$ | Nhất quán với mô hình phản ví dụ | Khớp với hiện tượng lỗi runtime |
| **Kiểm chứng Kripke / CTL (v1)** | Đạt 100% (6/6) công thức CTL an toàn | Nhất quán với chứng minh UNSAT | Thực thi an toàn 100%, 0 crash |
| **Lỗi tràn số Heap (CWE-190)** | Z3 Ch2 báo SAT (`x = 44739243`) | M2-CHECK-01 báo SAT (`x = 44739243`) | Fuzzing Ch2 sinh 14 test bắt lỗi biên |
| **Lỗi rò rỉ bộ nhớ OOM (CWE-401)** | Quét tĩnh bắt lỗi tại `expand_stock_items` | M2-CHECK-03 báo SAT (`list_ptr == 0`) | v1 dùng con trỏ tạm bảo toàn RAM |
| **Lỗi Use-After-Free (CWE-416)** | Quét tĩnh bắt lỗi tại `release_stock_items` | M2-CHECK-04 báo SAT (`state = 2, read = 1`) | ASan bắt trọn tại dòng 78 với byte `[fd]` |
| **Độ tin cậy của bản vá v1** | Báo sạch 0 lỗi con trỏ | 4/4 bài toán đạt UNSAT tuyệt đối | ASan xác nhận 0 errors, clean memory |

Sự trùng khớp tuyệt đối giữa ba nguồn công cụ độc lập chứng minh rằng các phân tích của em là hoàn toàn chính xác, khoa học và phản ánh đúng bản chất kỹ thuật của hệ thống.

---

## CHƯƠNG 6. PHỐI HỢP TRONG NHÓM

### 6.1. Nhận đầu vào từ Module M1 (Nguyễn Quang Thọ)
*Bảng 6.1. Đầu vào nhận từ các thành viên khác trong nhóm*

| Thành viên bàn giao | Sản phẩm đầu vào | Mục đích sử dụng trong Module M2 |
| :--- | :--- | :--- |
| **Nguyễn Quang Thọ (M1)** | Chuỗi ký tự mã phiếu và mảng token sản phẩm hợp lệ trên Stack sau khi đã bóc tách an toàn ở H2 | Dùng làm dữ liệu đầu vào để nạp vào các cấu trúc `Product` khi cấp phát động trên Heap tại H3 |
| **Nguyễn Quang Thọ (M1)** | Định dạng số lượng và đơn giá dạng chuỗi đã được chuẩn hóa | Đảm bảo kích thước chuỗi không vượt quá trường lưu trữ trong struct `Product` |

### 6.2. Bàn giao đầu ra cho Module M3 (Hà Văn Đô)
*Bảng 6.2. Đầu ra bàn giao cho các thành viên khác trong nhóm*

| Thành viên tiếp nhận | Sản phẩm bàn giao | Mục đích sử dụng trong Module M3 |
| :--- | :--- | :--- |
| **Hà Văn Đô (M3)** | Con trỏ mảng động `Product *items` hợp lệ và biến đếm số lượng `item_count` trên Heap | Cung cấp danh mục hàng hóa đã xác thực an toàn để M3 thực hiện tính toán tồn kho (H4) và tài sản (H5) |
| **Hà Văn Đô (M3)** | Trạng thái bảo vệ con trỏ sau khi M3 tính toán xong | M2 tiếp nhận lại quyền sở hữu bộ nhớ để thực hiện giải phóng tài nguyên an toàn tại H6 |

### 6.3. Rà soát chéo ma trận ca kiểm thử và mã nguồn
Trong quá trình triển khai, em chủ trì việc rà soát chéo ma trận 28 ca kiểm thử của toàn nhóm. Em trực tiếp kiểm tra:
* Ca kiểm thử của bạn Thọ (M1): Đảm bảo các chuỗi kiểm thử quá khổ (Buffer Overflow) được chặn lại ngay tại Stack trước khi chạm tới hàm `malloc()` của M2.
* Ca kiểm thử của bạn Đô (M3): Đảm bảo các giá trị tính toán số lượng tồn kho âm hoặc tràn số nguyên không làm sai lệch con trỏ chỉ số mảng của M2.

### 6.4. Phương thức quản lý dự án và điều hành nhóm của Trưởng nhóm
Với vai trò Trưởng nhóm, em áp dụng mô hình cộng tác chặt chẽ dựa trên hệ thống quản lý mã nguồn Git:
1. **Thiết lập chuẩn mực dự án:** Ban hành quy chuẩn đặt tên file, quy chuẩn đặt nhãn Kripke và cấu trúc thư mục nộp bài thống nhất.
2. **Quản lý phiên bản trên GitHub:** Mọi thay đổi về đặc tả và mã nguồn C đều được commit và tạo Pull Request với thông điệp rõ ràng trên kho lưu trữ `ATPM_Group_04_inventory-memory-audit`.
3. **Điều phối tiến độ theo mốc học phần:** Phân chia công việc theo từng giai đoạn (Đặc tả ➔ Mô hình hóa Kripke Ch1 ➔ Kiểm toán mã C Ch2 ➔ AddressSanitizer & Z3 ➔ Đóng gói báo cáo), tổ chức họp nhóm định kỳ để tháo gỡ vướng mắc kỹ thuật.

---

## CHƯƠNG 7. KHÓ KHĂN VÀ HẠN CHẾ

### 7.1. Các vấn đề kỹ thuật gặp phải và giải pháp xử lý
*Bảng 7.1. Tổng hợp các vấn đề kỹ thuật phát sinh và giải pháp xử lý*

| STT | Vấn đề kỹ thuật phát sinh | Hậu quả nếu không xử lý | Giải pháp kỹ thuật đã áp dụng |
| :---: | :--- | :--- | :--- |
| 1 | Bộ phân tích cú pháp SMT của tool không nhận số âm trực tiếp dạng `-2147483648` | Gây lỗi cú pháp `UnaryOp` khiến pipeline kiểm toán bị dừng đột ngột | Chuyển đổi biểu thức số học sang dạng tương đương: `current_capacity + 2147483648 >= 0` |
| 2 | Hiện tượng Fuzzer bị TIMEOUT khi kiểm thử hàm `realloc` với tham số kích thước âm | Kích thước âm bị ép kiểu ngầm định sang `size_t` không dấu khổng lồ gây treo | Bổ sung rào chắn cận dưới và cận trên: `if (size <= 0 \|\| size > MAX_SAFE_CAPACITY) return -1;` |
| 3 | Lỗi biên dịch Clang ASan trên Windows do xung đột thư viện runtime | Trình biên dịch báo thiếu `clang_rt.asan_dynamic-i386.lib` | Cấu hình đúng đường dẫn môi trường LLVM MSVC và biên dịch ở chế độ 32-bit x86 chuẩn |
| 4 | Trạng thái bẫy vi phạm gây lỗi treo vô hạn khi tính toán điểm bất động | Công cụ báo Deadlock do trạng thái lỗi không có cạnh đi tiếp | Bổ sung cạnh tự lặp (Self-loop) tại các trạng thái lỗi bẫy để thỏa mãn tính toàn phần của $R$ |

### 7.2. Khó khăn cá nhân trong quá trình nghiên cứu và thực nghiệm
Trong quá trình thực hiện bài tập lớn, khó khăn lớn nhất của em là việc kết nối giữa lý thuyết toán học trừu tượng (Cấu trúc Kripke, logic CTL, đại số SMT) với hành vi vật lý thực tế của thanh ghi và ô nhớ trong máy tính. Ban đầu, khi nhìn vào công thức $AG(!use_after_free)$, em chỉ hiểu đơn thuần là một phép kiểm tra nhãn logic trên đồ thị. 

Tuy nhiên, khi trực tiếp viết mã C, cấu hình trình biên dịch Clang và nhìn thấy nhật ký AddressSanitizer chỉ điểm chính xác byte bóng `[fd]` tại địa chỉ ô nhớ `0x120800000040` ở dòng lệnh thứ 78, em mới thực sự thấu hiểu sâu sắc cơ chế vận hành của bộ nhớ máy tính. Để vượt qua rào cản này, em đã dành nhiều ngày nghiên cứu tài liệu kỹ thuật của dự án LLVM, trao đổi với Giảng viên hướng dẫn và kiên trì thực nghiệm hàng chục lần trên Terminal để ghi lại các bằng chứng xác thực nhất.

### 7.3. Hạn chế của phần việc và phạm vi nghiên cứu
* Mô hình Kripke của Module M2 vẫn là một sự trừu tượng hóa mức cao: mỗi trạng thái đại diện cho một mốc logic của vòng đời Heap chứ chưa mô hình hóa chi tiết đến từng byte bộ nhớ của Allocator (như cấu trúc Chunk Header, Bin, Fastbin trong `ptmalloc`).
* Bộ giải SMT Z3 trong sheet `KiemTraSoHoc` hoạt động dựa trên miền số nguyên toán học vô hạn, do đó việc chứng minh an toàn ở bản v1 dựa trên giả định các phép toán nhân kích thước được kiểm tra trước khi chuyển đổi sang kiểu `size_t`.
* Mô hình Use-After-Free mới tập trung kiểm toán trên luồng đơn (Single-thread), chưa mở rộng kiểm toán tranh chấp truy cập con trỏ giữa nhiều luồng song song (Race Condition / Multithreading UAF).

---

## CHƯƠNG 8. ĐÁNH GIÁ KẾT QUẢ VÀ BÀI HỌC KINH NGHIỆM

### 8.1. Đối chiếu kết quả với các mục tiêu đề ra
*Bảng 8.1. Đối chiếu kết quả thực nghiệm với các mục tiêu đề ra*

| Mục tiêu ban đầu (Mục 1.3) | Kết quả thực nghiệm đạt được | Mức độ hoàn thành |
| :--- | :--- | :---: |
| 1. Mô hình Kripke M2 | Dựng thành công 8 trạng thái v0 (|R|=9) và 6 trạng thái v1 (|R|=7); 0 deadlock (Mục 5.1, 5.2) | **ĐẠT 100%** |
| 2. Kiểm chứng 6 công thức CTL | Bản v0 vi phạm 4/6 công thức; Bản v1 đạt 6/6 công thức an toàn và tính sống (Mục 5.4) | **ĐẠT 100%** |
| 3. Bốn bài toán SMT Z3 | Bản v0 đạt 4/4 SAT kèm mô hình phản ví dụ; Bản v1 đạt 4/4 UNSAT tuyệt đối (Mục 5.7) | **ĐẠT 100%** |
| 4. Kiểm toán C & AddressSanitizer | Bắt đúng CWE-401/416 trên SecLab Ch2; ASan kích hoạt lỗi UAF tại dòng 78 với byte `[fd]` (Mục 5.8, 5.10) | **ĐẠT 100%** |
| 5. Đối chiếu đa nguồn | Kết quả đồng thuận 100% giữa SecLabFramework, Terminal Z3 và Clang ASan (Mục 5.11) | **ĐẠT 100%** |

### 8.2. Bài học rút ra về kiểm toán an toàn phần mềm
1. **Kiểm tra biên (Boundary Checks) là nguyên tắc sống còn:** Một phép tính toán kích thước tưởng chừng vô hại như `capacity * sizeof(Item)` có thể trở thành vũ khí phá hoại hệ thống nếu không được chặn trên bởi ngưỡng an toàn `MAX_SAFE_CAPACITY`.
2. **Quy tắc sở hữu con trỏ và Safe Reallocation:** Tuyệt đối không bao giờ gán thẳng kết quả của `realloc()` vào con trỏ duy nhất đang quản lý vùng nhớ. Luôn sử dụng biến tạm trung gian để bảo toàn tài nguyên khi xảy ra sự cố OOM.
3. **Pointer Nullification sau khi giải phóng:** Lệnh `free(ptr)` phải luôn đi kèm với `ptr = NULL` ngay dòng tiếp theo. Đây là rào chắn phòng thủ đơn giản nhưng có khả năng triệt tiêu 100% lớp lỗ hổng Use-After-Free cực kỳ nguy hiểm.
4. **Sự kết hợp giữa Hình thức và Thực nghiệm:** Kiểm chứng hình thức Kripke/CTL/Z3 mang lại sự đảm bảo toán học ở mức thiết kế, trong khi công cụ động như AddressSanitizer mang lại bằng chứng thực tế tại thời gian chạy. Việc kết hợp cả hai tạo nên quy trình kiểm toán an toàn phần mềm toàn diện và đáng tin cậy nhất.

### 8.3. Hướng phát triển và hoàn thiện tiếp theo
* Mở rộng mô hình Kripke mức vi mô: Mô hình hóa trạng thái từng ô nhớ Heap dưới dạng mảng Bit-vector trong Z3 để kiểm chứng hành vi của bộ cấp phát khi bị phân mảnh (Heap Fragmentation).
* Tích hợp công cụ ThreadSanitizer (TSan) để kiểm toán rủi ro tranh chấp con trỏ Heap trong môi trường đa luồng nghiệp vụ.
* Ứng dụng kiểm chứng tự động vào đường ống tích hợp liên tục (CI/CD Pipeline) bằng cách tự động chạy script Z3 và ASan sau mỗi lần commit mã nguồn C.

### 8.4. Tự đánh giá mức độ đóng góp cá nhân
Theo nguyên tắc làm việc và thỏa thuận phân công của Nhóm 04, **khối lượng công việc được chia đều 33.33% cho mỗi thành viên (chia đều 1/3 cho ba bạn Thọ, Trang, Đô)**. Cả ba thành viên đều độc lập hoàn thành 100% trách nhiệm đối với module của mình từ đặc tả hình thức Kripke/CTL/Z3, viết mã C đến thực nghiệm kiểm toán động với AddressSanitizer, đồng thời phối hợp chặt chẽ trong giai đoạn tích hợp hệ thống. Bản thân em hoàn thành đầy đủ 100% nhiệm vụ của Module M2 và thực hiện trách nhiệm Trưởng nhóm điều phối chung.

---

## CAM KẾT VÀ GHI CHÚ CÔNG CỤ HỖ TRỢ

Em xin cam đoan toàn bộ các số liệu, vết phản ví dụ, địa chỉ bộ nhớ và ảnh chụp minh chứng trong báo cáo cá nhân này là kết quả thực nghiệm trung thực, khách quan của bản thân em trên công cụ `SecLabFramework` của Thầy và môi trường kiểm toán độc lập. Toàn bộ mã nguồn, tệp đặc tả và nhật ký log đều được lưu trữ đầy đủ trong thư mục dự án và kho lưu trữ GitHub của nhóm.

Về việc sử dụng công cụ hỗ trợ: Em có sử dụng mô hình trí tuệ nhân tạo (Claude / ChatGPT) với vai trò là trợ lý kỹ thuật để hỗ trợ rà soát cú pháp biểu thức SMT Z3, tra cứu mã định danh điểm yếu CWE và hỗ trợ định dạng trình bày văn bản theo chuẩn Markdown/Word. Toàn bộ việc thiết kế mô hình Kripke, viết mã C, cấu hình tham số kiểm thử và phân tích kết quả chuyên môn đều do em tự nghiên cứu, kiểm chứng và chịu trách nhiệm hoàn toàn về nội dung.

*Hà Nội, ngày 27 tháng 10 năm 2026*  
**Sinh viên thực hiện**  
*(Ký và ghi rõ họ tên)*  
**Nguyễn Kiều Trang**

---

## TÀI LIỆU THAM KHẢO

1. **Vũ Quang Dũng**, *Giáo trình An toàn phần mềm (CSE703093)*, Khoa Công nghệ Thông tin, Trường Đại học Phenikaa, 2026.
2. **Vũ Quang Dũng**, *Tài liệu hướng dẫn Bài tập lớn và Hướng dẫn sử dụng bộ công cụ SecLabFramework / SpecVerificationLab*, Đại học Phenikaa, 2026.
3. **Nhóm 04**, *Báo cáo tổng hợp Bài tập lớn: Kiểm toán an toàn bộ nhớ cho chương trình C mô phỏng nghiệp vụ quản lý kho hàng*, Học phần An toàn phần mềm, Đại học Phenikaa, 10/2026.
4. **E. M. Clarke, O. Grumberg, D. A. Peled**, *Model Checking*, MIT Press, 1999.
5. **L. de Moura, N. Bjørner**, *Z3: An Efficient SMT Solver*, Tools and Algorithms for the Construction and Analysis of Systems (TACAS), Springer, 2008.
6. **K. Serebryany, D. Bruening, A. Potapenko, D. Vyukov**, *AddressSanitizer: A Fast Address Sanity Checker*, USENIX Annual Technical Conference (ATC), 2012.
7. **MITRE Corporation**, *Common Weakness Enumeration (CWE) - CWE-190, CWE-401, CWE-416*, https://cwe.mitre.org/.

---

## PHỤ LỤC

### Phụ lục A. Danh mục file minh chứng thực nghiệm
*Bảng A.1. Danh sách 21 tệp ảnh minh chứng và nhật ký thực nghiệm của phần việc*

| Tên tệp ảnh / nhật ký minh chứng | Nội dung minh chứng kỹ thuật |
| :--- | :--- |
| `M2_V0_Ch1_SecLab_SoDo_Kripke_ToanManHinh_V0.png` | Sơ đồ Kripke bản lỗi M2-v0 trên SecLabFramework Ch1 (Hình 4.1) |
| `M2_V0_Ch1_SecLab_Console_CTL_KiemChung.png` | Console kiểm chứng CTL bản v0 vi phạm 4 thuộc tính (Hình 4.2) |
| `M2_V0_Ch1_SecLab_Console_Mutual_Exclusion_1.png` | Console kiểm tra Mutual Exclusion trên bản v0 (Hình 4.3) |
| `M2_V1_Ch1_SecLab_SoDo_Kripke_ToanManHinh_V1.png` | Sơ đồ Kripke bản vá M2-v1 sạch nút vi phạm (Hình 4.4) |
| `M2_V1_Ch1_SecLab_Console_CTL_KiemChung.png` | Console kiểm chứng CTL bản v1 đạt 100% công thức (Hình 4.5) |
| `M2_V1_Ch1_SecLab_Console_Mutual_Exclusion_1.png` | Console kiểm tra Mutual Exclusion bản v1 an toàn (Hình 4.6) |
| `M2_V0_Ch2_SecLab_Static_Scan_KetQua.png` | Kết quả phân tích tĩnh mã C M2-v0 trên SecLab Ch2 (Hình 5.1) |
| `M2_V0_Ch2_SecLab_Static_CWE401_CWE416_ChiTiet.png` | Chi tiết cảnh báo CWE-401 (Leak) và CWE-416 (UAF) (Hình 5.2) |
| `M2_V1_Ch2_SecLab_QuetLoiConTro_0Loi_AnToan.png` | Phân tích tĩnh bản v1 đạt 0 lỗi con trỏ an toàn (Hình 5.3) |
| `M2_V1_Ch2_SecLab_PhanTich_StructAlignment_KhongStruct.png` | Phân tích cấu trúc Struct Alignment cho M2-v1 (Hình 5.4) |
| `M2_V0_Ch2_SecLab_Fuzzing_ExpandStockItems.png` | Chiến dịch Fuzzing hàm `expand_stock_items` (Hình 5.5) |
| `M2_V0_Ch2_SecLab_Fuzzing_ReleaseStockItems.png` | Chiến dịch Fuzzing hàm `release_stock_items` (Hình 5.6) |
| `M2_V0_Ch2_SecLab_Fuzzing_CalcExpandCapacity_CWE190.png` | Fuzzing phát hiện lỗi tràn số CWE-190 trong `calc_expand` (Hình 5.7) |
| `M2_V0_Ch2_SecLab_AST_DoPhucTap_McCabe_7.png` | Phân tích AST và độ phức tạp chu trình McCabe = 7 (Hình 5.8) |
| `M2_V0_Ch2_SecLab_SMT_Z3_SAT_HeapAlloc_Overflow.png` | Z3 SMT giải bài toán Heap Alloc Overflow báo SAT (Hình 5.9) |
| `M2_V0_Ch2_SecLab_SMT_Z3_Overflow_Int32_Capacity_Double.png` | Z3 SMT giải tràn số nhân đôi Capacity báo SAT (Hình 5.10) |
| `M2_V0_V1_Terminal_SMT_Z3_SAT_UNSAT_ChiTiet.png` | Terminal chi tiết 4 bài toán Z3 SAT trên v0 và UNSAT trên v1 (Hình 5.11) |
| `M2_V0_V1_Terminal_SMT_Z3_SAT_UNSAT_BangTongHop.png` | Bảng tổng hợp đối chứng SMT Z3 trên Terminal (Hình 5.12) |
| `M2_V0_Terminal_ASan_Heap_Use_After_Free.png` | Clang ASan bắt trọn lỗi `heap-use-after-free` với byte `[fd]` (Hình 5.13) |
| `TongHop_V1_Terminal_ASan_Safe_Clean_Run.png` | Clang ASan xác nhận bản vá v1 chạy sạch 0 lỗi runtime (Hình 5.14) |

*Bảng A.2. Danh mục 9 tệp nhật ký kiểm toán (Audit Execution Logs) trong thư mục `04_results/logs/`*

| STT | Tên tệp nhật ký log | Kích thước | Công cụ kiểm toán | Nội dung và ý nghĩa chứng minh an toàn |
| :---: | :--- | :---: | :--- | :--- |
| 1 | `01_kiem_chung_mo_hinh_kripke.log` | 8.5 KB | Model Checker (SecLab Ch1 / Python) | Kiểm chứng 22 công thức CTL trên mô hình Kripke toàn hệ thống (35 trạng thái). Chỉ ra 18 rủi ro vi phạm kèm vết phản ví dụ trên v0, đạt 100% thỏa mãn trên v1. |
| 2 | `02_kiem_chung_ctl_bmc.log` | 2.5 KB | Bounded Model Checking (BMC) | Kiểm chứng tính an toàn qua từng bước chuyển trạng thái hữu hạn ($k$-step). Xác nhận các trạng thái lỗi xuất hiện ở độ sâu $k \le 4$. |
| 3 | `03_kiem_tra_so_hoc_z3_smt.log` | 1.6 KB | Z3 SMT Solver (Python) | Giải bài toán tràn số nguyên 32-bit (`int32_t`) khi cộng dồn số lượng và nhân đơn giá; bản v0 cho kết quả SAT (có lỗi), bản v1 cho kết quả UNSAT (an toàn tuyệt đối). |
| 4 | `04_kiem_chung_z3_bmc_memory.log` | 1.4 KB | Z3 BMC Memory Checker | Tích hợp giải SMT với mô hình bộ nhớ con trỏ; chứng minh không xảy ra con trỏ treo lơ lửng khi áp dụng cơ chế xóa con trỏ `NULL`. |
| 5 | `05_kiem_tra_sat_unsat_m2.log` | 10.2 KB | Chuyên biệt Module M2 (Z3 & CTL) | Nhật ký chạy kiểm chứng chuyên sâu toàn bộ 4 bài toán Heap của Nguyễn Kiều Trang; chi tiết từng ràng buộc SAT/UNSAT và vết chuyển trạng thái Kripke 8 nút. |
| 6 | `06_bien_ban_kiem_thu_pytest.log` | 0.6 KB | Pytest Suite | Tự động hóa kiểm thử hồi quy 10 ca kiểm thử đối chứng logic nghiệp vụ giữa v0 và v1; tỷ lệ đạt chuẩn 10/10 test pass (100%). |
| 7 | `07_kiem_toan_asan_v0_bat_loi.log` | 0.9 KB | Clang AddressSanitizer (ASan) | Chạy kiểm toán động bản v0; bắt trọn lỗi ghi tràn bộ đệm Stack (`stack-buffer-overflow` CWE-121) và rò rỉ bộ nhớ Heap (CWE-401). |
| 8 | `08_kiem_toan_asan_v1_sach_loi.log` | 0.5 KB | Clang AddressSanitizer (ASan) | Chạy kiểm toán động bản vá v1; xác nhận chương trình thực thi trơn tru, không có lỗi rò rỉ, không vi phạm Shadow Memory (0 crash, clean memory). |
| 9 | `09_bao_cao_pipeline_specverificationlab_v0.log` | 10.1 KB | Pipeline SpecVerificationLab | Báo cáo tích hợp toàn diện từ phân tích tĩnh AST, tính độ phức tạp McCabe, chiến dịch Fuzzing biên và giải SMT tự động trên công cụ của Thầy. |

### Phụ lục B. Trích xuất log AddressSanitizer và SMT Z3
```
=================================================================
==18420==ERROR: AddressSanitizer: heap-use-after-free on address 0x120800000040 pc 0x00412a80 bp 0x00a0f7ec sp 0x00a0f7e4
READ of size 4 at 0x120800000040 thread T0
    #0 0x00412a80 in cleanup_and_read_uaf_v0 f:/Truong_P/atpm/03_source/v0_ban_loi\m2_cap_phat_heap_v0.c:78
    #1 0x00413150 in main f:/Truong_P/atpm/03_source/v0_ban_loi\main_khohang_v0.c:45
0x120800000040 is located 0 bytes inside of 4-byte region [0x120800000040,0x120800000044)
freed by thread T0 here:
    #0 0x00451a20 in free
    #1 0x00412a68 in cleanup_and_read_uaf_v0 f:/Truong_P/atpm/03_source/v0_ban_loi\m2_cap_phat_heap_v0.c:76
previously allocated by thread T0 here:
    #0 0x00451810 in malloc
    #1 0x00412a40 in cleanup_and_read_uaf_v0 f:/Truong_P/atpm/03_source/v0_ban_loi\m2_cap_phat_heap_v0.c:73
Shadow bytes around the buggy address:
  0x120800000000: fa fa fa fa fa fa fa fa
=>0x120800000040: [fd] fd fd fd fa fa fa fa
=================================================================
==18420==ABORTING
```

### Phụ lục C. Đặc tả bài toán số học trong sheet KiemTraSoHoc
*Bảng C.1. Ràng buộc toán học của 4 bài toán kiểm tra số học và con trỏ ở bản v0 và v1*

| Tên bài toán | Ràng buộc toán học Bản lỗi v0 (Kỳ vọng SAT) | Ràng buộc toán học Bản vá v1 (Kỳ vọng UNSAT) |
| :--- | :--- | :--- |
| **heap_alloc_bytes** | `new_capacity >= 1 and item_size == 48 and new_capacity * item_size > 2147483647` | `new_capacity >= 1 and item_size == 48 and new_capacity <= 44739242 and new_capacity * item_size > 2147483647` |
| **calc_expand_capacity** | `current_capacity >= 16 and factor == 2 and current_capacity * factor > 2147483647` | `current_capacity >= 16 and factor == 2 and current_capacity <= 1073741823 and current_capacity * factor > 2147483647` |
| **realloc_buffer_leak** | `old_ptr > 0 and list_ptr == 0` | `old_ptr > 0 and list_ptr == old_ptr and (old_ptr > 0 and list_ptr == 0)` |
| **cleanup_and_read_uaf** | `ptr_state == 2 and read_op == 1` | `ptr_state == 0 and (ptr_state == 2 and read_op == 1)` |
