#include <stdio.h>

// static buffer for user input (up to 2KB)
static char input[20148];

int main(int argc, char **argv)
{

	puts("My-lisp Version 0.0.0.0.1");
	puts("Press Ctrl+C to exit\n");

	while (1) {
		/*
		 * We don't use puts here because puts would append a newline.
		 * fputs() requires the stdout argument.
		 */
		fputs("my-lisp> ", stdout);

		/*
		 * fgets() reads up until 2047 bytes, or a newline/EOF
		 * character is * reached.  fgets() reads from the stdin buffer
		 * though, so pasting in newline characters causes multiple
		 * runs through the loop to occur, each reading up to the next
		 * newline character.  Pressing <RETURN> at the prompt again
		 * will then make it output the last line.
		 *
		 * Note that pressing <RETURN> after pasting in 2047 bytes
		 * causes the total string to be 2048 bytes, so you'll see 
		 * an empty line of "No you're a " at the end followed by a
		 * newline.
		 */
		fgets(input, 2048, stdin);

		printf("No, you're a %s", input);
	}

	return 0;
}
