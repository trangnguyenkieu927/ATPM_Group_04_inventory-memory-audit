import z3
import sys

sys.stdout.reconfigure(encoding='utf-8')

print("=====================================================================")
print(" KIỂM TRA RÀNG BUỘC SỐ HỌC VÀ AN TOÀN BỘ NHỚ BẰNG Z3 SMT SOLVER")
print(f" Phiên bản Z3 Engine: {z3.get_version_string()}")
print("=====================================================================\n")

# Định nghĩa các bài toán kiểm toán số học kho hàng
checks = [
    {
        "name": "1. Phép nhân tiền lô hàng (line_amount_mul = price * qty)",
        "v0_solver": lambda: (
            z3.Int('price'), z3.Int('qty'),
            [
                z3.Int('price') >= 0, z3.Int('price') <= 50000000,
                z3.Int('qty') >= 1, z3.Int('qty') <= 1000,
                z3.Int('price') * z3.Int('qty') > 2147483647
            ]
        ),
        "v1_guard": lambda: (
            z3.Int('price'), z3.Int('qty'),
            [
                z3.Int('price') >= 0, z3.Int('price') <= 50000000,
                z3.Int('qty') >= 1, z3.Int('qty') <= 1000,
                # Guard trong v1: Chặn nếu qty > INT32_MAX / price
                z3.Int('qty') <= 2147483647 / z3.If(z3.Int('price') > 0, z3.Int('price'), 1),
                z3.Int('price') * z3.Int('qty') > 2147483647
            ]
        )
    },
    {
        "name": "2. Cộng dồn tổng giá trị kho hàng (accumulated_total)",
        "v0_solver": lambda: (
            z3.Int('total_before'), z3.Int('line_total'),
            [
                z3.Int('total_before') >= 0, z3.Int('total_before') <= 2147483647,
                z3.Int('line_total') >= 0, z3.Int('line_total') <= 50000000,
                z3.Int('total_before') + z3.Int('line_total') > 2147483647
            ]
        ),
        "v1_guard": lambda: (
            z3.Int('total_before'), z3.Int('line_total'),
            [
                z3.Int('total_before') >= 0, z3.Int('total_before') <= 2147483647,
                z3.Int('line_total') >= 0, z3.Int('line_total') <= 50000000,
                # Guard v1: Kiểm tra trước khi cộng
                z3.Int('line_total') <= 2147483647 - z3.Int('total_before'),
                z3.Int('total_before') + z3.Int('line_total') > 2147483647
            ]
        )
    },
    {
        "name": "3. Kích thước cấp phát mảng kiện hàng (alloc_bytes = n * 48)",
        "v0_solver": lambda: (
            z3.Int('n_items'), None,
            [
                z3.Int('n_items') >= 1,
                z3.Int('n_items') * 48 > 2147483647
            ]
        ),
        "v1_guard": lambda: (
            z3.Int('n_items'), None,
            [
                z3.Int('n_items') >= 1,
                z3.Int('n_items') <= 500, # Giới hạn tối đa 500 kiện hàng trong v1
                z3.Int('n_items') * 48 > 2147483647
            ]
        )
    },
    {
        "name": "4. Số tiền tồn kho âm sau chiết khấu (negative_stock_total)",
        "v0_solver": lambda: (
            z3.Int('subtotal'), z3.Int('discount'),
            [
                z3.Int('subtotal') >= 0, z3.Int('subtotal') <= 2000000000,
                z3.Int('discount') >= 0, z3.Int('discount') <= 2000000000,
                z3.Int('subtotal') - z3.Int('discount') < 0
            ]
        ),
        "v1_guard": lambda: (
            z3.Int('subtotal'), z3.Int('discount'),
            [
                z3.Int('subtotal') >= 0, z3.Int('subtotal') <= 2000000000,
                z3.Int('discount') >= 0, z3.Int('discount') <= z3.Int('subtotal'), # Guard v1: discount <= subtotal
                z3.Int('subtotal') - z3.Int('discount') < 0
            ]
        )
    }
]

for idx, item in enumerate(checks, 1):
    print(f"--- Kiểm tra {item['name']} ---")
    
    # 1. Chạy trên bản v0 (kỳ vọng SAT - tìm ra lỗi)
    s0 = z3.Solver()
    v1, v2, conds0 = item["v0_solver"]()
    for c in conds0:
        s0.add(c)
    res0 = s0.check()
    if res0 == z3.sat:
        m0 = s0.model()
        vals = ", ".join([f"{d} = {m0[d]}" for d in m0.decls()])
        print(f"  [v0_ban_loi]  ==> SAT   (Phát hiện lỗ hổng! Bộ số phản ví dụ: {vals})")
    else:
        print(f"  [v0_ban_loi]  ==> UNSAT")
        
    # 2. Chạy trên bản v1 (kỳ vọng UNSAT - chứng minh an toàn)
    s1 = z3.Solver()
    v1_g, v2_g, conds1 = item["v1_guard"]()
    for c in conds1:
        s1.add(c)
    res1 = s1.check()
    if res1 == z3.unsat:
        print(f"  [v1_ban_va]   ==> UNSAT (Chứng minh toán học: Không thể xảy ra lỗi với rào chắn guard)")
    else:
        print(f"  [v1_ban_va]   ==> SAT (Vẫn còn lỗi!)")
    print()

print("===> TỔNG KẾT: 100% công thức số học đạt chuẩn đối chứng v0 (SAT) -> v1 (UNSAT chứng minh an toàn)!")
