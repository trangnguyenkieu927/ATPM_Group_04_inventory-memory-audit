# CÁC LỆNH TRÌNH DIỄN DEMO NHANH (DEMO COMMANDS)

## 1. LỆNH CHẠY TOÀN BỘ CHUỖI TỰ ĐỘNG
```bash
python tools/demo/run_demo.py --all
```

## 2. CÁC LỆNH THAO TÁC THỦ CÔNG ĐỘC LẬP
### Bước 1: Kiểm tra Biên dịch Nghiêm ngặt
```bash
gcc -std=c17 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -I./source/include -c source/src/*.c
```

### Bước 2: Chạy SMT Z3 Solver
```bash
C:\Du_an\ATPM\lab5_sat_smt_verification\.deps\bin\z3.exe verification/step3/smt/smt_001a_capacity_growth.smt2
C:\Du_an\ATPM\lab5_sat_smt_verification\.deps\bin\z3.exe verification/step3/smt/smt_002b_realloc_size_guard.smt2
```

### Bước 3: Chạy 6 Kịch bản C Simulator
```bash
python tools/demo/run_demo.py --simulation
```

### Bước 4: Chạy Đối chứng Lỗi Cài Chủ Đích (Stack Smashing)
```bash
python tools/demo/run_demo.py --seeded
```

### Bước 5: Chạy Kiểm thử Hồi quy Phiên bản v1
```bash
python tools/demo/run_demo.py --regression
```

### Bước 6: Kiểm tra Tính Nhất Quán Toàn Dự Án
```bash
python tools/verification/check_project_consistency.py
```
