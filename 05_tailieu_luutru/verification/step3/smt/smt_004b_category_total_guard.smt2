(set-logic QF_BV)
(set-info :source "SMT-004B: Category Total Quantity Overflow with Guard (report.c)")
(set-info :status unsat)

(declare-const cat_total (_ BitVec 32))
(declare-const next_qty (_ BitVec 32))

(assert (bvsge cat_total (_ bv0 32)))
(assert (bvsgt next_qty (_ bv0 32)))

(define-fun int_max () (_ BitVec 32) #x7FFFFFFF)
(assert (bvsle cat_total (bvsub int_max next_qty)))

(define-fun cur_ext () (_ BitVec 33) ((_ sign_extend 1) cat_total))
(define-fun add_ext () (_ BitVec 33) ((_ sign_extend 1) next_qty))
(define-fun sum_33 () (_ BitVec 33) (bvadd cur_ext add_ext))
(define-fun int_max_33 () (_ BitVec 33) ((_ sign_extend 1) #x7FFFFFFF))

(assert (bvsgt sum_33 int_max_33))
(check-sat)
