(set-logic QF_BV)
(set-info :source "SMT-003B: Stock Addition with Guard check_addition_overflow (validation.c / inventory.c)")
(set-info :status unsat)

(declare-const current_stock (_ BitVec 32))
(declare-const add_quantity (_ BitVec 32))

; Tiền điều kiện
(assert (bvsge current_stock (_ bv0 32)))
(assert (bvsgt add_quantity (_ bv0 32)))

; Guard trong validation.c: check_addition_overflow
; Nếu current_stock > INT_MAX - add_quantity thì từ chối. Do đó nhánh thực hiện cộng thỏa:
(define-fun int_max () (_ BitVec 32) #x7FFFFFFF)
(assert (bvsle current_stock (bvsub int_max add_quantity)))

; Mở rộng 33-bit kiểm tra tràn
(define-fun cur_ext () (_ BitVec 33) ((_ sign_extend 1) current_stock))
(define-fun add_ext () (_ BitVec 33) ((_ sign_extend 1) add_quantity))
(define-fun sum_33 () (_ BitVec 33) (bvadd cur_ext add_ext))
(define-fun int_max_33 () (_ BitVec 33) ((_ sign_extend 1) #x7FFFFFFF))

; Có thể tràn không?
(assert (bvsgt sum_33 int_max_33))

(check-sat)
