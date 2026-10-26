(set-logic QF_BV)
(set-info :source "SMT-005A: File Byte Counter Overflow without Bound (file_io.c)")
(set-info :status sat)

(declare-const existing_bytes (_ BitVec 32))
(declare-const incoming_bytes (_ BitVec 32))

(assert (bvsge existing_bytes (_ bv0 32)))
(assert (bvsgt incoming_bytes (_ bv0 32)))

(define-fun cur_ext () (_ BitVec 33) ((_ sign_extend 1) existing_bytes))
(define-fun add_ext () (_ BitVec 33) ((_ sign_extend 1) incoming_bytes))
(define-fun sum_33 () (_ BitVec 33) (bvadd cur_ext add_ext))
(define-fun int_max_33 () (_ BitVec 33) ((_ sign_extend 1) #x7FFFFFFF))

(assert (bvsgt sum_33 int_max_33))
(check-sat)
(get-model)
