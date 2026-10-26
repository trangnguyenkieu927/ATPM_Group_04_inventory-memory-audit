# HƯỚNG DẪN TRÌNH DIỄN DEMO VỚI THẦY (DEMO GUIDE)

## 1. THÔNG TIN PHIÊN TRÌNH DIỄN
- **Thời lượng mục tiêu:** 8 - 10 phút.
- **Hình thức:** 1 người thao tác trực tiếp trên VS Code và trình bày theo chuỗi truy vết logic, không sử dụng slide.
- **Tiêu chí:** Minh chứng chuỗi khép kín:
  `Mã nguồn C -> Phát hiện rủi ro -> Đặc tả REQ -> Kiểm chứng Hình thức SMT -> Mô phỏng C -> Bằng chứng CWE -> Bản vá v1 -> Hồi quy`.

## 2. KỊCH BẢN TRÌNH DIỄN 8 BƯỚC (CHUẨN 8 PHÚT)
1. **Phút 01:** Mở `source/src/product.c`, chỉ ra đoạn mã thiếu guard tại dòng 118 (`capacity * 2`) và 119 (`new_cap * 208`).
2. **Phút 02:** Mở `docs/01_Source_Analysis/compiler_audit_report.md` và chạy thử lệnh build GCC strict. Giải thích: *Compiler 0 warning KHÔNG đồng nghĩa là an toàn tuyệt đối.*
3. **Phút 03:** Mở `docs/02_Risk_Scenarios/scenario_matrix.xlsx` và `docs/03_Safety_Specification/safety_requirements.xlsx`, chỉ ra kịch bản SC-03 và hai yêu cầu REQ-2, REQ-3.
4. **Phút 04:** Chạy Z3 SMT Solver: `z3 verification/step3/smt/smt_001a_capacity_growth.smt2` cho thấy kết quả `sat` (nghiệm tràn số $2^{63}$). Sau đó mở `smt_001b` cho thấy `unsat` khi bị chặn trần.
5. **Phút 05:** Chạy C Simulator: `python tools/demo/run_demo.py --simulation` chứng minh cả 6 kịch bản mô phỏng chạy thực tế trên API C.
6. **Phút 06:** Chạy thí nghiệm đối chứng lỗi cài chủ đích: `tests/seeded/seeded_stack_overflow.c`, quan sát stack smashing detected và phân loại CWE-120.
7. **Phút 07:** Mở bản vá `docs/08_Remediation/v1_defensive_guard.patch`, chỉ ra 2 guard phòng vệ mới được bổ sung trong `source/src/product.c`.
8. **Phút 08:** Chạy kiểm thử hồi quy: `python tools/demo/run_demo.py --regression` cho thấy 17/17 smoke tests PASS và mở bảng tổng kết master `docs/09_Final_Report/ATPM_Group04_Final_Verification.xlsx`.
