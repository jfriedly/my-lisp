#ifndef eval_h
#define eval_h

#include "lval.h"

#define LASSERT(expr, cond, fmt, ...) \
	if (!(cond)) { \
		struct lval *err = lval_err(fmt, ##__VA_ARGS__); \
		lval_del(expr); \
		return err; \
	}

#define LASSERT_ARGC(expr, expected, func_name) \
	LASSERT(expr, (expr->count == expected), \
		"Function %s passed incorrect number of arguments.  " \
		"Got %d.  Expected %d.", func_name, expr->count, expected);

#define LASSERT_TYPE(expr, expected, func_name) \
	LASSERT(expr, (expr->type == expected), \
		"Function %s passed incorrect type.  Got %s.  " \
		"Expected %s.", func_name, ltype(expr->type), ltype(expected));

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
struct lval *builtin_car(struct lenv *env, struct lval *args);
/* Returns all elements of an S-expression except the first (tail) */
struct lval *builtin_cdr(struct lenv *env, struct lval *args);
/* Creates a list containing the elements that are passed as arguments */
struct lval *builtin_list(struct lenv *env, struct lval *args);
/* Evaluates an S-expression */
struct lval *builtin_eval(struct lenv *env, struct lval *args);
/*
 * Use join to join many S-expressions together.  Ex:
 *
 * (join (quote (1)) (quote (2 3)) (quote ((4))))
 *  -> returns <sexpr: (1 2 3 (4))>
 */
struct lval *builtin_join(struct lenv *env, struct lval *args);
/* Returns the length of an S-expression */
struct lval *builtin_length(struct lenv *env, struct lval *args);
/* Sets a variable */
struct lval *builtin_set(struct lenv *env, struct lval *args);

/* Evaluate an S-expression */
struct lval *lval_eval_sexpr(struct lenv *env, struct lval *sexpr);
/*
 * Evaluate arbitrary lvals by looking symbols up in the environment,
 * calling lval_eval_sexpr on S-expressions, and otherwise just
 * returning them as is.
 */
struct lval *lval_eval(struct lenv *env, struct lval *v);

/****************************************************************************
 * Functions below here are defined in lmath.c
 ***************************************************************************/

/* Math operations on lvals */
struct lval *lval_add(struct lval *x, struct lval *y);
struct lval *lval_sub(struct lval *x, struct lval *y);
struct lval *lval_mul(struct lval *x, struct lval *y);
struct lval *lval_div(struct lval *x, struct lval *y);
struct lval *lval_mod(struct lval *x, struct lval *y);
struct lval *lval_pow(struct lval *x, struct lval *y);
struct lval *lval_max(struct lval *x, struct lval *y);
struct lval *lval_min(struct lval *x, struct lval *y);

/* evaluate a parsed operator on two parsed numbers */
struct lval *builtin_op(struct lenv *env, char *op, struct lval *numbers);


/*
 * Wrapper functions around lval math that just call builtin_op appropriately.
 * None of them actually use the environment variable; this just gives the
 * math functions the right signatures for use as function pointers.
 */
struct lval *builtin_add(struct lenv *env, struct lval *numbers);
struct lval *builtin_sub(struct lenv *env, struct lval *numbers);
struct lval *builtin_mul(struct lenv *env, struct lval *numbers);
struct lval *builtin_div(struct lenv *env, struct lval *numbers);
struct lval *builtin_mod(struct lenv *env, struct lval *numbers);
struct lval *builtin_pow(struct lenv *env, struct lval *numbers);
struct lval *builtin_max(struct lenv *env, struct lval *numbers);
struct lval *builtin_min(struct lenv *env, struct lval *numbers);

#endif
