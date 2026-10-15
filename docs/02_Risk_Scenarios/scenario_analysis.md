# BÁO CÁO PHÂN TÍCH VÀ ĐẶC TẢ CÁC KỊCH BẢN RỦI RO (SC-01 ĐẾN SC-06)

## 1. PHƯƠNG PHÁP XÂY DỰNG KỊCH BẢN
Dựa trên nhận xét chỉ đạo trực tiếp của Giảng viên:
> *"Lựa chọn một luồng nghiệp vụ cụ thể trong chương trình. Với mỗi chức năng trong luồng, xác định các trường hợp sử dụng vùng nhớ, buffer, cấp phát và giải phóng... Xác định các trường hợp giá trị biên và các tình huống có thể gây overflow, memory leak hoặc lỗi con trỏ. Xây dựng mã C mô phỏng những tình huống đã đặc tả."*

Nhóm ATPM_Group_04 đã xây dựng hệ thống **6 kịch bản rủi ro cốt lõi (SC-01 đến SC-06)** bao phủ toàn bộ vòng đời đối tượng bộ nhớ trong luồng nhập kho:

```
[SC-01: Parser Input]   --> [SC-02: Buffer Bounds]    --> [SC-03: Heap Allocation]
(Phân tích cú pháp tệp)      (Giới hạn đệm & Truncation)     (Mở rộng mảng động)
         |                           |                             |
         v                           v                             v
[SC-04: Allocation Failure] --> [SC-05: Pointer Lifetime] --> [SC-06: Stock Arithmetic]
(Xử lý hết RAM / Leak)       (Vô hiệu hóa con trỏ mượn)     (Tràn số nguyên tồn kho)
```

## 2. CHI TIẾT 6 KỊCH BẢN RỦI RO TRỌNG TÂM

### 2.1. SC-01: Parser Input Validation & Legacy 4-Field Compatibility
- **Hàm C:** `parse_product_line`, `validate_product_input`.
- **Rủi ro:** Khi file chứa dòng không đúng định dạng (dòng trống, thiếu trường, thừa token, dữ liệu số âm, trường rỗng).
- **Hành vi an toàn:** Tách chuỗi bằng `strtok_r`/`strsep`, kiểm tra số lượng token (`4 <= count <= 7`). Với tệp 4 trường cũ (`id|name|qty|price`), tự động điền giá trị mặc định `category="General"`, `unit="pcs"`.
- **Yêu cầu ánh xạ:** REQ-1, REQ-8, REQ-16, REQ-17, REQ-18.

### 2.2. SC-02: Buffer Bounds, Line Truncation & String Safety
- **Hàm C:** `load_products_from_file`, `case_insensitive_substring`.
- **Rủi ro:** Khi dòng trong tệp vượt quá kích thước buffer `line[512]`, `fgets` sẽ đọc không trọn vẹn và không chứa ký tự newline `\n`. Nếu đọc tiếp, phần đuôi bị coi là một dòng mới, gây vỡ logic phân tích. Đồng thời, sao chép chuỗi vào trường struct có thể gây tràn bộ đệm nếu không kiểm soát độ dài.
- **Hành vi an toàn:** Kiểm tra sự hiện diện của `\n`. Nếu không có và chưa gặp EOF, kích hoạt cơ chế drain buffer để bỏ qua dòng bị cắt cụt. Luôn đảm bảo null terminator cho mọi chuỗi trong struct.
- **Yêu cầu ánh xạ:** REQ-9, REQ-16, REQ-17.

### 2.3. SC-03: Heap Allocation, Dynamic Capacity Doubling & Arithmetic Safety
- **Hàm C:** `add_product_to_list`.
- **Rủi ro:** Khi danh sách đầy (`count == capacity`), thuật toán nhân đôi `new_cap = capacity * 2` có nguy cơ tràn số nguyên `size_t` (trên mô hình 64-bit, nếu capacity vượt `SIZE_MAX / 2`). Phép tính số byte `new_cap * sizeof(Product)` cũng có nguy cơ tràn nếu `new_cap > SIZE_MAX / 208`.
- **Hành vi an toàn:** Phải có guard tường minh trước khi thực hiện phép nhân.
- **Yêu cầu ánh xạ:** REQ-2, REQ-3, REQ-10.

### 2.4. SC-04: Allocation Failure Handling, Ownership Retention & Double Free Defense
- **Hàm C:** `add_product_to_list`, `free_product_list`.
- **Rủi ro:** Lỗi thường gặp trong C là gán trực tiếp `list->items = realloc(...)`. Nếu `realloc` thất bại (trả về NULL), địa chỉ vùng nhớ cũ bị mất vĩnh viễn dẫn đến **Memory Leak (CWE-401)**. Ngoài ra, việc gọi giải phóng hai lần gây **Double Free (CWE-415)**.
- **Hành vi an toàn:** Dùng con trỏ tạm `Product *temp = realloc(...)`. Nếu NULL, giữ nguyên `list->items` và trả về mã lỗi. Khi giải phóng, luôn đặt `list->items = NULL`, `count = 0`, `capacity = 0`.
- **Yêu cầu ánh xạ:** REQ-10, REQ-11, REQ-12, REQ-13.

### 2.5. SC-05: Pointer Lifetime, Borrowed References & Reallocation Invalidation
- **Hàm C:** `find_product_by_id`, `add_product_to_list`.
- **Rủi ro:** Hàm `find_product_by_id` trả về một con trỏ trỏ trực tiếp vào phần tử trong mảng động heap (`list->items`). Nếu caller lưu trữ con trỏ này và sau đó gọi `add_product_to_list` làm kích hoạt `realloc`, hệ điều hành có thể cấp phát một khối nhớ mới ở địa chỉ khác và giải phóng khối nhớ cũ. Con trỏ ban đầu biến thành **Dangling Pointer**, dẫn đến **Use-After-Free (CWE-416)**.
- **Hành vi an toàn:** Tài liệu hóa hợp đồng mượn con trỏ (Borrowed Reference Contract): con trỏ chỉ có giá trị tức thời, không được duy trì qua các thao tác biến đổi cấu trúc danh sách.
- **Yêu cầu ánh xạ:** REQ-14, REQ-15.

### 2.6. SC-06: Inventory Stock Arithmetic, Boundary Limits & Conservation Invariant
- **Hàm C:** `import_product_stock`, `export_product_stock`.
- **Rủi ro:** Khi nhập kho thêm số lượng lớn, phép cộng `product->quantity + delta` có thể vượt quá `INT_MAX (2147483647)`, gây tràn số nguyên có dấu (**Integer Overflow - CWE-190**), làm số lượng tồn kho thành số âm. Khi xuất kho, nếu không kiểm tra `delta > current_qty`, tồn kho sẽ bị âm (**Underflow - CWE-191**).
- **Hành vi an toàn:** Guard phòng vệ `INT_MAX - product->quantity < delta` trước khi cộng. Guard `product->quantity < delta` trước khi trừ. Giữ nguyên số lượng cũ nếu thao tác bị từ chối (Conservation Invariant).
- **Yêu cầu ánh xạ:** REQ-4, REQ-5, REQ-6, REQ-18.

## 3. DANH MỤC TÀI LIỆU VÀ BẢNG BIỂU ĐI KÈM
Tất cả các tài liệu được xuất trực tiếp trong thư mục `docs/02_Risk_Scenarios/`:
1. `scenario_matrix.xlsx`: Ma trận chi tiết 6 kịch bản với đầy đủ 12 trường thuộc tính.
2. `scenario_specifications.json`: Đặc tả dạng máy đọc JSON phục vụ sinh test tự động.
3. `scenario_traceability.csv`: Bảng ma trận truy vết từ Kịch bản đến Source C, REQ và CWE.
4. `input_equivalence_classes.xlsx`: Phân tích 17 lớp tương đương đầu vào.
5. `boundary_test_matrix.xlsx`: Phân tích giá trị biên N-1, N, N+1 cho từng kịch bản.
