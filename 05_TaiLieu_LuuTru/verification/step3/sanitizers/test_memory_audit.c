#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memory_tracker.h"

#define malloc(s) mem_tracker_malloc(s, __FILE__, __LINE__)
#define realloc(p, s) mem_tracker_realloc(p, s, __FILE__, __LINE__)
#define calloc(n, s) mem_tracker_calloc(n, s, __FILE__, __LINE__)
#define free(p) mem_tracker_free(p, __FILE__, __LINE__)

#include "../../source/src/product.c"
#include "../../source/src/inventory.c"
#include "../../source/src/report.c"
#include "../../source/src/file_io.c"
#include "../../source/src/validation.c"
#include "../../source/src/utils.c"

#undef malloc
#undef realloc
#undef calloc
#undef free

int main(void) {
    mem_tracker_init();
    printf("[RUNNING] Bắt đầu kiểm tra rò rỉ bộ nhớ động và bảo vệ Canary trên Baseline v0...\n");

    /* TEST 1: Thao tác vòng đời mảng động ProductList với 50 sản phẩm (kích hoạt realloc 16 -> 32 -> 64) */
    {
        ProductList list;
        int status = product_list_init(&list);
        if (status != STATUS_SUCCESS) return 1;

        for (int i = 0; i < 50; ++i) {
            Product p;
            memset(&p, 0, sizeof(p));
            snprintf(p.id, sizeof(p.id), "P%03d", i + 1);
            snprintf(p.name, sizeof(p.name), "Item_%d", i + 1);
            snprintf(p.category, sizeof(p.category), "Cat_%d", i % 5);
            snprintf(p.unit, sizeof(p.unit), "Cai");
            p.quantity = 100 + i;
            p.price = 25.5 + (double)i;

            status = add_product(&list, &p);
            if (status != STATUS_SUCCESS) {
                fprintf(stderr, "add_product failed at %d: %d\n", i, status);
                return 1;
            }
        }

        /* Xóa 10 sản phẩm */
        for (int i = 0; i < 10; ++i) {
            char id[32];
            snprintf(id, sizeof(id), "P%03d", i * 5 + 1);
            delete_product(&list, id);
        }

        /* Thống kê báo cáo danh mục */
        CategorySummary summaries[32];
        size_t summary_count = 0;
        calculate_category_summaries(&list, summaries, 32, &summary_count);

        product_list_free(&list);
    }

    /* TEST 2: Đọc và ghi tệp thông qua file_io */
    {
        Inventory inv;
        inventory_init(&inv);

        /* Nạp dữ liệu từ source/data/products.txt nếu có */
        load_products_from_file(&inv, "source/data/products.txt");

        /* Ghi ra tệp tạm */
        save_products_to_file(&inv, "scratch/temp_products_audit.txt");

        inventory_free(&inv);
    }

    /* Báo cáo kết quả kiểm tra rò rỉ */
    int report_status = mem_tracker_report();
    if (report_status == 0) {
        printf("[SUCCESS] Bộ kiểm tra động hoàn thành an toàn: 0 rò rỉ, 0 tràn buffer canary!\n");
        return 0;
    } else {
        printf("[FAILED] Phát hiện vi phạm bộ nhớ hoặc rò rỉ!\n");
        return 1;
    }
}
