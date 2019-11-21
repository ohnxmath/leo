#ifndef __RUNFUNC_INC
#define __RUNFUNC_INC
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <signal.h>
#include <string.h>

#include "api.h"

double *run_function(leo_api *ctx, const char *name, size_t argc, double *argv);

#endif /* __RUNFUNC_INC */
