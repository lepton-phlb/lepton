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
#ifndef __LWIP_UDP_H__
#define __LWIP_UDP_H__

#include "lwip/opt.h"

#if LWIP_UDP /* don't build if not configured for use in lwipopts.h */

#include "lwip/pbuf.h"
#include "lwip/netif.h"
#include "lwip/ip_addr.h"
#include "lwip/ip.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UDP_HLEN 8

/* Fields are (of course) in network byte order. */
#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/bpstruct.h"
#endif
PACK_STRUCT_BEGIN
struct udp_hdr {
  PACK_STRUCT_FIELD(u16_t src);
  PACK_STRUCT_FIELD(u16_t dest);  /* src/dest UDP ports */
  PACK_STRUCT_FIELD(u16_t len);
  PACK_STRUCT_FIELD(u16_t chksum);
} PACK_STRUCT_STRUCT;
PACK_STRUCT_END
#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/epstruct.h"
#endif

#define UDP_FLAGS_NOCHKSUM 0x01U
#define UDP_FLAGS_UDPLITE  0x02U
#define UDP_FLAGS_CONNECTED  0x04U

struct udp_pcb {
/* Common members of all PCB types */
  IP_PCB;

/* Protocol specific PCB members */

  struct udp_pcb *next;

  u8_t flags;
  /* ports are in host byte order */
  u16_t local_port, remote_port;

#if LWIP_IGMP
  /* outgoing network interface for multicast packets */
  struct ip_addr multicast_ip;
#endif /* LWIP_IGMP */

#if LWIP_UDPLITE
  /* used for UDP_LITE only */
  u16_t chksum_len_rx, chksum_len_tx;
#endif /* LWIP_UDPLITE */

  /* receive callback function
   * addr and port are in same byte order as in the pcb
   * The callback is responsible for freeing the pbuf
   * if it's not used any more.
   *
   * ATTENTION: Be aware that 'addr' points into the pbuf 'p' so freeing this pbuf
   *            makes 'addr' invalid, too.
   *
   * @param arg user supplied argument (udp_pcb.recv_arg)
   * @param pcb the udp_pcb which received data
   * @param p the packet buffer that was received
   * @param addr the remote IP address from which the packet was received
   * @param port the remote port from which the packet was received
   */
  void (* recv)(void *arg, struct udp_pcb *pcb, struct pbuf *p,
    struct ip_addr *addr, u16_t port);
  /* user-supplied argument for the recv callback */
  void *recv_arg;  
};
/* udp_pcbs export for exernal reference (e.g. SNMP agent) */
extern struct udp_pcb *udp_pcbs;

/* The following functions is the application layer interface to the
   UDP code. */
struct udp_pcb * udp_new        (void);
void             udp_remove     (struct udp_pcb *pcb);
err_t            udp_bind       (struct udp_pcb *pcb, struct ip_addr *ipaddr,
                 u16_t port);
err_t            udp_connect    (struct udp_pcb *pcb, struct ip_addr *ipaddr,
                 u16_t port);
void             udp_disconnect    (struct udp_pcb *pcb);
void             udp_recv       (struct udp_pcb *pcb,
         void (* recv)(void *arg, struct udp_pcb *upcb,
                 struct pbuf *p,
                 struct ip_addr *addr,
                 u16_t port),
         void *recv_arg);
err_t            udp_sendto_if  (struct udp_pcb *pcb, struct pbuf *p, struct ip_addr *dst_ip, u16_t dst_port, struct netif *netif);
err_t            udp_sendto     (struct udp_pcb *pcb, struct pbuf *p, struct ip_addr *dst_ip, u16_t dst_port);
err_t            udp_send       (struct udp_pcb *pcb, struct pbuf *p);

#define          udp_flags(pcb)  ((pcb)->flags)
#define          udp_setflags(pcb, f)  ((pcb)->flags = (f))

/* The following functions are the lower layer interface to UDP. */
void             udp_input      (struct pbuf *p, struct netif *inp);

#define udp_init() /* Compatibility define, not init needed. */

#if UDP_DEBUG
void udp_debug_print(struct udp_hdr *udphdr);
#else
#define udp_debug_print(udphdr)
#endif

#ifdef __cplusplus
}
#endif

#endif /* LWIP_UDP */

#endif /* __LWIP_UDP_H__ */
