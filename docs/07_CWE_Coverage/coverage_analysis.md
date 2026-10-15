# BÁO CÁO PHÂN TÍCH ĐỘ PHỦ KIỂM CHỨNG (COVERAGE ANALYSIS REPORT)

## 1. PHƯƠNG PHÁP ĐO ĐỘ PHỦ THỰC TẾ
Độ phủ mã nguồn được đo lường tự động bằng công cụ chuẩn của GNU Compiler Collection:
- **Trình biên dịch:** `GCC 16.1.0` với các cờ đo độ phủ: `--coverage -O0 -g`.
- **Trình tổng hợp báo cáo:** `Gcovr 8.6`.
- **Tập test thực thi:** Bao gồm toàn bộ Unit Tests, Smoke Tests (17 ca), 6 bộ C Simulator Test Drivers (SC-01 đến SC-06), và Fuzzing harness.

## 2. KẾT QUẢ ĐỘ PHỦ SO SÁNH VỚI MỐC BƯỚC 3

| Chỉ số Đo lường | Kết quả Mốc Bước 3 | Kết quả Thực tế Hiện tại (Stage 8) | Mức độ Tăng trưởng |
|---|---|---|---|
| **Tổng số dòng lệnh thực thi (Lines)** | 280 / 770 dòng | **355 / 770 dòng** | **+75 dòng lệnh mới được kích hoạt** |
| **Tỷ lệ Phủ Dòng (Line Coverage)** | 36.4% | **46.1%** | **+9.7%** |
| **Tỷ lệ Phủ Nhánh (Branch Coverage)** | 32.2% | **41.5%** | **+9.3%** |
| **Độ phủ Luồng Trọng tâm (file_io.c)** | 87.0% | **87.0%** | Duy trì mức bao phủ rất cao |
| **Độ phủ Quản lý Kho (inventory.c)** | 76.7% | **88.4%** | **+11.7%** (nhờ SC-06 phủ toàn bộ biên tràn số) |
| **Độ phủ Mảng Động (product.c)** | 48.9% | **60.7%** | **+11.8%** (nhờ SC-03 & SC-04 phủ realloc) |

## 3. PHÂN TÍCH VÙNG ĐÃ PHỦ VÀ VÙNG CHƯA PHỦ
1. **Vùng đã phủ hoàn hảo (High Coverage):**
   - Module `file_io.c` đạt **87.0%** độ phủ dòng lệnh: Toàn bộ quá trình đọc tệp, phân tích dòng, cắt ký tự xuống dòng, chuyển đổi số nguyên `parse_int`, số thực `parse_price`, hỗ trợ 6 trường và 4 trường đều được kích hoạt triệt để.
   - Module `inventory.c` đạt **88.4%** độ phủ dòng lệnh: Toàn bộ các hàm khởi tạo, giải phóng, nhập kho, xuất kho, kiểm tra tràn số `INT_MAX`, và kiểm tra underflow đều được phủ 100%.
2. **Vùng chưa phủ và giải trình kỹ thuật:**
   - Module `report.c` đạt **13.7%**: Đây là module in ấn giao diện dòng lệnh (CLI report formatting), gồm các hàm định dạng bảng ASCII rộng, không nằm trong luồng nghiệp vụ kiểm chứng an toàn bộ nhớ trọng tâm. Theo định hướng của Giảng viên: *"Lựa chọn một luồng nghiệp vụ cụ thể trong chương trình... report.c và main.c vẫn nằm trong phạm vi rà soát hệ thống, nhưng không cần ép tất cả thí nghiệm phải chạy qua chúng"*.
   - Module `utils.c` đạt **28.5%**: Chứa các hàm tiện ích như `find_file_path`, `to_upper_case` dành cho các tính năng tìm kiếm mở rộng CLI không được gọi trong luồng nạp tệp kho.
