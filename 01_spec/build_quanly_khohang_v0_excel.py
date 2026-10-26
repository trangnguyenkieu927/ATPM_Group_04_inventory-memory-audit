import openpyxl
from openpyxl.styles import Font, PatternFill, Alignment, Border, Side
from openpyxl.utils import get_column_letter
import os
import json
import sys

sys.stdout.reconfigure(encoding='utf-8')

header_font = Font(name="Calibri", size=11, bold=True, color="000000")
header_fill = PatternFill(start_color="D9E1F2", end_color="D9E1F2", fill_type="solid")
regular_font = Font(name="Calibri", size=11)
thin_border = Border(
    left=Side(style='thin', color='D9D9D9'),
    right=Side(style='thin', color='D9D9D9'),
    top=Side(style='thin', color='D9D9D9'),
    bottom=Side(style='thin', color='D9D9D9')
)

def format_sheet(ws):
    ws.views.sheetView[0].showGridLines = True
    for col in range(1, ws.max_column + 1):
        cell = ws.cell(row=1, column=col)
        cell.font = header_font
        cell.fill = header_fill
        cell.alignment = Alignment(horizontal="center", vertical="center", wrap_text=True)
        cell.border = thin_border

    for r in range(2, ws.max_row + 1):
        for c in range(1, ws.max_column + 1):
            cell = ws.cell(row=r, column=c)
            cell.font = regular_font
            cell.border = thin_border
            cell.alignment = Alignment(vertical="center", wrap_text=True)

    for col in ws.columns:
        max_len = 0
        col_letter = get_column_letter(col[0].column)
        for cell in col:
            val = str(cell.value or '')
            lines = val.split('\n')
            for line in lines:
                if len(line) > max_len:
                    max_len = len(line)
        ws.column_dimensions[col_letter].width = min(max(max_len + 3, 12), 65)

def build_v0_excel():
    wb = openpyxl.Workbook()
    wb.remove(wb.active)

    # 1. ThongTin
    ws1 = wb.create_sheet(title="ThongTin")
    ws1.append(["Truong", "GiaTri"])
    ws1.append(["name", "quanly_khohang_v0"])
    ws1.append([
        "description",
        "Quy trình quản lý phiếu kho hàng điện máy gồm 6 hành vi của 3 thành viên: H1 nạp chuỗi mã phiếu kho vào bộ đệm Stack 32 byte (ingest_raw_line - M1 Nguyễn Quang Thọ), H2 đọc trường dữ liệu token theo chỉ số mảng (get_token_field - M1 Nguyễn Quang Thọ), H3 mở rộng động danh mục hàng hóa trên Heap khi danh sách đầy (expand_stock_items - M2 Nguyễn Kiều Trang), H4 cộng dồn số lượng tồn kho mới vào tồn kho cũ (update_stock_quantity - M3 Hà Văn Đô), H5 quy đổi đơn giá sản phẩm theo tỉ giá (scale_product_price - M3 Hà Văn Đô), H6 thu hồi tài nguyên và đóng phiên kho (release_stock_items - M2 Nguyễn Kiều Trang). Bản v0 CÀI LỖI CÓ CHỦ ĐÍCH: 18 trường hợp rủi ro an toàn bộ nhớ (CWE-120/121 Stack Overflow, CWE-193 Off-by-one, CWE-125 Out-of-bounds Read, CWE-401 Memory Leak, CWE-416 Use-After-Free, CWE-415 Double Free) và tràn số nguyên (CWE-190) để kiểm chứng tính phát hiện của mô hình Kripke, BMC và AddressSanitizer."
    ])
    format_sheet(ws1)

    # 2. YeuCauChucNang
    ws2 = wb.create_sheet(title="YeuCauChucNang")
    ws2.append(["ID", "MoTa", "DanhMuc", "MucUuTien"])
    reqs = [
        ("REQ-01", "H1: Hàm nạp chuỗi (ingest_raw_line) không được ghi vượt biên bộ đệm Stack 32 byte, kể cả khi mã dài ĐÚNG 32 ký tự (C1.2, C1.3 - M1 Thọ)", "memory", "HIGH"),
        ("REQ-02", "H1: Chuỗi mã phiếu kho hợp lệ (len <= 31 byte) phải được sao chép nguyên vẹn và kết thúc bằng ký tự '\\0' (C1.1 - M1 Thọ)", "functional", "HIGH"),
        ("REQ-03", "H2: Hàm đọc token (get_token_field) phải kiểm tra chỉ số field_idx nằm trong khoảng hợp lệ [0, token_count - 1] trước khi đọc mảng (C2.1, C2.2 - M1 Thọ)", "memory", "HIGH"),
        ("REQ-04", "H2: Con trỏ mảng token_offsets phải được kiểm tra khác NULL và token_count > 0 trước khi đọc phần tử (C2.3 - M1 Thọ)", "safety", "HIGH"),
        ("REQ-05", "H3: Quá trình mở rộng mảng danh mục hàng (expand_stock_items) bằng realloc khi gặp sự cố OOM không được ghi đè làm mất địa chỉ vùng nhớ cũ gây rò rỉ bộ nhớ (C3.1, C3.2 - M2 Trang)", "memory", "HIGH"),
        ("REQ-06", "H3: Kích thước mở rộng mảng động (new_capacity * sizeof(Product)) không được gây tràn số nguyên dẫn đến cấp phát thiếu ô nhớ trên Heap (C3.3 - M2 Trang)", "memory", "HIGH"),
        ("REQ-07", "H4: Phép cộng dồn tồn kho mới vào tồn kho cũ (update_stock_quantity) không được gây tràn số nguyên có dấu 32-bit vượt trần INT_MAX (C4.1, C4.2 - M3 Đô)", "arithmetic", "HIGH"),
        ("REQ-08", "H4: Hệ thống phải từ chối số lượng nhập hàng là số âm (added_quantity <= 0) để đảm bảo toàn vẹn dữ liệu kho (C4.3 - M3 Đô)", "safety", "HIGH"),
        ("REQ-09", "H5: Phép nhân quy đổi giá trị hàng hóa theo tỉ giá (scale_product_price) không được gây tràn số nguyên có dấu 32-bit (C5.1, C5.2 - M3 Đô)", "arithmetic", "HIGH"),
        ("REQ-10", "H6: Hai tiến trình worker nhập kho và worker xuất kho tự động dùng chung phiên kho không được đồng thời can thiệp gây tranh chấp tài nguyên (C6.1 - M2 Trang)", "concurrency", "HIGH"),
        ("REQ-11", "H6: Khi đóng phiên kho hàng, con trỏ phiên làm việc không được giải phóng hai lần (CWE-415 Double Free) (C6.2 - M2 Trang)", "memory", "HIGH"),
        ("REQ-12", "H6: Sau khi giải phóng bộ nhớ danh mục hàng hóa (release_stock_items), con trỏ phải được gán NULL ngay lập tức; cấm tuyệt đối đọc/ghi vào vùng nhớ đã giải phóng (CWE-416 Use-After-Free) (C6.3 - M2 Trang)", "memory", "HIGH")
    ]
    for r in reqs: ws2.append(list(r))
    format_sheet(ws2)

    # 3. YeuCauPhiChucNang
    ws3 = wb.create_sheet(title="YeuCauPhiChucNang")
    ws3.append(["ID", "MoTa", "MucUuTien"])
    nfrs = [
        ("NFR-01", "Hệ thống xử lý tối thiểu 300 giao dịch nạp chuỗi và đọc token/giây trên phần cứng máy chủ tham chiếu", "MEDIUM"),
        ("NFR-02", "Phản hồi thao tác kiểm tra biên mảng token trong tối đa 1 mili-giây (thao tác trực tiếp trên RAM)", "HIGH"),
        ("NFR-03", "Bộ nhớ tiến trình quản lý kho không tăng quá 5% sau 1.000.000 lượt kiểm kê liên tiếp (chống rò rỉ bộ nhớ tích lũy)", "HIGH"),
        ("NFR-04", "Không xảy ra race condition giữa luồng nhập kho và luồng quét xuất kho tự động chu kỳ 15 phút", "HIGH"),
        ("NFR-05", "Con trỏ mảng động sau khi giải phóng bằng free() phải triệt tiêu hoàn toàn, không để sót dangling pointer", "HIGH"),
        ("NFR-06", "Thời gian phản hồi thao tác mở rộng bộ nhớ danh mục hàng đạt trung bình O(1) thông qua chiến lược nhân đôi dung lượng", "MEDIUM")
    ]
    for r in nfrs: ws3.append(list(r))
    format_sheet(ws3)

    # 4. RangBuoc
    ws4 = wb.create_sheet(title="RangBuoc")
    ws4.append(["ID", "MoTa", "MucUuTien"])
    cons = [
        ("CON-01", "Chuỗi mã phiếu kho tối đa 31 ký tự ASCII (31 ký tự cần 32 byte bộ đệm Stack kể cả '\\0' - M1 Thọ)", "HIGH"),
        ("CON-02", "Mảng token_offsets có số phần tử token_count > 0; chỉ số đọc field_idx phải thỏa 0 <= field_idx < token_count (M1 Thọ)", "HIGH"),
        ("CON-03", "Danh mục hàng hóa khởi tạo ban đầu có dung lượng 16 phần tử; khi đầy nhân đôi thành 32 phần tử trên Heap (M2 Trang)", "HIGH"),
        ("CON-04", "Số lượng tồn kho, số lượng nhập thêm và đơn giá là số nguyên có dấu 32-bit (signed int, tối đa INT_MAX = 2.147.483.647 - M3 Đô)", "HIGH"),
        ("CON-05", "Worker quét xuất kho tự động kích hoạt định kỳ sau 15 phút rảnh phiên (M2 Trang)", "MEDIUM"),
        ("CON-06", "Mảng động danh mục hàng ProductList bắt buộc cấp phát trên Heap qua malloc/realloc và giải phóng qua free (M2 Trang)", "HIGH")
    ]
    for r in cons: ws4.append(list(r))
    format_sheet(ws4)

    # 5. MoHinhTrangThai (v0 - 35 trạng thái)
    ws5 = wb.create_sheet(title="MoHinhTrangThai")
    ws5.append(["TrangThai", "KhoiTao", "Nhan", "ChuyenDenCacTrangThai", "MaTruongHop", "GhiChu"])
    v0_states = [
        ("start", "x", "start, safe", "h1_vao", "-", "Bắt đầu xử lý 1 phiếu kho hàng"),
        ("h1_vao", None, "h1, receiving", "h1_ok, h1_tran_1_byte, h1_tran_bo_dem", "H1", "Nhận chuỗi mã phiếu kho vào bộ đệm stack 32 byte (M1 Thọ)"),
        ("h1_ok", None, "h1, input_ok", "h2_vao", "C1.1", "Chuỗi mã phiếu <= 31 byte chép thành công có '\\0' (an toàn)"),
        ("h1_tran_1_byte", None, "h1, stack_buffer_overflow, off_by_one, violation", "dung_khong_an_toan", "C1.2", "Chuỗi dài đúng 32 ký tự: ghi đè byte '\\0' ra ngoài bộ đệm (CWE-193 Off-by-one)"),
        ("h1_tran_bo_dem", None, "h1, stack_buffer_overflow, violation", "dung_khong_an_toan", "C1.3", "Chuỗi dài >= 33 byte: ghi đè tràn stack frame (CWE-120/121 Buffer Overflow)"),
        ("h2_vao", None, "h2, indexing", "h2_ok, h2_doc_ngoai_bien, h2_chi_so_am, h2_null_mang", "H2", "Đọc trường dữ liệu token theo chỉ số mảng (M1 Thọ)"),
        ("h2_ok", None, "h2, index_ok, read_ok", "h3_vao", "C2.1", "Chỉ số 0 <= field_idx < token_count: đọc token thành công (an toàn)"),
        ("h2_doc_ngoai_bien", None, "h2, out_of_bounds_read, violation", "dung_khong_an_toan", "C2.2", "Chỉ số field_idx >= token_count: đọc ngoài biên trên mảng (CWE-125)"),
        ("h2_chi_so_am", None, "h2, out_of_bounds_read, violation", "dung_khong_an_toan", "C2.3", "Chỉ số field_idx < 0: đọc ngược vùng nhớ trước mảng (CWE-125)"),
        ("h2_null_mang", None, "h2, null_deref, violation", "dung_khong_an_toan", "C2.4", "Mảng token_offsets == NULL hoặc token_count <= 0 (CWE-476 NULL Deref)"),
        ("h3_vao", None, "h3, allocating", "h3_ok, h3_ro_ri_oom, h3_tran_kich_thuoc", "H3", "Mở rộng mảng danh mục hàng khi danh sách đầy bằng realloc (M2 Trang)"),
        ("h3_ok", None, "h3, realloc_ok", "h4_vao", "C3.1", "Tái cấp phát thành công, nhân đôi dung lượng an toàn"),
        ("h3_ro_ri_oom", None, "h3, mem_leak, violation", "dung_khong_an_toan", "C3.2", "realloc trả về NULL, con trỏ gốc bị ghi đè gây mất mảng cũ (CWE-401 Memory Leak)"),
        ("h3_tran_kich_thuoc", None, "h3, alloc_size_overflow, violation", "dung_khong_an_toan", "C3.3", "new_capacity * sizeof(Product) tràn int32 dẫn tới cấp phát thiếu Heap"),
        ("h4_vao", None, "h4, computing", "h4_ok, h4_tran_so_am, h4_so_luong_am", "H4", "Cộng dồn số lượng tồn kho mới vào tồn kho cũ (M3 Đô)"),
        ("h4_ok", None, "h4, sum_ok", "h5_vao", "C4.1", "current_stock + added_quantity <= INT_MAX: cộng dồn an toàn"),
        ("h4_tran_so_am", None, "h4, int_overflow, violation", "dung_khong_an_toan", "C4.2", "current_stock + added_quantity vượt 2^31 - 1: tràn số nguyên lật âm (CWE-190)"),
        ("h4_so_luong_am", None, "h4, negative_amount, violation", "dung_khong_an_toan", "C4.3", "added_quantity < 0: số lượng nhập âm làm sai lệch kho hàng"),
        ("h5_vao", None, "h5, scaling", "h5_ok, h5_nhan_tran_int", "H5", "Quy đổi đơn giá sản phẩm theo tỉ giá (M3 Đô)"),
        ("h5_ok", None, "h5, scale_ok", "h6_phien_ranh", "C5.1", "base_price * rate <= INT_MAX: nhân tỉ giá an toàn"),
        ("h5_nhan_tran_int", None, "h5, int_overflow, violation", "dung_khong_an_toan", "C5.2", "base_price * rate vượt INT_MAX: tràn phép nhân số nguyên (CWE-190)"),
        ("h6_phien_ranh", None, "h6, sess_free", "h6_nhap_giu, h6_xuat_giu", "H6", "Phiên kho rảnh, chờ worker kết thúc phiên (M2 Trang)"),
        ("h6_nhap_giu", None, "h6, nhap_locked", "h6_giai_phong, h6_tranh_chap", "C6.1", "Worker nhập kho giữ khóa phiên làm việc"),
        ("h6_xuat_giu", None, "h6, xuat_locked", "h6_giai_phong, h6_tranh_chap", "C6.1", "Worker xuất kho tự động giữ khóa phiên"),
        ("h6_tranh_chap", None, "h6, nhap_locked, xuat_locked, race, violation", "h6_double_free, h6_uaf", "C6.2", "Cả 2 worker cùng giữ phiên (Race condition vi phạm loại trừ lẫn nhau)"),
        ("h6_double_free", None, "h6, double_free, violation", "dung_khong_an_toan", "C6.3", "Cả 2 worker cùng gọi free() trên cùng con trỏ phiên (CWE-415)"),
        ("h6_uaf", None, "h6, use_after_free, violation", "dung_khong_an_toan", "C6.4", "Worker đến sau đọc/ghi vào vùng nhớ phiên đã bị free (CWE-416)"),
        ("h6_giai_phong", None, "h6, releasing", "ket_thuc_ok, h6_uaf_dangling", "C6.1", "Giải phóng tài nguyên danh mục hàng hóa"),
        ("h6_uaf_dangling", None, "h6, use_after_free, violation", "dung_khong_an_toan", "C6.5", "Sau free(items) không gán NULL, đọc lại items[0] gây Use-After-Free (CWE-416)"),
        ("ket_thuc_ok", None, "flow_end, stock_done, mem_clean, safe", "ket_thuc_ok", "C6.1", "Phiên kho kết thúc thành công, giải phóng toàn bộ bộ nhớ (trạng thái an toàn)"),
        ("ket_thuc_tu_choi", None, "flow_end, input_rejected, mem_clean, safe", "ket_thuc_tu_choi", "-", "Từ chối dữ liệu đầu vào không hợp lệ có kiểm soát (v0 thiếu nhánh này)"),
        ("dung_khong_an_toan", None, "flow_end, unsafe_halt, violation", "dung_khong_an_toan", "-", "Tiến trình crash (SIGSEGV/ABORT) do vi phạm an toàn bộ nhớ")
    ]
    for r in v0_states: ws5.append(list(r))
    format_sheet(ws5)

    # 6. ThuocTinhAnToan (22 công thức CTL)
    ws6 = wb.create_sheet(title="ThuocTinhAnToan")
    ws6.append(["CongThucCTL", "GhiChu"])
    ctls = [
        ("AG(!violation)", "Safety tổng: không bao giờ đạt trạng thái vi phạm nào"),
        ("AG(!stack_buffer_overflow)", "Safety H1 — CWE-120/121 (tràn stack bộ đệm mã phiếu 32 byte - M1 Thọ)"),
        ("AG(!off_by_one)", "Safety H1 — CWE-193 (mã phiếu dài đúng 32 ký tự ghi đè byte null - M1 Thọ)"),
        ("AG(!out_of_bounds_read)", "Safety H2 — CWE-125 (đọc token ngoài biên mảng hoặc chỉ số âm - M1 Thọ)"),
        ("AG(!null_deref)", "Safety H2 — CWE-476 (đọc mảng token khi con trỏ NULL - M1 Thọ)"),
        ("AG(!alloc_size_overflow)", "Safety H3 — CWE-190/787 (kích thước mở rộng mảng n * 48 byte tràn int32 - M2 Trang)"),
        ("AG(!mem_leak)", "Safety H3 — CWE-401 (realloc mở rộng mảng khi OOM làm mất con trỏ cũ - M2 Trang)"),
        ("AG(!int_overflow)", "Safety H4, H5 — CWE-190 (cộng dồn tồn kho vượt 2 tỷ hoặc nhân tỉ giá tràn int32 - M3 Đô)"),
        ("AG(!negative_amount)", "Safety H4 — (số lượng nhập hàng là số âm làm sai lệch số liệu kho - M3 Đô)"),
        ("AG(!race)", "Safety H6 — CWE-667 (worker nhập kho và worker xuất kho tự động cùng chiếm phiên - M2 Trang)"),
        ("AG(!double_free)", "Safety H6 — CWE-415 (giải phóng phiên kho 2 lần liên tiếp - M2 Trang)"),
        ("AG(!use_after_free)", "Safety H6 — CWE-416 (đọc dữ liệu con trỏ treo sau khi free - M2 Trang)"),
        ("AF(flow_end)", "Liveness: mọi nhánh xử lý đều kết thúc có kiểm soát"),
        ("EF(stock_done)", "Khả đạt: tồn tại đường đi hoàn thành phiên kho an toàn thành công"),
        ("EF(input_rejected)", "Khả đạt: tồn tại nhánh từ chối đầu vào độc hại/sai cú pháp (v0 KHÔNG có -> VI PHẠM)"),
        ("EG(!violation)", "Tồn tại ít nhất một đường đi luôn an toàn từ đầu đến cuối"),
        ("EF(nhap_locked)", "Khả đạt của cụm khóa phiên nhập kho (mô hình khóa phiên hoạt động được)"),
        ("EF(xuat_locked)", "Khả đạt của cụm khóa phiên xuất kho tự động (mô hình khóa phiên hoạt động được)")
    ]
    for r in ctls: ws6.append(list(r))
    format_sheet(ws6)

    # 7. LoaiTruLanNhau
    ws7 = wb.create_sheet(title="LoaiTruLanNhau")
    ws7.append(["MenhDe1", "MenhDe2"])
    mutex = [
        ("nhap_locked", "xuat_locked"),
        ("violation", "flow_end"),
        ("stock_done", "input_rejected"),
        ("unsafe_halt", "mem_clean"),
        ("use_after_free", "mem_clean")
    ]
    for r in mutex: ws7.append(list(r))
    format_sheet(ws7)

    # 8. HamSinhMa
    ws8 = wb.create_sheet(title="HamSinhMa")
    ws8.append(["Ten", "NgonNgu", "Loai", "ThamSo", "KichThuocBuffer", "Loi", "GiaTriBiMat1", "GiaTriBiMat2", "GhiChu"])
    funcs = [
        ("ingest_raw_line", "c", "buffer_copy", "dst, cap, raw_line", 32, "buffer-overflow", None, None, "H1 — Nạp chuỗi vào bộ đệm stack 32 byte (M1 Thọ)"),
        ("get_token_field", "c", "array_read", "token_offsets, token_count, field_idx", None, "out-of-bounds-read", None, None, "H2 — Đọc trường token theo chỉ số mảng (M1 Thọ)"),
        ("expand_stock_items", "c", "alloc_release", "list, new_capacity", 16, "memory-leak", None, None, "H3 — realloc mở rộng mảng khi OOM gây rò rỉ bộ nhớ (M2 Trang)"),
        ("release_stock_items", "c", "alloc_release", "list", 16, "use-after-free", None, None, "H6 — free() không gán NULL dẫn tới Use-After-Free (M2 Trang)"),
        ("update_stock_quantity", "c", "arithmetic", "current_stock, added_quantity", None, "integer-overflow", None, None, "H4 — Cộng dồn tồn kho tràn signed int 32-bit (M3 Đô)"),
        ("scale_product_price", "c", "arithmetic", "base_price, rate", None, "integer-overflow", None, None, "H5 — Nhân tỉ giá quy đổi tràn int32 (M3 Đô)"),
        ("calc_batch_capacity", "python", "divide", "total_items, batch_size", None, "divide-by-zero", None, None, "H3 — Chia cho 0 khi batch_size = 0 (black-box fuzz)")
    ]
    for r in funcs: ws8.append(list(r))
    format_sheet(ws8)

    # 9. KiemTraSoHoc
    ws9 = wb.create_sheet(title="KiemTraSoHoc")
    ws9.append(["Ten", "BienSoNguyen", "DieuKien", "BieuThucTran", "GhiChu"])
    ariths = [
        ("stock_addition_overflow", "current_stock, added_quantity", "current_stock >= 0 and current_stock <= 2147483647 and added_quantity >= 1 and added_quantity <= 2147483647 and current_stock + added_quantity > 2147483647", "current_stock + added_quantity", "H4 — phép cộng dồn tồn kho mới vào tồn kho cũ vượt trần INT_MAX (M3 Đô)"),
        ("heap_alloc_bytes", "new_capacity, item_size", "new_capacity >= 1 and item_size == 48 and new_capacity * item_size > 2147483647", "new_capacity * item_size", "H3 — kích thước tái cấp phát Heap vượt 2GB khi new_capacity lớn (M2 Trang)"),
        ("price_multiplication_overflow", "base_price, rate", "base_price > 0 and rate > 1 and base_price * rate > 2147483647", "base_price * rate", "H5 — phép nhân đơn giá x tỉ giá quy đổi tràn int32 (M3 Đô)"),
        ("negative_quantity_check", "added_quantity", "added_quantity < 0", "added_quantity", "H4 — số lượng nhập hàng là số âm làm sai lệch số liệu kho (M3 Đô)")
    ]
    for r in ariths: ws9.append(list(r))
    format_sheet(ws9)

    # 10. TestCase
    ws10 = wb.create_sheet(title="TestCase")
    ws10.append(["ID", "YeuCauLienQuan", "TieuDe", "DieuKienTienQuyet", "CacBuoc", "KetQuaMongDoi", "MucUuTien"])
    tests = [
        ("TC-01", "REQ-01, CON-01", "H1: Nạp chuỗi đúng 32 byte gây lỗi Off-by-one và 33 byte tràn stack", "ingest_raw_line sinh từ đặc tả (v0: buffer 32 byte, Loi=buffer-overflow) - M1 Thọ", "Gọi ingest_raw_line_v0 với chuỗi 32 và 33 ký tự\nChạy kiểm tra AG(!stack_buffer_overflow), AG(!off_by_one)", "v0: ghi đè byte '\\0' và tràn stack frame (CWE-120/121, CWE-193); v1: rào chắn từ chối trả về -2", "HIGH"),
        ("TC-02", "REQ-02, CON-01", "H1: Nạp chuỗi hợp lệ 31 byte vào bộ đệm stack 32 byte", "char buffer[32], cap = 32, chuỗi đầu vào dài đúng 31 ký tự - M1 Thọ", "Gọi v0 và v1 với chuỗi 31 byte\nKiểm tra giá trị trả về và nội dung bộ đệm", "Cả 2 phiên bản trả về 31, sao chép thành công có '\\0'", "HIGH"),
        ("TC-03", "REQ-03, CON-02", "H2: Đọc token tại chỉ số chạm biên trên field_idx == token_count", "token_offsets có 4 phần tử (chỉ số 0..3), field_idx = 4 - M1 Thọ", "Gọi get_token_field_v0 với field_idx = 4 dưới giám sát ASan\nGọi get_token_field_v1 với cùng tham số", "v0: Đọc ngoài biên mảng token (CWE-125); v1: Rào chắn chặn trả về -2", "HIGH"),
        ("TC-04", "REQ-04, CON-02", "H2: Đọc token với chỉ số âm field_idx < 0", "token_count = 4, field_idx = -1 - M1 Thọ", "Gọi get_token_field_v0 với field_idx = -1\nGọi đối chứng get_token_field_v1", "v0: Đọc lùi vùng nhớ trước mảng (CWE-125); v1: Từ chối an toàn trả về -2", "HIGH"),
        ("TC-05", "REQ-05, CON-03", "H3: Mô phỏng sự cố OOM khi realloc mảng danh mục hàng", "realloc trả về NULL khi hệ thống cạn kiệt RAM - M2 Trang", "Gọi expand_stock_items trên bản v0 và v1 khi mảng đầy cần nhân đôi\nQuan sát con trỏ list", "v0: list bị gán NULL làm mất địa chỉ mảng cũ (CWE-401 Leak); v1: giữ nguyên con trỏ cũ qua biến tạm tmp", "HIGH"),
        ("TC-06", "REQ-06, CON-03", "H3: Tràn số nguyên khi tính toán kích thước mở rộng mảng động", "new_capacity lớn gây tràn phép nhân new_capacity * sizeof(Product) - M2 Trang", "Chạy kiểm tra SMT Z3 trên điều kiện heap_alloc_bytes\nThực thi hàm mở rộng", "v0: SAT (tìm được nghiệm tràn int32 cấp phát thiếu ô nhớ); v1: UNSAT", "HIGH"),
        ("TC-07", "REQ-07, CON-04", "H4: Cộng dồn số lượng tồn kho mới vượt trần INT_MAX", "current_stock = 2.147.483.640, added_quantity = 20 - M3 Đô", "Gọi update_stock_quantity trên bản v0 và v1", "v0: 2147483640 + 20 = -2147483636 (tràn số âm CWE-190); v1: từ chối trước khi cộng", "HIGH"),
        ("TC-08", "REQ-08, CON-04", "H4: Số lượng nhập hàng là số âm (added_quantity < 0)", "added_quantity = -50 - M3 Đô", "Gọi update_stock_quantity với số lượng âm", "v0: trừ lùi tồn kho sai nghiệp vụ; v1: phát hiện số âm từ chối an toàn", "HIGH"),
        ("TC-09", "REQ-09, CON-04", "H5: Quy đổi đơn giá sản phẩm theo tỉ giá gây tràn phép nhân", "base_price = 50.000.000, rate = 100 - M3 Đô", "Gọi scale_product_price trên bản v0 và v1", "v0: 50.000.000 * 100 vượt INT_MAX gây tràn số (CWE-190); v1: rào chắn từ chối an toàn", "HIGH"),
        ("TC-10", "REQ-10, CON-05", "H6: Tranh chấp khóa phiên giữa worker nhập kho và xuất kho", "Cả 2 worker cùng giữ phiên làm việc tại h6_tranh_chap - M2 Trang", "Chạy check_mutual_exclusion(model, 'nhap_locked', 'xuat_locked')", "v0: vi phạm loại trừ lẫn nhau tại h6_tranh_chap (CWE-667); v1: đồng bộ mutex đạt chuẩn", "HIGH"),
        ("TC-11", "REQ-11, REQ-12", "H6: Double-free và Use-after-free khi giải phóng phiên kho", "release_stock_items và close_inventory_session - M2 Trang", "Chạy kiểm toán AddressSanitizer trên bản v0 và v1", "v0: bắt lỗi use-after-free (CWE-416) và double-free (CWE-415); v1: 0 crash ASan", "HIGH"),
        ("TC-12", "REQ-01 đến REQ-12", "Kiểm chứng toàn luồng quản lý kho hàng kết hợp", "Mô hình kết hợp tổng thể 35 trạng thái", "Chạy chay_kiem_chung_kripke.py kiểm tra deadlock và 22 công thức CTL", "Không deadlock; v0: phát hiện lỗi tại các trạng thái vi phạm; v1: 100% thỏa mãn thuộc tính", "HIGH")
    ]
    for r in tests: ws10.append(list(r))
    format_sheet(ws10)

    p0 = os.path.abspath("01_spec/quanly_khohang_v0.xlsx")
    wb.save(p0)
    print("Saved v0 Excel to:", p0)

def build_v1_excel():
    wb = openpyxl.Workbook()
    wb.remove(wb.active)

    # 1. ThongTin
    ws1 = wb.create_sheet(title="ThongTin")
    ws1.append(["Truong", "GiaTri"])
    ws1.append(["name", "quanly_khohang_v1"])
    ws1.append([
        "description",
        "Quy trình quản lý phiếu kho hàng điện máy BẢN VÁ AN TOÀN PHÒNG THỦ (v1): H1 kiểm soát biên buffer 32 byte (chặn len >= 32 trước khi memcpy - M1 Thọ), H2 kiểm tra chỉ số 0 <= field_idx < token_count trước khi đọc mảng (M1 Thọ), H3 dùng con trỏ tạm tmp kiểm tra NULL bảo toàn vùng nhớ khi OOM và nhân đôi dung lượng an toàn (M2 Trang), H4 cài defensive guard kiểm tra added > INT_MAX - stock trước khi cộng và từ chối số âm (M3 Đô), H5 kiểm tra tràn phép nhân đơn giá với tỉ giá (M3 Đô), H6 đồng bộ khóa mutex ngăn race condition và gán list = NULL ngay sau free (M2 Trang). 100% thỏa mãn thuộc tính an toàn CTL và Z3 SMT UNSAT."
    ])
    format_sheet(ws1)

    # 2. YeuCauChucNang
    ws2 = wb.create_sheet(title="YeuCauChucNang")
    ws2.append(["ID", "MoTa", "DanhMuc", "MucUuTien"])
    reqs = [
        ("REQ-01", "H1: Hàm nạp chuỗi (ingest_raw_line) kiểm tra con trỏ, cap == 32 và từ chối chuỗi len >= 32 (M1 Thọ)", "memory", "HIGH"),
        ("REQ-02", "H1: Chuỗi mã phiếu hợp lệ (len <= 31 byte) chép an toàn bằng memcpy và luôn gán '\\0' tại cuối buffer (M1 Thọ)", "functional", "HIGH"),
        ("REQ-03", "H2: Hàm đọc token (get_token_field) từ chối con trỏ NULL và kiểm tra nghiêm ngặt 0 <= field_idx < token_count (M1 Thọ)", "memory", "HIGH"),
        ("REQ-04", "H2: Đảm bảo không đọc ngoài biên mảng token trong mọi kịch bản thực thi (M1 Thọ)", "safety", "HIGH"),
        ("REQ-05", "H3: Sử dụng con trỏ tạm tmp khi gọi realloc; nếu OOM trả về STATUS_ERR_ALLOC và giữ nguyên vùng nhớ cũ (M2 Trang)", "memory", "HIGH"),
        ("REQ-06", "H3: Kiểm tra cận trên new_capacity <= MAX_SAFE_CAPACITY trước khi tái cấp phát (M2 Trang)", "memory", "HIGH"),
        ("REQ-07", "H4: Cài đặt rào chắn defensive guard if (INT_MAX - stock < added) return ERR_OVERFLOW (M3 Đô)", "arithmetic", "HIGH"),
        ("REQ-08", "H4: Từ chối số lượng nhập hàng added_quantity <= 0 trước khi cập nhật tồn kho (M3 Đô)", "safety", "HIGH"),
        ("REQ-09", "H5: Kiểm tra tràn phép nhân if (INT_MAX / rate < base_price) return ERR_OVERFLOW (M3 Đô)", "arithmetic", "HIGH"),
        ("REQ-10", "H6: Đồng bộ hóa phiên làm việc qua mutex ngăn chặn triệt để tranh chấp giữa 2 worker (M2 Trang)", "concurrency", "HIGH"),
        ("REQ-11", "H6: Cơ chế phân quyền đảm bảo phiên làm việc chỉ được giải phóng đúng 1 lần (M2 Trang)", "memory", "HIGH"),
        ("REQ-12", "H6: Gán list = NULL ngay lập tức sau lệnh free(list) triệt tiêu hoàn toàn dangling pointer (M2 Trang)", "memory", "HIGH")
    ]
    for r in reqs: ws2.append(list(r))
    format_sheet(ws2)

    # 3. YeuCauPhiChucNang
    ws3 = wb.create_sheet(title="YeuCauPhiChucNang")
    ws3.append(["ID", "MoTa", "MucUuTien"])
    nfrs = [
        ("NFR-01", "Hệ thống xử lý an toàn tối thiểu 300 giao dịch nạp chuỗi và đọc token/giây", "MEDIUM"),
        ("NFR-02", "Rào chắn kiểm tra biên mảng phản hồi trong dưới 1 mili-giây", "HIGH"),
        ("NFR-03", "Bộ nhớ tiến trình ổn định 100%, không rò rỉ (LeakSanitizer báo 0 byte leaked)", "HIGH"),
        ("NFR-04", "0 lỗi race condition được xác nhận bởi ThreadSanitizer", "HIGH"),
        ("NFR-05", "Con trỏ mảng động sau free() được gán NULL ngay lập tức", "HIGH"),
        ("NFR-06", "Hiệu năng mở rộng mảng đạt O(1) trung bình", "MEDIUM")
    ]
    for r in nfrs: ws3.append(list(r))
    format_sheet(ws3)

    # 4. RangBuoc
    ws4 = wb.create_sheet(title="RangBuoc")
    ws4.append(["ID", "MoTa", "MucUuTien"])
    cons = [
        ("CON-01", "Chuỗi mã phiếu kho tối đa 31 ký tự ASCII (dung lượng định mức 32 byte kể cả '\\0' - M1 Thọ)", "HIGH"),
        ("CON-02", "Chỉ số đọc field_idx phải thỏa 0 <= field_idx < token_count (M1 Thọ)", "HIGH"),
        ("CON-03", "Danh mục hàng hóa mở rộng tối đa 10.000 phần tử (M2 Trang)", "HIGH"),
        ("CON-04", "Số lượng tồn kho và đơn giá nằm trong phạm vi signed int 32-bit dương (M3 Đô)", "HIGH"),
        ("CON-05", "Worker xuất kho tự động đồng bộ qua mutex (M2 Trang)", "MEDIUM"),
        ("CON-06", "Mảng động cấp phát trên Heap bảo toàn quyền sở hữu khi OOM (M2 Trang)", "HIGH")
    ]
    for r in cons: ws4.append(list(r))
    format_sheet(ws4)

    # 5. MoHinhTrangThai (v1 - 21 trạng thái)
    ws5 = wb.create_sheet(title="MoHinhTrangThai")
    ws5.append(["TrangThai", "KhoiTao", "Nhan", "ChuyenDenCacTrangThai", "MaTruongHop", "GhiChu"])
    v1_states = [
        ("start", "x", "start, safe", "h1_vao", "-", "Bắt đầu xử lý phiếu kho"),
        ("h1_vao", None, "h1, receiving", "h1_kiem_tra_bien", "H1", "Nhận chuỗi mã phiếu kho vào bộ đệm stack (M1 Thọ)"),
        ("h1_kiem_tra_bien", None, "h1, checking_bounds", "h1_ok, h1_tu_choi", "H1", "Kiểm tra độ dài len < 32"),
        ("h1_ok", None, "h1, copy_safe, input_ok", "h2_vao", "C1.1", "Sao chép an toàn bằng memcpy và gán NUL"),
        ("h1_tu_choi", None, "h1, input_rejected", "ket_thuc_tu_choi", "C1.2", "Từ chối chuỗi len >= 32 trước khi sao chép"),
        ("h2_vao", None, "h2, indexing", "h2_kiem_tra_chi_so", "H2", "Nhận chỉ số đọc trường token (M1 Thọ)"),
        ("h2_kiem_tra_chi_so", None, "h2, checking_index", "h2_ok, h2_tu_choi", "H2", "Kiểm tra 0 <= field_idx < token_count"),
        ("h2_ok", None, "h2, read_safe, index_ok", "h3_vao", "C2.1", "Đọc token thành công trong phạm vi mảng"),
        ("h2_tu_choi", None, "h2, index_rejected", "ket_thuc_tu_choi", "C2.2", "Từ chối chỉ số ngoài biên hoặc mảng NULL"),
        ("h3_vao", None, "h3, allocating", "h3_kiem_tra_heap", "H3", "Mở rộng mảng động danh mục hàng (M2 Trang)"),
        ("h3_kiem_tra_heap", None, "h3, realloc_temp", "h3_ok, h3_tu_choi", "H3", "Dùng con trỏ tạm tmp kiểm tra kết quả realloc"),
        ("h3_ok", None, "h3, realloc_ok, owner_valid", "h4_vao", "C3.1", "Mở rộng thành công, cập nhật con trỏ chính"),
        ("h3_tu_choi", None, "h3, owner_preserved", "ket_thuc_tu_choi", "C3.2", "OOM: bảo toàn mảng cũ, từ chối an toàn"),
        ("h4_vao", None, "h4, computing", "h4_kiem_tra_rao_chan", "H4", "Cộng dồn tồn kho mới (M3 Đô)"),
        ("h4_kiem_tra_rao_chan", None, "h4, guard_checking", "h4_ok, h4_tu_choi", "H4", "Kiểm tra added > 0 và added <= INT_MAX - stock"),
        ("h4_ok", None, "h4, sum_safe", "h5_vao", "C4.1", "Cộng dồn tồn kho an toàn"),
        ("h4_tu_choi", None, "h4, overflow_prevented, negative_rejected", "ket_thuc_tu_choi", "C4.2", "Từ chối số âm hoặc nguy cơ tràn số"),
        ("h5_vao", None, "h5, scaling", "h5_kiem_tra_nhan", "H5", "Quy đổi đơn giá theo tỉ giá (M3 Đô)"),
        ("h5_kiem_tra_nhan", None, "h5, mul_guard_checking", "h5_ok, h5_tu_choi", "H5", "Kiểm tra base_price <= INT_MAX / rate"),
        ("h5_ok", None, "h5, scale_safe", "h6_phien_ranh", "C5.1", "Nhân tỉ giá an toàn"),
        ("h5_tu_choi", None, "h5, mul_overflow_prevented", "ket_thuc_tu_choi", "C5.2", "Từ chối tỉ giá gây tràn số"),
        ("h6_phien_ranh", None, "h6, sess_free", "h6_nhap_giu, h6_xuat_giu", "H6", "Phiên kho rảnh (M2 Trang)"),
        ("h6_nhap_giu", None, "h6, nhap_locked", "h6_giai_phong", "C6.1", "Worker nhập kho giữ khóa mutex an toàn"),
        ("h6_xuat_giu", None, "h6, xuat_locked", "h6_giai_phong", "C6.1", "Worker xuất kho tự động giữ khóa mutex"),
        ("h6_giai_phong", None, "h6, pointer_nullified, zero_leak", "ket_thuc_ok", "C6.1", "Giải phóng tài nguyên và gán list = NULL"),
        ("ket_thuc_ok", None, "flow_end, stock_done, mem_clean, safe", "ket_thuc_ok", "C6.1", "Hoàn tất phiên an toàn"),
        ("ket_thuc_tu_choi", None, "flow_end, input_rejected, mem_clean, safe", "ket_thuc_tu_choi", "C1.2", "Từ chối an toàn có kiểm soát")
    ]
    for r in v1_states: ws5.append(list(r))
    format_sheet(ws5)

    # 6. ThuocTinhAnToan (22 công thức CTL)
    ws6 = wb.create_sheet(title="ThuocTinhAnToan")
    ws6.append(["CongThucCTL", "GhiChu"])
    ctls = [
        ("AG(!violation)", "Safety tổng: không bao giờ đạt trạng thái vi phạm nào (v1 THỎA MÃN)"),
        ("AG(!stack_buffer_overflow)", "Safety H1: không có vết tràn stack (v1 THỎA MÃN)"),
        ("AG(!off_by_one)", "Safety H1: không có lỗi ghi đè null terminator (v1 THỎA MÃN)"),
        ("AG(!out_of_bounds_read)", "Safety H2: không có lỗi đọc ngoài biên token (v1 THỎA MÃN)"),
        ("AG(!null_deref)", "Safety H2: không dereference con trỏ NULL (v1 THỎA MÃN)"),
        ("AG(!alloc_size_overflow)", "Safety H3: không tràn kích thước cấp phát Heap (v1 THỎA MÃN)"),
        ("AG(!mem_leak)", "Safety H3: không rò rỉ bộ nhớ khi OOM (v1 THỎA MÃN)"),
        ("AG(!int_overflow)", "Safety H4, H5: không tràn số nguyên (v1 THỎA MÃN)"),
        ("AG(!negative_amount)", "Safety H4: từ chối số lượng nhập âm (v1 THỎA MÃN)"),
        ("AG(!race)", "Safety H6: mutex đồng bộ hoàn toàn (v1 THỎA MÃN)"),
        ("AG(!double_free)", "Safety H6: không giải phóng đúp (v1 THỎA MÃN)"),
        ("AG(!use_after_free)", "Safety H6: gán NULL sau free triệt tiêu UAF (v1 THỎA MÃN)"),
        ("AF(flow_end)", "Liveness: mọi luồng đều kết thúc có kiểm soát (v1 THỎA MÃN)"),
        ("EF(stock_done)", "Khả đạt: tồn tại đường đi hoàn tất phiên an toàn (v1 THỎA MÃN)"),
        ("EF(input_rejected)", "Khả đạt: tồn tại nhánh từ chối an toàn có kiểm soát (v1 THỎA MÃN)"),
        ("EG(!violation)", "Tồn tại đường đi an toàn xuyên suốt chu trình (v1 THỎA MÃN)"),
        ("EF(nhap_locked)", "Cụm khóa phiên nhập kho hoạt động được"),
        ("EF(xuat_locked)", "Cụm khóa phiên xuất kho tự động hoạt động được")
    ]
    for r in ctls: ws6.append(list(r))
    format_sheet(ws6)

    # 7. LoaiTruLanNhau
    ws7 = wb.create_sheet(title="LoaiTruLanNhau")
    ws7.append(["MenhDe1", "MenhDe2"])
    mutex = [
        ("nhap_locked", "xuat_locked"),
        ("violation", "flow_end"),
        ("stock_done", "input_rejected"),
        ("unsafe_halt", "mem_clean")
    ]
    for r in mutex: ws7.append(list(r))
    format_sheet(ws7)

    # 8. HamSinhMa
    ws8 = wb.create_sheet(title="HamSinhMa")
    ws8.append(["Ten", "NgonNgu", "Loai", "ThamSo", "KichThuocBuffer", "Loi", "GiaTriBiMat1", "GiaTriBiMat2", "GhiChu"])
    funcs = [
        ("ingest_raw_line", "c", "buffer_copy", "dst, cap, raw_line", 32, None, None, None, "H1 — Nạp chuỗi an toàn phòng thủ (M1 Thọ)"),
        ("get_token_field", "c", "array_read", "token_offsets, token_count, field_idx", None, None, None, None, "H2 — Đọc token an toàn phòng thủ (M1 Thọ)"),
        ("expand_stock_items", "c", "alloc_release", "list, new_capacity", 16, None, None, None, "H3 — realloc an toàn bảo toàn vùng nhớ qua tmp (M2 Trang)"),
        ("release_stock_items", "c", "alloc_release", "list", 16, None, None, None, "H6 — free() và gán con trỏ = NULL (M2 Trang)"),
        ("update_stock_quantity", "c", "arithmetic", "current_stock, added_quantity", None, None, None, None, "H4 — Cộng tồn kho an toàn phòng thủ (M3 Đô)"),
        ("scale_product_price", "c", "arithmetic", "base_price, rate", None, None, None, None, "H5 — Nhân tỉ giá an toàn phòng thủ (M3 Đô)"),
        ("calc_batch_capacity", "python", "divide", "total_items, batch_size", None, None, None, None, "H3 — Kiểm tra batch_size > 0 trước khi chia")
    ]
    for r in funcs: ws8.append(list(r))
    format_sheet(ws8)

    # 9. KiemTraSoHoc
    ws9 = wb.create_sheet(title="KiemTraSoHoc")
    ws9.append(["Ten", "BienSoNguyen", "DieuKien", "BieuThucTran", "GhiChu"])
    ariths = [
        ("stock_addition_overflow", "current_stock, added_quantity", "current_stock >= 0 and added_quantity >= 1 and current_stock + added_quantity > 2147483647", "current_stock + added_quantity", "H4 — rào chắn ngăn chặn hoàn toàn (Z3 UNSAT trên v1 - M3 Đô)"),
        ("heap_alloc_bytes", "new_capacity, item_size", "new_capacity >= 1 and item_size == 48 and new_capacity * item_size > 2147483647", "new_capacity * item_size", "H3 — rào chắn giới hạn MAX_SAFE_CAPACITY (Z3 UNSAT - M2 Trang)"),
        ("price_multiplication_overflow", "base_price, rate", "base_price > 0 and rate > 1 and base_price * rate > 2147483647", "base_price * rate", "H5 — rào chắn kiểm tra trần nhân (Z3 UNSAT - M3 Đô)"),
        ("negative_quantity_check", "added_quantity", "added_quantity < 0", "added_quantity", "H4 — từ chối số âm trước khi xử lý (Z3 UNSAT - M3 Đô)")
    ]
    for r in ariths: ws9.append(list(r))
    format_sheet(ws9)

    # 10. TestCase
    ws10 = wb.create_sheet(title="TestCase")
    ws10.append(["ID", "YeuCauLienQuan", "TieuDe", "DieuKienTienQuyet", "CacBuoc", "KetQuaMongDoi", "MucUuTien"])
    tests = [
        ("TC-01", "REQ-01, CON-01", "H1: Nạp chuỗi 32 và 33 byte bị từ chối an toàn", "ingest_raw_line_v1 có rào chắn kiểm tra len < 32 - M1 Thọ", "Gọi ingest_raw_line_v1 với chuỗi dài 32 và 33 ký tự", "Hàm từ chối xử lý, trả về mã lỗi an toàn -2, 0 byte bị ghi đè", "HIGH"),
        ("TC-02", "REQ-02, CON-01", "H1: Nạp chuỗi hợp lệ 31 byte thành công", "char buffer[32], cap = 32, chuỗi dài 31 ký tự - M1 Thọ", "Gọi ingest_raw_line_v1 với chuỗi 31 byte", "Trả về 31, sao chép thành công và kết thúc bằng '\\0'", "HIGH"),
        ("TC-03", "REQ-03, CON-02", "H2: Đọc token ngoài biên bị từ chối có kiểm soát", "token_count = 4, field_idx = 4 hoặc field_idx = -1 - M1 Thọ", "Gọi get_token_field_v1 với chỉ số ngoài biên", "Rào chắn phát hiện chỉ số vi phạm, từ chối an toàn trả về -2", "HIGH"),
        ("TC-04", "REQ-04, CON-02", "H2: Đọc token với con trỏ mảng NULL", "token_offsets = NULL - M1 Thọ", "Gọi get_token_field_v1 với con trỏ NULL", "Rào chắn phát hiện mảng NULL, trả về -1", "HIGH"),
        ("TC-05", "REQ-05, CON-03", "H3: OOM khi realloc bảo toàn vùng nhớ cũ", "mock_realloc trả về NULL - M2 Trang", "Gọi expand_stock_items_v1 khi hết RAM", "Bảo toàn 100% con trỏ cũ qua biến tạm tmp, trả về STATUS_ERR_ALLOC", "HIGH"),
        ("TC-06", "REQ-06, CON-03", "H3: Kích thước mở rộng được chặn cận trên", "new_capacity vượt giới hạn an toàn - M2 Trang", "Kiểm tra SMT Z3 trên heap_alloc_bytes", "Z3 chứng minh UNSAT: không tồn tại nghiệm vi phạm", "HIGH"),
        ("TC-07", "REQ-07, CON-04", "H4: Rào chắn ngăn chặn tràn số cộng dồn", "current_stock = 2.147.483.640, added_quantity = 20 - M3 Đô", "Gọi update_stock_quantity_v1", "Phát hiện nguy cơ tràn số trước khi thực hiện, trả về ERR_OVERFLOW", "HIGH"),
        ("TC-08", "REQ-08, CON-04", "H4: Từ chối số lượng nhập hàng âm", "added_quantity = -50 - M3 Đô", "Gọi update_stock_quantity_v1 với số âm", "Từ chối an toàn trả về ERR_NEGATIVE", "HIGH"),
        ("TC-09", "REQ-09, CON-04", "H5: Rào chắn ngăn chặn tràn phép nhân đơn giá", "base_price lớn và rate lớn - M3 Đô", "Gọi scale_product_price_v1", "Kiểm tra base_price > INT_MAX / rate, từ chối an toàn", "HIGH"),
        ("TC-10", "REQ-10, CON-05", "H6: Mutex đồng bộ hoàn toàn giữa 2 worker", "Cả 2 worker truy cập phiên kho - M2 Trang", "Kiểm tra loại trừ lẫn nhau trên Kripke v1", "Thỏa mãn 100%, 0 vi phạm tranh chấp", "HIGH"),
        ("TC-11", "REQ-11, REQ-12", "H6: Con trỏ được gán NULL ngay sau free", "Gọi release_stock_items_v1 - M2 Trang", "Kiểm tra con trỏ sau khi free()", "list == NULL, triệt tiêu hoàn toàn lỗi Use-After-Free và Double-Free", "HIGH"),
        ("TC-12", "REQ-01 đến REQ-12", "Kiểm chứng toàn luồng quản lý kho an toàn v1", "Mô hình kết hợp 27 trạng thái bản v1", "Chạy chay_kiem_chung_kripke.py", "Không deadlock, 100% 18 công thức CTL THỎA MÃN", "HIGH")
    ]
    for r in tests: ws10.append(list(r))
    format_sheet(ws10)

    p1 = os.path.abspath("01_spec/quanly_khohang_v1.xlsx")
    wb.save(p1)
    print("Saved v1 Excel to:", p1)

def export_json_specs():
    # -------------------------------------------------------------------------
    # Build quanly_khohang_v0.json
    # -------------------------------------------------------------------------
    wb0 = openpyxl.load_workbook("01_spec/quanly_khohang_v0.xlsx")
    ws_thongtin = wb0["ThongTin"]
    name_v0 = ws_thongtin.cell(row=2, column=2).value
    desc_v0 = ws_thongtin.cell(row=3, column=2).value

    # Requirements
    reqs_v0 = []
    ws_req = wb0["YeuCauChucNang"]
    for row in ws_req.iter_rows(min_row=2, values_only=True):
        if row[0]:
            reqs_v0.append({"id": row[0], "text": row[1], "category": row[2], "priority": row[3]})

    # States
    ws_state = wb0["MoHinhTrangThai"]
    v0_states_list = []
    v0_initial_list = []
    v0_labeling_dict = {}
    v0_transitions_list = []

    for row in ws_state.iter_rows(min_row=2, values_only=True):
        if not row[0]: continue
        s_name = str(row[0]).strip()
        v0_states_list.append(s_name)
        if row[1] and str(row[1]).strip().lower() in ['x', '1', 'true']:
            v0_initial_list.append(s_name)
        labels = [a.strip() for a in str(row[2] or '').split(',') if a.strip()]
        v0_labeling_dict[s_name] = labels
        targets = [t.strip() for t in str(row[3] or '').split(',') if t.strip() and t.strip() != '-']
        for tgt in targets:
            v0_transitions_list.append([s_name, tgt])

    # Safety properties
    ws_safety = wb0["ThuocTinhAnToan"]
    safety_v0 = [str(r[0]).strip() for r in ws_safety.iter_rows(min_row=2, values_only=True) if r[0]]

    # Mutex
    ws_mutex = wb0["LoaiTruLanNhau"]
    mutex_v0 = [[str(r[0]).strip(), str(r[1]).strip()] for r in ws_mutex.iter_rows(min_row=2, values_only=True) if r[0] and r[1]]

    # Functions
    ws_func = wb0["HamSinhMa"]
    funcs_v0 = []
    for r in ws_func.iter_rows(min_row=2, values_only=True):
        if not r[0]: continue
        fn = {"name": r[0], "language": r[1], "kind": r[2]}
        if r[3]: fn["params"] = [p.strip() for p in str(r[3]).split(',')]
        if r[4] is not None: fn["buffer_size"] = int(r[4])
        if r[5]: fn["bug"] = r[5]
        funcs_v0.append(fn)

    # Arithmetic
    ws_arith = wb0["KiemTraSoHoc"]
    arith_v0 = []
    for r in ws_arith.iter_rows(min_row=2, values_only=True):
        if not r[0]: continue
        arith_v0.append({
            "name": r[0],
            "int_vars": [v.strip() for v in str(r[1]).split(',')],
            "condition": r[2],
            "overflow_expr": r[3]
        })

    # Test cases
    ws_tc = wb0["TestCase"]
    tests_v0 = []
    for r in ws_tc.iter_rows(min_row=2, values_only=True):
        if not r[0]: continue
        tests_v0.append({
            "id": r[0],
            "requirement_ids": [req.strip() for req in str(r[1]).split(',')],
            "title": r[2],
            "preconditions": r[3],
            "steps": [s.strip() for s in str(r[4]).split('\n') if s.strip()],
            "expected_result": r[5],
            "priority": r[6]
        })

    spec_v0 = {
        "name": name_v0,
        "description": desc_v0,
        "requirements": reqs_v0,
        "states": v0_states_list,
        "initial": v0_initial_list,
        "labeling": v0_labeling_dict,
        "transitions": v0_transitions_list,
        "state_machine": {
            "states": v0_states_list,
            "initial": v0_initial_list,
            "labeling": v0_labeling_dict,
            "transitions": v0_transitions_list
        },
        "safety_properties": safety_v0,
        "mutual_exclusion": mutex_v0,
        "functions": funcs_v0,
        "arithmetic_checks": arith_v0,
        "test_cases": tests_v0
    }

    with open("01_spec/quanly_khohang_v0.json", "w", encoding="utf-8") as f:
        json.dump(spec_v0, f, ensure_ascii=False, indent=2)
    print("Exported quanly_khohang_v0.json")

    # -------------------------------------------------------------------------
    # Build quanly_khohang_v1.json
    # -------------------------------------------------------------------------
    wb1 = openpyxl.load_workbook("01_spec/quanly_khohang_v1.xlsx")
    ws_thongtin1 = wb1["ThongTin"]
    name_v1 = ws_thongtin1.cell(row=2, column=2).value
    desc_v1 = ws_thongtin1.cell(row=3, column=2).value

    reqs_v1 = []
    for row in wb1["YeuCauChucNang"].iter_rows(min_row=2, values_only=True):
        if row[0]:
            reqs_v1.append({"id": row[0], "text": row[1], "category": row[2], "priority": row[3]})

    v1_states_list = []
    v1_initial_list = []
    v1_labeling_dict = {}
    v1_transitions_list = []

    for row in wb1["MoHinhTrangThai"].iter_rows(min_row=2, values_only=True):
        if not row[0]: continue
        s_name = str(row[0]).strip()
        v1_states_list.append(s_name)
        if row[1] and str(row[1]).strip().lower() in ['x', '1', 'true']:
            v1_initial_list.append(s_name)
        labels = [a.strip() for a in str(row[2] or '').split(',') if a.strip()]
        v1_labeling_dict[s_name] = labels
        targets = [t.strip() for t in str(row[3] or '').split(',') if t.strip() and t.strip() != '-']
        for tgt in targets:
            v1_transitions_list.append([s_name, tgt])

    safety_v1 = [str(r[0]).strip() for r in wb1["ThuocTinhAnToan"].iter_rows(min_row=2, values_only=True) if r[0]]
    mutex_v1 = [[str(r[0]).strip(), str(r[1]).strip()] for r in wb1["LoaiTruLanNhau"].iter_rows(min_row=2, values_only=True) if r[0] and r[1]]

    funcs_v1 = []
    for r in wb1["HamSinhMa"].iter_rows(min_row=2, values_only=True):
        if not r[0]: continue
        fn = {"name": r[0], "language": r[1], "kind": r[2]}
        if r[3]: fn["params"] = [p.strip() for p in str(r[3]).split(',')]
        if r[4] is not None: fn["buffer_size"] = int(r[4])
        if r[5]: fn["bug"] = r[5]
        funcs_v1.append(fn)

    arith_v1 = []
    for r in wb1["KiemTraSoHoc"].iter_rows(min_row=2, values_only=True):
        if not r[0]: continue
        arith_v1.append({
            "name": r[0],
            "int_vars": [v.strip() for v in str(r[1]).split(',')],
            "condition": r[2],
            "overflow_expr": r[3]
        })

    tests_v1 = []
    for r in wb1["TestCase"].iter_rows(min_row=2, values_only=True):
        if not r[0]: continue
        tests_v1.append({
            "id": r[0],
            "requirement_ids": [req.strip() for req in str(r[1]).split(',')],
            "title": r[2],
            "preconditions": r[3],
            "steps": [s.strip() for s in str(r[4]).split('\n') if s.strip()],
            "expected_result": r[5],
            "priority": r[6]
        })

    spec_v1 = {
        "name": name_v1,
        "description": desc_v1,
        "requirements": reqs_v1,
        "states": v1_states_list,
        "initial": v1_initial_list,
        "labeling": v1_labeling_dict,
        "transitions": v1_transitions_list,
        "state_machine": {
            "states": v1_states_list,
            "initial": v1_initial_list,
            "labeling": v1_labeling_dict,
            "transitions": v1_transitions_list
        },
        "safety_properties": safety_v1,
        "mutual_exclusion": mutex_v1,
        "functions": funcs_v1,
        "arithmetic_checks": arith_v1,
        "test_cases": tests_v1
    }

    with open("01_spec/quanly_khohang_v1.json", "w", encoding="utf-8") as f:
        json.dump(spec_v1, f, ensure_ascii=False, indent=2)
    print("Exported quanly_khohang_v1.json")

if __name__ == '__main__':
    build_v0_excel()
    build_v1_excel()
    export_json_specs()
