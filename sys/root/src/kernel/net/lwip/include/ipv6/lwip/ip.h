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
#ifndef __LWIP_IP_H__
#define __LWIP_IP_H__

#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/pbuf.h"
#include "lwip/ip_addr.h"

#include "lwip/err.h"

#ifdef __cplusplus
extern "C" {
#endif

#define IP_HLEN 40

#define IP_PROTO_ICMP    58
#define IP_PROTO_UDP     17
#define IP_PROTO_UDPLITE 136
#define IP_PROTO_TCP     6

/* This is passed as the destination address to ip_output_if (not
   to ip_output), meaning that an IP header already is constructed
   in the pbuf. This is used when TCP retransmits. */
#ifdef IP_HDRINCL
#undef IP_HDRINCL
#endif /* IP_HDRINCL */
#define IP_HDRINCL  NULL

#if LWIP_NETIF_HWADDRHINT
#define IP_PCB_ADDRHINT ;u8_t addr_hint
#else
#define IP_PCB_ADDRHINT
#endif /* LWIP_NETIF_HWADDRHINT */

/* This is the common part of all PCB types. It needs to be at the
   beginning of a PCB type definition. It is located here so that
   changes to this common part are made in one location instead of
   having to change all PCB structs. */
#define IP_PCB struct ip_addr local_ip; \
  struct ip_addr remote_ip; \
   /* Socket options */  \
  u16_t so_options;      \
   /* Type Of Service */ \
  u8_t tos;              \
  /* Time To Live */     \
  u8_t ttl;              \
  /* link layer address resolution hint */ \
  IP_PCB_ADDRHINT


/* The IPv6 header. */
struct ip_hdr {
#if BYTE_ORDER == LITTLE_ENDIAN
  u8_t tclass1:4, v:4;
  u8_t flow1:4, tclass2:4;  
#else
  u8_t v:4, tclass1:4;
  u8_t tclass2:8, flow1:4;
#endif
  u16_t flow2;
  u16_t len;                /* payload length */
  u8_t nexthdr;             /* next header */
  u8_t hoplim;              /* hop limit (TTL) */
  struct ip_addr src, dest;          /* source and destination IP addresses */
};

#define IPH_PROTO(hdr) (iphdr->nexthdr)

void ip_init(void);

#include "lwip/netif.h"

struct netif *ip_route(struct ip_addr *dest);

void ip_input(struct pbuf *p, struct netif *inp);

/* source and destination addresses in network byte order, please */
err_t ip_output(struct pbuf *p, struct ip_addr *src, struct ip_addr *dest,
         u8_t ttl, u8_t proto);

err_t ip_output_if(struct pbuf *p, struct ip_addr *src, struct ip_addr *dest,
      u8_t ttl, u8_t proto,
      struct netif *netif);

#define ip_current_netif() NULL
#define ip_current_header() NULL

#if IP_DEBUG
void ip_debug_print(struct pbuf *p);
#endif /* IP_DEBUG */

#ifdef __cplusplus
}
#endif

#endif /* __LWIP_IP_H__ */


