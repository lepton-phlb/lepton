/*
 * Add appropriate license
 */
#include "stdlib.h"

div_t div(int num, int den) {
    div_t ret;

    ret.quot = num / den;
    ret.rem  = num % den;

    if ((ret.rem < 0) && (num > 0)) {
        ++ret.quot;
        ret.rem -= den;
    } else if ((ret.rem > 0) && (num < 0)) {
        --ret.quot;
        ret.rem += den;
    }

    return ret;
}

ldiv_t ldiv(long num, long den) {
    ldiv_t ret;

    ret.quot = num / den;
    ret.rem  = num % den;

    if ((ret.rem < 0) && (num > 0)) {
        ++ret.quot;
        ret.rem -= den;
    } else if ((ret.rem > 0) && (num < 0)) {
        --ret.quot;
        ret.rem += den;
    }

    return ret;
}
