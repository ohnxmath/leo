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
    b = malloc(sizeof(double));
    printf("value for variable `%s`? ", name);
    n = fgets(buf, 512, stdin);
    if (!n) return 0;
    sscanf(buf, "%lf", b);
    hashmap_put(hm, name, b);
    return *b;
}

int hm_cleaner(void *context, const char *key, void *value) {
    free(value);
    return 0;
}

int main() {
    queue *q;
    double *r;
    double *pi = malloc(sizeof(double));
    double *e  = malloc(sizeof(double));

    *pi = 3.1415926535;
    *e  = 2.7182818285;

    hm = hashmap_new();
    hashmap_put(hm, "pi", pi);
    hashmap_put(hm, "e", e);

    while (1) {
        printf("> ");
        n = fgets(buf, 512, stdin);
        if (!n) break;
        if (*buf == '\n') continue; /* skip if only newline */

        q = syard_run(buf);
        if (q == NULL) continue;
#ifdef __DEBUG
        queue_foreach(q, testfunc, NULL);
        printf("\n");
#endif

        r = rpn_calc(q, variable_resolver);
        if (r == NULL) continue;
        printf("= %g\n", *r);
        free(r);
    }

    hashmap_iterate(hm, hm_cleaner, NULL);
    hashmap_destroy(hm);
    puts("");
    return 0;
}
