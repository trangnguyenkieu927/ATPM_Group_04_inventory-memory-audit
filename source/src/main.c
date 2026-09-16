#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/models.h"
#include "../include/validation.h"
#include "../include/product.h"
#include "../include/inventory.h"
#include "../include/utils.h"

/* In danh sách sản phẩm đẹp mắt */
void print_product_list(const ProductList *list) {
    if (list == NULL || list->count == 0) {
        printf("\n[!] Danh sach san pham hien dang RONG!\n");
        return;
    }
    printf("\n--- DANH SACH SAN PHAM KHO (Tong so: %zu | Suc chua: %zu) ---\n", list->count, list->capacity);
    for (size_t i = 0; i < list->count; ++i) {
        printf("   [%zu] ID: %-8s | Ten: %-26s | Loai: %-18s | Don vi: %-8s | SL: %-4d | Gia: %.3f $\n",
               i + 1, list->items[i].id, list->items[i].name, list->items[i].category, list->items[i].unit,
               list->items[i].quantity, list->items[i].price);
    }
    printf("------------------------------------------------------------------------------------------\n");
}

/* Sub-menu quản lý sản phẩm */
void print_product_menu() {
    printf("\n____________________________________________________________________________________________________\n");
    printf("                                SUB-MENU: QUAN LY SAN PHAM                  \n");
    printf("______________________________________________________________________________________________________\n");
    printf("  1. Hien thi danh sach san pham\n");
    printf("  2. Them san pham moi\n");
    printf("  3. Sua thong tin san pham\n");
    printf("  4. Xoa san pham theo ID\n");
    printf("  5. Tim kiem san pham theo ID\n");
    printf("  6. Tim kiem san pham theo Ten\n");
    printf("  0. Quay lai Menu chinh\n");
    printf("______________________________________________________________________________________________________\n");
}

/* Xử lý các chức năng Sub-menu Product */
void handle_product_management(ProductList *list) {
    char input_buf[128];
    int choice = -1;

    while (1) {
        print_product_menu();
        safe_read_line("Lua chon Product (0-6): ", input_buf, sizeof(input_buf));

        if (is_empty_or_whitespace(input_buf)) {
            printf("[!] Vui long nhap mot lua chon hop le.\n");
            continue;
        }

        if (safe_str_to_int(input_buf, &choice) != STATUS_SUCCESS) {
            printf("[!] Lua chon khong phai la so nguyen. Vui long thu lai!\n");
            continue;
        }

        if (choice == 0) {
            break; /* Quay lại Menu chính */
        }

        switch (choice) {
            case 1: {
                /* Hiển thị danh sách sản phẩm */
                print_product_list(list);
                break;
            }
            case 2: {
                /* Thêm sản phẩm mới */
                Product prod;
                memset(&prod, 0, sizeof(Product));

                printf("\n>>> THEM SAN PHAM MOI <<<\n");
                safe_read_line("Nhap Ma SP (ID)       : ", prod.id, sizeof(prod.id));
                safe_read_line("Nhap Ten SP           : ", prod.name, sizeof(prod.name));
                safe_read_line("Nhap Loai SP          : ", prod.category, sizeof(prod.category));
                safe_read_line("Nhap Don vi tinh      : ", prod.unit, sizeof(prod.unit));

                safe_read_line("Nhap So luong ton kho : ", input_buf, sizeof(input_buf));
                if (safe_str_to_int(input_buf, &prod.quantity) != STATUS_SUCCESS) {
                    printf("[ERR] So luong phai la so nguyen!\n");
                    break;
                }

                safe_read_line("Nhap Don gia ($)      : ", input_buf, sizeof(input_buf));
                if (safe_str_to_double(input_buf, &prod.price) != STATUS_SUCCESS) {
                    printf("[ERR] Don gia phai la so thuc!\n");
                    break;
                }

                int status = add_product(list, &prod);
                if (status == STATUS_SUCCESS) {
                    printf("[OK] Them san pham '%s' thanh cong!\n", prod.id);
                } else {
                    printf("[ERR] Them san pham thất bai! (Ma loi: %d)\n", status);
                }
                break;
            }
            case 3: {
                /* Sửa thông tin sản phẩm */
                char id[MAX_ID_LEN];
                char name[MAX_NAME_LEN];
                char category[MAX_CATEGORY_LEN];
                char unit[MAX_UNIT_LEN];
                int quantity = -1;
                double price = -1.0;

                printf("\n>>> SUA THONG TIN SAN PHAM <<<\n");
                safe_read_line("Nhap Ma SP can sua (ID): ", id, sizeof(id));
                if (find_product_by_id(list, id) == NULL) {
                    printf("[ERR] Khong tim thay san pham co mã '%s'!\n", id);
                    break;
                }

                printf("(De ngoac vuong ranh ney va an Enter de giu nguyen thong tin cu)\n");
                safe_read_line("Ten moi                 : ", name, sizeof(name));
                safe_read_line("Loai moi                : ", category, sizeof(category));
                safe_read_line("Don vi tinh moi         : ", unit, sizeof(unit));

                safe_read_line("So luong moi (-1 giu nguyen): ", input_buf, sizeof(input_buf));
                if (!is_empty_or_whitespace(input_buf)) {
                    safe_str_to_int(input_buf, &quantity);
                }

                safe_read_line("Don gia moi (-1.0 giu nguyen): ", input_buf, sizeof(input_buf));
                if (!is_empty_or_whitespace(input_buf)) {
                    safe_str_to_double(input_buf, &price);
                }

                const char *p_name = is_empty_or_whitespace(name) ? NULL : name;
                const char *p_cat = is_empty_or_whitespace(category) ? NULL : category;
                const char *p_unit = is_empty_or_whitespace(unit) ? NULL : unit;

                int status = update_product(list, id, p_name, p_cat, p_unit, quantity, price);
                if (status == STATUS_SUCCESS) {
                    printf("[OK] Cap nhat san pham '%s' thanh cong!\n", id);
                } else {
                    printf("[ERR] Cap nhat thất bai! (Ma loi: %d)\n", status);
                }
                break;
            }
            case 4: {
                /* Xóa sản phẩm */
                char id[MAX_ID_LEN];
                printf("\n>>> XOA SAN PHAM <<<\n");
                safe_read_line("Nhap Ma SP can xoa (ID): ", id, sizeof(id));
                int status = delete_product(list, id);
                if (status == STATUS_SUCCESS) {
                    printf("[OK] Xoa san pham '%s' thanh cong!\n", id);
                } else {
                    printf("[ERR] Khong tim thay hoac xoa that bai! (Ma loi: %d)\n", status);
                }
                break;
            }
            case 5: {
                /* Tìm kiếm theo ID */
                char id[MAX_ID_LEN];
                printf("\n>>> TIM KIEM SAN PHAM THEO ID <<<\n");
                safe_read_line("Nhap Ma SP (ID): ", id, sizeof(id));
                Product *p = find_product_by_id(list, id);
                if (p != NULL) {
                    printf("[FOUND] ID: %s | Ten: %s | Loai: %s | Don vi: %s | SL: %d | Gia: %.3f $\n",
                           p->id, p->name, p->category, p->unit, p->quantity, p->price);
                } else {
                    printf("[!] Khong tim thay san pham co ma '%s'!\n", id);
                }
                break;
            }
            case 6: {
                /* Tìm kiếm theo Tên */
                char keyword[MAX_NAME_LEN];
                Product results[16];
                size_t out_count = 0;

                printf("\n>>> TIM KIEM SAN PHAM THEO TEN <<<\n");
                safe_read_line("Nhap tu khoa ten SP: ", keyword, sizeof(keyword));
                int status = search_products_by_name(list, keyword, results, 16, &out_count);
                if (status == STATUS_SUCCESS) {
                    printf("Tim thay %zu san pham phu hop:\n", out_count);
                    for (size_t i = 0; i < out_count; ++i) {
                        printf("   [%zu] ID: %s | Ten: %s | Loai: %s | Don vi: %s | SL: %d | Gia: %.3f $\n",
                               i + 1, results[i].id, results[i].name, results[i].category, results[i].unit,
                               results[i].quantity, results[i].price);
                    }
                } else {
                    printf("[ERR] Tim kiem thất bai! (Ma loi: %d)\n", status);
                }
                break;
            }
            default:
                printf("[!] Lua chon '%d' khong hop le.\n", choice);
                break;
        }
    }
}

/* Sub-menu quản lý tồn kho */
void print_inventory_menu() {
    printf("\n______________________________________________________________________________________________________\n");
    printf("                                SUB-MENU: QUAN LY TON KHO (INVENTORY)                 \n");
    printf("______________________________________________________________________________________________________\n");
    printf("  1. Hien thi danh sach ton kho san pham\n");
    printf("  2. Nhap kho (Tang so luong ton kho)\n");
    printf("  3. Xuat kho (Giam so luong ton kho)\n");
    printf("  4. Cap nhat truc tiep so luong ton kho\n");
    printf("  0. Quay lai Menu chinh\n");
    printf("______________________________________________________________________________________________________\n");
}

/* Xử lý các chức năng Sub-menu Inventory */
void handle_inventory_management(ProductList *list) {
    char input_buf[128];
    int choice = -1;

    while (1) {
        print_inventory_menu();
        safe_read_line("Lua chon Inventory (0-4): ", input_buf, sizeof(input_buf));

        if (is_empty_or_whitespace(input_buf)) {
            printf("[!] Vui long nhap mot lua chon hop le.\n");
            continue;
        }

        if (safe_str_to_int(input_buf, &choice) != STATUS_SUCCESS) {
            printf("[!] Lua chon khong phai la so nguyen. Vui long thu lai!\n");
            continue;
        }

        if (choice == 0) {
            break; /* Quay lại Menu chính */
        }

        switch (choice) {
            case 1: {
                /* Hiển thị danh sách tồn kho */
                print_product_list(list);
                break;
            }
            case 2: {
                /* Nhập kho (Import Stock) */
                char id[MAX_ID_LEN];
                int qty = 0;

                printf("\n>>> NHAP KHO SAN PHAM (IMPORT STOCK) <<<\n");
                safe_read_line("Nhap Ma SP (ID): ", id, sizeof(id));
                const Product *p = find_product(list, id);
                if (p == NULL) {
                    printf("[ERR] Khong tim thay san pham co ma '%s'!\n", id);
                    break;
                }
                printf("San pham: %s | Ton kho hien tai: %d %s\n", p->name, p->quantity, p->unit);

                safe_read_line("Nhap so luong can NHAP THEM: ", input_buf, sizeof(input_buf));
                if (safe_str_to_int(input_buf, &qty) != STATUS_SUCCESS || qty <= 0) {
                    printf("[ERR] So luong nhap phai la so nguyen duong (> 0)!\n");
                    break;
                }

                int status = import_stock(list, id, qty);
                if (status == STATUS_SUCCESS) {
                    const Product *updated_p = find_product(list, id);
                    printf("[OK] Nhap kho thanh cong! Ton kho moi cua '%s': %d %s\n",
                           id, updated_p ? updated_p->quantity : 0, updated_p ? updated_p->unit : "");
                } else if (status == STATUS_ERR_OVERFLOW) {
                    printf("[ERR] Loi tran so nguyen (Integer Overflow) khi nhap kho!\n");
                } else {
                    printf("[ERR] Nhap kho that bai! (Ma loi: %d)\n", status);
                }
                break;
            }
            case 3: {
                /* Xuất kho (Export Stock) */
                char id[MAX_ID_LEN];
                int qty = 0;

                printf("\n>>> XUAT KHO SAN PHAM (EXPORT STOCK) <<<\n");
                safe_read_line("Nhap Ma SP (ID): ", id, sizeof(id));
                const Product *p = find_product(list, id);
                if (p == NULL) {
                    printf("[ERR] Khong tim thay san pham co ma '%s'!\n", id);
                    break;
                }
                printf("San pham: %s | Ton kho hien tai: %d %s\n", p->name, p->quantity, p->unit);

                safe_read_line("Nhap so luong can XUAT: ", input_buf, sizeof(input_buf));
                if (safe_str_to_int(input_buf, &qty) != STATUS_SUCCESS || qty <= 0) {
                    printf("[ERR] So luong xuat phai la so nguyen duong (> 0)!\n");
                    break;
                }

                int status = export_stock(list, id, qty);
                if (status == STATUS_SUCCESS) {
                    const Product *updated_p = find_product(list, id);
                    printf("[OK] Xuat kho thanh cong! Ton kho moi cua '%s': %d %s\n",
                           id, updated_p ? updated_p->quantity : 0, updated_p ? updated_p->unit : "");
                } else if (status == STATUS_ERR_INSUFFICIENT_STOCK) {
                    printf("[ERR] KHONG DU TON KHO! (Ton kho hien tai: %d, So luong yeu cau xuat: %d)\n",
                           p->quantity, qty);
                } else {
                    printf("[ERR] Xuat kho that bai! (Ma loi: %d)\n", status);
                }
                break;
            }
            case 4: {
                /* Cập nhật tồn kho trực tiếp (Update Stock) */
                char id[MAX_ID_LEN];
                int new_qty = 0;

                printf("\n>>> CAP NHAT TRUC TIEP TON KHO <<<\n");
                safe_read_line("Nhap Ma SP (ID): ", id, sizeof(id));
                const Product *p = find_product(list, id);
                if (p == NULL) {
                    printf("[ERR] Khong tim thay san pham co ma '%s'!\n", id);
                    break;
                }
                printf("San pham: %s | Ton kho hien tai: %d %s\n", p->name, p->quantity, p->unit);

                safe_read_line("Nhap so luong ton kho MOI (>= 0): ", input_buf, sizeof(input_buf));
                if (safe_str_to_int(input_buf, &new_qty) != STATUS_SUCCESS || new_qty < 0) {
                    printf("[ERR] So luong ton kho phai la so nguyen >= 0!\n");
                    break;
                }

                int status = update_stock(list, id, new_qty);
                if (status == STATUS_SUCCESS) {
                    printf("[OK] Cap nhat ton kho thanh cong! Ton kho moi cua '%s': %d %s\n",
                           id, new_qty, p->unit);
                } else {
                    printf("[ERR] Cap nhat ton kho that bai! (Ma loi: %d)\n", status);
                }
                break;
            }
            default:
                printf("[!] Lua chon '%d' khong hop le.\n", choice);
                break;
        }
    }
}

/* Hiển thị Menu chính của hệ thống quản lý kho */
void print_main_menu() {
    printf("\n______________________________________________________________________________________________________\n");
    printf("                                  HE THONG QUAN LY KHO (INVENTORY MANAGEMENT)                             \n");
    printf("______________________________________________________________________________________________________\n");
    printf("  1. Quan ly san pham (Product Management)\n");
    printf("  2. Quan ly ton kho (Inventory Management)\n");
    printf("  3. Doc / Ghi du lieu file (File I/O)\n");
    printf("  0. Thoat chuong trinh (Exit)\n");
    printf("______________________________________________________________________________________________________\n");
}

int main() {
    ProductList product_list;
    if (product_list_init(&product_list) != STATUS_SUCCESS) {
        printf("[ERR] Khong thể khoi tao danh sach san pham!\n");
        return 1;
    }

    char input_buf[64];
    int choice = -1;

    while (1) {
        print_main_menu();
        safe_read_line("Nguoi dung chon (0-3): ", input_buf, sizeof(input_buf));

        if (is_empty_or_whitespace(input_buf)) {
            printf("[!] Vui long nhap mot lua chon hop le.\n");
            continue;
        }

        if (safe_str_to_int(input_buf, &choice) != STATUS_SUCCESS) {
            printf("[!] Lua chon khong phai la so nguyen. Vui long thu lai!\n");
            continue;
        }

        if (choice == 0) {
            printf("\n[OK] Cam on ban da su dung chuong trinh. Tam biet!\n");
            break;
        }

        switch (choice) {
            case 1:
                /* Gọi Sub-menu Quản lý sản phẩm */
                handle_product_management(&product_list);
                break;
            case 2:
                /* Gọi Sub-menu Quản lý tồn kho */
                handle_inventory_management(&product_list);
                break;
            case 3:
                printf("\n[MENU 3] Chuc nang Doc/Ghi File (File I/O) - Dang tich hop...\n");
                break;
            default:
                printf("\n[!] Lua chon '%d' khong nam trong menu. Vui long chon tu 0 den 3.\n", choice);
                break;
        }
    }

    /* Giải phóng bộ nhớ động trước khi thoát */
    product_list_free(&product_list);
    return 0;
}