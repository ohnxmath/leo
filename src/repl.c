#include <stdio.h>
#include "queue.h"
#include "stack.h"
#include "tokenizer.h"
#include "syard.h"
#include "rpn_calc.h"

int main() {
    char buf[513];
    queue *q;
    double *r;

    while (1) {
        printf("> ");
        fgets(buf, 512, stdin);
        if (*buf == 'q') break;

        q = syard_run(buf);
        if (q == NULL) continue;

        r = rpn_calc(q);
        if (r == NULL) continue;
        printf("= %g\n", *r);
        free(r);
    }
}
