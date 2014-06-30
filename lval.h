#ifndef lval_h
#define lval_h

#include "mpc.h"

/* Lisp value (or error) */
/* TODO(jfriedly):  S-expressions should use the union val. */
/* TODO(jfriedly):  Use cons cells instead of an array of lvals. */
struct lval {
	short type;
    union {
        long num_long;
        double num_double;
        char *err;
        char *sym;
    } val ;
    int count;
    struct lval **cell;
};

/* lval types */
enum {
	LVAL_LONG,
	LVAL_DOUBLE,
	LVAL_ERR,
    LVAL_SYM,
    LVAL_SEXPR,
};

/* lval constructors */
struct lval *lval_long(long x);
struct lval *lval_double(double x);
struct lval *lval_err(char *m);
struct lval *lval_sym(char *s);
struct lval *lval_sexpr(void);

/* lval destructor */
void lval_del(struct lval *v);

/* Use lval_append to put multiple lvals into a single S-expression */
struct lval *lval_append(struct lval *head, struct lval *tail);

/* Use lval_join to join two S-expressions together */
struct lval *lval_join(struct lval *head, struct lval *tail);

/*
 * Use lval_pop to "pop" an lval out of an S-expression.  The lval is
 * removed from the S-expression and returned, and all lvals after it in the
 * S-expression shift left.
 *
 * Be sure to delete both the lval returned by lval_pop and the original
 * S-expression.
 */
struct lval *lval_pop(struct lval *sexpr, int i);
/*
 * Convenience wrapper around lval_pop that auto-deletes the original S
 * expression.
 */
struct lval *lval_take(struct lval *sexpr, int i);

/* Functions for reading lvals from an AST */
struct lval *lval_read_num(mpc_ast_t *ast);
struct lval *lval_read(mpc_ast_t *ast);

/* Functions for printing lvals */
void lval_expr_print(FILE *stream, struct lval *v, char open, char close);
void lval_print(FILE *stream, struct lval *v);
void lval_println(FILE *stream, struct lval *v);
void lval_debug(struct lval *v);

#endif
