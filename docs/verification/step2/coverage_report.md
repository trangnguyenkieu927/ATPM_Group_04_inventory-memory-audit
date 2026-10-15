# BÁO CÁO ĐÁNH GIÁ TÍNH PHỦ ĐẶC TẢ VÀ RÀ SOÁT MÃ NGUỒN (STEP 2)

**Dự án:** Kiểm chứng an toàn bộ nhớ cho chương trình C mô phỏng quản lý kho  
**Nhánh thực hiện:** `audit/step2-risk-specification`  
**Commit Baseline:** `44202070c68002d9af068374478cdc83098dd4d8`  
**Kỹ sư phụ trách:** Nguyễn Kiều Trang (Integration & QA) & Hà Văn Đô (Specification)  

---

## 1. NGUYÊN TẮC ĐÁNH GIÁ TÍNH PHỦ

Theo chỉ đạo của Giảng viên hướng dẫn:
> *"Tổng hợp + đặc tả để hình thức hóa (tính phủ các trường hợp có thể xảy ra)..."*

Nhóm kiểm toán tuân thủ nguyên tắc **Trung thực Khoa học (Verification Integrity)**:
- **Tuyệt đối không đánh đồng Độ phủ Đặc tả (Specification Coverage) với Độ phủ Thực thi (Code Execution Coverage)** hoặc **Độ phủ Không gian Trạng thái Hình thức (Formal State Space Coverage)**.
- Khi một công cụ đo runtime (như `gcov`) hoặc model checker (như `CBMC`, `NuSMV`) chưa chạy, trạng thái bắt buộc phải ghi rõ là **`NOT_MEASURED`**.
- Không tự suy diễn tỷ lệ 100% dựa trên việc vẽ đủ sơ đồ trạng thái hoặc chạy thành công bộ smoke test.

---

## 2. BẢNG TỔNG HỢP 4 CHIỀU TÍNH PHỦ (COVERAGE DIMENSIONS)

| Chiều Tính Phủ | Phạm vi Mục tiêu | Tổng số | Đã Phủ | Tỷ lệ Đạt | Trạng thái | Cơ sở Đo lường / Bằng chứng |
|---|---|---|---|---|---|---|
| **1. Requirement Mapping Coverage** | 18 REQs từ Workbook chuẩn | 18 | 18 | **100.0%** | **COMPLETED** | Ánh xạ toàn bộ 18 REQ tới mã nguồn C, hàm, đối tượng bộ nhớ, và phương pháp kiểm chứng tại [req_traceability.csv](file:///c:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/docs/verification/step2/req_traceability.csv). |
| **2. Source Review Coverage** | 7 modules C trong `source/src/` và `include/` | 7 modules / 70 hàm | 7 / 70 | **100.0%** | **COMPLETED** | Rà soát 100% 70 hàm C, 2.229 dòng Code LOC (2.867 tổng số dòng) tại [source_inventory.md](file:///c:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/docs/verification/step2/source_inventory.md). |
| **3. Risk Specification Coverage** | Danh mục rủi ro an toàn bộ nhớ phát hiện | 10 rủi ro | 10 | **100.0%** | **COMPLETED** | 100% 10 rủi ro (`RISK-001` tới `RISK-010`) được đặc tả chi tiết tiền điều kiện, unsafe state, guard và CWE tại [risk_register.md](file:///c:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/docs/verification/step2/risk_register.md). |
| **4. Planned State Model Coverage** | Trạng thái và chuyển tiếp của 5 phân hệ | 21 trạng thái | 21 | **100.0% (Spec)** | **SPECIFIED** | 100% trạng thái vòng đời bộ nhớ, con trỏ và tệp tin đã được lập mô hình tại [formalization_plan.md](file:///c:/Du_an/ATPM/ATPM_Group_04_inventory-memory-audit/docs/verification/step2/formalization_plan.md). |
| **5. Dynamic Code/Branch Coverage** | Bao phủ dòng/nhánh khi chạy test | N/A | N/A | **NOT_MEASURED** | **NOT_MEASURED** | Công cụ `gcov/lcov` chưa được gắn cờ biên dịch trong Bước 2 (17 smoke tests PASS). Sẽ triển khai ở Bước 3. |
| **6. Formal State Space Coverage** | Không gian trạng thái kiểm chứng hình thức | N/A | N/A | **NOT_MEASURED** | **NOT_MEASURED** | Công cụ `CBMC/NuSMV` chưa chạy khám phá trạng thái toàn diện trong Bước 2. |

---

## 3. CHI TIẾT ĐỘ PHỦ YÊU CẦU (REQUIREMENT MAPPING COVERAGE)

Công thức xác định:
$$\text{REQ\_mapping\_coverage} = \frac{\text{Số lượng REQ đã ánh xạ chi tiết}}{\text{Tổng số REQ có thể áp dụng}} = \frac{18}{18} = 100\%$$

- **Số REQ có thể áp dụng:** 18 / 18.
- **Số REQ không áp dụng (`NOT_APPLICABLE`):** 0.
- **Phân bố theo nhóm chức năng / phi chức năng:**
  - *Quản lý Cấp phát & Heap (REQ-1, REQ-2, REQ-3, REQ-4):* 4/4 (100%).
  - *Bộ đệm & Xử lý Chuỗi (REQ-5, REQ-6, REQ-7, REQ-8, REQ-9):* 5/5 (100%).
  - *Bảo đảm Con trỏ & Ownership (REQ-13, REQ-14, REQ-15):* 3/3 (100%).
  - *Số học & Thống kê Kho (REQ-10, REQ-11, REQ-12):* 3/3 (100%).
  - *Phân tích Cú pháp Tệp tin (REQ-16, REQ-17, REQ-18):* 3/3 (100%).

Mỗi REQ đều có đầy đủ:
- Tên hàm C và tập tin nguồn tương ứng.
- Đối tượng bộ nhớ chịu tác động.
- Biểu thức số học hoặc bất biến CTL cần kiểm chứng.
- Phương pháp kiểm chứng đề xuất cho Bước 3.

---

## 4. CHI TIẾT ĐỘ PHỦ RÀ SOÁT MÃ NGUỒN (SOURCE REVIEW COVERAGE)

Tất cả 7 module trong `source/src/` đã được thống kê và rà soát thủ công kết hợp phân tích đối tượng con trỏ:
1. `models.h` (1 struct cốt lõi `Product`, 1 struct quản lý `ProductList`, 2 struct báo cáo `CategorySummary`, `InventorySummary`).
2. `product.c` (7 hàm, 173 Code LOC): Cấp phát mảng động `realloc`, tìm kiếm, thêm, xóa, giải phóng.
3. `inventory.c` (7 hàm, 170 Code LOC): Thao tác nhập/xuất kho, tìm kiếm, kiểm tra ID.
4. `file_io.c` (6 hàm, 246 Code LOC): Đọc/ghi tệp `products.txt`, đọc tệp `transactions.txt`, phân tách chuỗi token bằng `strtok_r`.
5. `validation.c` (9 hàm, 172 Code LOC): Kiểm tra định dạng ID, tên, danh mục, đơn vị tính, số lượng, giá cả.
6. `utils.c` (7 hàm, 114 Code LOC): Các hàm an toàn chuỗi `safe_string_copy`, `safe_str_to_int`, `safe_str_to_double`, `trim_whitespace`.
7. `report.c` (7 hàm, 203 Code LOC): Tính toán tổng hợp danh mục, cảnh báo tồn kho thấp, thống kê tổng kho.
8. `main.c` (27 hàm nội bộ / menu, 731 Code LOC): Giao diện người dùng CLI, điều hướng menu, gọi các API tầng dưới.

**Kết luận:** 70/70 hàm trong phạm vi biên dịch đã được rà soát đầy đủ (100%).

---

## 5. ĐỘ PHỦ MÔ HÌNH HÓA TRẠNG THÁI (PLANNED STATE COVERAGE)

5 phân hệ trạng thái đã được mô hình hóa đầy đủ:
1. **Quản lý danh sách động:** 7 trạng thái (`list_valid`, `list_growth`, `list_growth_ok`, `list_growth_failed`, `list_cleanup`, `list_freed_state`, `list_cleanup_rejected`).
2. **Con trỏ mượn:** 2 trạng thái (`ptr_valid`, `ptr_invalidated`).
3. **Phân tích tệp:** 4 trạng thái (`file_idle`, `file_parsing`, `file_loaded`, `file_rejected`).
4. **Bộ đệm chuỗi:** 4 trạng thái (`buffer_idle`, `buffer_processing`, `buffer_safe`, `buffer_rejected`).
5. **Nghiệp vụ kho:** 4 trạng thái (`product_op_idle`, `product_op_processing`, `product_op_done`, `product_op_rejected`).

Tất cả các chuyển tiếp then chốt giữa các trạng thái an toàn và trạng thái từ chối lỗi đã được đặc tả hình thức bằng 18 công thức CTL.

---

## 6. KẾ HOẠCH ĐO LƯỜNG ĐỘ PHỦ CHO BƯỚC 3

Để chuyển từ **Độ phủ Đặc tả (Step 2)** sang **Độ phủ Thực nghiệm (Step 3)**, nhóm sẽ triển khai:
1. **Đo độ phủ nhánh và dòng mã (Gcov/Lcov):**
   - Biên dịch với cờ `-fprofile-arcs -ftest-coverage`.
   - Chạy toàn bộ 17 smoke tests + bộ test mới cho parser/simulation.
   - Xuất báo cáo HTML lcov để đo tỷ lệ `% line` và `% branch` thực tế.
2. **Khám phá không gian trạng thái (Bounded Model Checking):**
   - Sử dụng `cbmc --unwind N --coverage` để đo độ phủ các nhánh mã nguồn được kiểm chứng hình thức.
