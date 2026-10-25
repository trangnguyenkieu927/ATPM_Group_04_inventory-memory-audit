"""Pytest WRAP cho mã C — sinh bởi sec_lab_framework (core/c_wrapper.py).
Chạy:  pytest -v test_audit_arithmetic_v1_wrap.py
Cần: module audit_arithmetic_v1_wrap.py (+ thư viện đã build) cùng thư mục."""
import pytest
import audit_arithmetic_v1_wrap as w

def test_wrap_update_stock_quantity_v1():
    # int update_stock_quantity_v1(int current_stock, int added_quantity)
    assert callable(w.update_stock_quantity_v1)
    r = w.call_isolated('update_stock_quantity_v1', 1, 1)
    assert r["status"] in ("ok", "crash", "exit"), r   # wrap gọi được (crash = lỗi CỦA MÃ C, không phải lỗi wrap)

def test_wrap_scale_product_price_v1():
    # int scale_product_price_v1(int base_price, int rate)
    assert callable(w.scale_product_price_v1)
    r = w.call_isolated('scale_product_price_v1', 1, 1)
    assert r["status"] in ("ok", "crash", "exit"), r   # wrap gọi được (crash = lỗi CỦA MÃ C, không phải lỗi wrap)
