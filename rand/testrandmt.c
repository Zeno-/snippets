#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include "randmt.h"

#define NUM 100000000

int main(void)
{
    RAND_MT *mt;    
    size_t i;
    int count[2] = {0};
    volatile unsigned long x;
    
    /* mt = mtrand_new(time(NULL)); */
    mt = mtrand_new(10);
    if (!mt) {
        fputs("Could not initialise RNG. Aborting.", stderr);
        exit(EXIT_FAILURE);
    }
    
    printf("Generating %lu random numbers\n", NUM);
    for (i = 0; i < NUM; i++)
        printf("%lu\n", mtrand_get(mt));
    printf("Done\n");


    mtrand_dispose(mt);
    return EXIT_SUCCESS;
}
