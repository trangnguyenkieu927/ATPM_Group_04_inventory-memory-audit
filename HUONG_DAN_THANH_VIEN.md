# HƯỚNG DẪN DÀNH CHO THÀNH VIÊN NHÓM 04
## DỰ ÁN: KIỂM TOÁN AN TOÀN BỘ NHỚ C CHO CHƯƠNG TRÌNH QUẢN LÝ KHO HÀNG
### Học phần: An toàn phần mềm (CSE703093) | GVHD: ThS. Vũ Quang Dũng

---

## 📌 1. TẠI SAO CÓ ĐỢT TÁI CẤU TRÚC NÀY?

1. **Khắc phục lỗi "lan man sang Chương 4 (Fuzzing)" mà Thầy Dũng đã nhắc nhở trong video:**
   - Cấu trúc cũ có quá nhiều thư mục phân mảnh (`cbmc`, `fuzzing`, `cppcheck`, `coverage`), khiến Thầy nhìn vào thấy bị rối và chê lệch đề tài.
   - Thầy yêu cầu nhóm 04 tập trung đúng 2 trọng tâm: **Chương 1 (Mô hình hóa Kripke & Z3)** và **Chương 2 (Kiểm toán mã nguồn C v0 vs v1 kèm ASan)**.
2. **Chuẩn hóa cây thư mục theo đúng form mẫu của nhóm bạn Trúc (nhóm được Thầy khen và điểm cao):**
   - Gom toàn bộ dự án về **5 thư mục chuẩn học thuật**.
   - Toàn bộ code cũ, file log, shim, dll phụ trợ **không hề bị mất** mà đã được cất an toàn trong thư mục `05_TaiLieu_LuuTru/`.
   - Toàn bộ tên tệp được đổi sang **tiếng Việt thuần túy, mang tên nghiệp vụ chuẩn M1, M2, M3**, dễ hiểu và dễ trình chiếu.

---

## 📌 2. TỪNG BẠN CẦN ĐỌC GÌ ĐỂ RÀ SOÁT & KHÔNG BỊ LỆCH HƯỚNG?

Mỗi bạn chỉ cần tập trung **duy nhất vào phần việc của mình**, không cần đọc lan man sang phần khác:

---

### 👤 NGUYỄN QUANG THỌ (Module 1 - M1: I/O Pipeline & Buffer Stack)
* **Kịch bản thực tế:** Cửa ngõ tiếp nhận dữ liệu kho hàng. Đọc dòng chữ từ file hoặc bàn phím vào khay nhớ tạm Stack (`char buffer[32]`) và phân tách các cột dữ liệu.
* **Những file Thọ cần mở ra đọc ngay:**
  1. 📄 `01_DacTa_Spec/dacta_M1_doc_chuoi_buffer.json`: Đọc để hiểu các trạng thái chuyển đổi từ lúc nhận chuỗi đến lúc ghi vào buffer.
  2. 📄 `03_KiemToan_MaNguon_C/v0_ban_loi/m1_doc_chuoi_buffer_v0.c`: Xem hàm dùng `strcpy` không kiểm tra độ dài $\rightarrow$ dính lỗi **CWE-120 (Stack Buffer Overflow)** khi chuỗi dài $>32$ byte.
  3. 📄 `03_KiemToan_MaNguon_C/v1_ban_va/m1_doc_chuoi_buffer_v1.c`: Xem bản vá thêm `if (strlen(line) >= 32) return -2;` để chặn đứng lỗi.
* **Viết Báo cáo cá nhân:** Mở file `BaoCaoCaNhan_NguyenQuangTho.docx` ở thư mục gốc, điền giải thích đoạn code before/after và chèn ảnh minh chứng của M1 vào.

---

### 👤 NGUYỄN KIỀU TRANG (Module 2 - M2: Dynamic Heap Lifecycle & Pointers)
* **Kịch bản thực tế:** Quản lý vùng nhớ Heap cho danh sách sản phẩm `ProductList`. Ban đầu tạo 16 ô nhớ; khi đầy thì gọi `realloc` để nhân đôi dung lượng; khi xóa hoặc kết thúc thì gọi `free`.
* **Những file Trang cần mở ra đọc ngay:**
  1. 📄 `01_DacTa_Spec/dacta_M2_cap_phat_heap.json`: Đọc để hiểu chu kỳ cấp phát, mở rộng và giải phóng RAM.
  2. 📄 `03_KiemToan_MaNguon_C/v0_ban_loi/m2_cap_phat_heap_v0.c`: Xem lệnh `list = realloc(list, ...)`: khi OOM trả về `NULL` làm mất địa chỉ gốc gây **CWE-401 (Memory Leak)**; và sau `free` không gán `NULL` gây **CWE-416 (Use-After-Free)**.
  3. 📄 `03_KiemToan_MaNguon_C/v1_ban_va/m2_cap_phat_heap_v1.c`: Xem bản vá dùng con trỏ tạm `tmp = realloc(...)` bảo toàn vùng nhớ và gán `ptr = NULL` sau khi `free`.
* **Viết Báo cáo cá nhân:** Mở file `BaoCaoCaNhan_NguyenKieuTrang.docx` ở thư mục gốc, điền nội dung kịch bản Heap và chèn ảnh minh chứng của M2 vào.

---

### 👤 HÀ VĂN ĐÔ (Module 3 - M3: Arithmetic & Stock Operations)
* **Kịch bản thực tế:** Tính toán số học kho hàng: cộng dồn số lượng hàng mới nhập vào tồn kho cũ (`tồn kho = cũ + mới`) và nhân giá trị hàng hóa.
* **Những file Đô cần mở ra đọc ngay:**
  1. 📄 `01_DacTa_Spec/dacta_M3_tinh_ton_kho.json`: Đọc để hiểu các ràng buộc toán học không âm và giới hạn biên số nguyên.
  2. 📄 `03_KiemToan_MaNguon_C/v0_ban_loi/m3_tinh_ton_kho_v0.c`: Xem phép cộng biến `int` 32-bit: khi kho có 2 tỷ món mà cộng thêm hàng lớn vượt `INT_MAX` sẽ bị lật thành số âm $\rightarrow$ dính lỗi **CWE-190 (Integer Overflow / Negative Wraparound)**.
  3. 📄 `03_KiemToan_MaNguon_C/v1_ban_va/m3_tinh_ton_kho_v1.c`: Xem bản vá thêm rào chắn `if (added > INT_MAX - stock) return -1;` để từ chối trước phép cộng và ép kiểu `int64_t` khi tính tổng tiền.
* **Viết Báo cáo cá nhân:** Mở file `BaoCaoCaNhan_HaVanDo.docx` ở thư mục gốc, điền nội dung kịch bản số học và chèn ảnh minh chứng của M3 vào.

---

## 📌 3. HƯỚNG DẪN CÁC BƯỚC PULL CODE VÀ CHẠY THỬ TRÊN MÁY

Sau khi pull nhánh `main` mới nhất về máy, các bạn mở Terminal lên chạy 2 lệnh sau để kiểm tra:

```bash
# 1. Kéo mã nguồn mới nhất về
git checkout main
git pull origin main

# 2. Kiểm chứng mô hình Kripke & CTL (Chương 1)
python 02_KiemChung_MoHinh/chay_kiem_chung_kripke.py
# -> Kết quả: 18/18 công thức CTL đạt TRUE (100% Pass)

# 3. Kiểm toán mã nguồn C đối chứng (Chương 2)
python 03_KiemToan_MaNguon_C/chay_kiem_thu_doi_chung.py
# -> Kết quả: Toàn bộ kịch bản của Thọ, Trang, Đô đạt 100% PASSED
```

---

## 📌 4. BỐ CỤC 4 SẢN PHẨM PHẢI NỘP CHO THẦY DŨNG

1. **Báo cáo chung của nhóm:** File `BaoCao_AnToanPhanMem_N04_Group_04.docx` (đã có sẵn link GitHub của nhóm trong phần Giới thiệu).
2. **Source code C:** Thư mục `03_KiemToan_MaNguon_C/` gồm `v0_ban_loi/` (mô phỏng rủi ro) và `v1_ban_va/` (đã sửa an toàn).
3. **Biên bản test:** Bảng tổng hợp test case Before/After và ảnh log.
4. **Link GitHub dự án:** `https://github.com/trangnguyenkieu927/ATPM_Group_04_inventory-memory-audit.git`
