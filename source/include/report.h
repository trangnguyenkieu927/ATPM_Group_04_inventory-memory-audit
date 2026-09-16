#ifndef REPORT_H
#define REPORT_H

#include <stddef.h>
#include <stdio.h>
#include "models.h"

#define DEFAULT_LOW_STOCK_THRESHOLD 10
#define MAX_CATEGORY_SUMMARY_COUNT 32

/**
 * Cấu trúc thống kê theo danh mục sản phẩm.
 */
typedef struct {
    char category[MAX_CATEGORY_LEN];
    size_t product_count;           /* Số lượng mặt hàng thuộc danh mục */
    long long total_quantity;       /* Tổng số lượng tồn kho của danh mục */
    double total_value;             /* Tổng giá trị tồn kho của danh mục ($) */
} CategorySummary;

/**
 * Cấu trúc báo cáo tổng quan kho hàng.
 */
typedef struct {
    size_t total_products;          /* Tổng số mặt hàng (SKU) */
    long long total_quantity;       /* Tổng số lượng tồn kho */
    double total_inventory_value;   /* Tổng giá trị tồn kho ($) */
    size_t out_of_stock_count;      /* Số mặt hàng đã hết hàng (SL = 0) */
    size_t low_stock_count;         /* Số mặt hàng sắp hết hàng (SL <= ngưỡng) */
    Product most_expensive_product; /* Sản phẩm có đơn giá cao nhất */
    Product least_expensive_product;/* Sản phẩm có đơn giá thấp nhất */
} InventorySummary;

/**
 * Tính toán số liệu thống kê tổng quan kho hàng.
 * @param list Danh sách sản phẩm.
 * @param low_stock_threshold Ngưỡng cảnh báo sắp hết hàng.
 * @param summary Con trỏ lưu kết quả thống kê.
 * @return STATUS_SUCCESS hoặc STATUS_ERR_NULL_PTR.
 */
int calculate_inventory_summary(const ProductList *list, int low_stock_threshold, InventorySummary *summary);

/**
 * Tính toán thống kê theo từng danh mục loại sản phẩm.
 * @param list Danh sách sản phẩm.
 * @param summaries Mảng chứa kết quả danh mục.
 * @param max_summaries Dung lượng mảng tối đa.
 * @param out_count Số lượng danh mục tìm thấy.
 * @return STATUS_SUCCESS, STATUS_ERR_NULL_PTR hoặc STATUS_ERR_OVERFLOW.
 */
int calculate_category_summaries(const ProductList *list, CategorySummary *summaries, size_t max_summaries, size_t *out_count);

/**
 * In báo cáo tổng quan kho hàng ra màn hình console.
 * @param list Danh sách sản phẩm.
 * @param low_stock_threshold Ngưỡng cảnh báo sắp hết hàng.
 */
void print_inventory_summary_report(const ProductList *list, int low_stock_threshold);

/**
 * In danh sách các sản phẩm sắp hết hàng (tồn kho <= ngưỡng).
 * @param list Danh sách sản phẩm.
 * @param threshold Ngưỡng số lượng cảnh báo.
 */
void print_low_stock_report(const ProductList *list, int threshold);

/**
 * In danh sách các sản phẩm đã hết hàng (tồn kho == 0).
 * @param list Danh sách sản phẩm.
 */
void print_out_of_stock_report(const ProductList *list);

/**
 * In báo cáo phân loại theo danh mục sản phẩm.
 * @param list Danh sách sản phẩm.
 */
void print_category_summary_report(const ProductList *list);

/**
 * Xuất toàn bộ báo cáo tổng hợp ra tệp văn bản.
 * @param list Danh sách sản phẩm.
 * @param filepath Đường dẫn tệp cần ghi báo cáo.
 * @param low_stock_threshold Ngưỡng cảnh báo sắp hết hàng.
 * @return STATUS_SUCCESS hoặc STATUS_ERR_FILE_IO / STATUS_ERR_NULL_PTR.
 */
int export_inventory_report_to_file(const ProductList *list, const char *filepath, int low_stock_threshold);

/**
 * Thống kê tồn kho và xuất tổng số lượng, giá trị cùng các chỉ số thống kê cần thiết.
 * @param list Danh sách sản phẩm trong kho.
 * @param total_quantity Con trỏ nhận tổng số lượng tồn (có thể NULL).
 * @param total_value Con trỏ nhận tổng giá trị tồn kho (có thể NULL).
 * @return STATUS_SUCCESS hoặc STATUS_ERR_NULL_PTR nếu con trỏ list không hợp lệ.
 */
int (report_statistics)(const ProductList *list, long long *total_quantity, double *total_value);

/* Hàm hỗ trợ gọi nhanh thống kê với 1 tham số duy nhất */
static inline int report_statistics_default(const ProductList *list) {
    return (report_statistics)(list, NULL, NULL);
}

#define _REP_STAT_3(list, qty, val) (report_statistics)((list), (qty), (val))
#define _REP_STAT_1(list)           report_statistics_default(list)
#define _GET_REP_STAT_MACRO(_1, _2, _3, NAME, ...) NAME
#define report_statistics(...) _GET_REP_STAT_MACRO(__VA_ARGS__, _REP_STAT_3, _REP_STAT_2_UNUSED, _REP_STAT_1)(__VA_ARGS__)

#endif /* REPORT_H */
