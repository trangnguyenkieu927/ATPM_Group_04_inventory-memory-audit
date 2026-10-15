(set-logic QF_BV)
(set-info :source "SMT-001B: Capacity Growth Reachability-Constrained (product.c)")
(set-info :status unsat)

; Khai báo biến capacity 64-bit
(declare-const capacity (_ BitVec 64))

; Giả định capacity được khởi tạo là 4 và không vượt quá giới hạn bộ nhớ vật lý thực tế
; Ví dụ: RAM 64GB = 68719476736 byte -> tối đa ~330,382,099 sản phẩm
(define-fun max_physical_cap () (_ BitVec 64) (_ bv330382099 64))

(assert (bvugt capacity (_ bv0 64)))
(assert (bvule capacity max_physical_cap))

; Mở rộng sang 65-bit để kiểm tra tràn
(define-fun cap_ext () (_ BitVec 65) ((_ zero_extend 1) capacity))
(define-fun two_ext () (_ BitVec 65) (_ bv2 65))
(define-fun mult_65 () (_ BitVec 65) (bvmul cap_ext two_ext))
(define-fun max_64_ext () (_ BitVec 65) ((_ zero_extend 1) #xFFFFFFFFFFFFFFFF))

; Ràng buộc: Liệu có thể xảy ra tràn trong phạm vi khả đạt vật lý không?
(assert (bvugt mult_65 max_64_ext))

(check-sat)
