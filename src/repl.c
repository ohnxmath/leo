#include <stdio.h>
#include "queue.h"
#include "syard.h"
#include "rpn_calc.h"

static char buf[513];
static void *n;

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
    printf("(f)%s ", ((char *)((struct syard_var *)d + 1)));
    break;
    }
}

double variable_resolver(const char* name) {
    double a;
    printf("value for variable `%s`? ", name);
    n = fgets(buf, 512, stdin);
    if (!n) return 0;
    sscanf(buf, "%lf", &a);
    return a;
}

int main() {
    queue *q;
    double *r;

    while (1) {
        printf("> ");
        n = fgets(buf, 512, stdin);
        if (!n) break;
        if (*buf == '\n') continue; /* skip if only newline */

        q = syard_run(buf);
        if (q == NULL) continue;
        queue_foreach(q, testfunc, NULL);
        printf("\n");

        r = rpn_calc(q, variable_resolver);
        if (r == NULL) continue;
        printf("= %g\t%lf\n", *r, *r);
        free(r);
    }
    puts("");
}
