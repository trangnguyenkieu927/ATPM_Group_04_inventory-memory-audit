#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../source/include/models.h"
#include "../../source/include/inventory.h"
#include "../../source/include/file_io.h"

typedef struct {
    const char *corpus_file;
    const char *description;
    int expected_status; /* 0: SUCCESS, hoặc < 0: ERROR */
    size_t min_loaded;
} CorpusCase;

int main(void) {
    printf("============================================================\n");
    printf("  SIM-002: MÔ PHỎNG FILE PARSER VÀ GIỚI HẠN BỘ ĐỆM          \n");
    printf("============================================================\n");

    CorpusCase cases[] = {
        {"tests/simulation/corpus/corpus_01_valid_6_fields.txt",
         "Dòng hợp lệ chuẩn 6 trường", STATUS_SUCCESS, 2},
        {"tests/simulation/corpus/corpus_02_valid_4_fields.txt",
         "Dòng cũ hợp lệ 4 trường (legacy format)", STATUS_SUCCESS, 2},
        {"tests/simulation/corpus/corpus_03_too_many_delimiters.txt",
         "Dòng có quá nhiều dấu phân cách (7+ pipes)", STATUS_ERR_FILE_IO, 0},
        {"tests/simulation/corpus/corpus_04_too_few_fields.txt",
         "Dòng có quá ít trường (1-3 trường)", STATUS_ERR_FILE_IO, 0},
        {"tests/simulation/corpus/corpus_05_line_over_512.txt",
         "Dòng dài hơn LINE_BUFFER_SIZE (512 bytes)", STATUS_ERR_FILE_IO, 0},
        {"tests/simulation/corpus/corpus_06_missing_newline_eof.txt",
         "Dòng cuối tệp không có ký tự xuống dòng (EOF)", STATUS_SUCCESS, 1},
        {"tests/simulation/corpus/corpus_07_embedded_nul_byte.txt",
         "Dòng có ký tự NUL ('\\0') nhúng giữa chuỗi", STATUS_ERR_FILE_IO, 0},
        {"tests/simulation/corpus/corpus_08_quantity_overflow.txt",
         "Số lượng vượt quá giới hạn INT_MAX", STATUS_ERR_FILE_IO, 0},
        {"tests/simulation/corpus/corpus_09_negative_quantity.txt",
         "Số lượng âm (< 0)", STATUS_ERR_FILE_IO, 0},
        {"tests/simulation/corpus/corpus_10_invalid_price.txt",
         "Đơn giá không hợp lệ (NaN, Infinity, ký tự chữ)", STATUS_ERR_FILE_IO, 0}
    };

    size_t num_cases = sizeof(cases) / sizeof(cases[0]);
    int pass_count = 0;

    for (size_t i = 0; i < num_cases; ++i) {
        Inventory inv;
        inventory_init(&inv);

        printf("\n[%02zu/10] Kiểm thử: %s\n", i + 1, cases[i].description);
        printf("       Tệp: %s\n", cases[i].corpus_file);

        int status = load_products_from_file(&inv, cases[i].corpus_file);
        printf("       Mã trả về: %d (Kỳ vọng: %s)\n",
               status, cases[i].expected_status == STATUS_SUCCESS ? "STATUS_SUCCESS (0)" : "LỖI (< 0)");
        printf("       Số sản phẩm đã nạp: %zu (Kỳ vọng tối thiểu: %zu)\n",
               inv.count, cases[i].min_loaded);

        int case_passed = 0;
        if (cases[i].expected_status == STATUS_SUCCESS) {
            if (status == STATUS_SUCCESS && inv.count >= cases[i].min_loaded) {
                case_passed = 1;
            }
        } else {
            /* Kỳ vọng parser từ chối an toàn */
            if (status != STATUS_SUCCESS) {
                case_passed = 1;
            }
        }

        if (case_passed) {
            printf("       -> ĐÁNH GIÁ: [PASS]\n");
            pass_count++;
        } else {
            printf("       -> ĐÁNH GIÁ: [DIFF/FAIL] Trạng thái không khớp kỳ vọng!\n");
        }

        inventory_free(&inv);
    }

    printf("\n============================================================\n");
    printf("  KẾT QUẢ TỔNG HỢP SIM-002: %d/%zu TEST CASES PASS\n", pass_count, num_cases);
    printf("============================================================\n");

    return (pass_count == (int)num_cases) ? 0 : 1;
}
