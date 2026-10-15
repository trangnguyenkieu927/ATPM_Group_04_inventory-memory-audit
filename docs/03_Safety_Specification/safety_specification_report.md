# BÁO CÁO ĐẶC TẢ AN TOÀN VÀ ÁNH XẠ 18 REQ (STAGE 4 REPORT)

## 1. NGUYÊN TẮC BẢO TOÀN VÀ CHUẨN HÓA 18 REQ
Theo yêu cầu bắt buộc của Giảng viên và Mục IV/IV của Master Prompt:
- **100% nội dung REQ-1 đến REQ-18 được giữ nguyên văn** từ tài liệu chuẩn Bước 2 (`Spec_Inventory_Memory_Safety_Audit_18REQ_CHUAN.xlsx` và `requirement_specifications.md`).
- Tuyệt đối không thay đổi mã định danh hoặc ý nghĩa kỹ thuật của từng yêu cầu.
- Toàn bộ 18 yêu cầu được phân bổ trực tiếp vào 6 kịch bản rủi ro (SC-01 đến SC-06).

## 2. PHÂN TÍCH TÌNH TRẠNG GUARD TRÊN BASELINE V0
Qua đối chiếu trực tiếp với mã nguồn C thực tế tại commit baseline `44202070c68002d9af068374478cdc83098dd4d8`, tình trạng kiểm soát an toàn được phân loại trung thực như sau:

| Nhóm trạng thái | Số lượng | Danh sách REQ | Đánh giá kỹ thuật |
|---|---|---|---|
| **Đã có guard hoàn toàn (GUARDED_IN_V0)** | 15 / 18 | REQ-1, REQ-4, REQ-5, REQ-6, REQ-7, REQ-8, REQ-9, REQ-10, REQ-11, REQ-12, REQ-13, REQ-15, REQ-16, REQ-17, REQ-18 | Mã nguồn baseline đã lập trình phòng vệ cẩn thận (ví dụ: dùng con trỏ tạm realloc, gán NULL sau free, chặn tràn số `INT_MAX`, kiểm tra NULL pointer, giới hạn `token_count < 8`). |
| **Bảo vệ theo hợp đồng (GUARDED_BY_CONTRACT)** | 1 / 18 | REQ-14 | Quy ước con trỏ mượn `find_product_by_id` chỉ được đọc tức thời; kịch bản lỗi Use-after-free được kích hoạt trong mô phỏng đối chứng lỗi seeded. |
| **Thiếu guard tường minh (GUARD_MISSING_IN_V0)** | 2 / 18 | **REQ-2, REQ-3** | **Điểm rủi ro then chốt của bài toán:** Baseline v0 thực hiện `new_cap = capacity * 2` và `new_cap * sizeof(Product)` mà chưa có guard kiểm tra tràn số nguyên `size_t`. Mô hình SMT Z3 chứng minh bài toán SAT có nghiệm toán học. Hai yêu cầu này được đánh dấu `PARTIALLY_VERIFIED` và sẽ được bổ sung bản vá ở Công đoạn 9. |

## 3. DANH MỤC CÁC TÀI LIỆU VÀ WORKBOOK ĐÃ TẠO
Tất cả các tài liệu chuẩn hóa đặt tại `docs/03_Safety_Specification/`:
1. `safety_requirements.xlsx`: Bảng đặc tả 18 REQ chuẩn với 13 trường thông tin chi tiết.
2. `formal_properties.xlsx`: Bảng ánh xạ các thuộc tính logic hình thức (CTL, SMT QF_BV, BMC Invariant).
3. `scenario_req_mapping.xlsx`: Ma trận truy vết 2 chiều giữa 6 Kịch bản rủi ro và 18 REQ.
4. `safety_specifications.json`: Tập tin JSON chuẩn hóa phục vụ các công cụ kiểm chứng tự động.
5. `risk_register_updated.xlsx`: Bảng đăng ký rủi ro an toàn bộ nhớ (Risk Register) cập nhật.
