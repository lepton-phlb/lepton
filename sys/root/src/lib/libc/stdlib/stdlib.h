/*
 * Add appropriate license
 */
#ifndef __CUSTOM_STDLIB_H__
#define __CUSTOM_STDLIB_H__

#include <stddef.h>

typedef struct {
    int quot;
    int rem;
} div_t;

typedef struct {
    long quot;
    long rem;
} ldiv_t;


div_t div(int num, int den);
/* __attribute__((__const__));*/

ldiv_t ldiv(long num, long den);
/*__attribute__((__const__));*/

void abort(void);
#endif /*__CUSTOM_STDLIB_H__*/
