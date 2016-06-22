#include <stdio.h>
#include <stdlib.h>
#include <string.h>

double *original_nums;
int original_amnt;

void findDiff(double *nums, int amnt, int depth) {
    int i, newamnt;
    double *diffs;
    
    newamnt = amnt - 1;
    
    if (newamnt == 1) {
        printf("A pattern was not found.\n");
        return;
    }
    
    diffs = calloc(newamnt, sizeof(double));
    if (diffs == NULL) {
        printf("Error allocating memory!\n");
    }
    
    for (i = 0; i < newamnt; i++) {
        diffs[i] = nums[i+1] - nums[i];
    }
    
    for (i = 1; i < newamnt; i++) {
        if (diffs[i] != diffs[0]) {
            /* recur more */
            findDiff(diffs, newamnt, depth + 1);
            free(diffs);
            return;
        }
    }
    
    /* found difference */
    printf("A pattern was found at the %dth level!\n", depth);
    printf("Common difference: %lf\n", diffs[0]);
    
    if (depth == 0) {
        printf("Equation: t(n) = %lfn + %lf\n", diffs[0], original_nums[0]);
    }
    free(diffs);
    return;
}

int main() {
    double *nums;
    int i = 0, amnt = 0, depth;
    
    printf("How many numbers are there in the series? ");
    scanf("%d", &amnt);
    
    printf("Enter series: \n");
    
    nums = calloc(amnt, sizeof(double));
    if (nums == NULL) {
        printf("Error allocating memory!\n");
    }
    
    for (i = 0; i < amnt; i++) {
        printf("%d\t:", i+1);
        scanf("%lf",&nums[i]);
    }
    
    original_nums = nums;
    original_amnt = amnt;
    
    findDiff(nums, amnt, 0);
    free(nums);
}