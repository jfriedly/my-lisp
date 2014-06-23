#include <stdio.h>
#include <stdlib.h>

#include "dbg.h"

#include "mpc.h"
#include "lval.h"

struct lval *lval_long(long x)
{
	struct lval *v = malloc(sizeof(struct lval));
	v->type = LVAL_LONG;
	union atom val;
	val.num_long = x;
	v->val = val;
	return v;
}

struct lval *lval_double(double x)
{
	struct lval *v = malloc(sizeof(struct lval));
	v->type = LVAL_DOUBLE;
	union atom val;
	val.num_double = x;
	v->val = val;
	return v;
}

struct lval *lval_err(char *m)
{
	struct lval *v = malloc(sizeof(struct lval));
	v->type = LVAL_ERR;
	union atom val;
	val.err = malloc(strlen(m) + 1);
	strcpy(val.err, m);
	v->val = val;
	return v;
}

struct lval *lval_sym(char *s)
{
	struct lval *v = malloc(sizeof(struct lval));
	v->type = LVAL_SYM;
	union atom val;
	val.sym = malloc(strlen(s) + 1);
	strcpy(val.sym, s);
	v->val = val;
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

struct lval *lval_append(struct lval *head, struct lval *tail)
{
	head->count++;
	head->cell = realloc(head->cell, sizeof(struct lval*) * head->count);
	head->cell[head->count - 1] = tail;
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
			return lval_err("Number out of range");
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

void lval_del(struct lval *v)
{
	switch (v->type) {
	/* We don't have to do anything special for numbers */
	case LVAL_LONG:
		break;
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
	default:
		log_err("Attempted to delete an unrecognized lval type: %d",
			v->type);
	}

	free(v);
}

/* Forward declare lval_print */
void lval_print(struct lval *v);

void lval_expr_print(struct lval *v, char open, char close)
{
	debug("Calling lval_expr_print");
	putchar(open);
	for (int i = 0; i < v->count; i++) {
		lval_print(v->cell[i]);
		/* Don't print trailing space if last element */
		if (i != (v->count-1))
			putchar(' ');
	}
	putchar(close);
}

void lval_print(struct lval *v)
{
	switch (v->type) {
	case LVAL_LONG:
		printf("%ld", v->val.num_long);
		break;
	case LVAL_DOUBLE:
		printf("%f", v->val.num_double);
		break;
	case LVAL_ERR:
		printf("Runtime Error: %s", v->val.err);
		break;
	case LVAL_SYM:
		printf("%s", v->val.sym);
		break;
	case LVAL_SEXPR:
		lval_expr_print(v, '(', ')');
		break;
	default:
		log_err("Attempted to print an unrecognized lval type %d",
			v->type);
	}
}

void lval_println(struct lval *v)
{
	debug("Calling lval_println");
	lval_print(v);
	putchar('\n');
}
