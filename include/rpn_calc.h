#ifndef __RPN_CALC_INC
#define __RPN_CALC_INC
#include <math.h>
#include <stdio.h>
#include "stack.h"
#include "queue.h"
#include "syard.h"
#include "run_function.h"

double *rpn_calc(queue *in, double (*variable_resolver)(const char*));

#endif /* __RPN_CALC_INC */
