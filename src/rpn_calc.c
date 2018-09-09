#include "rpn_calc.h"

double *rpn_calc(queue *in, double (*variable_resolver)(const char*)) {
    stack *s;
    struct syard_var *tok;
    int i;

    s = stack_new();

    /* While there are input tokens left; Read the next token from input. */
    while ((tok = (struct syard_var *)queue_dequeue(in)) != NULL) {
        if (tok->type == TYPE_DOUBLE) { /* If the token is a value */
            /* Push it onto the stack. */
            stack_push(s, tok);
        } else if (tok->type == TYPE_VAR) { /* If the token is a variable, resolve its value now */
            /* Push it onto the stack. */
            char *vn = (char *)(tok + 1);
            if (!variable_resolver) {
                printf("! Variables not supported\n");
                goto err_cleanup;
            }

            /* Resolve the variable */
            stack_push(s, syard_create_double_raw(variable_resolver(vn)));
            /* free the memory used to store the token */
            free(tok);
        } else if (tok->type == TYPE_CHAR && *((char *)((struct syard_var *)tok + 1)) == ',') {
            /* skip */
        } else if (tok->type == TYPE_FUNCTION) { /* if the token is a function, resolve its value now */
            double *args, *nbr;
            struct syard_var *a;

            args = malloc(*((short *)(tok + 1)) * sizeof(double));

            for (i = *((short *)(tok + 1)) - 1; i >= 0; i--) {
                a = (struct syard_var *)stack_pop(s);

                if (a == NULL) {
                    if (i == 0) break;
                    printf("! not enough values for function `%s`\n", ((char *)(((short *)(tok + 1)) + 1)));
                    free(args);
                    goto err_cleanup;
                }

                args[i] = *((double *)(a + 1));
                free((double *)a);
            }

            nbr = run_function(((char *)(((short *)(tok + 1)) + 1)), *((short *)(tok + 1)), args);

            if (nbr == NULL) {
                free(args);
                goto err_cleanup;
            }

            stack_push(s, syard_create_double_raw(*nbr));
            free(nbr);
            free(args);
            free(tok);
        } else { /* Otherwise, the token is an operator */
            struct syard_var *av, *bv;
            double *a, *b, c;

            /* unary operators check first */
            switch (*((char *)((struct syard_var *)tok + 1))) {
            case 'm':
                bv = (struct syard_var *)stack_pop(s);
                if (bv == NULL) {
                    free(bv);
                    printf("! not enough values for operator `-`\n");
                    goto err_cleanup;
                }
                b = ((double *)(bv + 1));
                c = 0-*b;
                goto single;
            default: break;
            }

            /* Pop the top 2 values from the stack. */
            av = (struct syard_var *)stack_pop(s);
            bv = (struct syard_var *)stack_pop(s);

            /* If there are fewer than 2 values on the stack */
            if (av == NULL || bv == NULL) {
                /* (Error) The user has not input sufficient values in the expression. */
                free(av);
                free(bv);
                printf("! not enough values for operator `%c`\n", *((char *)((struct syard_var *)tok + 1)));
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
            /* Free the memory used for the operator and data */
            free(av);
            single:
            free(bv);
            free(tok);

            /* Push the returned results, if any, back onto the stack. */
            stack_push(s, syard_create_double_raw(c));
        }
    }

    /* If there is only one value in the stack */
    tok = stack_pop(s);
    if (stack_top(s) == NULL) {
        /* That value is the result of the calculation. */
        double result, *result_ptr;

        /* verify tok is not null */
        if (tok == NULL) result = 0;
        else result = *((double *)(tok + 1));

        /* set pointer */
        result_ptr = malloc(sizeof(double));
        *result_ptr = result;

        /* clean up */
        queue_destroy(in);
        stack_destroy(s);
        free(tok);
        return result_ptr;
    }
    
    /* Otherwise, there are more values in the stack */
    /* (Error) The user input has too many values. */
    printf("! too many values inputted\n");
    err_cleanup:
    free(tok);
    queue_foreach(in, syard_queue_cleanup, NULL);
    queue_destroy(in);
    stack_foreach(s, syard_queue_cleanup, NULL);
    stack_destroy(s);
    return NULL;
}
