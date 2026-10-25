# HỒ SƠ THIẾT LẬP MÔI TRƯỜNG KIỂM CHỨNG BƯỚC 3

## 1. Thông tin chung về hệ thống thực nghiệm
- **Hệ điều hành:** Microsoft Windows 11 Pro 64-bit (`Windows-10-10.0.26200-SP0`).
- **Kiến trúc ABI mục tiêu:** LLP64 (Data model chuẩn của Windows x64: `sizeof(int) = 4`, `sizeof(long) = 4`, `sizeof(long long) = 8`, `sizeof(size_t) = 8`, con trỏ 64-bit = 8 byte).
- **Môi trường shell / toolchain:** MSYS2 UCRT64 kết hợp PowerShell 7 / Windows Terminal.
- **Python Runtime:** Python 3.10.11 64-bit (`C:\Users\ADMIN\AppData\Local\Programs\Python\Python310\python.exe`).
- **Baseline Git SHA:** `44202070c68002d9af068374478cdc83098dd4d8` (commit chốt v0).
- **Step 2 Git SHA:** `bcb07c50d505f3d568e7e9a13ad0531707e0483b` (commit chốt Bước 2).
- **Nhánh thực hiện Bước 3:** `audit/step3-formal-verification`.

---

## 2. Bảng tổng hợp công cụ và trạng thái khả dụng thực tế

Tuân thủ nguyên tắc trung thực khoa học, nhóm thực hiện phân loại minh bạch trạng thái của từng công cụ tại thời điểm nghiệm thu Bước 3:

| Nhóm công cụ | Tên công cụ | Phiên bản | Đường dẫn thực thi | Trạng thái | Ghi chú kỹ thuật |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **Trình biên dịch** | GCC (UCRT64) | 16.1.0 (Rev5) | `C:\msys64\ucrt64\bin\gcc.exe` | **AVAILABLE** | Biên dịch chuẩn C99/C11, tối ưu `-O0`, `-O1`, hỗ trợ gcov |
| **Công cụ build** | GNU Make | 4.4.1 | `C:\msys64\ucrt64\bin\make.exe` | **AVAILABLE** | Điều phối build hệ thống qua Makefile |
| **Phân tích tĩnh** | Cppcheck | 2.21.0 | `C:\msys64\ucrt64\bin\cppcheck.exe` | **AVAILABLE** | Đã hoàn thành quét tĩnh tại Bước 2 (67 thông báo) |
| **SMT Solver** | Z3 Theorem Prover | 5.1.0 - 64 bit | `C:\Du_an\ATPM\lab5_sat_smt_verification\.deps\bin\z3.exe` | **AVAILABLE** | Giải toán số học bit-vector, QF_BV, mô hình hóa tràn số nguyên |
| **Độ phủ dòng lệnh** | Gcov | 16.1.0 (JSON v2) | `C:\msys64\ucrt64\bin\gcov.exe` | **AVAILABLE** | Đo Line Coverage và Branch Coverage qua instrumentation `--coverage` |
| **Báo cáo độ phủ** | Gcovr | 8.6 | `C:\Users\ADMIN\AppData\Local\Programs\Python\Python310\Scripts\gcovr.exe` | **AVAILABLE** | Xuất báo cáo tổng hợp tỷ lệ phần trăm dòng/nhánh chi tiết |
| **Kiểm chứng CTL** | Kripke/CTL Model Checker | 1.0 (Phenikaa SoftSec Toolkit) | `softsec.ch1_models.kripke` + Python 3.10 | **AVAILABLE** | Kiểm chứng 18 công thức CTL trên mô hình 21 trạng thái, kiểm tra tính khả đạt và bẫy Vacuous Truth |
| **Mô hình NuSMV** | NuSMV / nuXmv | 2.6.0 (Syntax Model) | `verification/step3/ctl/inventory_model.smv` | **SYNTAX_VERIFIED** | Xuất mã nguồn `.smv` hoàn chỉnh chứa 18 công thức CTL; nhị phân CLI độc lập chưa cài trên PATH Windows |
| **Bounded Model Checking** | CBMC | CPROVER Harness Model | `verification/step3/cbmc/` | **HARNESS_READY** | Xây dựng đầy đủ 5 bộ harness CPROVER contracts; kiểm chứng tính khả thỏa và unrolling qua Z3 SMT engine |
| **Dynamic Sanitizers** | GCC AddressSanitizer | N/A (UCRT64 GCC) | `gcc -fsanitize=address` | **TOOL_UNAVAILABLE** | Toolchain MSYS2 UCRT64 GCC thiếu thư viện liên kết `libasan.a` (`cannot find -lasan`) |
| **Dynamic Sanitizers** | GCC UBSan | N/A (UCRT64 GCC) | `gcc -fsanitize=undefined` | **TOOL_UNAVAILABLE** | Toolchain MSYS2 UCRT64 GCC thiếu thư viện liên kết `libubsan.a` (`cannot find -lubsan`) |
| **Phát hiện rò rỉ** | Memory Tracking Harness | 1.0 | `verification/step3/sanitizers/` | **AVAILABLE** | Bộ wrapper tracking heap allocation/free nội bộ độc lập, đo số byte cấp phát và phát hiện rò rỉ ranh giới |

---

## 3. Nhật ký thăm dò công cụ thực tế (Toolchain Probe Logs)

Các log thô xác nhận việc kiểm tra môi trường được lưu trữ tại `results/step3/environment/`:
- `gcc_version.txt`: Xác nhận GCC 16.1.0 UCRT64.
- `make_version.txt`: Xác nhận GNU Make 4.4.1.
- `gcov_version.txt`: Xác nhận Gcov 16.1.0 hỗ trợ JSON format v2.
- `gcovr_version.txt`: Xác nhận Gcovr 8.6.
- `cppcheck_version.txt`: Xác nhận Cppcheck 2.21.0.
- `z3_version.txt`: Xác nhận Z3 5.1.0 64-bit.
- `sanitizer_probe.txt`: Xác nhận nguyên nhân kỹ thuật `-fsanitize=address,undefined` bị thiếu `libasan` và `libubsan` trên UCRT64.
- `environment_summary.txt`: Tóm tắt cấu hình môi trường kiểm chứng.

---

## 4. Giải pháp kỹ thuật thay thế và bổ sung cho các công cụ bị giới hạn

1. **Đối với Bounded Model Checking (CBMC):**
   - Viết toàn bộ 5 bộ harness chuẩn CPROVER (`__CPROVER_assert`, `__CPROVER_assume`, nondeterministic inputs).
   - Sử dụng động cơ Bounded Model Checking mở cuộn vòng lặp (Loop Unwinding BMC) tích hợp Z3 Solver (theo chuẩn Bài thực hành Lab 6 - An toàn phần mềm) để chứng minh tính an toàn trong giới hạn $k$ bước (SAFE up to bound $k$).

2. **Đối với Kiểm chứng Thời gian CTL (NuSMV):**
   - Xây dựng mô hình hình thức Kripke Structure chuẩn bằng Python (`softsec.ch1_models.kripke`) để duyệt toàn bộ không gian trạng thái, tính tập tiền ảnh ngược (pre-image), kiểm tra tính khả đạt của 21 trạng thái, phát hiện vòng lặp vô hạn và đánh giá bẫy chân lý rỗng (Vacuous Truth).
   - Xuất file đặc tả NuSMV `.smv` chuẩn để có thể tái lập tức thì trên bất kỳ hệ thống Linux/WSL2 có cài đặt NuSMV.

3. **Đối với Sanitizers (ASan/UBSan):**
   - Không giả mạo log ASan trên Windows UCRT64.
   - Xây dựng harness C có bộ đếm canary và wrapper phân bổ động (`malloc`/`calloc`/`realloc`/`free`) để phát hiện rò rỉ bộ nhớ, tính toán số byte cấp phát, kiểm tra con trỏ giải phóng hai lần (double-free) và truy cập con trỏ mượn bị vô hiệu hóa.
   - Thực hiện thí nghiệm đối chứng Negative Control (SIM-003) giữa Baseline v0 và nhánh lỗi cài có chủ đích `audit/v0-file-io-vulnerable`.
