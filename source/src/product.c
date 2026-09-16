#include "../include/product.h"
#include "../include/validation.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Khởi tạo danh sách sản phẩm động */
int product_list_init(ProductList *list) {
    if (list == NULL) {
        return STATUS_ERR_NULL_PTR;
    }

    list->items = (Product *)malloc(INITIAL_CAPACITY * sizeof(Product));
    if (list->items == NULL) {
        list->count = 0;
        list->capacity = 0;
        return STATUS_ERR_MEMORY;
    }

    list->count = 0;
    list->capacity = INITIAL_CAPACITY;
    return STATUS_SUCCESS;
}

/* Giải phóng bộ nhớ động của danh sách sản phẩm */
void product_list_free(ProductList *list) {
    if (list != NULL && list->items != NULL) {
        free(list->items);
        list->items = NULL;
        list->count = 0;
        list->capacity = 0;
    }
}

/* Hàm hỗ trợ sao chép chuỗi an toàn, luôn đảm bảo ký tự kết thúc '\0' */
static void safe_strcpy(char *dest, const char *src, size_t dest_size) {
    if (dest == NULL || dest_size == 0) return;
    if (src == NULL) {
        dest[0] = '\0';
        return;
    }
    snprintf(dest, dest_size, "%s", src);
}

/* TASK 3: Cài đặt chức năng Thêm sản phẩm vào danh sách động */
int add_product(ProductList *list, const Product *prod) {
    if (list == NULL || prod == NULL) {
        return STATUS_ERR_NULL_PTR;
    }

    /* 1. Kiểm tra tính hợp lệ của mã sản phẩm (ID) */
    int val_id = validate_product_id(prod->id);
    if (val_id != STATUS_SUCCESS) {
        return val_id;
    }

    /* 2. Kiểm tra tính hợp lệ của tên sản phẩm */
    if (prod->name[0] == '\0' || strlen(prod->name) >= MAX_NAME_LEN) {
        return STATUS_ERR_INVALID_NAME;
    }

    /* 3. Kiểm tra tính hợp lệ của loại sản phẩm */
    if (strlen(prod->category) >= MAX_CATEGORY_LEN) {
        return STATUS_ERR_INVALID_NAME;
    }

    /* 4. Kiểm tra tính hợp lệ của số lượng (> 0) */
    int val_qty = validate_quantity(prod->quantity);
    if (val_qty != STATUS_SUCCESS) {
        return val_qty;
    }

    /* 5. Kiểm tra tính hợp lệ của đơn giá (>= 0) */
    if (prod->price < 0.0) {
        return STATUS_ERR_INVALID_PRICE;
    }

    /* 6. Kiểm tra trùng lặp mã sản phẩm (Duplicate ID) */
    if (find_product_by_id(list, prod->id) != NULL) {
        return STATUS_ERR_DUPLICATE_ID;
    }

    /* 7. Mở rộng bộ nhớ mảng động an toàn khi đầy (Heap Reallocation Guard) */
    if (list->count >= list->capacity) {
        size_t new_cap = (list->capacity == 0) ? INITIAL_CAPACITY : (list->capacity * 2);
        Product *new_items = (Product *)realloc(list->items, new_cap * sizeof(Product));
        if (new_items == NULL) {
            return STATUS_ERR_MEMORY; /* Giữ nguyên list->items cũ, tránh rò rỉ bộ nhớ */
        }
        list->items = new_items;
        list->capacity = new_cap;
    }

    /* 8. Sao chép thông tin sản phẩm mới vào mảng */
    Product *target = &list->items[list->count];
    safe_strcpy(target->id, prod->id, sizeof(target->id));
    safe_strcpy(target->name, prod->name, sizeof(target->name));
    safe_strcpy(target->category, prod->category, sizeof(target->category));
    target->quantity = prod->quantity;
    target->price = prod->price;

    list->count++;
    return STATUS_SUCCESS;
}

/* TASK 4: Sửa thông tin sản phẩm theo mã ID */
int update_product(ProductList *list, const char *id, const char *new_name, const char *new_category, int new_quantity, double new_price) {
    if (list == NULL || id == NULL) {
        return STATUS_ERR_NULL_PTR;
    }

    /* Tìm vị trí sản phẩm theo ID */
    Product *target = find_product_by_id(list, id);
    if (target == NULL) {
        return STATUS_ERR_NOT_FOUND;
    }

    /* 1. Kiểm tra và cập nhật tên mới (nếu new_name != NULL) */
    if (new_name != NULL) {
        if (new_name[0] == '\0' || strlen(new_name) >= MAX_NAME_LEN) {
            return STATUS_ERR_INVALID_NAME;
        }
        safe_strcpy(target->name, new_name, sizeof(target->name));
    }

    /* 2. Kiểm tra và cập nhật loại sản phẩm mới (nếu new_category != NULL) */
    if (new_category != NULL) {
        if (strlen(new_category) >= MAX_CATEGORY_LEN) {
            return STATUS_ERR_INVALID_NAME;
        }
        safe_strcpy(target->category, new_category, sizeof(target->category));
    }

    /* 3. Kiểm tra và cập nhật số lượng mới (nếu new_quantity != -1) */
    if (new_quantity != -1) {
        int val_qty = validate_quantity(new_quantity);
        if (val_qty != STATUS_SUCCESS) {
            return val_qty;
        }
        target->quantity = new_quantity;
    }

    /* 4. Kiểm tra và cập nhật đơn giá mới (nếu new_price != -1.0) */
    if (new_price >= 0.0) {
        target->price = new_price;
    } else if (new_price != -1.0) {
        return STATUS_ERR_INVALID_PRICE;
    }

    return STATUS_SUCCESS;
}

int delete_product(ProductList *list, const char *id) {
    (void)list; (void)id;
    return STATUS_ERR_NOT_FOUND;
}

Product* find_product_by_id(const ProductList *list, const char *id) {
    if (list == NULL || id == NULL || list->items == NULL) {
        return NULL;
    }
    for (size_t i = 0; i < list->count; ++i) {
        if (strcmp(list->items[i].id, id) == 0) {
            return &list->items[i];
        }
    }
    return NULL;
}

int search_products_by_name(const ProductList *list, const char *keyword, Product *results, size_t max_results, size_t *out_count) {
    (void)list; (void)keyword; (void)results; (void)max_results;
    if (out_count) *out_count = 0;
    return STATUS_SUCCESS;
}
