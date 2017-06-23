#include "stack.h"

stack *stack_new() {
    stack *s;

    s = calloc(1, sizeof(stack));
    if (!s) return s;
    s->stack_len = 0;
    s->stack_buf_len = INITAL_STACK_LENGTH;
    s->stack_buf = calloc(INITAL_STACK_LENGTH, sizeof(void *));

    if (!s->stack_buf) {
        free(s);
        return NULL;
    }

    s->stack = s->stack_buf + INITAL_STACK_LENGTH;

    return s;
}

void *stack_pop(stack *s) {/* lazy pop */
    /* overflow check */
    if (s->stack_len == 0) return NULL;

    /* decrease the length of the stack and shift pointer over by 1 */
    s->stack_len--;
    return *(s->stack++); /* return the old pointer before the shift */
                          /* equivalent to doing (s->stack++)[0] */
}

void *stack_top(stack *s) {
    /* overflow check */
    if (s->stack_len == 0) return NULL;

    return *(s->stack); /* equivalent to doing (s->stack)[0] */
}

int stack_push(stack *s, void *data) {
    /* s->stack_buf_len is the length of memory available for stack data storage */
    
    if (s->stack_buf_len == s->stack_len) { /* we're running out of room! need to reallocate */
        void *tmp;
        /* increase stack length */
        s->stack_buf_len = s->stack_buf_len * 2;

        /* attempt realloc, return failure if failed */
        tmp = realloc(s->stack_buf, s->stack_buf_len * sizeof(void *));
        if (!tmp) return E_NOMEM;

        /* update pointer references */
        s->stack_buf = tmp;
        s->stack = s->stack_buf + s->stack_buf_len - s->stack_len;

        /* shift the stack over since realloc keeps stack data on leftmost block */
        memmove(s->stack, s->stack_buf, s->stack_len * sizeof(void *));
    }
    /* now we have enough room to add to the start. */
    s->stack_len++;
    *(--s->stack) = data;
    
    return E_NONE;
}

void stack_destroy(stack *s) {
    free(s->stack_buf);
    free(s);
}

void stack_foreach(stack *s, void (*f)(void *,void *), void *c) {
    int i = 0;
    for (i = 0; i < s->stack_len; i++) {
        f(s->stack[i], c);
    }
}

/* debug code - to use, compile with
 * `gcc src/stack.c -Iinclude/ -Wall -Werror -g -ansi -pedantic -D__STACK_DEBUG` */
#ifdef __STACK_DEBUG
#include <stdio.h>

/* simple wrapper around puts() to support stack_foreach */
void testfunc(void *d, void *c) {
    puts((char *)d);
}

int main() {
    /* test creation of new stack */
    stack *s = stack_new();

    char *str;
    char *one = "one";
    char *two = "two";
    char *three = "three";
    char *four = "four";
    char *five = "five";

    /* test addition of items normal case */
    stack_push(s, three);
    stack_push(s, two);
    stack_push(s, one);

    /* test stack top */
    puts((char *)stack_top(s));

    /* test removal of items until empty */
    while ((str = (char *)stack_pop(s)) != NULL) {
        puts(str);
    }
    puts("-----------");

    /* test addition of items normal case */
    stack_push(s, five);
    stack_push(s, four);
    stack_push(s, three);
    stack_push(s, two);

    /* test addition of items when realloc is required */
    stack_push(s, one);

    /* test foreach of stack */
    stack_foreach(s, testfunc, NULL);

    /* test removal of items until empty */
    while ((str = (char *)stack_pop(s)) != NULL);

    /* test destroying stack */
    stack_destroy(s);
    return 0;
}
#endif
