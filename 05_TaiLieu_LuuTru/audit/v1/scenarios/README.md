# Bản Vá An Toàn Bộ Nhớ v1 (Secure Patched Implementation)
**Học phần:** An toàn phần mềm (CSE703153) - **GVHD:** ThS. Vũ Quang Dũng  
**Nhóm thực hiện:** Nhóm 04 (Inventory Memory Safety Audit)

Thư mục này chứa 3 module C phiên bản **v1 đã được vá triệt để** dựa trên chính các test case và cảnh báo tìm được từ phiên bản v0.

---

## 1. Danh Sách Bản Vá & Biện Pháp Phòng Vệ

| STT | Module | Thành viên | File mã nguồn | Cơ chế phòng vệ an toàn |
|---|---|---|---|---|
| 1 | **Module 1: I/O Pipeline** | **Nguyễn Quang Thọ** | `audit_io_pipeline_v1.c` | Thay `strcpy` bằng kiểm tra `strlen < cap` an toàn; kiểm tra biên mảng `0 <= idx < token_count`. |
| 2 | **Module 2: Heap Lifecycle** | **Nguyễn Kiều Trang** | `audit_heap_lifecycle_v1.c` | Guard `MAX_SAFE_CAPACITY`, chặn số âm; dùng con trỏ tạm `tmp = realloc(...)`, dọn dẹp bộ nhớ an toàn; gán `buf = NULL` sau `free()` chống Use-After-Free. |
| 3 | **Module 3: Arithmetic** | **Hà Văn Đô** | `audit_arithmetic_v1.c` | Guard kiểm tra tràn số học trước khi thực hiện phép cộng (`current_stock > INT_MAX - added_quantity`) và phép nhân. |

---

## 2. Kiểm Chứng Thực Nghiệm Bản Vá v1

### Cách 1: Nạp vào Tool SecLabFramework
1. Nạp file v1 tương ứng trong thư mục này vào `SecLabFramework.exe`.
2. Bấm **"Quét mẫu lỗi"**: Kết quả sạch bóng lỗi (0 cảnh báo tĩnh).
3. Bấm **"Chạy Fuzzer"**: Toàn bộ các ca kiểm thử biên đều được xử lý an toàn, không bị crash hoặc timeout.

### Cách 2: Chạy Kiểm Thử Nghiệm Thu Tự Động (PyTest)
Lấy nguyên xi toàn bộ bộ test case Fuzzer sinh ra ở bản `v0` (kể cả ca kiểm thử cực biên 2GB RAM) ép chạy trực tiếp vào bản vá `v1`:
```powershell
pytest -v audit/v1/scenarios/test_verify_v0_cases_on_v1.py
```
**Kết quả mong đợi:** `10 passed (100%)`.
