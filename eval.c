#include <string.h>

#include "dbg.h"

#include "mpc.h"
#include "lval.h"
#include "eval.h"
#include "lmath.h"

#define LASSERT(sexpr, cond, err) if (!(cond)) { lval_del(sexpr); \
						 return lval_err(err); }


struct lval *builtin_op(char *op, struct lval *numbers)
{
	/* Ensure all arguments are numbers */
	for (int i = 0; i < numbers->count; i++) {
		if ((numbers->cell[i]->type != LVAL_LONG) && (numbers->cell[i]->type != LVAL_DOUBLE)) {
			lval_del(numbers);
			log_err("Attempted to evaluate operator %s on lval with type %d.",
				op, numbers->cell[i]->type);
			return lval_err("Operators can only be evaluated on numbers.");
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
			sentinel("Unrecognized operator:  '%s'", op);

		lval_del(y);

		if (acc->type == LVAL_ERR)
			break;
	}

	lval_del(numbers);
	return acc;

error:
	debug("Returning error from builtin_op");
	return lval_err("Unrecognized operator");
}

struct lval *builtin_car(struct lval *args)
{
	LASSERT(args, (args->count == 1), "Too many arguments passed to CAR");
	LASSERT(args, (args->cell[0]->type == LVAL_SEXPR), "Not a list");
	/* TODO(jfriedly):  Make this return NIL instead */
	LASSERT(args, (args->cell[0]->count != 0),
		"Too few arguments given to CAR");

	/* Otherwise take the first argument */
	struct lval *arg1 = lval_take(args, 0);

	while (arg1->count > 1)
		lval_del(lval_pop(arg1, 1));
	return lval_pop(arg1, 0);
}

struct lval *builtin_cdr(struct lval *args)
{
	LASSERT(args, (args->count == 1), "Too many arguments passed to CDR");
	LASSERT(args, (args->cell[0]->type == LVAL_SEXPR), "Not a list");
	/* TODO(jfriedly):  Make this return NIL instead */
	LASSERT(args, (args->cell[0]->count != 0),
		"Too few arguments given to CDR");

	/* Otherwise take the first argument */
	struct lval *arg1 = lval_take(args, 0);

	lval_del(lval_pop(arg1, 0));
	return arg1;
}

struct lval *builtin_list(struct lval *args)
{
	return args;
}

struct lval *builtin_eval(struct lval *args)
{
	LASSERT(args, (args->count == 1), "Too many arguments to eval");
	LASSERT(args, (args->cell[0]->type == LVAL_SEXPR), "Not a list");

	/* Otherwise take the first argument */
	struct lval *arg1 = lval_take(args, 0);

	return lval_eval(arg1);
}

struct lval *builtin_join(struct lval *args)
{
	for (int i = 0; i < args->count; i++) {
		if (args->cell[i]->type == LVAL_ERR)
			return args->cell[i];
		LASSERT(args, (args->cell[i]->type == LVAL_SEXPR),
			"Function JOIN passed incorrect type");
	}

	struct lval *acc = lval_pop(args, 0);
	while (args->count)
		acc = lval_join(acc, lval_pop(args, 0));

	lval_del(args);
	return acc;
}

struct lval *builtin(char *func, struct lval *args)
{
	debug("Entering builtin with function %s and args:", func);
	lval_debug(args);

	if (strcmp("car", func) == 0)
		return builtin_car(args);
	if (strcmp("cdr", func) == 0)
		return builtin_cdr(args);
	if (strcmp("list", func) == 0)
		return builtin_list(args);
	if (strcmp("eval", func) == 0)
		return builtin_eval(args);
	if (strcmp("join", func) == 0)
		return builtin_join(args);
	if (strcmp("max", func) == 0)
		return builtin_op("max", args);
	if (strcmp("min", func) == 0)
		return builtin_op("min", args);
	if (strstr("+-*/%^", func))
		return builtin_op(func, args);
	sentinel("Unrecognized function: %s", func);

error:
	lval_del(args);
	return lval_err("Unrecognized function");
}

/* Forward declare lval_eval */
struct lval *lval_eval(struct lval *expr);

struct lval *lval_eval_sexpr(struct lval *sexpr)
{
	/* nil */
	if (sexpr->count == 0)
		return sexpr;

	/* Don't evaluate quoted expressions */
	if ((sexpr->cell[0]->type == LVAL_SYM) && (strcmp(sexpr->cell[0]->val.sym, "quote") == 0)) {
		debug("Matched quote");
		/* Delete the quote symbol */
		lval_del(lval_pop(sexpr, 0));
		/* Pop out the args that were given to quote */
		struct lval *quoted_expr = lval_pop(sexpr, 0);
		lval_del(sexpr);
		return quoted_expr;
	}

	/* Recursively evaluate children */
	for (int i = 0; i < sexpr->count; i++)
		sexpr->cell[i] = lval_eval(sexpr->cell[i]);

	/* Error handling */
	for (int i = 0; i < sexpr->count; i++) {
		if (sexpr->cell[i]->type == LVAL_ERR)
			return lval_take(sexpr, i);
	}

	/* literal */
	if (sexpr->count == 1)
		return lval_take(sexpr, 0);

	/*
	 * Otherwise, we have an expression with multiple elements.
	 * Ensure the first element is a symbol.
	 */
	struct lval *car = lval_pop(sexpr, 0);
	if (car->type != LVAL_SYM) {
		log_err("Not a symbol:");
		lval_println(stderr, car);
		lval_del(car);
		lval_del(sexpr);
		return lval_err("S-expression must start with a symbol");
	}

	struct lval *result = builtin(car->val.sym, sexpr);
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

