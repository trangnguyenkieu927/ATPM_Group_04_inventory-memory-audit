"""Pytest WRAP cho mã C — sinh bởi sec_lab_framework (core/c_wrapper.py).
Chạy:  pytest -v test_audit_heap_lifecycle_v0_wrap.py
Cần: module audit_heap_lifecycle_v0_wrap.py (+ thư viện đã build) cùng thư mục."""
import pytest
import audit_heap_lifecycle_v0_wrap as w

def test_wrap_calc_expand_capacity_v0():
    # int calc_expand_capacity_v0(int current_cap, int factor)
    assert callable(w.calc_expand_capacity_v0)
    r = w.call_isolated('calc_expand_capacity_v0', 1, 1)
    assert r["status"] in ("ok", "crash", "exit"), r   # wrap gọi được (crash = lỗi CỦA MÃ C, không phải lỗi wrap)

def test_wrap_realloc_buffer_leak_v0():
    # int realloc_buffer_leak_v0(int old_size, int new_size, int simulate_oom)
    assert callable(w.realloc_buffer_leak_v0)
    r = w.call_isolated('realloc_buffer_leak_v0', 1, 1, 1)
    assert r["status"] in ("ok", "crash", "exit"), r   # wrap gọi được (crash = lỗi CỦA MÃ C, không phải lỗi wrap)

def test_wrap_cleanup_and_read_uaf_v0():
    # int cleanup_and_read_uaf_v0(int val)
    assert callable(w.cleanup_and_read_uaf_v0)
    r = w.call_isolated('cleanup_and_read_uaf_v0', 1)
    assert r["status"] in ("ok", "crash", "exit"), r   # wrap gọi được (crash = lỗi CỦA MÃ C, không phải lỗi wrap)
