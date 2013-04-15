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
#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__

#define NO_SYS                     0
#define LWIP_SOCKET                0 //(NO_SYS==0)
#define LWIP_NETCONN              (NO_SYS==0)

#define LWIP_IGMP                  1
#define LWIP_ICMP                  1
#define LWIP_SNMP                  0 //1

#define LWIP_DNS                   1

#define LWIP_HAVE_LOOPIF           1

#define TCP_LISTEN_BACKLOG         0

#define LWIP_PROVIDE_ERRNO         1 //lepton
#define LWIP_COMPAT_SOCKETS        1
#define LWIP_SO_RCVTIMEO           1
#define LWIP_SO_RCVBUF             1

#define LWIP_TCPIP_CORE_LOCKING    1

#define LWIP_NETIF_LINK_CALLBACK   0
#define LWIP_NETIF_STATUS_CALLBACK 0

#define MEMP_SANITY_CHECK       0
//#define MEMP_OVERFLOW_CHECK     2
#define MEM_LIBC_MALLOC 1 //lepton modifs use libc malloc instead   // default 0

#include "kernel/core/types.h"
#include "kernel/core/malloc.h"

#define mem_free(__x__) _sys_free(__x__)
#define mem_malloc(__x__) _sys_malloc((int)(__x__))
#define mem_calloc(__x__,__y__) _sys_calloc((int)(__x__),(int)(__y__))
#define mem_realloc(__x__,__size__) _sys_realloc(__x__,(int)(__size__))


//#define LWIP_DEBUG
#ifdef LWIP_DEBUG

   #define LWIP_DBG_MIN_LEVEL         0
   #define PPP_DEBUG                  LWIP_DBG_OFF
   #define MEM_DEBUG                  LWIP_DBG_OFF
   #define MEMP_DEBUG                 LWIP_DBG_OFF
   #define PBUF_DEBUG                 LWIP_DBG_OFF
   #define API_LIB_DEBUG              LWIP_DBG_OFF
   #define API_MSG_DEBUG              LWIP_DBG_OFF
   #define TCPIP_DEBUG                LWIP_DBG_OFF
   #define NETIF_DEBUG                LWIP_DBG_ON
   #define SOCKETS_DEBUG              LWIP_DBG_OFF
   #define DNS_DEBUG                  LWIP_DBG_OFF
   #define AUTOIP_DEBUG               LWIP_DBG_OFF
   #define DHCP_DEBUG                 LWIP_DBG_OFF
   #define IP_DEBUG                   LWIP_DBG_ON
   #define IP_REASS_DEBUG             LWIP_DBG_OFF
   #define ICMP_DEBUG                 LWIP_DBG_ON
   #define IGMP_DEBUG                 LWIP_DBG_OFF
   #define UDP_DEBUG                  LWIP_DBG_OFF
   #define TCP_DEBUG                  LWIP_DBG_OFF
   #define TCP_INPUT_DEBUG            LWIP_DBG_OFF
   #define TCP_OUTPUT_DEBUG           LWIP_DBG_OFF
   #define TCP_RTO_DEBUG              LWIP_DBG_OFF
   #define TCP_CWND_DEBUG             LWIP_DBG_OFF
   #define TCP_WND_DEBUG              LWIP_DBG_OFF
   #define TCP_FR_DEBUG               LWIP_DBG_OFF
   #define TCP_QLEN_DEBUG             LWIP_DBG_OFF
   #define TCP_RST_DEBUG              LWIP_DBG_OFF
#endif

#define LWIP_DBG_TYPES_ON         (LWIP_DBG_ON|LWIP_DBG_TRACE|LWIP_DBG_STATE|LWIP_DBG_FRESH| \
                                   LWIP_DBG_HALT)


/* ---------- Memory options ---------- */
/* MEM_ALIGNMENT: should be set to the alignment of the CPU for which
   lwIP is compiled. 4 byte alignment -> define MEM_ALIGNMENT to 4, 2
   byte alignment -> define MEM_ALIGNMENT to 2. */
/* MSVC port: intel processors don't need 4-byte alignment,
   but are faster that way! */
#define MEM_ALIGNMENT           4

/* MEM_SIZE: the size of the heap memory. If the application will send
a lot of data that needs to be copied, this should be set high. */
#define MEM_SIZE               (32*1024) //(64 lepton modifs) //default 10240

/* MEMP_NUM_PBUF: the number of memp struct pbufs. If the application
   sends a lot of data out of ROM (or other static memory), this
   should be set high. */
#define MEMP_NUM_PBUF           64 //(64 lepton modifs) //default 16
/* MEMP_NUM_RAW_PCB: the number of UDP protocol control blocks. One
   per active RAW "connection". */
#define MEMP_NUM_RAW_PCB        3
/* MEMP_NUM_UDP_PCB: the number of UDP protocol control blocks. One
   per active UDP "connection". */
#define MEMP_NUM_UDP_PCB        4
/* MEMP_NUM_TCP_PCB: the number of simulatenously active TCP
   connections. */
#define MEMP_NUM_TCP_PCB        24 //(24 lepton modifs) //default 5
/* MEMP_NUM_TCP_PCB_LISTEN: the number of listening TCP
   connections. */
#define MEMP_NUM_TCP_PCB_LISTEN 8
/* MEMP_NUM_TCP_SEG: the number of simultaneously queued TCP
   segments. */
#define MEMP_NUM_TCP_SEG        16
/* MEMP_NUM_SYS_TIMEOUT: the number of simulateously active
   timeouts. */
#define MEMP_NUM_SYS_TIMEOUT    15

/* The following four are used only with the sequential API and can be
   set to 0 if the application only will use the raw API. */
/* MEMP_NUM_NETBUF: the number of struct netbufs. */
#define MEMP_NUM_NETBUF         256  //64 lepton modifs) //default 2
/* MEMP_NUM_NETCONN: the number of struct netconns. */
#define MEMP_NUM_NETCONN        48 //(48 lepton modifs) //default 10
/* MEMP_NUM_TCPIP_MSG_*: the number of struct tcpip_msg, which is used
   for sequential API communication and incoming packets. Used in
   src/api/tcpip.c. */
#define MEMP_NUM_TCPIP_MSG_API   64 //(24 lepton modifs) //default 16
#define MEMP_NUM_TCPIP_MSG_INPKT 64 //(24 lepton modifs) //default 16


/* ---------- Pbuf options ---------- */
/* PBUF_POOL_SIZE: the number of buffers in the pbuf pool. */
#define PBUF_POOL_SIZE          100

/* PBUF_POOL_BUFSIZE: the size of each pbuf in the pbuf pool. */
#define PBUF_POOL_BUFSIZE       128

/* PBUF_LINK_HLEN: the number of bytes that should be allocated for a
   link level header. */
#define PBUF_LINK_HLEN          16

/** SYS_LIGHTWEIGHT_PROT
 * define SYS_LIGHTWEIGHT_PROT in lwipopts.h if you want inter-task protection
 * for certain critical regions during buffer allocation, deallocation and memory
 * allocation and deallocation.
 */
#define SYS_LIGHTWEIGHT_PROT    (NO_SYS==0)


/* ---------- TCP options ---------- */
#define LWIP_TCP                1
#define TCP_TTL                 255

/* Controls if TCP should queue segments that arrive out of
   order. Define to 0 if your device is low on memory. */
#define TCP_QUEUE_OOSEQ         1

/* TCP Maximum segment size. */
#define TCP_MSS                 1024

/* TCP sender buffer space (bytes). */
#define TCP_SND_BUF             2048

/* TCP sender buffer space (pbufs). This must be at least = 2 *
   TCP_SND_BUF/TCP_MSS for things to work. */
#define TCP_SND_QUEUELEN       (4 * TCP_SND_BUF/TCP_MSS)

/* TCP writable space (bytes). This must be less than or equal
   to TCP_SND_BUF. It is the amount of space which must be
   available in the tcp snd_buf for select to return writable */
#define TCP_SNDLOWAT           (TCP_SND_BUF/2)

/* TCP receive window. */
#define TCP_WND                 8096

/* Maximum number of retransmissions of data segments. */
#define TCP_MAXRTX              12

/* Maximum number of retransmissions of SYN segments. */
#define TCP_SYNMAXRTX           4


/* ---------- ARP options ---------- */
#define LWIP_ARP                1
#define ARP_TABLE_SIZE          10
#define ARP_QUEUEING            1


/* ---------- IP options ---------- */
/* Define IP_FORWARD to 1 if you wish to have the ability to forward
   IP packets across network interfaces. If you are going to run lwIP
   on a device with only one network interface, define this to 0. */
#define IP_FORWARD              1

/* IP reassembly and segmentation.These are orthogonal even
 * if they both deal with IP fragments */
#define IP_REASSEMBLY           1
#define IP_REASS_MAX_PBUFS      10
#define MEMP_NUM_REASSDATA      10
#define IP_FRAG                 1


/* ---------- ICMP options ---------- */
#define ICMP_TTL                255


/* ---------- DHCP options ---------- */
/* Define LWIP_DHCP to 1 if you want DHCP configuration of
   interfaces. */
#define LWIP_DHCP               0

/* 1 if you want to do an ARP check on the offered address
   (recommended). */
#define DHCP_DOES_ARP_CHECK    (LWIP_DHCP)


/* ---------- AUTOIP options ------- */
#define LWIP_AUTOIP             0
#define LWIP_DHCP_AUTOIP_COOP  (LWIP_DHCP && LWIP_AUTOIP)


/* ---------- UDP options ---------- */
#define LWIP_UDP                1
#define LWIP_UDPLITE            1
#define UDP_TTL                 255


/* ---------- Statistics options ---------- */

#define LWIP_STATS              1
#define LWIP_STATS_DISPLAY      1

#if LWIP_STATS
   #define LINK_STATS              1
   #define IP_STATS                1
   #define ICMP_STATS              1
   #define IGMP_STATS              1
   #define IPFRAG_STATS            1
   #define UDP_STATS               1
   #define TCP_STATS               1
   #define MEM_STATS               1
   #define MEMP_STATS              1
   #define PBUF_STATS              1
   #define SYS_STATS               1
#endif /* LWIP_STATS */


/* ---------- PPP options ---------- */

#define PPP_SUPPORT             0      /* Set > 0 for PPP */

#if PPP_SUPPORT

   #define NUM_PPP                 1   /* Max PPP sessions. */


/* Select modules to enable.  Ideally these would be set in the makefile but
 * we're limited by the command line length so you need to modify the settings
 * in this file.
 */
   #define PPPOE_SUPPORT           1
   #define PPPOS_SUPPORT           1

   #define PAP_SUPPORT             1   /* Set > 0 for PAP. */
   #define CHAP_SUPPORT            1   /* Set > 0 for CHAP. */
   #define MSCHAP_SUPPORT          0   /* Set > 0 for MSCHAP (NOT FUNCTIONAL!) */
   #define CBCP_SUPPORT            0   /* Set > 0 for CBCP (NOT FUNCTIONAL!) */
   #define CCP_SUPPORT             0   /* Set > 0 for CCP (NOT FUNCTIONAL!) */
   #define VJ_SUPPORT              1   /* Set > 0 for VJ header compression. */
   #define MD5_SUPPORT             1   /* Set > 0 for MD5 (see also CHAP) */

#endif /* PPP_SUPPORT */

#endif /* __LWIPOPTS_H__ */
