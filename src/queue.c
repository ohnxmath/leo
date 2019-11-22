#include "queue.h"

queue *queue_new() {
    queue *q;

    q = calloc(1, sizeof(queue));
    if (!q) return q;
    q->queue_len = 0;
    q->queue_buf_len = INITAL_QUEUE_LENGTH;
    q->queue = q->queue_buf = calloc(INITAL_QUEUE_LENGTH, sizeof(void *));
    if (!q->queue) {
        free(q);
        return NULL;
    }

    return q;
}

void *queue_dequeue(queue *q) {/* lazy dequeue */
    /* overflow check */
    if (q->queue_len == 0) return NULL;

    /* decrease the length of the queue and shift pointer over by 1 */
    q->queue_len--;
    return *(q->queue++); /* return the old pointer before the shift */
                          /* equivalent to doing (q->queue++)[0] */
}

void *queue_first(queue *q) {
    /* overflow check */
    if (q->queue_len == 0) return NULL;

    return *(q->queue); /* equivalent to doing (q->queue++)[0] */
}

int queue_enqueue(queue *q, void *data) {
    /* (q->queue_buf + q->queue_buf_len - q->queue) is the length of memory available for queue data storage */
    
    if ((q->queue_buf + q->queue_buf_len - q->queue) == q->queue_len) { /* we're running out of room! */
        if (q->queue == q->queue_buf) { /* cannot shift any more to the left, need to reallocate */
            void *tmp;
            /* increase queue length */
            q->queue_buf_len = q->queue_buf_len * 2;

            /* attempt realloc, return failure if failed */
            tmp = realloc(q->queue_buf, q->queue_buf_len * sizeof(void *));
            if (!tmp) return E_NOMEM;

            /* update pointer references */
            q->queue_buf = q->queue = tmp;
        } else { /* we still have room, we just need to shift everything over */
            memmove(q->queue_buf, q->queue, q->queue_len * sizeof(void *));
            q->queue = q->queue_buf;
        }
    }
    /* now we have enough room to add to the end. */
    q->queue[q->queue_len++] = data;
    
    return E_NONE;
}

void queue_destroy(queue *q) {
    free(q->queue_buf);
    free(q);
}

void queue_foreach(queue *q, void (*f)(void *,void *), void *c) {
    int i = 0;
    for (i = 0; i < q->queue_len; i++)
        f(q->queue[i], c);
}

void *queue_last(queue *q) {
    return q->queue[q->queue_len-1];
}

/* debug code - to use, compile with
 * `gcc src/queue.c -Iinclude/ -Wall -Werror -g -ansi -pedantic -D__QUEUE_DEBUG` */
#ifdef __QUEUE_DEBUG
#include <stdio.h>

/* simple wrapper around puts() to support queue_foreach */
void testfunc(void *d, void *c) {
    puts((char *)d);
}

int main() {
    /* test creation of new queue */
    queue *q = queue_new();

    char *str;
    char *one = "one";
    char *two = "two";
    char *three = "three";
    char *four = "four";
    char *five = "five";

    /* test addition of items normal case */
    queue_enqueue(q, one);
    queue_enqueue(q, two);
    queue_enqueue(q, three);

    /* test removal of items until empty */
    while ((str = (char *)queue_dequeue(q)) != NULL) {
        puts(str);
    }
    puts("-----------");

    /* test addition of items normal case */
    queue_enqueue(q, one);

    /* test addition of items when memmove is required */
    queue_enqueue(q, two);
    queue_enqueue(q, three);
    queue_enqueue(q, four);

    /* test addition of items when realloc is required */
    queue_enqueue(q, five);

    /* test foreach of queue */
    queue_foreach(q, testfunc, NULL);

    /* test removal of items until empty */
    while ((str = (char *)queue_dequeue(q)) != NULL);

    /* test destroying queue */
    queue_destroy(q);
    return 0;
}
#endif
