(set-logic QF_BV)
(set-info :source "SMT-003A: Stock Addition Overflow without Guard (inventory.c: inventory_add_stock)")
(set-info :status sat)

(declare-const current_stock (_ BitVec 32))
(declare-const add_quantity (_ BitVec 32))

; Tiền điều kiện nghiệp vụ: current_stock >= 0, add_quantity > 0
(assert (bvsge current_stock (_ bv0 32)))
(assert (bvsgt add_quantity (_ bv0 32)))

; Mở rộng sang 33-bit để kiểm tra tràn số nguyên có dấu (signed overflow)
(define-fun cur_ext () (_ BitVec 33) ((_ sign_extend 1) current_stock))
(define-fun add_ext () (_ BitVec 33) ((_ sign_extend 1) add_quantity))
(define-fun sum_33 () (_ BitVec 33) (bvadd cur_ext add_ext))

; INT_MAX = 2147483647 (#x7FFFFFFF)
(define-fun int_max_33 () (_ BitVec 33) ((_ sign_extend 1) #x7FFFFFFF))

; Tràn số có dấu: tổng toán học vượt quá INT_MAX
(assert (bvsgt sum_33 int_max_33))

(check-sat)
(get-model)
