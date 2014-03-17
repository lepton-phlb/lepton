
#ifndef RANDM_H
#define RANDM_H

/***********************
*** PUBLIC FUNCTIONS ***
***********************/
/*
 * Initialize the random number generator.
 */
void avRandomInit(void);

/*
 * Churn the randomness pool on a random event.  Call this early and often
 * on random and semi-random system events to build randomness in time for
 * usage.  For randomly timed events, pass a null pointer and a zero length
 * and this will use the system timer and other sources to add randomness.
 * If new random data is available, pass a pointer to that and it will be
 * included.
 */
void avChurnRand(char *randData, u32_t randLen);

/*
 * Randomize our random seed value.  To be called for truely random events
 * such as user operations and network traffic.
 */
#if MD5_SUPPORT
#define avRandomize() avChurnRand(NULL, 0)
#else  /* MD5_SUPPORT */
void avRandomize(void);
#endif /* MD5_SUPPORT */

/*
 * Use the random pool to generate random data.  This degrades to pseudo
 * random when used faster than randomness is supplied using churnRand().
 * Thus it's important to make sure that the results of this are not
 * published directly because one could predict the next result to at
 * least some degree.  Also, it's important to get a good seed before
 * the first use.
 */
void avGenRand(char *buf, u32_t bufLen);

/*
 * Return a new random number.
 */
u32_t avRandom(void);


#endif /* RANDM_H */
