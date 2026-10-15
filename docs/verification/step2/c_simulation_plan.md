# KẾ HOẠCH MÔ PHỎNG RỦI RO AN TOÀN BỘ NHỚ BẰNG C TEST HARNESS (STEP 2)

**Dự án:** Kiểm chứng an toàn bộ nhớ cho chương trình C mô phỏng quản lý kho  
**Nhánh thực hiện:** `audit/step2-risk-specification`  
**Commit Baseline:** `44202070c68002d9af068374478cdc83098dd4d8`  
**Kỹ sư phụ trách:** Nguyễn Quang Thọ (Toolchain & Testing) & Hà Văn Đô (Specification)  

---

## 1. MỤC TIÊU VÀ NGUYÊN TẮC THỰC HIỆN

Theo nhận xét định hướng của Giảng viên:
> *"Xây dựng code C mô phỏng lại rủi ro. Xây dựng đặc tả cho bài toán rủi ro an toàn."*

### Nguyên tắc Bắt buộc:
1. **Bảo vệ tuyệt đối Baseline v0:**  
   Không được sửa đổi mã nguồn trong `source/src/` để chèn lỗi mô phỏng, cũng không được merge nhánh có lỗi chủ đích `audit/v0-file-io-vulnerable` vào baseline hiện tại.
2. **Tách biệt Harness và Mã Sản xuất:**  
   Toàn bộ mã nguồn mô phỏng rủi ro sẽ được tổ chức trong thư mục kiểm thử riêng (`tests/simulation/` hoặc `tests/fuzzing/`) được thực thi trong Bước 3.
3. **Phân biệt Nghiệm Toán học và Lỗi Runtime:**  
   Việc một phương trình tràn số có nghiệm SAT trong SMT không đồng nghĩa với việc mã nguồn C sẽ crash hoặc gây lỗ hổng trong môi trường thực tế. Harness phải đo đạc chính xác hành vi thực tế (kết quả trả về, crash, tín hiệu SIGSEGV, hoặc từ chối an toàn).

---

## 2. BÀI TOÁN MÔ PHỎNG ƯU TIÊN 1 (SIM-001): HEAP ALLOCATION SIZE OVERFLOW VÀ CON TRỎ MƯỢN

### 2.1. Nguồn Gốc và Định danh
- **Mã Rủi ro:** `RISK-001`, `RISK-002`, `RISK-004`.
- **Ánh xạ Yêu cầu:** `REQ-2`, `REQ-3`, `REQ-14`.
- **Tập tin Nguồn:** `source/src/product.c` (`add_product`, `product_list_init`).
- **Biểu thức Trọng tâm:**
  - `new_cap = list->capacity * 2` (Dòng 42).
  - `new_items = realloc(list->items, new_cap * sizeof(Product))` (Dòng 44).

### 2.2. Trạng thái và Kịch bản Kích hoạt (Trigger Conditions)
- **Kịch bản A (Số học Tràn Bộ nhớ Cấp phát):**
  - Khởi tạo `ProductList` với `capacity = 0x4000000000000000` ($2^{62}$) hoặc gán cưỡng bức trong test harness.
  - Gọi `add_product()` để kích hoạt nhánh mở rộng dung lượng.
  - Tính toán: `new_cap = 2^{63}`.
  - Phép nhân `new_cap * sizeof(Product) = 2^{63} * 208` sẽ bị tràn bit trong số học 64-bit unsigned ($208 = 13 \times 16$).
  - Giá trị truyền vào `realloc` bị bọc vòng (*wrap-around*) thành một số byte rất nhỏ, dẫn tới việc cấp phát vùng nhớ quá nhỏ so với số phần tử dự kiến (*Heap Buffer Overflow* nếu tiếp tục ghi).
- **Kịch bản B (Con trỏ Mượn Bị Mất Hiệu lực - Borrowed Pointer Invalidation):**
  - Lấy con trỏ phần tử: `Product *p = find_product_by_id(list, "SP001")`.
  - Tiếp tục gọi `add_product()` nhiều lần để vượt ngưỡng `capacity`, buộc `realloc` phải di chuyển mảng `items` sang một khối nhớ heap mới.
  - Thực hiện đọc hoặc ghi qua `p->quantity`.

### 2.3. Khả năng Khả đạt trong Baseline v0 (Reachability Analysis)
- **Kịch bản A:** Trong đường thực thi chuẩn của baseline, `capacity` khởi tạo bằng 16 và chỉ tăng gấp đôi khi danh sách đầy thực tế. Để đạt tới $2^{62}$ phần tử, hệ thống cần lưu trữ hàng tỷ tỷ bản ghi, vượt xa dung lượng RAM vật lý ($> 10^{18}$ bytes). Vì vậy, rủi ro tràn số là **UNREACHABLE** trong luồng thông thường, nhưng là một thiếu sót phòng vệ (*Defensive Gap*) cần kiểm chứng bằng SMT và harness cô lập.
- **Kịch bản B:** Trong `main.c` và `inventory.c` của baseline `v0`, mã nguồn luôn xử lý trực tiếp hoặc tìm kiếm lại con trỏ, không lưu trữ con trỏ mượn qua các lời gọi `add_product()`. Rủi ro này là **REACHABILITY_UNPROVEN** trong v0, nhưng là một bài toán kiểm chứng hợp đồng kiến trúc then chốt.

### 2.4. Thiết kế Skeleton Test Harness cho Bước 3
```c
/* Skeleton: tests/simulation/test_sim001_heap_realloc.c */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "models.h"
#include "product.h"

void simulate_realloc_overflow_harness(void) {
    ProductList list;
    list.items = malloc(16 * sizeof(Product));
    list.count = 16;
    /* Cố tình thiết lập trạng thái cận biên tràn số */
    list.capacity = ((size_t)1 << 62); 
    
    Product p = {"SP999", "Simulated Overflow", "Test", "Cai", 10, 100.0};
    StatusCode code = add_product(&list, &p);
    
    /* Ghi nhận hành vi: Nếu trả về ERR_NO_MEMORY hay Crash */
    printf("[SIM-001] Result Code: %d\n", code);
    free(list.items);
}
```

### 2.5. Công cụ và Bằng chứng Cần thu thập ở Bước 3
- Biên dịch với GCC: `-fsanitize=address,undefined -g`.
- Chạy CBMC với unwinding giới hạn để tìm counterexample tràn số của `new_cap * sizeof(Product)`.
- Bằng chứng: Log ASan/UBSan, crash trace, hoặc mã lỗi trả về.

---

## 3. BÀI TOÁN MÔ PHỎNG ƯU TIÊN 2 (SIM-002): FILE PARSER BUFFER & TOKEN BOUNDS

### 3.1. Nguồn Gốc và Định danh
- **Mã Rủi ro:** `RISK-007`, `RISK-008`, `RISK-009`.
- **Ánh xạ Yêu cầu:** `REQ-5`, `REQ-6`, `REQ-7`, `REQ-8`, `REQ-9`, `REQ-16`, `REQ-17`, `REQ-18`.
- **Tập tin Nguồn:** `source/src/file_io.c` (`load_products_from_file`, `parse_product_line`).
- **Bộ đệm Mục tiêu:** `char line[512]`, mảng `tokens[8]`, các trường `char id[20]`, `name[100]`, `category[50]`, `unit[20]`.

### 3.2. Tập Dữ liệu Kiểm thử Mô phỏng (Corpus & Fuzzing Inputs)
Bộ test case dữ liệu được chuẩn bị sẵn cho Bước 3 bao gồm 6 nhóm tệp tin đầu vào:

| Tên Tệp Thử nghiệm | Đặc điểm Dữ liệu Đầu vào | Kỳ vọng Hành vi của Hệ thống | Phân loại Rủi ro |
|---|---|---|---|
| `corpus_valid_6col.txt` | Dòng hợp lệ chuẩn 6 cột phân tách bằng `,` | Tải thành công (`STATUS_OK`), nạp đủ sản phẩm | Chuẩn đối sánh |
| `corpus_legacy_4col.txt` | Dòng cũ 4 cột định dạng phiên bản trước | Bị từ chối an toàn hoặc nạp với giá trị mặc định | Tương thích ngược |
| `corpus_excess_tokens.txt` | Dòng chứa > 10 dấu phẩy (nhiều hơn kích thước mảng `tokens[8]`) | Guard `token_count < 8` kích hoạt; không ghi tràn mảng | Array bounds check |
| `corpus_insufficient_tokens.txt` | Dòng chỉ chứa 1 hoặc 2 trường bị cắt cụt | Bị từ chối an toàn (`STATUS_ERR_INVALID_DATA`), bỏ qua dòng lỗi | Missing fields handling |
| `corpus_oversized_line.txt` | Dòng dài 1.024 bytes không có ký tự newline `\n` | `fgets` đọc 511 bytes; guard phát hiện thiếu `\n` và từ chối dòng dài an toàn | Buffer truncation |
| `corpus_embedded_nul.txt` | Dòng chứa byte `\0` ở giữa trường tên sản phẩm | Parser xử lý chuỗi bị ngắt; không được đọc vượt biên ngoài vùng nhớ | NUL-byte injection |
| `corpus_numeric_malformed.txt` | Dòng chứa giá trị `quantity = 99999999999999999999` hoặc `NaN` | `safe_str_to_int` từ chối an toàn, không gây tràn số | Integer overflow |

### 3.3. So sánh với Nhánh Lỗi Cài đặt Chủ đích (`audit/v0-file-io-vulnerable`)
- Nhóm kiểm toán ghi nhận trong kho lưu trữ tồn tại nhánh thí nghiệm `audit/v0-file-io-vulnerable`.
- Nhánh này chứa một phiên bản `file_io.c` được cố ý xóa bỏ các guard kiểm tra độ dài chuỗi (`safe_string_copy`) và thay bằng hàm nguy hiểm `strcpy` / `strcat` để tạo lỗ hổng Stack Buffer Overflow (CWE-120).
- **Quy tắc Kiểm toán:** Nhánh này **KHÔNG ĐƯỢC PHÉP MERGE** vào baseline `v0`. Tuy nhiên, trong Bước 3, nhánh này sẽ được sử dụng làm **Mẫu Đối Chứng Lỗi (Vulnerable Benchmark Baseline)** để chứng minh rằng các công cụ phân tích tĩnh (Cppcheck/Clang Analyzer) và AddressSanitizer có khả năng phát hiện chính xác lỗ hổng khi các guard an toàn bị gỡ bỏ!

### 3.4. Thiết kế Skeleton Test Harness cho Bước 3
```c
/* Skeleton: tests/simulation/test_sim002_parser_fuzz.c */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "models.h"
#include "file_io.h"

void test_parser_with_file(const char *filepath) {
    ProductList list;
    product_list_init(&list);
    
    printf("[SIM-002] Testing parser on: %s\n", filepath);
    StatusCode code = load_products_from_file(&list, filepath);
    printf("[SIM-002] Return Code: %d | Loaded: %zu\n", code, list.count);
    
    product_list_free(&list);
}
```

### 3.5. Công cụ và Bằng chứng Cần thu thập ở Bước 3
- Chạy GCC với `-fsanitize=address,undefined`.
- Chạy công cụ Fuzzing nhẹ (AFL++ hoặc libFuzzer) cấp phát đột biến trên định dạng dòng CSV.
- Ghi nhận: Memory leak report từ LeakSanitizer (LSan) nếu có dòng bị lỗi cấp phát nhưng không giải phóng.

---

## 4. KẾT LUẬN VÀ LỊCH TRÌNH CHUYỂN GIAO BƯỚC 3

1. Hai bài toán mô phỏng `SIM-001` và `SIM-002` phản ánh đầy đủ hai rủi ro bộ nhớ trọng yếu nhất của dự án:
   - **Rủi ro Quản lý Bộ nhớ Động trên Heap** (Cấp phát, tràn kích thước, mất hiệu lực con trỏ).
   - **Rủi ro Xử lý Dữ liệu Đầu vào Không Tin cậy** (Tràn bộ đệm, tách token, định dạng tệp).
2. Toàn bộ thiết kế đã sẵn sàng để lập trình viên tạo mã kiểm thử độc lập trong Bước 3 mà không làm xáo trộn kiến trúc và tính toàn vẹn của baseline `v0`.
