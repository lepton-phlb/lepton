/*
The contents of this file are subject to the Mozilla Public License Version 1.1
(the "License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License for the
specific language governing rights and limitations under the License.

The Original Code is Lepton.

The Initial Developer of the Original Code is Chauvin-Arnoux.
Portions created by Chauvin-Arnoux are Copyright (C) 2011. All Rights Reserved.

Alternatively, the contents of this file may be used under the terms of the eCos GPL license
(the  [eCos GPL] License), in which case the provisions of [eCos GPL] License are applicable
instead of those above. If you wish to allow use of your version of this file only under the
terms of the [eCos GPL] License and not to allow others to use your version of this file under
the MPL, indicate your decision by deleting  the provisions above and replace
them with the notice and other provisions required by the [eCos GPL] License.
If you do not delete the provisions above, a recipient may use your version of this file under
either the MPL or the [eCos GPL] License."
*/

/*============================================
| Includes
==============================================*/

#include "lwip/debug.h"

#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/stats.h"
#include "lwip/sys.h"
#include "lwip/ip.h"

#include "netif/etharp.h"

#include "kernel/core/types.h"
#include "kernel/core/kernel.h"
#include "kernel/core/system.h"
#include "kernel/fs/vfs/vfskernel.h"
#include "kernel/fs/vfs/vfs.h"
#include "kernel/core/ioctl_eth.h"
#include "kernel/core/ioctl.h"

#include "kernel/core/time.h"
#include "kernel/core/kernel_clock.h"

#include "kernel/core/net/lwip_core/lwip_core.h"


/*============================================
| Global Declaration
==============================================*/
#undef NETIF_DEBUG

/* Define those to better describe your network interface. */
#define IFNAME0 'p'
#define IFNAME1 'k'

static volatile unsigned char if_broadcast_no=0;
static volatile unsigned char if_pointtopoint_no=0;

#define ETHIF_CORE_INPUT_DELAY   100   //ms
#define ETHIF_CORE_OUTPUT_DELAY  1000  //ms

#define ETHIF_RESET_TIMEOUT 10 //timeout in second for reset ethernet interface

//
/*
struct ethernetif {
        struct eth_addr *ethaddr;
   desc_t desc_r;
   desc_t desc_w;
   kernel_pthread_mutex_t     mutex_lowlevel_output;
   int timeout_error;
};
*/

//
//static const struct eth_addr ethbroadcast = {{0xff,0xff,0xff,0xff,0xff,0xff}};
extern const struct eth_addr ethbroadcast;

static int  ethif_core_input(struct netif *netif);
static err_t ethif_core_output(struct netif *netif, struct pbuf *p,struct ip_addr *ipaddr);

//to do: remove this ugly code quickly!
#if defined(CPU_WIN32) || defined(CPU_GNU32)
extern unsigned char lwip_ethaddr[6];
#endif

extern unsigned char *cur_packet;

extern int packet_send(void *buffer, int len);

/*============================================
| Implementation
==============================================*/


/*--------------------------------------------
| Name:        low_level_init
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
static void low_level_init(struct netif *netif){
   kernel_pthread_t* pthread_ptr;
   pthread_mutexattr_t mutex_attr=0;

   struct lwip_if_st *p_lwip_if=(struct lwip_if_st *)netif->state;

   //to do: to fix.
#if defined(CPU_WIN32)
   //win32
   memcpy(&lwip_ethaddr,p_lwip_if->ethaddr,6);
#elif defined(CPU_GNU32)
   memcpy(p_lwip_if->ethaddr,lwip_ethaddr,6);
#else
   {
      unsigned char lowlevel_ethaddr[6]={0};
      //arm7
      //memcpy(p_lwip_if->ethaddr,&lwip_ethaddr,6);
      //get hardware MAC address from ethernet interface.
      __atomic_in(); //critical section in. begin of parano�ac protection.
      //_vfs_ioctl(p_lwip_if->desc_r,ETHGETHWADDRESS,p_lwip_if->ethaddr);
      _vfs_ioctl(p_lwip_if->desc_r,ETHGETHWADDRESS,(unsigned char*)&lowlevel_ethaddr[0]);
      memcpy(p_lwip_if->ethaddr,&lowlevel_ethaddr[0],6);

      __atomic_out(); //critical section out. end of parano�ac protection.
   }
#endif

#ifdef NETIF_DEBUG
   LWIP_DEBUGF(NETIF_DEBUG, ("pktif: eth_addr %02X%02X%02X%02X%02X%02X\n",
                             p_lwip_if->ethaddr->addr[0],
                             p_lwip_if->ethaddr->addr[1],
                             p_lwip_if->ethaddr->addr[2],
                             p_lwip_if->ethaddr->addr[3],
                             p_lwip_if->ethaddr->addr[4],
                             p_lwip_if->ethaddr->addr[5]));
#endif    /* NETIF_DEBUG */
          /* Do whatever else is needed to initialize interface. */

   pthread_ptr = kernel_pthread_self();

   ofile_lst[p_lwip_if->desc_r].owner_pthread_ptr_read  = pthread_ptr;
   ofile_lst[p_lwip_if->desc_w].owner_pthread_ptr_write = pthread_ptr;

   ofile_lst[p_lwip_if->desc_r].nb_reader++;
   ofile_lst[p_lwip_if->desc_w].nb_writer++;


   kernel_pthread_mutex_init(&p_lwip_if->mutex_lowlevel_output,&mutex_attr);

   p_lwip_if->timeout_error=0;
}

/*--------------------------------------------
| Name:        low_level_ioctl
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int low_level_ioctl(desc_t desc, int request, ... ){
   va_list ap;
   int ret;
   va_start(ap, request);
   ret = ofile_lst[desc].pfsop->fdev.fdev_ioctl(desc,request,ap);
   va_end(ap);
   return ret;
}


/*--------------------------------------------
| Name:        low_level_output
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| Should do the actual transmission of the packet. The packet is
| contained in the pbuf that is passed to the function. This pbuf
| might be chained.
| See:
----------------------------------------------*/
static err_t low_level_output(struct netif *netif, struct pbuf *p){
   struct pbuf *q;
   //gloups bug :'(
   static unsigned char buffer[1600];
   unsigned char *ptr;
   int packet_len=0;

   kernel_pthread_t* pthread_ptr;

   struct lwip_if_st *p_lwip_if=(struct lwip_if_st *)netif->state;
   desc_t desc= p_lwip_if->desc_w;

   pthread_ptr = kernel_pthread_self();

   /* initiate transfer(); */
   if (p->tot_len>=1600)
      return ERR_BUF;

   //lock
   kernel_pthread_mutex_lock(&p_lwip_if->mutex_lowlevel_output);
   //bug fix:lwip 1.3.0  protect static buffer: must be static else pthread stack crash

   //set pthread owner, it can be tcpip_thread() or lwip_core_routine();
   ofile_lst[desc].owner_pthread_ptr_write = pthread_ptr;

   //
   for(q = p; q != NULL; q = q->next) {
      int eth_timeout_s=ETHIF_RESET_TIMEOUT;
      /* Send the data from the pbuf to the interface, one pbuf at a
         time. The size of the data in each pbuf is kept in the ->len
         variable. */
      /* send data from(q->payload, q->len); */
#ifdef NETIF_DEBUG
		   LWIP_DEBUGF(NETIF_DEBUG, ("netif: send ptr %p q->payload %p q->len %i q->next %p\n", ptr, q->payload, (int)q->len, q->next));
#endif

      //profiler
      __io_profiler_start(desc);
      //init new current packet
      if(!packet_len) {
         ptr=buffer;
         packet_len=p->tot_len;
      }
      //
      memcpy(ptr,q->payload,q->len);
      packet_len-=q->len;
      //next part of current packet;
      ptr+=q->len;
      //
      if(!packet_len) { //all packet is copied in buffer. now send current packet.

         /* signal that packet should be sent(); */
         if (ofile_lst[desc].pfsop->fdev.fdev_write(desc,buffer,p->tot_len) < 0) {
            //unlock
            kernel_pthread_mutex_unlock(&p_lwip_if->mutex_lowlevel_output);
            return ERR_BUF;
         }

         //
         while(ofile_lst[desc].pfsop->fdev.fdev_isset_write
               && ofile_lst[desc].pfsop->fdev.fdev_isset_write(desc)) {
            //
            struct timespec abs_timeout;

            abs_timeout.tv_sec   = (ETHIF_CORE_OUTPUT_DELAY/1000);
            abs_timeout.tv_nsec  = (ETHIF_CORE_OUTPUT_DELAY%1000)*1000000; //ms->ns

            //wait all data are transmitted
            if((__wait_io_int2(pthread_ptr,&abs_timeout))<0) {
               if(!(--eth_timeout_s)) //timeout system
                  break;
            }

         }
         //
         if(!eth_timeout_s) {
            p_lwip_if->timeout_error++;
            //ofile_lst[desc].pfsop->fdev.fdev_ioctl(
            //to do reset interface
            low_level_ioctl(desc,ETHRESET,0);
         }else{
            //profiler
            __io_profiler_stop(desc);
            __io_profiler_add_result(desc,O_WRONLY,p->tot_len,__io_profiler_get_counter(desc));
         }
         //end if(!packet_len)
      }

   }

#ifdef LINK_STATS
   lwip_stats.link.xmit++;
#endif    /* LINK_STATS */

   //unlock
   kernel_pthread_mutex_unlock(&p_lwip_if->mutex_lowlevel_output);
   //data sent

   return ERR_OK;
}

/*--------------------------------------------
| Name:        low_level_input
| Description:
| Parameters:  none
| Return Type: none
| Comments: Should allocate a pbuf and transfer the bytes of the incoming
| packet from the interface into the pbuf.
| See:
----------------------------------------------*/
static struct pbuf *low_level_input(struct netif *netif){
   struct pbuf *p, *q;
   int start, length;
   unsigned char buffer[1600];

   struct lwip_if_st *p_lwip_if=(struct lwip_if_st *)netif->state;
   desc_t desc= p_lwip_if->desc_r;


   /* Obtain the size of the packet and put it into the "len"
      variable. */
   length = ofile_lst[desc].pfsop->fdev.fdev_read(desc,buffer,sizeof(buffer));
   if (length<=0)
      return NULL;

   /* We allocate a pbuf chain of pbufs from the pool. */
   p = pbuf_alloc(PBUF_LINK, (u16_t)length, PBUF_POOL);
#ifdef NETIF_DEBUG
   LWIP_DEBUGF(NETIF_DEBUG, ("netif: recv length %i p->tot_len %i\n", length, (int)p->tot_len));
#endif

   if (p != NULL) {
      /* We iterate over the pbuf chain until we have read the entire
         packet into the pbuf. */
      start=0;
      for(q = p; q != NULL; q = q->next) {
         /* Read enough bytes to fill this pbuf in the chain. The
            available data in the pbuf is given by the q->len
            variable. */
         /* read data into(q->payload, q->len); */
#ifdef NETIF_DEBUG
	         LWIP_DEBUGF(NETIF_DEBUG, ("netif: recv start %i length %i q->payload %p q->len %i q->next %p\n", start, length, q->payload, (int)q->len, q->next));
#endif
         memcpy(q->payload,&buffer[start],q->len);
         start+=q->len;
         length-=q->len;
         if (length<=0)
            break;
      }
      /* acknowledge that packet has been read(); */
      //cur_length=0;
#ifdef LINK_STATS
      lwip_stats.link.recv++;
#endif       /* LINK_STATS */

      //profiler
      __io_profiler_stop(desc);
      __io_profiler_add_result(desc,O_RDONLY,start,__io_profiler_get_counter(desc));

   }else{
      /* drop packet(); */
      //cur_length=0;
#ifdef LINK_STATS
      lwip_stats.link.memerr++;
      lwip_stats.link.drop++;
#endif       /* LINK_STATS */
   }

   return p;
}

/*--------------------------------------------
| Name:        ethif_core_output
| Description:
| Parameters:  none
| Return Type: none
| Comments: This function is called by the TCP/IP stack when an IP packet
| should be sent. It calls the function called low_level_output() to
| do the actuall transmission of the packet.
|
| See:
----------------------------------------------*/
static err_t ethif_core_output(struct netif *netif, struct pbuf *p, struct ip_addr *ipaddr){
   if(netif->hwaddr_len) { //only for ethernet interface
      return etharp_output(netif, p,ipaddr);
   }else{ //only for slip interface
      return low_level_output(netif,p);
   }
}

/*--------------------------------------------
| Name:        ethif_core_input
| Description:
| Parameters:  none
| Return Type: none
| Comments: This function should be called when a packet is ready to be read
| from the interface. It uses the function low_level_input() that
| should handle the actual reception of bytes from the network
| interface.
|
| See:
----------------------------------------------*/
int ethif_core_input(struct netif *netif)
{
   struct eth_hdr *ethhdr;
   struct pbuf *p;
   struct lwip_if_st *p_lwip_if=(struct lwip_if_st *)netif->state;
   desc_t desc= p_lwip_if->desc_r;

   int r=-1;

   //
   if(desc<0)
      return -1;
   //
   if(!(r=ofile_lst[desc].pfsop->fdev.fdev_isset_read(desc))) {
      while( (p = low_level_input(netif))!=NULL){
         if (p !=NULL) {
            /* points to packet payload, which starts with an Ethernet header */
            ethhdr = p->payload;
            switch (htons(ethhdr->type)) {
            /* IP or ARP packet? */
            case ETHTYPE_IP:
            case ETHTYPE_ARP:
   #if PPPOE_SUPPORT
            /* PPPoE packet? */
            case ETHTYPE_PPPOEDISC:
            case ETHTYPE_PPPOE:
   #endif          /* PPPOE_SUPPORT */
                   /* full packet send to tcpip_thread to process */
               if (netif->input(p, netif)!=ERR_OK) {
                  LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
                  pbuf_free(p);
                  p = NULL;
               }
               break;
   
            default:
               pbuf_free(p);
               p = NULL;
               break;
            }
         }
      }//end of while
   }else{
      //profiler
      __io_profiler_start(desc);
   }

   return r;
}

int ethif_core_input_1_2_1(struct netif *netif)
{
   struct eth_hdr *ethhdr;
   struct pbuf *p;
   struct lwip_if_st *p_lwip_if=(struct lwip_if_st *)netif->state;
   desc_t desc= p_lwip_if->desc_r;

   int r;

   //
   if(desc<0)
      return -1;
   //
   if(!(r=ofile_lst[desc].pfsop->fdev.fdev_isset_read(desc))) {
      //
      p = low_level_input(netif);
      if (p != NULL) {

#ifdef LINK_STATS
         lwip_stats.link.recv++;
#endif          /* LINK_STATS */


         if(netif->hwaddr_len) { //only for ethernet interface
            int header_sz = sizeof(struct eth_hdr);

            ethhdr = p->payload;
            switch (htons(ethhdr->type)) {

            case ETHTYPE_IP:
               etharp_ip_input(netif, p);
               pbuf_header(p, -header_sz);   //-14
               netif->input(p, netif);
               break;

            case ETHTYPE_ARP:
               etharp_arp_input(netif, p_lwip_if->ethaddr, p);
               break;

            default:
               pbuf_free(p);
               break;
            } // switch
         }else{ //only for slip interface
            if (netif->input(p, netif) != ERR_OK) {
               pbuf_free(p);
               p = NULL;
            }
         }
      } // if p!=NULL
   } //while

   return r;
}

/*--------------------------------------------
| Name:        ethif_core_periodic_input
| Description:
| Parameters:  none
| Return Type: none
| Comments: Needs to be called periodically to get new packets.
|           This could be done inside a thread.
| See:
----------------------------------------------*/
int ethif_core_periodic_input(struct lwip_if_st *lwip_if_head){
   lwip_if_t* p_lwip_if=lwip_if_head;
   static kernel_pthread_t* ethif_core_pthread= (kernel_pthread_t*)0;
   int r=-1;

   if(!ethif_core_pthread) {
      ethif_core_pthread = kernel_pthread_self();
   }

   while(p_lwip_if) {
      //set interface parameter
      if(ethif_core_pthread!=ofile_lst[p_lwip_if->desc_r].owner_pthread_ptr_read) {
         ofile_lst[p_lwip_if->desc_r].owner_pthread_ptr_read = ethif_core_pthread;
      }
      if(ofile_lst[p_lwip_if->desc_r].nb_reader<=0) {
         //interface reset
         low_level_ioctl(p_lwip_if->desc_r,ETHRESET,0);
         ofile_lst[p_lwip_if->desc_r].nb_reader++;
      }
      r=ethif_core_input(&p_lwip_if->netif);
      //go to next interface
      p_lwip_if=p_lwip_if->lwip_if_next;
   }
   if(r<0) {
      struct timespec abs_timeout;

      abs_timeout.tv_sec   = (ETHIF_CORE_INPUT_DELAY/1000);
      abs_timeout.tv_nsec  = (ETHIF_CORE_INPUT_DELAY%1000)*1000000; //ms->ns

      __wait_io_int2(ethif_core_pthread,&abs_timeout); //100ms
   }
   //
   return 0;
}

/*--------------------------------------------
| Name:        arp_timer
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
static void arp_timer(void *arg){
   etharp_tmr();
   sys_timeout(ARP_TMR_INTERVAL, (sys_timeout_handler)arp_timer, NULL);
}

/*--------------------------------------------
| Name:        ethif_core__init
| Description:
| Parameters:  none
| Return Type: none
| Comments: Should be called at the beginning of the program to set up the
| network interface. It calls the function low_level_init() to do the
| actual setup of the hardware.
|
| See:
----------------------------------------------*/
err_t ethif_core_init(struct netif *netif){
   struct lwip_if_st *p_lwip_if=(struct lwip_if_st *)netif->state;

   if(p_lwip_if->if_config.if_flags&IFF_BROADCAST) {
      netif->name[0] = 'e';
      netif->name[1] = '0'+if_broadcast_no++;

      //ethernet connection type
      netif->mtu = 1500;
      netif->flags = NETIF_FLAG_BROADCAST| NETIF_FLAG_ETHARP | NETIF_FLAG_IGMP | NETIF_FLAG_LINK_UP;
      netif->hwaddr_len = ETHARP_HWADDR_LEN;

   }else if(p_lwip_if->if_config.if_flags&IFF_POINTTOPOINT) {
      netif->name[0] = 's';
      netif->name[1] = '0'+if_pointtopoint_no++;
      //slip connection type
      netif->mtu = 1500;
      netif->flags = NETIF_FLAG_POINTTOPOINT;
      netif->hwaddr_len=0;
   }
   netif->linkoutput = low_level_output;
   netif->output = ethif_core_output;

   p_lwip_if->ethaddr = (struct eth_addr *)&(netif->hwaddr[0]);

   low_level_init(netif);
   //etharp_init();

   sys_timeout(ARP_TMR_INTERVAL, (sys_timeout_handler)arp_timer, NULL);

   return ERR_OK;
}

/*============================================
| End of Source  : ethif_core.c
==============================================*/
