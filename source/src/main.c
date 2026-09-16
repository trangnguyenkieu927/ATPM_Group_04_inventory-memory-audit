#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../include/models.h"
#include "../include/product.h"
#include "../include/validation.h"
#include "../include/utils.h"
#include "../include/report.h"

/* =========================================================================
 * 1. QUẢN LÝ ĐƯỜNG DẪN TỆP DỮ LIỆU & LỊCH SỬ GIAO DỊCH
 * ========================================================================= */

static void resolve_data_paths(char *prod_path, char *trans_path, size_t max_len) {
    const char *p = find_file_path("products.txt");
    safe_string_copy(prod_path, max_len, p);

    const char *t = find_file_path("transactions.txt");
    safe_string_copy(trans_path, max_len, t);
}

static void log_transaction(const char *trans_file, const char *action, const char *id, int qty, int balance) {
    FILE *f = fopen(trans_file, "a");
    if (f == NULL) return;

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char time_str[32];
    if (t != NULL) {
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", t);
    } else {
        safe_string_copy(time_str, sizeof(time_str), "2026-09-16 12:00:00");
    }

    fprintf(f, "%s | %-6s | %-10s | %-5d | Balance: %d\n",
            time_str, action, id, qty, balance);
    fclose(f);
}

static void print_history(const char *trans_file) {
    FILE *f = fopen(trans_file, "r");
    if (f == NULL) {
        printf("[i] Chua co lich su giao dich nao ghi nhan.\n");
        return;
    }

    printf("\n================================ LICH SU GIAO DICH (%s) ================================\n", trans_file);
    char line[256];
    int count = 0;
    while (fgets(line, sizeof(line), f) != NULL) {
        printf("%s", line);
        count++;
    }
    printf("================================ Tong so giao dich: %d ================================\n\n", count);
    fclose(f);
}

/* =========================================================================
 * 2. ĐỌC / GHI DỮ LIỆU SẢN PHẨM TỪ TỆP PRODUCTS.TXT
 * ========================================================================= */

static int load_products_to_list(ProductList *list, const char *filepath) {
    FILE *f = fopen(filepath, "r");
    if (f == NULL) {
        return STATUS_ERR_FILE_IO;
    }

    char line[512];
    while (fgets(line, sizeof(line), f) != NULL) {
        trim_whitespace(line);
        if (line[0] == '\0' || line[0] == '#') {
            continue;
        }

        char *tokens[8];
        int num_tokens = split_line(line, '|', tokens, 8);
        if (num_tokens >= 6) {
            Product p;
            memset(&p, 0, sizeof(p));
            safe_string_copy(p.id, sizeof(p.id), trim_whitespace(tokens[0]));
            safe_string_copy(p.name, sizeof(p.name), trim_whitespace(tokens[1]));
            safe_string_copy(p.category, sizeof(p.category), trim_whitespace(tokens[2]));
            safe_string_copy(p.unit, sizeof(p.unit), trim_whitespace(tokens[3]));

            int qty = 0;
            if (safe_str_to_int(tokens[4], &qty) == STATUS_SUCCESS) {
                p.quantity = qty;
            } else {
                p.quantity = 0;
            }

            double price = 0.0;
            if (safe_str_to_double(tokens[5], &price) == STATUS_SUCCESS) {
                p.price = price;
            } else {
                p.price = 0.0;
            }

            /* Không dùng add_product trực tiếp để tránh ghi đè lỗi trùng lặp khi khởi động */
            if (find_product_by_id(list, p.id) == NULL) {
                add_product(list, &p);
            }
        }
    }

    fclose(f);
    return STATUS_SUCCESS;
}

static int save_products_from_list(const ProductList *list, const char *filepath) {
    if (list == NULL || filepath == NULL) {
        return STATUS_ERR_NULL_PTR;
    }

    FILE *f = fopen(filepath, "w");
    if (f == NULL) {
        return STATUS_ERR_FILE_IO;
    }

    fprintf(f, "# ID|Name|Category|Unit|Quantity|Price\n");
    for (size_t i = 0; i < list->count; ++i) {
        fprintf(f, "%s|%s|%s|%s|%d|%.2f\n",
                list->items[i].id,
                list->items[i].name,
                list->items[i].category,
                list->items[i].unit,
                list->items[i].quantity,
                list->items[i].price);
    }

    fclose(f);
    return STATUS_SUCCESS;
}

/* =========================================================================
 * 3. HIỂN THỊ DANH SÁCH & BÁO CÁO KIỂM TOÁN TỆP
 * ========================================================================= */

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
        case STATUS_ERR_DUPLICATE_ID: return "TRUNG MA SP";
        case STATUS_ERR_NOT_FOUND: return "KHONG TIM THAY";
        case STATUS_ERR_INSUFFICIENT_STOCK: return "KHONG DU TON KHO";
        default: return "LOI KHONG XAC DINH";
    }
}

static void print_product_table(const ProductList *list) {
    if (list == NULL || list->count == 0) {
        printf("\n[i] Danh sach san pham hien dang trong!\n");
        return;
    }

    printf("\n=========================================================================================================================\n");
    printf("                                      DANH SACH SAN PHAM TRONG KHO                                      \n");
    printf("=========================================================================================================================\n");
    printf("%-4s | %-10s | %-32s | %-16s | %-8s | %-8s | %-10s | %-12s\n",
           "STT", "MA SP", "TEN SAN PHAM", "LOAI SP", "DON VI", "TON KHO", "DON GIA ($)", "THANH TIEN ($)");
    printf("-------------------------------------------------------------------------------------------------------------------------\n");

    long long total_quantity = 0;
    double total_inventory_value = 0.0;

    for (size_t i = 0; i < list->count; ++i) {
        double item_total = (double)list->items[i].quantity * list->items[i].price;
        total_quantity += list->items[i].quantity;
        total_inventory_value += item_total;

        printf("%-4zu | %-10s | %-32s | %-16s | %-8s | %-8d | %-10.2f | %-12.2f\n",
               i + 1,
               list->items[i].id,
               list->items[i].name,
               list->items[i].category,
               list->items[i].unit,
               list->items[i].quantity,
               list->items[i].price,
               item_total);
    }

    printf("-------------------------------------------------------------------------------------------------------------------------\n");
    printf("Tong so mat hang: %zu | Tong ton kho: %lld | Tong gia tri kho: %.2f $\n",
           list->count, total_quantity, total_inventory_value);
    printf("=========================================================================================================================\n\n");
}

static void display_and_validate_file(const char *filepath) {
    FILE *f = fopen(filepath, "r");
    if (f == NULL) {
        printf("[LOI] Khong the mo tap tin du lieu mau: %s\n\n", filepath);
        return;
    }

    printf("\n=========================================================================================================================\n");
    printf("                               KIEM TOAN DU LIEU TEP SAN PHAM (%s)                               \n", filepath);
    printf("=========================================================================================================================\n");
    printf("%-8s | %-32s | %-12s | %-8s | %-8s | %-9s | %-18s\n",
           "MA SP", "TEN SAN PHAM", "LOAI SP", "DON VI", "SO LUONG", "GIA ($)", "TRANG THAI KIEM TOAN");
    printf("-------------------------------------------------------------------------------------------------------------------------\n");

    char line[512];
    int total_products = 0;
    int valid_products = 0;

    while (fgets(line, sizeof(line), f) != NULL) {
        trim_whitespace(line);
        if (line[0] == '\0' || line[0] == '#') {
            continue;
        }

        char *tokens[8];
        int num_tokens = split_line(line, '|', tokens, 8);

        if (num_tokens >= 6) {
            total_products++;
            const char *id = trim_whitespace(tokens[0]);
            const char *name = trim_whitespace(tokens[1]);
            const char *category = trim_whitespace(tokens[2]);
            const char *unit = trim_whitespace(tokens[3]);
            const char *qty_str = trim_whitespace(tokens[4]);
            const char *price_str = trim_whitespace(tokens[5]);

            int id_st = validate_id(id);
            int name_st = validate_product_name(name);
            int cat_st = validate_category(category);
            int unit_st = validate_unit(unit);
            int pq_st = validate_price_quantity(price_str, qty_str);

            char status_detail[64];
            if (id_st == STATUS_SUCCESS && name_st == STATUS_SUCCESS &&
                cat_st == STATUS_SUCCESS && unit_st == STATUS_SUCCESS && pq_st == STATUS_SUCCESS) {
                snprintf(status_detail, sizeof(status_detail), "[HOP LE]");
                valid_products++;
            } else if (id_st != STATUS_SUCCESS) {
                snprintf(status_detail, sizeof(status_detail), "[%s]", get_status_desc(id_st));
            } else if (name_st != STATUS_SUCCESS) {
                snprintf(status_detail, sizeof(status_detail), "[%s]", get_status_desc(name_st));
            } else if (cat_st != STATUS_SUCCESS) {
                snprintf(status_detail, sizeof(status_detail), "[%s]", get_status_desc(cat_st));
            } else if (unit_st != STATUS_SUCCESS) {
                snprintf(status_detail, sizeof(status_detail), "[%s]", get_status_desc(unit_st));
            } else {
                snprintf(status_detail, sizeof(status_detail), "[%s]", get_status_desc(pq_st));
            }

            printf("%-8s | %-32s | %-12s | %-8s | %-8s | %-9s | %-18s\n",
                   id, name, category, unit, qty_str, price_str, status_detail);
        }
    }

    printf("-------------------------------------------------------------------------------------------------------------------------\n");
    printf("Tong so ban ghi: %d | Ban ghi hop le: %d | Ban ghi loi: %d\n",
           total_products, valid_products, total_products - valid_products);
    printf("=========================================================================================================================\n\n");

    fclose(f);
}

/* =========================================================================
 * 4. BỘ KIỂM THỬ TỰ ĐỘNG (UNIT TEST SUITE)
 * ========================================================================= */

static void run_all_unit_tests() {
    printf("\n=========================================================================================\n");
    printf("                            CHAY BO KIEM THU TU DONG (UNIT TESTS)                        \n");
    printf("=========================================================================================\n");

    /* 1. Test module utils */
    printf("\n--- 1. Kiem thu Module Tien ich (utils.h) ---\n");
    printf("1. is_empty_or_whitespace('   ')           : %s\n",
           is_empty_or_whitespace("   ") ? "[PASS]" : "[FAIL]");
    char str_trim[32] = "  Laptop Dell  ";
    trim_whitespace(str_trim);
    printf("2. trim_whitespace('  Laptop Dell  ')      : %s\n",
           strcmp(str_trim, "Laptop Dell") == 0 ? "[PASS]" : "[FAIL]");
    char buf[10];
    int copy_st = safe_string_copy(buf, sizeof(buf), "VeryLongStringHere");
    printf("3. safe_string_copy (chong tran bo dem)    : %s\n",
           (copy_st == STATUS_ERR_OVERFLOW && buf[sizeof(buf) - 1] == '\0') ? "[PASS]" : "[FAIL]");
    int int_val = 0;
    printf("4. safe_str_to_int ('qưqw' chan chu)       : %s\n",
           safe_str_to_int("qưqw", &int_val) == STATUS_ERR_INVALID_QTY ? "[PASS]" : "[FAIL]");
    char case_str[16] = "sp001";
    to_upper_case(case_str);
    printf("5. to_upper_case ('sp001' -> 'SP001')      : %s\n",
           strcmp(case_str, "SP001") == 0 ? "[PASS]" : "[FAIL]");

    /* 2. Test module validation */
    printf("\n--- 2. Kiem thu Module Kiem tra du lieu (validation.h) ---\n");
    printf("1. validate_id hop le ('SP001')            : %s\n",
           validate_id("SP001") == STATUS_SUCCESS ? "[PASS]" : "[FAIL]");
    printf("2. validate_id chan ky tu cam ('SP|01')    : %s\n",
           validate_id("SP|01") == STATUS_ERR_INVALID_ID ? "[PASS]" : "[FAIL]");
    printf("3. validate_product_name rong              : %s\n",
           validate_product_name("   ") == STATUS_ERR_INVALID_NAME ? "[PASS]" : "[FAIL]");
    printf("4. validate_price_str chan gia am (-10.5)  : %s\n",
           validate_price_str("-10.5") == STATUS_ERR_INVALID_PRICE ? "[PASS]" : "[FAIL]");
    printf("5. validate_quantity_str chan chu ('qưqw') : %s\n",
           validate_quantity_str("qưqw") == STATUS_ERR_INVALID_QTY ? "[PASS]" : "[FAIL]");
    printf("6. validate_price_quantity hop le          : %s\n",
           validate_price_quantity("1499.99", "35") == STATUS_SUCCESS ? "[PASS]" : "[FAIL]");

    /* 3. Test module product */
    printf("\n--- 3. Kiem thu Module San pham (product.h) ---\n");
    ProductList test_list;
    product_list_init(&test_list);
    Product t1 = {"TEST01", "Chuot quang", "Phu kien", "cai", 10, 50.0};
    printf("1. add_product moi                         : %s\n",
           add_product(&test_list, &t1) == STATUS_SUCCESS ? "[PASS]" : "[FAIL]");
    printf("2. add_product trung ma (Duplicate)        : %s\n",
           add_product(&test_list, &t1) == STATUS_ERR_DUPLICATE_ID ? "[PASS]" : "[FAIL]");
    printf("3. update_product hop le                   : %s\n",
           update_product(&test_list, "TEST01", "Chuot khong day", NULL, NULL, 15, 60.0) == STATUS_SUCCESS ? "[PASS]" : "[FAIL]");
    printf("4. find_product_by_id tim thay             : %s\n",
           find_product_by_id(&test_list, "TEST01") != NULL ? "[PASS]" : "[FAIL]");
    printf("5. delete_product xoa thanh cong           : %s\n",
           delete_product(&test_list, "TEST01") == STATUS_SUCCESS ? "[PASS]" : "[FAIL]");
    printf("6. find_product_by_id sau khi xoa (NULL)   : %s\n",
           find_product_by_id(&test_list, "TEST01") == NULL ? "[PASS]" : "[FAIL]");
    product_list_free(&test_list);

    /* 4. Test module report */
    printf("\n--- 4. Kiem thu Module Bao cao (report.h) ---\n");
    ProductList rep_list;
    product_list_init(&rep_list);
    Product rp1 = {"R01", "Laptop Test", "Laptop", "Chiec", 0, 1200.0};
    Product rp2 = {"R02", "Chuot Test", "Phu kien", "Con", 5, 25.0};
    add_product(&rep_list, &rp1);
    add_product(&rep_list, &rp2);
    InventorySummary rep_sum;
    int rep_st = calculate_inventory_summary(&rep_list, 10, &rep_sum);
    printf("1. calculate_inventory_summary hop le      : %s\n",
           rep_st == STATUS_SUCCESS ? "[PASS]" : "[FAIL]");
    printf("2. Dem san pham het hang (SL = 0)          : %s\n",
           rep_sum.out_of_stock_count == 1 ? "[PASS]" : "[FAIL]");
    printf("3. Dem san pham sap het hang (SL <= 10)    : %s\n",
           rep_sum.low_stock_count == 1 ? "[PASS]" : "[FAIL]");
    CategorySummary cat_sums[8];
    size_t cat_c = 0;
    calculate_category_summaries(&rep_list, cat_sums, 8, &cat_c);
    printf("4. calculate_category_summaries (2 loai)   : %s\n",
           cat_c == 2 ? "[PASS]" : "[FAIL]");
    long long t_qty = 0;
    double t_val = 0.0;
    printf("5. report_statistics xuat so luong, gia tri: %s\n",
           ((report_statistics)(&rep_list, &t_qty, &t_val) == STATUS_SUCCESS && t_qty == 5 && t_val == 125.0) ? "[PASS]" : "[FAIL]");
    product_list_free(&rep_list);

    printf("\n=========================================================================================\n");
    printf("                     TAT CA CAC MODULE DEU HOAN THANH KIEM THU [OK]                      \n");
    printf("=========================================================================================\n\n");
}

/* =========================================================================
 * 5. CÁC TÍNH NĂNG TƯƠNG TÁC MENU
 * ========================================================================= */

static void handle_add_product(ProductList *list, const char *prod_file) {
    printf("\n--- THEM SAN PHAM MOI ---\n");
    Product p;
    memset(&p, 0, sizeof(p));
    char buf[256];

    safe_read_line("Nhap ma san pham (vi du: SP011): ", p.id, sizeof(p.id));
    trim_whitespace(p.id);
    int id_st = validate_id(p.id);
    if (id_st != STATUS_SUCCESS) {
        printf("[!] Ma san pham khong hop le: %s\n", get_status_desc(id_st));
        return;
    }
    if (find_product_by_id(list, p.id) != NULL) {
        printf("[!] Ma san pham '%s' da ton tai trong kho!\n", p.id);
        return;
    }

    safe_read_line("Nhap ten san pham: ", p.name, sizeof(p.name));
    trim_whitespace(p.name);
    int name_st = validate_product_name(p.name);
    if (name_st != STATUS_SUCCESS) {
        printf("[!] Ten san pham khong hop le: %s\n", get_status_desc(name_st));
        return;
    }

    safe_read_line("Nhap loai san pham: ", p.category, sizeof(p.category));
    trim_whitespace(p.category);
    int cat_st = validate_category(p.category);
    if (cat_st != STATUS_SUCCESS) {
        printf("[!] Loai san pham khong hop le: %s\n", get_status_desc(cat_st));
        return;
    }

    safe_read_line("Nhap don vi tinh (chiec/cai/hop...): ", p.unit, sizeof(p.unit));
    trim_whitespace(p.unit);
    int unit_st = validate_unit(p.unit);
    if (unit_st != STATUS_SUCCESS) {
        printf("[!] Don vi tinh khong hop le: %s\n", get_status_desc(unit_st));
        return;
    }

    safe_read_line("Nhap so luong ton kho: ", buf, sizeof(buf));
    int qty_st = validate_quantity_str(buf);
    if (qty_st != STATUS_SUCCESS) {
        printf("[!] So luong khong hop le (phai la so nguyen khong am)!\n");
        return;
    }
    safe_str_to_int(buf, &p.quantity);

    safe_read_line("Nhap don gia ($): ", buf, sizeof(buf));
    int price_st = validate_price_str(buf);
    if (price_st != STATUS_SUCCESS) {
        printf("[!] Don gia khong hop le (phai la so thuc khong am)!\n");
        return;
    }
    safe_str_to_double(buf, &p.price);

    int res = add_product(list, &p);
    if (res == STATUS_SUCCESS) {
        printf("[+] Them san pham '%s' thanh cong!\n", p.id);
        save_products_from_list(list, prod_file);
        printf("[*] Da cap nhat tep du lieu: %s\n", prod_file);
    } else {
        printf("[-] Them san pham that bai: %s\n", get_status_desc(res));
    }
}

static void handle_update_product(ProductList *list, const char *prod_file) {
    printf("\n--- SUA THONG TIN SAN PHAM ---\n");
    char id[MAX_ID_LEN];
    safe_read_line("Nhap ma san pham can sua: ", id, sizeof(id));
    trim_whitespace(id);

    Product *p = find_product_by_id(list, id);
    if (p == NULL) {
        printf("[!] Khong tim thay san pham co ma '%s'!\n", id);
        return;
    }

    printf("[i] Tim thay: %s | %s | %s | %s | SL: %d | Gia: %.2f $\n",
           p->id, p->name, p->category, p->unit, p->quantity, p->price);
    printf("(Nhan Enter neu muon giu nguyen thong tin hien tai)\n");

    char new_name[MAX_NAME_LEN] = "";
    char new_cat[MAX_CATEGORY_LEN] = "";
    char new_unit[MAX_UNIT_LEN] = "";
    char buf[64] = "";

    safe_read_line("Ten moi: ", new_name, sizeof(new_name));
    trim_whitespace(new_name);

    safe_read_line("Loai SP moi: ", new_cat, sizeof(new_cat));
    trim_whitespace(new_cat);

    safe_read_line("Don vi tinh moi: ", new_unit, sizeof(new_unit));
    trim_whitespace(new_unit);

    int new_qty = -1;
    safe_read_line("So luong moi: ", buf, sizeof(buf));
    trim_whitespace(buf);
    if (strlen(buf) > 0) {
        if (validate_quantity_str(buf) != STATUS_SUCCESS) {
            printf("[!] So luong khong hop le!\n");
            return;
        }
        safe_str_to_int(buf, &new_qty);
    }

    double new_price = -1.0;
    safe_read_line("Don gia moi ($): ", buf, sizeof(buf));
    trim_whitespace(buf);
    if (strlen(buf) > 0) {
        if (validate_price_str(buf) != STATUS_SUCCESS) {
            printf("[!] Don gia khong hop le!\n");
            return;
        }
        safe_str_to_double(buf, &new_price);
    }

    const char *p_name = (strlen(new_name) > 0) ? new_name : NULL;
    const char *p_cat = (strlen(new_cat) > 0) ? new_cat : NULL;
    const char *p_unit = (strlen(new_unit) > 0) ? new_unit : NULL;

    int res = update_product(list, id, p_name, p_cat, p_unit, new_qty, new_price);
    if (res == STATUS_SUCCESS) {
        printf("[+] Cap nhat san pham '%s' thanh cong!\n", id);
        save_products_from_list(list, prod_file);
        printf("[*] Da cap nhat tep du lieu: %s\n", prod_file);
    } else {
        printf("[-] Cap nhat that bai: %s\n", get_status_desc(res));
    }
}

static void handle_delete_product(ProductList *list, const char *prod_file) {
    printf("\n--- XOA SAN PHAM ---\n");
    char id[MAX_ID_LEN];
    safe_read_line("Nhap ma san pham can xoa: ", id, sizeof(id));
    trim_whitespace(id);

    Product *p = find_product_by_id(list, id);
    if (p == NULL) {
        printf("[!] Khong tim thay san pham co ma '%s'!\n", id);
        return;
    }

    printf("[?] Ban co chac chan muon xoa san pham: [%s - %s] (y/n)? ", p->id, p->name);
    char confirm[16];
    safe_read_line("", confirm, sizeof(confirm));
    trim_whitespace(confirm);

    if (confirm[0] == 'y' || confirm[0] == 'Y') {
        int res = delete_product(list, id);
        if (res == STATUS_SUCCESS) {
            printf("[+] Xoa san pham '%s' thanh cong!\n", id);
            save_products_from_list(list, prod_file);
            printf("[*] Da cap nhat tep du lieu: %s\n", prod_file);
        } else {
            printf("[-] Xoa that bai: %s\n", get_status_desc(res));
        }
    } else {
        printf("[*] Da huy thao tac xoa.\n");
    }
}

static void handle_search_product(const ProductList *list) {
    printf("\n--- TIM KIEM SAN PHAM ---\n");
    printf(" 1. Tim theo ma san pham (Chinh xac)\n");
    printf(" 2. Tim theo ten san pham (Tu khoa khong phan biet hoa/thuong)\n");
    char choice[16];
    safe_read_line("Chon phuong thuc tim kiem (1 hoac 2): ", choice, sizeof(choice));
    trim_whitespace(choice);

    if (choice[0] == '1') {
        char id[MAX_ID_LEN];
        safe_read_line("Nhap ma san pham can tim: ", id, sizeof(id));
        trim_whitespace(id);

        Product *p = find_product_by_id(list, id);
        if (p != NULL) {
            printf("\n>>> KET QUA TIM THAY <<<\n");
            printf("----------------------------------------------------------------------\n");
            printf("  - Ma san pham (ID)     : %s\n", p->id);
            printf("  - Ten san pham (Name)  : %s\n", p->name);
            printf("  - Loai san pham (Cat)  : %s\n", p->category);
            printf("  - Don vi tinh (Unit)   : %s\n", p->unit);
            printf("  - So luong ton kho     : %d\n", p->quantity);
            printf("  - Don gia niem yet     : %.2f $\n", p->price);
            printf("  - Tong gia tri ton     : %.2f $\n", (double)p->quantity * p->price);
            printf("----------------------------------------------------------------------\n");
        } else {
            printf("[!] Khong tim thay san pham voi ma '%s'!\n", id);
        }
    } else if (choice[0] == '2') {
        char keyword[MAX_NAME_LEN];
        safe_read_line("Nhap tu khoa ten san pham: ", keyword, sizeof(keyword));
        trim_whitespace(keyword);

        Product results[32];
        size_t found_count = 0;
        int res = search_products_by_name(list, keyword, results, 32, &found_count);

        if (res == STATUS_SUCCESS && found_count > 0) {
            printf("\n>>> TIM THAY %zu SAN PHAM PHU HOP VOI TU KHOA '%s' <<<\n", found_count, keyword);
            printf("%-4s | %-10s | %-32s | %-16s | %-8s | %-8s | %-10s\n",
                   "STT", "MA SP", "TEN SAN PHAM", "LOAI SP", "DON VI", "TON KHO", "DON GIA ($)");
            printf("-------------------------------------------------------------------------------------------------\n");
            for (size_t i = 0; i < found_count; ++i) {
                printf("%-4zu | %-10s | %-32s | %-16s | %-8s | %-8d | %-10.2f\n",
                       i + 1, results[i].id, results[i].name, results[i].category,
                       results[i].unit, results[i].quantity, results[i].price);
            }
            printf("-------------------------------------------------------------------------------------------------\n");
        } else {
            printf("[!] Khong tim thay san pham nao chua tu khoa '%s'!\n", keyword);
        }
    } else {
        printf("[!] Lua chon khong hop le.\n");
    }
}

static void handle_import_stock(ProductList *list, const char *prod_file, const char *trans_file) {
    printf("\n--- NHAP KHO (IMPORT STOCK) ---\n");
    char id[MAX_ID_LEN];
    safe_read_line("Nhap ma san pham: ", id, sizeof(id));
    trim_whitespace(id);

    Product *p = find_product_by_id(list, id);
    if (p == NULL) {
        printf("[!] San pham '%s' khong ton tai trong kho!\n", id);
        return;
    }

    printf("[i] San pham: %s | Ton kho hien tai: %d\n", p->name, p->quantity);
    char buf[64];
    safe_read_line("Nhap so luong can nhap kho (> 0): ", buf, sizeof(buf));
    trim_whitespace(buf);

    if (validate_quantity_str(buf) != STATUS_SUCCESS) {
        printf("[!] So luong nhap khong hop le (phai la so nguyen > 0)!\n");
        return;
    }

    int qty = 0;
    safe_str_to_int(buf, &qty);
    if (qty <= 0) {
        printf("[!] So luong nhap kho phai lon hon 0!\n");
        return;
    }

    /* Kiem tra nguy co tran so nguyen (Integer Overflow) */
    if (check_addition_overflow(p->quantity, qty) != STATUS_SUCCESS) {
        printf("[!] Canh bao nguy co tran so nguyen (Integer Overflow)! Khong the nhap kho.\n");
        return;
    }

    p->quantity += qty;
    printf("[+] Nhap kho thanh cong! Ton kho moi cua '%s': %d\n", p->id, p->quantity);

    log_transaction(trans_file, "IMPORT", p->id, qty, p->quantity);
    save_products_from_list(list, prod_file);
    printf("[*] Da ghi lich su giao dich va cap nhat tep kho hang.\n");
}

static void handle_export_stock(ProductList *list, const char *prod_file, const char *trans_file) {
    printf("\n--- XUAT KHO (EXPORT STOCK) ---\n");
    char id[MAX_ID_LEN];
    safe_read_line("Nhap ma san pham: ", id, sizeof(id));
    trim_whitespace(id);

    Product *p = find_product_by_id(list, id);
    if (p == NULL) {
        printf("[!] San pham '%s' khong ton tai trong kho!\n", id);
        return;
    }

    printf("[i] San pham: %s | Ton kho hien tai: %d\n", p->name, p->quantity);
    char buf[64];
    safe_read_line("Nhap so luong can xuat kho (> 0): ", buf, sizeof(buf));
    trim_whitespace(buf);

    if (validate_quantity_str(buf) != STATUS_SUCCESS) {
        printf("[!] So luong xuat khong hop le (phai la so nguyen > 0)!\n");
        return;
    }

    int qty = 0;
    safe_str_to_int(buf, &qty);
    if (qty <= 0) {
        printf("[!] So luong xuat kho phai lon hon 0!\n");
        return;
    }

    /* Kiem tra ton kho du hay khong (Underflow / Insufficient Stock) */
    if (validate_export_quantity(p->quantity, qty) != STATUS_SUCCESS) {
        printf("[!] Ton kho khong du de xuat (Hien co: %d, can xuat: %d)!\n", p->quantity, qty);
        return;
    }

    p->quantity -= qty;
    printf("[+] Xuat kho thanh cong! Ton kho con lai cua '%s': %d\n", p->id, p->quantity);

    log_transaction(trans_file, "EXPORT", p->id, qty, p->quantity);
    save_products_from_list(list, prod_file);
    printf("[*] Da ghi lich su giao dich va cap nhat tep kho hang.\n");
}

static void handle_report_menu(const ProductList *list) {
    int rep_running = 1;
    char rep_buf[32];

    while (rep_running) {
        printf("\n========================================================================\n");
        printf("                        BAO CAO & THONG KE KHO HANG                     \n");
        printf("========================================================================\n");
        printf(" [1] Thong ke ton kho (Xuat tong so luong, gia tri - report_statistics)\n");
        printf(" [2] Canh bao san pham sap het hang (Ton kho <= nguong dinh muc)\n");
        printf(" [3] Danh sach san pham da het hang (Ton kho = 0)\n");
        printf(" [4] Co cau nganh hang & gia tri ton theo danh muc\n");
        printf(" [5] Xuat toan bo bao cao ra tep van ban (inventory_report.txt)\n");
        printf(" [0] Quay lai menu chinh\n");
        printf("------------------------------------------------------------------------\n");

        safe_read_line("Nhap lua chon cua ban [0-5]: ", rep_buf, sizeof(rep_buf));
        trim_whitespace(rep_buf);

        if (strlen(rep_buf) == 0) continue;

        int rep_choice = -1;
        if (safe_str_to_int(rep_buf, &rep_choice) != STATUS_SUCCESS) {
            printf("[!] Lua chon khong hop le! Vui long nhap so tu 0 den 5.\n");
            continue;
        }

        switch (rep_choice) {
            case 1: {
                long long total_qty = 0;
                double total_val = 0.0;
                (report_statistics)(list, &total_qty, &total_val);
                break;
            }
            case 2: {
                char thresh_buf[16];
                safe_read_line("Nhap nguong so luong canh bao (mac dinh: 10): ", thresh_buf, sizeof(thresh_buf));
                trim_whitespace(thresh_buf);
                int thresh = DEFAULT_LOW_STOCK_THRESHOLD;
                if (strlen(thresh_buf) > 0) {
                    safe_str_to_int(thresh_buf, &thresh);
                    if (thresh < 0) thresh = DEFAULT_LOW_STOCK_THRESHOLD;
                }
                print_low_stock_report(list, thresh);
                break;
            }
            case 3:
                print_out_of_stock_report(list);
                break;
            case 4:
                print_category_summary_report(list);
                break;
            case 5: {
                const char *out_file = "inventory_report.txt";
                if (export_inventory_report_to_file(list, out_file, DEFAULT_LOW_STOCK_THRESHOLD) == STATUS_SUCCESS) {
                    printf("[+] Xuat bao cao thanh cong ra tep: %s\n", out_file);
                } else {
                    printf("[-] Xuat bao cao that bai!\n");
                }
                break;
            }
            case 0:
                rep_running = 0;
                break;
            default:
                printf("[!] Lua chon khong hop le! Vui long chon tu 0 den 5.\n");
                break;
        }
    }
}

/* =========================================================================
 * 6. HÀM MAIN & VÒNG LẶP MENU TƯƠNG TÁC
 * ========================================================================= */

int main(int argc, char *argv[]) {
    /* Khoi tao danh sach san pham dong */
    ProductList list;
    if (product_list_init(&list) != STATUS_SUCCESS) {
        fprintf(stderr, "[!] Loi cap phat bo nho dong cho ProductList.\n");
        return 1;
    }

    char prod_file[256];
    char trans_file[256];
    resolve_data_paths(prod_file, trans_file, sizeof(prod_file));

    /* Nap du lieu tu tep products.txt */
    load_products_to_list(&list, prod_file);

    /* Ho tro che do chay lenh dong CLI (Test / Batch scripts) */
    if (argc > 1) {
        int exit_code = 0;
        if (strcmp(argv[1], "--list") == 0) {
            print_product_table(&list);
        } else if (strcmp(argv[1], "--test") == 0) {
            run_all_unit_tests();
        } else if (strcmp(argv[1], "--validate") == 0) {
            display_and_validate_file(prod_file);
        } else if (strcmp(argv[1], "--history") == 0) {
            print_history(trans_file);
        } else if (strcmp(argv[1], "--report") == 0) {
            print_inventory_summary_report(&list, DEFAULT_LOW_STOCK_THRESHOLD);
            print_category_summary_report(&list);
        } else {
            fprintf(stderr, "Tham so khong hop le. Ho tro: --list, --test, --validate, --history, --report\n");
            exit_code = 1;
        }
        product_list_free(&list);
        return exit_code;
    }

    /* Vong lap Menu tuong tac nguoi dung */
    int running = 1;
    char input_buf[64];

    while (running) {
        printf("\n========================================================================\n");
        printf("          HE THONG QUAN LY KHO HANG & KIEM TOAN AN TOAN BO NHO           \n");
        printf("========================================================================\n");
        printf(" [1] Xem danh sach san pham trong kho (Bang chi tiet)\n");
        printf(" [2] Them san pham moi (Validate chat che 6 truong thong tin)\n");
        printf(" [3] Sua thong tin san pham (Theo ma ID)\n");
        printf(" [4] Xoa san pham khoi kho (Theo ma ID)\n");
        printf(" [5] Tim kiem san pham (Theo ma SP hoac tu khoa ten)\n");
        printf(" [6] Nhap kho (Import Stock - Chuyen doi so luong, chong tran so)\n");
        printf(" [7] Xuat kho (Export Stock - Kiem tra ton kho, chong am kho)\n");
        printf(" [8] Xem lich su giao dich (transactions.txt)\n");
        printf(" [9] Bao cao & Thong ke kho hang (Tong quan, Canh bao ton, Xuat file)\n");
        printf(" [10] Kiem toan du lieu tep tu dong (Validation File Audit)\n");
        printf(" [11] Chay bo kiem thu tu dong (All Unit Tests: Utils, Valid, Product, Report)\n");
        printf(" [0] Thoat chuong trinh & Luu du lieu\n");
        printf("------------------------------------------------------------------------\n");

        safe_read_line("Nhap lua chon cua ban [0-11]: ", input_buf, sizeof(input_buf));
        trim_whitespace(input_buf);

        if (strlen(input_buf) == 0) {
            continue;
        }

        int choice = -1;
        if (safe_str_to_int(input_buf, &choice) != STATUS_SUCCESS) {
            printf("[!] Lua chon khong hop le! Vui long nhap so tu 0 den 11.\n");
            continue;
        }

        switch (choice) {
            case 1:
                print_product_table(&list);
                break;
            case 2:
                handle_add_product(&list, prod_file);
                break;
            case 3:
                handle_update_product(&list, prod_file);
                break;
            case 4:
                handle_delete_product(&list, prod_file);
                break;
            case 5:
                handle_search_product(&list);
                break;
            case 6:
                handle_import_stock(&list, prod_file, trans_file);
                break;
            case 7:
                handle_export_stock(&list, prod_file, trans_file);
                break;
            case 8:
                print_history(trans_file);
                break;
            case 9:
                handle_report_menu(&list);
                break;
            case 10:
                display_and_validate_file(prod_file);
                break;
            case 11:
                run_all_unit_tests();
                break;
            case 0:
                printf("\n[*] Dang luu du lieu vao tep: %s...\n", prod_file);
                save_products_from_list(&list, prod_file);
                printf("[*] Da luu thanh cong. Thoat chuong trinh.\n");
                running = 0;
                break;
            default:
                printf("[!] Lua chon khong hop le! Vui long chon tu 0 den 11.\n");
                break;
        }
    }

    /* Giai phong toan bo bo nho dong an toan (Memory Safety - No Memory Leak) */
    product_list_free(&list);
    return 0;
}