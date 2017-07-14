#include <time.h>
#include <stdlib.h>

int hasSR = 0;

/**
 * To compile, run
gcc -c -Wall -Werror -fpic multivar.c
gcc -shared -o multivar.so multivar.o
 * Copy `multivar.so` somewhere, and you can LD_PRELOAD it when running leo.
 * ie, with repl, you can run
LD_PRELOAD=./multivar.so ./leo
 */

double add3(double a, double b, double c) {
    return a + b + c;
}

double randf() {
    if (!(hasSR++)) srand(time(NULL));
    return (double)rand() / (double)RAND_MAX;
}
