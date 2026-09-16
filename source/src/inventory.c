#include "../include/inventory.h"
#include "../include/product.h"
#include "../include/validation.h"

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

int inventory_init(Inventory *inventory) {
    return product_list_init(inventory);
}

void inventory_free(Inventory *inventory) {
    product_list_free(inventory);
}

const Product *find_product(const Inventory *inventory, const char *product_id) {
    return find_product_by_id(inventory, product_id);
}

Product *find_product_mutable(Inventory *inventory, const char *product_id) {
    return find_product_by_id(inventory, product_id);
}

int inventory_add_product(Inventory *inventory, const Product *product) {
    return add_product(inventory, product);
}

int import_stock(Inventory *inventory, const char *product_id, int quantity) {
    Product *product;
    int status;

    if (inventory == NULL || product_id == NULL) {
        return STATUS_ERR_NULL_PTR;
    }
    product = find_product_mutable(inventory, product_id);
    if (product == NULL) {
        return STATUS_ERR_NOT_FOUND;
    }
    status = check_addition_overflow(product->quantity, quantity);
    if (status != STATUS_SUCCESS) {
        return status;
    }
    product->quantity += quantity;
    return STATUS_SUCCESS;
}

int export_stock(Inventory *inventory, const char *product_id, int quantity) {
    Product *product;
    int status;

    if (inventory == NULL || product_id == NULL) {
        return STATUS_ERR_NULL_PTR;
    }
    product = find_product_mutable(inventory, product_id);
    if (product == NULL) {
        return STATUS_ERR_NOT_FOUND;
    }
    status = validate_export_quantity(product->quantity, quantity);
    if (status != STATUS_SUCCESS) {
        return status;
    }
    product->quantity -= quantity;
    return STATUS_SUCCESS;
}

int update_stock(Inventory *inventory, const char *product_id, int new_quantity) {
    Product *product;

    if (inventory == NULL || product_id == NULL) {
        return STATUS_ERR_NULL_PTR;
    }
    if (new_quantity < 0) {
        return STATUS_ERR_INVALID_QTY;
    }
    product = find_product_mutable(inventory, product_id);
    if (product == NULL) {
        return STATUS_ERR_NOT_FOUND;
    }
    product->quantity = new_quantity;
    return STATUS_SUCCESS;
}
