(set-logic QF_BV)
(set-info :source "SMT-001A: Capacity Growth 64-bit Arithmetic Overflow (product.c: product_list_add)")
(set-info :status sat)

; Khai báo biến capacity 64-bit size_t
(declare-const capacity (_ BitVec 64))

; Tiền điều kiện: capacity > 0
(assert (bvugt capacity (_ bv0 64)))

; Mở rộng sang 65-bit để kiểm tra tràn số chính xác theo số học toán học
; Phép nhân capacity * 2 trong bit-vector 65-bit
(define-fun cap_ext () (_ BitVec 65) ((_ zero_extend 1) capacity))
(define-fun two_ext () (_ BitVec 65) (_ bv2 65))
(define-fun mult_65 () (_ BitVec 65) (bvmul cap_ext two_ext))

; Điều kiện tràn 64-bit: kết quả 65-bit vượt quá UINT64_MAX (bit thứ 64 bằng 1)
(define-fun max_64_ext () (_ BitVec 65) ((_ zero_extend 1) #xFFFFFFFFFFFFFFFF))
(assert (bvugt mult_65 max_64_ext))

(check-sat)
(get-model)
