#ifndef FILE_IO_H
#define FILE_IO_H

#include "inventory.h"

/* Dinh dang tep: ID|Name|Quantity|Price (mot san pham tren mot dong). */
int load_products_from_file(Inventory *inventory, const char *path);
int save_products_to_file(const Inventory *inventory, const char *path);

#endif /* FILE_IO_H */
