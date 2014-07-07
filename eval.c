#include <string.h>

#include "dbg.h"

#include "mpc.h"
#include "lval.h"
#include "eval.h"

struct lval *builtin_car(struct lenv *env, struct lval *args)
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

struct lval *builtin_cdr(struct lenv *env, struct lval *args)
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

struct lval *builtin_list(struct lenv *env, struct lval *args)
{
	return args;
}

struct lval *builtin_eval(struct lenv *env, struct lval *args)
{
	LASSERT(args, (args->count == 1), "Too many arguments to eval");
	LASSERT(args, (args->cell[0]->type == LVAL_SEXPR), "Not a list");

	/* Otherwise take the first argument */
	struct lval *arg1 = lval_take(args, 0);

	return lval_eval(env, arg1);
}

struct lval *builtin_join(struct lenv *env, struct lval *args)
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

struct lval *builtin_length(struct lenv *env, struct lval *args)
{
	/* Otherwise take the first argument */
	struct lval *arg1 = lval_take(args, 0);
	return lval_long(arg1->count);
}

struct lval *builtin_set(struct lenv *env, struct lval *args)
{
	LASSERT(args, (args->count == 2),
		"Function SET must be passed exactly two arguments");
	/* First argument is a symbol to define */
	LASSERT(args, (args->cell[0]->type == LVAL_SYM),
		"Function SET must be passed a symbol to bind!");
	lenv_set(env, args->cell[0], args->cell[1]);
	lval_del(args);
	return lval_sexpr();
}

/* Forward declare lval_eval */
struct lval *lval_eval(struct lenv *env, struct lval *v);

struct lval *lval_eval_sexpr(struct lenv *env, struct lval *sexpr)
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
		sexpr->cell[i] = lval_eval(env, sexpr->cell[i]);

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
	 * Ensure the first element is a function.
	 */
	struct lval *f = lval_pop(sexpr, 0);
	if (f->type != LVAL_FUNC) {
		log_err("Not a function:");
		lval_println(stderr, f);
		lval_del(f);
		lval_del(sexpr);
		return lval_err("S-expression must start with a function");
	}

	struct lval *result = f->val.func(env, sexpr);
	lval_del(f);
	return result;
}

struct lval *lval_eval(struct lenv *env, struct lval *v)
{
	/* Look up symbols in the environment */
	if (v->type == LVAL_SYM) {
		struct lval *x = lenv_get(env, v);
		lval_del(v);
		return x;
	}
	/* Evaluate S-expressions */
	if (v->type == LVAL_SEXPR)
		return lval_eval_sexpr(env, v);
	/* All other lval types remain the same */
	return v;
}
