"""
TỔNG HỢP VÀ ĐỒNG BỘ TỆP EXCEL quanly_khohang_v0.xlsx
Dựa trên 3 tệp JSON cốt lõi của 3 thành viên:
1. m1_doc_chuoi_buffer.json (M1 - Nguyễn Quang Thọ - 23010322)
2. m2_cap_phat_heap.json (M2 - Nguyễn Kiều Trang - 23010495)
3. m3_tinh_ton_kho.json (M3 - Hà Văn Đô - 23010406)

Đảm bảo 100%:
- Đúng 10 sheet tiêu chuẩn của phần mềm Thầy và nhóm Trúc (thanhtoan_donhang_v0.xlsx)
- Đúng văn phong học thuật, câu chữ chỉ vị rõ ràng, không dùng văn AI
- Đồng bộ chuẩn xác tham số (Buffer 32 byte, các hàm C, kiểm tra số học SMT, 14 test cases)
"""
import openpyxl
from openpyxl.styles import Font, PatternFill, Alignment, Border, Side
from openpyxl.utils import get_column_letter
import os
import sys

sys.stdout.reconfigure(encoding='utf-8')

def build_quanly_khohang_v0():
    wb = openpyxl.Workbook()
    # Xóa sheet mặc định
    wb.remove(wb.active)

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

    # =========================================================================
    # 1. SHEET ThongTin
    # =========================================================================
    ws1 = wb.create_sheet(title="ThongTin")
    ws1.append(["Truong", "GiaTri"])
    ws1.append(["name", "quanly_khohang_v0"])
    ws1.append([
        "description",
        "Quy trình quản lý phiếu kho hàng điện máy gồm 6 hành vi: H1 tiếp nhận yêu cầu (chép mã phiếu vào bộ đệm Stack 32 byte - M1 Thọ), H2 tạo danh mục kiện hàng (cấp phát mảng struct Product trên Heap - M2 Trang), H3 tính tổng tiền và dung lượng mở rộng kho (số nguyên 32-bit, VND - M2/M3 Trang & Đô), H4 nhận ủy quyền thủ kho và cộng dồn tồn kho (update_stock_quantity - M3 Đô), H5 ghi nhật ký kiểm kê và quy đổi đơn giá (scale_product_price - M3 Đô), H6 đóng phiên kho hàng (worker nhập kho và worker xuất kho tự động dùng chung phiên, thu hồi tài nguyên - M2 Trang). Bản v0 CÀI LỖI CÓ CHỦ ĐÍCH: 18 trường hợp rủi ro an toàn bộ nhớ và tràn số nguyên (15 lỗi ẩn) để kiểm chứng tính phát hiện của mô hình Kripke, Bounded Model Checking và AddressSanitizer."
    ])
    format_sheet(ws1)

    # =========================================================================
    # 2. SHEET YeuCauChucNang
    # =========================================================================
    ws2 = wb.create_sheet(title="YeuCauChucNang")
    ws2.append(["ID", "MoTa", "DanhMuc", "MucUuTien"])
    reqs = [
        ("REQ-01", "H1: Hàm phân tích mã phiếu kho (parse_voucher_id) không được ghi vượt biên bộ đệm Stack 32 byte, kể cả khi mã dài ĐÚNG 32 ký tự (C1.2, C1.3)", "memory", "HIGH"),
        ("REQ-02", "H1: Mã phiếu kho hợp lệ không được bị cắt cụt hoặc biến đổi khi lưu vào bộ đệm Stack 32 byte (C1.4)", "functional", "HIGH"),
        ("REQ-03", "H2: Kết quả cấp phát mảng kiện hàng động (create_stock_items) trên Heap phải được kiểm tra NULL trước khi sử dụng (C2.2)", "memory", "HIGH"),
        ("REQ-04", "H2: Kích thước cấp phát mảng kiện hàng (n * sizeof(Product)) không được gây tràn số nguyên dẫn đến cấp phát thiếu bộ nhớ rồi ghi tràn Heap (C2.3)", "memory", "HIGH"),
        ("REQ-05", "H2: Mọi phần tử kiện hàng trong danh mục động phải được khởi tạo đầy đủ trước khi đọc, không đọc dữ liệu rác (C2.4)", "functional", "HIGH"),
        ("REQ-06", "H3: Phép tính tổng giá trị tiền tồn kho (đơn giá x số lượng + phí đóng gói) không được gây tràn số nguyên có dấu 32-bit (C3.2, C3.3)", "arithmetic", "HIGH"),
        ("REQ-07", "H3: Giá trị tiền tồn kho không được âm do chiết khấu lớn hơn tạm tính hoặc số lượng âm (C3.4)", "safety", "HIGH"),
        ("REQ-08", "H3: Phép phân bổ định mức chi phí kho theo kỳ hạn không được chia cho 0 khi kỳ hạn bằng 0 (C3.5)", "arithmetic", "MEDIUM"),
        ("REQ-09", "H4: Chuỗi mã token ủy quyền thủ kho từ nguồn ngoài không được vượt quá 20 ký tự (C4.2)", "memory", "HIGH"),
        ("REQ-10", "H4: Luồng xác thực quyền thủ kho không được chứa nhánh bí mật (magic value) bỏ qua kiểm tra an ninh (C4.3)", "security", "HIGH"),
        ("REQ-11", "H4: Phép cộng dồn tồn kho mới vào tồn kho cũ (update_stock_quantity) không được gây tràn số nguyên có dấu 32-bit và phải từ chối số lượng nhập âm (C4.4)", "arithmetic", "HIGH"),
        ("REQ-12", "H5: Hàm ghi nhật ký kiểm kê kho không được rò rỉ bộ nhớ tiến trình khi tái cấp phát bộ đệm nhật ký (C5.2)", "memory", "HIGH"),
        ("REQ-13", "H5: Nội dung ghi nhật ký kiểm kê không được sử dụng trực tiếp chuỗi do người dùng nhập làm format string cho printf/syslog (C5.3)", "security", "HIGH"),
        ("REQ-14", "H5: Phép nhân quy đổi giá trị hàng hóa theo tỉ giá (scale_product_price) không được gây tràn số nguyên có dấu 32-bit (C5.4)", "arithmetic", "MEDIUM"),
        ("REQ-15", "H6: Hai tiến trình worker nhập kho và worker xuất kho tự động dùng chung phiên kho không được đồng thời can thiệp gây tranh chấp tài nguyên (C6.2)", "concurrency", "HIGH"),
        ("REQ-16", "H6: Khi đóng phiên kho hàng, con trỏ phiên làm việc không được giải phóng hai lần (CWE-415 Double Free) (C6.3)", "memory", "HIGH"),
        ("REQ-17", "H6: Sau khi giải phóng bộ nhớ danh mục hàng hóa (release_stock_items), con trỏ phải được gán NULL ngay lập tức; cấm tuyệt đối đọc/ghi vào vùng nhớ đã giải phóng (CWE-416 Use-After-Free) (C6.4)", "memory", "HIGH"),
        ("REQ-18", "H6: Bộ nhớ chứa mã PIN thủ kho và dữ liệu nhạy cảm phải được xóa sạch bằng explicit_bzero trước khi gọi free(), không để sót lại trên Heap (C6.5)", "security", "HIGH")
    ]
    for r in reqs:
        ws2.append(list(r))
    format_sheet(ws2)

    # =========================================================================
    # 3. SHEET YeuCauPhiChucNang
    # =========================================================================
    ws3 = wb.create_sheet(title="YeuCauPhiChucNang")
    ws3.append(["ID", "MoTa", "MucUuTien"])
    nfrs = [
        ("NFR-01", "Hệ thống xử lý tối thiểu 300 giao dịch xuất nhập kho/giây trên phần cứng máy chủ tham chiếu", "MEDIUM"),
        ("NFR-02", "Phản hồi xác thực token ủy quyền thủ kho trong tối đa 2 giây (tránh timeout đồng bộ kho phân tán)", "HIGH"),
        ("NFR-03", "Bộ nhớ tiến trình quản lý kho không tăng quá 5% sau 1.000.000 lượt kiểm kê liên tiếp (chống rò rỉ bộ nhớ tích lũy)", "HIGH"),
        ("NFR-04", "Không xảy ra race condition giữa luồng nhập kho và luồng quét xuất kho tự động chu kỳ 15 phút", "HIGH"),
        ("NFR-05", "Mã PIN thủ kho và token phiên làm việc không tồn tại trong dump bộ nhớ tiến trình sau khi đóng phiên kho", "HIGH"),
        ("NFR-06", "Thời gian phản hồi thao tác mở rộng bộ nhớ danh mục hàng đạt trung bình O(1) thông qua chiến lược nhân đôi dung lượng", "MEDIUM")
    ]
    for r in nfrs:
        ws3.append(list(r))
    format_sheet(ws3)

    # =========================================================================
    # 4. SHEET RangBuoc
    # =========================================================================
    ws4 = wb.create_sheet(title="RangBuoc")
    ws4.append(["ID", "MoTa", "MucUuTien"])
    cons = [
        ("CON-01", "Mã phiếu kho tối đa 31 ký tự ASCII (31 ký tự cần 32 byte bộ đệm Stack kể cả '\\0')", "HIGH"),
        ("CON-02", "Mã token ủy quyền thủ kho tối đa 20 ký tự ASCII (cần 21 byte kể cả '\\0')", "HIGH"),
        ("CON-03", "Mỗi phiếu có 1–100 dòng kiện hàng; số lượng mỗi dòng 1–1000; đơn giá 0–50.000.000 VND; giá trị mỗi dòng tối đa 50.000.000 VND", "HIGH"),
        ("CON-04", "Đơn giá và tổng tiền là số nguyên có dấu 32-bit (signed int, đơn vị VND)", "HIGH"),
        ("CON-05", "Số kỳ hạn phân bổ định mức chi phí kho từ 1 đến 24 tháng", "HIGH"),
        ("CON-06", "Worker quét xuất kho tự động kích hoạt định kỳ sau 15 phút rảnh phiên", "MEDIUM"),
        ("CON-07", "Bộ nhớ chứa mã PIN thủ kho và mảng kiện hàng động bắt buộc cấp phát trên Heap qua malloc/calloc", "HIGH")
    ]
    for r in cons:
        ws4.append(list(r))
    format_sheet(ws4)

    # =========================================================================
    # 5. SHEET MoHinhTrangThai (35 trạng thái Kripke chuẩn mực)
    # =========================================================================
    ws5 = wb.create_sheet(title="MoHinhTrangThai")
    ws5.append(["TrangThai", "KhoiTao", "Nhan", "ChuyenDenCacTrangThai", "MaTruongHop", "GhiChu"])
    states = [
        ("start", "x", "start, safe", "h1_vao", "-", "Bắt đầu xử lý 1 phiếu kho hàng"),
        ("h1_vao", None, "h1, receiving", "h1_ok, h1_tran_1_byte, h1_tran_bo_dem", "H1", "Nhận chuỗi mã phiếu kho từ giao diện"),
        ("h1_ok", None, "h1, input_ok", "h2_vao", "C1.1", "Mã phiếu chép đủ vào buffer 32 byte, có '\\0' (an toàn)"),
        ("h1_tran_1_byte", None, "h1, stack_buffer_overflow, off_by_one, violation", "dung_khong_an_toan", "C1.2", "Mã phiếu dài đúng 32 ký tự: ghi đè byte '\\0' vào byte lân cận (lỗi ẩn Off-by-one)"),
        ("h1_tran_bo_dem", None, "h1, stack_buffer_overflow, violation", "dung_khong_an_toan", "C1.3", "Mã phiếu dài > 32 byte: ghi đè tràn stack frame (lỗi hiện Buffer Overflow)"),
        ("h2_vao", None, "h2, allocating", "h2_ok, h2_null, h2_sai_kich_thuoc, h2_doc_rac", "H2", "items = malloc(n * sizeof(Product))"),
        ("h2_ok", None, "h2, allocated, initialized", "h3_vao", "C2.1", "Cấp phát thành công và đã khởi tạo struct (an toàn)"),
        ("h2_null", None, "h2, null_deref, violation", "dung_khong_an_toan", "C2.2", "malloc trả NULL khi hết RAM nhưng vẫn truy cập ghi vào (lỗi ẩn OOM)"),
        ("h2_sai_kich_thuoc", None, "h2, alloc_size_overflow, heap_overflow, violation", "dung_khong_an_toan", "C2.3", "n*48 tràn int32 hoặc n âm: cấp phát sai kích thước rồi ghi tràn Heap (lỗi ẩn)"),
        ("h2_doc_rac", None, "h2, uninit_read, violation", "dung_khong_an_toan", "C2.4", "Đọc dữ liệu kiện hàng khi chưa khởi tạo memset/calloc (lỗi ẩn Uninit Read)"),
        ("h3_vao", None, "h3, computing", "h3_ok, h3_tran_so, h3_so_am, h3_chia_0", "H3", "Tính tổng tồn kho = Σ đơn giá × SL + phí đóng gói − chiết khấu"),
        ("h3_ok", None, "h3, amount_ok", "h4_vao", "C3.1", "Tổng giá trị tồn kho nằm trong miền hợp lệ [0, 2 tỷ VND] (an toàn)"),
        ("h3_tran_so", None, "h3, int_overflow, violation", "dung_khong_an_toan", "C3.2/C3.3", "Tràn int32 khi nhân đơn giá x SL hoặc khi cộng dồn nhiều kiện (lỗi ẩn)"),
        ("h3_so_am", None, "h3, negative_amount, violation", "dung_khong_an_toan", "C3.4", "SL/chiết khấu âm hoặc chiết khấu > tạm tính: tổng tiền âm (lỗi ẩn)"),
        ("h3_chia_0", None, "h3, div_by_zero, violation", "dung_khong_an_toan", "C3.5", "Phân bổ chiết khấu với kỳ hạn = 0: chia cho 0 gây crash tiến trình"),
        ("h4_vao", None, "h4, validating", "h4_ok, h4_tran_token, h4_nhanh_an, h4_tu_choi_quyen", "H4", "Xác thực token ủy quyền thủ kho và cộng dồn số lượng tồn kho"),
        ("h4_ok", None, "h4, token_ok, authorized", "h5_vao", "C4.1", "Token hợp lệ, chữ ký đúng, cập nhật tồn kho an toàn"),
        ("h4_tran_token", None, "h4, stack_buffer_overflow, violation", "dung_khong_an_toan", "C4.2", "Token ủy quyền dài > 20 byte: ghi tràn bộ đệm token (lỗi hiện)"),
        ("h4_nhanh_an", None, "h4, hidden_bypass, violation", "dung_khong_an_toan", "C4.3", "Token mang giá trị ma thuật (magic value 31337/8080) bỏ qua xác thực (cửa sau ẩn)"),
        ("h4_tu_choi_quyen", None, "h4, token_invalid", "h6_phien_ranh", "C4.4", "Token sai: từ chối quyền nhưng vẫn đẩy phiên sang bước đóng (an toàn)"),
        ("h5_vao", None, "h5, logging", "h5_ok, h5_ro_ri, h5_format_string", "H5", "Ghi nhật ký kiểm kê kho và quy đổi đơn giá sản phẩm"),
        ("h5_ok", None, "h5, log_ok, audit_clean", "h6_phien_ranh", "C5.1", "Ghi log thành công, giải phóng bộ đệm log sạch sẽ"),
        ("h5_ro_ri", None, "h5, mem_leak, violation", "dung_khong_an_toan", "C5.2", "realloc bộ đệm log thất bại gán đè NULL làm mất con trỏ cũ (CWE-401 rò rỉ bộ nhớ)"),
        ("h5_format_string", None, "h5, format_string, violation", "dung_khong_an_toan", "C5.3", "syslog(LOG_INFO, raw_comment): chuỗi nhập chứa %x/%n đọc ghi tùy ý bộ nhớ"),
        ("h6_phien_ranh", None, "h6, sess_free", "h6_nhap_giu, h6_xuat_giu", "H6", "Phiên kho đang mở, chưa worker nào giữ khóa"),
        ("h6_nhap_giu", None, "h6, nhap_locked", "h6_giai_phong, h6_tranh_chap", "C6.1", "Worker nhập kho đang giữ khóa phiên làm việc"),
        ("h6_xuat_giu", None, "h6, xuat_locked", "h6_giai_phong, h6_tranh_chap", "C6.1", "Worker xuất kho tự động (15 phút) đang giữ khóa phiên"),
        ("h6_tranh_chap", None, "h6, nhap_locked, xuat_locked, race, violation", "h6_double_free, h6_uaf", "C6.2", "Cả 2 worker cùng giữ phiên (Race condition, vi phạm loại trừ lẫn nhau)"),
        ("h6_double_free", None, "h6, double_free, violation", "dung_khong_an_toan", "C6.3", "Cả 2 worker cùng gọi free(session) trên cùng một con trỏ (CWE-415)"),
        ("h6_uaf", None, "h6, use_after_free, violation", "dung_khong_an_toan", "C6.4", "Worker đến sau đọc/ghi vào phiên kho đã bị worker trước free (CWE-416)"),
        ("h6_giai_phong", None, "h6, releasing", "ket_thuc_ok, h6_con_ma_pin", "C6.1", "Giải phóng bộ đệm mã PIN thủ kho và mảng sản phẩm"),
        ("h6_con_ma_pin", None, "h6, secret_residue, violation", "dung_khong_an_toan", "C6.5", "Mã PIN thủ kho còn nguyên trong Heap sau free (memset bị compiler tối ưu -O2 loại bỏ)"),
        ("ket_thuc_ok", None, "flow_end, stock_done, mem_clean, safe", "ket_thuc_ok", "C6.1", "Phiên kho kết thúc thành công, giải phóng toàn bộ bộ nhớ (trạng thái an toàn)"),
        ("ket_thuc_tu_choi", None, "flow_end, input_rejected, mem_clean, safe", "ket_thuc_tu_choi", "C1.4/C4.4", "Từ chối đầu vào không hợp lệ, thu hồi bộ nhớ an toàn (v0 thiếu nhánh này)"),
        ("dung_khong_an_toan", None, "flow_end, unsafe_halt, violation", "dung_khong_an_toan", "-", "Tiến trình sập (crash SIGSEGV/ABORT) do vi phạm an toàn bộ nhớ")
    ]
    for r in states:
        ws5.append(list(r))
    format_sheet(ws5)

    # =========================================================================
    # 6. SHEET ThuocTinhAnToan (22 công thức CTL)
    # =========================================================================
    ws6 = wb.create_sheet(title="ThuocTinhAnToan")
    ws6.append(["CongThucCTL", "GhiChu"])
    ctls = [
        ("AG(!violation)", "Safety tổng: không bao giờ đạt trạng thái vi phạm nào"),
        ("AG(!buffer_overflow)", "Safety C1.2, C1.3, C4.2 — CWE-120/787 (tràn stack bộ đệm mã phiếu 32 byte và token 20 byte)"),
        ("AG(!off_by_one)", "Safety C1.2 — CWE-193 (lỗi ẩn: mã phiếu dài đúng 32 ký tự ghi đè null terminator)"),
        ("AG(!null_deref)", "Safety C2.2 — CWE-476 (lỗi ẩn: malloc trả NULL khi cạn RAM nhưng vẫn ghi vào)"),
        ("AG(!alloc_size_overflow)", "Safety C2.3 — CWE-190/787 (lỗi ẩn: n * 48 byte tràn int32 dẫn tới cấp phát thiếu ô nhớ trên Heap)"),
        ("AG(!uninit_read)", "Safety C2.4 — CWE-457 (lỗi ẩn: đọc trường trạng thái kiện hàng khi chưa khởi tạo)"),
        ("AG(!int_overflow)", "Safety C3.2, C3.3, C4.4 — CWE-190 (lỗi ẩn: nhân đơn giá tràn int32 hoặc cộng dồn tồn kho vượt 2 tỷ)"),
        ("AG(!negative_amount)", "Safety C3.4, C4.4 (lỗi ẩn: số lượng âm hoặc chiết khấu lớn hơn tạm tính làm tổng âm)"),
        ("AG(!div_by_zero)", "Safety C3.5 — CWE-369 (lỗi crash: phân bổ chi phí kho với kỳ hạn bằng 0)"),
        ("AG(!hidden_bypass)", "Safety C4.3 — CWE-912 (lỗi ẩn: token ma thuật 31337/8080 vượt mặt xác thực thủ kho)"),
        ("AG(!mem_leak)", "Safety C5.2 — CWE-401 (lỗi ẩn: realloc thất bại gán đè NULL làm rò rỉ vùng nhớ tiến trình)"),
        ("AG(!format_string)", "Safety C5.3 — CWE-134 (lỗi bảo mật: chuỗi format string trong ghi nhật ký kiểm kê)"),
        ("AG(!race)", "Safety C6.2 — CWE-667 (lỗi ẩn: worker nhập kho và worker xuất kho tự động cùng chiếm phiên)"),
        ("AG(!double_free)", "Safety C6.3 — CWE-415 (lỗi crash: giải phóng phiên kho 2 lần liên tiếp)"),
        ("AG(!use_after_free)", "Safety C6.4 — CWE-416 (lỗi ẩn: đọc/ghi dữ liệu sau khi đã gọi free phiên kho)"),
        ("AG(!secret_residue)", "Safety C6.5 — CWE-226/14 (lỗi ẩn: mã PIN thủ kho còn sót lại trên Heap sau khi giải phóng)"),
        ("AF(flow_end)", "Liveness: mọi nhánh xử lý đều kết thúc (v0 kẹt tại trạng thái vi phạm lặp vô tận)"),
        ("EF(stock_done)", "Khả đạt REQ-18: tồn tại đường đi hoàn thành phiên kho an toàn thành công"),
        ("EF(input_rejected)", "Khả đạt: tồn tại nhánh từ chối đầu vào độc hại/sai cú pháp (v0 KHÔNG có -> VI PHẠM)"),
        ("EG(!violation)", "Tồn tại ít nhất một đường đi luôn an toàn từ đầu đến cuối"),
        ("EF(nhap_locked)", "Khả đạt của cụm khóa phiên nhập kho (mô hình khóa phiên hoạt động được)"),
        ("EF(xuat_locked)", "Khả đạt của cụm khóa phiên xuất kho tự động (mô hình khóa phiên hoạt động được)")
    ]
    for r in ctls:
        ws6.append(list(r))
    format_sheet(ws6)

    # =========================================================================
    # 7. SHEET LoaiTruLanNhau (6 cặp)
    # =========================================================================
    ws7 = wb.create_sheet(title="LoaiTruLanNhau")
    ws7.append(["MenhDe1", "MenhDe2"])
    mutex = [
        ("nhap_locked", "xuat_locked"),
        ("violation", "flow_end"),
        ("stock_done", "input_rejected"),
        ("unsafe_halt", "mem_clean"),
        ("use_after_free", "mem_clean"),
        ("secret_residue", "mem_clean")
    ]
    for r in mutex:
        ws7.append(list(r))
    format_sheet(ws7)

    # =========================================================================
    # 8. SHEET HamSinhMa
    # =========================================================================
    ws8 = wb.create_sheet(title="HamSinhMa")
    ws8.append(["Ten", "NgonNgu", "Loai", "ThamSo", "KichThuocBuffer", "Loi", "GiaTriBiMat1", "GiaTriBiMat2", "GhiChu"])
    funcs = [
        ("parse_voucher_id", "c", "buffer_copy", "raw_voucher_id", 32, "buffer-overflow", None, None, "H1 — C1.2/C1.3 (CON-01 bộ đệm Stack 32 byte - M1 Thọ)"),
        ("parse_auth_token", "c", "buffer_copy", "raw_token", 20, "buffer-overflow", None, None, "H4 — C4.2 (CON-02 token 20 byte)"),
        ("create_stock_items", "c", "alloc_release", None, 100, None, None, None, "H2 — C2.2/C2.3/C2.4 cấp phát động struct Product trên Heap (M2 Trang)"),
        ("expand_stock_items", "c", "alloc_release", None, 16, "memory-leak", None, None, "H3 — realloc mảng động khi OOM làm mất con trỏ gốc (CWE-401 - M2 Trang)"),
        ("release_stock_items", "c", "alloc_release", None, 16, "use-after-free", None, None, "H6 — free() không gán NULL dẫn tới Use-After-Free (CWE-416 - M2 Trang)"),
        ("write_inventory_log", "c", "alloc_release", None, 32, "memory-leak", None, None, "H5 — C5.2 rò rỉ bộ nhớ khi mở rộng buffer log"),
        ("close_inventory_session", "c", "alloc_release", None, 4, "double-free", None, None, "H6 — C6.3 giải phóng phiên làm việc hai lần (CWE-415)"),
        ("update_stock_quantity", "c", "arithmetic", "current_stock, added_quantity", None, None, None, None, "H4 — C4.4 cộng dồn tồn kho tràn signed int 32-bit (M3 Đô)"),
        ("scale_product_price", "c", "arithmetic", "base_price, rate", None, None, None, None, "H5 — C5.4 nhân tỉ giá quy đổi tràn int32 (M3 Đô)"),
        ("calc_batch_capacity", "python", "divide", "total_items, batch_size", None, None, None, None, "H3 — C3.5 chia cho 0 khi batch_size = 0 (black-box fuzz)"),
        ("auth_debug_bypass", "python", "magic_value", "warehouse_id, debug_pin", None, None, 31337, 8080, "H4 — C4.3 nhánh ẩn backdoor vượt mặt xác thực (concolic)")
    ]
    for r in funcs:
        ws8.append(list(r))
    format_sheet(ws8)

    # =========================================================================
    # 9. SHEET KiemTraSoHoc (6 ràng buộc Z3 SMT)
    # =========================================================================
    ws9 = wb.create_sheet(title="KiemTraSoHoc")
    ws9.append(["Ten", "BienSoNguyen", "DieuKien", "BieuThucTran", "GhiChu"])
    ariths = [
        ("stock_addition_overflow", "current_stock, added_quantity", "current_stock >= 0 and current_stock <= 2147483647 and added_quantity >= 1 and added_quantity <= 2147483647 and current_stock + added_quantity > 2147483647", "current_stock + added_quantity", "C4.4 — phép cộng dồn tồn kho mới vào tồn kho cũ vượt trần INT_MAX (M3 Đô)"),
        ("heap_alloc_bytes", "new_capacity, item_size", "new_capacity >= 1 and item_size == 48 and new_capacity * item_size > 2147483647", "new_capacity * item_size", "C2.3 — kích thước tái cấp phát Heap vượt 2GB khi new_capacity lớn (M2 Trang)"),
        ("line_amount_mul", "price, qty", "price >= 0 and price <= 50000000 and qty >= 1 and qty <= 1000 and price * qty > 2147483647", "price * qty", "C3.2 — phép nhân đơn giá x số lượng tràn TRƯỚC khi kịp kiểm tra hạn mức dòng"),
        ("accumulated_total", "total_before, line_total", "total_before >= 0 and total_before <= 2147483647 and line_total >= 0 and line_total <= 50000000 and total_before + line_total > 2147483647", "total_before + line_total", "C3.3 — bước cộng dồn nhiều kiện hàng: mỗi kiện hợp lệ nhưng tổng vượt trần int32"),
        ("negative_quantity_check", "added_quantity", "added_quantity < 0", "added_quantity", "C4.4 — số lượng nhập hàng là số âm làm sai lệch số liệu kho (M3 Đô)"),
        ("negative_total", "subtotal, discount", "subtotal >= 0 and subtotal <= 2000000000 and discount + 2000000000 >= 0 and discount <= 2000000000 and subtotal - discount < 0", "subtotal - discount", "C3.4 — chiết khấu giảm giá âm hoặc lớn hơn tạm tính làm tổng tiền âm")
    ]
    for r in ariths:
        ws9.append(list(r))
    format_sheet(ws9)

    # =========================================================================
    # 10. SHEET TestCase (14 ca kiểm thử chi tiết)
    # =========================================================================
    ws10 = wb.create_sheet(title="TestCase")
    ws10.append(["ID", "YeuCauLienQuan", "TieuDe", "DieuKienTienQuyet", "CacBuoc", "KetQuaMongDoi", "MucUuTien"])
    tests = [
        ("TC-01", "REQ-01, CON-01", "Mã phiếu kho dài đúng 32 ký tự và dài 100 ký tự", "parse_voucher_id sinh từ đặc tả (v0: buffer 32 byte, Loi=buffer-overflow) - M1 Thọ", "Sinh mã C cho parse_voucher_id\nChạy check_c_source() (Chương 2)\nKiểm tra AG(!buffer_overflow), AG(!off_by_one) trên mô hình Kripke", "v0: dangerous-function strcpy (CWE-120/121), 2 công thức VI PHẠM; v1: 0 phát hiện, công thức ĐÚNG", "HIGH"),
        ("TC-02", "REQ-02, CON-01", "Bản sửa của phần mềm có cắt cụt mã phiếu 32 ký tự không", "Mô hình con M1, luồng 'M1-v1a' (strncpy 31 + '\\0') - M1 Thọ", "Nạp mo_hinh_con/m1_doc_chuoi_buffer.json vào tab Ch1\nChọn luồng M1-v1a, kiểm tra AG(!truncated)\nSo sánh với luồng M1-v1b", "M1-v1a: AG(!truncated) VI PHẠM (mã bị cắt, phần mềm không báo); M1-v1b: ĐÚNG", "HIGH"),
        ("TC-03", "REQ-03, REQ-04, REQ-05", "Cấp phát mảng kiện hàng trên Heap", "create_stock_items (alloc_release); mô hình con M2 - M2 Trang", "Chạy check_c_source() trên mã sinh\nKiểm tra M2-v0: AG(!null_deref), AG(!heap_overflow), AG(!uninit_read)\nChạy KiemTraSoHoc heap_alloc_bytes", "Phần mềm quét mã: 0 phát hiện; mô hình M2-v0: 3 công thức VI PHẠM; heap_alloc_bytes SAT (v0) / UNSAT (v1)", "HIGH"),
        ("TC-04", "REQ-06, CON-03, CON-04", "Tổng tiền tồn kho và cộng dồn tràn int32", "KiemTraSoHoc stock_addition_overflow, line_amount_mul, accumulated_total; BMC vòng lặp trong file JSON", "Chạy check_satisfiability() trên DieuKien\nChạy BMC bmc_cong_don_ton_kho (k = 100)", "v0: SAT kèm phản ví dụ, BMC vi phạm tại bước 48 (total = 2.160.000.000); v1: UNSAT, BMC không vi phạm", "HIGH"),
        ("TC-05", "REQ-07, REQ-11, CON-03", "Số lượng nhập âm và số tiền âm do chiết khấu", "KiemTraSoHoc negative_quantity_check, negative_total - M3 Đô", "Chạy check_satisfiability() trên DieuKien của Z3", "v0: SAT (tìm được nghiệm added_quantity = -50 và discount > subtotal); v1: UNSAT", "HIGH"),
        ("TC-06", "REQ-08, CON-05", "Phân bổ chi phí kho với kỳ hạn = 0", "Hàm Python calc_batch_capacity(total_items, batch_size) (kind = divide) — chỉ có ở v0", "Chạy blackbox_fuzz() 300 lần", "v0: tìm được ít nhất 1 crash ZeroDivisionError (CWE-248/369); v1: hàm thay bằng kiểm tra 1 <= batch_size <= 24", "MEDIUM"),
        ("TC-07", "REQ-09, CON-02", "Mã token ủy quyền thủ kho dài hơn 20 ký tự", "parse_auth_token (v0: buffer 20, Loi=buffer-overflow)", "Sinh mã C\nChạy check_c_source()", "v0: dangerous-function strcpy (CWE-120); v1: 0 phát hiện", "HIGH"),
        ("TC-08", "REQ-10", "Nhánh ẩn backdoor bỏ qua xác thực quyền thủ kho", "Hàm Python auth_debug_bypass(warehouse_id, debug_pin), magic_values = (31337, 8080)", "Chạy black-box fuzz trước (kỳ vọng không tìm ra)\nChạy concolic_fuzz()", "Black-box: 0 lần chạm nhánh; concolic: tìm đúng (31337, 8080) sau vài lần thử (CWE-912)", "HIGH"),
        ("TC-09", "REQ-12, NFR-03", "Rò rỉ bộ nhớ khi mở rộng buffer ghi nhật ký kiểm kê", "write_inventory_log (v0: Loi=memory-leak)", "Sinh mã C\nChạy check_c_source()", "v0: memory-leak (CWE-401); v1: 0 phát hiện", "MEDIUM"),
        ("TC-10", "REQ-13", "Chuỗi định dạng format string trong nhật ký kiểm kê", "Mô hình con M5 (Ghi nhật ký kiểm kê kho)", "Nạp M5_ghi_nhat_ky.json vào tab Ch1\nChọn luồng M5-v0, kiểm tra AG(!format_string)\nChọn luồng M5-v1, kiểm tra lại", "M5-v0: AG(!format_string) VI PHẠM (CWE-134); M5-v1: ĐÚNG", "MEDIUM"),
        ("TC-11", "REQ-15, NFR-04", "Worker nhập kho và worker xuất kho tự động cùng đóng phiên", "Cụm trạng thái h6_* trong mô hình kết hợp; LoaiTruLanNhau dòng 1 (nhap_locked, xuat_locked)", "Worker xuất kho kích hoạt đúng lúc worker nhập kho đang giữ phiên\nChạy check_mutual_exclusion(model, 'nhap_locked', 'xuat_locked')", "v0: Mutual exclusion VI PHẠM tại h6_tranh_chap (CWE-667); v1: ĐÚNG", "HIGH"),
        ("TC-12", "REQ-16, REQ-17", "Double-free và Use-after-free khi giải phóng phiên kho", "close_inventory_session (double-free), release_stock_items (use-after-free) - M2 Trang", "Gộp mã C của 2 hàm\nChạy check_c_source()\nChạy kiểm toán AddressSanitizer", "v0: double-free (CWE-415) và use-after-free (CWE-416); v1: 0 phát hiện, 0 crash ASan", "HIGH"),
        ("TC-13", "REQ-18, NFR-05, CON-07", "Mã PIN thủ kho còn trong bộ nhớ Heap sau khi giải phóng", "Mô hình con M6 (Đóng phiên kho và thu hồi tài nguyên)", "Nạp M6_dong_phien.json vào tab Ch1\nChọn luồng M6-v0, kiểm tra AG(!secret_residue)\nChọn luồng M6-v1, kiểm tra lại", "M6-v0: VI PHẠM (CWE-226/244, CWE-14); M6-v1 (explicit_bzero trước free): ĐÚNG", "HIGH"),
        ("TC-14", "REQ-01 đến REQ-18", "Kiểm chứng toàn luồng quản lý kho hàng", "Mô hình kết hợp tổng thể (sheet MoHinhTrangThai 35 trạng thái)", "Chạy '1) Sinh Kripke + kiểm chứng (Ch1)'\nKiểm tra deadlock, AF(flow_end), EF(stock_done), EF(input_rejected)", "Không deadlock; v0: AF(flow_end) và EF(input_rejected) VI PHẠM; v1: tất cả 22 công thức ĐÚNG", "HIGH")
    ]
    for r in tests:
        ws10.append(list(r))
    format_sheet(ws10)

    # Lưu tệp
    out_path = os.path.join(os.path.dirname(__file__), "..", "01_spec", "quanly_khohang_v0.xlsx")
    wb.save(out_path)
    print(f"ĐÃ LƯU THÀNH CÔNG: {out_path}")

if __name__ == '__main__':
    build_quanly_khohang_v0()
