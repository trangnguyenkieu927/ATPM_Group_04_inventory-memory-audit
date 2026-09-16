#include "../include/report.h"
#include "../include/utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int calculate_inventory_summary(const ProductList *list, int low_stock_threshold, InventorySummary *summary) {
    if (list == NULL || summary == NULL) {
        return STATUS_ERR_NULL_PTR;
    }

    memset(summary, 0, sizeof(*summary));
    summary->total_products = list->count;

    if (list->count == 0 || list->items == NULL) {
        return STATUS_SUCCESS;
    }

    summary->most_expensive_product = list->items[0];
    summary->least_expensive_product = list->items[0];

    for (size_t i = 0; i < list->count; ++i) {
        const Product *p = &list->items[i];
        summary->total_quantity += p->quantity;
        summary->total_inventory_value += (double)p->quantity * p->price;

        if (p->quantity == 0) {
            summary->out_of_stock_count++;
        } else if (p->quantity <= low_stock_threshold) {
            summary->low_stock_count++;
        }

        if (p->price > summary->most_expensive_product.price) {
            summary->most_expensive_product = *p;
        }
        if (p->price < summary->least_expensive_product.price) {
            summary->least_expensive_product = *p;
        }
    }

    return STATUS_SUCCESS;
}

int calculate_category_summaries(const ProductList *list, CategorySummary *summaries, size_t max_summaries, size_t *out_count) {
    if (list == NULL || summaries == NULL || out_count == NULL) {
        return STATUS_ERR_NULL_PTR;
    }

    *out_count = 0;
    if (list->count == 0 || list->items == NULL) {
        return STATUS_SUCCESS;
    }

    for (size_t i = 0; i < list->count; ++i) {
        const Product *p = &list->items[i];
        int found_idx = -1;

        for (size_t j = 0; j < *out_count; ++j) {
            if (strcmp(summaries[j].category, p->category) == 0) {
                found_idx = (int)j;
                break;
            }
        }

        if (found_idx >= 0) {
            summaries[found_idx].product_count++;
            summaries[found_idx].total_quantity += p->quantity;
            summaries[found_idx].total_value += (double)p->quantity * p->price;
        } else {
            if (*out_count >= max_summaries) {
                return STATUS_ERR_OVERFLOW;
            }

            safe_string_copy(summaries[*out_count].category,
                             sizeof(summaries[*out_count].category),
                             p->category);
            summaries[*out_count].product_count = 1;
            summaries[*out_count].total_quantity = p->quantity;
            summaries[*out_count].total_value = (double)p->quantity * p->price;
            (*out_count)++;
        }
    }

    return STATUS_SUCCESS;
}

void print_inventory_summary_report(const ProductList *list, int low_stock_threshold) {
    if (list == NULL) {
        printf("[!] Danh sach san pham rong hoac NULL.\n");
        return;
    }

    InventorySummary summary;
    if (calculate_inventory_summary(list, low_stock_threshold, &summary) != STATUS_SUCCESS) {
        printf("[!] Khong the tao bao cao tong quan kho hang.\n");
        return;
    }

    printf("\n========================================================================\n");
    printf("                     BAO CAO TONG QUAN KHO HANG                         \n");
    printf("========================================================================\n");
    printf(" - Tong so mat hang quan ly (SKU)   : %zu san pham\n", summary.total_products);
    printf(" - Tong so luong hang ton kho       : %lld san pham\n", summary.total_quantity);
    printf(" - Tong gia tri hang ton kho        : %.2f $\n", summary.total_inventory_value);
    printf(" - So mat hang da het hang (SL = 0) : %zu san pham\n", summary.out_of_stock_count);
    printf(" - So mat hang sap het hang (<= %d) : %zu san pham\n", low_stock_threshold, summary.low_stock_count);

    if (summary.total_products > 0) {
        printf(" - San pham gia cao nhat            : %s (%s - %.2f $)\n",
               summary.most_expensive_product.name,
               summary.most_expensive_product.id,
               summary.most_expensive_product.price);
        printf(" - San pham gia thap nhat           : %s (%s - %.2f $)\n",
               summary.least_expensive_product.name,
               summary.least_expensive_product.id,
               summary.least_expensive_product.price);
    }
    printf("========================================================================\n\n");
}

void print_low_stock_report(const ProductList *list, int threshold) {
    if (list == NULL || list->count == 0) {
        printf("\n[i] Khong co san pham nao trong kho.\n");
        return;
    }

    printf("\n=================================================================================================\n");
    printf("                     CANH BAO: DANH SACH SAN PHAM SAP HET HANG (TON KHO <= %d)                   \n", threshold);
    printf("=================================================================================================\n");
    printf("%-4s | %-10s | %-32s | %-16s | %-8s | %-8s | %-10s\n",
           "STT", "MA SP", "TEN SAN PHAM", "LOAI SP", "DON VI", "TON KHO", "DON GIA ($)");
    printf("-------------------------------------------------------------------------------------------------\n");

    size_t count = 0;
    for (size_t i = 0; i < list->count; ++i) {
        const Product *p = &list->items[i];
        if (p->quantity > 0 && p->quantity <= threshold) {
            count++;
            printf("%-4zu | %-10s | %-32s | %-16s | %-8s | %-8d | %-10.2f\n",
                   count, p->id, p->name, p->category, p->unit, p->quantity, p->price);
        }
    }

    if (count == 0) {
        printf("         [OK] Khong co san pham nao duoi nguong canh bao %d san pham.\n", threshold);
    }
    printf("=================================================================================================\n\n");
}

void print_out_of_stock_report(const ProductList *list) {
    if (list == NULL || list->count == 0) {
        printf("\n[i] Khong co san pham nao trong kho.\n");
        return;
    }

    printf("\n=================================================================================================\n");
    printf("                               DANH SACH SAN PHAM DA HET HANG (TON KHO = 0)                      \n");
    printf("=================================================================================================\n");
    printf("%-4s | %-10s | %-32s | %-16s | %-8s | %-8s | %-10s\n",
           "STT", "MA SP", "TEN SAN PHAM", "LOAI SP", "DON VI", "TON KHO", "DON GIA ($)");
    printf("-------------------------------------------------------------------------------------------------\n");

    size_t count = 0;
    for (size_t i = 0; i < list->count; ++i) {
        const Product *p = &list->items[i];
        if (p->quantity == 0) {
            count++;
            printf("%-4zu | %-10s | %-32s | %-16s | %-8s | %-8d | %-10.2f\n",
                   count, p->id, p->name, p->category, p->unit, p->quantity, p->price);
        }
    }

    if (count == 0) {
        printf("         [OK] Tat ca mat hang deu con ton kho trong kho hang.\n");
    }
    printf("=================================================================================================\n\n");
}

void print_category_summary_report(const ProductList *list) {
    if (list == NULL || list->count == 0) {
        printf("\n[i] Khong co du lieu san pham de thong ke.\n");
        return;
    }

    CategorySummary cats[MAX_CATEGORY_SUMMARY_COUNT];
    size_t cat_count = 0;
    if (calculate_category_summaries(list, cats, MAX_CATEGORY_SUMMARY_COUNT, &cat_count) != STATUS_SUCCESS) {
        printf("[!] Loi tinh toan thong ke theo danh muc.\n");
        return;
    }

    printf("\n=========================================================================================\n");
    printf("                         BAO CAO CO CAU TON KHO THEO NGANH HANG                         \n");
    printf("=========================================================================================\n");
    printf("%-4s | %-24s | %-14s | %-14s | %-16s\n",
           "STT", "DANH MUC NGANH HANG", "SO MAT HANG", "TONG TON KHO", "TONG GIA TRI ($)");
    printf("-----------------------------------------------------------------------------------------\n");

    long long grand_qty = 0;
    double grand_val = 0.0;
    for (size_t i = 0; i < cat_count; ++i) {
        grand_qty += cats[i].total_quantity;
        grand_val += cats[i].total_value;
        printf("%-4zu | %-24s | %-14zu | %-14lld | %-16.2f\n",
               i + 1, cats[i].category, cats[i].product_count, cats[i].total_quantity, cats[i].total_value);
    }
    printf("-----------------------------------------------------------------------------------------\n");
    printf("Tong cong: %zu danh muc | Tong ton kho: %lld | Tong gia tri: %.2f $\n",
           cat_count, grand_qty, grand_val);
    printf("=========================================================================================\n\n");
}

int export_inventory_report_to_file(const ProductList *list, const char *filepath, int low_stock_threshold) {
    if (list == NULL || filepath == NULL) {
        return STATUS_ERR_NULL_PTR;
    }

    FILE *f = fopen(filepath, "w");
    if (f == NULL) {
        return STATUS_ERR_FILE_IO;
    }

    InventorySummary summary;
    calculate_inventory_summary(list, low_stock_threshold, &summary);

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char time_str[64];
    if (t != NULL) {
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", t);
    } else {
        safe_string_copy(time_str, sizeof(time_str), "N/A");
    }

    fprintf(f, "========================================================================\n");
    fprintf(f, "               HE THONG QUAN LY KHO HANG - BAO CAO TONG HOP             \n");
    fprintf(f, "========================================================================\n");
    fprintf(f, "Thoi diem xuat bao cao: %s\n", time_str);
    fprintf(f, "Tong so mat hang (SKU)   : %zu\n", summary.total_products);
    fprintf(f, "Tong so luong ton kho    : %lld\n", summary.total_quantity);
    fprintf(f, "Tong gia tri ton kho     : %.2f $\n", summary.total_inventory_value);
    fprintf(f, "So mat hang het hang     : %zu\n", summary.out_of_stock_count);
    fprintf(f, "So mat hang sap het hang : %zu (nguong <= %d)\n\n", summary.low_stock_count, low_stock_threshold);

    fprintf(f, "--- DANH MUC CO CAU NGANH HANG ---\n");
    CategorySummary cats[MAX_CATEGORY_SUMMARY_COUNT];
    size_t cat_count = 0;
    if (calculate_category_summaries(list, cats, MAX_CATEGORY_SUMMARY_COUNT, &cat_count) == STATUS_SUCCESS) {
        for (size_t i = 0; i < cat_count; ++i) {
            fprintf(f, " + %-20s: %zu mat hang | %lld sp ton | %.2f $\n",
                    cats[i].category, cats[i].product_count, cats[i].total_quantity, cats[i].total_value);
        }
    }

    fprintf(f, "\n--- DANH SACH SAN PHAM CHI TIET ---\n");
    fprintf(f, "%-8s | %-32s | %-16s | %-8s | %-8s | %-10s\n",
            "MA SP", "TEN SAN PHAM", "LOAI SP", "DON VI", "TON KHO", "DON GIA ($)");
    fprintf(f, "-------------------------------------------------------------------------------------------------\n");
    for (size_t i = 0; i < list->count; ++i) {
        const Product *p = &list->items[i];
        fprintf(f, "%-8s | %-32s | %-16s | %-8s | %-8d | %-10.2f\n",
                p->id, p->name, p->category, p->unit, p->quantity, p->price);
    }
    fprintf(f, "========================================================================\n");

    fclose(f);
    return STATUS_SUCCESS;
}

int (report_statistics)(const ProductList *list, long long *total_quantity, double *total_value) {
    if (list == NULL) {
        if (total_quantity != NULL) *total_quantity = 0;
        if (total_value != NULL) *total_value = 0.0;
        return STATUS_ERR_NULL_PTR;
    }

    InventorySummary summary;
    int st = calculate_inventory_summary(list, DEFAULT_LOW_STOCK_THRESHOLD, &summary);
    if (st != STATUS_SUCCESS) {
        return st;
    }

    if (total_quantity != NULL) {
        *total_quantity = summary.total_quantity;
    }
    if (total_value != NULL) {
        *total_value = summary.total_inventory_value;
    }

    /* Tim mat hang co tong gia tri ton lon nhat (quantity * price) */
    double max_item_value = -1.0;
    const Product *highest_val_prod = NULL;
    for (size_t i = 0; i < list->count; ++i) {
        double item_val = (double)list->items[i].quantity * list->items[i].price;
        if (item_val > max_item_value) {
            max_item_value = item_val;
            highest_val_prod = &list->items[i];
        }
    }

    double avg_value_per_product = (summary.total_products > 0)
        ? (summary.total_inventory_value / (double)summary.total_products)
        : 0.0;

    printf("\n========================================================================\n");
    printf("                  THONG KE TON KHO (REPORT STATISTICS)                  \n");
    printf("========================================================================\n");
    printf(" 1. Tong so mat hang quan ly (SKU)       : %zu san pham\n", summary.total_products);
    printf(" 2. Tong so luong ton kho (Quantity)     : %lld san pham\n", summary.total_quantity);
    printf(" 3. Tong gia tri hang ton kho (Value)    : %.2f $\n", summary.total_inventory_value);
    printf(" 4. Gia tri ton trung binh / mat hang    : %.2f $\n", avg_value_per_product);
    printf(" 5. So mat hang da het hang (SL = 0)     : %zu san pham\n", summary.out_of_stock_count);
    printf(" 6. So mat hang sap het hang (<= %d)     : %zu san pham\n", DEFAULT_LOW_STOCK_THRESHOLD, summary.low_stock_count);

    if (highest_val_prod != NULL && max_item_value >= 0.0) {
        printf(" 7. Mat hang co gia tri ton kho lon nhat : %s (%s) - %.2f $\n",
               highest_val_prod->name, highest_val_prod->id, max_item_value);
    }
    if (summary.total_products > 0) {
        printf(" 8. San pham co don gia cao nhat         : %s (%s) - %.2f $\n",
               summary.most_expensive_product.name,
               summary.most_expensive_product.id,
               summary.most_expensive_product.price);
        printf(" 9. San pham co don gia thap nhat        : %s (%s) - %.2f $\n",
               summary.least_expensive_product.name,
               summary.least_expensive_product.id,
               summary.least_expensive_product.price);
    }
    printf("========================================================================\n\n");

    return STATUS_SUCCESS;
}

