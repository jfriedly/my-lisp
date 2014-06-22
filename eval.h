#ifndef eval_h
#define eval_h

/* Lisp value union */
union lval_value {
    long num_long;
    double num_double;
    short err_short;
};

/* Lisp value (or error) */
struct lval {
	short type;
    union lval_value val;
};

/* lval types */
enum {
	LVAL_LONG,
	LVAL_DOUBLE,
	LVAL_ERR,
};

/* error types */
enum {
	LERR_DIV_ZERO,
	LERR_BAD_OP,
	LERR_BAD_NUM,
};


/* lval constructors */
struct lval lval_long(long x);
struct lval lval_double(double x);
struct lval lval_err(short x);

void lval_print(struct lval v);

/* operations on lvals */
struct lval lval_add(struct lval x, struct lval y);
struct lval lval_sub(struct lval x, struct lval y);
struct lval lval_mul(struct lval x, struct lval y);
struct lval lval_div(struct lval x, struct lval y);
struct lval lval_mod(struct lval x, struct lval y);
struct lval lval_pow(struct lval x, struct lval y);
struct lval lval_max(struct lval x, struct lval y);
struct lval lval_min(struct lval x, struct lval y);

/* Strip leading and trailing regex nodes from the AST */
struct lval eval_line(mpc_ast_t *ast);

/* evaluate an expression */
struct lval eval(mpc_ast_t *ast);

/* evaluate a parsed operator on two parsed numbers */
struct lval eval_op(char *op, struct lval x, struct lval y);

#endif
