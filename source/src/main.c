#include <stdio.h>
#include "../include/validation.h"

int main() {
    printf("Kiem tra ma hop le SP001: %d\n", validate_product_id("SP001"));
    printf("Kiem tra ma khong hop le (chua |): %d\n", validate_product_id("SP|01"));
    printf("Kiem tra so luong 10: %d\n", validate_quantity(10));
    printf("Kiem tra so luong -5: %d\n", validate_quantity(-5));
    return 0;
}
