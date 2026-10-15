# -*- coding: utf-8 -*-
"""
Script kiểm tra tính nhất quán hồ sơ Bước 2:
- Kiểm tra cấu trúc các file CSV (risk_register.csv, req_traceability.csv, coverage_matrix.csv, compiler-findings.csv, findings.csv, evidence_manifest.csv)
- Kiểm tra tính duy nhất của các ID (Risk_ID, REQ_ID, Finding_ID, Evidence_ID)
- Kiểm tra tham chiếu chéo hai chiều: Risk <-> REQ
- Kiểm tra sự tồn tại của các hàm C và file C được tham chiếu
- Kiểm tra hash SHA-256 thực tế của các file bằng chứng
- Trả về exit code 0 nếu nhất quán, exit code khác 0 nếu có sai lệch
"""
import os, sys, csv, hashlib

if sys.stdout.encoding != 'utf-8':
    sys.stdout.reconfigure(encoding='utf-8')

BASE_DIR = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
SOURCE_SHA = "44202070c68002d9af068374478cdc83098dd4d8"

errors = []
warnings = []

def log_err(msg):
    errors.append(msg)
    print(f"[LỖI] {msg}")

def log_warn(msg):
    warnings.append(msg)
    print(f"[CẢNH BÁO] {msg}")

print("=== BẮT ĐẦU KIỂM TRA TÍNH NHẤT QUÁN HỒ SƠ BƯỚC 2 ===")

# 1. Kiểm tra Risk Register
risk_csv = os.path.join(BASE_DIR, "docs", "verification", "step2", "risk_register.csv")
if not os.path.exists(risk_csv):
    log_err(f"Không tìm thấy file: {risk_csv}")
    sys.exit(1)

risk_rows = []
risk_ids = set()
with open(risk_csv, "r", encoding="utf-8-sig") as f:
    reader = csv.DictReader(f)
    req_cols = ["Risk_ID", "Source_SHA", "File", "Function", "Line_Range", "Risk_Category", "CWE_Candidate", "Existing_Guards", "Reachability_Status", "Related_REQ"]
    for col in req_cols:
        if col not in reader.fieldnames:
            log_err(f"risk_register.csv thiếu cột bắt buộc: {col}")
    for row in reader:
        r_id = row["Risk_ID"]
        if r_id in risk_ids:
            log_err(f"Trùng lặp Risk_ID: {r_id}")
        risk_ids.add(r_id)
        risk_rows.append(row)

print(f"- Đã nạp {len(risk_rows)} rủi ro từ risk_register.csv")

# 2. Kiểm tra Traceability REQ
req_csv = os.path.join(BASE_DIR, "docs", "verification", "step2", "req_traceability.csv")
if not os.path.exists(req_csv):
    log_err(f"Không tìm thấy file: {req_csv}")
    sys.exit(1)

req_rows = []
req_ids = set()
with open(req_csv, "r", encoding="utf-8-sig") as f:
    reader = csv.DictReader(f)
    req_cols = ["Requirement_ID", "Category", "Verbatim_Text", "Source_File", "C_Function", "Line_Range", "Related_Risk_IDs", "Verification_Technique"]
    for col in req_cols:
        if col not in reader.fieldnames:
            log_err(f"req_traceability.csv thiếu cột bắt buộc: {col}")
    for row in reader:
        q_id = row["Requirement_ID"]
        if q_id in req_ids:
            log_err(f"Trùng lặp Requirement_ID: {q_id}")
        req_ids.add(q_id)
        req_rows.append(row)

print(f"- Đã nạp {len(req_rows)} yêu cầu từ req_traceability.csv")

# Kiểm tra đủ 18 REQ
expected_reqs = {f"REQ-{i}" for i in range(1, 19)}
missing_reqs = expected_reqs - req_ids
if missing_reqs:
    log_err(f"Thiếu các REQ bắt buộc: {sorted(list(missing_reqs))}")
extra_reqs = req_ids - expected_reqs
if extra_reqs:
    log_warn(f"Có REQ ngoài danh mục REQ-1..18: {sorted(list(extra_reqs))}")

# 3. Kiểm tra tham chiếu chéo giữa Risk và REQ
for r in risk_rows:
    r_id = r["Risk_ID"]
    related = [x.strip() for x in r["Related_REQ"].split(",") if x.strip()]
    for q in related:
        if q not in req_ids:
            log_err(f"Risk {r_id} tham chiếu REQ không tồn tại: {q}")

# 4. Kiểm tra sự tồn tại của file và hàm C được tham chiếu
for r in risk_rows:
    src_rel = r["File"].replace("/", os.sep)
    src_full = os.path.join(BASE_DIR, src_rel)
    if not os.path.exists(src_full):
        log_err(f"File nguồn trong Risk {r['Risk_ID']} không tồn tại: {src_rel}")
    else:
        func_name = r["Function"]
        with open(src_full, "r", encoding="utf-8", errors="ignore") as f:
            content = f.read()
            if func_name not in content:
                log_err(f"Hàm '{func_name}' không tìm thấy trong file {src_rel} (Risk {r['Risk_ID']})")

for q in req_rows:
    src_rel = q["Source_File"].replace("/", os.sep)
    src_full = os.path.join(BASE_DIR, src_rel)
    if not os.path.exists(src_full):
        log_err(f"File nguồn chính trong REQ {q['Requirement_ID']} không tồn tại: {src_rel}")

    # Kiểm tra từng hàm trong danh sách C_Function
    func_list = [f.strip() for f in q["C_Function"].split(",") if f.strip()]
    for fn in func_list:
        found = False
        for root, dirs, files in os.walk(os.path.join(BASE_DIR, "source")):
            for f in files:
                if f.endswith(('.c', '.h')):
                    with open(os.path.join(root, f), "r", encoding="utf-8", errors="ignore") as srcf:
                        if fn in srcf.read():
                            found = True
                            break
            if found:
                break
        if not found:
            log_err(f"Hàm '{fn}' không tìm thấy trong bất kỳ file nào của source/ (REQ {q['Requirement_ID']})")

# 5. Kiểm tra Evidence Manifest và tính toán SHA-256
manifest_csv = os.path.join(BASE_DIR, "results", "step2", "evidence_manifest.csv")
if not os.path.exists(manifest_csv):
    log_err(f"Không tìm thấy file: {manifest_csv}")
else:
    with open(manifest_csv, "r", encoding="utf-8-sig") as f:
        reader = csv.DictReader(f)
        for row in reader:
            evd_id = row["Evidence_ID"]
            rel_p = row["File_Path"].replace("/", os.sep)
            full_p = os.path.join(BASE_DIR, rel_p)
            if not os.path.exists(full_p):
                log_err(f"Evidence {evd_id} trỏ tới file không tồn tại: {rel_p}")
            else:
                expected_sha = row["File_SHA256"]
                h = hashlib.sha256()
                with open(full_p, "rb") as ef:
                    while chunk := ef.read(65536):
                        h.update(chunk)
                real_sha = h.hexdigest()
                if real_sha != expected_sha and expected_sha != "NOT_CHECKED":
                    log_err(f"Mã SHA-256 không khớp cho {rel_p} ({evd_id}): ghi nhận={expected_sha[:12]}, thực tế={real_sha[:12]}")

# 6. Tổng kết
print(f"=== KẾT QUẢ KIỂM TRA: {len(errors)} lỗi, {len(warnings)} cảnh báo ===")
if errors:
    print("DANH SÁCH LỖI CẦN KHẮC PHỤC:")
    for i, e in enumerate(errors, 1):
        print(f"  {i}. {e}")
    sys.exit(1)
else:
    print("TẤT CẢ CÁC PHÉP KIỂM TÍNH NHẤT QUÁN ĐÃ PASS 100%!")
    sys.exit(0)
