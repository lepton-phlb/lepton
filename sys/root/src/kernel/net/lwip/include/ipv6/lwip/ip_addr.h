#ifndef __LWIP_IP_ADDR_H__
#define __LWIP_IP_ADDR_H__

#include "lwip/opt.h"

#ifdef __cplusplus
extern "C" {
#endif

#define IP_ADDR_ANY 0

#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/bpstruct.h"
#endif
PACK_STRUCT_BEGIN
 struct ip_addr {
  PACK_STRUCT_FIELD(u32_t addr[4]);
} PACK_STRUCT_STRUCT;
PACK_STRUCT_END
#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/epstruct.h"
#endif

/*
 * struct ipaddr2 is used in the definition of the ARP packet format in
 * order to support compilers that don't have structure packing.
 */
#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/bpstruct.h"
#endif
PACK_STRUCT_BEGIN
struct ip_addr2 {
  PACK_STRUCT_FIELD(u16_t addrw[2]);
} PACK_STRUCT_STRUCT;
PACK_STRUCT_END
#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/epstruct.h"
#endif

#define IP6_ADDR(ipaddr, a,b,c,d,e,f,g,h) do { (ipaddr)->addr[0] = htonl((u32_t)((a & 0xffff) << 16) | (b & 0xffff)); \
                                               (ipaddr)->addr[1] = htonl(((c & 0xffff) << 16) | (d & 0xffff)); \
                                               (ipaddr)->addr[2] = htonl(((e & 0xffff) << 16) | (f & 0xffff)); \
                                               (ipaddr)->addr[3] = htonl(((g & 0xffff) << 16) | (h & 0xffff)); } while(0)

u8_t ip_addr_netcmp(struct ip_addr *addr1, struct ip_addr *addr2,
        struct ip_addr *mask);
u8_t ip_addr_cmp(struct ip_addr *addr1, struct ip_addr *addr2);
void ip_addr_set(struct ip_addr *dest, struct ip_addr *src);
u8_t ip_addr_isany(struct ip_addr *addr);

#define ip_addr_debug_print(debug, ipaddr) \
        LWIP_DEBUGF(debug, ("%"X32_F":%"X32_F":%"X32_F":%"X32_F":%"X32_F":%"X32_F":%"X32_F":%"X32_F"\n", \
         (ntohl(ipaddr->addr[0]) >> 16) & 0xffff, \
         ntohl(ipaddr->addr[0]) & 0xffff, \
         (ntohl(ipaddr->addr[1]) >> 16) & 0xffff, \
         ntohl(ipaddr->addr[1]) & 0xffff, \
         (ntohl(ipaddr->addr[2]) >> 16) & 0xffff, \
         ntohl(ipaddr->addr[2]) & 0xffff, \
         (ntohl(ipaddr->addr[3]) >> 16) & 0xffff, \
         ntohl(ipaddr->addr[3]) & 0xffff));

#ifdef __cplusplus
}
#endif

#endif /* __LWIP_IP_ADDR_H__ */
