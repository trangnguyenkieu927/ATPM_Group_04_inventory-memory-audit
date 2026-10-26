(set-logic QF_BV)
(set-info :source "SMT-002A: Allocation Size Overflow without Guard (product.c: realloc size)")
(set-info :status sat)

; Khai báo new_cap 64-bit
(declare-const new_cap (_ BitVec 64))

; sizeof(Product) trên Windows LLP64 ABI = 208 byte (0xD0)
(define-fun sizeof_product () (_ BitVec 64) (_ bv208 64))

(assert (bvugt new_cap (_ bv0 64)))

; Mở rộng sang 128-bit để kiểm tra tràn số học của phép nhân 64-bit
(define-fun cap_128 () (_ BitVec 128) ((_ zero_extend 64) new_cap))
(define-fun size_128 () (_ BitVec 128) ((_ zero_extend 64) sizeof_product))
(define-fun alloc_bytes_128 () (_ BitVec 128) (bvmul cap_128 size_128))

(define-fun max_size_t_128 () (_ BitVec 128) ((_ zero_extend 64) #xFFFFFFFFFFFFFFFF))

; Điều kiện tràn: alloc_bytes vượt quá SIZE_MAX
(assert (bvugt alloc_bytes_128 max_size_t_128))

(check-sat)
(get-model)
