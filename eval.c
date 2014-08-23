#include <stdbool.h>
#include <string.h>

#include "dbg.h"

#include "mpc.h"
#include "lval.h"
#include "eval.h"

struct lval *builtin_car(struct lenv *env, struct lval *args)
{
	/* TODO(jfriedly):  Make this return NIL on 0 args */
	LASSERT_ARGC(args, 1, "car");
	LASSERT_TYPE(args->cell[0], LVAL_SEXPR, "car");
	debug("car passed type");

	/* Otherwise take the first argument */
	struct lval *arg1 = lval_take(args, 0);

	debug("car entering while loop");
	while (arg1->count > 1) {
		debug("car in while loop");
		lval_del(lval_pop(arg1, 1));
	}
	debug("car exited while loop");
	return lval_pop(arg1, 0);
}

struct lval *builtin_cdr(struct lenv *env, struct lval *args)
{
	/* TODO(jfriedly):  Make this return NIL on 0 args */
	LASSERT_ARGC(args, 1, "cdr");
	LASSERT_TYPE(args->cell[0], LVAL_SEXPR, "cdr");

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
	LASSERT_ARGC(args, 1, "eval");
	LASSERT_TYPE(args->cell[0], LVAL_SEXPR, "eval");

	/* Otherwise take the first argument */
	struct lval *arg1 = lval_take(args, 0);

	return lval_eval(env, arg1);
}

struct lval *builtin_lambda(struct lenv *env, struct lval *args)
{
	LASSERT_ARGC(args, 2, "lambda");
	LASSERT_TYPE(args->cell[0], LVAL_SEXPR, "lambda");
	LASSERT_TYPE(args->cell[1], LVAL_SEXPR, "lambda");

	/* Check that the first list contains only symbols */
	for (int i = 0; i < args->cell[0]->count; i++) {
		LASSERT(args, (args->cell[0]->cell[i]->type == LVAL_SYM),
			"Cannot define %s.  Expected %s.",
			ltype(args->cell[0]->cell[i]->type), ltype(LVAL_SYM));
	}

	/* Pop the formals and body and pass them to the lambda constructor */
	struct lval *formals = lval_pop(args, 0);
	struct lval *body = lval_pop(args, 0);
	lval_del(args);
	return lval_lambda(formals, body);
}

/* Utility function to turn other types of lvals into booleans */
bool _convert_to_bool(struct lval *v)
{
	switch (v->type) {
	case LVAL_LONG:
		if (!v->val.num_long)
			return false;
		break;
	case LVAL_DOUBLE:
		if (!v->val.num_double)
			return false;
		break;
	case LVAL_ERR:
		break;
	case LVAL_SYM:
		if (!v->val.sym)
			return false;
		break;
	case LVAL_SEXPR:
		if (v->count == 0)
			return false;
		break;
	case LVAL_FUNC:
		break;
	case LVAL_BOOL:
		if (!v->val.b)
			return false;
		break;
	}

	debug("_convert_to_bool returning true");
	return true;
}

struct lval *builtin_not(struct lenv *env, struct lval *args)
{
	LASSERT_ARGC(args, 1, "not");
	struct lval *arg1 = lval_take(args, 0);
	struct lval *bool1 = lval_bool(!_convert_to_bool(arg1));
	lval_del(arg1);
	return bool1;
}

struct lval *builtin_or(struct lenv *env, struct lval *args)
{
	if (args->count == 0)
		return lval_bool(false);

	/* Iterate over all but the last argument so that we can return it */
	while (args->count - 1) {
		struct lval *argi = lval_pop(args, 0);
		if (_convert_to_bool(argi)) {
			lval_del(args);
			return argi;
		}
		lval_del(argi);
	}

	struct lval *last_arg = lval_take(args, 0);
	return last_arg;
}

struct lval *builtin_and(struct lenv *env, struct lval *args)
{
	if (args->count == 0)
		return lval_bool(true);

	/* Iterate over all but the last argument so that we can return it */
	while (args->count - 1) {
		struct lval *argi = lval_pop(args, 0);
		if (!_convert_to_bool(argi)) {
			lval_del(args);
			return argi;
		}
		lval_del(argi);
	}

	struct lval *last_arg = lval_take(args, 0);
	return last_arg;
}

struct lval *builtin_if(struct lenv *env, struct lval *args)
{
	LASSERT_ARGC(args, 3, "if");
	struct lval *cond = lval_pop(args, 0);
	struct lval *then_branch = lval_pop(args, 0);
	struct lval *else_branch = lval_pop(args, 0);
	lval_del(args);

	if (_convert_to_bool(cond))
		return lval_eval(env, then_branch);
	else
		return lval_eval(env, else_branch);
}

struct lval *builtin_join(struct lenv *env, struct lval *args)
{
	for (int i = 0; i < args->count; i++) {
		if (args->cell[i]->type == LVAL_ERR)
			return args->cell[i];
	}

	struct lval *acc = lval_sexpr();
	debug("join entering while loop");
	while (args->count)
		acc = lval_join(acc, lval_pop(args, 0));

	lval_del(args);
	return acc;
}

struct lval *builtin_length(struct lenv *env, struct lval *args)
{
	/* Otherwise take the first argument */
	struct lval *arg1 = lval_take(args, 0);
	debug("builtin_length returning an lval_long of %d", arg1->count);
	return lval_long(arg1->count);
}

/* Common code used by both builtin_let and builtin_set */
struct lval *_builtin_var(struct lenv *env, struct lval *args, char *funcname)
{
	LASSERT_ARGC(args, 2, funcname);
	/* First argument is a symbol to define */
	LASSERT_TYPE(args->cell[0], LVAL_SYM, funcname);
	if (strcmp(funcname, "let") == 0)
		lenv_let(env, args->cell[0], args->cell[1]);
	if (strcmp(funcname, "set") == 0)
		lenv_set(env, args->cell[0], args->cell[1]);
	lval_del(args);
	return lval_sexpr();
}

struct lval *builtin_let(struct lenv *env, struct lval *args)
{
	return _builtin_var(env, args, "let");
}

struct lval *builtin_set(struct lenv *env, struct lval *args)
{
	return _builtin_var(env, args, "set");
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

struct lval *lval_call(struct lenv *env, struct lval *f, struct lval *args)
{
	/* If it's a builtin function, evaluate it directly */
	if (f->val.func.builtin)
		return f->val.func.builtin(env, args);

	int argc = args->count;
	int total = f->val.func.formals->count;

	/* Bind as many formal arguments as possible */
	while (args->count) {
		if (f->val.func.formals->count == 0) {
			lval_del(args);
			return lval_err("Function passed too many arguments.  "
				"Got %d.  Expected %d", argc, total);
		}
		/* Pop the first symbol from the formals and handle varargs */
		struct lval *sym = lval_pop(f->val.func.formals, 0);
		if (strcmp(sym->val.sym, "&") == 0) {
			if (f->val.func.formals->count != 1) {
				lval_del(sym);
				lval_del(args);
				/*
				 * TODO(jfriedly):  This should be raised on
				 * function creation, not when the function is
				 * called.
				 */
				return lval_err("Function format invalid.  "
					"Symbol '&' must be followed by "
					"exactly one symbol.");
			}
			/* Next formal should be bound to remaining args */
			struct lval *varargs = lval_pop(f->val.func.formals,
				0);
			lenv_let(f->val.func.env, varargs,
				builtin_list(env, args));
			lval_del(sym);
			lval_del(varargs);
			break;
		}
		/* Pop the next argument from the list */
		struct lval *val = lval_pop(args, 0);
		lenv_let(f->val.func.env, sym, val);
		lval_del(sym);
		lval_del(val);
	}

	lval_del(args);

	/* If '&' remains in formal list, it should be bound to nil */
	if (f->val.func.formals->count > 0 &&
		strcmp(f->val.func.formals->cell[0]->val.sym, "&") == 0) {
		if (f->val.func.formals->count != 2) {
			/*
			 * TODO(jfriedly):  This should be raised on function
			 * creation, not when the function is called.
			 */
			return lval_err("Function format invalid.  Symbol '&'"
				" must be followed by exactly one symbol.");
		}
		/* Pop and delete the '&' symbol */
		lval_del(lval_pop(f->val.func.formals, 0));
		/* Pop the next symbol and create an empty list for it */
		struct lval *varargs = lval_pop(f->val.func.formals, 0);
		struct lval *nil = lval_sexpr();
		/* Bind to the environment and clean up */
		lenv_let(f->val.func.env, varargs, nil);
		lval_del(varargs);
		lval_del(nil);
	}

	/* Evaluate and return if all formals have been bound */
	if (f->val.func.formals->count == 0) {
		f->val.func.env->parent = env;
		return builtin_eval(f->val.func.env, lval_append(lval_sexpr(),
			lval_copy(f->val.func.body)));
	} else {
		/* Otherwise return a partial function (curried function) */
		return lval_copy(f);
	}
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
		if (sexpr->count != 1)
			return lval_err("Special form quote must be passed "
					"exactly one argument");
		/* Pop out the arg that was given to quote */
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

	struct lval *result = lval_call(env, f, sexpr);
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
