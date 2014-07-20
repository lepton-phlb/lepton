/*--------------------------------------------
| 
|---------------------------------------------
| Project:         
| Project Manager: 
| Source:          ipv6cp.h
| Path:        C:\tauon\sys\root\src\kernel\dev\arch\all\ppp\dev_ppp_uip
| Authors:     
| Plateform:   
| Created:     
| Revision/Date: $Revision:$  $Date:$ 
| Description: 
|---------------------------------------------
| Historic:    
|---------------------------------------------
| Authors     | Date     | Comments  
| $Log:$
|---------------------------------------------*/


/*============================================
| Compiler Directive   
==============================================*/
#ifndef _IPV6CP_H
#define _IPV6CP_H


/*============================================
| Includes 
==============================================*/


/*============================================
| Declaration  
==============================================*/

#include "net/uip.h"
#include "contiki-conf.h"

// Config options (move to pppconfig)
#define  IPCP_RETRY_COUNT	5
#define  IPCP_TIMEOUT	5

/* IPCP Option Types */
//#define IPV6CP_IPADDRESS   0x04 // No IP address option with IPv6CP (RFC 2472)
#define IPV6CP_INTERFACE_IDENTIFIER         0x01
#define IPV6CP_IPV6_COMPRESSION_PROTOCOL    0x02

/* IPCP state machine flags */
#define IPV6CP_TX_UP		   0x01
#define IPV6CP_RX_UP		   0x02
#define IPV6CP_IP_BIT		0x04
#define IPV6CP_TX_TIMEOUT	0x08
#define IPV6CP_PRI_DNS_BIT	0x10
#define IPV6CP_SEC_DNS_BIT	0x20

typedef struct  _ipv6cp
{
  u8_t code;
  u8_t id;
  u16_t len;
  u8_t data;//[0];	
} IPV6CPPKT;



extern	u8_t ipv6cp_state;

void ipv6cp_init(void);
void ipv6cp_task(u8_t *buffer);
void ipv6cp_rx(u8_t *, u16_t);



#endif
