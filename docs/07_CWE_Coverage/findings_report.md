# BÁO CÁO TỔNG HỢP PHÁT HIỆN AN TOÀN VÀ ÁNH XẠ CWE (FINDINGS REPORT)

## 1. NGUYÊN TẮC PHÂN LOẠI PHÁT HIỆN TRUNG THỰC
Theo chỉ đạo bắt buộc tại Mục V và Mục VIII của Master Prompt, nhóm phân định nghiêm ngặt bản chất kỹ thuật của từng phát hiện:

```
+-------------------------------------------------------------------------+
|                        PHÂN HẠNG BẢN CHẤT PHÁT HIỆN                     |
+-------------------------------------------------------------------------+
| 1. CONFIRMED_BASELINE_DEFECT : Lỗi thực sự khả đạt ở runtime trên v0    |
| 2. ARITHMETIC_SAT_ONLY       : Nghiệm tràn số toán học, không khả đạt   |
| 3. GUARDED_RISK              : Rủi ro đã được code v0 phòng vệ hoàn hảo |
| 4. SEEDED_VULNERABILITY      : Lỗi cố ý tạo trên bản đối chứng          |
| 5. FALSE_POSITIVE/PORTABILITY: Cảnh báo tĩnh do đặc thù kiến trúc OS    |
+-------------------------------------------------------------------------+
```

## 2. BẢNG TỔNG KẾT CÁC PHÁT HIỆN AN TOÀN
- **Không có CONFIRMED_BASELINE_DEFECT nào gây crash hoặc exploit tức thời:** Baseline v0 đã được lập trình phòng vệ rất tốt.
- **2 Phát hiện ARITHMETIC_SAT_ONLY (Khiếm khuyết Thiết kế cần Hardening):**
  - `FIND-001` (REQ-2, CWE-190): Thiếu guard kiểm tra tràn số `capacity <= SIZE_MAX / 2` trước phép nhân đôi `capacity * 2`.
  - `FIND-002` (REQ-3, CWE-190, CWE-787): Thiếu guard kiểm tra tràn số `new_cap <= SIZE_MAX / sizeof(Product)` trước phép tính kích thước cấp phát `new_cap * 208`.
  - *Giải pháp:* Hai phát hiện này được lập kế hoạch vá tại **Công đoạn 9 (Remediation v1)**.
- **3 Phát hiện SEEDED_VULNERABILITY (Thí nghiệm đối chứng):**
  - `SEED-01`: Stack Buffer Overflow (CWE-120).
  - `SEED-02`: Use-After-Free (CWE-416).
  - `SEED-03`: Realloc Memory Leak (CWE-401).
  - Khẳng định: Các lỗi này KHÔNG tồn tại trên Baseline v0!

## 3. ÁNH XẠ CHUẨN COMMON WEAKNESS ENUMERATION (CWE)
Toàn bộ rủi ro của đề tài được quy chiếu chính xác về 7 mã CWE chuẩn quốc tế:
1. **CWE-120 / CWE-787:** Tràn bộ đệm ghi đè ngoài ranh giới (Out-of-bounds Write).
2. **CWE-125:** Đọc ngoài ranh giới (Out-of-bounds Read).
3. **CWE-190:** Tràn số nguyên (Integer Overflow or Wraparound).
4. **CWE-401:** Rò rỉ bộ nhớ (Missing Release of Memory).
5. **CWE-415:** Giải phóng đúp (Double Free).
6. **CWE-416:** Sử dụng sau giải phóng (Use After Free).
7. **CWE-476:** Giải tham chiếu con trỏ NULL (NULL Pointer Dereference).
