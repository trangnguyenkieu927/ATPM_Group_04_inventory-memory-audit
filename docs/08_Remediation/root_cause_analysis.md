# PHÂN TÍCH NGUYÊN NHÂN GỐC VÀ THIẾT KẾ BẢN VÁ (ROOT CAUSE ANALYSIS)

## 1. MÔ TẢ VẤN ĐỀ (FIND-001 & FIND-002)
Trong phiên bản Baseline v0 (`source/src/product.c`), tại hàm `add_product`:
```c
    /* Baseline v0 */
    if (list->count >= list->capacity) {
        size_t new_cap = (list->capacity == 0) ? INITIAL_CAPACITY : (list->capacity * 2);
        Product *new_items = (Product *)realloc(list->items, new_cap * sizeof(Product));
        if (new_items == NULL) {
            return STATUS_ERR_MEMORY;
        }
        list->items = new_items;
        list->capacity = new_cap;
    }
```

### 1.1. Nguyên nhân gốc của FIND-001 (REQ-2):
- Biểu thức `list->capacity * 2` thực hiện phép nhân trên kiểu dữ liệu không dấu `size_t` (64-bit trên hệ thống hiện tại).
- Nếu `list->capacity > SIZE_MAX / 2` (nghiệm SMT-001A: $capacity = 2^{63}$), phép nhân xảy ra hiện tượng **Integer Wraparound** theo chuẩn C17. Giá trị `new_cap` bị cuốn vòng về `0`, dẫn đến việc yêu cầu cấp phát bộ nhớ kích thước 0 byte.

### 1.2. Nguyên nhân gốc của FIND-002 (REQ-3):
- Biểu thức `new_cap * sizeof(Product)` (với `sizeof(Product) = 208` byte) không kiểm tra giới hạn trần `SIZE_MAX`.
- Nếu `new_cap > SIZE_MAX / 208` (nghiệm SMT-002A: $new_cap = 88,686,269,585,142,076$), phép nhân số byte bị tràn số, làm đối số truyền vào hàm `realloc` nhỏ hơn nhiều so với nhu cầu thực tế. Điều này dẫn đến nguy cơ **Heap Out-of-bounds Write (CWE-787)** khi sao chép dữ liệu vào các phần tử tiếp theo.

## 2. GIẢI PHÁP THIẾT KẾ BẢN VÁ PHÒNG VỆ (DEFENSIVE HARDENING)
Để bảo đảm tính an toàn bộ nhớ tuyệt đối mà không làm thay đổi hành vi nghiệp vụ hay vi phạm hợp đồng API, nhóm áp dụng các nguyên tắc kỹ thuật sau:
1. **Kiểm tra trước khi tính toán (Pre-computation Bound Check):**
   - Kiểm tra `list->capacity > SIZE_MAX / 2` trước khi nhân đôi.
   - Kiểm tra `new_cap > SIZE_MAX / sizeof(Product)` trước khi nhân số byte.
2. **Thất bại an toàn (Fail-Safe Error Return):**
   - Nếu điều kiện biên bị vi phạm, hàm lập tức trả về `STATUS_ERR_MEMORY (-8)`.
   - Vùng nhớ cũ `list->items`, số lượng phần tử `list->count` và dung lượng `list->capacity` được giữ nguyên vẹn 100%, không bị rò rỉ hay mất dấu con trỏ.
