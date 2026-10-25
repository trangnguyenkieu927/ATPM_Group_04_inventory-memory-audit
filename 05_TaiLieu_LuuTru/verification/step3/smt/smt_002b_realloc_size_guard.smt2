(set-logic QF_BV)
(set-info :source "SMT-002B: Allocation Size with Defensive Guard (product.c)")
(set-info :status unsat)

(declare-const new_cap (_ BitVec 64))
(define-fun sizeof_product () (_ BitVec 64) (_ bv208 64))

(assert (bvugt new_cap (_ bv0 64)))

; Guard phòng vệ: new_cap <= SIZE_MAX / sizeof(Product)
; SIZE_MAX / 208 = 18446744073709551615 / 208 = 88686269585142075 (#x013B13B13B13B13B)
(define-fun max_safe_cap () (_ BitVec 64) #x013B13B13B13B13B)
(assert (bvule new_cap max_safe_cap))

; Mở rộng 128-bit kiểm tra tràn
(define-fun cap_128 () (_ BitVec 128) ((_ zero_extend 64) new_cap))
(define-fun size_128 () (_ BitVec 128) ((_ zero_extend 64) sizeof_product))
(define-fun alloc_bytes_128 () (_ BitVec 128) (bvmul cap_128 size_128))
(define-fun max_size_t_128 () (_ BitVec 128) ((_ zero_extend 64) #xFFFFFFFFFFFFFFFF))

; Có thể tràn không?
(assert (bvugt alloc_bytes_128 max_size_t_128))

(check-sat)
