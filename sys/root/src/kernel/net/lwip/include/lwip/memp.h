/*
The contents of this file are subject to the Mozilla Public License Version 1.1 
(the "License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS" basis, 
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License for the 
specific language governing rights and limitations under the License.

The Original Code is ______________________________________.

The Initial Developer of the Original Code is ________________________.
Portions created by ______________________ are Copyright (C) ______ _______________________.
All Rights Reserved.

Contributor(s): ______________________________________.

Alternatively, the contents of this file may be used under the terms of the eCos GPL license 
(the  [eCos GPL] License), in which case the provisions of [eCos GPL] License are applicable 
instead of those above. If you wish to allow use of your version of this file only under the
terms of the [eCos GPL] License and not to allow others to use your version of this file under 
the MPL, indicate your decision by deleting  the provisions above and replace 
them with the notice and other provisions required by the [eCos GPL] License. 
If you do not delete the provisions above, a recipient may use your version of this file under 
either the MPL or the [eCos GPL] License."
*/

#ifndef __LWIP_MEMP_H__
#define __LWIP_MEMP_H__

#include "lwip/opt.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Create the list of all memory pools managed by memp. MEMP_MAX represents a NULL pool at the end */
typedef enum {
#define LWIP_MEMPOOL(name,num,size,desc)  MEMP_##name,
#include "lwip/memp_std.h"
  MEMP_MAX
} memp_t;

#if MEM_USE_POOLS
/* Use a helper type to get the start and end of the user "memory pools" for mem_malloc */
typedef enum {
    /* Get the first (via:
       MEMP_POOL_HELPER_START = ((u8_t) 1*MEMP_POOL_A + 0*MEMP_POOL_B + 0*MEMP_POOL_C + 0)*/
    MEMP_POOL_HELPER_FIRST = ((u8_t)
#define LWIP_MEMPOOL(name,num,size,desc)
#define LWIP_MALLOC_MEMPOOL_START 1
#define LWIP_MALLOC_MEMPOOL(num, size) * MEMP_POOL_##size + 0
#define LWIP_MALLOC_MEMPOOL_END
#include "lwip/memp_std.h"
    ) ,
    /* Get the last (via:
       MEMP_POOL_HELPER_END = ((u8_t) 0 + MEMP_POOL_A*0 + MEMP_POOL_B*0 + MEMP_POOL_C*1) */
    MEMP_POOL_HELPER_LAST = ((u8_t)
#define LWIP_MEMPOOL(name,num,size,desc)
#define LWIP_MALLOC_MEMPOOL_START
#define LWIP_MALLOC_MEMPOOL(num, size) 0 + MEMP_POOL_##size *
#define LWIP_MALLOC_MEMPOOL_END 1
#include "lwip/memp_std.h"
    )
} memp_pool_helper_t;

/* The actual start and stop values are here (cast them over)
   We use this helper type and these defines so we can avoid using const memp_t values */
#define MEMP_POOL_FIRST ((memp_t) MEMP_POOL_HELPER_FIRST)
#define MEMP_POOL_LAST   ((memp_t) MEMP_POOL_HELPER_LAST)
#endif /* MEM_USE_POOLS */

#if MEMP_MEM_MALLOC || MEM_USE_POOLS
extern const u16_t memp_sizes[MEMP_MAX];
#endif /* MEMP_MEM_MALLOC || MEM_USE_POOLS */

#if MEMP_MEM_MALLOC

#include "mem.h"

#define memp_init()
#define memp_malloc(type)     mem_malloc(memp_sizes[type])
#define memp_free(type, mem)  mem_free(mem)

#else /* MEMP_MEM_MALLOC */

#if MEM_USE_POOLS
/** This structure is used to save the pool one element came from. */
struct memp_malloc_helper
{
   memp_t poolnr;
};
#endif /* MEM_USE_POOLS */

void  memp_init(void);

#if MEMP_OVERFLOW_CHECK
void *memp_malloc_fn(memp_t type, const char* file, const int line);
#define memp_malloc(t) memp_malloc_fn((t), __FILE__, __LINE__)
#else
void *memp_malloc(memp_t type);
#endif
void  memp_free(memp_t type, void *mem);

#endif /* MEMP_MEM_MALLOC */

#ifdef __cplusplus
}
#endif

#endif /* __LWIP_MEMP_H__ */
