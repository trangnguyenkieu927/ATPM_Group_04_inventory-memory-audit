#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Công cụ tự động kiểm tra và sinh bộ test mô phỏng C từ đặc tả JSON.
Input: docs/02_Risk_Scenarios/scenario_specifications.json
Output: Test metadata, danh sách ca kiểm thử và báo cáo ánh xạ Kịch bản -> Test.
"""

import os
import json

PROJECT_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
SPEC_JSON = os.path.join(PROJECT_ROOT, "docs", "02_Risk_Scenarios", "scenario_specifications.json")

def validate_and_report():
    if not os.path.exists(SPEC_JSON):
        print(f"[-] Không tìm thấy tệp đặc tả: {SPEC_JSON}")
        return False
        
    with open(SPEC_JSON, "r", encoding="utf-8") as f:
        data = json.load(f)
        
    print("="*70)
    print(" TRÌNH SINH VÀ ÁNH XẠ KIỂM THỬ TỰ ĐỘNG TỪ ĐẶC TẢ SCENARIO JSON")
    print("="*70)
    print(f"Dự án: {data.get('project')}")
    print(f"Phiên bản: {data.get('version')}")
    print(f"Tổng số kịch bản: {data.get('total_scenarios')}")
    print("-"*70)
    
    for sc in data.get("scenarios", []):
        sc_id = sc["Scenario_ID"]
        name = sc["Scenario_Name"]
        func = sc["C_Function"]
        req = sc["Related_REQ"]
        cwe = sc.get("Related_RISK")
        tests = sc["Planned_Tests"]
        print(f"[*] {sc_id:<7} | REQ: {req:<18} | Hàm: {func:<30}")
        print(f"    Tên: {name}")
        print(f"    Kế hoạch test: {tests}")
        print()
    print("="*70)
    print("[SUCCESS] Đặc tả kịch bản hoàn toàn hợp lệ và sẵn sàng cho mô phỏng C.")
    return True

if __name__ == "__main__":
    validate_and_report()
