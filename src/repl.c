#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "linenoise/linenoise.h"

#include "api.h"
#include "queue.h"
#include "syard.h"
#include "rpn_calc.h"
#include "hashmap.h"
#include "run_function.h"

static char *libm_funcs[] = {
    "sin", "cos", "tan", "asin", "acos", "atan", "atan2", "exp", "exp2",
    "exp10", "log", "log10", "log2", "logb", "pow", "sqrt", "cbrt", "hypot",
    "expm1", "log1p", "sinh", "cosh", "tanh", "asinh", "acosh", "atanh", "erf",
    "erfc", "lgamma", "gamma", "tgamma", "j0", "j1", "y0", "y1", NULL};

struct function {
    queue *left;
    queue *right;
};

struct repl {
    leo_api api;
    hashmap *variables;
    hashmap *local_variables;
    hashmap *functions;
};

#ifdef __DEBUG
/* simple wrapper around puts() to support queue_foreach */
void testfunc(void *d, void *c) {
    switch(((struct syard_var *)d)->type) {
    case TYPE_DOUBLE:
    printf("(n)%f ", *((double *)((struct syard_var *)d + 1)));
    break;
    case TYPE_CHAR:
    printf("(c)%c ", *((char *)((struct syard_var *)d + 1)));
    break;
    case TYPE_VAR:
    printf("(v)%s ", ((char *)((struct syard_var *)d + 1)));
    break;
    case TYPE_FUNCTION:
    printf("(f)%s ", ((char *)(((short *)((struct syard_var *)d + 1)) + 1)));
    break;
    }
}
#endif

int hm_cleaner_vars(void *context, const char *key, void *value) {
    free(value);
    return 0;
}

int hm_cleaner_funcs(void *context, const char *key, void *value) {
    struct function *func = (struct function *)value;

    queue_foreach(func->left, syard_queue_cleanup, NULL);
    queue_destroy(func->left);

    queue_foreach(func->right, syard_queue_cleanup, NULL);
    queue_destroy(func->right);

    free(value);
    return 0;
}

double variable_resolver(void *ctx, const char *name) {
    double *b;
    hashmap *variables;

    /* check if we should be using local variables or global ones */
    if (((struct repl *)ctx)->local_variables) {
        variables = ((struct repl *)ctx)->local_variables;
    } else
        variables = ((struct repl *)ctx)->variables;

    if ((b = hashmap_get(variables, name)) != NULL) return *b;

    fprintf(stderr, "! unknown variable `%s`\n", name);
    return 0;
}

void add_function(hashmap *functions, const char *name, queue *l, queue *r) {
    struct function *func;

    /* check if func already exists */
    func = (struct function *)hashmap_get(functions, name);

    if (!func) {
        /* does not already exist; allocate space */
        /* todo: check if malloc() fails and handle */
        func = malloc(sizeof(struct function));

        /* add the function to the hashmap */
        hashmap_put(functions, name, func);
    } else {
        /* function exists, need to clear the old parts */
        queue_foreach(func->left, syard_queue_cleanup, NULL);
        queue_destroy(func->left);

        queue_foreach(func->right, syard_queue_cleanup, NULL);
        queue_destroy(func->right);
    }

    /* update the left and right parts */
    func->left = l;
    func->right = r;
}

int check_func_wl(const char *f) {
    char **wl = libm_funcs;

    /* loop while the elements aren't null */
    while (*wl != NULL) {
        /* check if the strings are equal, and if yes, return allowed */
        if (!strcmp(*(wl++), f)) return 1;
    }

    /* default: function not found, deny */
    return 0;
}

int duplicate_variables(void *context, const char *key, void *value) {
    hashmap_put((hashmap *)context, key, value);
    return 0;
}

void duplicate_items(void *d, void *c) {
    struct syard_var *var = (struct syard_var *)d;
    struct syard_var *var_dup;

    size_t required_size;

    /* determine how much space is needed to duplicate this item */
    switch(var->type) {
    case TYPE_DOUBLE:
        required_size = sizeof(struct syard_var) + sizeof(double);
        break;
    case TYPE_CHAR:
        required_size = sizeof(struct syard_var) + sizeof(char);
        break;
    case TYPE_VAR:
        required_size = sizeof(struct syard_var) + strlen(((char *)((struct syard_var *)d + 1))) + 1;
        break;
    case TYPE_FUNCTION:
        required_size = sizeof(struct syard_var) + sizeof(short) + strlen(((char *)(((short *)((struct syard_var *)d + 1)) + 1))) + 1;
        break;
    }

    /* allocate the space and copy over */
    var_dup = malloc(required_size);
    memcpy(var_dup, var, required_size);

    /* enqueue the duplicate version */
    queue_enqueue((queue *)c, var_dup);
}

double *calculate_function(void *ctx, struct function *func, size_t argc, double *argv) {
    hashmap *local_variables;
    const char *var_name;
    double *r;
    struct repl *repl;
    queue *func_rs;
    hashmap *tmp;
    int i;

    /* create a new hashmap for local variables */
    local_variables = hashmap_new();
    repl = (struct repl *)ctx;

    /* walk through the global variables and set them appropriately */
    hashmap_iterate(repl->variables, duplicate_variables, local_variables);

    /* sanity check */
    if ((func->left)->queue_len <= argc) {
        (repl->api).error = ESTR_FUNC_INVALID_ARGS;
        return NULL;
    }

    /* insert all of the variables in the local hashmap */
    for (i = 0; i < argc; i++) {
        var_name = ((const char *)((struct syard_var *)(func->left->queue[i]) + 1));

        /* cannot add two variables of the same name, so we need to delete the
         * old one first */
        if (hashmap_get(local_variables, var_name))
            hashmap_remove(local_variables, var_name);

        /* add the variable with the given name */
        hashmap_put(local_variables, var_name, argv + i);
    }

    /* evaluate the RHS */
    /* TODO: check logic here */

    /* need to copy the items in rs queue since rpn_calc destroys input */
    func_rs = queue_new();
    queue_foreach(func->right, duplicate_items, func_rs);

    /* temporarily overwrite repl->local_variables ... */
    tmp = repl->local_variables;
    repl->local_variables = local_variables;

    /* ... to calculate the right hand side */
    r = rpn_calc(&((repl->api)), func_rs);
    if (r == NULL) {
        fprintf(stderr, "! error: %s\n", (repl->api).error);
        (repl->api).error = NULL;
    }

    /* clean up local variables (no need to free their storage because the
     * references are still going to be somewhere else) */
    hashmap_destroy(local_variables);

    /* reset repl->local_variables */
    repl->local_variables = tmp;

    return r;
}

double *function_resolver(void *ctx, const char *name, size_t argc, double *argv) {
    double *nbr_ptr;
    struct function *func;

    /* first check if it is a libm function */
    if (check_func_wl(name)) {
        /* call into libleo run_function */
        nbr_ptr = run_function(&(((struct repl *)ctx)->api), name, argc, argv);
        if (nbr_ptr != NULL) return nbr_ptr;
        else {
            fprintf(stderr, "! error calling function %s: %s\n", name, (((struct repl *)ctx)->api).error);
            (((struct repl *)ctx)->api).error = NULL;
            return NULL;
        }
    }

    /* now check if it's in the function hashmap */
    if ((func = (struct function *)hashmap_get(((struct repl *)ctx)->functions, name)) != NULL) {
        /* found a function */
        return calculate_function(ctx, func, argc, argv);
    }

    /* function not found */
    (((struct repl *)ctx)->api).error = ESTR_FUNC_NOT_FOUND;
    return NULL;
}

int hasSR = 0;
double randd() {
    if (!(hasSR++)) srand(time(NULL));
    return (double)rand() / (double)RAND_MAX;
}

int main() {
    char *o, *p, *line;
    queue *rs, *ls;
    double *r, *s;
    double *pi = malloc(sizeof(double));
    double *e  = malloc(sizeof(double));
    double *ans = malloc(sizeof(double));
    int i;
    struct repl repl_ctx;

    repl_ctx.api.variable_resolver_ctx = &repl_ctx;
    repl_ctx.api.variable_resolver = variable_resolver;
    repl_ctx.api.function_resolver_ctx = &repl_ctx;
    repl_ctx.api.function_resolver = function_resolver;
    repl_ctx.api.error = NULL;
    rs = NULL;
    ls = NULL;

    *pi = 3.1415926535;
    *e  = 2.7182818285;
    *ans = 0;

    repl_ctx.variables = hashmap_new();
    repl_ctx.functions = hashmap_new();
    repl_ctx.local_variables = NULL;
    hashmap_put(repl_ctx.variables, "pi", pi);
    hashmap_put(repl_ctx.variables, "e", e);
    hashmap_put(repl_ctx.variables, "ans", ans);

    /* read input using linenoise */
    while((line = linenoise("> ")) != NULL) {
        /* add the line to history */
        linenoiseHistoryAdd(line);

        /* check if there is an equals sign */
        for (p = line; (*p != '\0') && (*p != '='); p++);
        if (*p == '\0') p = line;
        else { /* equals sign present */
            /* check for spaces */
            for (o = line; (*o != ' ') && (*o != '='); o++);
            *o = '\0';

            p++; /* skip equals sign when parsing equation */
        }

        /* line is ls, p is rs */
        if (p != line) {
            /* left side */
            ls = syard_run(&(repl_ctx.api), line);
            if (!ls)  {
                fprintf(stderr, "! error: %s\n", repl_ctx.api.error);
                repl_ctx.api.error = NULL;
                goto loop_done;
            }
        }

        /* right side */
        rs = syard_run(&(repl_ctx.api), p);
        if (!rs)  {
            fprintf(stderr, "! error: %s\n", repl_ctx.api.error);
            repl_ctx.api.error = NULL;
            goto loop_done;
        }
#ifdef __DEBUG
        if (ls) {
            printf("ls = ");
            queue_foreach(ls, testfunc, NULL);
            printf("\nrs = ");
        }
        queue_foreach(rs, testfunc, NULL);
        printf("\n");
#endif

        if (ls && (((struct syard_var *)queue_last(ls))->type == TYPE_FUNCTION)) {
            /* check if the ls queue is valid */
            for (i = 0; i < ls->queue_len-1; i++) {
                switch (((struct syard_var *)(ls->queue[i]))->type) {
                /* only allow variables and , */
                case TYPE_VAR: continue;
                case TYPE_CHAR:
                    if (*((char *)((struct syard_var *)(ls->queue[i]) + 1)) == ',')
                        continue;
                default:
                    /* other items will result in invalid left hand side */
                    goto invalid_ls;
                }
            }

            /* add the function */
            add_function(repl_ctx.functions, ((char *)(((short *)((struct syard_var *)queue_last(ls) + 1)) + 1)), ls, rs);
            rs = NULL;

            /* done */
            goto loop_done;

        invalid_ls:
            queue_foreach(ls, syard_queue_cleanup, NULL);
            queue_destroy(ls);
            fprintf(stderr, "! unrecognized left hand side\n");

            /* done with the loop */
            goto loop_done;
        }

        if (ls) {
            /* not a function, so no point in using ls anymore */
            queue_foreach(ls, syard_queue_cleanup, NULL);
            queue_destroy(ls);
            ls = NULL;
        }

        /* perform the computation of the right side */
        r = rpn_calc(&(repl_ctx.api), rs);
        if (r == NULL) {
            printf("! error: %s\n", repl_ctx.api.error);
            repl_ctx.api.error = NULL;
            goto loop_done;
        }

        /* print out result */
        printf("= %g\n", *r);

        /* store previous answer */
        *ans = *r;

        if (p != line) {
            /* equals sign present */
            if ((s = hashmap_get(repl_ctx.variables, line)) != NULL) {
                /* existing value, just overwrite it */
                *s = *r;
                free(r);
            } else {
                /* new value */
                hashmap_put(repl_ctx.variables, line, r);
            }
        } else {
            /* no equals sign, so we're done with the number now */
            free(r);
        }

    loop_done:
        /* free storage and stuff */
        ls = NULL;
        linenoiseFree(line);
    }

    hashmap_iterate(repl_ctx.variables, hm_cleaner_vars, NULL);
    hashmap_iterate(repl_ctx.functions, hm_cleaner_funcs, NULL);
    hashmap_destroy(repl_ctx.variables);
    hashmap_destroy(repl_ctx.functions);
    return 0;
}
