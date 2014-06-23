#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "dbg.h"

#include "mpc.h"
#include "eval.h"
#include "lval.h"

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

struct lval *eval_op(char *op, struct lval *numbers)
{
	/* Ensure all arguments are numbers */
	for (int i = 0; i < numbers->count; i++) {
		if ((numbers->cell[i]->type != LVAL_LONG) && (numbers->cell[i]->type != LVAL_DOUBLE)) {
			lval_del(numbers);
			log_err("Attempted to evaluate operator %s on lval with type %d",
				op, numbers->cell[i]->type);
			return lval_err("Cannot evaluate operator on non-numbers");
		}
	}

	/*
	 * Pop the first number; it becomes our accumulator.  There will
	 * be at least one number because lval_eval_sexpr guarantees that
	 * for us.
	 *
	 * If there is only one number, we'll return that number unchanged.
	 */
	struct lval *acc = lval_pop(numbers, 0);

	/* While there are still elements remaining */
	while (numbers->count > 0) {
		struct lval *y = lval_pop(numbers, 0);
		if (strcmp(op, "+") == 0)
			acc = lval_add(acc, y);
		else if (strcmp(op, "-") == 0)
			acc = lval_sub(acc, y);
		else if (strcmp(op, "*") == 0)
			acc = lval_mul(acc, y);
		else if (strcmp(op, "/") == 0)
			acc = lval_div(acc, y);
		else if (strcmp(op, "%") == 0)
			acc = lval_mod(acc, y);
		else if (strcmp(op, "^") == 0)
			acc = lval_pow(acc, y);
		else if (strcmp(op, "max") == 0)
			acc = lval_max(acc, y);
		else if (strcmp(op, "min") == 0)
			acc = lval_min(acc, y);
		else
			sentinel("Unrecognized operator:  '%c'", *op);

		lval_del(y);

		if (acc->type == LVAL_ERR)
			break;
	}

	lval_del(numbers);
	return acc;

error:
	debug("Returning error from eval_op");
	return lval_err("Unrecognized operator");
}

/* Forward declare lval_eval */
struct lval *lval_eval(struct lval *expr);

struct lval *lval_eval_sexpr(struct lval *sexpr)
{
	/* Recursively evaluate children */
	for (int i = 0; i < sexpr->count; i++)
		sexpr->cell[i] = lval_eval(sexpr->cell[i]);

	/* Error handling */
	for (int i = 0; i < sexpr->count; i++) {
		if (sexpr->cell[i]->type == LVAL_ERR)
			return lval_take(sexpr, i);
	}

	/* nil */
	if (sexpr->count == 0)
		return sexpr;

	/* literal */
	if (sexpr->count == 1)
		return lval_take(sexpr, 0);

	/*
	 * Otherwise, we have an expression with multiple elements.
	 * Ensure the first element is a symbol.
	 */
	struct lval *car = lval_pop(sexpr, 0);
	if (car->type != LVAL_SYM) {
		lval_del(car);
		lval_del(sexpr);
		return lval_err("S-expression does not start with a symbol");
	}

	struct lval *result = eval_op(car->val.sym, sexpr);
	lval_del(car);
	return result;
}

struct lval *lval_eval(struct lval *expr)
{
	/* Evaluate S-expressions */
	if (expr->type == LVAL_SEXPR)
		return lval_eval_sexpr(expr);
	/* All other lval types remain the same */
	return expr;
}

