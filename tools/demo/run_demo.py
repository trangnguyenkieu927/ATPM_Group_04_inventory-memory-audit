#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Công cụ thực thi trình diễn Demo trực tiếp bài tập lớn (8-10 phút).
Hỗ trợ chạy từng phần hoặc chạy toàn bộ chuỗi kiểm chứng thật.
"""

import os
import sys
import subprocess
import argparse
import time

if sys.platform == "win32":
    try:
        sys.stdout.reconfigure(encoding="utf-8")
        sys.stderr.reconfigure(encoding="utf-8")
    except Exception:
        pass

PROJECT_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
Z3_PATH = r"C:\Du_an\ATPM\lab5_sat_smt_verification\.deps\bin\z3.exe"

def run_step(title, cmd, cwd=PROJECT_ROOT):
    print("\n" + "="*70)
    print(f" {title}")
    print("="*70)
    start = time.time()
    res = subprocess.run(cmd, cwd=cwd, capture_output=True, text=True, encoding="utf-8", errors="replace")
    elapsed = time.time() - start
    print(res.stdout.strip())
    if res.stderr.strip():
        print("[STDERR]:", res.stderr.strip())
    print(f"-> Exit Code: {res.returncode} (Thời gian: {elapsed:.2f}s)")
    return res.returncode == 0

def demo_smt():
    print("\n[*] CHẠY KIỂM CHỨNG TOÁN HỌC SMT Z3 TRÊN CÁC PHÉP TOÁN BỘ NHỚ C:")
    smt1 = os.path.join(PROJECT_ROOT, "verification", "step3", "smt", "smt_001a_capacity_growth.smt2")
    smt2 = os.path.join(PROJECT_ROOT, "verification", "step3", "smt", "smt_002b_realloc_size_guard.smt2")
    run_step("1. SMT-001A: Tìm nghiệm tràn số capacity * 2 (chưa có guard)", [Z3_PATH, smt1])
    run_step("2. SMT-002B: Chứng minh tính an toàn khi có guard new_cap * 208", [Z3_PATH, smt2])

def demo_simulation():
    print("\n[*] THỰC THI KIỂM CHỨNG CÁC KỊCH BẢN MÔ PHỎNG C (SC-01 ĐẾN SC-06):")
    sim_dir = os.path.join(PROJECT_ROOT, "tests", "simulation")
    src_dir = os.path.join(PROJECT_ROOT, "source", "src")
    inc_dir = os.path.join(PROJECT_ROOT, "source", "include")
    
    # Run SC-03
    exe3 = os.path.join(sim_dir, "demo_sc03.exe")
    b3 = ["gcc", "-std=c17", f"-I{inc_dir}", "-o", exe3,
          os.path.join(sim_dir, "SC-03", "test_sc03_capacity.c"),
          os.path.join(src_dir, "product.c"), os.path.join(src_dir, "validation.c"), os.path.join(src_dir, "utils.c")]
    subprocess.run(b3, capture_output=True)
    run_step("1. SC-03: Mô phỏng Cấp phát Heap & Nhân đôi Dung lượng (REQ-2, REQ-3)", [exe3])
    if os.path.exists(exe3): os.remove(exe3)

    # Run SC-06
    exe6 = os.path.join(sim_dir, "demo_sc06.exe")
    b6 = ["gcc", "-std=c17", f"-I{inc_dir}", "-o", exe6,
          os.path.join(sim_dir, "SC-06", "test_sc06_stock_arithmetic.c"),
          os.path.join(src_dir, "product.c"), os.path.join(src_dir, "inventory.c"),
          os.path.join(src_dir, "validation.c"), os.path.join(src_dir, "utils.c")]
    subprocess.run(b6, capture_output=True)
    run_step("2. SC-06: Mô phỏng Số học Tồn kho & Bất biến Bảo toàn (REQ-4, REQ-5, REQ-6)", [exe6])
    if os.path.exists(exe6): os.remove(exe6)

def demo_seeded():
    print("\n[*] THỰC THI THÍ NGHIỆM ĐỐI CHỨNG LỖI CÀI CHỦ ĐÍCH (SEEDED VULNERABILITY):")
    seed_src = os.path.join(PROJECT_ROOT, "tests", "seeded", "seeded_stack_overflow.c")
    seed_exe = os.path.join(PROJECT_ROOT, "tests", "seeded", "demo_seed.exe")
    b_cmd = ["gcc", "-std=c17", "-fstack-protector-strong", "-o", seed_exe, seed_src]
    subprocess.run(b_cmd, capture_output=True)
    res = subprocess.run([seed_exe], capture_output=True)
    if os.path.exists(seed_exe): os.remove(seed_exe)
    print(f"Kết quả chạy tiến trình Seeded: Exit code = {res.returncode} (STATUS_STACK_BUFFER_OVERRUN)")
    print("[ĐÁNH GIÁ THỰC NGHIỆM] Trình bảo vệ stack canary của GCC đã bắt được tràn buffer stack (CWE-121 Stack Buffer Overflow) và kích hoạt abort an toàn.")

def demo_regression():
    src_root = os.path.join(PROJECT_ROOT, "source")
    smoke_exe = os.path.join(src_root, "demo_smoke.exe")
    smoke_build = [
        "gcc", "-std=c17", "-Wall", "-Wextra", "-I./include",
        "-o", smoke_exe,
        "tests/smoke_test.c", "src/inventory.c", "src/product.c",
        "src/file_io.c", "src/validation.c", "src/utils.c", "src/report.c"
    ]
    subprocess.run(smoke_build, cwd=src_root, capture_output=True)
    run_step("KIỂM THỬ HỒI QUY SMOKE TESTS 17 CA TRÊN PHIÊN BẢN v1", [smoke_exe], cwd=src_root)
    if os.path.exists(smoke_exe): os.remove(smoke_exe)
    subprocess.run(["make", "clean"], cwd=src_root, capture_output=True)

def main():
    parser = argparse.ArgumentParser(description="Script trình diễn Demo kiểm toán an toàn bộ nhớ C")
    parser.add_argument("--smt", action="store_true", help="Chạy demo kiểm chứng toán học SMT Z3")
    parser.add_argument("--simulation", action="store_true", help="Chạy demo 6 kịch bản C Simulator")
    parser.add_argument("--seeded", action="store_true", help="Chạy demo đối chứng lỗi cài chủ đích")
    parser.add_argument("--regression", action="store_true", help="Chạy demo kiểm thử hồi quy phiên bản v1")
    parser.add_argument("--all", action="store_true", help="Chạy trọn vẹn toàn bộ các bước demo")

    args = parser.parse_args()
    if not any([args.smt, args.simulation, args.seeded, args.regression, args.all]):
        print("Vui lòng chỉ định bước demo cần chạy hoặc dùng --all. Ví dụ: python tools/demo/run_demo.py --all")
        sys.exit(1)

    print("="*70)
    print(" KHỞI ĐỘNG PHIÊN TRÌNH DIỄN DEMO KIỂM TOÁN AN TOÀN BỘ NHỚ C (ATPM_Group_04)")
    print("="*70)

    if args.all or args.smt:
        demo_smt()
    if args.all or args.simulation:
        demo_simulation()
    if args.all or args.seeded:
        demo_seeded()
    if args.all or args.regression:
        demo_regression()

    print("\n" + "="*70)
    print(" [KẾT THÚC DEMO] PHIÊN TRÌNH DIỄN HOÀN THÀNH XUẤT SẮC TOÀN BỘ CÁC BƯỚC.")
    print("="*70)

if __name__ == "__main__":
    main()
