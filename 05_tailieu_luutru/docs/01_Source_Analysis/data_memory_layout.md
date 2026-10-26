# ĐẶC TẢ BỐ CỤC BỘ NHỚ VÀ CẤU TRÚC DỮ LIỆU C

## 1. THÔNG SỐ KIẾN TRÚC VÀ HỆ THỐNG BIÊN DỊCH
- **Môi trường thực thi:** Windows 11 Enterprise x86_64
- **Trình biên dịch:** GCC 16.1.0 (Built by MSYS2 project)
- **Mô hình dữ liệu ABI:** **LLP64**
  - `sizeof(char) = 1` byte
  - `sizeof(short) = 2` bytes
  - `sizeof(int) = 4` bytes
  - `sizeof(long) = 4` bytes
  - `sizeof(long long) = 8` bytes
  - `sizeof(size_t) = 8` bytes (unsigned 64-bit)
  - `sizeof(void*) = 8` bytes (64-bit pointer)
  - `sizeof(double) = 8` bytes (IEEE-754 double precision)

---

## 2. BỐ CỤC BỘ NHỚ CHI TIẾT CỦA STRUCT `Product`
Được đo kiểm trực tiếp thông qua macro chuẩn C `offsetof()` và `sizeof()` trên GCC 16.1.0:

| Trường (Field) | Kiểu dữ liệu | Kích thước khai báo | Offset bắt đầu | Offset kết thúc | Padding sau trường | Ý nghĩa nghiệp vụ |
| :--- | :--- | :---: | :---: | :---: | :---: | :--- |
| `id` | `char[20]` | 20 bytes | Byte 0 | Byte 19 | 0 bytes | Mã định danh duy nhất của sản phẩm |
| `name` | `char[100]` | 100 bytes | Byte 20 | Byte 119 | 0 bytes | Tên gọi sản phẩm |
| `category` | `char[50]` | 50 bytes | Byte 120 | Byte 169 | 0 bytes | Danh mục phân loại mặt hàng |
| `unit` | `char[20]` | 20 bytes | Byte 170 | Byte 189 | **2 bytes (190..191)** | Đơn vị tính (Cái, Hộp, Kg...) |
| `quantity` | `int` | 4 bytes | Byte 192 | Byte 195 | **4 bytes (196..199)** | Số lượng tồn kho (căn lề 4-byte) |
| `price` | `double` | 8 bytes | Byte 200 | Byte 207 | 0 bytes | Đơn giá xuất/nhập (căn lề 8-byte) |

### Phân tích căn lề bộ nhớ (Memory Alignment Analysis):
- Tổng kích thước các mảng ký tự: $20 + 100 + 50 + 20 = 190$ bytes.
- Do trường `quantity` là kiểu `int` (cần căn lề tại địa chỉ chia hết cho 4), trình biên dịch chèn **2 bytes padding** tại offset $190 \dots 191$.
- Sau `quantity` (kết thúc tại byte 195), trường `price` là kiểu `double` (yêu cầu căn lề tại địa chỉ chia hết cho 8). Trình biên dịch chèn tiếp **4 bytes padding** tại offset $196 \dots 199$.
- **Tổng kích thước `sizeof(Product)`: 208 bytes.**
- Kích thước này là bội số của 8 (độ căn lề lớn nhất của struct là `double`), do đó mảng liên tiếp các `Product` không phát sinh padding xen giữa các phần tử.

---

## 3. BỐ CỤC BỘ NHỚ CHI TIẾT CỦA STRUCT `ProductList` (CONTAINER)
| Trường | Kiểu dữ liệu | Kích thước | Offset | Padding | Ý nghĩa nghiệp vụ |
| :--- | :--- | :---: | :---: | :---: | :--- |
| `items` | `Product*` | 8 bytes | Byte 0 | 0 bytes | Con trỏ sở hữu trỏ tới mảng heap chứa các `Product` |
| `count` | `size_t` | 8 bytes | Byte 8 | 0 bytes | Số lượng bản ghi sản phẩm hiện đang lưu |
| `capacity` | `size_t` | 8 bytes | Byte 16 | 0 bytes | Sức chứa tối đa trước khi phải cấp phát lại |

- **Tổng kích thước `sizeof(ProductList)`: 24 bytes.**
- Bất biến bắt buộc: $0 \le count \le capacity$.
- Khi `count > 0`, con trỏ `items` bắt buộc phải khác `NULL` và trỏ vào vùng nhớ heap hợp lệ có kích thước tối thiểu $capacity 	imes 208$ bytes.
