#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int findDiff(double *nums, int amnt) {
    int i, newamnt;
    double *diffs;
    
    newamnt = amnt - 1;
    
    if (newamnt == 1) {
        return -10000;
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
            return findDiff(diffs, newamnt) + 1;
        }
    }
    
    printf("Common difference: %lf\n", diffs[0]);
    return 1;
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
    
    depth = findDiff(nums, amnt);
    if (depth > 0) printf("A pattern was found at the %dth level!\n", depth);
    else printf("A pattern was not found.\n");
    free(nums);
}