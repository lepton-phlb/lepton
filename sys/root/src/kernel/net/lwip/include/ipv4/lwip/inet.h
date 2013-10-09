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
#ifndef __LWIP_INET_H__
#define __LWIP_INET_H__

#include "lwip/opt.h"

#ifdef __cplusplus
extern "C" {
#endif

/* For compatibility with BSD code */
struct in_addr {
  u32_t s_addr;
};

#define INADDR_NONE         ((u32_t)0xffffffffUL)  /* 255.255.255.255 */
#define INADDR_LOOPBACK     ((u32_t)0x7f000001UL)  /* 127.0.0.1 */
#define INADDR_ANY          ((u32_t)0x00000000UL)  /* 0.0.0.0 */
#define INADDR_BROADCAST    ((u32_t)0xffffffffUL)  /* 255.255.255.255 */

u32_t inet_addr(const char *cp);
int inet_aton(const char *cp, struct in_addr *addr);
char *inet_ntoa(struct in_addr addr); /* returns ptr to static buffer; not reentrant! */

#ifdef htons
#undef htons
#endif /* htons */
#ifdef htonl
#undef htonl
#endif /* htonl */
#ifdef ntohs
#undef ntohs
#endif /* ntohs */
#ifdef ntohl
#undef ntohl
#endif /* ntohl */

#ifndef LWIP_PLATFORM_BYTESWAP
#define LWIP_PLATFORM_BYTESWAP 0
#endif

#if BYTE_ORDER == BIG_ENDIAN
#define htons(x) (x)
#define ntohs(x) (x)
#define htonl(x) (x)
#define ntohl(x) (x)
#else /* BYTE_ORDER != BIG_ENDIAN */
#ifdef LWIP_PREFIX_BYTEORDER_FUNCS
/* workaround for naming collisions on some platforms */
#define htons lwip_htons
#define ntohs lwip_ntohs
#define htonl lwip_htonl
#define ntohl lwip_ntohl
#endif /* LWIP_PREFIX_BYTEORDER_FUNCS */
#if LWIP_PLATFORM_BYTESWAP
#define htons(x) LWIP_PLATFORM_HTONS(x)
#define ntohs(x) LWIP_PLATFORM_HTONS(x)
#define htonl(x) LWIP_PLATFORM_HTONL(x)
#define ntohl(x) LWIP_PLATFORM_HTONL(x)
#else /* LWIP_PLATFORM_BYTESWAP */
u16_t htons(u16_t x);
u16_t ntohs(u16_t x);
u32_t htonl(u32_t x);
u32_t ntohl(u32_t x);
#endif /* LWIP_PLATFORM_BYTESWAP */

#endif /* BYTE_ORDER == BIG_ENDIAN */

#ifdef __cplusplus
}
#endif

#endif /* __LWIP_INET_H__ */
