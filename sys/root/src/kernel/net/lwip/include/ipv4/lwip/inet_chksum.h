#ifndef __LWIP_INET_CHKSUM_H__
#define __LWIP_INET_CHKSUM_H__

#include "lwip/opt.h"

#include "lwip/pbuf.h"
#include "lwip/ip_addr.h"

#ifdef __cplusplus
extern "C" {
#endif

u16_t inet_chksum(void *dataptr, u16_t len);
u16_t inet_chksum_pbuf(struct pbuf *p);
u16_t inet_chksum_pseudo(struct pbuf *p,
       struct ip_addr *src, struct ip_addr *dest,
       u8_t proto, u16_t proto_len);
#if LWIP_UDPLITE
u16_t inet_chksum_pseudo_partial(struct pbuf *p,
       struct ip_addr *src, struct ip_addr *dest,
       u8_t proto, u16_t proto_len, u16_t chksum_len);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __LWIP_INET_H__ */

