#include "../include/models.h"
#include "../include/product.h"
#include "../include/validation.h"
#include <stdio.h>

void print_product_list(const ProductList *list) {
  printf("--- DANH SACH SAN PHAM KHO (Tong so: %zu | Suc chua: %zu) ---\n",
         list->count, list->capacity);
  for (size_t i = 0; i < list->count; ++i) {
    printf(
        "   [%zu] ID: %-10s | Ten: %-30s | Loai: %-20s | SL: %-4d | Gia: %.3f\n",
        i + 1, list->items[i].id, list->items[i].name, list->items[i].category,
        list->items[i].quantity, list->items[i].price);
  }
  printf("----------------------------------------------------------------\n");
}

int main() {
  printf("___________________________________________________________\n");
  printf("  KIEM THU MODULE VALIDATION\n");
  printf("___________________________________________________________\n");
  printf("Kiem tra ma hop le SP001: %d\n", validate_product_id("SP001"));
  printf("Kiem tra ma khong hop le (chua |): %d\n", validate_product_id("SP|01"));
  printf("Kiem tra so luong 10: %d\n", validate_quantity(10));
  printf("Kiem tra so luong -5: %d\n", validate_quantity(-5));
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

  /* 2. Thêm các sản phẩm hợp lệ */
  Product p1 = {"SP001", "Laptop Dell XPS 15", "Dien tu", 10, 1500.0};
  Product p2 = {"SP002", "Chuot Logitech MX Master", "Phu kien", 50, 99.9};
  Product p3 = {"SP003", "Ban phim Keychron K2", "Phu kien", 30, 85.5};

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
  Product p_dup = {"SP001", "Laptop Duplicate", "Dien tu", 5, 1000.0};
  int res_dup = add_product(&list, &p_dup);
  printf("Them trung ID SP001: %d\n", res_dup); /*Ky vong STATUS_ERR_DUPLICATE_ID = -11*/

  /* Test thêm sản phẩm chứa ký tự cấm '|' */
  Product p_bad_id = {"SP|004", "San pham sai ID", "Khac", 5, 10.0};
  printf("Them ID co ky tu cam 'SP|004' : " "%d\n", add_product(&list, &p_bad_id)); /*Ky vong STATUS_ERR_INVALID_ID = -2*/

  /* Test thêm sản phẩm có số lượng âm */
  Product p_bad_qty = {"SP004", "San pham am SL", "Khac", -5, 10.0};
  printf("Them so luong am '-5': %d\n", add_product(&list, &p_bad_qty)); /*Ky vong STATUS_ERR_INVALID_QTY = -3*/
  printf("\n");

  /* Hien thi danh sach san pham da them thanh cong */
  print_product_list(&list);

  /* 4. KIEM THU TASK 4: CHUC NANG SUA SAN PHAM */
  printf("\n___________________________________________________________\n");
  printf("  KIEM THU TASK 4: CHUC NANG SUA SAN PHAM\n");
  printf("___________________________________________________________\n");
  printf("Cap nhat SP002 -> Ten moi: 'Chuot Logitech MX Master 3S', Loai moi: 'Phu kien Cao cap', SL: 45, Gia: 105.0\n");
  int res_upd = update_product(&list, "SP002", "Chuot Logitech MX Master 3S", "Phu kien Cao cap", 45, 105.0);
  printf("Ket qua cap nhat SP002: %s\n", res_upd == STATUS_SUCCESS ? "Thanh cong" : "That bai");

  /* Test cập nhật sản phẩm không tồn tại SP999 */
  int res_upd_notfound = update_product(&list, "SP999", "Ten SP", "Loai", 10, 20.0);
  printf("Cap nhat SP999 khong ton tai : %d\n", res_upd_notfound); /*Ky vong STATUS_ERR_NOT_FOUND = -4*/
  printf("\n");

  /* Hiển thị danh sách sau khi cập nhật */
  print_product_list(&list);

  /* Giải phóng bộ nhớ động */
  product_list_free(&list);
  printf("\n[OK] Giai phong bo nho thanh cong.\n");
  printf("___________________________________________________________\n");

  return 0;
}
