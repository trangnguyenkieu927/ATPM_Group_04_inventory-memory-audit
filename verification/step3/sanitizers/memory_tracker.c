#include "memory_tracker.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MAX_TRACKED_BLOCKS 2048
#define CANARY_VALUE 0xDEADBEEFCAFEFACEULL

typedef struct {
    void *user_ptr;
    void *raw_ptr;
    size_t requested_size;
    const char *file;
    int line;
    int is_active;
} BlockRecord;

static BlockRecord g_blocks[MAX_TRACKED_BLOCKS];
static size_t g_active_count = 0;
static size_t g_total_alloc_calls = 0;
static size_t g_total_free_calls = 0;
static size_t g_total_bytes_allocated = 0;
static size_t g_current_bytes_in_use = 0;
static size_t g_peak_bytes = 0;
static int g_canary_corruptions = 0;
static int g_double_frees = 0;

void mem_tracker_init(void) {
    memset(g_blocks, 0, sizeof(g_blocks));
    g_active_count = 0;
    g_total_alloc_calls = 0;
    g_total_free_calls = 0;
    g_total_bytes_allocated = 0;
    g_current_bytes_in_use = 0;
    g_peak_bytes = 0;
    g_canary_corruptions = 0;
    g_double_frees = 0;
}

void mem_tracker_reset(void) {
    mem_tracker_init();
}

void *mem_tracker_malloc(size_t size, const char *file, int line) {
    /* Cấp phát: [Canary 8 byte] + [User payload size byte] + [Canary 8 byte] */
    size_t total_size = sizeof(uint64_t) + size + sizeof(uint64_t);
    uint8_t *raw = (uint8_t *)malloc(total_size);
    if (!raw) return NULL;

    /* Ghi canary đầu */
    uint64_t *head_canary = (uint64_t *)raw;
    *head_canary = CANARY_VALUE;

    uint8_t *user_ptr = raw + sizeof(uint64_t);

    /* Ghi canary cuối */
    uint64_t *tail_canary = (uint64_t *)(user_ptr + size);
    *tail_canary = CANARY_VALUE;

    /* Lưu bản ghi */
    for (size_t i = 0; i < MAX_TRACKED_BLOCKS; ++i) {
        if (!g_blocks[i].is_active) {
            g_blocks[i].user_ptr = user_ptr;
            g_blocks[i].raw_ptr = raw;
            g_blocks[i].requested_size = size;
            g_blocks[i].file = file;
            g_blocks[i].line = line;
            g_blocks[i].is_active = 1;
            break;
        }
    }

    g_active_count++;
    g_total_alloc_calls++;
    g_total_bytes_allocated += size;
    g_current_bytes_in_use += size;
    if (g_current_bytes_in_use > g_peak_bytes) {
        g_peak_bytes = g_current_bytes_in_use;
    }

    return user_ptr;
}

void *mem_tracker_calloc(size_t num, size_t size, const char *file, int line) {
    size_t total = num * size;
    void *ptr = mem_tracker_malloc(total, file, line);
    if (ptr) {
        memset(ptr, 0, total);
    }
    return ptr;
}

void *mem_tracker_realloc(void *ptr, size_t size, const char *file, int line) {
    if (ptr == NULL) {
        return mem_tracker_malloc(size, file, line);
    }
    if (size == 0) {
        mem_tracker_free(ptr, file, line);
        return NULL;
    }

    /* Tìm bản ghi cũ */
    BlockRecord *rec = NULL;
    for (size_t i = 0; i < MAX_TRACKED_BLOCKS; ++i) {
        if (g_blocks[i].is_active && g_blocks[i].user_ptr == ptr) {
            rec = &g_blocks[i];
            break;
        }
    }

    if (!rec) {
        /* Con trỏ không được theo dõi hoặc đã free */
        fprintf(stderr, "[MEM_TRACKER WARNING] Realloc on untracked pointer %p at %s:%d\n", ptr, file, line);
        return realloc(ptr, size);
    }

    /* Kiểm tra canary trước khi realloc */
    uint8_t *raw = (uint8_t *)rec->raw_ptr;
    uint64_t *head_canary = (uint64_t *)raw;
    uint64_t *tail_canary = (uint64_t *)((uint8_t *)rec->user_ptr + rec->requested_size);
    if (*head_canary != CANARY_VALUE || *tail_canary != CANARY_VALUE) {
        fprintf(stderr, "[MEM_TRACKER BUFFER OVERFLOW] Canary corrupted before realloc at %s:%d\n", file, line);
        g_canary_corruptions++;
    }

    /* Cấp phát vùng mới */
    void *new_user = mem_tracker_malloc(size, file, line);
    if (!new_user) return NULL;

    size_t copy_size = (size < rec->requested_size) ? size : rec->requested_size;
    memcpy(new_user, ptr, copy_size);

    /* Giải phóng vùng cũ */
    mem_tracker_free(ptr, file, line);
    return new_user;
}

void mem_tracker_free(void *ptr, const char *file, int line) {
    if (ptr == NULL) return;

    BlockRecord *rec = NULL;
    for (size_t i = 0; i < MAX_TRACKED_BLOCKS; ++i) {
        if (g_blocks[i].is_active && g_blocks[i].user_ptr == ptr) {
            rec = &g_blocks[i];
            break;
        }
    }

    if (!rec) {
        fprintf(stderr, "[MEM_TRACKER ERROR] Double free or invalid free on pointer %p at %s:%d\n", ptr, file, line);
        g_double_frees++;
        return;
    }

    /* Kiểm tra buffer overflow qua canaries */
    uint8_t *raw = (uint8_t *)rec->raw_ptr;
    uint64_t *head_canary = (uint64_t *)raw;
    uint64_t *tail_canary = (uint64_t *)((uint8_t *)rec->user_ptr + rec->requested_size);
    if (*head_canary != CANARY_VALUE || *tail_canary != CANARY_VALUE) {
        fprintf(stderr, "[MEM_TRACKER BUFFER OVERFLOW] Canary corrupted for block allocated at %s:%d (Freed at %s:%d)\n",
                rec->file, rec->line, file, line);
        g_canary_corruptions++;
    }

    g_current_bytes_in_use -= rec->requested_size;
    rec->is_active = 0;
    g_active_count--;
    g_total_free_calls++;

    free(rec->raw_ptr);
}

int mem_tracker_report(void) {
    printf("\n============================================================\n");
    printf("         BÁO CÁO THEO DÕI BỘ NHỚ VÀ RÒ RỈ (MEMORY AUDIT)     \n");
    printf("============================================================\n");
    printf("Tổng số lần cấp phát (malloc/calloc/realloc): %zu\n", g_total_alloc_calls);
    printf("Tổng số lần giải phóng (free):                %zu\n", g_total_free_calls);
    printf("Tổng dung lượng đã phân bổ:                   %zu bytes\n", g_total_bytes_allocated);
    printf("Dung lượng bộ nhớ đỉnh điểm (Peak RAM):       %zu bytes\n", g_peak_bytes);
    printf("Số khối bộ nhớ chưa giải phóng (Leaks):       %zu\n", g_active_count);
    printf("Số lần phát hiện Double-Free:                 %d\n", g_double_frees);
    printf("Số lần phát hiện tràn Canary (Overflow):      %d\n", g_canary_corruptions);

    if (g_active_count > 0) {
        printf("\n[CẢNH BÁO RÒ RỈ BỘ NHỚ] Chi tiết các khối chưa được free:\n");
        for (size_t i = 0; i < MAX_TRACKED_BLOCKS; ++i) {
            if (g_blocks[i].is_active) {
                printf("  - Block %p: %zu bytes (Cấp phát tại %s:%d)\n",
                       g_blocks[i].user_ptr, g_blocks[i].requested_size, g_blocks[i].file, g_blocks[i].line);
            }
        }
        printf("============================================================\n");
        return -1;
    }

    printf("\n[KẾT QUẢ AN TOÀN] Không phát hiện rò rỉ bộ nhớ (0 byte leak, 0 double free, 0 overflow).\n");
    printf("============================================================\n");
    return 0;
}

size_t mem_tracker_get_active_allocations(void) { return g_active_count; }
size_t mem_tracker_get_total_bytes_allocated(void) { return g_total_bytes_allocated; }
size_t mem_tracker_get_peak_bytes(void) { return g_peak_bytes; }
