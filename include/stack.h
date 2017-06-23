#ifndef __STACK_INC
#define __STACK_INC

#define INITAL_STACK_LENGTH     4

#include <string.h>
#include <stdlib.h>
#include "error_codes.h"

typedef struct _stack {
    int stack_len; /* length of the stack */
    int stack_buf_len; /* length of the stack buffer */
    void **stack_buf; /* pointer to the start of the stack buffer */
    void **stack; /* pointer to the start of the stack */
} stack;

/* create new stack */
stack *stack_new();
/* remove an item from the stack */
void *stack_pop(stack *s);
/* get top item from the stack */
void *stack_top(stack *s);
/* add an item to the stack */
int stack_push(stack *s, void *data);
/* destroy the stack */
void stack_destroy(stack *q);
/* loop through the stack with a function, with a given context c
 * f is called as f(stack elem, c) */
void stack_foreach(stack *q, void (*f)(void *,void *), void *c);

#endif /* __STACK_INC */
