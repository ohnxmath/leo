#ifndef __SHUNTINGYARD_INC
#define __SHUNTINGYARD_INC
#include <stdlib.h>
#include "queue.h"
#include "stack.h"
#include "tokenizer.h"

enum syard_tvar {
    TYPE_DOUBLE,
    TYPE_CHAR
};

struct syard_var {
    enum syard_tvar type;
};

queue *syard_run(const char *in);
void syard_queue_cleanup(void *d, void *c);

#endif /* __SHUNTINGYARD_INC */