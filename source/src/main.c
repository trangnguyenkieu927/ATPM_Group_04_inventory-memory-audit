#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/models.h"
#include "../include/utils.h"

/* Hiển thị Menu chính của hệ thống quản lý kho */
void print_main_menu() {
    printf("\n=======================================================\n");
    printf("     HE THONG QUAN LY KHO (INVENTORY MANAGEMENT)       \n");
    printf("=======================================================\n");
    printf("  1. Quan ly san pham (Product Management)\n");
    printf("  2. Quan ly ton kho (Inventory Management)\n");
    printf("  3. Doc / Ghi du lieu file (File I/O)\n");
    printf("  0. Thoat chuong trinh (Exit)\n");
    printf("=======================================================\n");
}

int main() {
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
                printf("\n[MENU 1] Chuc nang Quan ly San pham (Product Management) - Dang tich hop...\n");
                break;
            case 2:
                printf("\n[MENU 2] Chuc nang Quan ly Ton kho (Inventory Management) - Dang tich hop...\n");
                break;
            case 3:
                printf("\n[MENU 3] Chuc nang Doc/Ghi File (File I/O) - Dang tich hop...\n");
                break;
            default:
                printf("\n[!] Lua chon '%d' khong nam trong menu. Vui long chon tu 0 den 3.\n", choice);
                break;
        }
    }

    return 0;
}