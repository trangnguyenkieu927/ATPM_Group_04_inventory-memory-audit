# BÁO CÁO ĐẶC TẢ AN TOÀN VÀ ÁNH XẠ 18 REQ (STAGE 4 REPORT)

## 1. NGUYÊN TẮC BẢO TOÀN VÀ CHUẨN HÓA 18 REQ
Theo yêu cầu bắt buộc của Giảng viên và Mục IV/IV của Master Prompt:
- **100% nội dung REQ-1 đến REQ-18 được giữ nguyên văn** từ tài liệu chuẩn Bước 2 (`Spec_Inventory_Memory_Safety_Audit_18REQ_CHUAN.xlsx` và `requirement_specifications.md`).
- Tuyệt đối không thay đổi mã định danh hoặc ý nghĩa kỹ thuật của từng yêu cầu.
- Toàn bộ 18 yêu cầu được phân bổ trực tiếp vào 6 kịch bản rủi ro (SC-01 đến SC-06).

## 2. PHÂN TÍCH TÌNH TRẠNG GUARD TRÊN BASELINE V0
Qua đối chiếu trực tiếp với mã nguồn C thực tế tại commit baseline `44202070c68002d9af068374478cdc83098dd4d8`, tình trạng kiểm soát an toàn được phân loại trung thực như sau:

| Nhóm trạng thái trên Baseline v0 | Số lượng | Danh sách REQ | Đánh giá kỹ thuật và Ánh xạ Rủi ro CWE |
|---|---|---|---|
| **Phòng vệ cơ bản (GUARDED_IN_V0)** | 12 / 18 | REQ-1, REQ-4, REQ-5, REQ-6, REQ-7, REQ-8, REQ-11, REQ-12, REQ-13, REQ-15, REQ-17, REQ-18 | Mã nguồn baseline đã có kiểm tra cơ bản: chặn số lượng âm, chặn giá âm, gán NULL sau free, kiểm tra NULL pointer (`STATUS_ERR_NULL_PTR (-1)`). |
| **Tiềm ẩn rủi ro Parser/Buffer (POTENTIAL_RISK_IN_V0)** | 3 / 18 | **REQ-9, REQ-10, REQ-16** | **Rủi ro bộ nhớ tiềm ẩn:** Baseline v0 dùng `tokens[6]` thiếu guard chặn `token_count < 6` (CWE-121 Stack Overflow); thiếu cơ chế drain dòng > 512B (CWE-120 Silent Truncation); dùng `strncpy` không đảm bảo null-terminated (CWE-125). Đã được khắc phục triệt để trong bản vá v1. |
| **Bảo vệ theo hợp đồng (GUARDED_BY_CONTRACT)** | 1 / 18 | **REQ-14** | Con trỏ mượn `find_product_by_id` trỏ trực tiếp vào heap mảng động; tiềm ẩn rủi ro Use-After-Free (CWE-416) khi xảy ra heap resize (realloc). |
| **Thiếu guard tràn số nguyên (GUARD_MISSING_IN_V0)** | 2 / 18 | **REQ-2, REQ-3** | **Rủi ro tràn số cấp phát bộ nhớ:** Baseline v0 thực hiện `new_cap = capacity * 2` và `new_cap * sizeof(Product)` thiếu kiểm tra chặn `SIZE_MAX / 2` và `SIZE_MAX / sizeof(Product)`. Mô hình SMT Z3 chứng minh SAT wrap-around 64-bit (CWE-190). Đã được vá hoàn chỉnh trong v1. |

## 3. DANH MỤC CÁC TÀI LIỆU VÀ WORKBOOK ĐÃ TẠO
Tất cả các tài liệu chuẩn hóa đặt tại `docs/03_Safety_Specification/`:
1. `safety_requirements.xlsx`: Bảng đặc tả 18 REQ chuẩn với 13 trường thông tin chi tiết.
2. `formal_properties.xlsx`: Bảng ánh xạ các thuộc tính logic hình thức (CTL, SMT QF_BV, BMC Invariant).
3. `scenario_req_mapping.xlsx`: Ma trận truy vết 2 chiều giữa 6 Kịch bản rủi ro và 18 REQ.
4. `safety_specifications.json`: Tập tin JSON chuẩn hóa phục vụ các công cụ kiểm chứng tự động.
5. `risk_register_updated.xlsx`: Bảng đăng ký rủi ro an toàn bộ nhớ (Risk Register) cập nhật.
