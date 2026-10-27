"""Pytest WRAP cho mã C — sinh bởi sec_lab_framework (core/c_wrapper.py).
Chạy:  pytest -v test_m3_tinh_ton_kho_v0_wrap.py
Cần: module m3_tinh_ton_kho_v0_wrap.py (+ thư viện đã build) cùng thư mục."""
import pytest
import m3_tinh_ton_kho_v0_wrap as w

def test_wrap_update_stock_quantity():
    # int update_stock_quantity(int current_stock, int added_quantity)
    assert callable(w.update_stock_quantity)
    r = w.call_isolated('update_stock_quantity', 1, 1)
    assert r["status"] in ("ok", "crash", "exit"), r   # wrap gọi được (crash = lỗi CỦA MÃ C, không phải lỗi wrap)

def test_wrap_scale_product_price():
    # int scale_product_price(int base_price, int rate)
    assert callable(w.scale_product_price)
    r = w.call_isolated('scale_product_price', 1, 1)
    assert r["status"] in ("ok", "crash", "exit"), r   # wrap gọi được (crash = lỗi CỦA MÃ C, không phải lỗi wrap)

def test_wrap_update_stock_quantity_v0():
    # int update_stock_quantity_v0(int current_stock, int added_quantity)
    assert callable(w.update_stock_quantity_v0)
    r = w.call_isolated('update_stock_quantity_v0', 1, 1)
    assert r["status"] in ("ok", "crash", "exit"), r   # wrap gọi được (crash = lỗi CỦA MÃ C, không phải lỗi wrap)

def test_wrap_scale_product_price_v0():
    # int scale_product_price_v0(int base_price, int rate)
    assert callable(w.scale_product_price_v0)
    r = w.call_isolated('scale_product_price_v0', 1, 1)
    assert r["status"] in ("ok", "crash", "exit"), r   # wrap gọi được (crash = lỗi CỦA MÃ C, không phải lỗi wrap)
