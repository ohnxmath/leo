#ifndef __TOKENIZER_INC
#define __TOKENIZER_INC
#include <stdlib.h>

typedef enum _tokenizer_type {
    TOKEN_NUMBER,
    TOKEN_OPERATOR,
    TOKEN_LBRACKET,
    TOKEN_RBRACKET,
    TOKEN_END,
    TOKEN_UNKNOWN
} tokenizer_type;

#define DECIMAL_POINT '.'

typedef struct _tokenizer_ctx {
    char *pos;
    tokenizer_type type;
    char removed;
} tokenizer_ctx;

/* create new tokenizer instance */
tokenizer_ctx *tokenizer_new();
/* reset the tokenizer context */
void tokenizer_reset(tokenizer_ctx *ctx, char *ptr);
/* get the next token */
char *tokenizer_next(tokenizer_ctx *ctx);
/* destroy the tokenizer */
void tokenizer_destroy(tokenizer_ctx *ctx);

#endif /* __TOKENIZER_INC */
