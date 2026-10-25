import os
print("=== SƠ ĐỒ TRẠNG THÁI KRIPKE TỔNG THỂ & 3 MÔ HÌNH CON ===")
print("S0 (Chờ input) -> S1 (Nạp Buffer) -> S2 (Tách Token) -> S3 (Tính Capacity) -> S4 (Cấp phát Heap) -> S5 (Cộng kho) -> S6 (Giải phóng)")
print("Bản v0: Tồn tại nhánh vi phạm: S1 -> S_overflow | S4 -> S_leak | S5 -> S_wrap_around (SAT)")
print("Bản v1: Cài Defensive Guards: Chặn đứng mọi nhánh lỗi -> Tất cả đi về S_safe_complete (UNSAT)")
