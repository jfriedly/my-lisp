#ifndef lval_h
#define lval_h

#include "mpc.h"

/* Forward declare the Lisp environment and lvals */
struct lenv;
struct lval;

/*
 * A Lisp function.
 *
 * If the ``builtin`` function pointer is NULL, then it is a user defined
 * function and it will have a body to be evaluated.  Builtin functions
 * do not have a body.
 *
 * The formals are formal arguments to the function, and the env is an
 * environment to bind the formal arguments in.
 */
/* TODO(jfriedly):  Put the function pointer and body into a union */
struct function {
        struct lval *(*builtin)(struct lenv *env, struct lval *v);
        struct lenv *env;
        struct lval *formals;
        struct lval *body;
};

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
                struct function func;
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
        LVAL_FUNC,
};

char *ltype(int type);

/* Lisp environment, a key-value store of strings : lvals */
struct lenv {
        struct lenv *parent;
        int count;
        char **syms;
        struct lval **vals;
};

/* lenv constructor */
struct lenv *lenv_new(void);

/* lval constructors */
struct lval *lval_long(long x);
struct lval *lval_double(double x);
struct lval *lval_err(char *fmt, ...);
struct lval *lval_sym(char *s);
struct lval *lval_sexpr(void);
struct lval *lval_func(struct lval *(*builtin)(struct lenv *env, struct lval *v));
struct lval *lval_lambda(struct lval* formals, struct lval* body);

/* lenv and lval lval destructors */
void lenv_del(struct lenv *env);
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

/* Create a deep (recursive) copy of an lval */
struct lval *lval_copy(struct lval *v);

/*
 * Functions for working with Lisp environments
 *
 * Each of these is O(n) on the size of the environmnet.
 */
/* Read a symbol from the environment to get a value. */
struct lval *lenv_get(struct lenv *env, struct lval *k);
/* Bind a symbol to a value in a local scope. */
void lenv_let(struct lenv *env, struct lval *k, struct lval *v);
/* Bind a symbol to a value in a global scope. */
void lenv_set(struct lenv *env, struct lval *k, struct lval *v);
/* Create a copy of an environment and return it. */
struct lenv *lenv_copy(struct lenv *original);

/* Add builtin functions to the environement */
void lenv_add_builtin(struct lenv *env,
                        char *name,
                        struct lval *(*builtin)(struct lenv *env, struct lval *v));

/* Functions for reading lvals from an AST */
struct lval *lval_read_num(mpc_ast_t *ast);
struct lval *lval_read(mpc_ast_t *ast);

/* Functions for printing lvals */
void lval_expr_print(FILE *stream, struct lval *v, char open, char close);
void lval_print(FILE *stream, struct lval *v);
void lval_println(FILE *stream, struct lval *v);

#endif
