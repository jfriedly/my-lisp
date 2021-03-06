#include <math.h>
#include <stdbool.h>

#include "dbg.h"

#include "lval.h"
#include "eval.h"

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
	return lval_err("Invalid number types: %s and %s.", ltype(x->type),
		ltype(y->type));
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
	return lval_err("Invalid number types: %s and %s.", ltype(x->type),
		ltype(y->type));
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
	return lval_err("Invalid number types: %s and %s.", ltype(x->type),
		ltype(y->type));
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
	return lval_err("Invalid number types: %s and %s.", ltype(x->type),
		ltype(y->type));
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
	return lval_err("Invalid number types: %s and %s.", ltype(x->type),
		ltype(y->type));
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
	return lval_err("Invalid number types: %s and %s.", ltype(x->type),
		ltype(y->type));
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
	return lval_err("Invalid number types: %s and %s.", ltype(x->type),
		ltype(y->type));
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
	return lval_err("Invalid number types: %s and %s.", ltype(x->type),
		ltype(y->type));
}

struct lval *lval_eq(struct lval *x, struct lval *y)
{
	if (x->type == LVAL_LONG && y->type == LVAL_LONG)
		return lval_bool(x->val.num_long == y->val.num_long);
	if (x->type == LVAL_LONG && y->type == LVAL_DOUBLE)
		return lval_bool(x->val.num_long == y->val.num_double);
	if (x->type == LVAL_DOUBLE && y->type == LVAL_LONG)
		return lval_bool(x->val.num_double == y->val.num_long);
	if (x->type == LVAL_DOUBLE && y->type == LVAL_DOUBLE)
		return lval_bool(x->val.num_double == y->val.num_double);
	return lval_err("Invalid number types: %s and %s.", ltype(x->type),
		ltype(y->type));
}

struct lval *lval_geq(struct lval *x, struct lval *y)
{
	if (x->type == LVAL_LONG && y->type == LVAL_LONG)
		return lval_bool(x->val.num_long >= y->val.num_long);
	if (x->type == LVAL_LONG && y->type == LVAL_DOUBLE)
		return lval_bool(x->val.num_long >= y->val.num_double);
	if (x->type == LVAL_DOUBLE && y->type == LVAL_LONG)
		return lval_bool(x->val.num_double >= y->val.num_long);
	if (x->type == LVAL_DOUBLE && y->type == LVAL_DOUBLE)
		return lval_bool(x->val.num_double >= y->val.num_double);
	return lval_err("Invalid number types: %s and %s.", ltype(x->type),
		ltype(y->type));
}

struct lval *lval_leq(struct lval *x, struct lval *y)
{
	if (x->type == LVAL_LONG && y->type == LVAL_LONG)
		return lval_bool(x->val.num_long <= y->val.num_long);
	if (x->type == LVAL_LONG && y->type == LVAL_DOUBLE)
		return lval_bool(x->val.num_long <= y->val.num_double);
	if (x->type == LVAL_DOUBLE && y->type == LVAL_LONG)
		return lval_bool(x->val.num_double <= y->val.num_long);
	if (x->type == LVAL_DOUBLE && y->type == LVAL_DOUBLE)
		return lval_bool(x->val.num_double <= y->val.num_double);
	return lval_err("Invalid number types: %s and %s.", ltype(x->type),
		ltype(y->type));
}

/* greater than */
struct lval *lval_g(struct lval *x, struct lval *y)
{
	if (x->type == LVAL_LONG && y->type == LVAL_LONG)
		return lval_bool(x->val.num_long > y->val.num_long);
	if (x->type == LVAL_LONG && y->type == LVAL_DOUBLE)
		return lval_bool(x->val.num_long > y->val.num_double);
	if (x->type == LVAL_DOUBLE && y->type == LVAL_LONG)
		return lval_bool(x->val.num_double > y->val.num_long);
	if (x->type == LVAL_DOUBLE && y->type == LVAL_DOUBLE)
		return lval_bool(x->val.num_double > y->val.num_double);
	return lval_err("Invalid number types: %s and %s.", ltype(x->type),
		ltype(y->type));
}

/* less than */
struct lval *lval_l(struct lval *x, struct lval *y)
{
	if (x->type == LVAL_LONG && y->type == LVAL_LONG)
		return lval_bool(x->val.num_long < y->val.num_long);
	if (x->type == LVAL_LONG && y->type == LVAL_DOUBLE)
		return lval_bool(x->val.num_long < y->val.num_double);
	if (x->type == LVAL_DOUBLE && y->type == LVAL_LONG)
		return lval_bool(x->val.num_double < y->val.num_long);
	if (x->type == LVAL_DOUBLE && y->type == LVAL_DOUBLE)
		return lval_bool(x->val.num_double < y->val.num_double);
	return lval_err("Invalid number types: %s and %s.", ltype(x->type),
		ltype(y->type));
}

/* Ensures all arguments are numbers */
struct lval *_ensure_numbers(char * op, struct lval *numbers)
{
	for (int i = 0; i < numbers->count; i++) {
		if ((numbers->cell[i]->type != LVAL_LONG) && (numbers->cell[i]->type != LVAL_DOUBLE)) {
			return lval_err("Attempted to evaluate operator %s "
					"on type %s", op,
					ltype(numbers->cell[i]->type));
		}
	}
	return NULL;
}


struct lval *builtin_op(struct lenv *env, char *op, struct lval *numbers)
{
	LASSERT(numbers, (numbers->count > 0), "Function %s requires at least "
		"one argument", op);
	struct lval *type_err = _ensure_numbers(op, numbers);
	if (type_err) {
		lval_del(numbers);
		return type_err;
	}

	/*
	 * Pop the first number; it becomes our accumulator.  There will
	 * be at least one number because lval_eval_sexpr guarantees that
	 * for us.
	 *
	 * If there is only one number, we'll return that number unchanged.
	 * That is, the default argument is always an identity operation.
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
			return lval_err("Unrecognized operator: '%s'", op);

		lval_del(y);

		if (acc->type == LVAL_ERR)
			break;
	}

	lval_del(numbers);
	return acc;
}

struct lval *builtin_comp(struct lenv *env, char *op, struct lval *numbers)
{
	debug("Called builtin_comp");
	LASSERT_ARGC(numbers, 2, op);
	debug("Numbers contains 2 arguments");
	struct lval *type_err = _ensure_numbers(op, numbers);
	if (type_err)
		return type_err;

	debug ("Getting x");
	struct lval *x = lval_pop(numbers, 0);
	debug("Got x");
	struct lval *y = lval_pop(numbers, 0);
	debug("Got x and y");
	if (strcmp(op, "=") == 0)
		return lval_eq(x, y);
	else if (strcmp(op, ">=") == 0)
		return lval_geq(x, y);
	else if (strcmp(op, "<=") == 0)
		return lval_leq(x, y);
	else if (strcmp(op, ">") == 0)
		return lval_g(x, y);
	else if (strcmp(op, "<") == 0)
		return lval_l(x, y);
	else
		goto cleanup;

cleanup:
	lval_del(x);
	lval_del(y);
	lval_del(numbers);
	return lval_err("Unrecognized operator: '%s'", op);
}

struct lval *builtin_add(struct lenv *env, struct lval *numbers)
{
	return builtin_op(env, "+", numbers);
}

struct lval *builtin_sub(struct lenv *env, struct lval *numbers)
{
	return builtin_op(env, "-", numbers);
}

struct lval *builtin_mul(struct lenv *env, struct lval *numbers)
{
	return builtin_op(env, "*", numbers);
}

struct lval *builtin_div(struct lenv *env, struct lval *numbers)
{
	return builtin_op(env, "/", numbers);
}

struct lval *builtin_mod(struct lenv *env, struct lval *numbers)
{
	return builtin_op(env, "%", numbers);
}

struct lval *builtin_pow(struct lenv *env, struct lval *numbers)
{
	return builtin_op(env, "^", numbers);
}

struct lval *builtin_max(struct lenv *env, struct lval *numbers)
{
	return builtin_op(env, "max", numbers);
}

struct lval *builtin_min(struct lenv *env, struct lval *numbers)
{
	return builtin_op(env, "min", numbers);
}

struct lval *builtin_eq(struct lenv *env, struct lval *numbers)
{
	return builtin_comp(env, "=", numbers);
}

struct lval *builtin_geq(struct lenv *env, struct lval *numbers)
{
	return builtin_comp(env, ">=", numbers);
}

struct lval *builtin_leq(struct lenv *env, struct lval *numbers)
{
	return builtin_comp(env, "<=", numbers);
}

struct lval *builtin_g(struct lenv *env, struct lval *numbers)
{
	return builtin_comp(env, ">", numbers);
}

struct lval *builtin_l(struct lenv *env, struct lval *numbers)
{
	return builtin_comp(env, "<", numbers);
}
