# BÁO CÁO ĐÁNH GIÁ TÍNH PHỦ ĐẶC TẢ VÀ RÀ SOÁT MÃ NGUỒN (BƯỚC 2)

**Dự án:** Kiểm chứng an toàn bộ nhớ cho chương trình C mô phỏng quản lý kho
**Nhánh Git:** `audit/step2-risk-specification`
**Commit Baseline:** `44202070c68002d9af068374478cdc83098dd4d8`
**Kỹ sư phụ trách:** Nguyễn Kiều Trang (QA & Integration) & Hà Văn Đô (Specification)

---

## 1. NGUYÊN TẮC ĐÁNH GIÁ ĐỘ PHỦ

Theo chỉ đạo của Giảng viên hướng dẫn:
> *"Tổng hợp + đặc tả để hình thức hóa (tính phủ các trường hợp có thể xảy ra)..."*

Nhóm kiểm toán tuân thủ nguyên tắc **Trung thực Khoa học (Verification Integrity)**:
- **Phân biệt rõ ràng:** Độ phủ Đặc tả (Specification Coverage) $\neq$ Độ phủ Thực thi Dòng lệnh (Code Execution Coverage) $\neq$ Độ phủ Không gian Trạng thái Hình thức (Formal State Space Coverage).
- **Tuyệt đối không ghi 100% cho các chỉ số chưa đo:** Khi một công cụ đo runtime (`gcov/lcov`) hoặc model checker (`CBMC`, `NuSMV`) chưa được chạy khám phá toàn diện trong Bước 2, kết quả bắt buộc phải ghi là **`CHƯA ĐO (NOT_MEASURED)`**.

---

## 2. BẢNG TỔNG HỢP CÁC CHIỀU TÍNH PHỦ

| Chiều Tính Phủ | Phạm vi Mục tiêu | Tổng số | Đã Phủ | Tỷ lệ Đạt | Trạng thái | Cơ sở Đo lường / Bằng chứng |
|---|---|---|---|---|---|---|
| **1. Requirement Mapping Coverage** | 18 REQs từ Workbook chuẩn | 18 | 18 | **100.0%** | **COMPLETED** | 18/18 REQ được trích xuất nguyên văn, ánh xạ tới file, hàm, dòng mã tại [req_traceability.csv](req_traceability.csv). |
| **2. Source Review Coverage** | 7 modules C trong `source/src/` | 7 modules / 70 hàm | 7 / 70 | **100.0%** | **COMPLETED** | Rà soát 100% 70 hàm C, 2.229 Code LOC tại [source_inventory.md](source_inventory.md). |
| **3. Risk Specification Coverage** | Danh mục rủi ro an toàn bộ nhớ | 10 rủi ro | 10 | **100.0%** | **COMPLETED** | 10/10 rủi ro (`RISK-001` tới `RISK-010`) được đặc tả chi tiết tiền điều kiện, guard và CWE tại [risk_register.csv](risk_register.csv). |
| **4. Static Analysis Coverage** | 7 tệp nguồn C | 7 tệp | 7 | **100.0%** | **COMPLETED** | Cppcheck 2.21.0 thực thi thực tế, phân tích 67 cảnh báo tại [results/step2/cppcheck/findings.csv](../../../results/step2/cppcheck/findings.csv). |
| **5. Planned State Model Coverage** | Trạng thái của 5 phân hệ rời rạc | 21 trạng thái | 21 | **100.0% (Spec)** | **SPECIFIED** | 21 trạng thái vòng đời đã được lập mô hình tại [formalization_plan.md](formalization_plan.md). |
| **6. Dynamic Code/Branch Coverage** | Bao phủ dòng/nhánh khi chạy test | N/A | N/A | **CHƯA ĐO (NOT_MEASURED)** | **NOT_MEASURED** | Công cụ `gcov/lcov` chưa được tích hợp đo runtime trong Bước 2 (17 smoke tests đã PASS). |
| **7. Formal State Space Coverage** | Khám phá không gian trạng thái hình thức | N/A | N/A | **CHƯA ĐO (NOT_MEASURED)** | **NOT_MEASURED** | Model checker chưa chạy khám phá toàn diện không gian trạng thái trong Bước 2. |

---

## 3. CHI TIẾT ĐỘ PHỦ YÊU CẦU (REQUIREMENT MAPPING COVERAGE: 18/18 = 100.0%)

Tất cả 18 yêu cầu được phân nhóm và ánh xạ chính xác:
- **Nhóm Cấp phát và Quản lý Danh sách (REQ-1, REQ-2, REQ-3, REQ-12):** Ánh xạ tới `product_list_init`, `add_product` trong `product.c`.
- **Nhóm Chuỗi Ký tự và Vùng đệm (REQ-9, REQ-10, REQ-16):** Ánh xạ tới `safe_string_copy`, `load_products_from_file`, `parse_product_line`, `calculate_category_summaries`.
- **Nhóm Số học và Tính toán Kho (REQ-4, REQ-5, REQ-18):** Ánh xạ tới `import_stock`, `export_stock`, `check_addition_overflow`, `validate_export_quantity`.
- **Nhóm Con trỏ và Quyền Sở hữu (REQ-6, REQ-11, REQ-13, REQ-14, REQ-15):** Ánh xạ tới các API tra cứu, giải phóng và kiểm tra NULL pointer.
- **Nhóm Tính dừng và Phân tích Cú pháp (REQ-7, REQ-8, REQ-17):** Ánh xạ tới các vòng lặp xử lý giao dịch và parser 4/6 trường.
