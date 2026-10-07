# BÁO CÁO ĐẶC TẢ VÀ ĐỐI CHIẾU VALIDATION DỮ LIỆU (TASK 7 - P0)
**Học phần:** An toàn phần mềm  
**Đề tài:** Memory Safety Audit cho chương trình C mô phỏng nghiệp vụ quản lý kho  
**Người phụ trách:** Hà Văn Đô (Task STT: 7 - `feature/spec-validation-do`)  
**Mục tiêu:** Đối chiếu điều kiện ID / Name / Unit / Price / Quantity và Logic kho an toàn bộ nhớ giữa Đặc tả (SPEC) và Mã nguồn C (`validation.c`/`validation.h`).

---

## 1. BẢNG TRANG TÍNH: YeuCauChucNang (Sheet YeuCauChucNang)

| ID | MoTa | DanhMuc | MucUuTien |
| :--- | :--- | :--- | :--- |
| **REQ-1** | Con trỏ chuỗi mã sản phẩm (ID) truyền vào hàm kiểm tra không được là con trỏ NULL nhằm ngăn chặn lỗi Null Pointer Dereference (CWE-476). | memory | HIGH |
| **REQ-2** | Mã sản phẩm (ID) phải có độ dài từ 1 đến 19 ký tự (giới hạn mảng `MAX_ID_LEN = 20` ký tự bao gồm byte kết thúc chuỗi `\0`), không được là chuỗi rỗng hoặc vượt quá dung lượng buffer gây tràn bộ đệm. | safety | HIGH |
| **REQ-3** | Mã sản phẩm (ID) không được chứa khoảng trắng, ký tự điều khiển (control characters) hoặc các ký tự phân tách dữ liệu tệp (`\|`, `,`, `;`) nhằm chống lỗi parse tệp và lỗi định dạng. | functional | HIGH |
| **REQ-4** | Tên sản phẩm (Name) không được là con trỏ NULL, độ dài từ 1 đến 99 ký tự (`MAX_NAME_LEN = 100`), không được chỉ chứa khoảng trắng và không chứa các ký tự xuống dòng `\n`, `\r` hoặc ký tự phân tách `\|`. | safety | HIGH |
| **REQ-5** | Loại sản phẩm (Category) nếu được cung cấp không được là con trỏ NULL (khi truyền vào), độ dài từ 1 đến 49 ký tự (`MAX_CATEGORY_LEN = 50`), không được chỉ toàn khoảng trắng và không chứa ký tự cấm (`\|`, `\n`, `\r`). | functional | MEDIUM |
| **REQ-6** | Đơn vị tính (Unit) không được là con trỏ NULL, độ dài từ 1 đến 19 ký tự (`MAX_UNIT_LEN = 20`), không được rỗng hoặc toàn khoảng trắng và không chứa ký tự phân tách trường `\|`. | functional | HIGH |
| **REQ-7** | Hàm kiểm tra tổ hợp thông tin sản phẩm (`validate_product_info`) phải đảm bảo toàn bộ các trường tên, phân loại và đơn vị tính đồng thời thỏa mãn ràng buộc trước khi cho phép tạo sản phẩm. | functional | HIGH |
| **REQ-8** | Đơn giá sản phẩm (Price) dạng số thực `double` phải có giá trị lớn hơn hoặc bằng 0.0 (không chấp nhận giá trị âm). | functional | HIGH |
| **REQ-9** | Chuỗi nhập đơn giá (`price_str`) phải được chuẩn hóa bỏ khoảng trắng thừa, đúng định dạng số thực dương (tối đa 1 dấu chấm thập phân), không được là NULL và không chứa ký tự chữ cái hay ký tự lạ. | functional | HIGH |
| **REQ-10** | Số lượng tồn kho/nhập kho (Quantity) dạng số nguyên `int` phải có giá trị lớn hơn hoặc bằng 0 (không chấp nhận số lượng âm). | functional | HIGH |
| **REQ-11** | Chuỗi nhập số lượng (`qty_str`) phải là chuỗi số nguyên dương hợp lệ, không chứa ký tự chữ, không chứa dấu thập phân, và giá trị parse không được vượt quá `INT_MAX` (2,147,483,647) nhằm triệt tiêu nguy cơ tràn số nguyên (Integer Overflow). | safety | HIGH |
| **REQ-12** | Nghiệp vụ xuất kho (`validate_export_quantity`) phải kiểm tra số lượng xuất lớn hơn 0 và không vượt quá số lượng tồn kho thực tế (`export_quantity <= current_stock`), tồn kho không được âm. | functional | HIGH |
| **REQ-13** | Nghiệp vụ nhập kho (`check_addition_overflow`) phải thực hiện kiểm tra an toàn số học: tổng số lượng sau khi cộng dồn không được vượt quá giới hạn cực đại kiểu số nguyên (`current_stock > INT_MAX - add_quantity`) trước khi thực hiện phép cộng thực tế. | safety | HIGH |

---

## 2. MA TRẬN ÁNH XẠ ĐẶC TẢ SANG MÃ NGUỒN (MAPPING SPEC → CODE) [Yêu cầu B]

| Mã Yêu Cầu | Hàm Hiện Thực (Code C) | Tệp Mã Nguồn | Vị Trí Dòng | Mã Lỗi / Trạng Thái Trả Về |
| :--- | :--- | :--- | :--- | :--- |
| **REQ-1** | `validate_product_id` | `source/src/validation.c` | L14–16 | `STATUS_ERR_NULL_PTR` (-1) |
| **REQ-2** | `validate_product_id` | `source/src/validation.c` | L18–21 | `STATUS_ERR_INVALID_ID` (-2) |
| **REQ-3** | `validate_product_id` | `source/src/validation.c` | L23–28 | `STATUS_ERR_INVALID_ID` (-2) |
| **REQ-4** | `validate_product_name` | `source/src/validation.c` | L38–60 | `STATUS_ERR_NULL_PTR`, `STATUS_ERR_INVALID_NAME` (-9) |
| **REQ-5** | `validate_category` | `source/src/validation.c` | L62–84 | `STATUS_ERR_NULL_PTR`, `STATUS_ERR_INVALID_CATEGORY` (-10) |
| **REQ-6** | `validate_unit` | `source/src/validation.c` | L86–108 | `STATUS_ERR_NULL_PTR`, `STATUS_ERR_INVALID_UNIT` (-11) |
| **REQ-7** | `validate_product_info` | `source/src/validation.c` | L119–133 | Mã lỗi tương ứng theo trường vi phạm |
| **REQ-8** | `validate_price` | `source/src/validation.c` | L136–141 | `STATUS_ERR_INVALID_PRICE` (-13) |
| **REQ-9** | `validate_price_str` | `source/src/validation.c` | L199–255 | `STATUS_ERR_NULL_PTR`, `STATUS_ERR_INVALID_PRICE` (-13) |
| **REQ-10** | `validate_quantity` | `source/src/validation.c` | L143–148 | `STATUS_ERR_INVALID_QTY` (-3) |
| **REQ-11** | `validate_quantity_str` | `source/src/validation.c` | L150–197 | `STATUS_ERR_OVERFLOW` (-6), `STATUS_ERR_INVALID_QTY` (-3) |
| **REQ-12** | `validate_export_quantity` | `source/src/validation.c` | L285–295 | `STATUS_ERR_INVALID_QTY` (-3), `STATUS_ERR_INSUFFICIENT_STOCK` (-5) |
| **REQ-13** | `check_addition_overflow` | `source/src/validation.c` | L297–311 | `STATUS_ERR_OVERFLOW` (-6), `STATUS_ERR_INVALID_QTY` (-3) |

---

## 3. MÔ HÌNH HÓA ĐIỀU KIỆN & TEST CASES (VALID / BOUNDARY / ABNORMAL) [Yêu cầu A & C]

### 3.1. Nhóm 1: Kiểm tra mã sản phẩm (ID)
* **Đặc tả toán học / điều kiện:**  
  $$\text{id} \neq \text{NULL} \quad \wedge \quad 1 \le \text{strlen(id)} \le 19 \quad \wedge \quad \forall c \in \text{id}: c \notin \{\text{space}, \text{cntrl}, '|', ',', ';'\}$$
* **Bộ dữ liệu kiểm thử:**
  - **Hợp lệ (Valid):** `"SP001"` $\rightarrow$ `STATUS_SUCCESS` (0)
  - **Biên cực tiểu (Boundary Min):** `"A"` (độ dài = 1) $\rightarrow$ `STATUS_SUCCESS` (0)
  - **Biên cực đại (Boundary Max):** `"AAAAAAAAAAAAAAAAAAA"` (độ dài = 19) $\rightarrow$ `STATUS_SUCCESS` (0)
  - **Vượt biên (Boundary Overflow):** Chuỗi 20 ký tự $\rightarrow$ `STATUS_ERR_INVALID_ID` (-2)
  - **Bất thường (Abnormal):**
    - `NULL` $\rightarrow$ `STATUS_ERR_NULL_PTR` (-1)
    - `""` (rỗng) $\rightarrow$ `STATUS_ERR_INVALID_ID` (-2)
    - `"SP 001"` (chứa space) $\rightarrow$ `STATUS_ERR_INVALID_ID` (-2)
    - `"SP|001"`, `"SP,001"`, `"SP;001"` $\rightarrow$ `STATUS_ERR_INVALID_ID` (-2)

### 3.2. Nhóm 2: Thông tin sản phẩm (Name, Category, Unit)
* **Đặc tả toán học / điều kiện:**  
  - Tên: $\text{name} \neq \text{NULL} \wedge 1 \le \text{strlen} \le 99 \wedge \text{not\_all\_whitespace} \wedge \forall c \notin \{\text{cntrl}, '|', '\backslash n', '\backslash r'\}$
  - Đơn vị: $\text{unit} \neq \text{NULL} \wedge 1 \le \text{strlen} \le 19 \wedge \text{not\_all\_whitespace} \wedge \forall c \notin \{\text{cntrl}, '|', '\backslash n', '\backslash r'\}$
  - Phân loại: $\text{category} == \text{NULL} \vee (1 \le \text{strlen} \le 49 \wedge \dots)$
* **Bộ dữ liệu kiểm thử:**
  - **Hợp lệ:** `"Laptop Dell XPS"`, `"Dien tu"`, `"Chiec"` $\rightarrow$ `STATUS_SUCCESS` (0)
  - **Biên:** Name 99 ký tự $\rightarrow$ Thành công; Name 100 ký tự $\rightarrow$ `STATUS_ERR_INVALID_NAME` (-9)
  - **Bất thường:** `NULL`, `"   "`, `"Laptop|Dell"`, `"Laptop\nDell"` $\rightarrow$ Lỗi tương ứng.

### 3.3. Nhóm 3: Giá và Số lượng (Price & Quantity)
* **Đặc tả toán học / điều kiện:**  
  - $\text{price} \ge 0.0$
  - $\text{quantity} \ge 0 \wedge \text{quantity} \le \text{INT\_MAX}$
* **Bộ dữ liệu kiểm thử:**
  - **Hợp lệ:** `price = 1500.50`, `quantity = 100` $\rightarrow$ `STATUS_SUCCESS` (0)
  - **Biên cực tiểu:** `price = 0.0`, `quantity = 0` $\rightarrow$ `STATUS_SUCCESS` (0)
  - **Biên cực đại:** `quantity_str = "2147483647"` (`INT_MAX`) $\rightarrow$ `STATUS_SUCCESS` (0)
  - **Vượt biên số nguyên (Integer Overflow):** `quantity_str = "2147483648"` (`INT_MAX + 1`) $\rightarrow$ `STATUS_ERR_OVERFLOW` (-6)
  - **Bất thường:** `price = -0.01` $\rightarrow$ `STATUS_ERR_INVALID_PRICE` (-13); `qty_str = "abc"`, `"-5"`, `"12.5"` $\rightarrow$ `STATUS_ERR_INVALID_QTY` (-3).

### 3.4. Nhóm 4: Logic xuất nhập kho & Chống tràn số bộ nhớ
* **Đặc tả toán học / điều kiện:**  
  - Xuất kho: $0 < \text{export\_quantity} \le \text{current\_stock}$
  - Nhập kho an toàn: $\text{add\_quantity} > 0 \wedge \text{current\_stock} \ge 0 \wedge \text{current\_stock} \le (\text{INT\_MAX} - \text{add\_quantity})$
* **Bộ dữ liệu kiểm thử:**
  - **Hợp lệ:** Tồn 100, xuất 50 $\rightarrow$ `STATUS_SUCCESS` (0); Tồn 100, nhập 200 $\rightarrow$ `STATUS_SUCCESS` (0)
  - **Biên:** Tồn 50, xuất 50 $\rightarrow$ `STATUS_SUCCESS` (0); Tồn `INT_MAX - 100`, nhập 100 $\rightarrow$ `STATUS_SUCCESS` (0)
  - **Bất thường / Nguy cơ an toàn:**
    - Tồn 50, xuất 51 $\rightarrow$ `STATUS_ERR_INSUFFICIENT_STOCK` (-5)
    - Xuất/nhập $\le 0$ $\rightarrow$ `STATUS_ERR_INVALID_QTY` (-3)
    - Tồn `INT_MAX - 100`, nhập 101 $\rightarrow$ `STATUS_ERR_OVERFLOW` (-6) (Bảo vệ tràn số nguyên 32-bit).

---

## 4. KẾT QUẢ THỰC NGHIỆM KIỂM THỬ (EVIDENCE TEST RUN) [Yêu cầu E]

* File kiểm thử tự động: `tests/test_validation.c`
* Lệnh biên dịch: `gcc -Wall -Wextra -I./source/include -o test_validation.exe tests/test_validation.c source/src/validation.c source/src/utils.c`
* Kết quả thực thi:
  ```text
  ====================================================================
       BO KIEM THU VALIDATION SPEC & BOUNDARY/ABNORMAL TEST SUITE     
       Nguoi thuc hien: Ha Van Do (Task 7 - P0)                       
  ====================================================================

  === 1. KIEM THU NHOM 1: MA SAN PHAM (ID) ===
    [PASS] REQ-V2 [Valid]: Ma san pham tieu chuan 'SP001'
    [PASS] REQ-V2 [Boundary Min]: Do dai = 1 ky tu
    [PASS] REQ-V2 [Boundary Max]: Do dai = 19 ky tu (MAX_ID_LEN - 1)
    [PASS] REQ-V2 [Boundary Overflow]: Do dai = 20 ky tu (>= MAX_ID_LEN) bi chan
    [PASS] REQ-V2 [Abnormal]: Chuoi rong
    [PASS] REQ-V1 [Abnormal]: Con tro NULL
    [PASS] REQ-V3 [Abnormal]: Chanh chua khoang trang
    [PASS] REQ-V3 [Abnormal]: Chanh chua ky tu tab/control
    [PASS] REQ-V3 [Abnormal]: Chanh chua dau phan cach '|'
    [PASS] REQ-V3 [Abnormal]: Chanh chua dau phan cach ','
    [PASS] REQ-V3 [Abnormal]: Chanh chua dau phan cach ';'

  === 2. KIEM THU NHOM 2: THONG TIN SAN PHAM (NAME, CATEGORY, UNIT) ===
    [PASS] REQ-V4 [Valid]: Ten hop le
    [PASS] REQ-V4 [Abnormal]: Name NULL
    [PASS] REQ-V4 [Abnormal]: Name chuoi rong
    [PASS] REQ-V4 [Abnormal]: Name toan khoang trang
    [PASS] REQ-V4 [Abnormal]: Name chua dau phan cach '|'
    [PASS] REQ-V4 [Abnormal]: Name chua ky tu xuong dong
    [PASS] REQ-V4 [Boundary Max]: Name 99 ky tu hop le
    [PASS] REQ-V4 [Boundary Overflow]: Name 100 ky tu bi chan
    [PASS] REQ-V5 [Valid]: Category hop le
    [PASS] REQ-V5 [Abnormal]: Category NULL
    [PASS] REQ-V5 [Abnormal]: Category toan khoang trang
    [PASS] REQ-V5 [Abnormal]: Category chua '|'
    [PASS] REQ-V6 [Valid]: Unit hop le
    [PASS] REQ-V6 [Abnormal]: Unit NULL
    [PASS] REQ-V6 [Abnormal]: Unit toan khoang trang
    [PASS] REQ-V6 [Abnormal]: Unit chua '|'
    [PASS] REQ-V7 [Valid]: Bo thong tin san pham hop le
    [PASS] REQ-V7 [Abnormal]: Name loi trong bo thong tin
    [PASS] REQ-V7 [Abnormal]: Category loi trong bo thong tin
    [PASS] REQ-V7 [Abnormal]: Unit loi trong bo thong tin

  === 3. KIEM THU NHOM 3: GIA VA SO LUONG (PRICE & QUANTITY) ===
    [PASS] REQ-V8 [Valid]: Don gia 1500.50 hop le
    [PASS] REQ-V8 [Boundary Min]: Don gia 0.0 hop le
    [PASS] REQ-V8 [Abnormal Negative]: Don gia am bi chan
    [PASS] REQ-V9 [Valid]: Chuoi gia hop le
    [PASS] REQ-V9 [Valid]: Chuoi gia co khoang trang dau/cuoi duoc parse dung
    [PASS] REQ-V9 [Boundary Min]: Chuoi '0'
    [PASS] REQ-V9 [Abnormal]: Chuoi gia am bi chan
    [PASS] REQ-V9 [Abnormal]: Chuoi gia chua chu cai bi chan
    [PASS] REQ-V9 [Abnormal]: Chuoi gia nhieu dau cham thap phan bi chan
    [PASS] REQ-V9 [Abnormal]: Chuoi gia NULL bi chan
    [PASS] REQ-V10 [Valid]: So luong 100 hop le
    [PASS] REQ-V10 [Boundary Min]: So luong 0 hop le
    [PASS] REQ-V10 [Abnormal Negative]: So luong am bi chan
    [PASS] REQ-V11 [Valid]: Chuoi so luong hop le
    [PASS] REQ-V11 [Boundary Min]: Chuoi so luong '0'
    [PASS] REQ-V11 [Boundary Max]: Chuoi so luong INT_MAX hop le
    [PASS] REQ-V11 [Safety Overflow]: Vuot INT_MAX phat hien tran so nguyen
    [PASS] REQ-V11 [Abnormal]: Chuoi so luong am bi chan
    [PASS] REQ-V11 [Abnormal]: So thap phan truyen vao so luong nguyen bi chan
    [PASS] REQ-V11 [Abnormal]: Chuoi so luong ky tu chu bi chan
    [PASS] REQ-V11 [Abnormal]: Chuoi so luong NULL bi chan

  === 4. KIEM THU NHOM 4: LOGIC KHO & AN TOAN TRAN SO NGUYEN ===
    [PASS] REQ-V12 [Valid]: Xuat kho binh thuong (ton 100, xuat 50)
    [PASS] REQ-V12 [Boundary Max]: Xuat kho bang dung ton kho (ton 50, xuat 50)
    [PASS] REQ-V12 [Abnormal]: Xuat kho bang 0 bi chan
    [PASS] REQ-V12 [Abnormal]: Xuat kho am bi chan
    [PASS] REQ-V12 [Abnormal]: Xuat kho vuot ton kho (ton 50, xuat 51) bao loi khong du hang
    [PASS] REQ-V12 [Abnormal]: Ton kho am bi chan
    [PASS] REQ-V13 [Valid]: Cong binh thuong (100 + 200)
    [PASS] REQ-V13 [Boundary Max]: Cong cham dung nguong INT_MAX
    [PASS] REQ-V13 [Safety Overflow]: Cong vuot nguong INT_MAX bi chan triet de
    [PASS] REQ-V13 [Abnormal]: So luong nhap = 0 bi chan
    [PASS] REQ-V13 [Abnormal]: So luong nhap am bi chan
    [PASS] REQ-V13 [Abnormal]: Ton kho hien tai am bi chan

  --------------------------------------------------------------------
  KET QUA KIEM THU: Passed 64 / 64 tests (100.00%)
  --------------------------------------------------------------------
  ```
