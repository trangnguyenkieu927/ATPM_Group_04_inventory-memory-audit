(set-logic QF_BV)
(set-info :source "SMT-004A: Category Total Quantity Overflow without Guard (report.c)")
(set-info :status sat)

(declare-const cat_total (_ BitVec 32))
(declare-const next_qty (_ BitVec 32))

(assert (bvsge cat_total (_ bv0 32)))
(assert (bvsgt next_qty (_ bv0 32)))

(define-fun cur_ext () (_ BitVec 33) ((_ sign_extend 1) cat_total))
(define-fun add_ext () (_ BitVec 33) ((_ sign_extend 1) next_qty))
(define-fun sum_33 () (_ BitVec 33) (bvadd cur_ext add_ext))
(define-fun int_max_33 () (_ BitVec 33) ((_ sign_extend 1) #x7FFFFFFF))

(assert (bvsgt sum_33 int_max_33))
(check-sat)
(get-model)
