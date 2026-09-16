#ifndef INVENTORY_H
#define INVENTORY_H

#include <stddef.h>

#include "models.h"

/* Danh sach san pham duoc cap phat dong va so huu bo nho cua products. */
typedef struct {
    Product *products;
    size_t count;
    size_t capacity;
} Inventory;

int inventory_init(Inventory *inventory);
void inventory_free(Inventory *inventory);

const Product *find_product(const Inventory *inventory, const char *product_id);
Product *find_product_mutable(Inventory *inventory, const char *product_id);
int inventory_add_product(Inventory *inventory, const Product *product);

int import_stock(Inventory *inventory, const char *product_id, int quantity);
int export_stock(Inventory *inventory, const char *product_id, int quantity);

/* Dat ton kho chinh xac; so luong 0 la gia tri hop le. */
int update_stock(Inventory *inventory, const char *product_id, int new_quantity);

#endif /* INVENTORY_H */
