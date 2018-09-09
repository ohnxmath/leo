#ifndef __RPN_CALC_INC
#define __RPN_CALC_INC
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "stack.h"
#include "queue.h"
#include "syard.h"
#include "run_function.h"

double *rpn_calc(queue *in, double (*variable_resolver)(const char*), char **func_wl);

#endif /* __RPN_CALC_INC */
