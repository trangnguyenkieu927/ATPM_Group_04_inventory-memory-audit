#ifndef VALIDATION_H
#define VALIDATION_H

#include "models.h"

/**
 * 1. Tính năng: Kiểm tra mã sản phẩm
 * - Không được NULL.
 * - Độ dài > 0 và < MAX_ID_LEN.
 * - Không chứa ký tự đặc biệt, ký tự điều khiển, khoảng trắng hoặc ký tự phân cách (|, ,, ;).
 */
int validate_product_id(const char *product_id);
int validate_id(const char *id);

/**
 * Kiểm tra tính hợp lệ của số lượng:
 * - Phải lớn hơn 0 (quantity > 0).
 */
int validate_quantity(int quantity);

/**
 * Kiểm tra số lượng xuất kho so với tồn kho:
 * - export_quantity > 0.
 * - export_quantity <= current_stock (đảm bảo tồn kho không âm).
 */
int validate_export_quantity(int current_stock, int export_quantity);

/**
 * Kiểm tra nguy cơ tràn số nguyên (Integer Overflow) khi nhập kho:
 */
int check_addition_overflow(int current_stock, int add_quantity);

#endif /* VALIDATION_H */
