# KỊCH BẢN BÁO CÁO TIẾN ĐỘ ĐỢT 1
## ĐỀ TÀI: KIỂM TOÁN AN TOÀN BỘ NHỚ CHO CHƯƠNG TRÌNH C MÔ PHỎNG NGHIỆP VỤ QUẢN LÝ KHO
**Nhóm thực hiện**: Nhóm 04 (Hà Văn Đô, Nguyễn Kiều Trang, Nguyễn Quang Thọ)  
**Học phần**: An toàn phần mềm (ATPM)  
**Thời lượng trình bày**: 7 - 10 phút  

---

## PHẦN I: TỔNG QUAN PHẠM VI DỰ ÁN & TIẾN ĐỘ TRIỂN KHAI

### 1. Lời mở đầu & Mục tiêu đề tài (1 phút)
> *"Kính thưa Thầy/Cô và các bạn, hôm nay nhóm 04 xin phép báo cáo tiến độ Đợt 1 của bài tập lớn môn An toàn phần mềm với đề tài: **'Kiểm toán an toàn bộ nhớ (Memory Safety Audit) cho chương trình C mô phỏng nghiệp vụ quản lý kho'**.*
>
> *Mục tiêu cốt lõi của đề tài không phải là tạo ra một phần mềm quản lý kho thương mại đồ sộ, mà là xây dựng một **chương trình mục tiêu (Target System) bằng ngôn ngữ C chuẩn mực**, áp dụng phương pháp luận **Kiểm chứng hình thức (Formal Methods)** kết hợp với **Kiểm toán an toàn (Security Audit)** nhằm phát hiện, chứng minh toán học và phòng thủ triệt để các lỗ hổng bộ nhớ kinh điển như: Tràn bộ đệm (Buffer Overflow), Rò rỉ bộ nhớ (Memory Leak), Giải phóng bộ nhớ kép (Double Free), Sử dụng con trỏ sau giải phóng (Use-After-Free) và Tràn số nguyên (Integer Overflow)."*

### 2. Phạm vi dự án (Project Scope) (1 phút)
> *"Về phạm vi triển khai, dự án của nhóm bao trùm 3 trục chính:*
> 1. ***Trục Nghiệp vụ (Domain Core)***: Mô phỏng đầy đủ chu trình kho gồm 12 yêu cầu (REQ-1 đến REQ-12): Khởi tạo mảng động danh sách sản phẩm, CRUD sản phẩm, Nhập/Xuất kho, Tìm kiếm, Đọc/Ghi dữ liệu tệp `products.txt` và xuất báo cáo `inventory_report.txt`.
> 2. ***Trục Kiểm chứng Hình thức (Formal Verification Core)***: Đặc tả hệ thống dưới dạng Kripke Structure, kiểm chứng các thuộc tính bất biến bằng Temporal Logic CTL, và giải các ràng buộc biên bằng Z3 SMT Solver.
> 3. ***Trục Kiểm toán An toàn (Audit & Defensive Core)***: Đối chiếu phân loại theo chuẩn quốc tế CWE/CVE, kiểm tra tĩnh (Static Analysis), kiểm thử thực nghiệm động (Unit Tests, Stress Tests) và xây dựng cơ chế phòng thủ bộ nhớ chủ động."*

---

## PHẦN II: TRẢ LỜI 5 CÂU HỎI TRỌNG TÂM CỦA GIẢNG VIÊN

---

### CÂU HỎI 1: NHÓM ĐÃ LÀM ĐƯỢC NHỮNG GÌ? (WHAT HAS BEEN DONE?)

> *"Thưa Thầy/Cô, trong giai đoạn đợt 1, nhóm đã hoàn thành trọn vẹn 5 khối lượng công việc then chốt:*
>
> 1. ***Chuẩn hóa Bộ đặc tả yêu cầu và Ràng buộc hệ thống***:
>    * Xây dựng tài liệu đặc tả chi tiết gồm 12 yêu cầu chức năng & an toàn (`REQ-1` đến `REQ-12`), 3 yêu cầu phi chức năng (`NFR-1` đến `NFR-3`) và 4 ràng buộc kỹ thuật khắt khe (`CON-1` đến `CON-4`).
>
> 2. ***Xây dựng toàn bộ Mã nguồn C của Hệ thống Quản lý kho (Target Codebase)***:
>    * Đã hoàn thiện 100% kiến trúc module hóa sạch trong thư mục `source/` bao gồm:
>      * `models.h`: Định nghĩa cấu trúc `Product`, `ProductList`, `Inventory` và hệ thống mã lỗi chuẩn hóa (`STATUS_ERR_*`).
>      * `product.c` & `inventory.c`: Quản lý cấp phát mảng động, realloc an toàn, chống tràn `SIZE_MAX`.
>      * `validation.c`: Bộ lọc dữ liệu đầu vào và kiểm tra điều kiện an toàn toán học.
>      * `utils.c`: Các hàm tiện ích dùng chung xử lý chuỗi và đọc nhập an toàn (`safe_string_copy`, `safe_read_line`).
>      * `file_io.c` & `report.c`: Đọc ghi dữ liệu file và tổng hợp số liệu tồn kho.
>      * `main.c`: Giao diện dòng lệnh tương tác kiểm soát lỗi người dùng.
>
> 3. ***Thực hiện Kiểm chứng hình thức trên Mô hình trạng thái (Kripke & CTL)***:
>    * Mô hình hóa vòng đời dữ liệu bộ nhớ qua 5 trạng thái: `uninitialized`, `ready`, `overflow_risk`, `terminated`, `error_state`.
>    * Chứng minh tự động các thuộc tính CTL về an toàn bộ nhớ.
>
> 4. ***Xác lập Bằng chứng toán học bằng Z3 SMT Solver & Static Checker***:
>    * Sử dụng Z3 Solver để tự động sinh ra vector tấn công biên gây tràn số nguyên (SAT model).
>    * Chạy bộ phân tích tĩnh gắn cờ các hàm nguy hiểm (`strcpy`) và mẫu mã lỗi cấp phát.
>
> 5. ***Xây dựng Suite Kiểm thử thực nghiệm và Ma trận truy vết (Traceability Matrix)***:
>    * Xây dựng ma trận truy vết từ yêu cầu `REQ` sang Test Case (`TC-1` đến `TC-6`).
>    * Cài đặt các chương trình kiểm thử chuyên biệt: `test_cwe415.c` (kiểm toán Double Free), `test_cwe476.c` (kiểm toán con trỏ NULL), `test_validation.c` (kiểm toán biên giá trị và chuỗi bẩn)."*

---

### CÂU HỎI 2: CHỨNG THỰC BẰNG CÁCH NÀO? (VERIFICATION & PROOF)

> *"Thưa Thầy/Cô, việc chứng thực tính an toàn và phát hiện lỗi của nhóm được bảo đảm qua **3 tầng chứng cứ khách quan, có thể tái lập 100%**:
>
> #### Tầng 1: Chứng thực bằng Kiểm chứng Hình thức (Formal Verification Proof)
> * Trong mô hình Kripke, nhóm đặt ra 4 công thức Temporal Logic CTL để kiểm chứng:
>   * $AG(\neg \text{memory\_leak})$ $\rightarrow$ **ĐÚNG (Pass)**: Mọi đường thực thi hợp lệ đều dẫn đến giải phóng bộ nhớ.
>   * $AG(\neg \text{buffer\_overflow})$ $\rightarrow$ **ĐÚNG (Pass)**: Không có trạng thái nào bị tràn bộ đệm khi áp dụng guard.
>   * $AG(\neg \text{integer\_overflow})$ $\rightarrow$ **ĐÚNG (Pass)**: Phép cộng tồn kho được kiểm soát nguy cơ.
>   * $AG(\text{quantity} \ge 0)$ $\rightarrow$ **VI PHẠM (Violated)**: Mô hình toán học chứng minh rằng *nếu không có cơ chế chặn xuất kho vượt tồn*, hệ thống chắc chắn rơi vào trạng thái số lượng âm. Đây là bằng chứng hình thức chứng minh sự cần thiết bắt buộc của hàm kiểm tra `validate_export_quantity()`.
>
> #### Tầng 2: Chứng thực bằng Bộ giải SMT Z3 (SMT Solver Proof)
> * Đối với nguy cơ tràn số nguyên 32-bit (CWE-190), thay vì đoán mò giá trị, nhóm sử dụng bộ giải Z3 SMT Solver.
> * Z3 giải hệ phương trình ràng buộc và trả về trạng thái **SAT** với lời giải toán học chính xác:
>   $$\text{current\_stock} = 1, \quad \text{add\_quantity} = 2147483647 \; (\text{INT\_MAX})$$
>   Khi cộng: $1 + 2147483647 = 2147483648$, giá trị này vượt ngưỡng $2^{31}-1$ và tràn bit dấu thành $-2147483648$, làm sai lệch toàn bộ giá trị kho hàng. Bằng chứng này là cơ sở để nhóm cài đặt công thức phòng thủ toán học trong mã nguồn: `if (current_stock > INT_MAX - add_quantity) return STATUS_ERR_OVERFLOW;`.
>
> #### Tầng 3: Chứng thực bằng Kiểm thử thực nghiệm (Unit & Stress Test Proof)
> * Nhóm đã biên dịch và thực thi trực tiếp các bộ test trên máy:
>   * Chạy `test_cwe415.exe`: Thực hiện **Stress Test 50 chu kỳ liên tục**, mỗi chu kỳ gọi giải phóng `free()` tới **5 lần liên tiếp** trên cùng một con trỏ. Kết quả: **100% PASS**, không hề xảy ra hiện tượng crash bộ nhớ nhờ cơ chế gán con trỏ về `NULL` ngay sau khi giải phóng.
>   * Chạy `test_validation.exe`: Đưa vào các chuỗi bẩn, chuỗi vượt quá 100 ký tự, giá trị âm, số vượt `INT_MAX`. Kết quả: Hệ thống chặn thành công 100% và trả về đúng mã lỗi tương ứng."*

---

### CÂU HỎI 3: MÔ HÌNH Ở ĐÂU? (WHERE IS THE MODEL?)

> *"Thưa Thầy/Cô, mô hình của dự án tồn tại ở 3 cấp độ cụ thể, được định vị rõ ràng trong tài liệu và hệ thống:*
>
> 1. ***Mô hình Chuyển trạng thái Hình thức (Kripke State Transition Model)***:
>    * **Vị trí**: Nằm tại **Mục 2.1 của Báo cáo tích hợp** (kèm mã nguồn vector hóa TikZ/LaTeX).
>    * **Cấu trúc**: Gồm 5 nút trạng thái:
>      * `nd_uninitialized`: Trạng thái ban đầu trước khi cấp phát bộ nhớ.
>      * `nd_ready`: Trạng thái mảng động đã sẵn sàng cho thao tác nghiệp vụ thông thường.
>      * `nd_overflow_risk`: Trạng thái cảnh báo khi lượng nhập kho tiệm cận ngưỡng tràn số.
>      * `nd_error_state`: Trạng thái vi phạm khi phát hiện lỗi đầu vào hoặc bất thường.
>      * `nd_terminated`: Trạng thái kết thúc an toàn khi bộ nhớ đã được thu hồi toàn bộ.
>
> 2. ***Mô hình Dữ liệu và Cấu trúc Bộ nhớ (Data & Memory Model)***:
>    * **Vị trí**: Định nghĩa trực tiếp tại file `source/include/models.h`.
>    * **Cấu trúc**: Phân tách rõ ràng giữa mô hình thực thể `Product` (kích thước cố định chống tràn stack) và mô hình quản lý bộ nhớ heap `ProductList` / `Inventory` (chứa con trỏ động `Product *items`, biến đếm `count`, và dung lượng mở rộng `capacity`).
>
> 3. ***Mô hình Kiến trúc Luồng Xử lý Phần mềm (Architecture Flow Model)***:
>    * **Vị trí**: Lưu trữ trong thư mục `docs/architecture/`.
>    * **Cấu trúc**: Mô hình hóa luồng xử lý khép kín: Khởi tạo $\rightarrow$ Đọc dữ liệu file $\rightarrow$ Kiểm định dữ liệu đầu vào (Validation Gate) $\rightarrow$ Thao tác bộ nhớ động (Heap Allocation/Reallocation) $\rightarrow$ Lưu trữ an toàn $\rightarrow$ Giải phóng dọn dẹp triệt để (Cleanup)."*

---

### CÂU HỎI 4: SINH MÃ / CODE Ở ĐÂU? (WHERE IS THE CODE?)

> *"Thưa Thầy/Cô, mã nguồn của dự án được tổ chức rất rõ ràng thành 2 phần tương ứng với mục tiêu kiểm toán:
>
> #### 1. Phần Mã C Sinh ra Mô phỏng Lỗ hổng (Synthetic Vulnerable Skeleton Code)
> * **Vị trí**: Nằm tại **Mục 3 (Trang 4 - 5) trong Báo cáo tích hợp**.
> * **Bản chất**: Đây là các đoạn mã C được sinh tự động từ đặc tả hình thức nhằm mục đích tái hiện các lỗi bảo mật điển hình để kiểm toán và chứng minh công cụ phát hiện:
>   * `safe_string_copy()`: Dùng `strcpy(buffer, dest)` với buffer 100 byte $\rightarrow$ Minh họa lỗ hổng Buffer Overflow (CWE-120).
>   * `add_product()`: Cấp phát `malloc` nhưng cố tình không gọi `free()` $\rightarrow$ Minh họa lỗi Memory Leak (CWE-401).
>   * `inventory_free()`: Gọi `free(buf)` 2 lần liên tiếp $\rightarrow$ Minh họa lỗi Double Free (CWE-415).
>   * `delete_product()`: Gọi `free(buf)` rồi ghi dữ liệu `buf[0] = 1` $\rightarrow$ Minh họa lỗi Use-After-Free (CWE-416).
>   * `import_stock()`: Phép cộng số lượng không chặn biên $\rightarrow$ Minh họa lỗi Integer Overflow (CWE-190).
>
> #### 2. Phần Mã nguồn Hệ thống Mục tiêu Thực tế (Target System Codebase)
> * **Vị trí**: Nằm hoàn toàn trong thư mục **`source/`** của repository:
>   * `source/include/`: Chứa các header file quy định giao diện và API chuẩn (`models.h`, `product.h`, `inventory.h`, `validation.h`, `utils.h`, `file_io.h`, `report.h`).
>   * `source/src/`: Chứa toàn bộ hiện thực nghiệp vụ thực tế đã được lập trình cẩn thận theo nguyên tắc phòng thủ an toàn bộ nhớ.
>   * `source/data/`: Chứa file cơ sở dữ liệu `products.txt`.
>   * `source/Makefile`: Kịch bản build chương trình chuẩn với các cờ cảnh báo biên dịch."*

---

### CÂU HỎI 5: TEST / ĐIỀU KIỆN NHƯ THẾ NÀO? (TESTS & CONDITIONS?)

> *"Thưa Thầy/Cô, chiến lược kiểm thử và các điều kiện an toàn được nhóm thiết kế chặt chẽ theo 2 trục:
>
> #### 1. Các Điều kiện Ràng buộc An toàn (Safety Guards & Invariants)
> Toàn bộ logic phòng thủ trong mã nguồn được xây dựng dựa trên các điều kiện tiên quyết (Pre-conditions) bất biến:
> * **Ràng buộc Chuỗi (String Length Guard)**:
>   $$\text{len} == 0 \quad \lor \quad \text{len} \ge \text{MAX\_LEN}$$
>   Từ chối ngay lập tức các chuỗi không kết thúc bằng `\0` hoặc vượt quá giới hạn bộ đệm, đồng thời hàm `safe_string_copy` luôn ép ký tự `\0` ở vị trí `dest_size - 1`.
> * **Ràng buộc Chống tràn số nguyên (Arithmetic Overflow Guard)**:
>   $$\text{current\_stock} > \text{INT\_MAX} - \text{add\_quantity}$$
>   Đây là điều kiện toán học bắt buộc trước khi thực hiện bất kỳ phép cộng số lượng tồn kho nào.
> * **Ràng buộc Xuất kho (Business Stock Guard)**:
>   $$\text{export\_quantity} \le 0 \quad \lor \quad \text{export\_quantity} > \text{current\_stock}$$
>   Ngăn chặn triệt để trạng thái tồn kho âm đã bị phát hiện trong kiểm chứng CTL.
> * **Ràng buộc Con trỏ và Bộ nhớ (Pointer Lifecycle Guard)**:
>   Sau khi giải phóng bộ nhớ `free(ptr)`, biến con trỏ luôn được gán ngay `ptr = NULL` và đồng bộ `count = 0, capacity = 0`. Mọi hàm truy cập đều kiểm tra `if (ptr == NULL) return STATUS_ERR_NULL_PTR;`.
>
> #### 2. Tổ chức Bộ Kiểm thử (Test Suites & Traceability Matrix)
> * Nhóm xây dựng Ma trận truy vết từ yêu cầu `REQ` sang các Test Case (như bảng tại Mục 7 trong báo cáo):
>   * **TC-1** (REQ-1, REQ-11): Test tràn bộ đệm chuỗi $\rightarrow$ Kết quả mong đợi: Cắt an toàn, không tràn.
>   * **TC-2** (REQ-5, REQ-12): Test nhập kho số lượng cực lớn $\rightarrow$ Kết quả mong đợi: Báo lỗi `STATUS_ERR_OVERFLOW`.
>   * **TC-3** (REQ-6): Test xuất kho vượt tồn $\rightarrow$ Kết quả mong đợi: Báo lỗi `STATUS_ERR_INSUFFICIENT_STOCK`.
>   * **TC-4** (REQ-10): Test giải phóng mảng động $\rightarrow$ Kết quả: 0 Memory Leaks.
>   * **TC-5** (REQ-3, REQ-10): Test xóa sản phẩm $\rightarrow$ Kết quả: Chống Use-After-Free, mảng dồn dịch chính xác.
>   * **TC-6** (REQ-8): Test đọc file dữ liệu bẩn $\rightarrow$ Kết quả: Bỏ qua dòng hỏng an toàn, không sập chương trình.
> * Các test case này được hiện thực hóa trực tiếp thành các file kiểm thử độc lập trong thư mục `tests/`:
>   * `tests/test_cwe415.c`: Suite kiểm thử Double Free chuyên sâu.
>   * `tests/test_cwe476.c`: Suite kiểm thử phòng thủ con trỏ NULL.
>   * `tests/test_validation.c`: Suite kiểm thử các điều kiện biên và định dạng dữ liệu."*

---

## PHẦN III: KẾ HOẠCH BƯỚC TIẾP THEO (GIAI ĐOẠN ĐỢT 2) (1 phút)

> *"Dựa trên nền tảng vững chắc của Đợt 1, trong giai đoạn Đợt 2, nhóm 04 sẽ tập trung hoàn thiện các nội dung sau:
> 1. Đưa phiên bản có chủ đích lỗi vào thư mục `audit/v0` và phiên bản phòng thủ vào `audit/v1` để tạo cặp đối chứng so sánh trực quan.
> 2. Chạy công cụ kiểm tra tự động nâng cao: Sử dụng **Cppcheck**, **AddressSanitizer (ASan)**, **UBSan** và xuất toàn bộ log thực nghiệm vào thư mục `results/`.
> 3. Tiến hành Fuzzing trên mã C bằng bộ sinh test tự động để tìm kiếm các ca biên tiềm ẩn.
> 4. Hoàn thiện tài liệu phân tích chuyên sâu các finding trong thư mục `docs/findings/` và đóng gói báo cáo tổng kết môn học.
>
> Nhóm 04 xin chân thành cảm ơn Thầy/Cô đã lắng nghe và rất mong nhận được những ý kiến đóng góp quý báu từ Thầy/Cô để hoàn thiện đề tài tốt hơn nữa!"*
