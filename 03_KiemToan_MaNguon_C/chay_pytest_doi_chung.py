import subprocess
import os
import sys

print("============================================================")
print("  CHẠY BỘ KIỂM TOÁN ĐỐI CHỨNG TEST SUITE TRÊN BẢN VÁ V1   ")
print("============================================================")
shim_dir = os.path.join(os.path.dirname(__file__), "..", "05_TaiLieu_LuuTru", "phu_tro_va_shims")
cmd = [sys.executable, "-m", "pytest", "test_verify_v0_cases_on_v1.py", "test_audit_io_pipeline_v1_wrap.py", "test_audit_heap_lifecycle_v1_wrap.py", "test_audit_arithmetic_v1_wrap.py", "-v"]
res = subprocess.run(cmd, cwd=shim_dir)
sys.exit(res.returncode)
