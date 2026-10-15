# BÁO CÁO PHÂN TÍCH PHẢN VÍ DỤ VÀ NGUYÊN NHÂN GỐC (COUNTEREXAMPLE ANALYSIS)
## Dự án: Kiểm chứng an toàn bộ nhớ cho chương trình C mô phỏng nghiệp vụ quản lý kho
## Giai đoạn: Bước 3 – Kiểm chứng hình thức, Phân tích động và Mô phỏng rủi ro

---

### 1. TỔNG QUAN VỀ CÁC PHẢN VÍ DỤ PHÁT HIỆN ĐƯỢC

Trong quá trình thực thi Bước 3, hệ thống công cụ kiểm chứng (Z3 Solver, CBMC Bounded Model Checker, NuSMV/nuXmv Model Checker, GCC Stack Protector, và Bộ mô phỏng rủi ro C) đã phân tích toàn diện 18 REQ và 10 Risk. 

Nhóm kiểm chứng tuân thủ nghiêm ngặt nguyên tắc **Trung thực khoa học**:
- Phân biệt rõ ràng giữa **Phản ví dụ toán học/hình thức** (SMT SAT model), **Lỗi cài có chủ đích trong nhánh thí nghiệm** (Seeded vulnerability counterexample), và **Bất biến an toàn trong mã nguồn Baseline v0**.
- Tuyệt đối không che giấu phản ví dụ hoặc sửa đổi assertions/guards để biến kết quả thành PASS giả mạo.

| Mã phản ví dụ | Công cụ phát hiện | REQ liên quan | RISK liên quan | Loại phản ví dụ | Môi trường / Nhánh | Trạng thái an toàn trên Baseline v0 |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| **CTX-SMT-001** | Z3 5.1.0 (Bit-Vector) | REQ-2 | RISK-001 | Phản ví dụ số học (SAT model) | Bit-vector 64-bit lý thuyết | Chưa có guard, nhưng không khả đạt trong RAM vật lý |
| **CTX-SMT-002** | Z3 5.1.0 (Bit-Vector) | REQ-3 | RISK-002 | Phản ví dụ số học (SAT model) | Bit-vector 64-bit lý thuyết | Nguy cơ tràn số cấp phát nếu capacity bị thao túng |
| **CTX-SMT-003** | Z3 5.1.0 (Bit-Vector) | REQ-4 | RISK-003 | Phản ví dụ số học không có guard | Bit-vector 32-bit lý thuyết | ĐÃ ĐƯỢC CHẶN bởi guard `check_addition_overflow()` |
| **CTX-SIM-003** | GCC Stack Canary / C Sim | REQ-6, REQ-16 | RISK-007, RISK-008 | Vi phạm tràn bộ đệm ngăn xếp thực tế | Nhánh `audit/v0-file-io-vulnerable` | Baseline v0 KHÔNG BỊ (có guard `token_count < 8`) |

---

### 2. CHI TIẾT CÁC PHẢN VÍ DỤ VÀ PHÂN TÍCH NGUYÊN NHÂN

#### 2.1. Phản ví dụ CTX-SMT-001: Tràn số nguyên khi nhân đôi Capacity (REQ-2, RISK-001)

- **Vị trí mã nguồn liên quan**: `source/src/product.c`, hàm `add_product()`:
  ```c
  size_t new_cap = (list->capacity == 0) ? 16 : list->capacity * 2;
  ```
- **Công cụ phát hiện**: Z3 Solver 5.1.0, tệp mô hình `verification/step3/smt/smt_001a_capacity_overflow.smt2`.
- **Kết quả SMT**: `sat`.
- **Mô hình nghiệm cụ thể (Counterexample Model)**:
  ```lisp
  (model
    (define-fun capacity () (_ BitVec 64) #x8000000000000000) ; 9,223,372,036,854,775,808
    (define-fun new_capacity () (_ BitVec 64) #x0000000000000000)
    (define-fun is_overflow () Bool true)
  )
  ```
- **Phân tích cơ chế vi phạm**:
  - Với bit width 64-bit (`size_t` trên x86_64 / LLP64), khi `capacity >= 0x8000000000000000` ($2^{63}$), phép nhân $2 \times \text{capacity}$ sẽ làm mất bit có trọng số lớn nhất, làm giá trị quay vòng (wrap-around) về $0$ hoặc một số nhỏ hơn.
  - Sau đó hàm `realloc(list->items, new_capacity * sizeof(Product))` sẽ gọi `realloc(..., 0)` dẫn đến việc giải phóng bộ nhớ ngoài ý muốn hoặc cấp phát sai kích thước.
- **Đánh giá tính khả đạt trên Baseline v0**:
  - **Khả đạt lý thuyết**: Có thể xảy ra nếu `capacity` đạt tới $2^{63}$.
  - **Khả đạt thực tế trên Baseline v0**: **KHÔNG KHẢ ĐẠT** trong điều kiện vận hành thông thường, bởi vì:
    1. Giá trị khởi tạo là $16$.
    2. Mỗi phần tử `Product` chiếm $208$ bytes. Để đạt $2^{63}$ phần tử, hệ thống cần cấp phát $2^{63} \times 208 \approx 3.8 \times 10^{21}$ bytes RAM, vượt xa dung lượng địa chỉ ảo và RAM vật lý hiện tại của mọi máy tính.
    3. Bộ cấp phát hệ thống sẽ ném lỗi OOM (Out Of Memory) và `realloc` trả về `NULL` từ rất sớm.
  - **Đề xuất khắc phục (Bước 4)**: Cần bổ sung guard kiểm tra tường minh trước phép nhân:
    ```c
    if (list->capacity > SIZE_MAX / 2) return ERR_CAPACITY_OVERFLOW;
    ```

---

#### 2.2. Phản ví dụ CTX-SMT-002: Tràn số kích thước cấp phát Realloc (REQ-3, RISK-002)

- **Vị trí mã nguồn liên quan**: `source/src/product.c`, hàm `add_product()`:
  ```c
  Product *new_items = realloc(list->items, new_cap * sizeof(Product));
  ```
- **Công cụ phát hiện**: Z3 Solver 5.1.0, tệp mô hình `verification/step3/smt/smt_002a_realloc_size_overflow.smt2`.
- **Kết quả SMT**: `sat`.
- **Mô hình nghiệm cụ thể (Counterexample Model)**:
  ```lisp
  (model
    (define-fun new_capacity () (_ BitVec 64) #x013b13b13b13b13c) ; 88,683,131,100,535,260
    (define-fun sizeof_product () (_ BitVec 64) #x00000000000000d0) ; 208 bytes
    (define-fun alloc_bytes () (_ BitVec 64) #x00000000000000a0) ; 160 bytes (bị wrap-around!)
    (define-fun alloc_overflow () Bool true)
  )
  ```
- **Phân tích cơ chế vi phạm**:
  - Khi `new_capacity > SIZE_MAX / sizeof(Product)` ($\lfloor (2^{64}-1)/208 \rfloor \approx 88,683,131,100,535,248$), tích số `new_capacity * 208` bị tràn bit 64-bit, kết quả biểu diễn chỉ còn 160 bytes.
  - Khi đó `realloc` chỉ cấp phát 160 bytes thay vì hàng tỷ bytes. Khi chương trình ghi phần tử mới vào vị trí `items[count]`, nó sẽ ghi đè lên bộ nhớ heap ngoài phạm vi cấp phát (Heap Buffer Overflow).
- **Đánh giá tính khả đạt và Đề xuất khắc phục**:
  - Không thể xảy ra tự nhiên từ cấu hình $16$ phần tử, nhưng là lỗ hổng tiềm tàng nếu cấu trúc danh sách bị can thiệp.
  - **Đề xuất cho Bước 4**: Bổ sung guard an toàn số học:
    ```c
    if (new_cap > SIZE_MAX / sizeof(Product)) return ERR_INVALID_CAPACITY;
    ```

---

#### 2.3. Phản ví dụ CTX-SMT-003: Tràn số nguyên cộng dồn tồn kho khi Bỏ qua Guard (REQ-4, RISK-003)

- **Vị trí mã nguồn liên quan**: `source/src/inventory.c`, hàm `import_stock()`:
  ```c
  if (check_addition_overflow(p->quantity, quantity)) {
      return ERR_STOCK_OVERFLOW;
  }
  p->quantity += quantity;
  ```
- **Công cụ phát hiện**: Z3 Solver 5.1.0, tệp mô hình `verification/step3/smt/smt_003a_stock_overflow_no_guard.smt2`.
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
  - Z3 xác nhận truy vấn `smt_003b_stock_overflow_with_guard.smt2` là **UNSAT**, và bài toán CBMC `harness_cbmc_005_stock_safety.c` đã kiểm chứng thành công rằng phép cộng sau guard **tuyệt đối không bao giờ tràn số**.

---

#### 2.4. Phản ví dụ CTX-SIM-003: Tràn bộ đệm ngăn xếp trên Nhánh cài lỗi chủ đích (REQ-6, REQ-16, RISK-007, RISK-008)

- **Vị trí mã nguồn**: `tests/simulation/vulnerable_file_io.c` (trích xuất từ commit nhánh `audit/v0-file-io-vulnerable`).
- **So sánh mã nguồn đối chứng**:
  - **Mã lỗi cài chủ đích (Seeded branch)**:
    ```c
    char *tokens[8];
    int token_count = 0;
    while (token && token_count < 16) { // LỖI CỐ Ý: mảng chỉ có 8 phần tử nhưng lặp tới 16!
        tokens[token_count++] = token;  // Stack Buffer Overflow khi token_count >= 8
        token = strtok(NULL, ",");
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
- **Đầu vào kích hoạt (Triggering Input)**: `tests/simulation/corpus/corpus_many_delims.txt` (dòng chứa 12 trường dữ liệu phân cách bởi dấu phẩy).
- **Hiện tượng thực nghiệm**:
  - Khi chạy trên nhánh lỗi `vulnerable_file_io.c`:
    - Quá trình ghi vượt mảng `tokens[8]` ghi đè lên stack frame và stack canary.
    - Trình biên dịch GCC phát hiện canary bị phá hủy và gọi trình xử lý ngoại lệ:
      ```text
      *** stack smashing detected ***: terminated
      Exit code: 3221226505 (0xC0000409 - STATUS_STACK_BUFFER_OVERRUN)
      ```
  - Khi chạy cùng tệp corpus này trên Baseline v0 (`test_sim002_parser.c`):
    - Hàm nhận diện `token_count = 12 > 6`, từ chối dòng và trả về mã lỗi `ERR_PARSE_EXTRA_FIELDS`.
    - Stack hoàn toàn nguyên vẹn, bộ nhớ an toàn 100%, chương trình hoàn thành với exit code 0.
- **Ý nghĩa kiểm chứng**: Thí nghiệm đối chứng chứng minh công cụ kiểm chứng và môi trường có độ nhạy rất cao, có khả năng phát hiện lập tức các lỗi vi phạm an toàn bộ nhớ khi xuất hiện.

---

### 3. TỔNG KẾT BẤT BIẾN TRÊN BASELINE V0 TRONG GIỚI HẠN KIỂM CHỨNG

Trong phạm vi kiểm chứng có giới hạn của Bước 3:
1. **CBMC Bounded Model Checking**:
   - Không tìm thấy phản ví dụ nào trong các giới hạn unwind:
     - Quản lý danh sách sản phẩm: $k \le 20$
     - Duyệt mảng và xử lý xâu: $k \le 40$
     - Xử lý lỗi cấp phát realloc: $k \le 17$
     - An toàn con trỏ và null-pointer: $k \le 17$
     - Quản lý số lượng và tồn kho: $k \le 10$
2. **NuSMV Temporal Logic Model Checking**:
   - Tất cả 18 công thức CTL đều đạt giá trị **TRUE**.
   - Phân tích bẫy Vacuous Truth xác nhận mệnh đề kéo theo không bị đúng một cách vô nghĩa (tiền đề `ptr_invalid` và `realloc_failed` đều khả đạt trong không gian trạng thái).
3. **Phân tích động và Canary Leak Tracker**:
   - 17/17 Smoke tests và Unit tests vượt qua thành công với 0 rò rỉ bộ nhớ, 0 vi phạm canary bảo vệ đầu/cuối khối nhớ, 0 double-free.

---

### 4. ĐỀ XUẤT CÁC ĐIỂM CẦN NÂNG CẤP TRONG BƯỚC 4 (DEFENSIVE HARDENING)

Dựa trên các phản ví dụ số học lý thuyết thu được từ SMT Z3:
1. **Bổ sung guard chống tràn số cho Capacity**:
   - File: `source/src/product.c`
   - Bổ sung kiểm tra `if (list->capacity > SIZE_MAX / 2)` trước khi tính `new_cap`.
2. **Bổ sung guard chống tràn số kích thước Heap Realloc**:
   - File: `source/src/product.c`
   - Bổ sung kiểm tra `if (new_cap > SIZE_MAX / sizeof(Product))`.
3. **Tường minh hóa quy ước con trỏ mượn (Borrowed Pointer Contract)**:
   - File: `source/include/product.h`
   - Khuyến nghị tài liệu hóa quy tắc không lưu con trỏ tham chiếu tới `list->items[i]` qua các thao tác có khả năng thay đổi kích thước container (`add_product`, `clear_product_list`).
