#include <stdlib.h>
#include <math.h>
#include <string.h>

/* included for testing */
#include <limits.h>
#include <stdio.h>

#define BYTEBITS 8  /* if CHAR_BIT > 8 then bits are 'wasted'. Oh well. */
#define BYTEMASK (BYTEBITS - 1)

#define SIEVELIMIT 50000000

unsigned char *gensieve(unsigned long limit);
unsigned char checkprime(const unsigned char *sieve, unsigned long n);

unsigned long ceilpow2(unsigned long n, unsigned long base2multiple);
void setbit(void *addr, unsigned long b);
void clrbit(void *addr, unsigned long b);
unsigned char isbitset(const void *addr, unsigned long b);

int main(void)
{
    unsigned long i, count;
    unsigned char *sieve;
    
    sieve = gensieve(SIEVELIMIT);
    
    if (sieve) {
        count = 0;
        for (i = 0; i < SIEVELIMIT; i++)
            if (checkprime(sieve, i)) {
                count++;
                //printf("%lu (#%lu)\n", i, count, i);
            }
        printf("Counted %lu\n", count);
        free(sieve);            
    }

    return 0;
}

/* Generate a sieve; calling function is responsible for calling free on
 * the returned pointer.
 * 
 * 0, 1 and 2 are not in the sieve.
 * Even numbers are not in the seive.
 */
unsigned char *
gensieve(unsigned long limit)
{
    unsigned char *bitarr;
    unsigned long arrsz, i, j, chklim;
    
    arrsz = ceilpow2(limit / 2, BYTEBITS) / BYTEBITS;
    
    if ((bitarr = malloc(arrsz)) == NULL)
        return NULL;
    
    memset(bitarr, 0x00, arrsz);     /* Init with all as not deleted */
    chklim = sqrt(limit);
    
    for (i = 3; i <= chklim; i += 2)
        if (!isbitset(bitarr, i / 2))
            for (j = i * i; j < limit; j += i + i)
                setbit(bitarr, j / 2);

    return bitarr;
}

unsigned char 
checkprime(const unsigned char *sieve, unsigned long n)
{
    if (n < 2)
        return 0;
    if (n == 2)
        return 1;
    if (!(n & 1))   /* even numbers not prime (apart from 2) */
        return 0;
    return !isbitset(sieve, n / 2);
} 

/*************************************************************************
 * Bit-related functions
 * Could be macros or inline
 ************************************************************************/

/* Rounds up 'n' to the nearest multiple of 'base2muliple' (which *must* 
 * be a power of 2)
 */
unsigned long 
ceilpow2(unsigned long n, unsigned long base2multiple)
{
    base2multiple--;
    return (n + base2multiple) & ~base2multiple;
}

/* Sets 'b' in the bit array 'addr' */
void
setbit(void *addr, unsigned long b)
{
    unsigned char *p = addr;
    p[b / BYTEBITS] |= (0x1UL << (b & BYTEMASK));
}

/* Clears 'b' in the bit array 'addr' */
void
clrbit(void *addr, unsigned long b)
{
    unsigned char *p = addr;
    p[b / BYTEBITS] &= ~(0x1UL << (b & BYTEMASK));
}

/* Returns 1 if 'b' in the bit array is set, otherwise 0 */
unsigned char
isbitset(const void *addr, unsigned long b)
{
    const unsigned char *p = addr;
    return !!(p[b / BYTEBITS] & (0x1UL << (b & BYTEMASK)));
}
