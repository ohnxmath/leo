#include <stdio.h>
#include "queue.h"
#include "syard.h"
#include "rpn_calc.h"
#include "hashmap.h"

static char buf[513];
static void *n;
static hashmap *hm;

#ifdef __DEBUG
/* simple wrapper around puts() to support queue_foreach */
void testfunc(void *d, void *c) {
    switch(((struct syard_var *)d)->type) {
    case TYPE_DOUBLE:
    printf("(n)%lf ", *((double *)((struct syard_var *)d + 1)));
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

int main() {
    char *o, *p;
    queue *q;
    double *r, *s;
    double *pi = malloc(sizeof(double));
    double *e  = malloc(sizeof(double));
    double *ans = malloc(sizeof(double));

    *pi = 3.1415926535;
    *e  = 2.7182818285;
    *ans = 0;

    hm = hashmap_new();
    hashmap_put(hm, "pi", pi);
    hashmap_put(hm, "e", e);
    hashmap_put(hm, "ans", ans);

    while (1) {
        printf("> ");
        n = fgets(buf, 512, stdin);
        if (!n) break;
        if (*buf == '\n') continue; /* skip if only newline */

        /* check if there is an equals sign */
        for (p = buf; (*p != '\0') && (*p != '='); p++);
        if (*p == '\0') p = buf;
        else { /* equals sign present */
            /* check for spaces */
            for (o = buf; (*o != ' ') && (*o != '='); o++);
            *o = '\0';

            p++; /* skip equals sign when parsing equation */
        }

        q = syard_run(p);
        if (q == NULL) continue;
#ifdef __DEBUG
        queue_foreach(q, testfunc, NULL);
        printf("\n");
#endif

        r = rpn_calc(q, variable_resolver);
        if (r == NULL) continue;

        printf("= %g\n", *r);

        /* store ans */
        *ans = *r;

        if (p != buf) {
            /* equals sign present */
            if ((s = hashmap_get(hm, buf)) != NULL) {
                /* existing value, just overwrite it */
                *s = *r;
                free(r);
            } else {
                /* new value */
                hashmap_put(hm, buf, r);
            }
        } else {
            /* no equals sign, so we're done with the number now */
            free(r);
        }
    }

    hashmap_iterate(hm, hm_cleaner, NULL);
    hashmap_destroy(hm);
    puts("");
    return 0;
}
