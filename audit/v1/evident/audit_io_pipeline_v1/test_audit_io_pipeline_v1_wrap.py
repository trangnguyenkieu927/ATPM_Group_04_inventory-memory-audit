"""Pytest WRAP cho mã C — sinh bởi sec_lab_framework (core/c_wrapper.py).
Chạy:  pytest -v test_audit_io_pipeline_v1_wrap.py
Cần: module audit_io_pipeline_v1_wrap.py (+ thư viện đã build) cùng thư mục."""
import pytest
import audit_io_pipeline_v1_wrap as w

def test_wrap_ingest_raw_line_v1():
    # int ingest_raw_line_v1(char * dst, int cap, const char * raw_line)
    assert callable(w.ingest_raw_line_v1)
    r = w.call_isolated('ingest_raw_line_v1', 'hello', 4, 'hello')
    assert r["status"] in ("ok", "crash", "exit"), r   # wrap gọi được (crash = lỗi CỦA MÃ C, không phải lỗi wrap)

def test_wrap_get_token_field_v1():
    # int get_token_field_v1(const int * token_offsets, int token_count, int field_idx)
    assert callable(w.get_token_field_v1)
    r = w.call_isolated('get_token_field_v1', [1, 2, 3, 4], 4, 1)
    assert r["status"] in ("ok", "crash", "exit"), r   # wrap gọi được (crash = lỗi CỦA MÃ C, không phải lỗi wrap)
