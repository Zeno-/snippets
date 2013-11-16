#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include "randmt.h"

int main(void)
{
    RAND_MT *mt;    
    size_t i;
    int count[2] = {0};
    
    /* mt = mtrand_new(time(NULL)); */
    mt = mtrand_new(10);
    if (!mt) {
        fputs("Could not initialise RNG. Aborting.", stderr);
        exit(EXIT_FAILURE);
    }
   
    for (i = 0; i < 2000; i++) {
        int x = mtrand_get(mt);
        count[x%2]++;
        assert(x <= RAND_MT_MAX);
        printf("%d\n", x);
    }
    
    printf("0s: %d  1s: %d\n", count[0], count[1]);
    
    mtrand_dispose(mt);
    return EXIT_SUCCESS;
}
