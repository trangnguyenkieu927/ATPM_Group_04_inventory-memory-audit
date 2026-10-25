import subprocess
import os
import sys

print("=== CHẠY KIỂM CHỨNG HÌNH THỨC KRIPKE TRÊN 18 THUỘC TÍNH CTL ===")
target_dir = os.path.join(os.path.dirname(__file__), "..", "05_TaiLieu_LuuTru", "smt_smv_goc")
res = subprocess.run([sys.executable, "verify_ctl.py"], cwd=target_dir)
sys.exit(res.returncode)
