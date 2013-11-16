/*
 * Mersenne Twister MT19937 (32-bit)
 * 
 * (C) 2013 Robbins, C.D.
 * All rights reserved
 * 10-Nov-2013
 * 
 * License: BSD-3
 */

/* 
 * References: 
 * http://en.wikipedia.org/wiki/Mersenne_twister
 * http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/MT2002/emt19937ar.html
 */

#include "randmt.h"
#include <stdlib.h>
#include <limits.h>

/**********************************************************************
 * Private
 **********************************************************************/

/* See Knuth, TAOCP Vol 2, pp. 106-107 (line 13 of table)
 */
#define KNUTH_MULTIPLIER    1812433253UL
#define KNUTH_SHIFT         30

#define MT_UTNLEN           624
#define MT_MAGICN           397
#define MT_MATRIX           0x9908b0dfUL

#define MT_SHIFTA           11
#define MT_SHIFTB           7
#define MT_SHIFTC           15
#define MT_SHIFTD           18

#define MT_MAGICMASKA       0x9d2c5680UL
#define MT_MAGICMASKB       0xefc60000UL

#define MT_BIT31            0x80000000UL
#define MT_BITS0TO30        0x7FFFFFFFUL
#define MT_MASK32           0xFFFFFFFFUL

/* The question remains whether or not using unsinged long is faster than
 * using uint32_t. If, for example, a 64-bit machine has to shift unaligned
 * uint32_t values (to align them) then perhaps using unsigned long is better.
 * Because the question remains unanswered I am leaving everything as unsigned
 * long for now
 */
#if ULONG_MAX >> 32
#   define MASKRESULT(d,s) ( (d) = (s) & MT_MASK32 )
#else
#   define MASKRESULT(d,s) (void)(0)
#endif

struct mt {
    unsigned long   utn[MT_UTNLEN];
    int             idx;
};

inline static void mt_gen_(struct mt *mt);

inline static unsigned long 
knuth_prng(unsigned long pn, unsigned long n)
{
    unsigned nrn = KNUTH_MULTIPLIER * (pn ^ (pn >> KNUTH_SHIFT)) + n;
    MASKRESULT(nrn, nrn);
    return nrn;
}

inline static void 
mt_init_(struct mt *mt, unsigned long seed)
{
    int i;
    unsigned long *utn = mt->utn;
    
    utn[0] = seed & MT_MASK32;
    for (i = 1; i < MT_UTNLEN; i++)
        utn[i] = knuth_prng(utn[i-1], i);
    
    /* Set mt->idx to i (MT_UTNLEN) so that the first call to mt_rand_()
     * triggers a call to mt_gen_()
     */
    mt->idx = i;
}

inline static unsigned long
mt_combinebits_(unsigned long a, unsigned long b)
{
    return (a & MT_BIT31) | (b & MT_BITS0TO30);
}

inline static unsigned long
mt_matrixmultiply_(unsigned long a, unsigned long b)
{
    static unsigned long magic[2] = {0UL, MT_MATRIX};
    return a ^ (b >> 1) ^ magic[b & 0x01];
}

inline static void 
mt_gen_(struct mt *mt)
{
    int i;
    unsigned long *utn = mt->utn, y;    
    
    /* Note: The "reference algorithm" checks if mt_init_() has been called and
     *       if not calls mt_init_() with a seed of 5489UL. Because of the
     *       way this implementation is designed it's not possible for 
     *       mt_init_() to have not been called because it gets called by
     *       mtrand_new() which creates the user program MT object
     */
        
    for (i = 0; i < MT_UTNLEN - MT_MAGICN; i++) {
        y = mt_combinebits_(utn[i], utn[i + 1]);
        utn[i] = mt_matrixmultiply_(utn[i + MT_MAGICN], y);
    }
    for (; i < MT_UTNLEN - 1; i++) {
        y = mt_combinebits_(utn[i], utn[i + 1]);
        utn[i] = mt_matrixmultiply_(utn[i + MT_MAGICN - MT_UTNLEN], y);
    }
    y = mt_combinebits_(utn[i], utn[0]);
    utn[i] = mt_matrixmultiply_(utn[MT_MAGICN - 1], y);
            
    mt->idx = 0;
}

inline static unsigned long
mt_rand_(struct mt *mt)
{    
    unsigned long y;
    
    if (mt->idx >= MT_UTNLEN)
        mt_gen_(mt);
    
    y = mt->utn[mt->idx++];
    y ^= (y >> MT_SHIFTA);
    y ^= (y << MT_SHIFTB) & MT_MAGICMASKA;
    y ^= (y << MT_SHIFTC) & MT_MAGICMASKB;
    y ^= (y >> MT_SHIFTD);
    
    return y;
}


/**********************************************************************
 * Public
 **********************************************************************/

RAND_MT *
mtrand_new(unsigned long seed)
{
    RAND_MT *mt;
    if ((mt = malloc(sizeof *mt)) != NULL)
        mt_init_(mt, seed);
    return mt;
}

void 
mtrand_dispose(RAND_MT *mt)
{
    free(mt);
}

unsigned long
mtrand_get(RAND_MT *mt)
{
    return mt_rand_(mt);
}
