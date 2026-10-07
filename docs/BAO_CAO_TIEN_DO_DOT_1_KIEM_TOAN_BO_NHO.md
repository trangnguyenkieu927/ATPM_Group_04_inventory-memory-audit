# BÁO CÁO TIẾN ĐỘ ĐỢT 1 & KỊCH BẢN BÁO CÁO
## ĐỀ TÀI: KIỂM TOÁN AN TOÀN BỘ NHỚ CHO CHƯƠNG TRÌNH C QUẢN LÝ KHO
* **Hình thức đăng ký đề tài**: **LỰA CHỌN 2 — CHUYÊN SÂU CHƯƠNG KIỂM TOÁN AN TOÀN BỘ NHỚ C (Memory Safety Audit)**.
* **Quy mô triển khai**: Đi sâu toàn diện vào **TOÀN BỘ 7 MODULE** của hệ thống thực tế (Không làm theo hướng End-to-End hình thức trên 1 hàm đồ chơi).
* **Nhóm thực hiện**: Nhóm 04 (Hà Văn Đô, Nguyễn Kiều Trang, Nguyễn Quang Thọ)
* **Học phần**: An toàn phần mềm (ATPM)

---

## PHẦN I: TUYÊN BỐ ĐỊNH HƯỚNG & PHẠM VI DỰ ÁN

### 1. Bản chất đề tài của Nhóm 04
Trong chương trình môn học An toàn phần mềm, có 2 cách tiếp cận:
* **Hướng tiếp cận 1 (End-to-End)**: Chạy lướt qua 4 chương (Đặc tả $\rightarrow$ Kripke $\rightarrow$ SMT $\rightarrow$ Fuzzing) nhưng chỉ áp dụng trên **1 hàm hoặc 1 module nhỏ lẻ**.
* **Hướng tiếp cận 2 (Chuyên sâu 1 chương — LỰA CHỌN CỦA NHÓM 04)**: **LÀM SÂU TOÀN DIỆN CHƯƠNG KIỂM TOÁN AN TOÀN BỘ NHỚ C** trên **TẤT CẢ CÁC MODULE** của một hệ thống quản lý kho hoàn chỉnh.

### 2. Mục tiêu trọng tâm của Nhóm
* Khảo sát, phân tích và kiểm toán toàn bộ vòng đời bộ nhớ (Stack, Heap, Pointer, Buffer, Arithmetic) trên một codebase C thực tế gồm **hàng nghìn dòng lệnh**, mô phỏng đầy đủ nghiệp vụ kho.
* Tập trung giải quyết triệt để **6 nhóm lỗ hổng bộ nhớ nguy hiểm nhất trong C** (theo danh mục bảo mật CWE quốc tế):
  1. **CWE-120**: Tràn bộ đệm (Classic Buffer Overflow).
  2. **CWE-401**: Rò rỉ bộ nhớ (Memory Leak - Missing Release).
  3. **CWE-415**: Giải phóng bộ nhớ kép (Double Free).
  4. **CWE-416**: Sử dụng con trỏ sau khi giải phóng (Use-After-Free).
  5. **CWE-476**: Giải tham chiếu con trỏ NULL (NULL Pointer Dereference).
  6. **CWE-190**: Tràn số nguyên gây sai lệch bộ nhớ và logic (Integer Overflow).
* Xây dựng cặp đối chứng kiểm toán: **Phiên bản lỗi tiềm ẩn (`audit/v0`)** và **Phiên bản phòng thủ chủ động (`audit/v1`)**.

---

## PHẦN II: BẢNG TỔNG HỢP TRẢ LỜI 5 CÂU HỎI CỐT LÕI CỦA GIẢNG VIÊN

| STT | Câu hỏi của Giảng viên | Trả lời theo Hướng Chuyên sâu Kiểm toán Bộ nhớ C | Dẫn chứng Mã nguồn / Thư mục trong Dự án |
| :---: | :--- | :--- | :--- |
| **1** | **LÀM ĐƯỢC NHỮNG GÌ?** *(Kết quả & Tiến độ)* | • Xây dựng hoàn chỉnh chương trình C mục tiêu gồm **toàn bộ 7 module nghiệp vụ liên hoàn** (CRUD sản phẩm, quản lý kho động, I/O file, báo cáo, validation, utils, CLI).<br>• Xác lập ma trận kiểm toán bao phủ **6 nhóm lỗi CWE bộ nhớ cốt lõi** xuyên suốt cả 7 module.<br>• Thiết lập quy trình kiểm toán so sánh 2 phiên bản: `audit/v0` (vulnerable) và `audit/v1` (hardened).<br>• Lập trình và biên dịch thành công các bộ kiểm thử tự động chuyên biệt chạy thực tế trên máy. | • Mã nguồn hệ thống: [source/](file:///c:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/source)<br>• Cặp phiên bản đối chứng: [audit/v0/](file:///c:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/audit/v0) & [audit/v1/](file:///c:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/audit/v1)<br>• Test suites: [tests/](file:///c:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/tests) |
| **2** | **CHỨNG THỰC BẰNG CÁCH NÀO?** *(Evidence & Proof)* | • **Chứng thực kích hoạt lỗi & phòng thủ**: Viết kịch bản PoC gây lỗi trên `v0` và chứng minh `v1` xử lý an toàn.<br>• **Chứng thực Stress Test Double Free (CWE-415)**: Chạy `test_cwe415.exe` lặp **50 chu kỳ** liên tục, mỗi chu kỳ gọi `free()` tới **5 lần** $\rightarrow$ **100% PASS, 0 crash**, không hỏng Heap metadata.<br>• **Chứng thực phòng thủ NULL Pointer (CWE-476)**: Chạy `test_cwe476.exe` truyền NULL vào 100% các API của 7 module $\rightarrow$ Bắt lỗi sạch, trả về mã lỗi `STATUS_ERR_NULL_PTR`, ngăn chặn hoàn toàn Segmentation Fault.<br>• **Chứng thực chặn biên & tràn số (CWE-190)**: Chạy `test_validation.exe` kiểm chứng chặn đứng $INT\_MAX$ và chuỗi quá dài. | • [test_cwe415.c](file:///c:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/tests/test_cwe415.c)<br>• [test_cwe476.c](file:///c:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/tests/test_cwe476.c)<br>• [test_validation.c](file:///c:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/tests/test_validation.c)<br>• Các file thực thi `.exe` đã biên dịch trong thư mục gốc. |
| **3** | **MÔ HÌNH Ở ĐÂU?** *(Memory Model)* | Vì làm **chuyên sâu Kiểm toán Bộ nhớ C**, mô hình của nhóm là **Mô hình Vòng đời & Cấu trúc Bộ nhớ C (C Memory & Pointer Lifecycle Model)**:<br>1. **Mô hình Phân vùng Bộ nhớ (Stack vs Heap Layout)**: Tách bạch rõ cấu trúc tĩnh `Product` (Stack) và mảng động `ProductList` / `Inventory` (Heap).<br>2. **Mô hình Vòng đời Cấp phát (Allocation Lifecycle)**: Quy tắc bất biến `Init (NULL) -> Grow (realloc an toàn) -> Safe Free (gán NULL)`.<br>3. **Mô hình Bất biến Con trỏ (Pointer Invariant)**: Triệt tiêu trạng thái con trỏ lơ lửng (*No Dangling Pointer Invariant*). | • Định nghĩa cấu trúc: [models.h](file:///c:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/source/include/models.h)<br>• Vòng đời bộ nhớ kho: [inventory.c](file:///c:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/source/src/inventory.c#L9-L52)<br>• Vòng đời bộ nhớ sản phẩm: [product.c](file:///c:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/source/src/product.c)<br>• Tài liệu kiến trúc: `docs/architecture/` |
| **4** | **SINH MÃ / CODE Ở ĐÂU?** *(Code Location)* | • **Không dùng mã sinh đồ chơi vài dòng**: Toàn bộ mã nguồn là hệ thống C thực tế, hoàn chỉnh và có thể biên dịch, chạy được ngay trong thư mục **`source/`**.<br>• Phân chia thành 7 module nghiệp vụ độc lập:<br>  - `models.h`: Định nghĩa cấu trúc dữ liệu, hằng số kích thước, bảng mã lỗi chuẩn.<br>  - `product.c`: Quản lý mảng động sản phẩm và giải thuật dồn dịch bộ nhớ khi xóa.<br>  - `inventory.c`: Quản lý kho, cơ chế tăng trưởng dung lượng (`inventory_grow`) an toàn.<br>  - `validation.c`: Cổng phòng thủ dữ liệu đầu vào (Validation Gate).<br>  - `utils.c`: Thư viện xử lý chuỗi an toàn chống tràn bộ đệm (`safe_string_copy`).<br>  - `file_io.c` & `report.c`: File I/O và báo cáo thống kê kiểm soát kích thước đệm đọc.<br>  - `main.c`: Giao diện điều phối luồng thực thi an toàn. | • Thư mục mã nguồn: [source/src/](file:///c:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/source/src)<br>• Thư mục giao diện API: [source/include/](file:///c:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/source/include)<br>• Thư mục kiểm thử: [tests/](file:///c:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/tests) |
| **5** | **TEST / ĐIỀU KIỆN NHƯ THẾ NÀO?** *(Defensive Invariants & Test Suites)* | • **Các Điều kiện Ràng buộc Phòng thủ (Safety Guards)**:<br>  - Guard con trỏ: `if (ptr == NULL) return STATUS_ERR_NULL_PTR;`<br>  - Guard realloc: `new_capacity > SIZE_MAX / sizeof(...)` $\rightarrow$ chặn tràn số khi mở rộng heap.<br>  - Guard thu hồi bộ nhớ: `free(ptr); ptr = NULL; count = 0; capacity = 0;` $\rightarrow$ tính chất Idempotent free, chống Double Free và Use-After-Free.<br>  - Guard chuỗi: Bắt buộc chèn ký tự `\0` tại vị trí `dest_size - 1`.<br>  - Guard số nguyên: `current_stock > INT_MAX - add_quantity` $\rightarrow$ chặn tràn số khi nhập kho.<br>• **Tổ chức bộ Test**: Phân chia theo 3 tập dữ liệu: `tests/valid/` (dữ liệu hợp lệ), `tests/boundary/` (dữ liệu tại biên $INT\_MAX$, độ dài cực đại), `tests/abnormal/` (dữ liệu độc hại / file hỏng). | • Phòng thủ đầu vào: [validation.c](file:///c:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/source/src/validation.c)<br>• Phòng thủ chuỗi: [utils.c](file:///c:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/source/src/utils.c)<br>• Phòng thủ Heap: [inventory.c](file:///c:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/source/src/inventory.c)<br>• Bộ test: [test_cwe415.c](file:///c:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/tests/test_cwe415.c), [test_cwe476.c](file:///c:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/tests/test_cwe476.c) |

---

## PHẦN III: KỊCH BẢN THUYẾT TRÌNH BÁO CÁO TIẾN ĐỘ ĐỢT 1

### 1. Phần Mở đầu: Khẳng định Định hướng Đề tài (1 phút)
> *"Kính thưa Thầy/Cô và các bạn, hôm nay nhóm 04 xin báo cáo tiến độ Đợt 1 bài tập lớn môn An toàn phần mềm với đề tài: **'Kiểm toán an toàn bộ nhớ cho chương trình C mô phỏng nghiệp vụ quản lý kho'**.*
>
> *Trước hết, nhóm em xin làm rõ định hướng thực hiện đề tài: Trong môn học có 2 hướng là End-to-End (chạy lướt 4 chương trên 1 hàm/module nhỏ) và **Lựa chọn 2: Đi sâu toàn diện vào 1 chương chuyên đề**. Nhóm 04 chúng em đã **đăng ký lựa chọn 2: Đi sâu toàn diện vào CHƯƠNG KIỂM TOÁN AN TOÀN BỘ NHỚ C (Memory Safety Audit)** trên **TOÀN BỘ CÁC MODULE** của hệ thống.*
>
> *Lý do nhóm lựa chọn hướng này: Trong ngôn ngữ C, các lỗi an toàn bộ nhớ không bao giờ xuất hiện đơn lẻ ở một hàm vài dòng, mà là sự tương tác phức tạp giữa việc cấp phát mảng động trên Heap, truyền con trỏ qua nhiều tầng hàm, xử lý chuỗi I/O và dọn dẹp khi gặp lỗi. Do đó, nhóm quyết định xây dựng một hệ thống hoàn chỉnh và kiểm toán sâu trên toàn bộ các module."*

---

### 2. Trả lời Trực diện 5 Câu hỏi của Giảng viên (5 - 7 phút)

#### Câu hỏi 1: Nhóm đã làm được những gì?
> *"Thưa Thầy/Cô, trong đợt 1 nhóm đã hoàn thành 4 khối lượng công việc thực chất:
> 1. **Xây dựng hoàn chỉnh Hệ thống C mục tiêu**: Viết đầy đủ 7 module độc lập với kiến trúc phân tầng sạch sẽ trong `source/` gồm: `models.h`, `product.c`, `inventory.c`, `validation.c`, `utils.c`, `file_io.c`, `report.c` và `main.c`.
> 2. **Xác lập Ma trận kiểm toán bao phủ 6 lớp lỗi bộ nhớ cốt lõi**:
>    * CWE-120 (Buffer Overflow) trong xử lý chuỗi và đọc file.
>    * CWE-401 (Memory Leak) khi cấp phát và mở rộng mảng động.
>    * CWE-415 (Double Free) khi dọn dẹp bộ nhớ ở các nhánh rẽ lỗi.
>    * CWE-416 (Use-After-Free) sau khi xóa sản phẩm hoặc thu hồi kho.
>    * CWE-476 (NULL Pointer Dereference) trên toàn bộ các hàm API công khai.
>    * CWE-190 (Integer Overflow) khi tính toán số lượng tồn kho cực lớn.
> 3. **Thiết lập Cấu trúc Kiểm toán Đối chứng**: Phân chia thư mục `audit/v0` (phiên bản mô phỏng lỗi) và `audit/v1` (phiên bản vá phòng thủ).
> 4. **Xây dựng các Suite Kiểm thử Tự động Chuyên sâu**: Biên dịch và chạy trực tiếp các bộ test độc lập `test_cwe415.exe`, `test_cwe476.exe`, `test_validation.exe`."*

#### Câu hỏi 2: Chứng thực bằng cách nào?
> *"Thưa Thầy/Cô, vì làm chuyên sâu kiểm toán bộ nhớ C, chứng thực của nhóm là **Chứng thực Thực nghiệm trên cơ sở dữ liệu và trạng thái Heap/Stack thực tế**:
> 1. **Chứng thực phòng thủ Double Free (CWE-415)**: Nhóm đã viết và chạy chương trình `test_cwe415.c`. Chương trình thực hiện **Stress Test 50 chu kỳ lặp lại**, mỗi chu kỳ gọi hàm giải phóng `free()` tới **5 lần liên tiếp** trên cùng một biến. Kết quả: **100% PASS, 0 crash**, vùng nhớ Heap hoàn toàn sạch sẽ nhờ cơ chế tự động gán con trỏ về `NULL` ngay sau khi giải phóng.
> 2. **Chứng thực phòng thủ NULL Dereference (CWE-476)**: Nhóm chạy `test_cwe476.c` truyền con trỏ `NULL` vào tất cả các tham số của toàn bộ API trong 7 module. Kết quả: 100% các hàm bắt lỗi an toàn và trả về mã lỗi `STATUS_ERR_NULL_PTR`, không một trường hợp nào bị Segmentation Fault hoặc sập ứng dụng.
> 3. **Chứng thực phòng thủ Tràn số nguyên (CWE-190)**: Đưa giá trị biên $INT\_MAX$ ($2147483647$) vào phép cộng nhập kho, chứng thực hàm `check_addition_overflow` chặn đứng trước khi xảy ra tràn bit và trả về mã lỗi `STATUS_ERR_OVERFLOW`."*

#### Câu hỏi 3: Mô hình ở đâu?
> *"Thưa Thầy/Cô, đối với chuyên đề Kiểm toán Bộ nhớ C, mô hình cốt lõi của nhóm là **Mô hình Vòng đời và Cấu trúc Bộ nhớ C (C Memory & Pointer Lifecycle Model)**:
> 1. **Mô hình Phân vùng Bộ nhớ (Memory Layout)**: Thể hiện trong `models.h`. Các trường dữ liệu chuỗi (ID 20 byte, Name 100 byte, Category 50 byte) được cấp phát tĩnh trong struct `Product` để tối ưu Stack; trong khi danh sách sản phẩm được quản lý động trên Heap thông qua con trỏ mảng `Product *items` trong struct `ProductList` / `Inventory`.
> 2. **Mô hình Vòng đời Cấp phát (Allocation Lifecycle)**:
>    * Bước 1: Khởi tạo sạch con trỏ về `NULL`, dung lượng bằng 0 (`inventory_init`).
>    * Bước 2: Tăng trưởng dung lượng động qua `realloc` với điều kiện kiểm tra an toàn tràn số `SIZE_MAX`.
>    * Bước 3: Thu hồi bộ nhớ và vô hiệu hóa con trỏ ngay lập tức (`inventory_free`).
> 3. **Mô hình Bất biến Con trỏ (Pointer Invariants)**: Đảm bảo nguyên lý: Không một con trỏ nào được phép trỏ vào vùng nhớ đã giải phóng (*Dangling Pointer Invariant*)."*

#### Câu hỏi 4: Code ở đâu?
> *"Thưa Thầy/Cô, toàn bộ mã nguồn của nhóm là **Mã C thực tế, hoàn chỉnh và có thể biên dịch, chạy được ngay**:
> * Toàn bộ mã nguồn nghiệp vụ của 7 module nằm trong thư mục **`source/src/`** và giao diện API tại **`source/include/`**.
> * Bộ mã kiểm thử kiểm toán độc lập nằm tại thư mục **`tests/`**.
> * Nhóm không sử dụng các đoạn code sinh đồ chơi vài dòng, mà kiểm toán trực tiếp trên hệ thống hoàn chỉnh với hàng nghìn dòng lệnh C bao gồm đầy đủ nghiệp vụ CRUD, đọc ghi tệp tin, dồn dịch mảng động, và menu console tương tác."*

#### Câu hỏi 5: Test và Điều kiện như thế nào?
> *"Thưa Thầy/Cô, chiến lược kiểm thử và điều kiện phòng thủ của nhóm được thiết kế chặt chẽ:
> 1. **Về các điều kiện phòng thủ (Safety Guards)**:
>    * Mọi hàm đều có các Guard kiểm tra đầu vào trước khi thao tác bộ nhớ:
>      * Guard con trỏ: `if (ptr == NULL) return STATUS_ERR_NULL_PTR;`
>      * Guard chuỗi: Luôn chèn `\0` tại vị trí cuối bộ đệm trong `safe_string_copy`.
>      * Guard số học: `if (current_stock > INT_MAX - add_quantity) return STATUS_ERR_OVERFLOW;`
>      * Guard giải phóng: Gán con trỏ về `NULL` ngay sau `free()`.
> 2. **Về tổ chức bộ Test**:
>    * Nhóm chia dữ liệu test thành 3 phân vùng rõ ràng trong thư mục `tests/`: `valid/` (dữ liệu chuẩn), `boundary/` (dữ liệu tại biên $INT\_MAX$, độ dài cực đại), và `abnormal/` (dữ liệu độc hại, file cấu trúc lỗi để test rollback cleanup).
>    * Mỗi nhóm CWE trọng tâm đều có file kiểm thử riêng biệt để đo lường định lượng tỷ lệ vượt qua."*

---

### 3. Kế hoạch Hoàn thiện Đợt 2 (1 phút)
> *"Dựa trên kết quả Đợt 1, trong giai đoạn Đợt 2, nhóm 04 sẽ tập trung hoàn thiện:
> 1. Đóng gói hoàn chỉnh phiên bản có lỗi vào `audit/v0` và phiên bản vá phòng thủ vào `audit/v1` để làm đối chứng trực quan trước và sau kiểm toán.
> 2. Chạy công cụ kiểm toán tĩnh tự động **Cppcheck** và **Pattern Scanner**, xuất log phân tích vào `results/`.
> 3. Biên dịch và kiểm thử với các bộ phát hiện lỗi bộ nhớ thời gian thực của GCC/Clang: **AddressSanitizer (ASan)** và **UBSan** để có bằng chứng thực nghiệm chuyên sâu.
> 4. Hoàn thiện tài liệu báo cáo phân tích chi tiết từng phát hiện (Findings) trong `docs/findings/`.
>
> Nhóm 04 xin cảm ơn Thầy/Cô đã lắng nghe và rất mong nhận được góp ý từ Thầy/Cô!"*
