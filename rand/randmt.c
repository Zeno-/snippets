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
#include <stdint.h>
#include <stdlib.h>


/**********************************************************************
 * Private
 **********************************************************************/
#define MT_UTNLEN       624
#define MT_MAGICN       397
#define MT_MULTIPLIER   0x6C078965UL
#define MT_MATRIX       0x9908b0dfUL

#define MT_SHIFTA       30
#define MT_SHIFTB       11
#define MT_SHIFTC       7
#define MT_SHIFTD       15
#define MT_SHIFTE       18

#define MT_MAGICMASKA   0x9d2c5680UL
#define MT_MAGICMASKB   0xefc60000UL

#define MT_BIT31        0x80000000UL
#define MT_BITS0TO30    0x7FFFFFFFUL
#define MT_MASK32       0xFFFFFFFFUL

struct mt {
    unsigned long   utn[MT_UTNLEN];
    int             idx;
};

inline static void mt_gen_(struct mt *mt);

inline static void 
mt_init_(struct mt *mt, unsigned long seed)
{
    int i;
    unsigned long *utn = mt->utn;
    
    utn[0] = seed & MT_MASK32;
    for (i = 1; i < MT_UTNLEN; i++) {
        utn[i] = MT_MULTIPLIER  * (utn[i-1] ^ (utn[i-1] >> MT_SHIFTA)) + i;
        utn[i] &= MT_MASK32;
    }
    mt->idx = i;
}

inline static void 
mt_gen_(struct mt *mt)
{
    int i;
    unsigned long *utn = mt->utn, y;    
    static unsigned long magic[2] = {0UL, MT_MATRIX};

    /* Note: The "reference algorithm" checks if mt_init_() has been called and
     *       if not calls mt_init_() with a seed of 5489UL. Because of the
     *       way this implementation is designed it's not possible for 
     *       mt_init_() to have not been called because it gets called by
     *       mtrand_new() which creates the user program MT object
     */
        
    for (i = 0; i < MT_UTNLEN - MT_MAGICN; i++) {
        y = (utn[i] & MT_BIT31) | (utn[i + 1] & MT_BITS0TO30);
        utn[i] = utn[i + MT_MAGICN] ^ (y >> 1) ^ magic[y & 0x1];
    }
    for (; i < MT_UTNLEN - 1; i++) {
        y = (utn[i] & MT_BIT31) | (utn[i + 1] & MT_BITS0TO30);
        utn[i] = utn[i + MT_MAGICN - MT_UTNLEN] ^ (y >> 1) ^ magic[y & 0x1];
    }
    y = (utn[i] & MT_BIT31) | (utn[0] & MT_BITS0TO30);
    utn[i] = utn[MT_MAGICN - 1] ^ (y >> 1) ^ magic[y & 0x1];
        
    mt->idx = 0;
}

inline static unsigned long
mt_rand_(struct mt *mt)
{    
    unsigned long y;
    
    if (mt->idx >= MT_UTNLEN)
        mt_gen_(mt);
    
    y = mt->utn[mt->idx++];
    y ^= (y >> MT_SHIFTB);
    y ^= (y << MT_SHIFTC) & MT_MAGICMASKA;
    y ^= (y << MT_SHIFTD) & MT_MAGICMASKB;
    y ^= (y >> MT_SHIFTE);
    
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
