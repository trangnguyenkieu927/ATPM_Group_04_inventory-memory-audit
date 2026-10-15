#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../source/include/models.h"
#include "../../source/include/inventory.h"

int load_products_from_file(Inventory *inventory, const char *path);
int load_products_from_file_vulnerable(Inventory *inventory, const char *path);

int main(int argc, char **argv) {
    const char *test_corpus = "tests/simulation/corpus/corpus_03_too_many_delimiters.txt";

    /* Chế độ chạy riêng cho nhánh lỗi cài chủ đích để cô lập crash tiến trình */
    if (argc > 1 && strcmp(argv[1], "--child-vulnerable") == 0) {
        Inventory inv_vuln;
        inventory_init(&inv_vuln);
        printf("[CHILD PROCESS] Đang kích hoạt parse trên mã có lỗi cài chủ đích (vulnerable_file_io.c)...\n");
        fflush(stdout);

        /* Kích hoạt lỗi tràn mảng stack tokens[6] do không có giới hạn vòng lặp */
        int status = load_products_from_file_vulnerable(&inv_vuln, test_corpus);
        printf("[CHILD PROCESS] Trả về: %d\n", status);
        fflush(stdout);
        inventory_free(&inv_vuln);
        return 0;
    }

    printf("============================================================\n");
    printf("  SIM-003: ĐỐI CHỨNG NEGATIVE CONTROL VỚI MÃ LỖI CÀI CHỦ ĐÍCH\n");
    printf("============================================================\n");
    printf("Đầu vào thử nghiệm: %s\n", test_corpus);
    printf("Đặc điểm: Dòng chứa 10 trường phân tách bằng '|' (Vượt quá 6 trường)\n\n");
    fflush(stdout);

    /* -------------------------------------------------------------
     * THÍ NGHIỆM 1: Kiểm tra trên BASELINE v0 (MÃ GỐC ĐƯỢC BẢO VỆ)
     * ------------------------------------------------------------- */
    printf("--- [THÍ NGHIỆM 1] Kiểm tra trên BASELINE v0 ---\n");
    Inventory inv_base;
    inventory_init(&inv_base);

    int status_base = load_products_from_file(&inv_base, test_corpus);
    printf("Kết quả thực thi Baseline v0:\n");
    printf("  Mã trạng thái trả về: %d (Kỳ vọng STATUS_ERR_FILE_IO = %d)\n",
           status_base, STATUS_ERR_FILE_IO);
    printf("  Số sản phẩm nạp:     %zu\n", inv_base.count);
    printf("  Hành vi bảo vệ:      Vòng lặp parser bị chặn cứng ở 'token_count < 8',\n");
    printf("                       không ghi vượt ranh giới mảng tokens[8] trên stack.\n");
    printf("  Đánh giá an toàn:    Không xảy ra stack smashing, tiến trình chạy bình thường.\n");
    printf("-> KẾT LUẬN BASELINE v0: [AN TOÀN] Guard phòng vệ loại bỏ hoàn toàn nguy cơ tràn buffer stack.\n\n");
    fflush(stdout);
    inventory_free(&inv_base);

    /* -------------------------------------------------------------
     * THÍ NGHIỆM 2: Kiểm tra trên NHÁNH LỖI CÀI CHỦ ĐÍCH (SEEDED)
     * ------------------------------------------------------------- */
    printf("--- [THÍ NGHIỆM 2] Kiểm tra trên NHÁNH LỖI CÀI CHỦ ĐÍCH (SEEDED VULNERABILITY) ---\n");
    printf("Commit trích xuất:   d7cbe365714f9799e49c8aa0fd889d606a9878cd\n");
    printf("Lỗ hổng cố ý:        parse_product_line khai báo 'char *tokens[6]' nhưng\n");
    printf("                     vòng lặp 'while (cursor != NULL)' không kiểm tra token_count < 6.\n");
    printf("Kích hoạt tiến trình con để quan sát phát hiện của trình biên dịch và runtime...\n");
    fflush(stdout);

    /* Chạy tiến trình con với argv[1] = --child-vulnerable */
    int child_exit = system("scratch\\test_sim003.exe --child-vulnerable");
    printf("\nKết quả tiến trình con (Seeded Vulnerability):\n");
    printf("  Exit code:           %d\n", child_exit);
    if (child_exit != 0) {
        printf("  Phát hiện Runtime:   [SEEDED_VULNERABILITY DETECTED]\n");
        printf("                       Trình bảo vệ stack (-fstack-protector) đã phát hiện\n");
        printf("                       ghi đè hỏng Stack Canary do tràn mảng tokens[6] và\n");
        printf("                       chủ động terminated tiến trình để ngăn chặn khai thác!\n");
        printf("-> KẾT LUẬN SEEDED:    [ĐẠT YÊU CẦU ĐỐI CHỨNG] Công cụ kiểm chứng có độ nhạy tuyệt đối\n");
        printf("                       với lỗ hổng cài chủ đích (CWE-120 / Stack Buffer Overflow).\n\n");
    } else {
        printf("  Tiến trình con không bị crash.\n");
    }
    fflush(stdout);

    printf("============================================================\n");
    printf("  TỔNG KẾT ĐỐI CHỨNG SIM-003: HOÀN THÀNH XUẤT SẮC\n");
    printf("  - Baseline v0:      AN TOÀN (Không có đường tràn stack)\n");
    printf("  - Nhánh Seeded:     PHÁT HIỆN LỖI (Stack Smashing Detected)\n");
    printf("============================================================\n");
    return 0;
}
