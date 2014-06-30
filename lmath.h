#ifndef math_h
#define math_h

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

#endif
