#include "rpn_calc.h"

static void *create_double_data(double repr) {
    struct syard_var *v = malloc(sizeof(struct syard_var) + sizeof(double));
    v->type = TYPE_DOUBLE;
    /* advance past the type variable to reach the data storage for double */
    *((double *)(v + 1)) = repr;
    return v;
}

double *rpn_calc(queue *in) {
    stack *s;
    struct syard_var *tok;

    s = stack_new();

    /* While there are input tokens left; Read the next token from input. */
    while ((tok = (struct syard_var *)queue_dequeue(in)) != NULL) {
        /* If the token is a value */
        if (tok->type == TYPE_DOUBLE) {
            /* Push it onto the stack. */
            stack_push(s, tok);
        } else { /* Otherwise, the token is an operator */
            /* It is already known that the operator takes 2 arguments. */
            struct syard_var *av, *bv;
            double *a, *b, c;
            /* Pop the top 2 values from the stack. */
            av = (struct syard_var *)stack_pop(s);
            bv = (struct syard_var *)stack_pop(s);

            /* If there are fewer than 2 values on the stack */
            if (av == NULL || bv == NULL) {
                /* (Error) The user has not input sufficient values in the expression. */
                free(av);
                free(bv);
                printf("! Not enough values for operator `%c`\n", *((char *)((struct syard_var *)tok + 1)));
                goto err_cleanup;
            }

            /* Get values */
            a = ((double *)(av + 1));
            b = ((double *)(bv + 1));
            /* Evaluate the operator, with the values as arguments. */
            switch (*((char *)((struct syard_var *)tok + 1))) {
            case '+':
                c = *b + *a;
                break;
            case '-':
                c = *b - *a;
                break;
            case '*':
                c = *b * *a;
                break;
            case '/':
                c = *b / *a;
                break;
            case '^':
                c = pow(*b, *a);
                break;
            default:
                /* unrecognized... */
                break;
            }
            /* Push the returned results, if any, back onto the stack. */
            stack_push(s, create_double_data(c));

            /* Free the memory used for the operator and data */
            free(av);
            free(bv);
            free(tok);
        }
    }

    /* If there is only one value in the stack */
    tok = stack_pop(s);
    if (stack_top(s) == NULL) {
        /* That value is the result of the calculation. */
        double result = *((double *)(tok + 1));
        double *result_ptr = malloc(sizeof(double));
        *result_ptr = result;
        queue_destroy(in);
        stack_destroy(s);
        free(tok);
        return result_ptr;
    }
    
    /* Otherwise, there are more values in the stack */
    /* (Error) The user input has too many values. */
    printf("! Too many values inputted\n");
    err_cleanup:
    free(tok);
    queue_foreach(in, syard_queue_cleanup, NULL);
    queue_destroy(in);
    stack_foreach(s, syard_queue_cleanup, NULL);
    stack_destroy(s);
    return NULL;
}