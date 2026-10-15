# BÁO CÁO MÔI TRƯỜNG THỰC NGHIỆM VÀ CÔNG CỤ (TOOL ENVIRONMENT)

## 1. MÔI TRƯỜNG PHẦN CỨNG VÀ HỆ ĐIỀU HÀNH
- **Hệ điều hành:** Windows 11 64-bit (OS Data Model: LLP64).
- **Môi trường Unix Toolchain:** MSYS2 UCRT64.
- **Trình biên dịch C:** `GCC 16.1.0` (Rev5, Built by MSYS2 project).
- **Trình thông dịch Python:** Python 3.10.11 (với các thư viện `openpyxl 3.1.5`, `et-xmlfile 2.0.0`).
- **SMT Solver:** `Z3 Solver 5.1.0` (64-bit).
- **Static Analysis:** `Cppcheck 2.21.0`.
- **Coverage Tool:** `Gcov 16.1.0` / `Gcovr 8.6`.

## 2. BẢNG TRẠNG THÁI CÁC CÔNG CỤ THEO MỤC V QUY TRÌNH
| Công cụ | Phiên bản | Trạng thái | Giải pháp Thay thế Kỹ thuật Hợp lệ |
|---|---|---|---|
| **GCC Compiler** | 16.1.0 | **AVAILABLE** | Biên dịch với tập cờ cảnh báo tối đa `-Wall -Wextra -Wpedantic -Wconversion -Wshadow -Wformat=2` |
| **Cppcheck** | 2.21.0 | **AVAILABLE** | Phân tích tĩnh toàn diện với `--enable=all --inconclusive` |
| **Z3 SMT Solver** | 5.1.0 | **AVAILABLE** | Giải 10 bài toán SMT-LIB2 số học hữu hạn Bit-Vector (QF_BV) |
| **Gcov / Gcovr** | 16.1.0 / 8.6 | **AVAILABLE** | Đo độ phủ dòng và nhánh lệnh thực tế |
| **CBMC Native** | N/A | **TOOL_UNAVAILABLE** | Xây dựng 5 bộ kiểm tra **GCC Bounded Contract Testing** ($k=10..40$) |
| **NuSMV / nuXmv** | N/A | **TOOL_UNAVAILABLE** | Xây dựng **Custom CTL Model Checker** bằng Python nội bộ |
| **Clang / Scan-build**| N/A | **TOOL_UNAVAILABLE** | Sử dụng Cppcheck thay thế; ghi nhận trung thực |
| **ASan / LSan / Valgrind** | N/A | **TOOL_UNAVAILABLE** | Xây dựng **Canary Memory Tracker** hook malloc/realloc/free kiểm tra rò rỉ và tràn buffer |
| **AFL++ Native** | N/A | **TOOL_UNAVAILABLE** | Xây dựng **In-Process Mutation Fuzzing Harness** (1000 executions) |
