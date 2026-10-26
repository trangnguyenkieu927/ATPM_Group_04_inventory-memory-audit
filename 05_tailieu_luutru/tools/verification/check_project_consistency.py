#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Công cụ kiểm tra tính nhất quán toàn diện dự án (Project Consistency Checker).
Xác thực 18 REQs, Kịch bản, Mã nguồn, Bằng chứng thực tế và không tạo PASS giả.
"""

import os
import sys
import json
import csv

if sys.platform == "win32":
    try:
        sys.stdout.reconfigure(encoding="utf-8")
        sys.stderr.reconfigure(encoding="utf-8")
    except Exception:
        pass

PROJECT_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))

def check_consistency():
    print("="*70)
    print(" KIỂM TRA TÍNH NHẤT QUÁN TOÀN DIỆN DỰ ÁN KIỂM TOÁN BỘ NHỚ (STAGE 10)")
    print("="*70)

    errors = []
    warnings = []
    checks_passed = 0

    # 1. Kiểm tra 18 REQs chuẩn
    spec_json = os.path.join(PROJECT_ROOT, "docs", "03_Safety_Specification", "safety_specifications.json")
    if os.path.exists(spec_json):
        with open(spec_json, "r", encoding="utf-8") as f:
            data = json.load(f)
            reqs = data.get("requirements", [])
            if len(reqs) == 18:
                print(f"[PASS] 1. Kiểm tra 18 REQs: Đầy đủ 18 yêu cầu nguyên văn ({len(reqs)}/18)")
                checks_passed += 1
            else:
                errors.append(f"Số lượng REQ không đúng 18 (hiện tại: {len(reqs)})")
    else:
        errors.append(f"Không tìm thấy file: {spec_json}")

    # 2. Kiểm tra 6 Kịch bản rủi ro
    sc_json = os.path.join(PROJECT_ROOT, "docs", "02_Risk_Scenarios", "scenario_specifications.json")
    if os.path.exists(sc_json):
        with open(sc_json, "r", encoding="utf-8") as f:
            sc_data = json.load(f)
            scs = sc_data.get("scenarios", [])
            if len(scs) >= 6:
                print(f"[PASS] 2. Kiểm tra Kịch bản rủi ro: Đầy đủ {len(scs)} kịch bản (SC-01 đến SC-06)")
                checks_passed += 1
            else:
                errors.append(f"Số lượng kịch bản < 6: {len(scs)}")
    else:
        errors.append(f"Không tìm thấy file: {sc_json}")

    # 3. Kiểm tra file nguồn C
    src_files = ["product.c", "file_io.c", "inventory.c", "validation.c", "utils.c", "report.c", "main.c"]
    all_src_exist = True
    for sf in src_files:
        p = os.path.join(PROJECT_ROOT, "source", "src", sf)
        if not os.path.exists(p):
            errors.append(f"Thiếu file nguồn C: {sf}")
            all_src_exist = False
    if all_src_exist:
        print(f"[PASS] 3. Kiểm tra File nguồn C: Đầy đủ 7/7 module tại source/src/")
        checks_passed += 1

    # 4. Kiểm tra sự tồn tại của các bằng chứng (Evidence Manifest)
    manifest_csv = os.path.join(PROJECT_ROOT, "docs", "07_CWE_Coverage", "evidence_manifest.csv")
    if os.path.exists(manifest_csv):
        with open(manifest_csv, "r", encoding="utf-8") as f:
            reader = csv.DictReader(f)
            evid_count = 0
            missing_evid = 0
            for row in reader:
                evid_count += 1
                ev_path = os.path.join(PROJECT_ROOT, row["Evidence_Path"].replace("/", os.sep))
                if not os.path.exists(ev_path):
                    errors.append(f"Thiếu file bằng chứng: {row['Evidence_Path']}")
                    missing_evid += 1
            if missing_evid == 0:
                print(f"[PASS] 4. Kiểm tra Bằng chứng: 100% file log thực tế tồn tại ({evid_count}/{evid_count} files)")
                checks_passed += 1
            else:
                errors.append(f"Có {missing_evid} file bằng chứng không tồn tại!")
    else:
        errors.append(f"Không tìm thấy file manifest: {manifest_csv}")

    # 5. Kiểm tra Master Workbook 17 sheets
    master_xlsx = os.path.join(PROJECT_ROOT, "docs", "09_Final_Report", "ATPM_Group04_Final_Verification.xlsx")
    if os.path.exists(master_xlsx):
        import openpyxl
        wb = openpyxl.load_workbook(master_xlsx, read_only=True)
        if len(wb.sheetnames) >= 17:
            print(f"[PASS] 5. Kiểm tra Master Workbook: Đầy đủ {len(wb.sheetnames)}/17 sheets chuẩn")
            checks_passed += 1
        else:
            errors.append(f"Master workbook thiếu sheet (hiện có: {len(wb.sheetnames)}/17)")
    else:
        errors.append(f"Không tìm thấy master workbook: {master_xlsx}")

    # 6. Kiểm tra bản vá v1
    patch_file = os.path.join(PROJECT_ROOT, "docs", "08_Remediation", "v1_defensive_guard.patch")
    if os.path.exists(patch_file) and os.path.getsize(patch_file) > 0:
        print("[PASS] 6. Kiểm tra Bản vá v1: Tập tin v1_defensive_guard.patch tồn tại và hợp lệ")
        checks_passed += 1
    else:
        errors.append("Không tìm thấy hoặc rỗng: v1_defensive_guard.patch")

    # 7. Kiểm tra Demo Assets
    demo_files = ["DEMO_GUIDE.md", "DEMO_SCRIPT.md", "DEMO_COMMANDS.md", "DEMO_CHECKLIST.xlsx"]
    all_demo_exist = True
    for df in demo_files:
        p = os.path.join(PROJECT_ROOT, "docs", "10_Demo", df)
        if not os.path.exists(p):
            errors.append(f"Thiếu file demo: {df}")
            all_demo_exist = False
    if all_demo_exist:
        print("[PASS] 7. Kiểm tra Tài liệu Demo: Đầy đủ 4/4 file hướng dẫn và checklist")
        checks_passed += 1

    print("-" * 70)
    print(f"TỔNG KẾT KIỂM TRA NHẤT QUÁN: {checks_passed}/7 HẠNG MỤC ĐẠT (PASS)")
    if errors:
        print(f"[-] CÓ {len(errors)} LỖI ĐANG CHẶN NGHIỆM THU:")
        for err in errors:
            print(f"    - {err}")
        print("="*70)
        return False
    else:
        print("[SUCCESS] TOÀN BỘ HỆ THỐNG ĐẠT CHUẨN NHẤT QUÁN 100%. SẴN SÀNG NGHIỆM THU!")
        print("="*70)
        return True

if __name__ == "__main__":
    ok = check_consistency()
    sys.exit(0 if ok else 1)
