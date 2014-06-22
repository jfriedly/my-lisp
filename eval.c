#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "dbg.h"

#include "mpc.h"
#include "eval.h"

struct lval lval_long(long x)
{
	struct lval v;
	v.type = LVAL_LONG;
	union lval_value val;
	val.num_long = x;
	v.val = val;
	return v;
}

struct lval lval_double(double x)
{
	struct lval v;
	v.type = LVAL_DOUBLE;
	union lval_value val;
	val.num_double = x;
	v.val = val;
	return v;
}

struct lval lval_err(short x)
{
	struct lval v;
	v.type = LVAL_ERR;
	union lval_value val;
	val.err_short = x;
	v.val = val;
	return v;
}

struct lval lval_add(struct lval x, struct lval y)
{
	if (x.type == LVAL_LONG && y.type == LVAL_LONG)
		return lval_long(x.val.num_long + y.val.num_long);
	if (x.type == LVAL_LONG && y.type == LVAL_DOUBLE)
		return lval_double(x.val.num_long + y.val.num_double);
	if (x.type == LVAL_DOUBLE && y.type == LVAL_LONG)
		return lval_double(x.val.num_double + y.val.num_long);
	if (x.type == LVAL_DOUBLE && y.type == LVAL_DOUBLE)
		return lval_double(x.val.num_double + y.val.num_double);
	return lval_err(LERR_BAD_NUM);
}

struct lval lval_sub(struct lval x, struct lval y)
{
	if (x.type == LVAL_LONG && y.type == LVAL_LONG)
		return lval_long(x.val.num_long - y.val.num_long);
	if (x.type == LVAL_LONG && y.type == LVAL_DOUBLE)
		return lval_double(x.val.num_long - y.val.num_double);
	if (x.type == LVAL_DOUBLE && y.type == LVAL_LONG)
		return lval_double(x.val.num_double - y.val.num_long);
	if (x.type == LVAL_DOUBLE && y.type == LVAL_DOUBLE)
		return lval_double(x.val.num_double - y.val.num_double);
	return lval_err(LERR_BAD_NUM);
}

struct lval lval_mul(struct lval x, struct lval y)
{
	if (x.type == LVAL_LONG && y.type == LVAL_LONG)
		return lval_long(x.val.num_long * y.val.num_long);
	if (x.type == LVAL_LONG && y.type == LVAL_DOUBLE)
		return lval_double(x.val.num_long * y.val.num_double);
	if (x.type == LVAL_DOUBLE && y.type == LVAL_LONG)
		return lval_double(x.val.num_double * y.val.num_long);
	if (x.type == LVAL_DOUBLE && y.type == LVAL_DOUBLE)
		return lval_double(x.val.num_double * y.val.num_double);
	return lval_err(LERR_BAD_NUM);
}

struct lval lval_div(struct lval x, struct lval y)
{
	if (x.type == LVAL_LONG && y.type == LVAL_LONG)
		return y.val.num_long == 0 ? lval_err(LERR_DIV_ZERO) : lval_long(x.val.num_long / y.val.num_long);
	if (x.type == LVAL_LONG && y.type == LVAL_DOUBLE)
		return y.val.num_double == 0.0 ? lval_err(LERR_DIV_ZERO) : lval_double(x.val.num_long / y.val.num_double);
	if (x.type == LVAL_DOUBLE && y.type == LVAL_LONG)
		return y.val.num_long == 0 ? lval_err(LERR_DIV_ZERO) : lval_double(x.val.num_double / y.val.num_long);
	if (x.type == LVAL_DOUBLE && y.type == LVAL_DOUBLE)
		return y.val.num_double == 0 ? lval_err(LERR_DIV_ZERO) : lval_double(x.val.num_double / y.val.num_double);
	return lval_err(LERR_BAD_NUM);
}

struct lval lval_mod(struct lval x, struct lval y)
{
	if (x.type == LVAL_LONG && y.type == LVAL_LONG)
		return lval_long(x.val.num_long % y.val.num_long);
	if (x.type == LVAL_LONG && y.type == LVAL_DOUBLE)
		return lval_double(fmod(x.val.num_long, y.val.num_double));
	if (x.type == LVAL_DOUBLE && y.type == LVAL_LONG)
		return lval_double(fmod(x.val.num_double, y.val.num_long));
	if (x.type == LVAL_DOUBLE && y.type == LVAL_DOUBLE)
		return lval_double(fmod(x.val.num_double, y.val.num_double));
	return lval_err(LERR_BAD_NUM);
}

struct lval lval_pow(struct lval x, struct lval y)
{
	if (x.type == LVAL_LONG && y.type == LVAL_LONG)
		return lval_long(pow(x.val.num_long, y.val.num_long));
	if (x.type == LVAL_LONG && y.type == LVAL_DOUBLE)
		return lval_double(pow(x.val.num_long, y.val.num_double));
	if (x.type == LVAL_DOUBLE && y.type == LVAL_LONG)
		return lval_double(pow(x.val.num_double, y.val.num_long));
	if (x.type == LVAL_DOUBLE && y.type == LVAL_DOUBLE)
		return lval_double(pow(x.val.num_double, y.val.num_double));
	return lval_err(LERR_BAD_NUM);
}

struct lval lval_max(struct lval x, struct lval y)
{
	if (x.type == LVAL_LONG && y.type == LVAL_LONG)
		return lval_long(fmax(x.val.num_long, y.val.num_long));
	if (x.type == LVAL_LONG && y.type == LVAL_DOUBLE)
		return lval_double(fmax(x.val.num_long, y.val.num_double));
	if (x.type == LVAL_DOUBLE && y.type == LVAL_LONG)
		return lval_double(fmax(x.val.num_double, y.val.num_long));
	if (x.type == LVAL_DOUBLE && y.type == LVAL_DOUBLE)
		return lval_double(fmax(x.val.num_double, y.val.num_double));
	return lval_err(LERR_BAD_NUM);
}

struct lval lval_min(struct lval x, struct lval y)
{
	if (x.type == LVAL_LONG && y.type == LVAL_LONG)
		return lval_long(fmin(x.val.num_long, y.val.num_long));
	if (x.type == LVAL_LONG && y.type == LVAL_DOUBLE)
		return lval_double(fmin(x.val.num_long, y.val.num_double));
	if (x.type == LVAL_DOUBLE && y.type == LVAL_LONG)
		return lval_double(fmin(x.val.num_double, y.val.num_long));
	if (x.type == LVAL_DOUBLE && y.type == LVAL_DOUBLE)
		return lval_double(fmin(x.val.num_double, y.val.num_double));
	return lval_err(LERR_BAD_NUM);
}

void lval_print(struct lval v)
{
	switch (v.type) {
	case LVAL_LONG:
		printf("%ld\n", v.val.num_long);
		break;
	case LVAL_DOUBLE:
		printf("%f\n", v.val.num_double);
		break;
	case LVAL_ERR:
		if (v.val.err_short == LERR_DIV_ZERO)
			log_err("Division by zero");
		else if (v.val.err_short == LERR_BAD_OP)
			log_err("Invalid operator");
		else if (v.val.err_short == LERR_BAD_NUM) {
			log_err("Invalid number");
			errno = 0;
		} else
			log_err("Unrecognized error %d", v.val.err_short);
		break;
	default:
		log_err("Unrecognized lval type %d", v.type);
	}
}

struct lval eval_op(char *op, struct lval x, struct lval y)
{
	if (x.type == LVAL_ERR)
		return x;
	else if (y.type == LVAL_ERR)
		return y;

	if (strcmp(op, "+") == 0)
		return lval_add(x, y);
	else if (strcmp(op, "-") == 0)
	      return lval_sub(x, y);
	else if (strcmp(op, "*") == 0)
	      return lval_mul(x, y);
	else if (strcmp(op, "/") == 0)
	      return lval_div(x, y);
	else if (strcmp(op, "%") == 0)
	      return lval_mod(x, y);
	else if (strcmp(op, "^") == 0)
	      return lval_pow(x, y);
	else if (strcmp(op, "max") == 0)
		return lval_max(x, y);
	else if (strcmp(op, "min") == 0)
		return lval_min(x, y);
	else
		sentinel("Unrecognized operator:  '%c'", *op);

error:
	debug("Returning error from eval_op");
	return lval_err(LERR_BAD_OP);
}

struct lval eval(mpc_ast_t *ast)
{
	debug("Entering eval, tag is %s", ast->tag);

	if (strstr(ast->tag, "number")) {
		debug("Parsing a number: %s", ast->contents);
		errno = 0;
		if (strstr(ast->contents, ".")) {
			double x = strtof(ast->contents, NULL);
			debug("Float parsed as %f", x);
			return lval_double(x);
		} else {
			long x = strtol(ast->contents, NULL, 10);
			return errno != ERANGE ? lval_long(x) : lval_err(LERR_BAD_NUM);
		}
	}
	
	/* The operator is always the second child */
	char * op = ast->children[1]->contents;

	/* Recursively call eval on the third child */
	struct lval x = eval(ast->children[2]);
	
	/* Iterate over the remaining children, combining with the operator */
	int i = 3;
	while (strstr(ast->children[i]->tag, "expr")) {
		x = eval_op(op, x, eval(ast->children[i]));
		i++;
	}

	return x;
}

struct lval eval_line(mpc_ast_t *ast)
{
	/* Ignore the regexes ^ and $ */
	mpc_ast_t *expr = ast->children[1];
	return eval(expr);
}

