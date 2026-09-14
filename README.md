# Memory Safety Audit for C-based Inventory Management Program

## 1. Giới thiệu

Đây là bài tập lớn môn **An toàn phần mềm**, thực hiện theo hướng:

> **Memory Safety Audit – Kiểm toán an toàn bộ nhớ C**

Đề tài:

> **Kiểm toán an toàn bộ nhớ cho chương trình C mô phỏng nghiệp vụ quản lý kho**

Chương trình mục tiêu được xây dựng bằng ngôn ngữ **C**, mô phỏng một số nghiệp vụ quản lý kho ở mức vừa phải, nhằm tạo đối tượng cho quá trình kiểm toán an toàn bộ nhớ.

## 2. Phạm vi chương trình

Chương trình tập trung vào các nghiệp vụ chính:

- Quản lý sản phẩm
- Nhập kho
- Xuất kho
- Cập nhật tồn kho
- Đọc/ghi dữ liệu từ file
- Kiểm tra tính hợp lệ của input
- Hiển thị và thống kê dữ liệu tồn kho

> **Lưu ý:** Đây không phải là một hệ thống quản lý kho hoàn chỉnh. Nghiệp vụ kho được sử dụng làm chương trình C mục tiêu phục vụ kiểm toán Memory Safety.

## 3. Mục tiêu kiểm toán

Quá trình kiểm toán tập trung vào việc:

- Xác định các vị trí có nguy cơ mất an toàn bộ nhớ.
- Phát hiện lỗi bằng phân tích mã nguồn và công cụ kiểm tra.
- Xây dựng input có khả năng kích hoạt lỗi.
- Xác nhận lỗi bằng kiểm thử thực tế.
- Phân tích nguyên nhân và tác động của lỗi.
- Sửa lỗi.
- Chạy lại kiểm thử và công cụ để so sánh trước/sau khi sửa.

## 4. Công cụ và kỹ thuật dự kiến

Các kỹ thuật/công cụ được sử dụng trong quá trình thực hiện:

- Pattern Scanner
- Cppcheck
- AddressSanitizer (ASan)
- UndefinedBehaviorSanitizer (UBSan)
- Fuzzing

> Các lỗi/CWE cụ thể chỉ được xác định sau khi có kết quả kiểm toán và bằng chứng thực tế.

## 5. Cấu trúc thư mục

```text
inventory-memory-audit/
│
├── source/
│   ├── include/          # Header files
│   ├── src/              # Source files
│   ├── data/             # Dữ liệu chương trình
│   └── Makefile          # Build chương trình
│
├── audit/
│   ├── v0/               # Phiên bản trước khi vá
│   └── v1/               # Phiên bản sau khi vá
│
├── tests/
│   ├── valid/            # Input hợp lệ
│   ├── boundary/         # Input tại biên
│   └── abnormal/         # Input bất thường
│
├── results/
│   ├── cppcheck/         # Kết quả Cppcheck
│   ├── pattern_scanner/  # Kết quả Pattern Scanner
│   ├── asan/             # Kết quả ASan
│   ├── ubsan/            # Kết quả UBSan
│   └── fuzzing/          # Kết quả Fuzzing
│
├── docs/
│   ├── architecture/    # Kiến trúc và luồng chương trình
│   ├── test_cases/      # Test case
│   └── findings/        # Phân tích các finding
│
└── README.md

## 6. Thành viên thực hiện

| STT | Họ và tên |
|-----|-----------|
| 1 | Hà Văn Đô |
| 2 | Nguyễn Kiều Trang |
| 3 | Nguyễn Quang Thọ |

## 7. Trạng thái dự án

Dự án hiện đang ở giai đoạn **khởi tạo**.

Các công việc đang được thực hiện:

- Xây dựng chương trình C mô phỏng nghiệp vụ quản lý kho.
- Hoàn thiện cấu trúc và các module của chương trình.
- Chuẩn bị môi trường kiểm toán và kiểm thử.
- Xây dựng các phiên bản chương trình phục vụ quá trình kiểm toán.
- Chuẩn bị các test case và dữ liệu kiểm thử.

Các kết quả kiểm toán, lỗi Memory Safety, bằng chứng thực nghiệm và kết quả sau khi sửa lỗi sẽ được cập nhật vào repository trong quá trình thực hiện.