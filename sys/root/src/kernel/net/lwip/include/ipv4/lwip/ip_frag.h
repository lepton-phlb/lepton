
#ifndef __LWIP_IP_FRAG_H__
#define __LWIP_IP_FRAG_H__

#include "lwip/opt.h"
#include "lwip/err.h"
#include "lwip/pbuf.h"
#include "lwip/netif.h"
#include "lwip/ip_addr.h"
#include "lwip/ip.h"

#ifdef __cplusplus
extern "C" {
#endif

#if IP_REASSEMBLY
/* The IP reassembly timer interval in milliseconds. */
#define IP_TMR_INTERVAL 1000

/* IP reassembly helper struct.
 * This is exported because memp needs to know the size.
 */
struct ip_reassdata {
  struct ip_reassdata *next;
  struct pbuf *p;
  struct ip_hdr iphdr;
  u16_t datagram_len;
  u8_t flags;
  u8_t timer;
};

void ip_reass_init(void);
void ip_reass_tmr(void);
struct pbuf * ip_reass(struct pbuf *p);
#endif /* IP_REASSEMBLY */

#if IP_FRAG
err_t ip_frag(struct pbuf *p, struct netif *netif, struct ip_addr *dest);
#endif /* IP_FRAG */

#ifdef __cplusplus
}
#endif

#endif /* __LWIP_IP_FRAG_H__ */
