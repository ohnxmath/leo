#ifndef __RPN_CALC_INC
#define __RPN_CALC_INC
#include <math.h>
#include <string.h>
#include "stack.h"
#include "queue.h"
#include "syard.h"
#include "run_function.h"

double *rpn_calc(leo_api *ctx, queue *in);

#endif /* __RPN_CALC_INC */
