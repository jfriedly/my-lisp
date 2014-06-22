/*
 * Logging macros from Zed Shaw's Learn C the Hard Way
 */
#ifndef __dbg_h__
#define __dbg_h__

#include <errno.h>
#include <stdio.h>
#include <string.h>

#define clean_errno() (errno == 0 ? "None" : strerror(errno))

#define DEBUG 0
#if DEBUG
/* ##__VA_ARGS__ tells the preprocessor to substitute in the extra arguments
 * ("...")
 * NOTE(jfriedly):  Some C compilers, such as cpp, don't support this.
 */
#define debug(M, ...) fprintf(stderr, "DEBUG %s:%d: " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)

/* Use check_debg for common errors that shouldn't typically be reported to the
 * user, but still need to be handled.
 */
#define check_debug(A, M, ...) if(!(A)) { debug(M, ##__VA_ARGS__); errno=0; goto error; }
#else
#define debug(M, ...)
#endif

#define log_info(M, ...) fprintf(stderr, "[INFO] (%s:%d) " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define log_warn(M, ...) fprintf(stderr, "[WARN] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)

#define log_err(M, ...) fprintf(stderr, "[ERROR] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)
/* Use check to check  a condition, log, and cleans up if the condition is
 * false.
 */
#define check(A, M, ...) if(!(A)) { log_err(M, ##__VA_ARGS__); errno=0; goto error; }

/* Put sentinel in places that execution should never reach, like the default
 * case for a switch statement.
 */
#define sentinel(M, ...) { log_err(M, ##__VA_ARGS__); errno=0; goto error; }

/* Use check_mem to check if pointers are valid.  Example::
 * 
 *     char *block = NULL;
 *     block = malloc(4096);
 *     check_mem(block);
 */
#define check_mem(A) check((A), "Out of memory.")

#endif
