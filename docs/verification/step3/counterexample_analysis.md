# BÁO CÁO PHÂN TÍCH PHẢN VÍ DỤ VÀ NGUYÊN NHÂN GỐC (COUNTEREXAMPLE ANALYSIS)
## Dự án: Kiểm chứng an toàn bộ nhớ cho chương trình C mô phỏng nghiệp vụ quản lý kho
## Giai đoạn: Bước 3 – Kiểm chứng hình thức, Phân tích động và Mô phỏng rủi ro
## Phiên bản: Đã hiệu chỉnh và kiểm toán độc lập (Independent Review Edition)

---

### 1. TỔNG QUAN VỀ CÁC PHẢN VÍ DỤ PHÁT HIỆN ĐƯỢC

Trong quá trình thực thi Bước 3, hệ thống công cụ kiểm chứng (Z3 SMT Solver, CPROVER Contract Assertion Harness qua GCC Test Oracle, Custom Kripke CTL Verifier, GCC Stack Protector, và Bộ mô phỏng rủi ro C) đã phân tích toàn diện 18 REQ và 10 Risk.

Nhóm kiểm chứng tuân thủ nghiêm ngặt nguyên tắc **Trung thực khoa học**:
- Phân biệt rõ ràng giữa **Phản ví dụ số học hình thức** (SMT SAT model), **Lỗi cài có chủ đích trong nhánh thí nghiệm đối chứng** (Seeded vulnerability counterexample), và **Bất biến an toàn trong mã nguồn Baseline v0**.
- Tuyệt đối không che giấu phản ví dụ hoặc sửa đổi assertions/guards để biến kết quả thành PASS giả mạo.

| Mã phản ví dụ | Công cụ phát hiện | REQ liên quan | RISK liên quan | Loại phản ví dụ | Môi trường / Nhánh | Trạng thái an toàn trên Baseline v0 |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| **CTX-SMT-001** | Z3 5.1.0 (Bit-Vector) | REQ-2 | RISK-001 | Phản ví dụ số học (SAT model) | Bit-vector 64-bit lý thuyết | Chưa có guard trong C, nhưng không khả đạt trong RAM vật lý |
| **CTX-SMT-002** | Z3 5.1.0 (Bit-Vector) | REQ-3 | RISK-002 | Phản ví dụ số học (SAT model) | Bit-vector 64-bit lý thuyết | Nguy cơ tràn số cấp phát nếu new_cap vượt trần toán học |
| **CTX-SMT-003** | Z3 5.1.0 (Bit-Vector) | REQ-4 | RISK-003 | Phản ví dụ số học không có guard | Bit-vector 32-bit lý thuyết | ĐÃ ĐƯỢC CHẶN bởi guard `check_addition_overflow()` |
| **CTX-SIM-003** | GCC Stack Canary / C Sim | REQ-16 | RISK-008 | Tràn bộ đệm ngăn xếp thực tế | Nhánh `audit/v0-file-io-vulnerable` | Baseline v0 KHÔNG BỊ (có guard `token_count < 8`) |

---

### 2. CHI TIẾT CÁC PHẢN VÍ DỤ VÀ PHÂN TÍCH NGUYÊN NHÂN

#### 2.1. Phản ví dụ CTX-SMT-001: Tràn số nguyên khi nhân đôi Capacity (REQ-2, RISK-001)

- **Vị trí mã nguồn liên quan**: `source/src/product.c`, hàm `add_product()`:
  ```c
  size_t new_cap = (list->capacity == 0) ? INITIAL_CAPACITY : (list->capacity * 2);
  ```
- **Công cụ phát hiện**: Z3 Solver 5.1.0, tệp mô hình `verification/step3/smt/smt_001a_capacity_growth.smt2`.
- **Kết quả SMT**: `sat`.
- **Mô hình nghiệm cụ thể (Counterexample Model)**:
  ```lisp
  (model
    (define-fun capacity () (_ BitVec 64) #x8000000000000000) ; 9,223,372,036,854,775,808
    (define-fun new_cap () (_ BitVec 64) #x0000000000000000)
  )
  ```
- **Phân tích cơ chế vi phạm**:
  - Với bit width 64-bit (`size_t` trên x86_64 / LLP64), khi `capacity >= 0x8000000000000000` ($2^{63}$), phép nhân $2 \times \text{capacity}$ làm mất bit trọng số lớn nhất, khiến kết quả quay vòng (wraparound) về $0$ hoặc một số nhỏ hơn.
  - Sau đó hàm `realloc(list->items, new_cap * sizeof(Product))` sẽ gọi `realloc(..., 0)` dẫn đến việc giải phóng bộ nhớ ngoài ý muốn hoặc cấp phát sai kích thước.
- **Đánh giá tính khả đạt trên Baseline v0**:
  - **Khả đạt lý thuyết**: Tồn tại nếu `capacity` đạt tới $2^{63}$.
  - **Khả đạt thực tế trên Baseline v0**: **KHÔNG KHẢ ĐẠT** trong điều kiện vận hành thông thường, bởi vì:
    1. Giá trị khởi tạo là $16$.
    2. Mỗi phần tử `Product` chiếm $208$ bytes. Để đạt $2^{63}$ phần tử, hệ thống cần cấp phát $2^{63} \times 208 \approx 3.8 \times 10^{21}$ bytes RAM, vượt xa dung lượng địa chỉ ảo và RAM vật lý của bất kỳ máy tính nào hiện nay.
    3. Bộ cấp phát hệ thống sẽ ném lỗi OOM (Out Of Memory) và `realloc` trả về `NULL` (`STATUS_ERR_MEMORY = -8`) từ rất sớm.
  - **Đề xuất khắc phục (Bước 4)**: Cần bổ sung guard kiểm tra tường minh trước phép nhân:
    ```c
    if (list->capacity > SIZE_MAX / 2) return STATUS_ERR_MEMORY;
    ```

---

#### 2.2. Phản ví dụ CTX-SMT-002: Tràn số kích thước cấp phát Realloc (REQ-3, RISK-002)

- **Vị trí mã nguồn liên quan**: `source/src/product.c`, hàm `add_product()`:
  ```c
  Product *new_items = (Product *)realloc(list->items, new_cap * sizeof(Product));
  ```
- **Công cụ phát hiện**: Z3 Solver 5.1.0, tệp mô hình `verification/step3/smt/smt_002a_realloc_size.smt2`.
- **Kết quả SMT**: `sat`.
- **Ranh giới an toàn toán học đối với unsigned 64-bit**:
  $$\text{Ngưỡng tối đa an toàn} = \left\lfloor \frac{2^{64} - 1}{208} \right\rfloor = \left\lfloor \frac{18,446,744,073,709,551,615}{208} \right\rfloor = \mathbf{88,686,269,585,142,075}$$
- **Mô hình nghiệm thực tế do Z3 5.1.0 trả về (Counterexample Model)**:
  ```lisp
  (model
    (define-fun new_cap () (_ BitVec 64) #xeea85800eaaaaaaa) ; 17,197,091,938,074,536,618
    (define-fun sizeof_product () (_ BitVec 64) #x00000000000000d0) ; 208 bytes
  )
  ```
- **Phân tích cơ chế và số học vi phạm**:
  - Giá trị nghiệm của Z3: $\text{new\_cap} = 17,197,091,938,074,536,618$.
  - Rõ ràng: $17,197,091,938,074,536,618 > 88,686,269,585,142,075$ (vượt xa ngưỡng an toàn tối đa).
  - Phép nhân toán học ở độ rộng mở rộng 128-bit:
    $$17,197,091,938,074,536,618 \times 208 = \mathbf{3,576,995,123,119,503,616,544} > 2^{64} - 1$$
  - Khi biểu diễn trong kiểu `size_t` 64-bit của ngôn ngữ C, kết quả bị cắt gọt theo modulo $2^{64}$ (64-bit wraparound):
    $$3,576,995,123,119,503,616,544 \pmod{2^{64}} = \mathbf{16,773,516,893,560,154,656} \text{ bytes}$$
  - **Hậu quả bảo mật**: `realloc` sẽ nhận một kích thước nhỏ hơn rất nhiều so với dung lượng cần thiết để chứa `new_cap` phần tử, dẫn tới tình trạng cấp phát thiếu bộ đệm nghiêm trọng (Heap Buffer Under-allocation).
  - *(Lưu ý hiệu chỉnh)*: Một số bản thảo trước đây từng trích dẫn nhầm số $88,683,131,100,535,260$ (nhỏ hơn ngưỡng $88,686,269,585,142,075$). Phép kiểm toán độc lập xác nhận rằng nghiệm thực tế từ Z3 là `#xeea85800eaaaaaaa` ($17,197,091,938,074,536,618$) và vi phạm hoàn toàn bất biến tràn số.
- **Đánh giá tính khả đạt và Đề xuất khắc phục**:
  - Không thể xảy ra tự nhiên từ cấu hình $16$ phần tử trong giới hạn RAM máy tính hiện nay.
  - Tuy nhiên, đây là rủi ro tiềm ẩn nếu cấu trúc danh sách bị can thiệp.
  - **Đề xuất cho Bước 4**: Bổ sung guard an toàn số học:
    ```c
    if (new_cap > SIZE_MAX / sizeof(Product)) return STATUS_ERR_MEMORY;
    ```

---

#### 2.3. Phản ví dụ CTX-SMT-003: Tràn số nguyên cộng dồn tồn kho khi Bỏ qua Guard (REQ-4, RISK-003)

- **Vị trí mã nguồn liên quan**: `source/src/inventory.c`, hàm `import_stock()`:
  ```c
  if (check_addition_overflow(p->quantity, quantity)) {
      return STATUS_ERR_OVERFLOW;
  }
  p->quantity += quantity;
  ```
- **Công cụ phát hiện**: Z3 Solver 5.1.0, tệp mô hình `verification/step3/smt/smt_003a_stock_addition.smt2`.
- **Kết quả SMT**: `sat` khi không có guard; `unsat` khi có guard (SMT-003B).
- **Mô hình nghiệm (Counterexample Model khi bỏ guard)**:
  ```lisp
  (model
    (define-fun current_stock () (_ BitVec 32) #x7fffffff) ; 2,147,483,647 (INT_MAX)
    (define-fun add_quantity () (_ BitVec 32) #x00000001) ; 1
    (define-fun resulting_stock () (_ BitVec 32) #x80000000) ; -2,147,483,648 (Số âm!)
  )
  ```
- **Phân tích đối chiếu với Baseline v0**:
  - Nếu không có hàm bảo vệ `check_addition_overflow()`, việc cộng 1 vào `INT_MAX` sẽ gây ra Signed Integer Overflow (Hành vi không xác định - Undefined Behavior theo chuẩn C99/C11), dẫn đến giá trị tồn kho bị biến thành số âm.
  - **Trên Baseline v0 thực tế**: Nhánh `source/src/validation.c` đã triển khai hàm kiểm tra:
    ```c
    bool check_addition_overflow(int a, int b) {
        if (b > 0 && a > INT_MAX - b) return true;
        return false;
    }
    ```
  - Z3 xác nhận truy vấn `smt_003b_stock_addition_guard.smt2` là **UNSAT**, và bài toán assertion harness `harness_cbmc_005_stock_safety.c` đã kiểm chứng thành công rằng phép cộng sau guard ngăn chặn được nguy cơ tràn số nguyên có dấu trong phạm vi kiểm chứng.

---

#### 2.4. Phản ví dụ CTX-SIM-003: Tràn bộ đệm ngăn xếp trên Nhánh cài lỗi chủ đích (REQ-16, RISK-008)

- **Vị trí mã nguồn**: `tests/simulation/vulnerable_file_io.c` (trích xuất từ commit `d7cbe365714f9799e49c8aa0fd889d606a9878cd` trên nhánh `audit/v0-file-io-vulnerable`).
- **So sánh mã nguồn đối chứng**:
  - **Mã lỗi cài chủ đích (Seeded branch)**:
    ```c
    char *tokens[6];
    int token_count = 0;
    while (cursor != NULL) { // LỖI CỐ Ý: mảng chỉ có 6 phần tử nhưng không kiểm tra token_count < 6!
        tokens[token_count++] = cursor;  // Stack Buffer Overflow khi token_count >= 6
        ...
    }
    ```
  - **Mã nguồn Baseline v0 (`source/src/file_io.c`)**:
    ```c
    char *tokens[8];
    int token_count = 0;
    while (token != NULL) {
        if (token_count < 8) {
            tokens[token_count++] = token; // ĐƯỢC BẢO VỆ CHẶT CHẼ
        } else {
            token_count++; // Đếm để phát hiện định dạng thừa trường
        }
        token = strtok(NULL, ",");
    }
    ```
- **Đầu vào kích hoạt (Triggering Input)**: `tests/simulation/corpus/corpus_03_too_many_delimiters.txt` (dòng chứa 10 trường dữ liệu phân cách bởi dấu `|`).
- **Hiện tượng thực nghiệm**:
  - Khi chạy trên nhánh lỗi `vulnerable_file_io.c`:
    - Quá trình ghi vượt mảng `tokens[6]` ghi đè lên stack frame và stack canary.
    - Cơ chế bảo vệ stack của trình biên dịch GCC phát hiện canary bị phá hủy và gọi trình xử lý ngoại lệ:
      ```text
      *** stack smashing detected ***: terminated
      Exit code: 0xC0000409 (STATUS_STACK_BUFFER_OVERRUN)
      ```
  - Khi chạy cùng tệp corpus này trên Baseline v0 (`test_sim002_parser.c`):
    - Hàm nhận diện `token_count = 10 != 6 && token_count != 4`, từ chối dòng và trả về mã lỗi `STATUS_ERR_FILE_IO = -7`.
    - Stack và bộ đệm nguyên vẹn, không phát sinh tràn mảng trong kịch bản này, chương trình hoàn thành với exit code 0.
- **Ý nghĩa kiểm chứng**: 
  - Thí nghiệm đối chứng chứng minh cơ chế `-fstack-protector-strong` có khả năng phát hiện lỗi tràn mảng stack buffer (CWE-120) trong kịch bản này.
  - Tuy nhiên, nhóm ghi nhận rõ rằng **một phép thử đơn lẻ không thể xem là bằng chứng công cụ có độ nhạy tuyệt đối đối với mọi loại lỗ hổng an toàn bộ nhớ**.

---

### 3. XÁC MINH MÃ LỖI TRONG THÍ NGHIỆM SIM-001.2

Trong thí nghiệm mô phỏng `realloc` thất bại bằng Fault Injection (`tests/simulation/test_sim001_heap.c`):
- Khi `realloc()` trả về `NULL`, hàm `add_product()` trong `source/src/product.c` thực thi:
  ```c
  if (new_items == NULL) {
      return STATUS_ERR_MEMORY;
  }
  ```
- Theo định nghĩa chuẩn trong `source/include/models.h:41`:
  ```c
  #define STATUS_ERR_MEMORY -8
  ```
- Mã lỗi trả về của hàm C: **`-8`** (`STATUS_ERR_MEMORY`).
- Mã kết thúc của tiến trình test (Process exit code): **`0`** (Thành công, vì harness kiểm tra `assert(status == STATUS_ERR_MEMORY)` và xác nhận `list.items` cũ được giữ nguyên).
- *Hiệu chỉnh*: Đã sửa các chỗ trong báo cáo từng ghi nhầm mã lỗi thành `-2` (`-2` thực tế là `STATUS_ERR_INVALID_ID`).

---

### 4. ĐỀ XUẤT CÁC ĐIỂM CẦN NÂNG CẤP TRONG BƯỚC 4 (DEFENSIVE HARDENING)

Dựa trên các phản ví dụ số học lý thuyết thu được từ SMT Z3:
1. **Bổ sung guard chống tràn số cho Capacity**:
   - File: `source/src/product.c`
   - Bổ sung kiểm tra `if (list->capacity > SIZE_MAX / 2) return STATUS_ERR_MEMORY;` trước khi tính `new_cap`.
2. **Bổ sung guard chống tràn số kích thước Heap Realloc**:
   - File: `source/src/product.c`
   - Bổ sung kiểm tra `if (new_cap > SIZE_MAX / sizeof(Product)) return STATUS_ERR_MEMORY;`.
3. **Tường minh hóa quy ước con trỏ mượn (Borrowed Pointer Contract)**:
   - File: `source/include/product.h`
   - Tài liệu hóa quy tắc không lưu con trỏ tham chiếu tới `list->items[i]` qua các thao tác có khả năng thay đổi kích thước container (`add_product`, `product_list_free`).
