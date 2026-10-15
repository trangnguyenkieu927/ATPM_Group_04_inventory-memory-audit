# BÁO CÁO KIỂM CHỨNG HÌNH THỨC SỐ HỌC BẰNG SMT SOLVER (Z3)

**Học phần:** An toàn phần mềm (CSE703093)  
**Đề tài:** Kiểm chứng an toàn bộ nhớ cho chương trình C mô phỏng nghiệp vụ quản lý kho  
**Phụ trách chính:** Hà Văn Đô – Kỹ sư Kiểm chứng hình thức & Mô hình bộ nhớ  
**Công cụ thực thi:** Z3 Theorem Prover 5.1.0 - 64 bit (`C:\Du_an\ATPM\lab5_sat_smt_verification\.deps\bin\z3.exe`)  
**Kiến trúc ABI mô hình hóa:** Windows LLP64 (`sizeof(size_t) = 8`, `sizeof(int) = 4`, `sizeof(Product) = 208` byte)  
**Nhánh Git:** `audit/step3-formal-verification`  
**Ngày thực nghiệm:** 15/10/2026  

---

## 1. Mục tiêu và phạm vi kiểm chứng SMT

Bước 2 đã xác định 10 danh mục rủi ro bộ nhớ (RISK-001 đến RISK-010) và 18 yêu cầu kiểm toán (REQ-1 đến REQ-18). Trong số đó, các lỗi tràn số nguyên (CWE-190: Integer Overflow or Wraparound) là tiền đề trực tiếp dẫn đến:
1. Tính sai kích thước bộ đệm heap trong `realloc(..., new_capacity * sizeof(Product))`.
2. Tràn số nguyên có dấu gây âm tồn kho hoặc sai lệch số lượng hàng hóa.
3. Vòng lặp cấp phát vô tận hoặc tràn con số thống kê báo cáo.

Mục tiêu của Giai đoạn 2 là sử dụng lý thuyết Bit-Vector (`QF_BV`) trong SMT-LIB v2 để kiểm chứng tính khả thỏa (Satisfiability) của các biểu thức số học thực tế trong mã nguồn C, phân biệt rõ giữa:
- **Khả thỏa số học thuần túy (Pure Arithmetic Satisfiability):** Tồn tại nghiệm toán học làm biểu thức vượt ngưỡng bit-width.
- **Tính khả đạt trong thực tế (Reachability under Program Semantics):** Liệu trạng thái tràn số đó có thể kích hoạt được từ trạng thái khởi tạo hợp lệ và luồng điều khiển của chương trình hay không.

---

## 2. Bảng tổng hợp kết quả thực thi Z3 Solver

| Mã bài toán | Yêu cầu truy vết | Tệp nguồn & Hàm C | Biểu thức mô hình | Giả định / Điều kiện kiểm tra | Kết quả Z3 | Model nghiệm (nếu SAT) | Đánh giá tính khả đạt |
| :--- | :--- | :--- | :--- | :--- | :---: | :--- | :--- |
| **SMT-001A** | REQ-2, RISK-001 | `product.c`<br>`product_list_add` | `capacity * 2`<br>`(_ BitVec 64)` | `capacity > 0`<br>Không ràng buộc biên trên | **SAT** | `capacity = 0x8000000000000000`<br>($9,223,372,036,854,775,808$) | **KHÔNG KHẢ ĐẠT THỰC TẾ**<br>(Cần $1.9 \times 10^{21}$ byte RAM, vượt xa dung lượng vật lý) |
| **SMT-001B** | REQ-2, RISK-002 | `product.c`<br>`product_list_add` | `capacity * 2`<br>`(_ BitVec 64)` | `capacity <= 330,382,099`<br>(Ngưỡng RAM 64GB tối đa) | **UNSAT** | Không có nghiệm | **CHỨNG MINH AN TOÀN TRONG BIÊN**<br>Không thể tràn khi capacity nằm trong giới hạn RAM |
| **SMT-002A** | REQ-3, RISK-003 | `product.c`<br>`product_list_add` | `new_cap * 208`<br>`(_ BitVec 64)` | `new_cap > 0`<br>Không có guard kiểm tra tràn | **SAT** | `new_cap = 0xeea85800eaaaaaaa`<br>($\ge 88,686,269,585,142,076$) | **RỦI RO LÝ THUYẾT NẾU THIẾU GUARD**<br>(Nếu `new_cap` bị ép giá trị tùy ý) |
| **SMT-002B** | REQ-3, RISK-003 | `product.c`<br>`product_list_add` | `new_cap * 208`<br>`(_ BitVec 64)` | Có guard kiểm tra biên:<br>`new_cap <= SIZE_MAX / 208` | **UNSAT** | Không có nghiệm | **CHỨNG MINH CƠ CHẾ PHÒNG VỆ ĐẠT CHUẨN**<br>Biểu thức không thể tràn khi tuân thủ guard |
| **SMT-003A** | REQ-4, RISK-005 | `inventory.c`<br>`inventory_add_stock` | `current_stock + add_quantity`<br>`(_ BitVec 32)` | `current_stock >= 0`<br>`add_quantity > 0`<br>Bỏ qua guard | **SAT** | `current_stock = 0x7FFFFFFF`<br>`add_quantity = 0x7FFFFFFF` | **KHẢ ĐẠT NẾU THIẾU GUARD**<br>Chứng minh phép cộng C trần có lỗ hổng CWE-190 |
| **SMT-003B** | REQ-4, RISK-005 | `validation.c`<br>`check_addition_overflow` | `current_stock + add_quantity`<br>`(_ BitVec 32)` | Có guard chuẩn `validation.c`:<br>`current_stock <= INT_MAX - add_quantity` | **UNSAT** | Không có nghiệm | **CHỨNG MINH GUARD VALIDATION HOÀN TOÀN AN TOÀN**<br>Nhánh cộng được bảo vệ triệt để |
| **SMT-004A** | REQ-4, RISK-005 | `report.c`<br>`generate_inventory_summary` | `cat_total + next_qty`<br>`(_ BitVec 32)` | Bỏ qua guard cộng dồn | **SAT** | `cat_total = 0x7FFFFFFF`<br>`next_qty = 0x00000001` | **RỦI RO NẾU DỮ LIỆU TỒN KHO LỚN** |
| **SMT-004B** | REQ-4, RISK-005 | `report.c`<br>`generate_inventory_summary` | `cat_total + next_qty`<br>`(_ BitVec 32)` | Có guard kiểm tra trước khi cộng | **UNSAT** | Không có nghiệm | **AN TOÀN KHI CÓ GUARD** |
| **SMT-005A** | REQ-17, RISK-009 | `file_io.c`<br>`load_products_from_file` | `existing_bytes + incoming`<br>`(_ BitVec 32)` | Không giới hạn kích thước tệp | **SAT** | `existing_bytes = 0x7FFFFFFF`<br>`incoming = 0x00000001` | **RỦI RO KHI TỆP CỰC LỚN (> 2GB)** |
| **SMT-005B** | REQ-17, RISK-009 | `file_io.c`<br>`load_products_from_file` | `existing_bytes + incoming`<br>`(_ BitVec 32)` | Ràng buộc buffer dòng 512 byte & tệp hợp lệ <= 100MB | **UNSAT** | Không có nghiệm | **AN TOÀN TRONG NGHIỆP VỤ THỰC TẾ** |

---

## 3. Phân tích chi tiết từng bài toán SMT trọng tâm

### 3.1. Bài toán SMT-001: Tràn số khi nhân đôi capacity (`product.c`)
- **Vị trí mã nguồn:** `source/src/product.c:116`
  ```c
  size_t new_capacity = list->capacity == 0 ? 4 : list->capacity * 2;
  ```
- **Mô hình hóa hình thức SMT-LIB v2:**
  - `capacity` có kiểu `size_t` được biểu diễn bằng vector bit 64-bit không dấu `(_ BitVec 64)`.
  - Giới hạn trên `SIZE_MAX = 0xFFFFFFFFFFFFFFFF` ($18,446,744,073,709,551,615$).
  - Để phát hiện tràn bit chính xác theo quy chuẩn toán học (tránh bẫy so sánh bit-vector bị cắt cụt), solver mở rộng `capacity` sang vector 65-bit (`zero_extend 1`), thực hiện `bvmul` với 2, và so sánh với giá trị mở rộng của `SIZE_MAX`.
- **Kết quả thực nghiệm Z3:**
  - **SMT-001A (Không ràng buộc biên trên):** Solver trả về `SAT`. Model nghiệm trích xuất: `capacity = #x8000000000000000` ($9,223,372,036,854,775,808$). Khi nhân 2, giá trị vượt quá biểu diễn 64-bit và quay vòng về 0 trong phép toán modulo $2^{64}$.
  - **SMT-001B (Ràng buộc tính khả đạt thực tế):** Khởi tạo `capacity` ban đầu bằng 4. Mỗi sản phẩm chiếm 208 byte (`sizeof(Product)`). Để đạt tới ngưỡng tràn $2^{63}$, hệ thống cần tối thiểu $2^{63} \times 208 \approx 1.9 \times 10^{21}$ byte RAM (gần 2 Zettabytes). Trên hệ thống thực tế tối đa 64GB RAM ($330,382,099$ sản phẩm), solver trả về `UNSAT`.
- **Kết luận:** Lỗ hổng tràn số khi nhân đôi `capacity` là một rủi ro lý thuyết thuần túy số học, **không thể khả đạt trên phần cứng thực tế** trước khi `realloc()` bị từ chối do thiếu bộ nhớ (`OOM - Out Of Memory`).
  - *Đối chiếu Baseline v0 & Nghiệm thu REQ-2*: Trong `source/src/product.c`, baseline chưa có guard tường minh `capacity <= SIZE_MAX / 2`. Yêu cầu REQ-2 được đánh dấu **`PARTIALLY_VERIFIED`** (SMT phân tích rủi ro hoàn tất; baseline chưa có guard; chuyển việc bổ sung guard sang Bước 4).

---

### 3.2. Bài toán SMT-002: Tràn kích thước cấp phát bằng `realloc` (`product.c`)
- **Vị trí mã nguồn:** `source/src/product.c:119`
  ```c
  Product *new_items = realloc(list->items, new_capacity * sizeof(Product));
  ```
- **Mô hình hóa hình thức:**
  - `new_capacity` là `size_t` 64-bit.
  - `sizeof(Product) = 208` byte trên Windows LLP64 ABI.
  - Tích số `new_capacity * 208` được mở rộng sang 128-bit (`zero_extend 64`) để kiểm tra xem có vượt quá `SIZE_MAX` (128-bit) hay không.
  - Ngưỡng giới hạn an toàn: $\lfloor \text{SIZE\_MAX} / 208 \rfloor = 88,686,269,585,142,075$ (`#x013B13B13B13B13B`).
- **Kết quả thực nghiệm Z3:**
  - **SMT-002A (Không có guard kiểm tra):** Trả về `SAT`. Model nghiệm: `new_cap = #xeea85800eaaaaaaa`. Tích số vượt quá $2^{64}-1$, dẫn đến giá trị truyền vào `realloc` bị wraps-around về một số nhỏ, gây cấp phát thiếu bộ nhớ và heap buffer overflow nghiêm trọng.
  - **SMT-002B (Có guard `new_cap <= SIZE_MAX / sizeof(Product)`):** Trả về `UNSAT`. Chứng minh rằng cơ chế kiểm tra biên trước khi gọi allocator là điều kiện đủ để triệt tiêu hoàn toàn rủi ro tràn số cấp phát.
  - *Đối chiếu Baseline v0 & Nghiệm thu REQ-3*: Trong `source/src/product.c`, baseline chưa có guard tường minh `new_cap <= SIZE_MAX / sizeof(Product)`. Yêu cầu REQ-3 được đánh dấu **`PARTIALLY_VERIFIED`** (SMT phân tích rủi ro hoàn tất; baseline chưa có guard; chuyển việc bổ sung guard sang Bước 4).

---

### 3.3. Bài toán SMT-003: Tràn số nguyên khi nhập kho (`inventory.c` & `validation.c`)
- **Vị trí mã nguồn:**
  - `source/src/inventory.c:45`:
    ```c
    product->quantity += quantity;
    ```
  - `source/src/validation.c:297-311`:
    ```c
    int check_addition_overflow(int a, int b) {
        if (b > 0 && a > INT_MAX - b) {
            return STATUS_ERR_OVERFLOW;
        }
        return STATUS_SUCCESS;
    }
    ```
- **Mô hình hóa hình thức:**
  - `current_stock` và `add_quantity` là số nguyên có dấu 32-bit `(_ BitVec 32)`.
  - Tiền điều kiện: `current_stock >= 0` và `add_quantity > 0`.
  - Mở rộng sang 33-bit có dấu (`sign_extend 1`) để phát hiện tràn số `INT_MAX = 2147483647` (`#x7FFFFFFF`).
- **Kết quả thực nghiệm Z3:**
  - **SMT-003A (Bỏ qua guard kiểm tra):** Trả về `SAT`. Model nghiệm: `current_stock = 0x7FFFFFFF`, `add_quantity = 0x7FFFFFFF` (hoặc `add_quantity = 1`). Phép cộng trực tiếp sẽ gây ra tràn số nguyên có dấu (Undefined Behavior theo chuẩn C99/C11), biến số dương thành số âm.
  - **SMT-003B (Bảo vệ bởi guard `validation.c`):** Trả về `UNSAT`. Z3 chứng minh toán học rằng khi thỏa mãn `current_stock <= INT_MAX - add_quantity`, tổng `current_stock + add_quantity` **không bao giờ vượt quá INT_MAX** trong phạm vi 32-bit có dấu.
- **Kết luận:** Hàm kiểm tra phòng vệ `check_addition_overflow` trong `validation.c` đã loại bỏ hoàn toàn khả năng tràn số tại `inventory.c:45`, thỏa mãn REQ-4.

---

## 4. Các giới hạn của mô hình SMT và bài học kinh nghiệm

Tuân thủ nguyên tắc trung thực khoa học:
1. **UNSAT trên mô hình trừu tượng không tương đương với toàn bộ mã C an toàn:**
   - Solver chỉ chứng minh UNSAT dựa trên các tiên đề và giả định (Assumptions) đã đưa vào mô hình SMT. Nếu trong mã C thực tế có đường thực thi bỏ qua hàm kiểm tra (bypass guard), lỗi tràn số vẫn có thể xảy ra.
2. **SAT không đồng nghĩa với việc tồn tại lỗ hổng có thể khai thác:**
   - Việc SMT-001A trả về SAT chỉ phản ánh thuộc tính số học của vành modulo $2^{64}$. Trong môi trường thực tế, hệ điều hành sẽ từ chối cấp phát bộ nhớ từ rất lâu trước khi dung lượng chạm tới ngưỡng tràn.
3. **Giá trị sizeof(Product) phụ thuộc chặt chẽ vào ABI:**
   - Trên LLP64 (Windows), `sizeof(Product) = 208`. Trên các kiến trúc khác hoặc khi có cờ đóng gói struct (`#pragma pack`), hằng số này có thể thay đổi, đòi hỏi phải tái lập mô hình SMT tương ứng.

---

## 5. Bằng chứng thực nghiệm lưu trữ
- Các file đặc tả `.smt2`: `verification/step3/smt/`
- Log thực thi Z3 thực tế: `results/step3/smt/`
- Tổng hợp JSON kết quả: `results/step3/smt/smt_results_summary.json`
