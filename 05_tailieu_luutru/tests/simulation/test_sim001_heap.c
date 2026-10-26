#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "../../source/include/models.h"
#include "../../source/include/product.h"

/* Biến cờ điều khiển fault injection */
static int g_inject_realloc_failure = 0;
static void *(*g_real_realloc)(void *, size_t) = realloc;

void *sim001_mock_realloc(void *ptr, size_t size) {
    if (g_inject_realloc_failure) {
        return NULL;
    }
    return g_real_realloc(ptr, size);
}

#define realloc sim001_mock_realloc

/* Include trực tiếp logic product.c để liên kết với mock allocator */
#include "../../source/src/product.c"

#undef realloc

/**
 * SIM-001: Mô phỏng Heap Allocation & Vòng đời con trỏ
 * Yêu cầu: REQ-2, REQ-3, REQ-12, REQ-14
 * Rủi ro: RISK-001, RISK-002, RISK-003, RISK-004
 */

int main(void) {
    printf("============================================================\n");
    printf("  SIM-001: MÔ PHỎNG VÒNG ĐỜI BỘ NHỚ HEAP VÀ CON TRỎ C       \n");
    printf("============================================================\n");

    /* -------------------------------------------------------------
     * THÍ NGHIỆM 1: Mở rộng mảng hợp lệ khi chạm ngưỡng capacity
     * ------------------------------------------------------------- */
    printf("\n--- THÍ NGHIỆM 1: Mở rộng mảng hợp lệ (Dynamic Array Growth) ---\n");
    ProductList list;
    int status = product_list_init(&list);
    if (status != STATUS_SUCCESS) {
        fprintf(stderr, "[THÍ NGHIỆM 1 THẤT BẠI] Không thể khởi tạo list\n");
        return 1;
    }

    printf("Trạng thái ban đầu: count = %zu, capacity = %zu, items = %p\n",
           list.count, list.capacity, (void *)list.items);

    /* Nạp 16 sản phẩm để làm đầy mảng */
    Product p;
    for (int i = 0; i < (int)INITIAL_CAPACITY; ++i) {
        memset(&p, 0, sizeof(p));
        snprintf(p.id, sizeof(p.id), "P%03d", i + 1);
        snprintf(p.name, sizeof(p.name), "Item_%d", i + 1);
        snprintf(p.category, sizeof(p.category), "IT");
        snprintf(p.unit, sizeof(p.unit), "Cai");
        p.quantity = 10;
        p.price = 50.0;
        status = add_product(&list, &p);
        if (status != STATUS_SUCCESS) {
            fprintf(stderr, "[THÍ NGHIỆM 1 THẤT BẠI] Thêm phần tử %d thất bại: %d\n", i, status);
            return 1;
        }
    }

    printf("Trạng thái khi đầy:  count = %zu, capacity = %zu, items = %p\n",
           list.count, list.capacity, (void *)list.items);

    void *items_before_growth = (void *)list.items;

    /* Thêm sản phẩm thứ 17 để kích hoạt mở rộng capacity 16 -> 32 */
    memset(&p, 0, sizeof(p));
    snprintf(p.id, sizeof(p.id), "P017");
    snprintf(p.name, sizeof(p.name), "Item_17");
    snprintf(p.category, sizeof(p.category), "IT");
    snprintf(p.unit, sizeof(p.unit), "Cai");
    p.quantity = 20;
    p.price = 60.0;
    status = add_product(&list, &p);

    if (status != STATUS_SUCCESS || list.capacity != 32 || list.count != 17) {
        fprintf(stderr, "[THÍ NGHIỆM 1 THẤT BẠI] Sai lệch trạng thái sau realloc: count=%zu, cap=%zu\n",
                list.count, list.capacity);
        return 1;
    }

    printf("Trạng thái sau tăng: count = %zu, capacity = %zu, items = %p (Trước: %p)\n",
           list.count, list.capacity, (void *)list.items, items_before_growth);
    printf("-> [ĐẠT] Bất biến count (%zu) <= capacity (%zu) được bảo toàn sau mở rộng.\n",
           list.count, list.capacity);

    product_list_free(&list);

    /* -------------------------------------------------------------
     * THÍ NGHIỆM 2: Mô phỏng realloc thất bại (Fault Injection OOM)
     * ------------------------------------------------------------- */
    printf("\n--- THÍ NGHIỆM 2: Mô phỏng Realloc thất bại (Fault Injection OOM) ---\n");
    status = product_list_init(&list);
    if (status != STATUS_SUCCESS) return 1;

    for (int i = 0; i < (int)INITIAL_CAPACITY; ++i) {
        memset(&p, 0, sizeof(p));
        snprintf(p.id, sizeof(p.id), "P%03d", i + 1);
        snprintf(p.name, sizeof(p.name), "Item_%d", i + 1);
        snprintf(p.category, sizeof(p.category), "IT");
        snprintf(p.unit, sizeof(p.unit), "Cai");
        p.quantity = 10;
        p.price = 50.0;
        add_product(&list, &p);
    }

    Product *old_items_ptr = list.items;
    size_t old_cap = list.capacity;
    size_t old_cnt = list.count;

    /* BẬT FAULT INJECTION */
    g_inject_realloc_failure = 1;
    printf("Kích hoạt Fault Injection: ép realloc() trả về NULL tại lần cấp phát tiếp theo...\n");

    memset(&p, 0, sizeof(p));
    snprintf(p.id, sizeof(p.id), "P017");
    snprintf(p.name, sizeof(p.name), "Item_17");
    snprintf(p.category, sizeof(p.category), "IT");
    snprintf(p.unit, sizeof(p.unit), "Cai");
    p.quantity = 20;
    p.price = 60.0;

    status = add_product(&list, &p);

    /* TẮT FAULT INJECTION */
    g_inject_realloc_failure = 0;

    printf("Kết quả gọi add_product khi OOM: status = %d (kỳ vọng STATUS_ERR_MEMORY = %d)\n",
           status, STATUS_ERR_MEMORY);
    printf("Con trỏ items: hiện tại = %p, trước khi OOM = %p\n",
           (void *)list.items, (void *)old_items_ptr);
    printf("Capacity:      hiện tại = %zu, trước khi OOM = %zu\n", list.capacity, old_cap);
    printf("Count:         hiện tại = %zu, trước khi OOM = %zu\n", list.count, old_cnt);

    if (status == STATUS_ERR_MEMORY && list.items == old_items_ptr &&
        list.capacity == old_cap && list.count == old_cnt) {
        printf("-> [ĐẠT] Con trỏ sở hữu cũ được bảo toàn 100%%; không bị rò rỉ hoặc ghi đè NULL.\n");
    } else {
        fprintf(stderr, "[THÍ NGHIỆM 2 THẤT BẠI] Vi phạm hợp đồng bảo toàn con trỏ khi OOM!\n");
        return 1;
    }

    product_list_free(&list);

    /* -------------------------------------------------------------
     * THÍ NGHIỆM 3: Hợp đồng Vô hiệu hóa Con trỏ mượn (Borrowed Pointer)
     * ------------------------------------------------------------- */
    printf("\n--- THÍ NGHIỆM 3: Vòng đời và Vô hiệu hóa Con trỏ Mượn (REQ-14) ---\n");
    status = product_list_init(&list);
    if (status != STATUS_SUCCESS) return 1;

    for (int i = 0; i < (int)INITIAL_CAPACITY; ++i) {
        memset(&p, 0, sizeof(p));
        snprintf(p.id, sizeof(p.id), "P%03d", i + 1);
        snprintf(p.name, sizeof(p.name), "Item_%d", i + 1);
        snprintf(p.category, sizeof(p.category), "IT");
        snprintf(p.unit, sizeof(p.unit), "Cai");
        p.quantity = 10;
        p.price = 50.0;
        add_product(&list, &p);
    }

    /* Mượn con trỏ tới phần tử P001 */
    Product *borrowed = find_product_by_id(&list, "P001");
    printf("Con trỏ mượn P001 trước khi mở rộng: %p\n", (void *)borrowed);

    /* Kích hoạt realloc tăng mảng */
    memset(&p, 0, sizeof(p));
    snprintf(p.id, sizeof(p.id), "P017");
    snprintf(p.name, sizeof(p.name), "Item_17");
    snprintf(p.category, sizeof(p.category), "IT");
    snprintf(p.unit, sizeof(p.unit), "Cai");
    p.quantity = 20;
    p.price = 60.0;
    add_product(&list, &p);

    /* Tái mượn con trỏ sau realloc */
    Product *re_borrowed = find_product_by_id(&list, "P001");
    printf("Con trỏ mượn P001 sau khi mở rộng:   %p\n", (void *)re_borrowed);

    if (re_borrowed != NULL && strcmp(re_borrowed->id, "P001") == 0) {
        printf("-> [ĐẠT] Quy tắc tái mượn (Re-borrowing Contract): Mã gọi phải luôn cập nhật lại con trỏ qua API sau khi mảng thay đổi dung lượng.\n");
    } else {
        fprintf(stderr, "[THÍ NGHIỆM 3 THẤT BẠI] Tái mượn con trỏ không hợp lệ!\n");
        return 1;
    }

    product_list_free(&list);

    printf("\n[KẾT QUẢ TỔNG HỢP] Toàn bộ 3/3 thí nghiệm SIM-001 HOÀN THÀNH ĐẠT YÊU CẦU.\n");
    return 0;
}
