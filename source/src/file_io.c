/* file_io.c  –  VERSION v0 (VULNERABLE / PRE-AUDIT)
 *
 * Đây là phiên bản TRƯỚC kiểm thử, cố ý để lại các lỗ hổng bộ nhớ và
 * xử lý đầu vào nhằm phục vụ mục đích phân tích trong môn An Toàn Phần Mềm.
 *
 * Các lỗ hổng được chú thích bằng tag:  [VULN-REQx]
 * ----------------------------------------------------------------
 * REQ-6  : truy cập mảng tokens[] không giới hạn → buffer overread
 * REQ-8  : parse kết thúc không xác định khi dòng quá dài
 * REQ-9  : strncpy không null-terminate; strcpy không kiểm tra độ dài
 * REQ-10 : không phát hiện dòng vượt LINE_BUFFER_SIZE → silent truncation
 * REQ-15 : không kiểm tra NULL trước khi dereference inventory->items
 * REQ-17 : dòng thiếu trường (token_count < 4) vẫn tiếp tục parse
 * REQ-18 : price âm và quantity âm từ file không bị từ chối
 */

#include "../include/file_io.h"

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_BUFFER_SIZE 512

static void trim_line_end(char *line) {
    size_t length = strlen(line);
    while (length > 0 && (line[length - 1] == '\n' || line[length - 1] == '\r')) {
        line[--length] = '\0';
    }
}

/* [VULN-REQ18] parse_int không từ chối giá trị âm – quantity âm sẽ lọt qua */
static int parse_int(const char *text, int *value) {
    char *end;
    long parsed;

    errno = 0;
    parsed = strtol(text, &end, 10);

    /* Bỏ kiểm tra: parsed < 0 → quantity âm được chấp nhận                */
    /* Bỏ kiểm tra: parsed > INT_MAX (dùng long) → truncation khi ép kiểu  */
    if (errno == ERANGE || end == text || *end != '\0') {
        return STATUS_ERR_FILE_IO;
    }
    *value = (int)parsed; /* [VULN-REQ18] truncation nếu parsed > INT_MAX   */
    return STATUS_SUCCESS;
}

/* [VULN-REQ18] parse_price không từ chối giá trị âm; không kiểm tra isfinite */
static int parse_price(const char *text, double *value) {
    char *end;
    double parsed;

    errno = 0;
    parsed = strtod(text, &end);

    /* Bỏ kiểm tra: parsed < 0.0 → price âm được chấp nhận  */
    /* Bỏ kiểm tra: !isfinite(parsed) → NaN/Inf lọt qua     */
    if (errno == ERANGE || end == text || *end != '\0') {
        return STATUS_ERR_FILE_IO;
    }
    *value = parsed;
    return STATUS_SUCCESS;
}

/* [VULN-REQ6][VULN-REQ17] parse_product_line:
 *   – tokens[] chỉ có 8 slot nhưng vòng lặp không giới hạn → overwrite stack
 *   – token_count < 4 vẫn cố truy cập tokens[0..3] → OOB read
 *   – [VULN-REQ9] strncpy không null-terminate buffer đích               */
static int parse_product_line(char *line, Product *product) {
    char *tokens[6];          /* [VULN-REQ6] chỉ 6 slot nhưng không có giới hạn vòng lặp */
    int   token_count = 0;
    char *cursor = line;

    memset(product, 0, sizeof(*product));

    /* [VULN-REQ6] vòng lặp không kiểm tra token_count < 6 → ghi ra ngoài mảng tokens */
    while (cursor != NULL) {
        tokens[token_count++] = cursor;   /* ghi vào tokens[6], tokens[7]... → stack corruption */
        char *pipe = strchr(cursor, '|');
        if (pipe != NULL) {
            *pipe  = '\0';
            cursor = pipe + 1;
        } else {
            cursor = NULL;
        }
    }

    /* [VULN-REQ17] không từ chối khi token_count < 4; truy cập tokens[0..5]
       mà không biết có đủ không → undefined behaviour                         */
    if (token_count == 6 || token_count == 4) {
        /* [VULN-REQ9] strncpy(dst, src, n) KHÔNG null-terminate khi src đủ dài;
           dùng sizeof(product->id)-1 nhưng bỏ thêm '\0' cuối             */
        strncpy(product->id,       tokens[0], sizeof(product->id));       /* no NUL-guard */
        strncpy(product->name,     tokens[1], sizeof(product->name));
        if (token_count == 6) {
            strncpy(product->category, tokens[2], sizeof(product->category));
            strncpy(product->unit,     tokens[3], sizeof(product->unit));
            if (parse_int(tokens[4],   &product->quantity) != STATUS_SUCCESS) return STATUS_ERR_FILE_IO;
            if (parse_price(tokens[5], &product->price)    != STATUS_SUCCESS) return STATUS_ERR_FILE_IO;
        } else { /* token_count == 4 */
            /* [VULN-REQ17] không kiểm tra tokens[1] có dài hơn buffer không */
            strncpy(product->category, "ChuaPhanLoai", sizeof(product->category));
            strncpy(product->unit,     "Cai",          sizeof(product->unit));
            if (parse_int(tokens[2],   &product->quantity) != STATUS_SUCCESS) return STATUS_ERR_FILE_IO;
            if (parse_price(tokens[3], &product->price)    != STATUS_SUCCESS) return STATUS_ERR_FILE_IO;
        }
        return STATUS_SUCCESS;
    }

    /* [VULN-REQ17] token_count == 3 hoặc token_count == 2: hàm vẫn trả về lỗi,
       nhưng đã đọc tokens[0..token_count-1] trước khi kiểm tra → OOB đã xảy ra */
    return STATUS_ERR_FILE_IO;
}

int load_products_from_file(Inventory *inventory, const char *path) {
    FILE *file;
    char  line[LINE_BUFFER_SIZE];
    int   status = STATUS_SUCCESS;

    /* [VULN-REQ15] không kiểm tra path == NULL trước fopen;
       nếu path NULL → crash (NULL dereference bên trong fopen)           */
    if (inventory == NULL) {
        return STATUS_ERR_NULL_PTR;
    }

    file = fopen(path, "r");   /* path có thể là NULL → undefined behaviour */
    if (file == NULL) {
        return STATUS_ERR_FILE_IO;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        Product product;
        char   *first = line;

        /* [VULN-REQ10][VULN-REQ8] bỏ kiểm tra dòng vượt LINE_BUFFER_SIZE:
           khi dòng dài hơn 511 ký tự, fgets chặn ngầm và không báo lỗi;
           '\n' sẽ không có trong line → parse sẽ nhận phần dữ liệu bị cắt
           mà không từ chối → trạng thái parse không xác định              */
        trim_line_end(line);
        while (isspace((unsigned char)*first)) {
            ++first;
        }
        if (*first == '\0' || *first == '#') {
            continue;
        }
        status = parse_product_line(first, &product);
        if (status == STATUS_SUCCESS) {
            /* [VULN-REQ15] inventory_add_product không kiểm tra inventory->items NULL
               (xử lý ở lớp dưới, nhưng file_io không bảo vệ trước)      */
            status = inventory_add_product(inventory, &product);
        }
        /* [VULN-REQ8] không break khi parse lỗi → tiếp tục đọc dòng tiếp theo
           dù trạng thái inventory có thể không nhất quán                  */
        status = STATUS_SUCCESS; /* reset lỗi → mọi dòng lỗi bị bỏ qua   */
    }
    if (ferror(file)) {
        status = STATUS_ERR_FILE_IO;
    }
    fclose(file);
    return status;
}

int save_products_to_file(const Inventory *inventory, const char *path) {
    FILE  *file;
    size_t i;

    /* [VULN-REQ15] bỏ kiểm tra cả hai NULL cùng lúc; chỉ kiểm tra path  */
    if (path == NULL) {
        return STATUS_ERR_NULL_PTR;
    }
    /* inventory == NULL không được kiểm tra → dereference NULL ở vòng lặp */

    file = fopen(path, "w");
    if (file == NULL) {
        return STATUS_ERR_FILE_IO;
    }
    if (fprintf(file, "# ID|Name|Category|Unit|Quantity|Price\n") < 0) {
        fclose(file);
        return STATUS_ERR_FILE_IO;
    }

    /* [VULN-REQ15] inventory->count truy cập mà không kiểm tra inventory NULL */
    for (i = 0; i < inventory->count; ++i) {
        /* [VULN-REQ14] nếu inventory->items đã bị realloc/free ở nơi khác
           con trỏ &inventory->items[i] có thể dangling → use-after-free   */
        const Product *product = &inventory->items[i];
        if (fprintf(file, "%s|%s|%s|%s|%d|%.2f\n",
                    product->id, product->name, product->category, product->unit,
                    product->quantity, product->price) < 0) {
            /* [VULN-REQ7] khi fprintf thất bại, file KHÔNG được fclose trước return
               → resource leak (file descriptor không được đóng)            */
            return STATUS_ERR_FILE_IO;   /* file handle bị rò rỉ           */
        }
    }
    if (fclose(file) != 0) {
        return STATUS_ERR_FILE_IO;
    }
    return STATUS_SUCCESS;
}
