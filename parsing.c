#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "mpc.h"

#define DEBUG 0
#include "dbg.h"

/* If we are compiling on Windws, compile these functions */
#ifdef _WIN32

#include <string.h>

static char buffer[2048];

/* Fake Windows readline function */
char *readline(char *prompt)
{
	fputs(prompt, stdout);
	fgets(buffer, 2048, stdin);
	/* TODO(jfriedly):  is strlen() safe against buffer overflows? */
	/* I tried to force it to overflow, but it didn't work. */
	char *cpy = malloc(strlen(buffer) + 1);
	strcpy(cpy, buffer);
	cpy[strlen(cpy) - 1] = '\0';
	return cpy;
}

/* Fake Windows add_history function */
void add_history(char *unused) {}

/* If we're not on Windows, include editline here */
#else

#include <editline/readline.h>
#include <editline/history.h>

#endif

long eval_op(long x, char *op, long y)
{
	if (strcmp(op, "+") == 0)
		return x + y;
	else if (strcmp(op, "-") == 0)
		return x - y;
	else if (strcmp(op, "*") == 0)
		return x * y;
	else if (strcmp(op, "/") == 0)
		return x / y;
	else if (strcmp(op, "%") == 0)
		return x % y;
	else if (strcmp(op, "^") == 0)
		return pow(x, y);
	else if (strcmp(op, "min") == 0)
		return fmin(x, y);
	else if (strcmp(op, "max") == 0)
		return fmax(x, y);
	else
		sentinel("ERROR:  unrecognized operator:  '%c'", *op);

error:
	return 0;
}

long eval(mpc_ast_t *ast)
{
	/* If it's tagged as a number, parse out a long and return it */
	/* TODO(jfriedly):  Support floats! */
	if (strstr(ast->tag, "number"))
		return atoi(ast->contents);
	
	/* The operator is always the second child */
	char * op = ast->children[1]->contents;

	/* Recursively call eval on the third child */
	long x = eval(ast->children[2]);
	
	/* Iterate over the remaining children, combining with the operator */
	int i = 3;
	while (strstr(ast->children[i]->tag, "expr")) {
		x = eval_op(x, op, eval(ast->children[i]));
		i++;
	}

	return x;
}

long eval_line(mpc_ast_t *ast)
{
	/* Ignore the regexes ^ and $ */
	mpc_ast_t *expr = ast->children[1];
	return eval(expr);
}

/* Attempt to parse the user input */
mpc_ast_t *parse(mpc_parser_t *parser, char *input)
{
	mpc_result_t r;
	debug("Parsing...");
	if (!mpc_parse("<stdin>", input, parser, &r)) {
		log_err("Failed to parse expression:");
		mpc_err_print(r.error);
		mpc_err_delete(r.error);
		return NULL;
	}
	debug("Parsed into an AST successfully.");
	return r.output;
}

int main(int argc, char **argv)
{
	/* Create some mpc parsers */
	mpc_parser_t *Digits   = mpc_new("digits");
	mpc_parser_t *Number   = mpc_new("number");
	mpc_parser_t *Operator = mpc_new("operator");
	mpc_parser_t *Expr     = mpc_new("expr");
	mpc_parser_t *MyLisp   = mpc_new("mylisp");
	/* Define the parsers with the followning language */
	mpca_lang(MPCA_LANG_DEFAULT,
		"										\
		digits   : /-?[0-9]+/ ;								\
		number   : <digits> '.' <digits> | <digits> ;					\
		operator : '+' | '-' | '*' | '/' | '%' | '^' | \"min\" | \"max\" ;		\
		expr     : '(' <operator> <expr>+ ')' | <number> ;				\
		mylisp   : /^/ <expr> /$/ ;",
		Digits, Number, Operator, Expr, MyLisp);

	puts("My-lisp Version 0.0.0.0.1");
	puts("Press Ctrl+C to exit\n");

	/* TODO(jfriedly):  Why does initializing readline set ENOENT? */
	rl_initialize();
	errno = 0;

	while (1) {
		char *input = readline("my-lisp> ");
		add_history(input);

		mpc_ast_t *ast = parse(MyLisp, input);
		if (ast != NULL) {
			printf("%ld\n", eval_line(ast));
			mpc_ast_delete(ast);
		}

		free(input);
	}

	/* Undefine and delete our parsers */
	mpc_cleanup(5, Digits, Number, Operator, Expr, MyLisp);
	return 0;
}
