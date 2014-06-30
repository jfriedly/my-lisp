#include <math.h>

#include "dbg.h"

#include "lval.h"
#include "lmath.h"

struct lval *lval_add(struct lval *x, struct lval *y)
{
	if (x->type == LVAL_LONG && y->type == LVAL_LONG)
		return lval_long(x->val.num_long + y->val.num_long);
	if (x->type == LVAL_LONG && y->type == LVAL_DOUBLE)
		return lval_double(x->val.num_long + y->val.num_double);
	if (x->type == LVAL_DOUBLE && y->type == LVAL_LONG)
		return lval_double(x->val.num_double + y->val.num_long);
	if (x->type == LVAL_DOUBLE && y->type == LVAL_DOUBLE)
		return lval_double(x->val.num_double + y->val.num_double);
	return lval_err("Invalid number types");
}

struct lval *lval_sub(struct lval *x, struct lval *y)
{
	if (x->type == LVAL_LONG && y->type == LVAL_LONG)
		return lval_long(x->val.num_long - y->val.num_long);
	if (x->type == LVAL_LONG && y->type == LVAL_DOUBLE)
		return lval_double(x->val.num_long - y->val.num_double);
	if (x->type == LVAL_DOUBLE && y->type == LVAL_LONG)
		return lval_double(x->val.num_double - y->val.num_long);
	if (x->type == LVAL_DOUBLE && y->type == LVAL_DOUBLE)
		return lval_double(x->val.num_double - y->val.num_double);
	return lval_err("Invalid number types");
}

struct lval *lval_mul(struct lval *x, struct lval *y)
{
	if (x->type == LVAL_LONG && y->type == LVAL_LONG)
		return lval_long(x->val.num_long * y->val.num_long);
	if (x->type == LVAL_LONG && y->type == LVAL_DOUBLE)
		return lval_double(x->val.num_long * y->val.num_double);
	if (x->type == LVAL_DOUBLE && y->type == LVAL_LONG)
		return lval_double(x->val.num_double * y->val.num_long);
	if (x->type == LVAL_DOUBLE && y->type == LVAL_DOUBLE)
		return lval_double(x->val.num_double * y->val.num_double);
	return lval_err("Invalid number types");
}

struct lval *lval_div(struct lval *x, struct lval *y)
{
	if (x->type == LVAL_LONG && y->type == LVAL_LONG)
		return y->val.num_long == 0 ? lval_err("Division by zero") : lval_long(x->val.num_long / y->val.num_long);
	if (x->type == LVAL_LONG && y->type == LVAL_DOUBLE)
		return y->val.num_double == 0.0 ? lval_err("Division by zero") : lval_double(x->val.num_long / y->val.num_double);
	if (x->type == LVAL_DOUBLE && y->type == LVAL_LONG)
		return y->val.num_long == 0 ? lval_err("Division by zero") : lval_double(x->val.num_double / y->val.num_long);
	if (x->type == LVAL_DOUBLE && y->type == LVAL_DOUBLE)
		return y->val.num_double == 0 ? lval_err("Division by zero") : lval_double(x->val.num_double / y->val.num_double);
	return lval_err("Invalid number types");
}

struct lval *lval_mod(struct lval *x, struct lval *y)
{
	if (x->type == LVAL_LONG && y->type == LVAL_LONG)
		return lval_long(x->val.num_long % y->val.num_long);
	if (x->type == LVAL_LONG && y->type == LVAL_DOUBLE)
		return lval_double(fmod(x->val.num_long, y->val.num_double));
	if (x->type == LVAL_DOUBLE && y->type == LVAL_LONG)
		return lval_double(fmod(x->val.num_double, y->val.num_long));
	if (x->type == LVAL_DOUBLE && y->type == LVAL_DOUBLE)
		return lval_double(fmod(x->val.num_double, y->val.num_double));
	return lval_err("Invalid number types");
}

struct lval *lval_pow(struct lval *x, struct lval *y)
{
	if (x->type == LVAL_LONG && y->type == LVAL_LONG)
		return lval_long(pow(x->val.num_long, y->val.num_long));
	if (x->type == LVAL_LONG && y->type == LVAL_DOUBLE)
		return lval_double(pow(x->val.num_long, y->val.num_double));
	if (x->type == LVAL_DOUBLE && y->type == LVAL_LONG)
		return lval_double(pow(x->val.num_double, y->val.num_long));
	if (x->type == LVAL_DOUBLE && y->type == LVAL_DOUBLE)
		return lval_double(pow(x->val.num_double, y->val.num_double));
	return lval_err("Invalid number types");
}

struct lval *lval_max(struct lval *x, struct lval *y)
{
	if (x->type == LVAL_LONG && y->type == LVAL_LONG)
		return lval_long(fmax(x->val.num_long, y->val.num_long));
	if (x->type == LVAL_LONG && y->type == LVAL_DOUBLE)
		return lval_double(fmax(x->val.num_long, y->val.num_double));
	if (x->type == LVAL_DOUBLE && y->type == LVAL_LONG)
		return lval_double(fmax(x->val.num_double, y->val.num_long));
	if (x->type == LVAL_DOUBLE && y->type == LVAL_DOUBLE)
		return lval_double(fmax(x->val.num_double, y->val.num_double));
	return lval_err("Invalid number types");
}

struct lval *lval_min(struct lval *x, struct lval *y)
{
	if (x->type == LVAL_LONG && y->type == LVAL_LONG)
		return lval_long(fmin(x->val.num_long, y->val.num_long));
	if (x->type == LVAL_LONG && y->type == LVAL_DOUBLE)
		return lval_double(fmin(x->val.num_long, y->val.num_double));
	if (x->type == LVAL_DOUBLE && y->type == LVAL_LONG)
		return lval_double(fmin(x->val.num_double, y->val.num_long));
	if (x->type == LVAL_DOUBLE && y->type == LVAL_DOUBLE)
		return lval_double(fmin(x->val.num_double, y->val.num_double));
	return lval_err("Invalid number types");
}
