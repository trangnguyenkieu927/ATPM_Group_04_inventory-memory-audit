# TỔNG QUAN DỰ ÁN KIỂM TOÁN AN TOÀN BỘ NHỚ C

## 1. THÔNG TIN CHUNG
- **Đề tài:** Kiểm chứng an toàn bộ nhớ cho chương trình C mô phỏng nghiệp vụ quản lý kho.
- **Học phần:** An toàn phần mềm (Phenikaa University).
- **Nhóm thực hiện (ATPM_Group_04):**
  - **Nguyễn Kiều Trang:** Nhóm trưởng, tích hợp và nghiệm thu hệ thống.
  - **Hà Văn Đô:** Chuyên trách đặc tả an toàn, mô hình Kripke/CTL, SMT Solver và Bounded Model Checking.
  - **Nguyễn Quang Thọ:** Chuyên trách môi trường công cụ, mô phỏng rủi ro C, dynamic testing/fuzzing và đo độ phủ.
- **Repository:** `https://github.com/trangnguyenkieu927/ATPM_Group_04_inventory-memory-audit`
- **Nhánh làm việc tích hợp 10 công đoạn:** `audit/end-to-end-memory-audit`
- **Mã SHA Baseline v0 được bảo vệ:** `44202070c68002d9af068374478cdc83098dd4d8`

---

## 2. NGUYÊN TẮC KHOA HỌC VÀ CHUỖI TRUY VẾT BẮT BUỘC
Dự án bám sát định hướng của giảng viên hướng dẫn:
```text
SOURCE C -> COMPILER/STATIC ANALYSIS -> FINDINGS -> RISK SCENARIOS -> SAFETY SPECIFICATIONS -> FORMAL VERIFICATION -> C SIMULATION -> DYNAMIC TESTING/FUZZING -> CWE -> PATCH -> REGRESSION -> COVERAGE -> EVIDENCE
```

1. **Bảo tồn tính trung thực khoa học:**
   - Phân biệt minh bạch giữa kết quả toán học SMT (SAT) và tính khả đạt trên phần cứng thực tế (Reachability).
   - Phân biệt giữa GCC Harness Testing và CBMC symbolic execution native.
   - Phân biệt giữa Custom CTL Model Checker và NuSMV native.
   - Ghi nhận rõ Canary Memory Tracker là công cụ đo kiểm nội bộ, không tương đương AddressSanitizer/Valgrind.
   - Phân loại rõ ràng giữa Seeded Vulnerability (lỗi cài chủ đích đối chứng) và Confirmed Baseline Defect (lỗ hổng trên baseline).
2. **Cấu trúc lưu trữ thống nhất:**
   - Toàn bộ báo cáo Markdown `.md`, bảng tính Excel `.xlsx`, đặc tả `.json` và ma trận `.csv` nằm trọn vẹn trong `docs/`.
   - Raw logs kỹ thuật được bảo toàn trong `results/`.
   - Toàn bộ mã nguồn kiểm thử và kịch bản mô phỏng nằm trong `tests/` và `verification/`.

---

## 3. SƠ ĐỒ ĐIỀU HƯỚNG TÀI LIỆU (DOCUMENTATION SITEMAP)
| Thư mục | Nội dung trọng tâm |
| :--- | :--- |
| `docs/00_Project_Overview/` | Tổng quan dự án, nguyên tắc kiểm toán và sitemap |
| `docs/01_Source_Analysis/` | Luồng nghiệp vụ, bố cục bộ nhớ struct, hợp đồng input và kết quả compiler/static analysis |
| `docs/02_Risk_Scenarios/` | Ma trận 6 kịch bản rủi ro lõi (SC-01 đến SC-06) và phân lớp tương đương |
| `docs/03_Safety_Specification/` | Đặc tả hình thức 18 REQ chuẩn verbatim, tiền/hậu điều kiện, bất biến và rủi ro |
| `docs/04_Formal_Verification/` | Báo cáo kiểm chứng SMT Z3, GCC Contract Harnesses, CTL Model Checker và Kế hoạch độ phủ |
| `docs/05_C_Simulation/` | Bộ mã C Simulator thực nghiệm cho 6 kịch bản và thí nghiệm đối chứng Negative Control |
| `docs/06_Dynamic_Fuzzing/` | Phân tích động Canary, Stack Protector và Chiến dịch Fuzzing chuyên sâu cho Parser |
| `docs/07_CWE_Coverage/` | Phân loại CWE, Ma trận bằng chứng (Evidence Manifest với SHA256) và Báo cáo độ phủ Gcov |
| `docs/08_Remediation/` | Phân tích nguyên nhân gốc rễ (RCA), Bản vá v1 (Hardening) và Kiểm thử hồi quy |
| `docs/09_Final_Report/` | Báo cáo tổng kết nghiệm thu toàn diện và Master Workbook Excel 17 sheets |
| `docs/10_Demo/` | Kịch bản demo 8-10 phút trên VS Code, checklist và script demo tự động |
| `docs/verification/` | Hồ sơ kiểm chứng Bước 2 và Bước 3 kế thừa |
