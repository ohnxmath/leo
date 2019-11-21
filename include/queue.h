#ifndef __QUEUE_INC
#define __QUEUE_INC

#define INITAL_QUEUE_LENGTH     4

#include <string.h>
#include <stdlib.h>
#include "api.h"

typedef struct _queue {
    int queue_len; /* length of the queue */
    int queue_buf_len; /* length of the queue buffer */
    void **queue_buf; /* pointer to the start of the queue buffer */
    void **queue; /* pointer to the start of the queue */
} queue;

/* create new queue */
queue *queue_new();
/* remove an item from the queue */
void *queue_dequeue(queue *q);
/* get the first item in the queue */
void *queue_first(queue *q);
/* add an item to the queue */
int queue_enqueue(queue *q, void *data);
/* destroy the queue */
void queue_destroy(queue *q);
/* loop through the queue with a function, with a given context c
 * f is called as f(queue elem, c) */
void queue_foreach(queue *q, void (*f)(void *,void *), void *c);

#endif /* __QUEUE_INC */
