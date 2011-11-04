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

/* Some ICMP messages should be passed to the transport protocols. This
   is not implemented. */

#include "lwip/opt.h"

#if LWIP_ICMP /* don't build if not configured for use in lwipopts.h */

#include "lwip/icmp.h"
#include "lwip/inet.h"
#include "lwip/ip.h"
#include "lwip/def.h"
#include "lwip/stats.h"

void
icmp_input(struct pbuf *p, struct netif *inp)
{
  u8_t type;
  struct icmp_echo_hdr *iecho;
  struct ip_hdr *iphdr;
  struct ip_addr tmpaddr;

  ICMP_STATS_INC(icmp.recv);

  /* TODO: check length before accessing payload! */

  type = ((u8_t *)p->payload)[0];

  switch (type) {
  case ICMP6_ECHO:
    LWIP_DEBUGF(ICMP_DEBUG, ("icmp_input: ping\n"));

    if (p->tot_len < sizeof(struct icmp_echo_hdr)) {
      LWIP_DEBUGF(ICMP_DEBUG, ("icmp_input: bad ICMP echo received\n"));

      pbuf_free(p);
      ICMP_STATS_INC(icmp.lenerr);
      return;
    }
    iecho = p->payload;
    iphdr = (struct ip_hdr *)((u8_t *)p->payload - IP_HLEN);
    if (inet_chksum_pbuf(p) != 0) {
      LWIP_DEBUGF(ICMP_DEBUG, ("icmp_input: checksum failed for received ICMP echo (%"X16_F")\n", inet_chksum_pseudo(p, &(iphdr->src), &(iphdr->dest), IP_PROTO_ICMP, p->tot_len)));
      ICMP_STATS_INC(icmp.chkerr);
    /*      return;*/
    }
    LWIP_DEBUGF(ICMP_DEBUG, ("icmp: p->len %"S16_F" p->tot_len %"S16_F"\n", p->len, p->tot_len));
    ip_addr_set(&tmpaddr, &(iphdr->src));
    ip_addr_set(&(iphdr->src), &(iphdr->dest));
    ip_addr_set(&(iphdr->dest), &tmpaddr);
    iecho->type = ICMP6_ER;
    /* adjust the checksum */
    if (iecho->chksum >= htons(0xffff - (ICMP6_ECHO << 8))) {
      iecho->chksum += htons(ICMP6_ECHO << 8) + 1;
    } else {
      iecho->chksum += htons(ICMP6_ECHO << 8);
    }
    LWIP_DEBUGF(ICMP_DEBUG, ("icmp_input: checksum failed for received ICMP echo (%"X16_F")\n", inet_chksum_pseudo(p, &(iphdr->src), &(iphdr->dest), IP_PROTO_ICMP, p->tot_len)));
    ICMP_STATS_INC(icmp.xmit);

    /*    LWIP_DEBUGF("icmp: p->len %"U16_F" p->tot_len %"U16_F"\n", p->len, p->tot_len);*/
    ip_output_if (p, &(iphdr->src), IP_HDRINCL,
     iphdr->hoplim, IP_PROTO_ICMP, inp);
    break;
  default:
    LWIP_DEBUGF(ICMP_DEBUG, ("icmp_input: ICMP type %"S16_F" not supported.\n", (s16_t)type));
    ICMP_STATS_INC(icmp.proterr);
    ICMP_STATS_INC(icmp.drop);
  }

  pbuf_free(p);
}

void
icmp_dest_unreach(struct pbuf *p, enum icmp_dur_type t)
{
  struct pbuf *q;
  struct ip_hdr *iphdr;
  struct icmp_dur_hdr *idur;

  /* @todo: can this be PBUF_LINK instead of PBUF_IP? */
  q = pbuf_alloc(PBUF_IP, 8 + IP_HLEN + 8, PBUF_RAM);
  /* ICMP header + IP header + 8 bytes of data */
  if (q == NULL) {
    LWIP_DEBUGF(ICMP_DEBUG, ("icmp_dest_unreach: failed to allocate pbuf for ICMP packet.\n"));
    pbuf_free(p);
    return;
  }
  LWIP_ASSERT("check that first pbuf can hold icmp message",
             (q->len >= (8 + IP_HLEN + 8)));

  iphdr = p->payload;

  idur = q->payload;
  idur->type = (u8_t)ICMP6_DUR;
  idur->icode = (u8_t)t;

  SMEMCPY((u8_t *)q->payload + 8, p->payload, IP_HLEN + 8);

  /* calculate checksum */
  idur->chksum = 0;
  idur->chksum = inet_chksum(idur, q->len);
  ICMP_STATS_INC(icmp.xmit);

  ip_output(q, NULL,
      (struct ip_addr *)&(iphdr->src), ICMP_TTL, IP_PROTO_ICMP);
  pbuf_free(q);
}

void
icmp_time_exceeded(struct pbuf *p, enum icmp_te_type t)
{
  struct pbuf *q;
  struct ip_hdr *iphdr;
  struct icmp_te_hdr *tehdr;

  LWIP_DEBUGF(ICMP_DEBUG, ("icmp_time_exceeded\n"));

  /* @todo: can this be PBUF_LINK instead of PBUF_IP? */
  q = pbuf_alloc(PBUF_IP, 8 + IP_HLEN + 8, PBUF_RAM);
  /* ICMP header + IP header + 8 bytes of data */
  if (q == NULL) {
    LWIP_DEBUGF(ICMP_DEBUG, ("icmp_dest_unreach: failed to allocate pbuf for ICMP packet.\n"));
    pbuf_free(p);
    return;
  }
  LWIP_ASSERT("check that first pbuf can hold icmp message",
             (q->len >= (8 + IP_HLEN + 8)));

  iphdr = p->payload;

  tehdr = q->payload;
  tehdr->type = (u8_t)ICMP6_TE;
  tehdr->icode = (u8_t)t;

  /* copy fields from original packet */
  SMEMCPY((u8_t *)q->payload + 8, (u8_t *)p->payload, IP_HLEN + 8);

  /* calculate checksum */
  tehdr->chksum = 0;
  tehdr->chksum = inet_chksum(tehdr, q->len);
  ICMP_STATS_INC(icmp.xmit);
  ip_output(q, NULL,
      (struct ip_addr *)&(iphdr->src), ICMP_TTL, IP_PROTO_ICMP);
  pbuf_free(q);
}

#endif /* LWIP_ICMP */
