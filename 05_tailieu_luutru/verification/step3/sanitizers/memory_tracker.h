#ifndef MEMORY_TRACKER_H
#define MEMORY_TRACKER_H

#include <stddef.h>

/**
 * Memory Tracker & Leak Detection Runtime Harness
 * Cung cấp wrapper kiểm tra rò rỉ bộ nhớ động và bảo vệ boundary canary.
 */

void mem_tracker_init(void);
void *mem_tracker_malloc(size_t size, const char *file, int line);
void *mem_tracker_realloc(void *ptr, size_t size, const char *file, int line);
void *mem_tracker_calloc(size_t num, size_t size, const char *file, int line);
void mem_tracker_free(void *ptr, const char *file, int line);
int mem_tracker_report(void);
void mem_tracker_reset(void);

size_t mem_tracker_get_active_allocations(void);
size_t mem_tracker_get_total_bytes_allocated(void);
size_t mem_tracker_get_peak_bytes(void);

#endif /* MEMORY_TRACKER_H */
