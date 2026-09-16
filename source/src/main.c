#include "../include/models.h"
#include "../include/product.h"
#include "../include/validation.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/utils.h"

/*Phần test của Đô*/

static const char* get_status_desc(int status) {
    switch (status) {
        case STATUS_SUCCESS: return "HOP LE";
        case STATUS_ERR_INVALID_NAME: return "LOI TEN SP";
        case STATUS_ERR_INVALID_CATEGORY: return "LOI LOAI SP";
        case STATUS_ERR_INVALID_UNIT: return "LOI DON VI";
        case STATUS_ERR_INVALID_ID: return "LOI MA SP";
        case STATUS_ERR_INVALID_QTY: return "LOI SO LUONG";
        case STATUS_ERR_INVALID_PRICE: return "LOI DON GIA";
        case STATUS_ERR_OVERFLOW: return "TRAN SO NGUYEN";
        default: return "KHONG HOP LE";
    }
}

static void display_and_validate_file(const char *filepath) {
    FILE *f = fopen(filepath, "r");
    if (f == NULL) {
        printf("[LOI] Khong the mo tap tin du lieu mau: %s\n\n", filepath);
        return;
    }

    printf("_________________________________________________________________________________________________________________________\n");
    printf("                  KIEM TRA GIA VA SO LUONG SAN PHAM \n");
    printf("_________________________________________________________________________________________________________________________\n");
    printf("%-8s | %-32s | %-12s | %-8s | %-8s | %-9s | %-18s\n",
           "MA SP", "TEN SAN PHAM", "LOAI SP", "DON VI", "SO LUONG", "GIA ($)", "K.TRA GIA & SL");
    printf("-------------------------------------------------------------------------------------------------------------------------\n");

    char line[512];
    int total_products = 0;
    int valid_products = 0;

    while (fgets(line, sizeof(line), f) != NULL) {
        size_t len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[len - 1] = '\0';
            len--;
        }

        if (len == 0 || line[0] == '#') {
            continue;
        }

        char *tokens[8];
        int num_tokens = split_line(line, '|', tokens, 8);

        if (num_tokens >= 6) {
            total_products++;
            const char *id = tokens[0];
            const char *name = tokens[1];
            const char *category = tokens[2];
            const char *unit = tokens[3];
            const char *qty_str = tokens[4];
            const char *price_str = tokens[5];

            // Kiểm tra giá và số lượng bằng validate_price_quantity
            int pq_st = validate_price_quantity(price_str, qty_str);

            char status_detail[64];
            if (pq_st == STATUS_SUCCESS) {
                snprintf(status_detail, sizeof(status_detail), "[HOP LE]");
                valid_products++;
            } else {
                snprintf(status_detail, sizeof(status_detail), "[%s]", get_status_desc(pq_st));
            }

            printf("%-8s | %-32s | %-12s | %-8s | %-8s | %-9s | %-18s\n",
                   id, name, category, unit, qty_str, price_str, status_detail);
        }
    }

    printf("-------------------------------------------------------------------------------------------------------------------------\n");
    printf("Tong so ban ghi mau: %d | Gia & SL hop le: %d | Gia & SL loi: %d\n",
           total_products, valid_products, total_products - valid_products);
    printf("_________________________________________________________________________________________________________________________\n\n");

    fclose(f);
}

static int lookup_product_in_file(const char *filepath, const char *search_id) {
    FILE *f = fopen(filepath, "r");
    if (f == NULL) {
        return 0;
    }

    char line[512];
    while (fgets(line, sizeof(line), f) != NULL) {
        size_t len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[len - 1] = '\0';
            len--;
        }

        if (len == 0 || line[0] == '#') {
            continue;
        }

        char *tokens[8];
        int num_tokens = split_line(line, '|', tokens, 8);

        if (num_tokens >= 6 && strcmp(tokens[0], search_id) == 0) {
            const char *id = tokens[0];
            const char *name = tokens[1];
            const char *category = tokens[2];
            const char *unit = tokens[3];
            const char *qty_str = tokens[4];
            const char *price_str = tokens[5];

            printf("\n KET QUA TRA CUU: %s\n", id);
            printf("----------------------------------------------------------------------\n");
            printf("  - Ma san pham (ID)     : %s\n", id);
            printf("  - Ten san pham (Name)  : %s\n", name);
            printf("  - Loai san pham (Cat)  : %s\n", category);
            printf("  - Don vi tinh (Unit)   : %s\n", unit);
            printf("  - So luong ton kho     : %s\n", qty_str);
            printf("  - Don gia niem yet     : %s $\n", price_str);
            printf("----------------------------------------------------------------------\n");

            int price_st = validate_price_str(price_str);
            int qty_st = validate_quantity_str(qty_str);
            int pq_st = validate_price_quantity(price_str, qty_str);

            printf("  * Kiem tra Don gia (validate_price_str)   : %s\n",
                   price_st == STATUS_SUCCESS ? "HOP LE [OK]" : "KHONG HOP LE (AM/SAI DINH DANG) [FAIL]");
            printf("  * Kiem tra So luong (validate_quantity_str): %s\n",
                   qty_st == STATUS_SUCCESS ? "HOP LE [OK]" : "KHONG HOP LE (AM/SAI DINH DANG) [FAIL]");
            printf("  * validate_price_quantity(\"%s\", \"%s\") : %s (Ma loi: %d)\n",
                   price_str, qty_str,
                   pq_st == STATUS_SUCCESS ? "HOP LE [OK]" : "CHAN THANH CONG [CHAN GIA TRI SAI]",
                   pq_st);
            printf("----------------------------------------------------------------------\n\n");
            fclose(f);
            return 1;
        }
    }

    fclose(f);
    return 0;
}

static void run_utils_tests() {
    printf("--- Chay cac ca kiem thu: Cac ham tien ich dung chung (utils.h) ---\n");
    printf("1. is_empty_or_whitespace('   ')           : %s\n",
           is_empty_or_whitespace("   ") ? "[PASS]" : "[FAIL]");
    printf("2. is_empty_or_whitespace(NULL)            : %s\n",
           is_empty_or_whitespace(NULL) ? "[PASS]" : "[FAIL]");

    char str_trim[32] = "  Laptop Dell  ";
    trim_whitespace(str_trim);
    printf("3. trim_whitespace('  Laptop Dell  ')      : %s\n",
           strcmp(str_trim, "Laptop Dell") == 0 ? "[PASS]" : "[FAIL]");

    char buf[10];
    int copy_st = safe_string_copy(buf, sizeof(buf), "VeryLongStringHere");
    printf("4. safe_string_copy (chong tran bo dem)    : %s\n",
           (copy_st == STATUS_ERR_OVERFLOW && buf[sizeof(buf) - 1] == '\0') ? "[PASS]" : "[FAIL]");

    int int_val = 0;
    int int_st = safe_str_to_int("quqw", &int_val);
    printf("5. safe_str_to_int ('quqw' chan chu)       : %s\n",
           int_st == STATUS_ERR_INVALID_QTY ? "[PASS]" : "[FAIL]");

    double dbl_val = 0.0;
    int dbl_st = safe_str_to_double("1499.99", &dbl_val);
    printf("6. safe_str_to_double ('1499.99' hop le)   : %s\n",
           (dbl_st == STATUS_SUCCESS && dbl_val > 1499.0) ? "[PASS]" : "[FAIL]");

    char case_str[16] = "sp001";
    to_upper_case(case_str);
    printf("7. to_upper_case ('sp001' -> 'SP001')      : %s\n",
           strcmp(case_str, "SP001") == 0 ? "[PASS]" : "[FAIL]");
    printf("-------------------------------------------------------------------------------------------------------------------------\n\n");
}

/*Phần test của Trang*/
void print_product_list(const ProductList *list) {
    printf("--- DANH SACH SAN PHAM KHO (Tong so: %zu | Suc chua: %zu) ---\n",
            list->count, list->capacity);
    for (size_t i = 0; i < list->count; ++i) {
        printf(
            "   [%zu] ID: %-10s | Ten: %-30s | Loai: %-25s | Don vi: %-10s | SL: %-4d | Gia: %.3f\n",
            i + 1, list->items[i].id, list->items[i].name, list->items[i].category, list->items[i].unit,
            list->items[i].quantity, list->items[i].price);
    }
    printf("------------------------------------------------------------------------------------------\n");
}

int main() {
    printf("___________________________________________________________\n");
    printf("  KIEM THU MODULE VALIDATION\n");
    printf("___________________________________________________________\n");
    run_utils_tests();

    const char *filepath = find_file_path("products.txt");
    display_and_validate_file(filepath);

    printf("TRA CUU VA KIEM TRA CHI TIET THONG TIN SAN PHAM\n");
    char id[256];
    safe_read_line("Nhap ma san pham: ", id, sizeof(id));

    if (strlen(id) > 0) {
        if (!lookup_product_in_file(filepath, id)) {
            printf("\n[THONG BAO] Khong tim thay ma '%s'!\n\n", id);
        }
    }

    printf("\n");

    printf("___________________________________________________________\n");
    printf("  KIEM THU TASK 3: CHUC NANG THEM SAN PHAM\n");
    printf("___________________________________________________________\n\n");

    /* 1. Khởi tạo danh sách sản phẩm động */
    ProductList list;
    if (product_list_init(&list) != STATUS_SUCCESS) {
        printf("[ERR] Khoi tao danh sach san pham that bai!\n");
        return 1;
    }
    printf("[OK] Khoi tao thanh cong danh sach san pham dong.\n\n");

    /* 2. Thêm các sản phẩm hợp lệ đầy đủ 6 trường thông tin (ID, Name, Category, Unit, Quantity, Price) */
    Product p1 = {"SP001", "Laptop Dell XPS 15", "Dien tu", "chiec", 10, 1500.0};
    Product p2 = {"SP002", "Chuot Logitech MX Master", "Phu kien", "cai", 50, 99.9};
    Product p3 = {"SP003", "Ban phim Keychron K2", "Phu kien", "chiec", 30, 85.5};

    printf("Them SP001: %s\n",
            add_product(&list, &p1) == STATUS_SUCCESS ? "Thanh cong" : "That bai");
    printf("Them SP002: %s\n",
            add_product(&list, &p2) == STATUS_SUCCESS ? "Thanh cong" : "That bai");
    printf("Them SP003: %s\n",
            add_product(&list, &p3) == STATUS_SUCCESS ? "Thanh cong" : "That bai");
    printf("\n");

    /* 3. Thử nghiệm các trường hợp Input bất thường (Invalid Inputs) */
    printf(">>> KIEM THU INPUT BAT THUONG <<<\n");

    /* Test thêm sản phẩm trùng ID */
    Product p_dup = {"SP001", "Laptop Duplicate", "Dien tu", "chiec", 5, 1000.0};
    int res_dup = add_product(&list, &p_dup);
    printf("Them trung ID SP001: %d\n", res_dup); /* Ky vong STATUS_ERR_DUPLICATE_ID = -12 */

    /* Test thêm sản phẩm chứa ký tự cấm '|' */
    Product p_bad_id = {"SP|004", "San pham sai ID", "Khac", "cai", 5, 10.0};
    printf("Them ID co ky tu cam 'SP|004' : %d\n", add_product(&list, &p_bad_id)); /* Ky vong STATUS_ERR_INVALID_ID = -2 */

    /* Test thêm sản phẩm có số lượng âm */
    Product p_bad_qty = {"SP004", "San pham am SL", "Khac", "cai", -5, 10.0};
    printf("Them so luong am '-5': %d\n", add_product(&list, &p_bad_qty)); /* Ky vong STATUS_ERR_INVALID_QTY = -3 */
    printf("\n");

    /* Hien thi danh sach san pham da them thanh cong */
    print_product_list(&list);

    /* 4. KIEM THU TASK 4: CHUC NANG SUA SAN PHAM */
    printf("\n___________________________________________________________\n");
    printf("  KIEM THU TASK 4: CHUC NANG SUA SAN PHAM\n");
    printf("___________________________________________________________\n");
    printf("Cap nhat SP002 -> Ten moi: 'Chuot Logitech MX Master 3S', Loai moi: 'Phu kien Cao cap', Don vi: 'cai', SL: 45, Gia: 105.0\n");
    int res_upd = update_product(&list, "SP002", "Chuot Logitech MX Master 3S", "Phu kien Cao cap", "cai", 45, 105.0);
    printf("Ket qua cap nhat SP002: %s\n", res_upd == STATUS_SUCCESS ? "Thanh cong" : "That bai");

    /* Test cập nhật sản phẩm không tồn tại SP999 */
    int res_upd_notfound = update_product(&list, "SP999", "Ten SP", "Loai", "cai", 10, 20.0);
    printf("Cap nhat SP999 khong ton tai : %d\n", res_upd_notfound); /* Ky vong STATUS_ERR_NOT_FOUND = -4 */
    printf("\n");

    /* Hiển thị danh sách sau khi cập nhật */
    print_product_list(&list);

    /* 5. KIEM THU TASK 5: CHUC NANG XOA SAN PHAM */
    printf("\n___________________________________________________________\n");
    printf("  KIEM THU TASK 5: CHUC NANG XOA SAN PHAM\n");
    printf("___________________________________________________________\n");
    printf("Xoa san pham SP002:\n");
    int res_del = delete_product(&list, "SP002");
    printf("Ket qua xoa SP002: %s\n", res_del == STATUS_SUCCESS ? "Thanh cong" : "That bai");

    /* Test xóa lại SP002 đã bị xóa */
    int res_del_again = delete_product(&list, "SP002");
    printf("Xoa lai SP002 da bi xoa: %d\n", res_del_again); /* Ky vong STATUS_ERR_NOT_FOUND = -4 */
    printf("\n");

    /* Hiển thị danh sách sau khi xóa */
    print_product_list(&list);

    /* 6. KIEM THU TASK 6: CHUC NANG TIM KIEM SAN PHAM */
    printf("\n___________________________________________________________\n");
    printf("  KIEM THU TASK 6: CHUC NANG TIM KIEM SAN PHAM\n");
    printf("___________________________________________________________\n");

    printf("1. Tim kiem theo ID 'SP001':\n");
    Product *found_id = find_product_by_id(&list, "SP001");
    if (found_id != NULL) {
        printf("   Found -> ID: %s | Ten: %s | Loai: %s | Don vi: %s | SL: %d | Gia: %.3f\n",
                found_id->id, found_id->name, found_id->category, found_id->unit, found_id->quantity, found_id->price);
    } else {
        printf("   Not found!\n");
    }

    printf("2. Tim kiem theo ID khong ton tai 'SP999':\n");
    Product *not_found_id = find_product_by_id(&list, "SP999");
    printf("   Ket qua : %s\n", not_found_id == NULL ? "NULL" : "Co du lieu"); /* Ky vong NULL */

    printf("3. Tim kiem theo ten (tu khoa 'keychron'):\n");
    Product search_results[10];
    size_t found_count = 0;
    if (search_products_by_name(&list, "keychron", search_results, 10, &found_count) == STATUS_SUCCESS) {
        printf("   Tim thay %zu san pham phu hop:\n", found_count);
        for (size_t i = 0; i < found_count; ++i) {
            printf("   ID: %s | Ten: %s | Loai: %s | Don vi: %s | SL: %d\n",
                    search_results[i].id, search_results[i].name, search_results[i].category, search_results[i].unit, search_results[i].quantity);
        }
    }
    printf("\n");

    /* Giải phóng bộ nhớ động */
    product_list_free(&list);
    printf("\n[OK] Giai phong bo nho thanh cong.\n");
    printf("___________________________________________________________\n");
    
    return 0;
}