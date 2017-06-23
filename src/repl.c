#include <stdio.h>
#include "queue.h"
#include "stack.h"
#include "tokenizer.h"
#include "syard.h"

/* simple wrapper around puts() to support queue_foreach */
void testfunc(void *d, void *c) {
    switch(((struct syard_var *)d)->type) {
    case TYPE_DOUBLE:
    printf("(dbl)%lf ", *((double *)((struct syard_var *)d + 1)));
    break;
    case TYPE_CHAR:
    printf("(c)%c ", *((char *)((struct syard_var *)d + 1)));
    break;
    }
}

int main() {
    char buf[513];
    queue *q;

    while (1) {
        printf("> ");
        fgets(buf, 512, stdin);
        if (*buf == 'q') break;

        q = syard_run(buf);
        if (q == NULL) continue;
        queue_foreach(q, testfunc, NULL);
        printf("\n");
        
        queue_foreach(q, syard_queue_cleanup, NULL);
        queue_destroy(q);
    }
}
