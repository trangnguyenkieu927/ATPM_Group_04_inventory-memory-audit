#include "../include/validation.h"
#include <string.h>
#include <ctype.h>
#include <limits.h>

static int is_all_whitespace(const char *str) {
    if (str == NULL) {
        return 1;
    }
    while (*str) {
        if (!isspace((unsigned char)*str)) {
            return 0;
        }
        str++;
    }
    return 1;
}

// 1. Tính năng: Kiểm tra mã sản phẩm
int validate_product_id(const char *product_id) {
    if (product_id == NULL) {
        return STATUS_ERR_NULL_PTR;
    }

    size_t len = strlen(product_id);
    if (len == 0 || len >= MAX_ID_LEN) {
        return STATUS_ERR_INVALID_ID;
    }

    for (size_t i = 0; i < len; ++i) {
        char c = product_id[i];
        if (iscntrl((unsigned char)c) || isspace((unsigned char)c) || c == '|' || c == ',' || c == ';') {
            return STATUS_ERR_INVALID_ID;
        }
    }

    return STATUS_SUCCESS;
}

int validate_id(const char *id) {
    return validate_product_id(id);
}

// 2. Tính năng: Kiểm tra thông tin sản phẩm
int validate_product_name(const char *name) {
    if (name == NULL) {
        return STATUS_ERR_NULL_PTR;
    }

    size_t len = strlen(name);
    if (len == 0 || len >= MAX_NAME_LEN) {
        return STATUS_ERR_INVALID_NAME;
    }

    if (is_all_whitespace(name)) {
        return STATUS_ERR_INVALID_NAME;
    }

    for (size_t i = 0; i < len; ++i) {
        char c = name[i];
        if (iscntrl((unsigned char)c) || c == '|' || c == '\n' || c == '\r') {
            return STATUS_ERR_INVALID_NAME;
        }
    }

    return STATUS_SUCCESS;
}

int validate_category(const char *category) {
    if (category == NULL) {
        return STATUS_ERR_NULL_PTR;
    }

    size_t len = strlen(category);
    if (len == 0 || len >= MAX_CATEGORY_LEN) {
        return STATUS_ERR_INVALID_CATEGORY;
    }

    if (is_all_whitespace(category)) {
        return STATUS_ERR_INVALID_CATEGORY;
    }

    for (size_t i = 0; i < len; ++i) {
        char c = category[i];
        if (iscntrl((unsigned char)c) || c == '|' || c == '\n' || c == '\r') {
            return STATUS_ERR_INVALID_CATEGORY;
        }
    }

    return STATUS_SUCCESS;
}

int validate_unit(const char *unit) {
    if (unit == NULL) {
        return STATUS_ERR_NULL_PTR;
    }

    size_t len = strlen(unit);
    if (len == 0 || len >= MAX_UNIT_LEN) {
        return STATUS_ERR_INVALID_UNIT;
    }

    if (is_all_whitespace(unit)) {
        return STATUS_ERR_INVALID_UNIT;
    }

    for (size_t i = 0; i < len; ++i) {
        char c = unit[i];
        if (iscntrl((unsigned char)c) || c == '|' || c == '\n' || c == '\r') {
            return STATUS_ERR_INVALID_UNIT;
        }
    }

    return STATUS_SUCCESS;
}

int validate_name_unit(const char *name, const char *unit) {
    int status = validate_product_name(name);
    if (status != STATUS_SUCCESS) {
        return status;
    }

    return validate_unit(unit);
}

int validate_product_info(const char *name, const char *category, const char *unit) {
    int status = validate_name_unit(name, unit);
    if (status != STATUS_SUCCESS) {
        return status;
    }

    if (category != NULL) {
        status = validate_category(category);
        if (status != STATUS_SUCCESS) {
            return status;
        }
    }

    return STATUS_SUCCESS;
}

// 3. Tính năng: Kiểm tra giá và số lượng
int validate_price(double price) {
    if (price < 0.0) {
        return STATUS_ERR_INVALID_PRICE;
    }
    return STATUS_SUCCESS;
}

int validate_quantity(int quantity) {
    if (quantity < 0) {
        return STATUS_ERR_INVALID_QTY;
    }
    return STATUS_SUCCESS;
}

int validate_quantity_str(const char *qty_str) {
    if (qty_str == NULL) {
        return STATUS_ERR_NULL_PTR;
    }

    while (isspace((unsigned char)*qty_str)) {
        qty_str++;
    }

    if (*qty_str == '\0') {
        return STATUS_ERR_INVALID_QTY;
    }

    // Chặn số âm
    if (*qty_str == '-') {
        return STATUS_ERR_INVALID_QTY;
    }

    if (*qty_str == '+') {
        qty_str++;
    }

    if (*qty_str == '\0') {
        return STATUS_ERR_INVALID_QTY;
    }

    long long val = 0;
    const char *p = qty_str;
    while (*p && !isspace((unsigned char)*p)) {
        if (!isdigit((unsigned char)*p)) {
            return STATUS_ERR_INVALID_QTY; // Chặn sai định dạng như "aaa", "qưqw", "12a", "3.5"
        }
        val = val * 10 + (*p - '0');
        if (val > INT_MAX) {
            return STATUS_ERR_OVERFLOW;
        }
        p++;
    }

    while (*p) {
        if (!isspace((unsigned char)*p)) {
            return STATUS_ERR_INVALID_QTY;
        }
        p++;
    }

    return STATUS_SUCCESS;
}

int validate_price_str(const char *price_str) {
    if (price_str == NULL) {
        return STATUS_ERR_NULL_PTR;
    }

    while (isspace((unsigned char)*price_str)) {
        price_str++;
    }

    if (*price_str == '\0') {
        return STATUS_ERR_INVALID_PRICE;
    }

    // Chặn số âm
    if (*price_str == '-') {
        return STATUS_ERR_INVALID_PRICE;
    }

    if (*price_str == '+') {
        price_str++;
    }

    if (*price_str == '\0') {
        return STATUS_ERR_INVALID_PRICE;
    }

    int has_digits = 0;
    int dot_count = 0;
    const char *p = price_str;

    while (*p && !isspace((unsigned char)*p)) {
        if (isdigit((unsigned char)*p)) {
            has_digits = 1;
        } else if (*p == '.') {
            dot_count++;
            if (dot_count > 1) {
                return STATUS_ERR_INVALID_PRICE;
            }
        } else {
            return STATUS_ERR_INVALID_PRICE;
        }
        p++;
    }

    if (!has_digits) {
        return STATUS_ERR_INVALID_PRICE;
    }

    while (*p) {
        if (!isspace((unsigned char)*p)) {
            return STATUS_ERR_INVALID_PRICE;
        }
        p++;
    }

    return STATUS_SUCCESS;
}

int validate_price_quantity(const char *price_str, const char *qty_str) {
    int price_st = validate_price_str(price_str);
    if (price_st != STATUS_SUCCESS) {
        return price_st;
    }

    int qty_st = validate_quantity_str(qty_str);
    if (qty_st != STATUS_SUCCESS) {
        return qty_st;
    }

    return STATUS_SUCCESS;
}

int validate_price_quantity_values(double price, int quantity) {
    int price_st = validate_price(price);
    if (price_st != STATUS_SUCCESS) {
        return price_st;
    }

    int qty_st = validate_quantity(quantity);
    if (qty_st != STATUS_SUCCESS) {
        return qty_st;
    }

    return STATUS_SUCCESS;
}

int validate_export_quantity(int current_stock, int export_quantity) {
    if (export_quantity <= 0) {
        return STATUS_ERR_INVALID_QTY;
    }

    if (current_stock < 0 || export_quantity > current_stock) {
        return STATUS_ERR_INSUFFICIENT_STOCK;
    }

    return STATUS_SUCCESS;
}

int check_addition_overflow(int current_stock, int add_quantity) {
    if (add_quantity <= 0) {
        return STATUS_ERR_INVALID_QTY;
    }

    if (current_stock < 0) {
        return STATUS_ERR_INSUFFICIENT_STOCK;
    }

    if (current_stock > INT_MAX - add_quantity) {
        return STATUS_ERR_OVERFLOW;
    }

    return STATUS_SUCCESS;
}

