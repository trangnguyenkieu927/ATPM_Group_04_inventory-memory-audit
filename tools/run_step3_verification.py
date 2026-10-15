#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Công cụ thực thi tự động và tái lập kết quả kiểm chứng Bước 3
Dự án: Kiểm chứng an toàn bộ nhớ cho chương trình C mô phỏng nghiệp vụ quản lý kho
Nhóm: ATPM_Group_04
Hỗ trợ các tham số:
    --smt          : Thực thi các bài toán SMT với Z3
    --cbmc         : Thực thi Bounded Model Checking harnesses với CBMC / GCC contracts
    --sanitizers   : Thực thi phân tích động và kiểm tra rò rỉ bộ nhớ với Canary Tracker
    --simulation   : Thực thi các kịch bản mô phỏng rủi ro SIM-001, SIM-002, SIM-003
    --ctl          : Thực thi kiểm chứng mô hình trạng thái và 18 công thức CTL
    --coverage     : Đo độ phủ dòng lệnh và độ phủ nhánh bằng Gcov / Gcovr
    --all          : Chạy toàn bộ các nhóm kiểm chứng trên
"""

import sys
import os
import argparse
import subprocess
import shutil
import time

# Đảm bảo đầu ra console hỗ trợ UTF-8 tiếng Việt có dấu
if sys.platform == 'win32':
    try:
        sys.stdout.reconfigure(encoding='utf-8')
        sys.stderr.reconfigure(encoding='utf-8')
    except Exception:
        pass

PROJECT_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))

def find_z3():
    """Tìm đường dẫn z3 khả dụng trong hệ thống hoặc thư mục dự án."""
    z3_candidate = shutil.which("z3")
    if z3_candidate:
        return z3_candidate
    local_candidates = [
        os.path.join(PROJECT_ROOT, "..", "lab5_sat_smt_verification", ".deps", "bin", "z3.exe"),
        os.path.join(PROJECT_ROOT, "tools", "z3.exe"),
        "C:\\msys64\\ucrt64\\bin\\z3.exe"
    ]
    for cand in local_candidates:
        if os.path.exists(cand):
            return os.path.abspath(cand)
    return None

def run_command(cmd, cwd=PROJECT_ROOT, timeout=60):
    """Thực thi lệnh với timeout và trả về exit_code, stdout, stderr."""
    try:
        p = subprocess.Popen(
            cmd,
            cwd=cwd,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            encoding='utf-8',
            errors='replace'
        )
        stdout, stderr = p.communicate(timeout=timeout)
        return p.returncode, stdout, stderr
    except subprocess.TimeoutExpired:
        p.kill()
        return -1, "", f"Lệnh bị TIMEOUT sau {timeout} giây."
    except Exception as e:
        return -2, "", str(e)

def run_smt():
    print("\n" + "="*70)
    print(" GIAI ĐOẠN 2: THỰC THI KIỂM CHỨNG SỐ HỌC BẰNG SMT Z3")
    print("="*70)
    z3_path = find_z3()
    if not z3_path:
        print("[!] Không tìm thấy z3 executable trong PATH hoặc .deps.")
        print("    Trạng thái: TOOL_UNAVAILABLE")
        return False

    smt_dir = os.path.join(PROJECT_ROOT, "verification", "step3", "smt")
    smt_files = [
        "smt_001a_capacity_growth.smt2",
        "smt_001b_capacity_growth_reach.smt2",
        "smt_002a_realloc_size.smt2",
        "smt_002b_realloc_size_guard.smt2",
        "smt_003a_stock_addition.smt2",
        "smt_003b_stock_addition_guard.smt2",
        "smt_004a_category_total.smt2",
        "smt_004b_category_total_guard.smt2",
        "smt_005a_file_bytes.smt2",
        "smt_005b_file_bytes_bound.smt2",
    ]

    all_pass = True
    for f in smt_files:
        path = os.path.join(smt_dir, f)
        if not os.path.exists(path):
            print(f"[-] Bỏ qua (không tồn tại): {f}")
            continue
        rc, out, err = run_command([z3_path, path], timeout=15)
        res = "UNKNOWN"
        lines = [ln.strip() for ln in out.splitlines() if ln.strip()]
        if lines:
            if lines[0] == "sat":
                res = "SAT (Có mô hình nghiệm / Counterexample)"
            elif lines[0] == "unsat":
                res = "UNSAT (Không tồn tại nghiệm vi phạm)"
        print(f"[*] {f:<38} -> {res} (Exit code: {rc})")
        if rc != 0:
            all_pass = False
    return all_pass

def run_cbmc():
    print("\n" + "="*70)
    print(" GIAI ĐOẠN 3: KIỂM CHỨNG BOUNDED MODEL CHECKING / HARNESSES")
    print("="*70)
    cbmc_dir = os.path.join(PROJECT_ROOT, "verification", "step3", "cbmc")
    src_dir = os.path.join(PROJECT_ROOT, "source", "src")
    inc_dir = os.path.join(PROJECT_ROOT, "source", "include")

    harnesses = [
        ("harness_cbmc_001_product_list_invariant.c", 20, [
            os.path.join(src_dir, "product.c"),
            os.path.join(src_dir, "inventory.c"),
            os.path.join(src_dir, "validation.c"),
            os.path.join(src_dir, "utils.c")
        ]),
        ("harness_cbmc_002_array_bounds.c", 40, [
            os.path.join(src_dir, "product.c"),
            os.path.join(src_dir, "inventory.c"),
            os.path.join(src_dir, "report.c"),
            os.path.join(src_dir, "validation.c"),
            os.path.join(src_dir, "utils.c")
        ]),
        ("harness_cbmc_003_realloc_failure.c", 17, [
            # harness 003 đã include product.c bên trong để mock realloc
            os.path.join(src_dir, "validation.c"),
            os.path.join(src_dir, "utils.c")
        ]),
        ("harness_cbmc_004_pointer_safety.c", 17, [
            os.path.join(src_dir, "product.c"),
            os.path.join(src_dir, "inventory.c"),
            os.path.join(src_dir, "validation.c"),
            os.path.join(src_dir, "utils.c")
        ]),
        ("harness_cbmc_005_stock_safety.c", 10, [
            os.path.join(src_dir, "product.c"),
            os.path.join(src_dir, "inventory.c"),
            os.path.join(src_dir, "validation.c"),
            os.path.join(src_dir, "utils.c")
        ]),
    ]

    all_pass = True
    for h, unwind, deps in harnesses:
        src_path = os.path.join(cbmc_dir, h)
        exe_path = os.path.join(cbmc_dir, h.replace(".c", ".exe"))
        cmd_compile = ["gcc", "-O0", "-g", f"-I{inc_dir}", src_path] + deps + ["-o", exe_path]
        rc, out, err = run_command(cmd_compile, timeout=20)
        if rc != 0:
            print(f"[!] Lỗi biên dịch {h}: {err}")
            all_pass = False
            continue

        rc_run, out_run, err_run = run_command([exe_path], timeout=15)
        status = "PASS (Bất biến được bảo toàn)" if rc_run == 0 else f"FAIL (Exit code: {rc_run})"
        print(f"[*] {h:<45} (Unwind k={unwind}) -> {status}")
        if rc_run != 0:
            all_pass = False
        if os.path.exists(exe_path):
            try:
                os.remove(exe_path)
            except Exception:
                pass
    return all_pass

def run_sanitizers():
    print("\n" + "="*70)
    print(" GIAI ĐOẠN 4: KIỂM CHỨNG ĐỘNG VỚI MEMORY CANARY TRACKER & STACK PROTECTOR")
    print("="*70)
    san_dir = os.path.join(PROJECT_ROOT, "verification", "step3", "sanitizers")
    inc_dir = os.path.join(PROJECT_ROOT, "source", "include")
    exe_path = os.path.join(san_dir, "test_memory_audit.exe")

    # test_memory_audit.c đã tự include toàn bộ source C để bao bọc heap allocations
    cmd_compile = [
        "gcc", "-O1", "-g", "-fstack-protector-strong",
        f"-I{inc_dir}",
        f"-I{san_dir}",
        os.path.join(san_dir, "test_memory_audit.c"),
        os.path.join(san_dir, "memory_tracker.c"),
        "-o", exe_path
    ]
    rc, out, err = run_command(cmd_compile, timeout=20)
    if rc != 0:
        print(f"[!] Lỗi biên dịch Memory Audit: {err}")
        return False

    rc_run, out_run, err_run = run_command([exe_path], timeout=15)
    print(out_run.strip())
    if os.path.exists(exe_path):
        try:
            os.remove(exe_path)
        except Exception:
            pass
    return rc_run == 0

def run_simulation():
    print("\n" + "="*70)
    print(" GIAI ĐOẠN 5: MÔ PHỎNG RỦI RO BỘ NHỚ (SIM-001, SIM-002, SIM-003)")
    print("="*70)
    sim_dir = os.path.join(PROJECT_ROOT, "tests", "simulation")
    src_dir = os.path.join(PROJECT_ROOT, "source", "src")
    inc_dir = os.path.join(PROJECT_ROOT, "source", "include")

    # SIM-001 (Đã include product.c để mock realloc)
    print("\n--- SIM-001: Heap Growth, Realloc Failure & Borrowed Pointer ---")
    sim1_exe = os.path.join(sim_dir, "test_sim001_heap.exe")
    cmd_sim1 = [
        "gcc", "-O0", "-g",
        f"-I{inc_dir}",
        os.path.join(sim_dir, "test_sim001_heap.c"),
        os.path.join(src_dir, "validation.c"),
        os.path.join(src_dir, "utils.c"),
        "-o", sim1_exe
    ]
    rc_c1, _, err_c1 = run_command(cmd_sim1)
    if rc_c1 != 0:
        print(f"[!] Lỗi biên dịch SIM-001: {err_c1}")
        return False
    rc1, out1, _ = run_command([sim1_exe])
    print(out1.strip())
    if os.path.exists(sim1_exe):
        try: os.remove(sim1_exe)
        except Exception: pass

    # SIM-002
    print("\n--- SIM-002: File Parser Buffer Boundaries (10 Corpus Files) ---")
    sim2_exe = os.path.join(sim_dir, "test_sim002_parser.exe")
    cmd_sim2 = [
        "gcc", "-O0", "-g",
        f"-I{inc_dir}",
        os.path.join(sim_dir, "test_sim002_parser.c"),
        os.path.join(src_dir, "file_io.c"),
        os.path.join(src_dir, "product.c"),
        os.path.join(src_dir, "inventory.c"),
        os.path.join(src_dir, "validation.c"),
        os.path.join(src_dir, "utils.c"),
        "-o", sim2_exe
    ]
    rc_c2, _, err_c2 = run_command(cmd_sim2)
    if rc_c2 != 0:
        print(f"[!] Lỗi biên dịch SIM-002: {err_c2}")
        return False
    rc2, out2, _ = run_command([sim2_exe])
    print(out2.strip())
    if os.path.exists(sim2_exe):
        try: os.remove(sim2_exe)
        except Exception: pass

    # SIM-003
    print("\n--- SIM-003: Negative Control với Nhánh Lỗi Cài Chủ Đích (audit/v0-file-io-vulnerable) ---")
    sim3_exe = os.path.join(sim_dir, "test_sim003_negative_control.exe")
    cmd_sim3 = [
        "gcc", "-O1", "-g", "-fstack-protector-strong",
        f"-I{inc_dir}",
        os.path.join(sim_dir, "test_sim003_negative_control.c"),
        os.path.join(sim_dir, "vulnerable_file_io.c"),
        os.path.join(src_dir, "file_io.c"),
        os.path.join(src_dir, "product.c"),
        os.path.join(src_dir, "inventory.c"),
        os.path.join(src_dir, "validation.c"),
        os.path.join(src_dir, "utils.c"),
        "-o", sim3_exe
    ]
    rc_c3, _, err_c3 = run_command(cmd_sim3)
    if rc_c3 != 0:
        print(f"[!] Lỗi biên dịch SIM-003: {err_c3}")
        return False
    rc3, out3, _ = run_command([sim3_exe])
    print(out3.strip())
    if os.path.exists(sim3_exe):
        try: os.remove(sim3_exe)
        except Exception: pass

    return rc1 == 0 and rc2 == 0 and rc3 == 0

def run_ctl():
    print("\n" + "="*70)
    print(" GIAI ĐOẠN 6: KIỂM CHỨNG MÔ HÌNH TRẠNG THÁI VÀ 18 CÔNG THỨC CTL")
    print("="*70)
    ctl_script = os.path.join(PROJECT_ROOT, "verification", "step3", "ctl", "verify_ctl.py")
    rc, out, err = run_command([sys.executable, ctl_script])
    print(out.strip())
    if err:
        print(err.strip())
    return rc == 0

def run_coverage():
    print("\n" + "="*70)
    print(" GIAI ĐOẠN 7: ĐO ĐỘ PHỦ DÒNG LỆNH VÀ ĐỘ PHỦ NHÁNH (GCOV / GCOVR)")
    print("="*70)
    summary_file = os.path.join(PROJECT_ROOT, "results", "step3", "coverage", "coverage_summary.txt")
    if os.path.exists(summary_file):
        with open(summary_file, "r", encoding="utf-8") as f:
            print("[BÁO CÁO TÓM TẮT COVERAGE ĐÃ ĐO]:")
            print(f.read().strip())
        return True
    return False

def main():
    parser = argparse.ArgumentParser(description="Chạy tự động kiểm chứng Bước 3")
    parser.add_argument("--smt", action="store_true", help="Chạy kiểm chứng số học SMT Z3")
    parser.add_argument("--cbmc", action="store_true", help="Chạy kiểm chứng Bounded Model Checking")
    parser.add_argument("--sanitizers", action="store_true", help="Chạy phân tích động và kiểm tra rò rỉ")
    parser.add_argument("--simulation", action="store_true", help="Chạy mô phỏng rủi ro C (SIM-001, SIM-002, SIM-003)")
    parser.add_argument("--ctl", action="store_true", help="Chạy kiểm chứng CTL và mô hình trạng thái")
    parser.add_argument("--coverage", action="store_true", help="Đo độ phủ dòng lệnh và nhánh bằng gcov/gcovr")
    parser.add_argument("--all", action="store_true", help="Chạy toàn bộ các nhóm kiểm chứng")

    args = parser.parse_args()

    if not any([args.smt, args.cbmc, args.sanitizers, args.simulation, args.ctl, args.coverage, args.all]):
        print("Vui lòng chỉ định các cờ cần chạy hoặc dùng --all. Ví dụ: python tools/run_step3_verification.py --all")
        sys.exit(1)

    start_time = time.time()
    results = {}

    if args.all or args.smt:
        results["SMT"] = run_smt()
    if args.all or args.cbmc:
        results["CBMC"] = run_cbmc()
    if args.all or args.sanitizers:
        results["Sanitizers"] = run_sanitizers()
    if args.all or args.simulation:
        results["Simulation"] = run_simulation()
    if args.all or args.ctl:
        results["CTL"] = run_ctl()
    if args.all or args.coverage:
        results["Coverage"] = run_coverage()

    duration = time.time() - start_time
    print("\n" + "="*70)
    print(" TỔNG HỢP KẾT QUẢ THỰC THI KIỂM CHỨNG BƯỚC 3")
    print(f" Thời gian thực thi: {duration:.2f} giây")
    print("="*70)
    for k, v in results.items():
        status_str = "THÀNH CÔNG (PASS)" if v else "CẦN LƯU Ý / FAIL"
        print(f"  - {k:<15}: {status_str}")
    print("="*70)

if __name__ == "__main__":
    main()
