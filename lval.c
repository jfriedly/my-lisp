#include <stdio.h>
#include <stdlib.h>

#include "dbg.h"

#include "mpc.h"
#include "lval.h"

char *ltype(int type)
{
	switch (type) {
	case LVAL_LONG:
		return "Integer";
	case LVAL_DOUBLE:
		return "Float";
	case LVAL_ERR:
		return "Error";
	case LVAL_SYM:
		return "Symbol";
	case LVAL_SEXPR:
		return "S-Expression";
	case LVAL_FUNC:
		return "Function";
	default: {
		char *err = malloc(32);
		sprintf(err, "Unknown (%d)", type);
		return realloc(err, strlen(err) + 1); }
	}
}

struct lenv *lenv_new(void)
{
	struct lenv *env = malloc(sizeof(struct lenv));
	env->count = 0;
	env->syms = NULL;
	env->vals = NULL;
	return env;
}

struct lval *lval_long(long x)
{
	struct lval *v = malloc(sizeof(struct lval));
	v->type = LVAL_LONG;
	v->val.num_long = x;
	return v;
}

struct lval *lval_double(double x)
{
	struct lval *v = malloc(sizeof(struct lval));
	v->type = LVAL_DOUBLE;
	v->val.num_double = x;
	return v;
}

struct lval *lval_err(char *fmt, ...)
{
	struct lval *v = malloc(sizeof(struct lval));
	v->type = LVAL_ERR;

	/* Create a va list and initialize it */
	va_list va;
	va_start(va, fmt);

	/* Allocate 512 bytes for the error message (hopefully enough) */
	v->val.err = malloc(512);

	/* printf into the error string up to 511 characters */
	vsnprintf(v->val.err, 511, fmt, va);

	/* Reallocate to the number of bytes actually used */
	v->val.err = realloc(v->val.err, strlen(v->val.err) + 1);
	va_end(va);
	return v;
}

struct lval *lval_sym(char *s)
{
	struct lval *v = malloc(sizeof(struct lval));
	v->type = LVAL_SYM;
	v->val.sym = malloc(strlen(s) + 1);
	strcpy(v->val.sym, s);
	return v;
}

struct lval *lval_sexpr(void)
{
	struct lval *v = malloc(sizeof(struct lval));
	v->type = LVAL_SEXPR;
	v->count = 0;
	v->cell = NULL;
	return v;
}

struct lval *lval_func(struct lval *(*func)(struct lenv *env, struct lval *v))
{
	struct lval *v = malloc(sizeof(struct lval));
	v->type = LVAL_FUNC;
	v->val.func = func;
	return v;
}

struct lval *lval_append(struct lval *head, struct lval *tail)
{
	head->count++;
	head->cell = realloc(head->cell, sizeof(struct lval*) * head->count);
	head->cell[head->count - 1] = tail;
	return head;
}

struct lval *lval_join(struct lval *head, struct lval *tail)
{
	if (tail->type == LVAL_SEXPR) {
		/* For each cell in the tail, append it onto the end of the head */
		while (tail->count)
			head = lval_append(head, lval_pop(tail, 0));
		lval_del(tail);
	} else {
		lval_append(head, tail);
	}
	return head;
}

struct lval *lval_pop(struct lval *sexpr, int i)
{
	/* Find the child at index i */
	struct lval *ret = sexpr->cell[i];

	/*
	 * Shift the memory following the lval at i over the top of it.
	 *
	 * Note:  I originally thought this would overwrite the return value,
	 * but it doesn't because it's an array of pointers, not an array of
	 * lval structs.  This means that the copy we made above is an actual
	 * copy of the pointer we need, and we can safely shift left here.
	 */
	memmove(&sexpr->cell[i], &sexpr->cell[i+1],
		sizeof(struct lval*) * (sexpr->count - i - 1));

	sexpr->count--;
	sexpr->cell = realloc(sexpr->cell, sizeof(struct lval*) * sexpr->count);
	return ret;
}

struct lval *lval_take(struct lval *sexpr, int i)
{
	struct lval *ret = lval_pop(sexpr, i);
	lval_del(sexpr);
	return ret;
}

struct lval *lval_copy(struct lval *v)
{
	struct lval *x = malloc(sizeof(struct lval));
	x->type = v->type;

	switch (v->type) {
	/* Copy numbers directly */
	case LVAL_LONG:
		x->val.num_long = v->val.num_long;
		break;
	case LVAL_DOUBLE:
		x->val.num_double = v->val.num_double;
		break;

	/* Copy strings using malloc and strcpy */
	case LVAL_ERR:
		x->val.err = malloc(strlen(v->val.err) + 1);
		strcpy(x->val.err, v->val.err);
		break;
	case LVAL_SYM:
		x->val.sym = malloc(strlen(v->val.sym) + 1);
		strcpy(x->val.sym, v->val.sym);
		break;

	/* Copy lists by recursively copying sub expressions */
	case LVAL_SEXPR:
		x->count = v->count;
		x->cell = malloc(sizeof(struct lval *) * v->count);
		for (int i = 0; i < x->count; i++) {
			x->cell[i] = lval_copy(v->cell[i]);
		}
		break;

	/* Copy functions directly */
	case LVAL_FUNC:
		x->val.func = v->val.func;
		break;
	default:
		lval_del(x);
		/* There's a bug if this ever doesn't print Unknown. */
		return lval_err("Attempted to copy unknown type %s.",
			ltype(v->type));
	}

	return x;
}

struct lval *lenv_get(struct lenv *env, struct lval *k)
{
	/* Iterate over every element in the environnment to find the key */
	for (int i = 0; i < env->count; i++) {
		if (strcmp(env->syms[i], k->val.sym) == 0)
			return lval_copy(env->vals[i]);
	}

	/* If the key isn't found, return an error */
	return lval_err("Unbound symbol:  '%s'", k->val.sym);
}

void lenv_set(struct lenv *env, struct lval *k, struct lval *v)
{
	/*
	 * Iterate over every element in the environment in case the key is
	 * already bound and just needs to be replaced.
	 */
	for (int i = 0; i < env->count; i++) {
		if (strcmp(env->syms[i], k->val.sym) == 0) {
			lval_del(env->vals[i]);
			env->vals[i] = lval_copy(v);
			return;
		}
	}

	/* If no existing key was found, allocate space for a new pair */
	env->count++;
	env->vals = realloc(env->vals, sizeof(struct lval*) * env->count);
	env->syms = realloc(env->syms, sizeof(char*) * env->count);

	/* Copy the key and value into the environment */
	env->vals[env->count-1] = lval_copy(v);
	env->syms[env->count-1] = malloc(strlen(k->val.sym) + 1);
	strcpy(env->syms[env->count-1], k->val.sym);
}

void lenv_add_builtin(struct lenv *env,
			char *name,
			struct lval *(*func)(struct lenv *env, struct lval *v))
{
	struct lval *k = lval_sym(name);
	struct lval *v = lval_func(func);
	lenv_set(env, k, v);
	lval_del(k);
	lval_del(v);
}

struct lval *lval_read_num(mpc_ast_t *ast)
{
	debug("Parsing a number: %s", ast->contents);
	errno = 0;
	if (strstr(ast->contents, ".")) {
		double x = strtof(ast->contents, NULL);
		debug("Float parsed as %f", x);
		return lval_double(x);
	} else {
		long x = strtol(ast->contents, NULL, 10);
		if (errno != ERANGE)
			return lval_long(x);
		else
			return lval_err("Number out of range: %s",
				ast->contents);
	}
}

struct lval *lval_read(mpc_ast_t *ast)
{
	debug("Entering lval_read, with tag='%s' and contents='%s'",
		ast->tag, ast->contents);
	if (strstr(ast->tag, "number"))
		return lval_read_num(ast);
	else if (strstr(ast->tag, "symbol"))
		return lval_sym(ast->contents);

	/* If root, ignore the first and last child (^ and $ regexes) */
	struct lval *sexpr = NULL;
	if (strcmp(ast->tag, ">") == 0)
		return lval_read(ast->children[1]);

	if (strstr(ast->tag, "sexpr"))
		sexpr = lval_sexpr();
	else
		sentinel("AST tag did not match any of number, symbol, root, sexpr");

	for (int i = 0; i < ast->children_num; i++) {
		if (strcmp(ast->children[i]->contents, "(") == 0)
			continue;
		if (strcmp(ast->children[i]->contents, ")") == 0)
			continue;
		if (strcmp(ast->children[i]->contents, "{") == 0)
			continue;
		if (strcmp(ast->children[i]->contents, "}") == 0)
			continue;
		if (strcmp(ast->children[i]->tag, "regex") == 0)
			continue;
		sexpr = lval_append(sexpr, lval_read(ast->children[i]));
	}
	return sexpr;

error:
	return NULL;
}

void lenv_del(struct lenv *env)
{
	for (int i = 0; i < env->count; i++) {
		free(env->syms[i]);
		lval_del(env->vals[i]);
	}
	free(env->syms);
	free(env->vals);
	free(env);
}

void lval_del(struct lval *v)
{
	switch (v->type) {
	/* We don't have to do anything special for numbers */
	case LVAL_LONG:
	case LVAL_DOUBLE:
		break;

	/* Free the strings for errors and symbols */
	case LVAL_ERR:
		free(v->val.err);
		break;
	case LVAL_SYM:
		free(v->val.sym);
		break;

	/* Recursively free lvals in S-expressions */
	case LVAL_SEXPR:
		for (int i = 0; i < v->count; i++)
			lval_del(v->cell[i]);
		/* Also free the memory allocated to contain the pointers */
		free(v->cell);
		break;

	/* We don't have to do anything special for functions */
	case LVAL_FUNC:
		break;

	default:
		/* There's a bug if this ever doesn't print Unknown. */
		log_err("Attempted to delete an unrecognized lval type: %s.",
			ltype(v->type));
	}

	free(v);
}

/* Forward declare lval_print */
void lval_print(FILE *stream, struct lval *v);

void lval_expr_print(FILE *stream, struct lval *v, char open, char close)
{
	putc(open, stream);
	for (int i = 0; i < v->count; i++) {
		lval_print(stream, v->cell[i]);
		/* Don't print trailing space if last element */
		if (i != (v->count-1))
			putc(' ', stream);
	}
	putc(close, stream);
}

void lval_print(FILE *stream, struct lval *v)
{
	switch (v->type) {
	case LVAL_LONG:
		fprintf(stream, "%ld", v->val.num_long);
		break;
	case LVAL_DOUBLE:
		fprintf(stream, "%f", v->val.num_double);
		break;
	case LVAL_ERR:
		fprintf(stream, "Runtime Error: %s", v->val.err);
		break;
	case LVAL_SYM:
		fprintf(stream, "%s", v->val.sym);
		break;
	case LVAL_SEXPR:
		lval_expr_print(stream, v, '(', ')');
		break;
	case LVAL_FUNC:
		/*
		 * TODO(jfriedly):  Figure out a way to make this print the
		 * name of the function.
		 */
		fprintf(stream, "<function at %p>", v->val.func);
		break;
	default:
		/* There's a bug if this ever doesn't print Unknown. */
		log_err("Attempted to print an unrecognized lval type %s.",
			ltype(v->type));
	}
}

void lval_println(FILE *stream, struct lval *v)
{
	lval_print(stream, v);
	putc('\n', stream);
}
