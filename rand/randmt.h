/*
 * Mersenne Twister MT19937 (32-bit)
 * 
 * (C) 2013 Robbins, C.D.
 * All rights reserved
 * 10-Nov-2013
 * 
 * License: BSD-3
 */

#ifndef Z_RAND_MT
#define Z_RAND_MT

#define RAND_MT_MAX 0x7fffffff

/* "Handle" for Mersenne Twister object */
typedef struct mt RAND_MT;

/* Create a new RAND_MT object and initalise it using seed. */
RAND_MT *mtrand_new(unsigned long seed);

/* Free resources allocated for RAND_MT created with mtrand_new() */
void mtrand_dispose(RAND_MT *mt);

/* Get random number. */
int mtrand_get(RAND_MT *mt);

#endif /* Z_RAND_MT */
