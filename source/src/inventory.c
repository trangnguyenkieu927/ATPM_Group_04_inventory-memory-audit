#include "../include/inventory.h"
#include "../include/validation.h"

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

static int inventory_grow(Inventory *inventory) {
    size_t new_capacity;
    Product *new_products;

    if (inventory->count < inventory->capacity) {
        return STATUS_SUCCESS;
    }

    new_capacity = inventory->capacity == 0 ? INITIAL_CAPACITY : inventory->capacity * 2;
    if (new_capacity < inventory->capacity || new_capacity > SIZE_MAX / sizeof(*new_products)) {
        return STATUS_ERR_MEMORY;
    }

    new_products = realloc(inventory->products, new_capacity * sizeof(*new_products));
    if (new_products == NULL) {
        return STATUS_ERR_MEMORY;
    }

    inventory->products = new_products;
    inventory->capacity = new_capacity;
    return STATUS_SUCCESS;
}

int inventory_init(Inventory *inventory) {
    if (inventory == NULL) {
        return STATUS_ERR_NULL_PTR;
    }

    inventory->products = NULL;
    inventory->count = 0;
    inventory->capacity = 0;
    return STATUS_SUCCESS;
}

void inventory_free(Inventory *inventory) {
    if (inventory == NULL) {
        return;
    }

    free(inventory->products);
    inventory->products = NULL;
    inventory->count = 0;
    inventory->capacity = 0;
}

const Product *find_product(const Inventory *inventory, const char *product_id) {
    size_t i;

    if (inventory == NULL || product_id == NULL) {
        return NULL;
    }

    for (i = 0; i < inventory->count; ++i) {
        if (strcmp(inventory->products[i].id, product_id) == 0) {
            return &inventory->products[i];
        }
    }
    return NULL;
}

Product *find_product_mutable(Inventory *inventory, const char *product_id) {
    size_t i;

    if (inventory == NULL || product_id == NULL) {
        return NULL;
    }

    for (i = 0; i < inventory->count; ++i) {
        if (strcmp(inventory->products[i].id, product_id) == 0) {
            return &inventory->products[i];
        }
    }
    return NULL;
}

int inventory_add_product(Inventory *inventory, const Product *product) {
    int status;

    if (inventory == NULL || product == NULL) {
        return STATUS_ERR_NULL_PTR;
    }
    if (validate_product_id(product->id) != STATUS_SUCCESS || product->name[0] == '\0' ||
        strnlen(product->name, MAX_NAME_LEN) == MAX_NAME_LEN || product->quantity < 0 ||
        product->price < 0 || !isfinite(product->price)) {
        return STATUS_ERR_INVALID_ID;
    }
    if (find_product(inventory, product->id) != NULL) {
        return STATUS_ERR_INVALID_ID;
    }

    status = inventory_grow(inventory);
    if (status != STATUS_SUCCESS) {
        return status;
    }
    inventory->products[inventory->count++] = *product;
    return STATUS_SUCCESS;
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
