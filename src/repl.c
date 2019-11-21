#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "linenoise/linenoise.h"

#include "api.h"
#include "queue.h"
#include "syard.h"
#include "rpn_calc.h"
#include "hashmap.h"

/*static char *func_wl[] = {
    "sin", "cos", "tan", "asin", "acos", "atan", "atan2", "exp", "exp2",
    "exp10", "log", "log10", "log2", "logb", "pow", "sqrt", "cbrt", "hypot",
    "expm1", "log1p", "sinh", "cosh", "tanh", "asinh", "acosh", "atanh", "erf",
    "erfc", "lgamma", "gamma", "tgamma", "j0", "j1", "y0", "y1", NULL};
*/static hashmap *hm;

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

double variable_resolver(const char* name) {
    double *b;

    if ((b = hashmap_get(hm, name)) != NULL) return *b;

    fprintf(stderr, "! unknown variable `%s`\n", name);
    return 0;
}

int hm_cleaner(void *context, const char *key, void *value) {
    free(value);
    return 0;
}

int hasSR = 0;
double randd() {
    if (!(hasSR++)) srand(time(NULL));
    return (double)rand() / (double)RAND_MAX;
}

int main() {
    char *o, *p, *line;
    queue *q;
    double *r, *s;
    double *pi = malloc(sizeof(double));
    double *e  = malloc(sizeof(double));
    double *ans = malloc(sizeof(double));
    leo_api api;

    api.variable_resolver = variable_resolver;
    api.function_resolver = NULL;
    api.error = NULL;

    *pi = 3.1415926535;
    *e  = 2.7182818285;
    *ans = 0;

    hm = hashmap_new();
    hashmap_put(hm, "pi", pi);
    hashmap_put(hm, "e", e);
    hashmap_put(hm, "ans", ans);

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

        q = syard_run(&api, p);
        if (q == NULL)  {
            printf("! error: %s\n", api.error);
            api.error = NULL;
            continue;
        }
#ifdef __DEBUG
        queue_foreach(q, testfunc, NULL);
        printf("\n");
#endif

        r = rpn_calc(&api, q);
        if (r == NULL) {
            printf("! error: %s\n", api.error);
            api.error = NULL;
            continue;
        }

        printf("= %g\n", *r);

        /* store ans */
        *ans = *r;

        if (p != line) {
            /* equals sign present */
            if ((s = hashmap_get(hm, line)) != NULL) {
                /* existing value, just overwrite it */
                *s = *r;
                free(r);
            } else {
                /* new value */
                hashmap_put(hm, line, r);
            }
        } else {
            /* no equals sign, so we're done with the number now */
            free(r);
        }

        /* free storage and stuff */
        linenoiseFree(line);
    }

    hashmap_iterate(hm, hm_cleaner, NULL);
    hashmap_destroy(hm);
    return 0;
}
