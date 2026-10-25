"""
=============================================================================
KIỂM THỬ ĐỐI CHỨNG: CHẠY BỘ TEST CASE CỦA v0 LÊN BẢN VÁ v1
Học phần: An toàn phần mềm - GVHD: ThS. Vũ Quang Dũng
Mục tiêu: Chứng minh bản vá v1 đã khắc phục triệt để 100% các test case gây lỗi của v0.
=============================================================================
"""
import pytest
import ctypes
import os
import subprocess

# Đường dẫn mã C bản vá v1
CURRENT_DIR = os.path.dirname(os.path.abspath(__file__))
C_FILE = os.path.join(CURRENT_DIR, "audit_heap_lifecycle_v1.c")
DLL_FILE = os.path.join(CURRENT_DIR, "audit_heap_lifecycle_v1.dll")

@pytest.fixture(scope="session", autouse=True)
def build_dll():
    """Biên dịch bản vá v1 thành thư viện DLL"""
    cmd = ["gcc", "-shared", "-fPIC", "-O0", "-g", C_FILE, "-o", DLL_FILE]
    res = subprocess.run(cmd, capture_output=True, text=True)
    assert res.returncode == 0, f"Biên dịch thất bại: {res.stderr}"

def call_v1(func_name, *args):
    """Gọi hàm C bản vá v1 qua ctypes"""
    lib = ctypes.CDLL(DLL_FILE)
    fn = getattr(lib, func_name)
    fn.argtypes = [ctypes.c_int] * len(args)
    fn.restype = ctypes.c_int
    return fn(*args)

# =============================================================================
# 10 TEST CASE CỦA BẢN v0 ÉP CHẠY VÀO BẢN VÁ v1
# =============================================================================

def test_case_01_boundary_extreme_size():
    """Test 1: Giá trị biên cực lớn từng làm v0 bị TIMEOUT (2 tỷ byte)
    v0: KẸT TIMEOUT do malloc 2GB
    v1: Phải chặn ngay lập tức và trả về mã lỗi an toàn -1 (PASS)"""
    ret = call_v1("realloc_buffer_leak_v1", 2147483646, 1, 1)
    assert ret == -1, f"Kỳ vọng Guard chặn trả về -1, thực tế: {ret}"

def test_case_02_boundary_normal_oom():
    """Test 2: Kích thước 1 byte, mô phỏng OOM
    v0: Gán buf = NULL làm mất địa chỉ -> Leak CWE-401
    v1: Từ chối OOM an toàn, trả về -2 (PASS)"""
    ret = call_v1("realloc_buffer_leak_v1", 1, 1, 1)
    assert ret == -2

def test_case_03_boundary_success():
    """Test 3: Kích thước 1 byte, không OOM -> Cấp phát và realloc thành công
    v1: Trả về 0 (Thành công)"""
    ret = call_v1("realloc_buffer_leak_v1", 1, 1, 0)
    assert ret == 0

def test_case_04_mutation_extreme_int_max():
    """Test 4: Giá trị đột biến INT_MAX từng làm v0 bị TIMEOUT
    v0: KẸT TIMEOUT
    v1: Phải chặn ngay lập tức và trả về -1 (PASS)"""
    ret = call_v1("realloc_buffer_leak_v1", 2147483647, 1, 1025)
    assert ret == -1

def test_case_05_mutation_new_size_zero():
    """Test 5: Kích thước mới = 0
    v1: Chặn kích thước không hợp lệ, trả về -1"""
    ret = call_v1("realloc_buffer_leak_v1", 1, 0, 0)
    assert ret == -1

def test_case_06_mutation_normal_oom():
    """Test 6: old_size=2, new_size=1, simulate_oom=1
    v1: Xử lý an toàn, trả về -2"""
    ret = call_v1("realloc_buffer_leak_v1", 2, 1, 1)
    assert ret == -2

def test_case_07_mutation_negative_size():
    """Test 7: Kích thước âm (old_size = -19)
    v0: Bị ép kiểu unsigned nguy hiểm
    v1: Chặn số âm ngay từ đầu, trả về -1"""
    ret = call_v1("realloc_buffer_leak_v1", -19, 1, 0)
    assert ret == -1

def test_case_08_coverage_guided_expand():
    """Test 8: Mở rộng hợp lệ lên 2049 bytes
    v1: Thành công, trả về 0"""
    ret = call_v1("realloc_buffer_leak_v1", 2, 2049, 0)
    assert ret == 0

def test_case_09_coverage_guided_negative_new_size():
    """Test 9: new_size âm (-43)
    v1: Chặn số âm, trả về -1"""
    ret = call_v1("realloc_buffer_leak_v1", 1, -43, 0)
    assert ret == -1

def test_case_10_concolic_negative_old_size():
    """Test 10: old_size = -1
    v1: Chặn số âm, trả về -1"""
    ret = call_v1("realloc_buffer_leak_v1", -1, 1, 1)
    assert ret == -1
