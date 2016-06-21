/*#include "number.h"*/
#include <stdio.h>
#include <stdlib.h>

#define num_real(x) x->val[0]
#define num_imaginary(x) x->val[1]
#define num_new() malloc(sizeof(number))
#define num_free(x) free(x)

typedef struct _number_raw {
    double val[2];
} number_raw;

typedef number_raw* number;

void num_print(number a) {
    printf("%lf + %lfi\n", a->val[0], a->val[1]);
}

void main() {
    number n = num_new();
    num_real(n) = 5000;
    num_imaginary(n) = 10;
    num_print(n);
    num_free(n);
}