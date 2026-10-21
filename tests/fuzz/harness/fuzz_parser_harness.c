#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "models.h"
#include "inventory.h"
#include "file_io.h"

// Fuzz harness nhận dữ liệu qua file tạm hoặc stdin
int main(int argc, char *argv[]) {
    if (argc < 2) {
        return 1;
    }

    const char *target_file = argv[1];
    Inventory inv;
    if (inventory_init(&inv) != STATUS_SUCCESS) {
        return 2;
    }

    // Thực thi API parser trên file đột biến (mutated input)
    int rc = load_products_from_file(&inv, target_file);
    
    // Cleanup an toàn
    inventory_free(&inv);
    
    // rc có thể là STATUS_SUCCESS (0) hoặc STATUS_ERR_* (< 0). Cả 2 đều hợp lệ và không được crash.
    return 0;
}
