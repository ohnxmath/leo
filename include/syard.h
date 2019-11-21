#ifndef __SHUNTINGYARD_INC
#define __SHUNTINGYARD_INC
#include <stdlib.h>
#include <stdio.h>
#include "api.h"
#include "queue.h"
#include "stack.h"
#include "tokenizer.h"

enum syard_tvar {
    TYPE_DOUBLE,
    TYPE_CHAR,
    TYPE_VAR,
    TYPE_FUNCTION
};

struct syard_var {
    enum syard_tvar type;
};

void *syard_create_double_raw(double nbr);
queue *syard_run(leo_api *ctx, const char *in);
void syard_string_cleanup(void *d, void *c);
void syard_queue_cleanup(void *d, void *c);

#endif /* __SHUNTINGYARD_INC */
