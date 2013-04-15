/*
 * Copyright (c) 2010, STMicroelectronics.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * This file is part of the Contiki OS
 *
 */
/*---------------------------------------------------------------------------*/
/**
* \file
*					contiki-conf.h for MB851.
* \author
*					Salvatore Pitrulli <salvopitru@users.sourceforge.net>
* \version
*					1.1
*/
/*---------------------------------------------------------------------------*/


#ifndef __CONTIKI_CONF_H__
#define __CONTIKI_CONF_H__

//#include PLATFORM_HEADER

//#include <inttypes.h>
//modif phlb
#include "kernel/core/etypes.h"
#include "kernel/core/kernelconf.h"
#define UIP_RAM_REGION      //_Pragma("location = \"EXT_RAM\"")
//#define EXT_RAM_REGION

#include <string.h>  // For memcpm().

//modify phlb
#define PAP_USERNAME  "user"
#define PAP_PASSWORD  "pass"

#define IPCP_RETRY_COUNT        5
#define IPCP_TIMEOUT            5
#define IPV6CP_RETRY_COUNT        5
#define IPV6CP_TIMEOUT            5
#define xxdebug_printf          printf
#define debug_printf            printf
#define LCP_RETRY_COUNT         10
#define PAP_USERNAME_SIZE       16
#define PAP_PASSWORD_SIZE       16
#define PPP_RX_BUFFER_SIZE      2048 //1024  //GD 2048 for 1280 IPv6 MTU
#define PPP_TX_BUFFER_SIZE      64
#define AHDLC_TX_OFFLINE        5
#define AHDLC_COUNTERS          1 //defined for AHDLC stats support, Guillaume Descamps, September 19th, 2011
#define UIP_CONF_STATISTICS     1 //GD: enable stats like AHDLC

#define CC_CONF_REGISTER_ARGS          0
#define CC_CONF_FUNCTION_POINTER_ARGS  1
#define CC_CONF_FASTCALL
#define CC_CONF_VA_ARGS                1
#define CC_CONF_INLINE                 inline

#define CCIF
#define CLIF

typedef uint8_t   u8_t;
typedef uint16_t u16_t;
typedef uint32_t u32_t;
typedef  int32_t s32_t;
typedef unsigned int uip_stats_t;


#define FIXED_NET_ADDRESS             1
#define NET_ADDR_A                    0x2001
#define NET_ADDR_B                    0xdb8
#define NET_ADDR_C                    0xbbbb
#define NET_ADDR_D                    0xabcd

#define GW_LL_ADDR 0x00,0x80,0xe1,0x02,0,0,0x00,0x8a

#define GW_NET_ADDR_A                0x2001
#define GW_NET_ADDR_B                0xdb8
#define GW_NET_ADDR_C                0xbbbb
#define GW_NET_ADDR_D                0xabce

#define PC_IID_A                     0x0280
#define PC_IID_B                     0xe102
#define PC_IID_C                     0
#define PC_IID_D                     0x154

#ifdef RPL_BORDER_ROUTER
#define UIP_CONF_ROUTER			1
#else
#define UIP_CONF_ROUTER			0
#endif

#define CUSTOM_UIP6_ROUTE               1
#define UIP6_ROUTE_CONF_ENTRIES         1

#define RIMEROUTE 0

#define ROUTING RIMEROUTE
#define RIMEROUTE_CONF_DISCOVERY_TIMEOUT 2

#define WITH_RIME                       0
#define UART1_CONF_TX_WITH_INTERRUPT    0
#define WITH_SERIAL_LINE_INPUT          1
#define ENERGEST_CONF_ON                0


#define QUEUEBUF_CONF_NUM               2

#define TELNETD_CONF_NUMLINES           6            



#define RIMEADDR_CONF_SIZE              8

#define UIP_CONF_LL_802154              0  //modif phlb: set to 0 instead 1

#if defined(USE_IF_SLIP) || defined(USE_IF_PPP)
   #define UIP_CONF_LLH_LEN              0  //modif phlb: udef LLH len will be set to 14 in uipopt.h. for slip must be set to 0.
#endif

#define RADIO_RXBUFS                    2   // Set to a number greater than 1 to decrease packet loss probability at high rates (e.g, with fragmented packets)

#define UIP_CONF_IPV6                   0//1
#define UIP_CONF_IPV6_QUEUE_PKT         0   // This is a very costly feature as it increases the RAM usage by approximately UIP_ND6_MAX_NEIGHBORS * UIP_LINK_MTU bytes.
#define UIP_CONF_IPV6_CHECKS            1
#define UIP_CONF_IPV6_REASSEMBLY        0
#define UIP_CONF_NETIF_MAX_ADDRESSES    3
#define UIP_CONF_ND6_MAX_PREFIXES       2
#define UIP_CONF_ND6_MAX_NEIGHBORS      2
#define UIP_CONF_ND6_MAX_DEFROUTERS     1
#define UIP_CONF_IP_FORWARD             0
#define UIP_CONF_BUFFER_SIZE		       1500 //modif phlb: set 1500 instead 200 
#define UIP_CONF_MAX_CONNECTIONS        2
#define UIP_CONF_MAX_LISTENPORTS        2
#define UIP_CONF_UDP_CONNS              2

#define SICSLOWPAN_UDP_PORT_MIN	0
#define SICSLOWPAN_UDP_PORT_MAX	65535

#define SICSLOWPAN_CONF_COMPRESSION_IPV6        0
#define SICSLOWPAN_CONF_COMPRESSION_HC1         1
#define SICSLOWPAN_CONF_COMPRESSION_HC01        2
#define SICSLOWPAN_CONF_COMPRESSION             SICSLOWPAN_CONF_COMPRESSION_HC1
#define SICSLOWPAN_CONF_FRAG                    1
#define SICSLOWPAN_CONF_CONVENTIONAL_MAC	1
#define SICSLOWPAN_CONF_MAX_ADDR_CONTEXTS       2
#define SICSLOWPAN_CONF_MAXAGE                  2

#define UIP_CONF_UDP              1                 //For 6lowpan UDP compression (and also for uip)
#define UIP_CONF_TCP              1                 //modif phlb: tcp enabled. For 6lowpan TCP compression (and also for uip)

#define UIP_CONF_ICMP6            0

#define UIP_CONF_IPV6_RPL   0 //modif phlb: RPL disabled

#define IEEE802154_CONF_PANID 0x1234

#define STM32W_NODE_ID  0x5678   // to be deleted
#define RF_CHANNEL                15

typedef unsigned long clock_time_t;

#define CLOCK_CONF_SECOND 1000

/* LEDs ports MB851 */
#define LEDS_CONF_RED			0x20
#define LEDS_CONF_GREEN			0x40
#define LEDS_PORT                       GPIO_PBOUT
// Open drain
#define LED_CONFIG()    GPIO_PBCFGH |= 0x0550;   \
                        GPIO_PBCFGH &= 0xf55f


#define UIP_ARCH_ADD32           0 //modif phlb: set to 0 instead 1
#define UIP_ARCH_CHKSUM          0

#define UIP_CONF_BYTE_ORDER      UIP_LITTLE_ENDIAN


#ifdef PROJECT_CONF_H
#include PROJECT_CONF_H
#endif /* PROJECT_CONF_H */


#endif /* __CONTIKI_CONF_H__ */
