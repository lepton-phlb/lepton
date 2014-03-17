#ifndef __LWIP_INET_H__
#define __LWIP_INET_H__

#include "lwip/opt.h"
#include "lwip/pbuf.h"
#include "lwip/ip_addr.h"

#ifdef __cplusplus
extern "C" {
#endif

u16_t inet_chksum(void *data, u16_t len);
u16_t inet_chksum_pbuf(struct pbuf *p);
u16_t inet_chksum_pseudo(struct pbuf *p,
       struct ip_addr *src, struct ip_addr *dest,
       u8_t proto, u32_t proto_len);

u32_t inet_addr(const char *cp);
s8_t inet_aton(const char *cp, struct in_addr *addr);

#ifndef _MACHINE_ENDIAN_H_
#ifndef _NETINET_IN_H
#ifndef _LINUX_BYTEORDER_GENERIC_H
u16_t htons(u16_t n);
u16_t ntohs(u16_t n);
u32_t htonl(u32_t n);
u32_t ntohl(u32_t n);
#endif /* _LINUX_BYTEORDER_GENERIC_H */
#endif /* _NETINET_IN_H */
#endif /* _MACHINE_ENDIAN_H_ */

#ifdef __cplusplus
}
#endif

#endif /* __LWIP_INET_H__ */

