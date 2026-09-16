#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/validation.h"

static void safe_read_line(const char *prompt, char *buffer, size_t size) {
    printf("%s", prompt);
    if (fgets(buffer, (int)size, stdin) != NULL) {
        size_t len = strlen(buffer);
        while (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r')) {
            buffer[len - 1] = '\0';
            len--;
        }
    } else {
        buffer[0] = '\0';
    }
}

static void test_validate_id_case(const char *label, const char *id, int expected_status) {
    int status = validate_id(id);
    printf("%-35s | Ma: %-15s | Ket qua: %-12s | %s\n",
           label,
           (id == NULL ? "NULL" : id),
           (status == STATUS_SUCCESS ? "HOP LE" : "KHONG HOP LE"),
           (status == expected_status ? "[PASS]" : "[FAIL]"));
}

int main() {
    printf("========================================================================================\n");
    printf("                  DEMO / TEST: TINH NANG KIEM TRA MA SAN PHAM                           \n");
    printf("========================================================================================\n");

    printf("\n--- Chay cac ca kiem thu tu dong (Test Cases) ---\n");
    test_validate_id_case("Ma tieu chuan hop le", "SP001", STATUS_SUCCESS);
    test_validate_id_case("Ma gom chu va so hop le", "LAPTOP123", STATUS_SUCCESS);
    test_validate_id_case("Ma chua ky tu phan tach '|'", "SP|01", STATUS_ERR_INVALID_ID);
    test_validate_id_case("Ma chua dau phay ','", "SP,01", STATUS_ERR_INVALID_ID);
    test_validate_id_case("Ma chua dau cham phay ';'", "SP;01", STATUS_ERR_INVALID_ID);
    test_validate_id_case("Ma chua khoang trang", "SP 001", STATUS_ERR_INVALID_ID);
    test_validate_id_case("Ma rong (empty string)", "", STATUS_ERR_INVALID_ID);
    test_validate_id_case("Con tro NULL", NULL, STATUS_ERR_NULL_PTR);
    test_validate_id_case("Ma vuot qua do dai MAX_ID_LEN", "SP01234567890123456789", STATUS_ERR_INVALID_ID);

    printf("\n--- Kiem tra truc tiep tu ban phim ---\n");
    char input_id[256];
    safe_read_line("Nhap ma san pham can kiem tra: ", input_id, sizeof(input_id));

    int status = validate_id(input_id);
    if (status == STATUS_SUCCESS) {
        printf("=> Ket qua: Ma '%s' HOP LE [OK]!\n", input_id);
    } else {
        printf("=> Ket qua: Ma '%s' KHONG HOP LE (Ma loi: %d) [ERROR]!\n", input_id, status);
    }

    printf("========================================================================================\n");
    return 0;
}
