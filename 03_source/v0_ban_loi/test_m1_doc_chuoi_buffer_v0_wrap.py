"""Pytest WRAP cho mã C — sinh bởi sec_lab_framework (core/c_wrapper.py).
Chạy:  pytest -v test_m1_doc_chuoi_buffer_v0_wrap.py
Cần: module m1_doc_chuoi_buffer_v0_wrap.py (+ thư viện đã build) cùng thư mục."""
import pytest
import m1_doc_chuoi_buffer_v0_wrap as w

def test_wrap_ingest_raw_line():
    # void ingest_raw_line(const char * raw_line)
    assert callable(w.ingest_raw_line)
    r = w.call_isolated('ingest_raw_line', 'hello')
    assert r["status"] in ("ok", "crash", "exit"), r   # wrap gọi được (crash = lỗi CỦA MÃ C, không phải lỗi wrap)

def test_wrap_get_token_field():
    # int get_token_field(int token_count, int field_idx)
    assert callable(w.get_token_field)
    r = w.call_isolated('get_token_field', 1, 1)
    assert r["status"] in ("ok", "crash", "exit"), r   # wrap gọi được (crash = lỗi CỦA MÃ C, không phải lỗi wrap)

def test_wrap_ingest_raw_line_v0():
    # int ingest_raw_line_v0(char * dst, int cap, const char * raw_line)
    assert callable(w.ingest_raw_line_v0)
    r = w.call_isolated('ingest_raw_line_v0', 'hello', 4, 'hello')
    assert r["status"] in ("ok", "crash", "exit"), r   # wrap gọi được (crash = lỗi CỦA MÃ C, không phải lỗi wrap)

def test_wrap_get_token_field_v0():
    # int get_token_field_v0(const int * token_offsets, int token_count, int field_idx)
    assert callable(w.get_token_field_v0)
    r = w.call_isolated('get_token_field_v0', [1, 2, 3, 4], 4, 1)
    assert r["status"] in ("ok", "crash", "exit"), r   # wrap gọi được (crash = lỗi CỦA MÃ C, không phải lỗi wrap)
