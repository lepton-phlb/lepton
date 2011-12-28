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
#ifndef VJBSDHDR_H
#define VJBSDHDR_H

#include "lwip/tcp.h"

/*
 * Structure of an internet header, naked of options.
 *
 * We declare ip_len and ip_off to be short, rather than u_short
 * pragmatically since otherwise unsigned comparisons can result
 * against negative integers quite easily, and fail in subtle ways.
 */
PACK_STRUCT_BEGIN
struct ip
{
#if defined(NO_CHAR_BITFIELDS)
  u_char   ip_hl_v;  /* bug in GCC for mips means the bitfield stuff will sometimes break - so we use a char for both and get round it with macro's instead... */
#else
#if BYTE_ORDER == LITTLE_ENDIAN
  unsigned ip_hl:4,              /* header length */
           ip_v :4;              /* version */
#elif BYTE_ORDER == BIG_ENDIAN 
  unsigned ip_v :4,              /* version */
           ip_hl:4;              /* header length */
#else
  COMPLAIN - NO BYTE ORDER SELECTED!
#endif
#endif
  u_char  ip_tos;                /* type of service */
  u_short ip_len;                /* total length */
  u_short ip_id;                 /* identification */
  u_short ip_off;                /* fragment offset field */
#define  IP_DF 0x4000            /* dont fragment flag */
#define  IP_MF 0x2000            /* more fragments flag */
#define  IP_OFFMASK 0x1fff       /* mask for fragmenting bits */
  u_char  ip_ttl;                /* time to live */
  u_char  ip_p;                  /* protocol */
  u_short ip_sum;                /* checksum */
  struct  in_addr ip_src,ip_dst; /* source and dest address */
};
PACK_STRUCT_END

typedef u32_t tcp_seq;

/*
 * TCP header.
 * Per RFC 793, September, 1981.
 */
PACK_STRUCT_BEGIN
struct tcphdr  
{
  u_short  th_sport;    /* source port */
  u_short  th_dport;    /* destination port */
  tcp_seq  th_seq;      /* sequence number */
  tcp_seq  th_ack;      /* acknowledgement number */
#if defined(NO_CHAR_BITFIELDS)
  u_char   th_x2_off;
#else
#if BYTE_ORDER == LITTLE_ENDIAN
  unsigned th_x2 :4,    /* (unused) */
           th_off:4;    /* data offset */
#endif
#if BYTE_ORDER == BIG_ENDIAN 
  unsigned th_off:4,    /* data offset */
           th_x2 :4;    /* (unused) */
#endif
#endif
  u_char   th_flags;
  u_short  th_win;      /* window */
  u_short  th_sum;      /* checksum */
  u_short  th_urp;      /* urgent pointer */
};
PACK_STRUCT_END

#endif /* VJBSDHDR_H */
