#include "tokenizer.h"

tokenizer_ctx *tokenizer_new() {
    /* allocate and verify memory */
    tokenizer_ctx *ctx = calloc(1, sizeof(tokenizer_ctx));
    if (!ctx) return ctx;

    /* return */
    return ctx;
}

void tokenizer_reset(tokenizer_ctx *ctx, char *ptr) {
    /* restore the original character if needed */
    if (ctx->removed) *(ctx->pos) = ctx->removed;

    /* clear values */
    ctx->pos = ptr;
    ctx->removed = 0;
}

static char operator_chars[] = {'+', '-', '/', '*', '^', 0};

static int char_eq(char in, char *eq) {
    /* loop through array until 0 is reached */
    while (*eq) {
        /* check if equals and return 1 */
        if (in == *(eq++)) return 1;
    }
    return 0;
}

char *tokenizer_next(tokenizer_ctx *ctx) {
    char *start, *pos;

    /* sanity check */
    if (ctx->pos == NULL) return NULL;

    /* restore the original character if needed */
    if (ctx->removed) *(ctx->pos) = ctx->removed;

    /* start at last ending */
    pos = ctx->pos;

    /* consume whitespace */
    while (*pos <= ' ' && *pos != 0) pos++;

    /* set start to area past whitespace */
    start = pos;

    /* get the type of token */
    if (*pos >= '0' && *pos <= '9') { /* number */
        unsigned short hasDot = 0;
        checkint:
        /* keep going until we are no longer at a number */
        while (*pos != 0 && (*pos >= '0' && *pos <= '9')) pos++;

        /* was this number a dot? if yes, set the hasDot flag to true and keep going */
        if (!hasDot && *pos == DECIMAL_POINT) {
            hasDot = 1;
            pos++;
            goto checkint; /* return back to loop code */
        }

        /* set the type */
        ctx->type = TOKEN_NUMBER;
    } else if (*pos == '(') { /* left bracket */
        /* pass the operator char and set type */
        pos++;
        ctx->type = TOKEN_LBRACKET;
    } else if (*pos == ')') { /* right bracket */
        /* pass the operator char and set type */
        pos++;
        ctx->type = TOKEN_RBRACKET;
    } else if (char_eq(*pos, operator_chars)) { /* operator */
        /* pass the operator char and set type */
        pos++;
        ctx->type = TOKEN_OPERATOR;
    } else if (*pos == '\0') { /* end */
        ctx->type = TOKEN_END;
        return NULL;
    } else { /* unknown */
        ctx->type = TOKEN_UNKNOWN;
        return NULL;
    }

    /* replace the char with NULL, */
    ctx->removed = *pos;
    *pos = '\0';

    /* update context location, */
    ctx->pos = pos;

    /* return the start position */
    return start;
}

void tokenizer_destroy(tokenizer_ctx *ctx) {
    tokenizer_reset(ctx, NULL);
    free(ctx);
}

/* debug code - to use, compile with
 * `gcc src/tokenizer.c -Iinclude/ -Wall -Werror -g -ansi -pedantic -D__TOKENIZER_DEBUG` */
#ifdef __TOKENIZER_DEBUG
#include <stdio.h>
#include <string.h>

char *strdup(const char *a) {
    size_t len;
    char *dup;

    len = strlen(a) + 1;
    dup = malloc(sizeof(char) * len);
    if (!dup) return dup;

    memcpy(dup, a, len);
    return dup;
}

const char *tokenizer_type_tostring(tokenizer_type tt) {
    switch (tt) {
    case TOKEN_NUMBER:
        return "NUMBER";
    case TOKEN_OPERATOR:
        return "OPERATOR";
    case TOKEN_LBRACKET:
        return "OPEN BRACKET";
    case TOKEN_RBRACKET:
        return "CLOSE BRACKET";
    case TOKEN_END:
        return "END OF STRING";
    default:
        return "UNKNOWN CHARACTER";
    }
}

int main() {
    tokenizer_ctx *ctx;
    char *teststr, *tok;

    /* test creation of new context */
    ctx = tokenizer_new();

    /* tokenizer does not allocate new memory outside of the context since it
     * edits in-place, so we need to strdup() */
    teststr = strdup("12    -(3  /4 * 5) + 6 - 7.89");

    /* test setting the tokenizer string */
    tokenizer_reset(ctx, teststr);
    
    /* output token types */
    while ((tok = tokenizer_next(ctx)) != NULL) {
        printf("token: `%s`; type %s\n", tok, tokenizer_type_tostring(ctx->type));
    }

    /* check ending reason */
    printf("ending due to reason `%s`\n", tokenizer_type_tostring(ctx->type));

    /* don't forget to clean up! */
    tokenizer_destroy(ctx);
    free(teststr);
    return 0;
}
#endif

