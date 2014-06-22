#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "dbg.h"

#include "mpc.h"
#include "eval.h"

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
	mpc_parser_t *Number   = mpc_new("number");
	mpc_parser_t *Operator = mpc_new("operator");
	mpc_parser_t *Expr     = mpc_new("expr");
	mpc_parser_t *MyLisp   = mpc_new("mylisp");
	/* Define the parsers with the followning language */
	mpca_lang(MPCA_LANG_DEFAULT,
		"										\
		number   : /-?[0-9.]+/ ;							\
		operator : '+' | '-' | '*' | '/' | '%' | '^' | \"min\" | \"max\" ;		\
		expr     : '(' <operator> <expr>+ ')' | <number> ;				\
		mylisp   : /^/ <expr> /$/ ;",
		Number, Operator, Expr, MyLisp);

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
			lval_print(eval_line(ast));
			mpc_ast_delete(ast);
		}

		free(input);
	}

	/* Undefine and delete our parsers */
	mpc_cleanup(4, Number, Operator, Expr, MyLisp);
	return 0;
}
