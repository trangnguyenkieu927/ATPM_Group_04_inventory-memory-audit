# Kịch Bản Kiểm Toán Bộ Nhớ v0 (Vulnerable Pre-Audit Baseline)
**Học phần:** An toàn phần mềm (CSE703153) - **GVHD:** ThS. Vũ Quang Dũng  
**Nhóm thực hiện:** Nhóm 04 (Inventory Memory Safety Audit)

Thư mục này chứa 3 module C mô phỏng các giai đoạn vòng đời bộ nhớ trong hệ thống quản lý kho, được cài cắm các lỗ hổng bộ nhớ có chủ đích để nạp vào công cụ kiểm toán (`SecLabFramework`) và các công cụ phân tích tĩnh/động (Cppcheck, ASan, Fuzzer).

---

## 1. Phân Công Thành Viên & Danh Sách Module

| STT | Module | Thành viên phụ trách | File mã nguồn | Các lỗi cài cắm (CWE) |
|---|---|---|---|---|
| 1 | **Module 1: I/O Raw Input Pipeline** | **Nguyễn Quang Thọ** | `audit_io_pipeline_v0.c` | **CWE-120/121**: Tràn bộ đệm Stack (`ingest_raw_line_v0`)<br>**CWE-125**: Đọc ngoài biên mảng (`get_token_field_v0`) |
| 2 | **Module 2: Heap Lifecycle Management** | **Nguyễn Kiều Trang** | `audit_heap_lifecycle_v0.c` | **CWE-190/787**: Tràn số dẫn tới Heap Overflow (`calc_expand_capacity_v0`)<br>**CWE-401**: Rò rỉ bộ nhớ khi realloc thất bại (`realloc_buffer_leak_v0`)<br>**CWE-416**: Use-After-Free khi dọn dẹp buffer (`cleanup_and_read_uaf_v0`) |
| 3 | **Module 3: Arithmetic & State Update** | **Hà Văn Đô** | `audit_arithmetic_v0.c` | **CWE-190**: Tràn số nguyên có dấu khi cộng dồn tồn kho (`update_stock_quantity_v0`)<br>**CWE-190**: Tràn số nguyên khi nhân quy đổi tỉ giá (`helper_scale_price`) |

---

## 2. Hướng Dẫn Nạp Vào Tool Của Thầy (SecLabFramework)

1. Khởi chạy `SecLabFramework.exe`.
2. Bấm nút **"Chọn file C"** (hoặc Open Source), điều hướng đến thư mục:
   `ATPM_Group_04_inventory-memory-audit/audit/v0/scenarios/`
3. Chọn file cần kiểm toán (ví dụ: `audit_heap_lifecycle_v0.c`).
4. Bấm **"Quét mẫu lỗi"**:
   - Tool sẽ nhận diện các hàm và quét ra các cảnh báo phân tích tĩnh (Static Pattern Scanner).
5. Bấm **"Chạy Fuzzer / Dynamic"**:
   - Tool tự động sinh ca kiểm thử biên (Boundary, Mutation, Concolic).
   - Quan sát hiện tượng crash / timeout do các lỗi bộ nhớ gây ra (ví dụ: cấp phát 2GB gây Timeout).
