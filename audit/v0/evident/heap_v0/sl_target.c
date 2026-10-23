/* =========================================================================
 * MODULE 2: DYNAMIC MEMORY & HEAP LIFECYCLE MANAGEMENT (v0 - CO LOI)
 * Thanh vien phu trach: Nguyen Kieu Trang
 * Muc tieu: Kiem toan giai doan Cap phat Heap, Mo rong dung luong, va Thu hoi RAM.
 * Cac loi chu dich:
 *   - calc_expand_capacity_v0 : Nhan he so gay tran so nguyen (CWE-190/787)
 *   - realloc_buffer_leak_v0  : Gan de truc tiep con tro lam mat dia chi (CWE-401)
 *   - cleanup_and_read_uaf_v0 : Doc lai vung nho da giai phong (CWE-416)
 * ========================================================================= */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* KICH BAN SC-03: CWE-190 -> CWE-787 (Integer Overflow dan den Heap Overflow)
 * Giai doan RAM: Danh sach san pham day, can mo rong suc chua buffer kho.
 * Root cause: Nhan cap voi factor ma khong kiem tra nguong tran INT_MAX. */
int calc_expand_capacity_v0(int current_cap, int factor) {
    return current_cap * factor; /* Neu current_cap lon, phep nhan bi tran ve so am/nho */
}

/* KICH BAN SC-04: CWE-401 (Memory Leak khi realloc that bai)
 * Giai doan RAM: Mo rong buffer kho bang realloc nhung he thong het RAM.
 * Root cause: Gan thang ket qua realloc vao con tro goc, neu NULL se mat dia chi cu. */
int realloc_buffer_leak_v0(int old_size, int new_size, int simulate_oom) {
    char *buf = (char *)malloc(old_size > 0 ? old_size : 16);
    if (!buf) return -1;
    memset(buf, 'A', (old_size > 0 ? old_size : 16) - 1);

    if (simulate_oom) {
        /* Mo phong realloc tra ve NULL (OOM) nhung ghi de lam mat dia chi buf */
        buf = NULL; 
        return -2; /* Mat con tro vung nho cu -> Leak bo nho */
    }

    char *new_buf = (char *)realloc(buf, new_size);
    if (!new_buf) {
        /* Lỗi v0: Neu realloc that bai tra ve NULL thi buf bi mat neu gan truc tiep */
        return -3;
    }
    free(new_buf);
    return 0;
}

/* KICH BAN SC-05: CWE-416 (Use-After-Free khi don dep buffer kho)
 * Giai doan RAM: Giai phong vung nho buffer san pham loi nhung van doc gia tri.
 * Root cause: Khong set NULL sau khi free, tiep tuc doc con tro troi noi (Dangling pointer). */
int cleanup_and_read_uaf_v0(int val) {
    int *ptr = (int *)malloc(sizeof(int));
    if (!ptr) return -1;
    *ptr = val;
    free(ptr);
    /* Lỗi UAF: Van tiep tuc doc du lieu tu vung nho da duoc tra ve cho he dieu hanh */
    return *ptr;
}

