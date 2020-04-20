#ifndef __API_INC
#define __API_INC

#include <stdlib.h>

#define E_NONE      0
#define E_NOMEM     -4
#define ESTR_EXTRA_PAREN_CLOSE "mismatched parentheses; extra )"
#define ESTR_EXTRA_PAREN_OPEN  "mismatched parentheses; extra ("
#define ESTR_UNKNOWN_CHAR "unknown character in equation"
#define ESTR_NO_VARIRABLES "variables not supported"
#define ESTR_NO_FUNCTIONS "function calls not supported"
#define ESTR_FUNC_NOT_FOUND "function not found"
#define ESTR_DANGLING_OPERATOR "dangling operator"
#define ESTR_FUNC_INVALID_ARGS "invalid number of arguments to function"
#define ESTR_NOT_ENOUGH_NUMBERS "not enough values for operator; missing number"
#define ESTR_TOO_MANY_NUMBERS "too many values given; missing operator"
#define ESTR_EXT_FUNC_LOAD "could not load external function"
#define ESTR_EXT_FUNC_SEGF "segmentation fault occurred in external function"

typedef struct _leo_api {
    void *variable_resolver_ctx;
    double (*variable_resolver)(void *, const char *);
    void *function_resolver_ctx;
    double *(*function_resolver)(void *, const char *, size_t, double *);
    const char *error;
} leo_api;

#endif /* __API_INC */
