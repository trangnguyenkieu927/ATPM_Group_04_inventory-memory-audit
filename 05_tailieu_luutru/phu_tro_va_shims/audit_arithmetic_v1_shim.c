/* Tệp shim SINH TỰ ĐỘNG bởi sec_lab_framework (core/c_wrapper.py) — KHÔNG sửa tay.
   Nó #include mã nguồn gốc rồi xuất các hàm sl_wrap_<tên> có C linkage để Python (ctypes)
   gọi được — kể cả hàm `static` và `main` (được đổi tên thành sl_orig_main). */
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
#define SL_EXTC extern "C"
#else
#define SL_EXTC
#endif
#ifdef _WIN32
#define SL_EXPORT SL_EXTC __declspec(dllexport)
#else
#define SL_EXPORT SL_EXTC __attribute__((visibility("default")))
#endif
#ifdef __cplusplus
extern "C" { unsigned char sl_cov_map[1]; }
#else
unsigned char sl_cov_map[1];
#endif
#define SL_HIT(n) (sl_cov_map[(n)] = 1)
#define main sl_orig_main
#include "audit_arithmetic_v1.c"
#undef main
SL_EXPORT unsigned char *sl_cov_ptr(void) { return sl_cov_map; }
SL_EXPORT int sl_cov_size(void) { return 1; }
SL_EXPORT void sl_cov_reset(void) { for (int i = 0; i < 1; i++) sl_cov_map[i] = 0; }

SL_EXPORT int sl_wrap_update_stock_quantity_v1(int current_stock, int added_quantity) {
    return update_stock_quantity_v1(current_stock, added_quantity);
}
SL_EXPORT int sl_wrap_scale_product_price_v1(int base_price, int rate) {
    return scale_product_price_v1(base_price, rate);
}
