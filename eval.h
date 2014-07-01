#ifndef eval_h
#define eval_h

#include "lval.h"

/* evaluate a parsed operator on two parsed numbers */
struct lval *buildin_op(char *op, struct lval *numbers);

/*
 * C functions that implement Lisp primitives
 *
 * The arguments here get a bit tricky.  eval will call builtin, which
 * calls builtin_*.  The S-expression passed to builtin_* is the entire
 * outer expression, but with the symbol lval popped out.  Examples:
 *
 * (car (quote (+ 1 2)))
 *  -> builtin("car", <sexpr: ((+ 1 2))>)
 *   -> builtin_car(<sexpr: ((+ 1 2))>)
 *    -> returns <sym: +>
 *
 * (list (1 2) 3)
 *  -> builtin("list", <sexpr: ((1 2) 3)>)
 *   -> builtin_list(<sexpr: ((1 2) 3)>)
 *    -> returns <sexpr: ((1 2) 3)>
 *
 * This allows passing multiple arguments to builtin functions, but means
 * that functions of one argument, such as car, must reach through an extra
 * layer of S-expressions in order to evaluate correctly.
 */
/* Returns the first element of an S-expression (head) */
struct lval *builtin_car(struct lval *args);
/* Returns all elements of an S-expression except the first (tail) */
struct lval *builtin_cdr(struct lval *args);
/* Creates a list containing the elements that are passed as arguments */
struct lval *builtin_list(struct lval *args);
/* Evaluates an S-expression */
struct lval *builtin_eval(struct lval *args);
/*
 * Use join to join many S-expressions together.  Ex:
 *
 * (join (quote (1)) (quote (2 3)) (quote ((4))))
 *  -> returns <sexpr: (1 2 3 (4))>
 */
struct lval *builtin_join(struct lval *args);
/* Returns the length of an S-expression */
struct lval *builtin_length(struct lval *args);

/* Evaluate builtin symbols */
struct lval *builtin(char *func, struct lval *sexpr);

/* Evaluate an S-expression */
struct lval *lval_eval_sexpr(struct lval *sexpr);
/*
 * Wrapper around lval_eval_sexpr that returns the lval if it isn't an
 * S-expression.
 */
struct lval *lval_eval(struct lval *expr);

#endif
