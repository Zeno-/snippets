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
 * Reference: http://en.wikipedia.org/wiki/Mersenne_twister
 */

#include "randmt.h"
#include <stdint.h>
#include <stdlib.h>


/**********************************************************************
 * Private
 **********************************************************************/
#define MT_UTNLEN       624  
#define MT_MULTIPLIER   0x6C078965
#define MT_SHIFTA       30
#define MT_SHIFTB       11
#define MT_SHIFTC       7
#define MT_SHIFTD       15
#define MT_SHIFTE       18
#define MT_MAGICMASKA   0x9d2c5680
#define MT_MAGICMASKB   0xefc60000
#define MT_MAGICXOR     0x9908b0df
#define MT_BIT31        0x80000000
#define MT_BITS0TO30    0x7FFFFFFF

struct mt {
    int32_t         utn[MT_UTNLEN];
    size_t          idx;
};

inline static void mt_gen_(struct mt *mt);

inline static void 
mt_init_(struct mt *mt, int32_t seed)
{
    size_t i;
    int32_t *utn = mt->utn;
    
    utn[0] = seed;
    for (i = 1; i < MT_UTNLEN; i++) {
        uint32_t tmp = (utn[i-1] ^ (utn[i-1] >> MT_SHIFTA)) + i;
        utn[i] = MT_MULTIPLIER * tmp;
    }
    mt->idx = 0;
}

inline static void 
mt_gen_(struct mt *mt)
{
    size_t i;
    int32_t *utn = mt->utn;
    int32_t y;

    for (i = 0; i < MT_UTNLEN; i++) {
        y = (utn[i] & MT_BIT31) + (utn[(i+1) % MT_UTNLEN] & MT_BITS0TO30);
        utn[i] = utn[(i + 397) % MT_UTNLEN] ^ (y >> 1);
        if ((unsigned)y & 1)  /* If odd */
            utn[i] ^= MT_MAGICXOR;
    }
}

inline static uint32_t 
mt_rand_(struct mt *mt)
{
    int32_t y;
    
    if (mt->idx == 0)
        mt_gen_(mt);
    
    y = mt->utn[mt->idx];
    y ^= (y >> MT_SHIFTB);
    y ^= (y << MT_SHIFTC) & MT_MAGICMASKA;
    y ^= (y << MT_SHIFTD) & MT_MAGICMASKB;
    y ^= (y >> MT_SHIFTE);
    
    mt->idx = (mt->idx + 1) % MT_UTNLEN;
    
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

int 
mtrand_get(RAND_MT *mt)
{
    return mt_rand_(mt) & MT_BITS0TO30;
}
