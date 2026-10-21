/*
 * SEEDED_VULNERABILITY: Stack Buffer Overflow (CWE-120 / CWE-787)
 * Mục đích: Mô phỏng hành vi lỗi cài chủ đích trên nhánh `audit/v0-file-io-vulnerable`
 * Cơ chế: Mảng `char *tokens[6]` trên stack bị ghi đè khi chuỗi đầu vào có nhiều hơn 6 trường,
 *         do vòng lặp strtok_r không kiểm tra `token_count < 6`.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void vulnerable_parse_tokens(char *line) {
    char *tokens[6]; // Cố ý khai báo chỉ có 6 phần tử
    int count = 0;
    char *saveptr = NULL;
    char *token = strtok_r(line, "|", &saveptr);
    
    // LỖ HỔNG CÀI CHỦ ĐÍCH: Không kiểm tra count < 6!
    while (token != NULL) {
        tokens[count++] = token; // Ghi đè stack frame nếu count >= 6!
        token = strtok_r(NULL, "|", &saveptr);
    }
    printf("[SEEDED] Đã parse %d tokens trên stack buffer 6 phần tử\n", count);
}

int main(int argc, char *argv[]) {
    printf("============================================================\n");
    printf(" [SEEDED_VULNERABILITY] STACK BUFFER OVERFLOW DEMO (CWE-120)\n");
    printf("============================================================\n");

    // Chuỗi đầu vào có 10 trường phân tách bởi '|'
    char input_overflow[] = "F1|F2|F3|F4|F5|F6|F7|F8|F9|F10\n";
    printf("Kích hoạt hàm dễ bị tổn thương với 10 tokens...\n");
    vulnerable_parse_tokens(input_overflow);
    printf("[CẢNH BÁO] Nếu dòng này in ra, stack protector chưa chặn được tràn buffer.\n");
    return 0;
}
