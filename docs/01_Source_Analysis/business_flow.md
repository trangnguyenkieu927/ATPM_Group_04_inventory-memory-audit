# PHÂN TÍCH LUỒNG NGHIỆP VỤ VÀ ĐIỀU KHIỂN HỆ THỐNG

## 1. LUỒNG NGHIỆP VỤ TRỌNG TÂM
Luồng nghiên cứu xuyên suốt được lựa chọn là:
**Đọc tệp dữ liệu kho -> Phân tích cú pháp từng dòng -> Kiểm định tính hợp lệ -> Cập nhật danh sách ProductList/Inventory -> Mở rộng dung lượng mảng động khi đầy -> Giải phóng tài nguyên an toàn.**

### Sơ đồ luồng dữ liệu và điều khiển (Mermaid Flowchart)
```mermaid
flowchart TD
    A([Bắt đầu: load_products_from_file]) --> B[Mở file fopen r]
    B -- Thất bại (NULL) --> B1[Trả về STATUS_ERR_FILE_IO: -7]
    B -- Thành công --> C[Khởi tạo vòng lặp: fgets đọc tối đa 512 byte]
    
    C -- Đọc được dòng --> D[Hàm parse_product_line]
    C -- Hết file EOF --> K[Đóng file fclose và hoàn tất]
    
    D --> D1{Kiểm tra số token}
    D1 -- Không phải 6 hoặc 4 --> D2[Trả về STATUS_ERR_FILE_IO: -7, bỏ qua dòng]
    D1 -- Hợp lệ 6 hoặc 4 trường --> E[Bộ kiểm định Validation]
    
    E --> E1{Kiểm tra ID, Tên, Danh mục, Đơn vị}
    E1 -- Chuỗi sai quy cách --> E2[Trả về mã lỗi STATUS_ERR_INVALID_... Bỏ qua]
    E1 -- Chuỗi hợp lệ --> E3{Kiểm tra Số lượng & Đơn giá}
    
    E3 -- Số lượng <= 0 hoặc > INT_MAX --> E4[Trả về STATUS_ERR_INVALID_QTY: -3 / OVERFLOW: -6]
    E3 -- Đơn giá < 0 hoặc NaN/Inf --> E5[Trả về STATUS_ERR_INVALID_PRICE: -13]
    E3 -- Hợp lệ --> F{Tìm sản phẩm trong kho: find_product_by_id}
    
    F -- Đã tồn tại --> G[import_stock: Cộng dồn tồn kho có guard overflow]
    G -- Tràn INT_MAX --> G1[Bị chặn, trả về STATUS_ERR_OVERFLOW: -6]
    G -- Thành công --> C
    
    F -- Chưa tồn tại --> H{Kiểm tra sức chứa: count >= capacity}
    H -- Đầy --> I[add_product: Mở rộng realloc capacity * 2]
    H -- Còn chỗ --> J[Ghi struct vào items[count], count++]
    
    I -- realloc thành công --> J
    I -- realloc thất bại (OOM) --> I1[Bảo toàn con trỏ cũ items, trả về STATUS_ERR_MEMORY: -8]
    
    J --> C
    K --> L[Vòng đời kết thúc: product_list_free / inventory_free]
    L --> M([Kết thúc an toàn: Active Allocations = 0])
```

---

## 2. PHÂN TÍCH CHI TIẾT CÁC HÀM TRONG LUỒNG
1. `load_products_from_file(const char *filename, Inventory *inv)` (`file_io.c`):
   - Mở file an toàn với chế độ đọc `"r"`.
   - Cung cấp bộ đệm cục bộ `char line[512]` trên stack frame.
   - Sử dụng `fgets` đọc từng dòng có giới hạn kích thước buffer.
2. `parse_product_line(char *line, Product *p)` (`file_io.c`):
   - Bẻ nhỏ dòng thành các token bằng dấu phân cách `,` hoặc `|`.
   - Giới hạn cứng mảng `tokens[8]` với guard `token_count < 8` để ngăn tràn ngăn xếp.
   - Hỗ trợ định dạng chuẩn 6 trường (`id,name,category,unit,quantity,price`) và 4 trường legacy.
3. `validate_product(const Product *p)` (`validation.c`):
   - Kiểm tra ranh giới độ dài chuỗi và tập ký tự cho phép.
   - Kiểm tra `quantity > 0` và không vượt quá `INT_MAX`.
   - Kiểm tra `price >= 0.0` và `isfinite(price)`.
4. `add_product(ProductList *list, const Product *p)` (`product.c`):
   - Kiểm tra con trỏ `NULL` đầu vào (`STATUS_ERR_NULL_PTR = -1`).
   - Kiểm tra ID trùng lặp (`STATUS_ERR_DUPLICATE_ID = -12`).
   - Mở rộng mảng tự động khi đầy (`count >= capacity`) với `INITIAL_CAPACITY = 16`.
   - Sử dụng con trỏ tạm `Product *new_items = realloc(...)` để chống mất quyền sở hữu khi OOM.
5. `import_stock(Inventory *inv, const char *id, int add_qty)` (`inventory.c`):
   - Kiểm tra tràn số bằng hàm `check_addition_overflow(current_stock, add_qty)`.
6. `product_list_free(ProductList *list)` (`product.c`):
   - Giải phóng con trỏ `items`, gán về `NULL`, đặt `count = 0`, `capacity = 0` (Idempotent cleanup).
