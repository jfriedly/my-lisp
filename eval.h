#ifndef eval_h
#define eval_h

#include "lval.h"

/* operations on lvals */
struct lval *lval_add(struct lval *x, struct lval *y);
struct lval *lval_sub(struct lval *x, struct lval *y);
struct lval *lval_mul(struct lval *x, struct lval *y);
struct lval *lval_div(struct lval *x, struct lval *y);
struct lval *lval_mod(struct lval *x, struct lval *y);
struct lval *lval_pow(struct lval *x, struct lval *y);
struct lval *lval_max(struct lval *x, struct lval *y);
struct lval *lval_min(struct lval *x, struct lval *y);

/* evaluate a parsed operator on two parsed numbers */
struct lval *eval_op(char *op, struct lval *numbers);

/* Evaluate an S-expression */
struct lval *lval_eval_sexpr(struct lval *sexpr);
/*
 * Wrapper around lval_eval_sexpr that returns the lval if it isn't an
 * S-expression.
 */
struct lval *lval_eval(struct lval *expr);

#endif
