#include "../include/validation.h"
#include <string.h>
#include <ctype.h>
#include <limits.h>

/**
 * 1. Tính năng: Kiểm tra mã sản phẩm
 */
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
