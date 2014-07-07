#include <string.h>

#include "dbg.h"

#include "mpc.h"
#include "lval.h"
#include "eval.h"

struct lval *builtin_car(struct lenv *env, struct lval *args)
{
	/* TODO(jfriedly):  Make this return NIL on 0 args */
	LASSERT_ARGC(args, 1, "CAR");
	LASSERT_TYPE(args->cell[0], LVAL_SEXPR, "CAR");
	debug("CAR passed type");

	/* Otherwise take the first argument */
	struct lval *arg1 = lval_take(args, 0);

	debug("CAR entering while loop");
	while (arg1->count > 1) {
		debug("CAR in while loop");
		lval_del(lval_pop(arg1, 1));
	}
	debug("CAR exited while loop");
	return lval_pop(arg1, 0);
}

struct lval *builtin_cdr(struct lenv *env, struct lval *args)
{
	/* TODO(jfriedly):  Make this return NIL on 0 args */
	LASSERT_ARGC(args, 1, "CDR");
	LASSERT_TYPE(args->cell[0], LVAL_SEXPR, "CDR");

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
	LASSERT_ARGC(args, 1, "EVAL");
	LASSERT_TYPE(args->cell[0], LVAL_SEXPR, "EVAL");

	/* Otherwise take the first argument */
	struct lval *arg1 = lval_take(args, 0);

	return lval_eval(env, arg1);
}

struct lval *builtin_join(struct lenv *env, struct lval *args)
{
	for (int i = 0; i < args->count; i++) {
		if (args->cell[i]->type == LVAL_ERR)
			return args->cell[i];
	}

	struct lval *acc = lval_sexpr();
	debug("JOIN entering while loop");
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
	LASSERT_ARGC(args, 2, "SET");
	/* First argument is a symbol to define */
	LASSERT_TYPE(args->cell[0], LVAL_SYM, "SET");
	lenv_set(env, args->cell[0], args->cell[1]);
	lval_del(args);
	return lval_sexpr();
}

struct lval *builtin_env(struct lenv *env, struct lval *args)
{
	for (int i = 0; i < env->count; i++) {
		printf("%s:  ", env->syms[i]);
		lval_println(stdout, env->vals[i]);
	}
	return lval_sexpr();
}

struct lval *builtin_exit(struct lenv *env, struct lval *args)
{
	printf("Bye.\n");
	exit(0);
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

	/* Ensure the first element is a function.  */
	struct lval *f = lval_pop(sexpr, 0);
	if (f->type != LVAL_FUNC) {
		log_err("Not a function:");
		lval_println(stderr, f);
		lval_del(f);
		lval_del(sexpr);
		return lval_err("S-expression must start with a function");
	}

	debug("Calling func");
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
