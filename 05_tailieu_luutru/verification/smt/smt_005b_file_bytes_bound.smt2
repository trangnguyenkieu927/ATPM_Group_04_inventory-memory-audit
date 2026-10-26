(set-logic QF_BV)
(set-info :source "SMT-005B: File Byte Counter with Buffer Bound (file_io.c: LINE_BUFFER_SIZE = 512)")
(set-info :status unsat)

(declare-const existing_bytes (_ BitVec 32))
(declare-const incoming_bytes (_ BitVec 32))

(assert (bvsge existing_bytes (_ bv0 32)))
; incoming_bytes cho 1 dòng tối đa 512 byte
(assert (bvule incoming_bytes (_ bv512 32)))
; File dữ liệu kho thông thường giới hạn dưới 100MB = 104857600 byte
(assert (bvule existing_bytes (_ bv104857600 32)))

(define-fun cur_ext () (_ BitVec 33) ((_ sign_extend 1) existing_bytes))
(define-fun add_ext () (_ BitVec 33) ((_ sign_extend 1) incoming_bytes))
(define-fun sum_33 () (_ BitVec 33) (bvadd cur_ext add_ext))
(define-fun int_max_33 () (_ BitVec 33) ((_ sign_extend 1) #x7FFFFFFF))

(assert (bvsgt sum_33 int_max_33))
(check-sat)
