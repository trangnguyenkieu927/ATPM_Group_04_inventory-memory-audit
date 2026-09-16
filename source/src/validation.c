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

// 3. Tính năng: Kiểm tra số lượng
int validate_quantity(int quantity) {
    if (quantity <= 0) {
        return STATUS_ERR_INVALID_QTY;
    }
    return STATUS_SUCCESS;
}

int validate_export_quantity(int current_stock, int export_quantity) {
    int qty_status = validate_quantity(export_quantity);
    if (qty_status != STATUS_SUCCESS) {
        return qty_status;
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
