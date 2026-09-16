#ifndef VALIDATION_H
#define VALIDATION_H

#include "models.h"

// 1. Tính năng: Kiểm tra mã sản phẩm
int validate_product_id(const char *product_id);
int validate_id(const char *id);

// 2. Tính năng: Kiểm tra thông tin sản phẩm
int validate_product_name(const char *name);
int validate_category(const char *category);
int validate_unit(const char *unit);
int validate_name_unit(const char *name, const char *unit);
int validate_product_info(const char *name, const char *category, const char *unit);

// 3. Tính năng: Kiểm tra số lượng
int validate_quantity(int quantity);
// Kiểm tra số lượng xuất kho so với tồn kho:
int validate_export_quantity(int current_stock, int export_quantity);
// Kiểm tra nguy cơ tràn số nguyên (Integer Overflow) khi nhập kho:
int check_addition_overflow(int current_stock, int add_quantity);

#endif 
