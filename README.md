# KIỂM TOÁN AN TOÀN BỘ NHỚ C CHO CHƯƠNG TRÌNH QUẢN LÝ KHO HÀNG
## HỌC PHẦN: AN TOÀN PHẦN MỀM (CSE703093) – ĐẠI HỌC PHENIKAA
### Giảng viên hướng dẫn: ThS. Vũ Quang Dũng | Nhóm: 04 | Lớp: N02

---

## 1. THÀNH VIÊN VÀ PHÂN CÔNG KỊCH BẢN XUYÊN SUỐT

| STT | Thành viên | Mã sinh viên | Kịch bản phụ trách chính | Mô hình con | Mã lỗi CWE kiểm toán |
|:---:|:---|:---:|:---|:---:|:---|
| 1 | **Nguyễn Quang Thọ** | 23010162 | **Module 1 (I/O Pipeline):** Đọc chuỗi thô từ file/bàn phím, phân tách token, buffer stack | **M1** | CWE-120/121 (Tràn Stack Buffer), CWE-125 |
| 2 | **Nguyễn Kiều Trang** | 23010495 | **Module 2 (Heap Lifecycle):** Cấp phát động ProductList, realloc OOM, quản lý con trỏ | **M2** | CWE-401 (Rò rỉ bộ nhớ), CWE-416 (Use-After-Free) |
| 3 | **Hà Văn Đô** | 23010406 | **Module 3 (Arithmetic):** Tính toán cộng dồn tồn kho, nhân tỉ lệ giá, tràn số nguyên | **M3** | CWE-190 (Integer Overflow / Lật số âm) |

---

## 2. CẤU TRÚC THƯ MỤC DỰ ÁN (CHUẨN NGHIỆP VỤ TIẾNG VIỆT)

```text
ATPM_Group_04_inventory-memory-audit/
│
├── 📄 BaoCao_AnToanPhanMem_N04_Group_04.docx   <-- Báo cáo chung của nhóm (mẫu [ATPM]mauchungnhomtutao.docx)
├── 📄 BaoCaoCaNhan_NguyenKieuTrang.docx        <-- Báo cáo cá nhân Trang (Module 2: Heap)
├── 📄 BaoCaoCaNhan_NguyenQuangTho.docx         <-- Báo cáo cá nhân Thọ (Module 1: I/O Buffer)
├── 📄 BaoCaoCaNhan_HaVanDo.docx                <-- Báo cáo cá nhân Đô (Module 3: Số học)
├── 📄 README.md                                <-- Hướng dẫn chạy lại và link GitHub
│
├── 📁 01_DacTa_Spec/                           <-- [CHƯƠNG 1] Bảng đặc tả yêu cầu & ràng buộc
│   ├── 📄 quanly_khohang_v0.xlsx               (Excel 10 sheet đặc tả v0 - có rủi ro)
│   ├── 📄 quanly_khohang_v1.xlsx               (Excel 10 sheet đặc tả v1 - đã vá)
│   ├── 📄 quanly_khohang_v0.json               (File JSON đặc tả v0 cho phần mềm)
│   ├── 📄 quanly_khohang_v1.json               (File JSON đặc tả v1 cho phần mềm)
│   ├── 📄 dacta_M1_doc_chuoi_buffer.json       (Đặc tả riêng M1: Đọc chuỗi, Buffer Stack)
│   ├── 📄 dacta_M2_cap_phat_heap.json          (Đặc tả riêng M2: Cấp phát Heap & Con trỏ)
│   ├── 📄 dacta_M3_tinh_ton_kho.json           (Đặc tả riêng M3: Số học Tồn kho)
│   └── 📄 ve_so_do_kripke.py                   (Script vẽ sơ đồ trạng thái Kripke)
│
├── 📁 02_KiemChung_MoHinh/                     <-- [CHƯƠNG 1] Kiểm chứng Kripke, CTL & SMT Z3
│   ├── 📄 chay_kiem_chung_kripke.py            (Script chạy kiểm chứng CTL: 18/18 TRUE)
│   ├── 📄 kiem_chung_ctl_bmc.py                (Kiểm chứng thuộc tính AG(!error) đạt CT)
│   └── 📄 kiem_tra_so_hoc_z3.py                (Z3 SMT kiểm tra tràn số: v0 SAT -> v1 UNSAT)
│
├── 📁 03_KiemToan_MaNguon_C/                   <-- [CHƯƠNG 2] Mã nguồn kiểm toán C v0 vs v1
│   ├── 📁 v0_ban_loi/                          (Bản gốc C có lỗi - mô phỏng phản ví dụ)
│   │   ├── 📄 m1_doc_chuoi_buffer_v0.c         (M1: Lỗi tràn mảng do dùng strcpy)
│   │   ├── 📄 m2_cap_phat_heap_v0.c            (M2: Lỗi realloc OOM mất con trỏ & UAF)
│   │   └── 📄 m3_tinh_ton_kho_v0.c             (M3: Lỗi cộng dồn tràn INT_MAX lật số âm)
│   │
│   ├── 📁 v1_ban_va/                           (Bản đã vá lỗi an toàn 100%)
│   │   ├── 📄 m1_doc_chuoi_buffer_v1.c         (M1: Thêm chặn trước strlen >= 32)
│   │   ├── 📄 m2_cap_phat_heap_v1.c            (M2: Con trỏ tạm tmp & gán NULL sau free)
│   │   └── 📄 m3_tinh_ton_kho_v1.c             (M3: Chặn trước tràn số MAX - stock)
│   │
│   ├── 📄 chay_asan.bat                        (Biên dịch Clang / GCC AddressSanitizer)
│   └── 📄 chay_kiem_thu_doi_chung.py           (Chạy bộ test case kiểm toán đối chứng)
│
├── 📁 04_KetQua_MinhChung/                     <-- [BẰNG CHỨNG THỰC NGHIỆM]
│   ├── 📁 anh_chay_tool_thay/                  (16 ảnh giao diện test case trên tool của Thầy)
│   ├── 📁 anh_minh_chung_asan/                 (Ảnh/log bắt lỗi AddressSanitizer)
│   ├── 📁 so_do_kripke/                        (Ảnh sơ đồ Kripke tổng và mô hình con)
│   └── 📁 logs/                                (Log chạy ASan, UBSan, NuSMV, Z3)
│
└── 📁 05_TaiLieu_LuuTru/                       <-- Lưu trữ các tệp shims, dll, tài liệu cũ
```

---

## 3. LINK REPOSITORY GITHUB
- **URL Repo:** [https://github.com/trangnguyenkieu927/ATPM_Group_04_inventory-memory-audit.git](https://github.com/trangnguyenkieu927/ATPM_Group_04_inventory-memory-audit.git)
- **Nhánh kiểm toán:** `refactor/clean-project-structure`