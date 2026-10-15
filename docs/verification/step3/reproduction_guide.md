# HƯỚNG DẪN TÁI LẬP TOÀN DIỆN KẾT QUẢ KIỂM CHỨNG BƯỚC 3
## Dự án: Kiểm chứng an toàn bộ nhớ cho chương trình C mô phỏng nghiệp vụ quản lý kho
## Giai đoạn: Bước 3 – Kiểm chứng hình thức, Mô phỏng rủi ro và Đánh giá độ phủ

---

### 1. TỔNG QUAN VÀ NGUYÊN TẮC TÁI LẬP

Tài liệu này cung cấp quy trình từng bước để tái lập (reproduce) toàn bộ các kết quả kiểm chứng hình thức, mô hình trạng thái, kiểm tra bộ nhớ động, mô phỏng rủi ro C và đo độ phủ kiểm chứng từ một bản sao chép mới (clean checkout) trên môi trường Windows hoặc Linux/WSL2.

**Nguyên tắc cốt lõi**:
1. **Bảo toàn mã nguồn Baseline v0**: Tuyệt đối không cần chỉnh sửa bất kỳ dòng mã nào trong `source/src/` và `source/include/`.
2. **Trung thực khoa học**: Báo cáo đầy đủ trạng thái công cụ (`TOOL_UNAVAILABLE` nếu môi trường thiếu thư viện như ASan trên UCRT64).
3. **An toàn tài nguyên**: Toàn bộ script thực thi đều có giới hạn thời gian (timeout) và kiểm soát bộ nhớ, ngăn chặn tình trạng tràn RAM vật lý hoặc treo máy do phân tích không giới hạn.

---

### 2. YÊU CẦU MÔI TRƯỜNG VÀ CÔNG CỤ

#### 2.1. Môi trường Windows 11 (MSYS2 UCRT64)
- **Trình biên dịch C**: GCC 16.1.0 hoặc MinGW-w64 GCC tương thích C99.
- **Hệ điều hành**: Windows 11 x86_64 (LLP64 ABI).
- **SMT Solver**: Z3 Solver phiên bản $\ge 4.8$ (được đặt trong PATH hoặc tại `lab5_sat_smt_verification/.deps/bin/z3.exe`).
- **Python Runtime**: Python 3.10+ (có thư viện `gcovr` để tổng hợp coverage).
- **Công cụ đo coverage**: `gcov` và `gcovr`.

#### 2.2. Môi trường Linux / WSL2 (Ubuntu 22.04 LTS / Debian 12)
Nếu thực hiện trên Linux/WSL2 để kiểm tra thêm AddressSanitizer/LeakSanitizer native:
```bash
sudo apt-get update
sudo apt-get install -y build-essential gcc gcovr z3 cbmc python3 python3-pip
```

---

### 3. QUY TRÌNH THỰC THI NHANH BẰNG MASTER PIPELINE

Dự án đã tích hợp công cụ thực thi toàn diện: `tools/run_step3_verification.py`.

#### 3.1. Chạy toàn bộ các nhóm kiểm chứng
Tại thư mục gốc của repository, thực thi:
```bash
python tools/run_step3_verification.py --all
```
Script sẽ tuần tự kích hoạt 6 giai đoạn:
1. **SMT Arithmetic**: Giải 10 bài toán SMT với Z3 (SAT / UNSAT).
2. **CBMC Bounded Verification**: Biên dịch và chạy 5 harness kiểm chứng bất biến với GCC contract assertions ($k=10 \dots 40$).
3. **Dynamic Memory Tracker**: Kiểm tra rò rỉ bộ nhớ, heap canary corruption, double-free.
4. **C Risk Simulations**: Thực thi SIM-001 (Heap lifecycle/OOM), SIM-002 (Parser buffer limits với 10 corpus), SIM-003 (Negative control đối chứng với nhánh lỗi cài chủ đích).
5. **CTL Model Checking**: Kiểm chứng 21 trạng thái khả đạt và 18 công thức CTL bằng NuSMV logic checker.
6. **Code Coverage**: Đo lường và báo cáo độ phủ dòng lệnh và độ phủ nhánh.

#### 3.2. Chạy riêng lẻ từng nhóm kiểm chứng
| Mục tiêu kiểm chứng | Lệnh thực thi | Mô tả kết quả kỳ vọng |
| :--- | :--- | :--- |
| **Kiểm chứng số học SMT** | `python tools/run_step3_verification.py --smt` | 10 bài toán được Z3 giải: SAT (có phản ví dụ số học) hoặc UNSAT (được bảo vệ) |
| **Bounded Model Checking** | `python tools/run_step3_verification.py --cbmc` | 5 harness chạy thành công, không vi phạm bất biến trong bound $k$ |
| **Kiểm tra rò rỉ động** | `python tools/run_step3_verification.py --sanitizers` | 0 leaks, 0 double-free, 0 canary corrupted |
| **Mô phỏng rủi ro C** | `python tools/run_step3_verification.py --simulation` | SIM-001 PASS, SIM-002 PASS (10/10), SIM-003 phát hiện stack smashing trên seeded code |
| **Kiểm chứng mô hình CTL** | `python tools/run_step3_verification.py --ctl` | 21/21 trạng thái khả đạt, 18/18 công thức CTL đạt TRUE |
| **Đo độ phủ Coverage** | `python tools/run_step3_verification.py --coverage` | Xuất bảng tỷ lệ phần trăm dòng lệnh và nhánh đã được thực thi |

---

### 4. HƯỚNG DẪN THỰC THI THỦ CÔNG TỪNG CÔNG CỤ ĐỘC LẬP

#### 4.1. Thực thi SMT Z3 độc lập
```bash
# Ví dụ chạy bài toán tràn số capacity
z3 verification/step3/smt/smt_001a_capacity_growth.smt2
z3 verification/step3/smt/smt_001b_capacity_growth_reach.smt2
```

#### 4.2. Thực thi Harness CBMC độc lập
```bash
gcc -O0 -g -Isource/include verification/step3/cbmc/harness_cbmc_001_product_list_invariant.c \
    source/src/product.c source/src/inventory.c source/src/validation.c source/src/utils.c \
    -o harness_001.exe
./harness_001.exe
```

#### 4.3. Thực thi Thí nghiệm đối chứng SIM-003 (Negative Control)
```bash
gcc -O1 -g -fstack-protector-strong -Isource/include \
    tests/simulation/test_sim003_negative_control.c \
    tests/simulation/vulnerable_file_io.c \
    source/src/file_io.c source/src/product.c source/src/inventory.c source/src/validation.c source/src/utils.c \
    -o test_sim003.exe
./test_sim003.exe
```
**Kết quả hiển thị**:
- Thí nghiệm 1 (Baseline v0): An toàn, không tràn stack.
- Thí nghiệm 2 (Seeded Branch): Trình phát hiện `*** stack smashing detected ***` và tiến trình con bị terminated với mã `0xC0000409`.

#### 4.4. Thực thi NuSMV Model Checking
```bash
python verification/step3/ctl/verify_ctl.py
```

---

### 5. KIỂM TRA TÍNH TOÀN VẸN CỦA BẰNG CHỨNG (INTEGRITY CHECK)

Sau khi tái lập, kiểm tra mã băm SHA-256 của các tệp log và báo cáo bằng cách đối chiếu với `results/step3/evidence_manifest.csv`:
```bash
# Trên Windows PowerShell:
Get-FileHash results/step3/smt/smt_execution_log.txt -Algorithm SHA256
Get-FileHash results/step3/simulation/sim003_negative_control.log -Algorithm SHA256
```
Tất cả các tệp đều được lưu vết minh bạch và có thể kiểm tra chéo độc lập.
