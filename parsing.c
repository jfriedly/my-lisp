#include <stdio.h>
#include <stdlib.h>

#include "mpc.h"

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
mpc_result_t parse(mpc_parser_t *parser, char *input)
{
	mpc_result_t r;
	if (mpc_parse("<stdin>", input, parser, &r)) {
		/* Success:  print the AST */
		mpc_ast_print(r.output);
		mpc_ast_delete(r.output);
	} else {
		/* Failure:  print the error */
		mpc_err_print(r.error);
		mpc_err_delete(r.error);
	}
	return r;
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
		"							\
		digits   : /-?[0-9]+/ ;					\
		number   : <digits> '.' <digits> | <digits> ;		\
		operator : '+' | '-' | '*' | '/' | '%' ;		\
		expr     : '(' <operator> <expr>+ ')' | <number> ;	\
		mylisp   : /^/ <operator> <expr>+ /$/ ;",
		Digits, Number, Operator, Expr, MyLisp);

	puts("My-lisp Version 0.0.0.0.1");
	puts("Press Ctrl+C to exit\n");

	while (1) {
		char *input = readline("my-lisp> ");

		add_history(input);

		parse(MyLisp, input);

		free(input);
	}

	/* Undefine and delete our parsers */
	mpc_cleanup(5, Digits, Number, Operator, Expr, MyLisp);
	return 0;
}
