# KIỂM TOÁN AN TOÀN BỘ NHỚ C CHO CHƯƠNG TRÌNH QUẢN LÝ KHO HÀNG
## HỌC PHẦN: AN TOÀN PHẦN MỀM (CSE703093) – ĐẠI HỌC PHENIKAA
### Giảng viên hướng dẫn: ThS. Vũ Quang Dũng | Nhóm thực hiện: Nhóm 04

---

## 1. THÀNH VIÊN VÀ PHÂN CÔNG KỊCH BẢN XUYÊN SUỐT

| STT | Thành viên | Mã sinh viên | Kịch bản phụ trách chính | Mô hình con | Mã CWE kiểm toán |
|:---:|:---|:---:|:---|:---:|:---|
| 1 | **Nguyễn Quang Thọ** | Nhóm 04 | **Module 1 (I/O Pipeline):** Đọc chuỗi, phân tách token, buffer stack | **M1** | CWE-120/121 (Tràn bộ đệm), CWE-125 |
| 2 | **Nguyễn Kiều Trang** | Nhóm 04 | **Module 2 (Heap Lifecycle):** Cấp phát động, realloc OOM, quản lý con trỏ | **M2** | CWE-401 (Rò rỉ bộ nhớ), CWE-416 (UAF) |
| 3 | **Hà Văn Đô** | Nhóm 04 | **Module 3 (Arithmetic):** Tính toán tồn kho, giá sản phẩm, tràn số nguyên | **M3** | CWE-190 (Integer Overflow / Wrap-around) |

---

## 2. CẤU TRÚC THƯ MỤC CHUẨN HÓA CỦA DỰ ÁN

```text
ATPM_Group_04_inventory-memory-audit/
│
├── 📄 BaoCao_AnToanPhanMem_N04_Group_04.docx   <-- Báo cáo chung của nhóm (Form chuẩn BTL_02)
├── 📄 README.md                                <-- Hướng dẫn tổng quan và chạy nghiệm thu
│
├── 📁 01_DacTa_Spec/                           <-- [CHƯƠNG 1] Bảng đặc tả yêu cầu & ràng buộc
│   ├── 📄 Spec_Inventory_Memory_Safety_Audit_18REQ_CHUAN.xlsx  (10 sheet đặc tả 18 REQ chuẩn)
│   ├── 📄 Ho_so_ke_hoach_va_kiem_chung_v0_ATPM_Chuong2.xlsx   (Hồ sơ kế hoạch kiểm chứng)
│   └── 📄 inventory_safety_specifications.json                 (File JSON đặc tả hình thức)
│
├── 📁 02_KiemChung_MoHinh/                     <-- [CHƯƠNG 1] Kiểm chứng Kripke, CTL & SMT Z3
│   ├── 📄 inventory_model.smv                  (Mô hình NuSMV kiểm chứng công thức CTL)
│   ├── 📄 verify_ctl.py                        (Script chạy kiểm chứng CTL tự động)
│   └── 📄 smt_*.smt2                           (Bộ công thức SMT Z3: v0 SAT -> v1 UNSAT)
│
├── 📁 03_KiemToan_MaNguon_C/                   <-- [CHƯƠNG 2] Mã nguồn kiểm toán C v0 vs v1
│   ├── 📁 v0_ban_loi/                          (Bản gốc C có lỗi - đối chứng phản ví dụ)
│   │   ├── 📄 audit_io_pipeline_v0.c           (M1: Thọ - lỗi tràn stack buffer)
│   │   ├── 📄 audit_heap_lifecycle_v0.c        (M2: Trang - lỗi rò rỉ RAM & UAF)
│   │   └── 📄 audit_arithmetic_v0.c            (M3: Đô - lỗi tràn số nguyên âm)
│   │
│   └── 📁 v1_ban_va/                           (Bản vá an toàn 100% - kèm defensive guards)
│       ├── 📄 audit_io_pipeline_v1.c           (M1: Thọ - chặn trước strlen >= 32)
│       ├── 📄 audit_heap_lifecycle_v1.c        (M2: Trang - con trỏ tạm & pointer nullification)
│       ├── 📄 audit_arithmetic_v1.c            (M3: Đô - chặn tràn số MAX - stock)
│       └── 📄 test_verify_v0_cases_on_v1.py    (Bộ test suite kiểm tra lại trên bản vá)
│
├── 📁 04_KetQua_MinhChung/                     <-- [BẰNG CHỨNG THỰC NGHIỆM]
│   ├── 📁 anh_minh_chung/                      (Ảnh test case v0 lỗi đỏ lòm vs v1 xanh 100%)
│   └── 📁 logs/                                (Log chạy ASan, UBSan, Z3 và NuSMV)
│
└── 📁 05_TaiLieu_LuuTru/                       <-- Lưu trữ tài liệu phát triển cũ của nhóm
```

---

## 3. CÁCH CHẠY LẠI CÁC BƯỚC NGHIỆM THU

### Bước 1: Kiểm chứng mô hình Kripke & CTL (Chương 1)
```bash
cd 02_KiemChung_MoHinh
python verify_ctl.py
```

### Bước 2: Kiểm toán mã nguồn C v0 vs v1 (Chương 2)
```bash
# Kiểm tra đối chứng các ca lỗi trên bản v1:
cd 03_KiemToan_MaNguon_C/v1_ban_va
python test_verify_v0_cases_on_v1.py
```

Kết quả: **25/25 test cases đạt 100% PASS** trên bản vá v1.