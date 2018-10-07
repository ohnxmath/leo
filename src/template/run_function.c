{%
vars = {"xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7"}
 -- If you are seeing this message, you can ignore the following message. %}/*
 * DO NOT EDIT THIS FILE DIRECTLY! This file is automatically generated from
 * the file `template/run_function.c`. The generated file was last updated
 * {{ os.date("%a %b %d %Y at %I:%M %p") }}.
 */
#include "run_function.h"

sigjmp_buf point;

static void handler(int sig, siginfo_t *siginfo, void *arg) {
   longjmp(point, 1);
}

/*
 * call a x86-64 cdecl function
 * 
 * cdecl specifies that the first few program args are, in order:
 * `rdi, rsi, rdx, rcx, r8, r9`. If it's doubles/floats, we use
 * "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7"
 * (aka our case :p)
 * further args are passed in the stack, which is not yet implemented.
 */
double *run_function(const char *name, size_t argc, double *argv) {
    void *f, *handle;
    double ret, *rt;
    struct sigaction sa;

    /* open libm */
    handle = dlopen("libm.so.6", RTLD_LAZY);
    if (!handle) { /* verify success */
        fprintf(stderr, "! failed to support calling functions: %s\n", dlerror());
        return NULL;
    }

    /* find the address of function */
    f = dlsym(handle, name);
    if (f == NULL) { /* verify success */
        fprintf(stderr, "! failed to call function `%s`: %s\n", name, dlerror());
        dlclose(handle);
        return NULL;
    }

    rt = malloc(sizeof(double));
    ret = 0;

    /* handle errors */
    memset(&sa, 0, sizeof(struct sigaction));
    sigemptyset(&sa.sa_mask);
    sa.sa_flags     = SA_NODEFER;
    sa.sa_sigaction = handler;
    sigaction(SIGSEGV, &sa, NULL);
    /* catch error if setjmp() returns 1 */
    if (setjmp(point) == 1) {
        /* a segfault occurred */
        fprintf(stderr, "! a segfault occurred while trying to call function %s(%zu)\n", name, argc);
        free(rt);
        dlclose(handle);
        return NULL;
    }

    switch (argc) {/* generated code */{% for i=0,7 do %}
    {% if i == 7 then %}default:{%
    ri = 6
    else %}case {{ i }}:{%
    ri = i
    end %}
        asm __volatile__ (
            /* create new stack frame */
            "pushq %%rbp\n\t"
            "mov %%rsp, %%rbp\n\t"

            /* put the right items in corresponding registers */{% for j=1,ri do %}
            "movq %{{ j+1 }}, %%{{ vars[j] }}\n\t"{% end %}

            /* call function */
            "call *%1\n\t"
            "movq %%xmm0, %0\n\t"

            /* restore old stack frame */
            "popq %%rbp\n\t"
            : "=r"(ret)
            : "r"(f){% for j=1,ri do %}, "r"(argv[{{ j-1 }}]) {% end %}
            : {% for j=1,ri do %}"{{ vars[j] }}", {% end %}"xmm0", "r15"
        );
        {% --[[ if the number is > 6 then we need to push to stack lol and i'm not 
        gonna bother implementing that xD]]
        if i == 7 then %}return NULL;{% end %}
        break;{% end %}
    }

    dlclose(handle);
    *rt = ret;
    return rt;
}

/* debug code - to use, compile with
 * `gcc src/run_function.c -Iinclude/ -Wall -ldl -Werror -g -D__RUNFUNC_DEBUG -rdynamic`
 * to run, run `LD_PRELOAD=/lib/x86_64-linux-gnu/libm.so.6 ./a.out `
 */
#ifdef __RUNFUNC_DEBUG
#include <stdio.h>
int main() {
    double vars[] = {3.1415926535f / 2.0f, 1.2f};
    double *nbr = run_function("sin", 2, vars);
    if (nbr) {
        printf("Result: %lf\n", *nbr);
        free(nbr);
    }
    return 0;
}
#endif
