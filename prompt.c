#include <stdio.h>
#include <stdlib.h>

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

int main(int argc, char **argv)
{

	puts("My-lisp Version 0.0.0.0.1");
	puts("Press Ctrl+C to exit\n");

	while (1) {
		char *input = readline("my-lisp> ");

		add_history(input);

		printf("No, you're a %s\n", input);

		free(input);
	}

	return 0;
}
