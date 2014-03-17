#ifndef __LWIP_ICMP_H__
#define __LWIP_ICMP_H__

#include "lwip/opt.h"

#if LWIP_ICMP /* don't build if not configured for use in lwipopts.h */

#include "lwip/pbuf.h"
#include "lwip/netif.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ICMP6_DUR  1
#define ICMP6_TE   3
#define ICMP6_ECHO 128    /* echo */
#define ICMP6_ER   129      /* echo reply */


enum icmp_dur_type {
  ICMP_DUR_NET = 0,    /* net unreachable */
  ICMP_DUR_HOST = 1,   /* host unreachable */
  ICMP_DUR_PROTO = 2,  /* protocol unreachable */
  ICMP_DUR_PORT = 3,   /* port unreachable */
  ICMP_DUR_FRAG = 4,   /* fragmentation needed and DF set */
  ICMP_DUR_SR = 5      /* source route failed */
};

enum icmp_te_type {
  ICMP_TE_TTL = 0,     /* time to live exceeded in transit */
  ICMP_TE_FRAG = 1     /* fragment reassembly time exceeded */
};

void icmp_input(struct pbuf *p, struct netif *inp);

void icmp_dest_unreach(struct pbuf *p, enum icmp_dur_type t);
void icmp_time_exceeded(struct pbuf *p, enum icmp_te_type t);

struct icmp_echo_hdr {
  u8_t type;
  u8_t icode;
  u16_t chksum;
  u16_t id;
  u16_t seqno;
};

struct icmp_dur_hdr {
  u8_t type;
  u8_t icode;
  u16_t chksum;
  u32_t unused;
};

struct icmp_te_hdr {
  u8_t type;
  u8_t icode;
  u16_t chksum;
  u32_t unused;
};

#ifdef __cplusplus
}
#endif

#endif /* LWIP_ICMP */

#endif /* __LWIP_ICMP_H__ */

