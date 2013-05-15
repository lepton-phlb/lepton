/*															
 *---------------------------------------------------------------------------
 * ipv6cp.c - PPP IPV6CP (internet protocol) Processor/Handler
 *
 *---------------------------------------------------------------------------
 *
 * Version 
 *		0.1 Original Version Jun 3, 2000
 *	
 *---------------------------------------------------------------------------
 *        
 * Copyright (C) 2000, Mycal Labs www.mycal.com
 * 
 *---------------------------------------------------------------------------
 */
/*
 * Copyright (c) 2003, Mike Johnson, Mycal Labs, www.mycal.net
 * All rights reserved. 
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met: 
 * 1. Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the distribution. 
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by Mike Johnson/Mycal Labs
 *		www.mycal.net.
 * 4. The name of the author may not be used to endorse or promote
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
 * This file is part of the Mycal Modified uIP TCP/IP stack.
 *
 * $Id: ipv6cp.c,v 1.2 2010/10/19 18:29:03 adamdunkels Exp $
 *
 */

/*			*/ 
/* include files 	*/
/*			*/ 

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define DEBUG DEBUG_NONE //DEBUG_PRINT
#endif

#include "net/uip-debug.h"

#if UIP_LOGGING == 1
//   #include <stdio.h>
//   #define DEBUG1(x) debug_printf x
#include "uip_core/uip_events.h"
#define EVT_UIP_LOCAL_FILE_CODE     6
#else
   #define UIP_LOG(x)
#endif

#include "net/uip.h"
/*#include "time.h"*/
#include "ipv6cp.h"
#include "ppp.h"
#include "ahdlc.h"

static unsigned int _ipv6cp_timeout=1;
#define TIMER_expire()_ipv6cp_timeout=1
#define TIMER_set() _ipv6cp_timeout=0;
#define TIMER_timeout(x) _ipv6cp_timeout

//modify phlb
//#define IPCP_GET_PEER_IP


/*
 * Local IPCP state
 */
u8_t ipv6cp_state;

/*
 * in the future add compression protocol and name servers (possibly for servers only)
 */
u8_t ipv6cplist[] = {0x01,0x2,0x3,0x4, 0};	

/*---------------------------------------------------------------------------*/
#if 1
void
printip(uip_ipaddr_t ip2)
{
#if (UIP_LOGGING == 1) && (DEBUG & DEBUG_ANNOTATE)
  unsigned char *ip = (unsigned char *)ip2.u8;
  ANNOTATE(" %d:%d:%d:%d ",ip[0],ip[1],ip[2],ip[3]);
#endif
}
#else
#define printip(x)
#endif
/*---------------------------------------------------------------------------*/
void
ipv6cp_init(void)
{
  ANNOTATE("ipv6cp init\n");
  ipv6cp_state = 0;
  ppp_retry = 0;
  pppif.ipaddr.u16[0] = pppif.ipaddr.u16[1] = 0;
  TIMER_expire();
}
/*---------------------------------------------------------------------------*/
/*
 * IPCP RX protocol Handler
 */
void
ipv6cp_rx(u8_t *buffer, u16_t count)
{
  u8_t *bptr = buffer;
//  IPCPPKT *pkt=(IPCPPKT *)buffer;
  u16_t len;

  ANNOTATE("IPV6CP len %d\n",count);
	
  switch(*bptr++) {

  case CONF_REQ:
    /* parce request and see if we can ACK it */
    ++bptr;
    len = (*bptr++ << 8);
    len |= *bptr++;
    /* len-=2; */

    ANNOTATE("check lcplist\n");
    if(scan_packet(IPV6CP, ipv6cplist, buffer, bptr, (u16_t)(len - 4))) {
      UIP_LOG("option was bad\n");
    } else {
      ANNOTATE("IPV6CP options are good\n");
      /*
       * Parse out the results
       */
      /* lets try to implement what peer wants */
      /* Reject any protocol not */
      /* Error? if we we need to send a config Reject ++++ this is
	 good for a subroutine*/
      //GD-2011-09-15 None of the IPv6CP options are supported with current implementation.
#if 0
#warning option implementation example based on IPCP IPv4 address configuration.
      /* All we should get is the peer IP address */
      if(IPV6CP_IPADDRESS == *bptr++) {
	         /* dump length */
	         ++bptr;
         #ifdef IPV6CP_GET_PEER_IP
	         ((u8_t*)peer_ip_addr.u8)[0] = *bptr++;
	         ((u8_t*)peer_ip_addr.u8)[1] = *bptr++;
	         ((u8_t*)peer_ip_addr.u8)[2] = *bptr++;
	         ((u8_t*)peer_ip_addr.u8)[3] = *bptr++;
	         ANNOTATE("Peer IP ");
	         /*	printip(peer_ip_addr);*/
	         ANNOTATE("\n");
         #else
	         bptr += 18;
         #endif
      } else {
	      UIP_LOG("HMMMM this shouldn't happen IPV6CP1\n");
      }
#endif
      
#if 0			
      if(error) {
	      /* write the config NAK packet we've built above, take on the header */
	      bptr = buffer;
	      *bptr++ = CONF_NAK;		/* Write Conf_rej */
	      *bptr++;
	      /*tptr++;*/  /* skip over ID */

	      /* Write new length */
	      *bptr++ = 0;
	      *bptr = tptr - buffer;
	      
	      /* write the reject frame */
	      UIP_LOG("Writing NAK frame \n");
	      ahdlc_tx(IPV6CP, buffer, (u16_t)(tptr - buffer));
	      ANNOTATE("- End NAK Write frame\n");
	      
      } else {
      }
#endif
         /*
          * If we get here then we are OK, lets send an ACK and tell the rest
          * of our modules our negotiated config.
          */
         ipv6cp_state |= IPV6CP_RX_UP;
         ipv6cp_state &= ~IPV6CP_TX_UP;//phlb force send request with ipv6cp task
         ANNOTATE("Send IPV6CP ACK!\n");
         bptr = buffer;
         *bptr++ = CONF_ACK;		/* Write Conf_ACK */
         bptr++;				/* Skip ID (send same one) */
         /*
          * Set stuff
          */
         ppp_flags |= tflag;
         ANNOTATE("SET- stuff -- are we up? c=%d dif=%d \n", count, (u16_t)(bptr-buffer));
	   
         /* write the ACK frame */
         ANNOTATE("Writing ACK frame \n");
         /* Send packet ahdlc_txz(procol,header,data,headerlen,datalen);	*/
         ahdlc_tx(IPV6CP, 0, buffer, 0, count /*bptr-buffer*/);
         ANNOTATE("- End ACK Write frame\n");
	   
         /* expire the timer to make things happen after a state change */
         TIMER_expire(); //timer_expire(); //modify phlb uncomment
	   
         /*			} */
    }
    break;
    
    //
  case CONF_ACK:			/* config Ack */
       ANNOTATE("CONF ACK\n");
       /*
        * Parse out the results
        *
        * Dump the ID and get the length.
        */
       /* dump the ID */
       bptr++;

       /* get the length */
       len = (*bptr++ << 8);
       len |= *bptr++;
   #if 0 //modify phlb 
       /* Parse ACK and set data */
       while(bptr < buffer + len) {
         switch(*bptr++) {
         case IPV6CP_IPADDRESS:
	   /* dump length */
	   bptr++;		
	   ((u8_t*)ipaddr)[0] = *bptr++;
	   ((u8_t*)ipaddr)[1] = *bptr++;
	   ((u8_t*)ipaddr)[2] = *bptr++;
	   ((u8_t*)ipaddr)[3] = *bptr++;
	   break;
         case IPV6CP_PRIMARY_DNS:
	   bptr++;
	   ((u8_t*)pri_dns_addr)[0] = *bptr++;
	   ((u8_t*)pri_dns_addr)[1] = *bptr++;
	   ((u8_t*)pri_dns_addr)[2] = *bptr++;
	   ((u8_t*)pri_dns_addr)[3] = *bptr++;
	   break;
         case IPV6CP_SECONDARY_DNS:
	   bptr++;
	   ((u8_t*)sec_dns_addr)[0] = *bptr++;
	   ((u8_t*)sec_dns_addr)[1] = *bptr++;
	   ((u8_t*)sec_dns_addr)[2] = *bptr++;
	   ((u8_t*)sec_dns_addr)[3] = *bptr++;
	   break;
         default:
	   UIP_LOG("IPV6CP CONFIG_ACK problem1\n");
         }
       }
   #endif
       ipv6cp_state |= IPV6CP_TX_UP;
       //ipv6cp_state &= ~IPV6CP_RX_UP;
       ANNOTATE("were up! \n");
       printip(pppif.ipaddr);
   #ifdef IPV6CP_GET_PRI_DNS
       printip(pri_dns_addr);
   #endif
   #ifdef IPV6CP_GET_SEC_DNS
       printip(sec_dns_addr);
   #endif
       ANNOTATE("\n");
		   
       /* expire the timer to make things happen after a state change */
       TIMER_expire();
    break;

    //
  case CONF_NAK:			/* Config Nack */
    UIP_LOG("CONF NAK\n");
    /* dump the ID */
    bptr++;
    /* get the length */
    len = (*bptr++ << 8);
    len |= *bptr++;

    /* Parse ACK and set data */
    while(bptr < buffer + len) {
      switch(*bptr++) {
#if 0
#warning option implementation example based on IPCP IPv4 address configuration.
               case IPV6CP_IPADDRESS:
	         /* dump length */
	         bptr++;
	         ((u8_t*)pppif.ipaddr.u8)[0] = *bptr++;
	         ((u8_t*)pppif.ipaddr.u8)[1] = *bptr++;
	         ((u8_t*)pppif.ipaddr.u8)[2] = *bptr++;
	         ((u8_t*)pppif.ipaddr.u8)[3] = *bptr++;
	         uip_fw_register( &pppif );
	         ANNOTATE("My PPP-ipno: (%d.%d.%d.%d)\n", ((u8_t*)pppif.ipaddr.u8)[0], ((u8_t*)pppif.ipaddr.u8)[1], ((u8_t*)pppif.ipaddr.u8)[2], ((u8_t*)pppif.ipaddr.u8)[3]); 
	         break;
#endif
         #ifdef IPV6CP_GET_PRI_DNS
               case IPV6CP_PRIMARY_DNS:
	         bptr++;
	         ((u8_t*)pri_dns_addr)[0] = *bptr++;
	         ((u8_t*)pri_dns_addr)[1] = *bptr++;
	         ((u8_t*)pri_dns_addr)[2] = *bptr++;
	         ((u8_t*)pri_dns_addr)[3] = *bptr++;
	         break;
         #endif
         #ifdef IPV6CP_GET_SEC_DNS
               case IPV6CP_SECONDARY_DNS:
	         bptr++;
	         ((u8_t*)sec_dns_addr)[0] = *bptr++;
	         ((u8_t*)sec_dns_addr)[1] = *bptr++;
	         ((u8_t*)sec_dns_addr)[2] = *bptr++;
	         ((u8_t*)sec_dns_addr)[3] = *bptr++;
	         break;
         #endif

            //
            default:
	            UIP_LOG("IPCP CONFIG_ACK problem 2\n");
          }
       }
       ppp_id++;
       printip(pppif.ipaddr);
   #ifdef IPV6CP_GET_PRI_DNS
       printip(pri_dns_addr);
   #endif
   #ifdef IPV6CP_GET_PRI_DNS
       printip(sec_dns_addr);
   #endif
       ANNOTATE("\n");
       /* expire the timer to make things happen after a state change */
       TIMER_expire();
       break;

     case CONF_REJ:			/* Config Reject */
       ANNOTATE("CONF REJ\n");
       /* Remove the offending options*/
       ppp_id++;
       /* dump the ID */
       bptr++;
       /* get the length */
       len = (*bptr++ << 8);
       len |= *bptr++;

       /* Parse ACK and set data */
       while(bptr < buffer + len) {
         switch(*bptr++) {
#if 0
#warning option implementation example based on IPCP IPv4 address configuration.
            case IPV6CP_IPADDRESS:
	         ipv6cp_state |= IPV6CP_IP_BIT;
	         bptr += 5;
	         break;
#endif

         #ifdef IPV6CP_GET_PRI_DNS
            case IPV6CP_PRIMARY_DNS:
	         ipv6cp_state |= IPV6CP_PRI_DNS_BIT;
	         bptr += 5;
	         break;
         #endif

         #ifdef IPV6CP_GET_PRI_DNS
             case IPV6CP_SECONDARY_DNS:
	         ipv6cp_state |= IPV6CP_SEC_DNS_BIT;
	         bptr += 5;
	         break;
         #endif

            default:
	         UIP_LOG("IPV6CP this shoudln't happen 3\n");
         }
    }
    /* expire the timer to make things happen after a state change */
    /*timer_expire(); */
    break;

    default:
      UIP_LOG("-Unknown 4\n");
  }
}
  
/*---------------------------------------------------------------------------*/
void
ipv6cp_task(u8_t *buffer)
{
  u8_t *bptr;
  u16_t	t;
  IPCPPKT *pkt;

  /* IPCP tx not up and hasn't timed out then lets see if we need to
     send a request */
  if(!(ipv6cp_state & IPV6CP_TX_UP) && !(ipv6cp_state & IPV6CP_TX_TIMEOUT)) {
    /* Check if we have a request pending */
    /*t=get_seconds()-ipv6cp_tx_time;*/
#if 1//0//phlb modify
    if(TIMER_timeout(IPV6CP_TIMEOUT)) {
#else
    if((clock_seconds() - prev_ipv6cp_seconds) > IPV6CP_TIMEOUT) {
      prev_ipv6cp_seconds = clock_seconds();
#endif

      /*
       * No pending request, lets build one
       */
      pkt=(IPCPPKT *)buffer;		
      
      /* Configure-Request only here, write id */
      pkt->code = CONF_REQ;
      pkt->id = ppp_id;
			
      bptr = (u8_t*)&pkt->data;       

      /*
       * Write options, we want IP address, and DNS addresses if set.
       */
			
      /* Write zeros for IP address the first time */
      *bptr++ = 0x01;//IPV6CP_IPADDRESS;
      *bptr++ = 0x0a;//10
      *bptr++ = ((u8_t*)uip_hostaddr.u8)[8];
      *bptr++ = ((u8_t*)uip_hostaddr.u8)[9];
      *bptr++ = ((u8_t*)uip_hostaddr.u8)[10];
      *bptr++ = ((u8_t*)uip_hostaddr.u8)[11];
      *bptr++ = ((u8_t*)uip_hostaddr.u8)[12];
      *bptr++ = ((u8_t*)uip_hostaddr.u8)[13];
      *bptr++ = ((u8_t*)uip_hostaddr.u8)[14];
      *bptr++ = ((u8_t*)uip_hostaddr.u8)[15];
      
#ifdef IPV6CP_GET_PRI_DNS
      if(!(ipv6cp_state & IPCP_PRI_DNS_BIT)) {
	/* Write zeros for IP address the first time */
	*bptr++ = IPV6CP_PRIMARY_DNS;
	*bptr++ = 0x6;
	*bptr++ = ((u8_t*)pri_dns_addr.u8)[0];
	*bptr++ = ((u8_t*)pri_dns_addr.u8)[1];
	*bptr++ = ((u8_t*)pri_dns_addr.u8)[2];
	*bptr++ = ((u8_t*)pri_dns_addr.u8)[3];
      }
#endif
#ifdef IPV6CP_GET_SEC_DNS
      if(!(ipv6cp_state & IPV6CP_SEC_DNS_BIT)) {
	/* Write zeros for IP address the first time */
	*bptr++ = IPV6CP_SECONDARY_DNS;
	*bptr++ = 0x6;
	*bptr++ = ((u8_t*)sec_dns_addr)[0];
	*bptr++ = ((u8_t*)sec_dns_addr)[1];
	*bptr++ = ((u8_t*)sec_dns_addr)[2];
	*bptr++ = ((u8_t*)sec_dns_addr)[3];
      }
#endif
      /* Write length */
      t = bptr - buffer;
      /* length here -  code and ID + */
      pkt->len = uip_htons(t);	
      
      ANNOTATE("\n**Sending IPV6CP Request packet\n");
      
      /* Send packet ahdlc_txz(procol,header,data,headerlen,datalen); */
      ahdlc_tx(IPV6CP, 0, buffer, 0, t);

      /* Set timer */
      /*ipv6cp_tx_time=get_seconds();*/
      TIMER_set();
      /* Inc retry */
      /*ipv6cp_retry++;*/
     // ppp_retry++;
      /*
       * Have we timed out? (combide the timers?)
       */
      if(ppp_retry > IPV6CP_RETRY_COUNT)
	      ipv6cp_state |= IPV6CP_TX_TIMEOUT;	
      }
  }
}
/*---------------------------------------------------------------------------*/ 
