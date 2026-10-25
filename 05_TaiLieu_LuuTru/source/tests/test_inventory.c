#include "../include/file_io.h"
#include "../include/inventory.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

static Product make_product(const char *id, const char *name, int quantity, double price) {
    Product product = {0};
    snprintf(product.id, sizeof(product.id), "%s", id);
    snprintf(product.name, sizeof(product.name), "%s", name);
    snprintf(product.category, sizeof(product.category), "DienTu");
    snprintf(product.unit, sizeof(product.unit), "Cai");
    product.quantity = quantity;
    product.price = price;
    return product;
}

int main(void) {
    Inventory inventory;
    Inventory reloaded;
    Product laptop = make_product("SP001", "Laptop", 10, 1000.0);
    const char *output_path = "data/test_products_output.txt";

    assert(inventory_init(&inventory) == STATUS_SUCCESS);
    assert(inventory_add_product(&inventory, &laptop) == STATUS_SUCCESS);
    assert(import_stock(&inventory, "SP001", 5) == STATUS_SUCCESS);
    assert(find_product(&inventory, "SP001")->quantity == 15);
    assert(export_stock(&inventory, "SP001", 6) == STATUS_SUCCESS);
    assert(find_product(&inventory, "SP001")->quantity == 9);
    assert(export_stock(&inventory, "SP001", 10) == STATUS_ERR_INSUFFICIENT_STOCK);
    assert(update_stock(&inventory, "SP001", 0) == STATUS_SUCCESS);
    assert(find_product(&inventory, "SP001")->quantity == 0);
    assert(import_stock(&inventory, "MISSING", 1) == STATUS_ERR_NOT_FOUND);
    assert(save_products_to_file(&inventory, output_path) == STATUS_SUCCESS);

    assert(inventory_init(&reloaded) == STATUS_SUCCESS);
    assert(load_products_from_file(&reloaded, output_path) == STATUS_SUCCESS);
    assert(reloaded.count == 1);
    assert(strcmp(reloaded.items[0].id, "SP001") == 0);
    assert(reloaded.items[0].quantity == 0);

    inventory_free(&reloaded);
    inventory_free(&inventory);
    remove(output_path);
    puts("inventory tests passed");
    return 0;
}
