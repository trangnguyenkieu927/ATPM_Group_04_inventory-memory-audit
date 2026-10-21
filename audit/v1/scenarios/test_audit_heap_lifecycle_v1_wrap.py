"""Pytest WRAP cho mã C — sinh bởi sec_lab_framework (core/c_wrapper.py).
Chạy:  pytest -v test_audit_heap_lifecycle_v1_wrap.py
Cần: module audit_heap_lifecycle_v1_wrap.py (+ thư viện đã build) cùng thư mục."""
import pytest
import audit_heap_lifecycle_v1_wrap as w

def test_wrap_calc_expand_capacity_v1():
    # int calc_expand_capacity_v1(int current_cap, int factor)
    assert callable(w.calc_expand_capacity_v1)
    r = w.call_isolated('calc_expand_capacity_v1', 1, 1)
    assert r["status"] in ("ok", "crash", "exit"), r   # wrap gọi được (crash = lỗi CỦA MÃ C, không phải lỗi wrap)

def test_wrap_realloc_buffer_leak_v1():
    # int realloc_buffer_leak_v1(int old_size, int new_size, int simulate_oom)
    assert callable(w.realloc_buffer_leak_v1)
    r = w.call_isolated('realloc_buffer_leak_v1', 1, 1, 1)
    assert r["status"] in ("ok", "crash", "exit"), r   # wrap gọi được (crash = lỗi CỦA MÃ C, không phải lỗi wrap)

def test_wrap_cleanup_and_read_uaf_v1():
    # int cleanup_and_read_uaf_v1(int val)
    assert callable(w.cleanup_and_read_uaf_v1)
    r = w.call_isolated('cleanup_and_read_uaf_v1', 1)
    assert r["status"] in ("ok", "crash", "exit"), r   # wrap gọi được (crash = lỗi CỦA MÃ C, không phải lỗi wrap)
