#ifndef __LWIP_DEF_H__
#define __LWIP_DEF_H__

/* this might define NULL already */
#include "lwip/arch.h"

#define LWIP_MAX(x , y)  (((x) > (y)) ? (x) : (y))
#define LWIP_MIN(x , y)  (((x) < (y)) ? (x) : (y))

#ifndef NULL
#define NULL ((void *)0)
#endif


#endif /* __LWIP_DEF_H__ */

