# BÁO CÁO KIỂM TOÁN AN TOÀN BỘ NHỚ ĐỘNG (MEMORY AUDIT REPORT)

## 1. MỤC TIÊU VÀ CÔNG CỤ THỰC HIỆN
- **Mục tiêu:** Đo lường và xác minh hành vi quản lý bộ nhớ heap, rò rỉ (memory leak), giải phóng đúp (double-free), và tràn vùng đệm (buffer overflow) tại thời điểm thực thi (runtime).
- **Công cụ sử dụng:** `Canary Memory Tracker` tích hợp trực tiếp qua hook `malloc / realloc / calloc / free` tại `verification/step3/sanitizers/`.
- **Ghi nhận môi trường:** Do môi trường Windows MSYS2 hiện tại chưa hỗ trợ ASan/LSan/Valgrind native, nhóm sử dụng bộ Canary Tracker nội bộ được thiết kế đặc thù cho việc phát hiện leak và canary byte header/footer.

## 2. KẾT QUẢ KIỂM TOÁN TRÊN BASELINE V0
Dựa trên raw log tại `results/sanitizers/canary_audit.log`:
- **Tổng số lần cấp phát:** 4 lần.
- **Tổng số lần giải phóng:** 4 lần.
- **Tổng dung lượng bộ nhớ đã phân bổ:** 26,624 bytes.
- **Dung lượng đỉnh điểm (Peak RAM):** 19,968 bytes.
- **Số khối bộ nhớ rò rỉ (Memory Leaks):** **0 bytes (0 khối)**.
- **Số lần Double-Free:** **0 lần**.
- **Số lần vi phạm tràn Canary:** **0 lần**.

## 3. ĐÁNH GIÁ 10 VẤN ĐỀ BẮT BUỘC THEO QUY CHUẨN
1. **Stack-buffer-overflow:** Không phát hiện trên Baseline v0 (nhờ guard `token_count < 8` và `fgets` có giới hạn 512 bytes).
2. **Heap-buffer-overflow:** Không phát hiện (mảng động luôn mở rộng trước khi ghi `count < capacity`).
3. **Out-of-bounds read/write:** Không phát hiện qua các vòng lặp kiểm soát chỉ số.
4. **Null pointer dereference:** 100% API đều có guard kiểm tra con trỏ NULL đầu vào.
5. **Use-after-free:** Được phòng vệ theo hợp đồng mượn con trỏ (REQ-14); chỉ xuất hiện trên bản mô phỏng đối chứng `tests/seeded/seeded_use_after_free.c`.
6. **Double-free:** Phòng vệ thành công nhờ cơ chế gán con trỏ về `NULL` ngay sau khi `free()`.
7. **Integer overflow:** Chặn thành công tại nhập kho (`INT_MAX`) và xác thực số nguyên trong parser.
8. **Allocation failure (OOM):** Khi `realloc` trả về `NULL`, con trỏ cũ được bảo toàn $100\%$, không bị leak.
9. **Memory leak:** 0 byte rò rỉ sau khi kết thúc chu trình ứng dụng.
10. **Resource cleanup:** Đóng file và giải phóng bộ nhớ đầy đủ theo vòng đời.
