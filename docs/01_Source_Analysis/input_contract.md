# HỢP ĐỒNG ĐẦU VÀO VÀ RÀNG BUỘC DỮ LIỆU TỆP (INPUT CONTRACT)

## 1. HỢP ĐỒNG DÒNG DỮ LIỆU TỆP KHO (FILE LINE CONTRACT)
Chương trình đọc tệp văn bản UTF-8 dòng theo dòng thông qua hàm `load_products_from_file`:

| Thuộc tính hợp đồng | Quy định chuẩn | Xử lý khi vi phạm |
| :--- | :--- | :--- |
| **Kích thước dòng tối đa** | $\le 511$ ký tự (bao gồm `\n`), buffer 512 bytes | Dòng vượt quá 512B bị cắt đoạn; parser từ chối an toàn với mã `STATUS_ERR_FILE_IO (-7)` |
| **Dấu phân cách cho phép** | Dấu phẩy `,` hoặc dấu gạch đứng `\|` | Các ký tự khác không được phân tách trường, dẫn đến sai số lượng token |
| **Định dạng 6 trường chuẩn** | `ID,Name,Category,Unit,Quantity,Price` | Chấp nhận đầy đủ thông tin |
| **Định dạng 4 trường tương thích**| `ID,Name,Quantity,Price` | Tự động điền mặc định `Category="Chung"`, `Unit="Cai"` |
| **Số trường không hợp lệ** | $< 4$ trường hoặc $> 6$ trường | Từ chối dòng, ghi log lỗi, không thêm vào danh sách kho |
| **Dòng trống / Chỉ có khoảng trắng**| Bỏ qua | Parser bỏ qua, không gây lỗi hệ thống |
| **Ký tự kết thúc tệp (EOF)** | Dòng cuối không có `\n` | Vẫn chấp nhận bản ghi nếu cấu trúc trường hợp lệ |
| **Ký tự nhúng NUL (`\0`)** | Không được chứa NUL giữa dòng | `strchr`/`strtok` dừng sớm tại byte NUL đầu tiên, từ chối dòng |

---

## 2. HỢP ĐỒNG RÀNG BUỘC TỪNG TRƯỜNG DỮ LIỆU
| Trường | Giới hạn độ dài | Kiểu & Miền giá trị | Ràng buộc an toàn & Từ chối |
| :--- | :---: | :--- | :--- |
| `ID` | $1 \dots 19$ ký tự | Chuỗi ký tự `[A-Za-z0-9_-]` | Rỗng, dài $\ge 20$, chứa dấu cách hoặc ký tự đặc biệt bị từ chối (`-2`) |
| `Name` | $1 \dots 99$ ký tự | Chuỗi văn bản UTF-8 | Rỗng, dài $\ge 100$ bị từ chối (`-9`) |
| `Category` | $1 \dots 49$ ký tự | Chuỗi văn bản UTF-8 | Rỗng, dài $\ge 50$ bị từ chối (`-10`) |
| `Unit` | $1 \dots 19$ ký tự | Chuỗi văn bản UTF-8 | Rỗng, dài $\ge 20$ bị từ chối (`-11`) |
| `Quantity` | Chuỗi số nguyên | Số nguyên có dấu $1 \dots 2,147,483,647$ | Giá trị $\le 0$ (`-3`), tràn `INT_MAX` (`-6`), chuỗi chữ (`-3`) |
| `Price` | Chuỗi số thực | Số thực hữu hạn $\ge 0.0$ | Âm (`-13`), chuỗi chữ (`-13`), NaN hoặc Infinity (`-13`) |

---

## 3. BẢNG MÃ TRẠNG THÁI VÀ HÀM C TRẢ VỀ
| Tên định danh mã lỗi | Giá trị | Ý nghĩa kỹ thuật | Module phát sinh |
| :--- | :---: | :--- | :--- |
| `STATUS_SUCCESS` | `0` | Thao tác thành công trọn vẹn | Toàn bộ API |
| `STATUS_ERR_NULL_PTR` | `-1` | Đối số con trỏ truyền vào hàm là `NULL` | `product.c, inventory.c, file_io.c` |
| `STATUS_ERR_INVALID_ID` | `-2` | Mã sản phẩm sai định dạng hoặc quá dài | `validation.c` |
| `STATUS_ERR_INVALID_QTY` | `-3` | Số lượng không dương ($\le 0$) | `validation.c, inventory.c` |
| `STATUS_ERR_NOT_FOUND` | `-4` | Không tìm thấy mã sản phẩm trong kho | `inventory.c, product.c` |
| `STATUS_ERR_INSUFFICIENT_STOCK`| `-5` | Lượng xuất kho lớn hơn tồn kho hiện tại | `inventory.c` |
| `STATUS_ERR_OVERFLOW` | `-6` | Nguy cơ tràn số nguyên có dấu `INT_MAX` | `validation.c, inventory.c` |
| `STATUS_ERR_FILE_IO` | `-7` | Lỗi đọc/ghi file hoặc sai định dạng dòng | `file_io.c` |
| `STATUS_ERR_MEMORY` | `-8` | Lỗi cấp phát bộ nhớ động (`realloc/malloc` OOM) | `product.c` |
| `STATUS_ERR_INVALID_NAME` | `-9` | Tên sản phẩm rỗng hoặc quá 99 ký tự | `validation.c` |
| `STATUS_ERR_INVALID_CATEGORY` | `-10` | Danh mục rỗng hoặc quá 49 ký tự | `validation.c` |
| `STATUS_ERR_INVALID_UNIT` | `-11` | Đơn vị tính rỗng hoặc quá 19 ký tự | `validation.c` |
| `STATUS_ERR_DUPLICATE_ID` | `-12` | Mã sản phẩm đã tồn tại trong kho | `product.c` |
| `STATUS_ERR_INVALID_PRICE` | `-13` | Đơn giá âm, sai định dạng hoặc NaN/Inf | `validation.c` |
