#include <stdio.h>
#include <stdlib.h>

#include <editline/readline.h>
#include <editline/history.h>

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
