# BÁO CÁO GIỚI HẠN KIỂM CHỨNG HÌNH THỨC (FORMAL LIMITATIONS)

## 1. NGUYÊN TẮC MINH BẠCH VÀ TRUNG THỰC KHOA HỌC
Theo tôn chỉ tại Mục V của Master Prompt, một báo cáo kiểm chứng an toàn phần mềm chuẩn mực phải phân định ranh giới rõ ràng giữa những gì đã chứng minh được và những gì còn nằm ngoài phạm vi bao phủ. Nhóm ATPM_Group_04 ghi nhận đầy đủ các giới hạn kỹ thuật sau đây:

## 2. CÁC CÔNG CỤ NATIVE KHÔNG KHẢ DỤNG (TOOL_UNAVAILABLE)
1. **CBMC Native Model Checker (`TOOL_UNAVAILABLE`):**
   - *Thực tế:* Trên môi trường Windows MSYS2 hiện tại, package `cbmc` không được cài đặt sẵn.
   - *Biện pháp thay thế trung thực:* Nhóm xây dựng 5 bộ kiểm tra Bounded Testing chạy bằng trình biên dịch GCC (`HARNESS-001` đến `HARNESS-005`). Toàn bộ báo cáo ghi rõ tên là **GCC Bounded Testing / Contract Harness**, tuyệt đối không giả lập log kết quả của CBMC.
2. **NuSMV / nuXmv (`TOOL_UNAVAILABLE`):**
   - *Thực tế:* Không có binary `NuSMV` trên hệ thống host.
   - *Biện pháp thay thế trung thực:* Nhóm xây dựng mô hình Kripke và công cụ kiểm chứng CTL bằng Python nội bộ (`verify_ctl.py`). Báo cáo ghi rõ tên công cụ là **Custom CTL Model Checker**.
3. **Clang Static Analyzer (`TOOL_UNAVAILABLE`):**
   - *Thực tế:* Không có `clang` và `scan-build` trong PATH.

## 3. PHÂN BIỆT GIỮA NGHIỆM SAT VÀ TÍNH KHẢ ĐẠT THỰC TẾ
- **Nghiệm SMT SAT không đồng nghĩa với Lỗ hổng có thể khai thác tại Runtime (Exploitable Vulnerability):**
  - Bài toán `SMT-001A` tìm ra nghiệm $capacity = 2^{63}$ làm tràn phép nhân $capacity * 2$.
  - Tuy nhiên, trong môi trường thực tế, để đạt được dung lượng $2^{63}$ phần tử `Product` (mỗi phần tử 208 bytes), hệ thống cần $2^{63} 	imes 208 pprox 3.8 	imes 10^{21}$ bytes RAM (vượt xa dung lượng bộ nhớ vật lý của toàn cầu).
  - Do đó, việc thiếu guard là một **khiếm khuyết thiết kế (Design Defect / Missing Defensive Guard)** chứ không phải một lỗ hổng có thể khai thác tức thời trong điều kiện chạy bình thường.

## 4. GIỚI HẠN CỦA ĐỘ SÂU UNWINDING TRONG BOUNDED TESTING
- Các harness GCC được kiểm thử tại các độ sâu hữu hạn: $k = 10, 17, 20, 40$.
- Kết quả PASS chỉ bảo đảm bất biến trong phạm vi $k$ bước lặp; không thể hiện sự chứng minh toán học trên miền vô hạn nếu không có các định lý bất biến quy nạp (inductive invariants).
