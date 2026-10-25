# ẢNH MINH CHỨNG CHẠY ADDRESSSANITIZER (ASan / UBSan)
Thư mục này dành riêng để lưu ảnh chụp màn hình terminal khi biên dịch và chạy kiểm toán bằng Clang/GCC Sanitizer:
1. `asan_stack_buffer_overflow.png` - Ảnh crash lỗi tràn Stack (Module 1 - Thọ / CWE-120)
2. `asan_heap_use_after_free.png`   - Ảnh crash lỗi UAF hoặc leak (Module 2 - Trang / CWE-401, 416)
3. `ubsan_integer_overflow.png`      - Ảnh bắt lỗi tràn số nguyên có dấu (Module 3 - Đô / CWE-190)

*Lưu ý: Các ảnh giao diện kiểm thử trên công cụ của Thầy (sec_lab_framework) được lưu riêng tại thư mục anh_chay_tool_thay/*
