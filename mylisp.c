#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "dbg.h"

#include "mpc.h"
#include "eval.h"
#include "lval.h"

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

void lenv_add_builtins(struct lenv *env)
{
	lenv_add_builtin(env, "car", builtin_car);
	lenv_add_builtin(env, "cdr", builtin_cdr);
	lenv_add_builtin(env, "list", builtin_list);
	lenv_add_builtin(env, "eval", builtin_eval);
	lenv_add_builtin(env, "lambda", builtin_lambda);
	lenv_add_builtin(env, "not", builtin_not);
	lenv_add_builtin(env, "or", builtin_or);
	lenv_add_builtin(env, "and", builtin_and);
	lenv_add_builtin(env, "if", builtin_if);
	lenv_add_builtin(env, "join", builtin_join);
	lenv_add_builtin(env, "length", builtin_length);
	lenv_add_builtin(env, "set", builtin_set);
	lenv_add_builtin(env, "let", builtin_let);
	lenv_add_builtin(env, "env", builtin_env);
	lenv_add_builtin(env, "exit", builtin_exit);
	lenv_add_builtin(env, "max", builtin_max);
	lenv_add_builtin(env, "min", builtin_min);
	lenv_add_builtin(env, "+", builtin_add);
	lenv_add_builtin(env, "-", builtin_sub);
	lenv_add_builtin(env, "*", builtin_mul);
	lenv_add_builtin(env, "/", builtin_div);
	lenv_add_builtin(env, "%", builtin_mod);
	lenv_add_builtin(env, "^", builtin_pow);
	lenv_add_builtin(env, "=", builtin_eq);
	lenv_add_builtin(env, ">=", builtin_geq);
	lenv_add_builtin(env, "<=", builtin_leq);
	lenv_add_builtin(env, ">", builtin_g);
	lenv_add_builtin(env, "<", builtin_l);

	lenv_set(env, lval_sym("T"), lval_bool(true));
	lenv_set(env, lval_sym("F"), lval_bool(false));
}

int main(int argc, char **argv)
{
	/* Create some mpc parsers */
	mpc_parser_t *Number   = mpc_new("number");
	mpc_parser_t *Symbol   = mpc_new("symbol");
	mpc_parser_t *Expr     = mpc_new("expr");
	mpc_parser_t *SExpr     = mpc_new("sexpr");
	mpc_parser_t *MyLisp   = mpc_new("mylisp");
	/* Define the parsers with the following language
	 *
	 * TODO(jfriedly):  Implement concepts of unary, binary, etc. symbols.
	 * For example, (/ 1) should be a syntax error.
	 */
	mpca_lang(MPCA_LANG_DEFAULT,
		"							\
		number   : /-?[0-9.]+/ ;				\
		symbol   : /[a-zA-Z0-9_+\\-*\\/\\\\=<>!%^&]+/;		\
		expr     : <number> | <symbol> | <sexpr> ;		\
		sexpr    : '(' <expr>* ')' ;				\
		mylisp   : /^/ <sexpr> /$/ ;",
		Number, Symbol, Expr, SExpr, MyLisp);

	puts("My-lisp Version 0.0.0.0.1");
	puts("Use (exit) to quit the REPL, or press Ctrl+C\n");

	/* TODO(jfriedly):  Why does initializing readline set ENOENT? */
	rl_initialize();
	errno = 0;

	struct lenv *env = lenv_new();
	lenv_add_builtins(env);

	while (1) {
		char *input = readline("my-lisp> ");
		add_history(input);

		mpc_ast_t *ast = parse(MyLisp, input);
		if (ast != NULL) {
			/*
			 * To print the expression as it was parsed before
			 * evaluation, uncomment this line:
			 *
			 * lval_println(stderr, lval_read(ast));
			 */
			struct lval *v = lval_eval(env, lval_read(ast));
			lval_println(stdout, v);
			lval_del(v);
			mpc_ast_delete(ast);
		}

		free(input);
	}

	/* Undefine and delete our parsers */
	mpc_cleanup(5, Number, Symbol, Expr, SExpr, MyLisp);
	return 0;
}
