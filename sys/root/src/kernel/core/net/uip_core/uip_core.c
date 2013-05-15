/*
The contents of this file are subject to the Mozilla Public License Version 1.1
(the "License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License for the
specific language governing rights and limitations under the License.

The Original Code is Lepton.

The Initial Developer of the Original Code is Philippe Le Boulanger.
Portions created by Philippe Le Boulanger are Copyright (C) 2011 <lepton.phlb@gmail.com>.
All Rights Reserved.

Contributor(s): Jean-Jacques Pitrolle <lepton.jjp@gmail.com>.

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
#include <stdlib.h>

#include "kernel/core/kernelconf.h"

#include "kernel/core//errno.h"
#include "kernel/core/types.h"
#include "kernel/core/interrupt.h"
#include "kernel/core/kernel.h"
#include "kernel/core/syscall.h"
#include "kernel/core/process.h"
#include "kernel/core/signal.h"
#include "kernel/core/statvfs.h"
#include "kernel/core/bin.h"
#include "kernel/core/rttimer.h"
#include "kernel/core/time.h"
#include "kernel/core/flock.h"
#include "kernel/core/libstd.h"
#include "kernel/core/ioctl.h"


#include "kernel/core/time.h"
#include "kernel/core/kernel_clock.h"



#if defined (__KERNEL_NET_IPSTACK)
   #include "kernel/core/net/uip_core/uip_slip.h"
   #include "kernel/core/net/uip_core/uip_sock.h"
#include "kernel/dev/arch/all/ppp/dev_ppp_uip/dev_ppp_uip.h"

#if USE_UIP_VER == 1000 
   #pragma message ("uip 1.0")
   #include "kernel/net/uip1.0/net/uip.h"
   #include "kernel/net/uip1.0/net/uip_arch.h"
#endif

#if USE_UIP_VER == 2500 
#pragma message ("uip 2.5")
#include "kernel/net/uip2.5/contiki-conf.h"
#include "kernel/net/uip2.5/net/uip.h"
#include "kernel/net/uip2.5/net/uip_arch.h"
#define __uip_sendpacket(__conn__) do { uip_conn = &uip_conns[__conn__]; \
                                uip_process(UIP_TIMER); } while (0)

#define __uip_makeconnection(__conn__) do { uip_conn = &uip_conns[__conn__]; \
                                uip_process(UIP_POLL_REQUEST); } while (0)
#if UIP_CONF_IPV6
#define uip_udp_sendpacket(__conn_udp_no__,__dest_addr__) do { \
      uip_udp_conn = &uip_udp_conns[__conn_udp_no__]; \
      uip_udp_conn->rport = (__dest_addr__).sin6_port;\
      uip_ipaddr_copy(&uip_udp_conn->ripaddr, (uip_ipaddr_t*)&(__dest_addr__).sin6_addr.s6_addr );\
      uip_process(UIP_UDP_TIMER);\
      uip_udp_conn->rport =0;\
   } while (0)
#else
#define __uip_udp_sendpacket(__conn_udp_no__,__dest_addr__) do { \
      uip_udp_conn = &uip_udp_conns[__conn_udp_no__]; \
      uip_udp_conn->rport = (__dest_addr__).sin_port;\
      uip_ipaddr_copy(&uip_udp_conn->ripaddr, (uip_ipaddr_t*)&(__dest_addr__).sin_addr.s_addr );\
      uip_process(UIP_UDP_TIMER);\
      uip_udp_conn->rport =0;\
   } while (0)
#endif
#endif
//
//

   #include "kernel/core/net/uip_core/uip_core.h"
   #include "kernel/core/net/uip_core/uip_socket.h"
   #if defined(USE_IF_ETHERNET)
//#include "kernel/net/uip0.9/net/uip_arp.h"
//#include "kernel/net/uip1.0/net/uip_arp.h"
      #if USE_UIP_VER == 1000 
      #pragma message ("uip 1.0")
      #include "kernel/net/uip1.0/net/uip_arp.h"
      #endif

      #if USE_UIP_VER == 2500 
         #pragma message ("uip 2.5")
         #include "kernel/net/uip2.5/net/uip_arp.h"
      #endif
   #endif
#endif


#include "kernel/fs/vfs/vfskernel.h"

/*============================================
| Global Declaration
==============================================*/

#define UIP_CORE_POLLING_PERIOD         100//ms 200 10
#define UIP_CORE_RETRY_LINK_PERIOD      50 //1 second
#define UIP_CORE_RETRY_LCP_ECHO_PERIOD  10 //1 second
#define UIP_CORE_STACK_SIZE  2048 //1024//1024
#define UIP_CORE_PRIORITY    10 //20//10 //140

//specific for network interface and ip stack
#if defined (__KERNEL_NET_IPSTACK)
_macro_stack_addr char uip_core_stack[UIP_CORE_STACK_SIZE];
#endif

kernel_pthread_t uip_core_thread;

typedef int (*_pf_uip_core_send_ip_packet)(desc_t desc, const char* buf, int size);
typedef _pf_uip_core_send_ip_packet _pf_uip_core_send_ip_packet_t;

typedef int (*_pf_uip_core_recv_ip_packet)(desc_t desc, char* buf, int size);
typedef _pf_uip_core_recv_ip_packet _pf_uip_core_recv_ip_packet_t;

typedef struct {
   _pf_uip_core_send_ip_packet_t _pf_uip_core_send_ip_packet;
   _pf_uip_core_recv_ip_packet_t _pf_uip_core_recv_ip_packet;
}_uip_core_ip_packet_op_t;


int _uip_core_send_frame(desc_t desc, const char* buf, int size);
int _uip_core_recv_frame(desc_t desc,char* buf, int size);

enum _uip_core_ip_packet_op_type_t{
#if defined(USE_IF_SLIP)
   NET_IP_PACKET_SLIP,
#endif
#if defined(USE_IF_ETHERNET)
   NET_IP_PACKET_ETHERNET,
#endif
#if defined(USE_IF_PPP)
   NET_IP_PACKET_PPP,
#endif
   NET_IP_PACKET_UNDEF
};

#if defined (__KERNEL_NET_IPSTACK)
_uip_core_ip_packet_op_t _uip_core_ip_packet_op_list[]={
#if defined(USE_IF_SLIP)
   {_slip_send_packet,_slip_recv_packet},
#endif
#if defined(USE_IF_ETHERNET)
   {_uip_core_send_frame,_uip_core_recv_frame},
#endif
#if defined(USE_IF_PPP)
   {_uip_core_send_frame,_uip_core_recv_frame},
#endif
   {(void*)0,(void*)0},
};
#endif

#if defined(USE_IF_SLIP)
const char g_uip_core_ip_packet_op_type=NET_IP_PACKET_SLIP;
#elif defined(USE_IF_ETHERNET)
const char g_uip_core_ip_packet_op_type=NET_IP_PACKET_ETHERNET;
#elif defined(USE_IF_PPP)
   const char g_uip_core_ip_packet_op_type=NET_IP_PACKET_PPP;
#endif

static _pf_uip_core_send_ip_packet_t g_pf_uip_core_send_ip_packet;
static _pf_uip_core_recv_ip_packet_t g_pf_uip_core_recv_ip_packet;

typedef struct uip_core_if_info_st{
  char* name;
  int   if_no;
  desc_t desc_r;
  desc_t desc_w;
}uip_core_if_info_t;
uip_core_if_info_t uip_core_if_list[IF_LIST_MAX]={
#if defined(USE_IF_ETHERNET)
  {
  .name="eth0",
  .if_no=0,
  .desc_r=INVALID_DESC,
  .desc_w=INVALID_DESC
  }
#elif defined(USE_IF_PPP)
  {
  "/dev/net/ppp",
  0,
  INVALID_DESC,
  INVALID_DESC
  }
#else
  {
  "/dev/net/loopback",
  0,
  INVALID_DESC,
  INVALID_DESC
  }
#endif
};
//#define USE_TCP_HIGHSPEED_ACK

#if defined (USE_TCP_HIGHSPEED_ACK)
   #define __uip_core_send_ip_packet(__uip_desc__,__uip_buf__,__uip_len__) _uip_split_output(__uip_desc__,__uip_buf__,__uip_len__)
#else
   #define __uip_core_send_ip_packet(__uip_desc__,__uip_buf__,__uip_len__) g_pf_uip_core_send_ip_packet(__uip_desc__,__uip_buf__,__uip_len__)
#endif

typedef struct uip_core_queue_header_st{
   uint8_t uip_flag;
   desc_t desc;
   int size;
}uip_core_queue_header_t;
#define UIPCORE_QUEUE_PCKT_MAX 4 
#define UIPCORE_QUEUE_SZ 2048 //(2^n)
UIP_RAM_REGION static unsigned char uip_core_queue[UIPCORE_QUEUE_SZ]={0};
static volatile unsigned int uip_core_queue_r=0;
static volatile unsigned int uip_core_queue_w=0;
static volatile unsigned int uip_core_queue_size=UIPCORE_QUEUE_SZ;
kernel_pthread_mutex_t uip_core_queue_mutex;
kernel_sem_t           uip_core_queue_sem;
/*============================================
| Implementation
==============================================*/

#if defined (__KERNEL_NET_IPSTACK) && defined (USE_UIP_CORE)
unsigned int uip_core_if_nametoindex(const char *ifname){
   int i;
   if(ifname)
     return 0;
   for(i=0;i<IF_LIST_MAX;i++)
     if(!strcmp(ifname,uip_core_if_list[i].name))
       return i;
   return 0;
}
desc_t uip_core_if_indextodesc(int ifindex, unsigned long oflag){
   if(ifindex>=IF_LIST_MAX)
     return INVALID_DESC;
   if(oflag&O_RDONLY)
     return uip_core_if_list[ifindex].desc_r;
   if(oflag&O_WRONLY)
     return uip_core_if_list[ifindex].desc_w;
   return INVALID_DESC;
}
/*-------------------------------------------
| Name:_kernel_recv_char
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
unsigned char _uip_core_recv_char(desc_t desc){
   unsigned char c;
   uchar8_t _kernel_int;
   while(ofile_lst[desc].pfsop->fdev.fdev_isset_read(desc)) {
      _kernel_int = __wait_io_int(ofile_lst[desc].owner_pthread_ptr_read);
      if (!__K_IS_IOINTR(_kernel_int))
         continue;
   }
   ofile_lst[desc].pfsop->fdev.fdev_read(desc,&c,1);
   return c;
}

/*-------------------------------------------
| Name:_uip_core_send_char
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void _uip_core_send_char(desc_t desc,unsigned char c){
   uchar8_t _kernel_int;
   int r=-1;
   ofile_lst[desc].pfsop->fdev.fdev_write(desc,&c,1);
   do {
      _kernel_int = __wait_io_int(ofile_lst[desc].owner_pthread_ptr_write);
      if (!__K_IS_IOINTR(_kernel_int))
         continue;
   } while(ofile_lst[desc].pfsop->fdev.fdev_isset_write(desc));
}

/*-------------------------------------------
| Name:_uip_core_recv_frame
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _uip_core_recv_frame(desc_t desc, char* buf, int size){
   uchar8_t _kernel_int;
   #if defined(USE_IF_ETHERNET)  
   while(ofile_lst[desc].pfsop->fdev.fdev_isset_read(desc)) {
         _kernel_int = __wait_io_int(ofile_lst[desc].owner_pthread_ptr_read);
      if (!__K_IS_IOINTR(_kernel_int))
         continue;
   }
   #endif
   return ofile_lst[desc].pfsop->fdev.fdev_read(desc,buf,size);
}

/*-------------------------------------------
| Name:_uip_core_send_frame
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _uip_core_send_frame(desc_t desc, const char* buf, int size){
   uchar8_t _kernel_int;
   int cb;
   cb=ofile_lst[desc].pfsop->fdev.fdev_write(desc,buf,size);
   #if defined(USE_IF_ETHERNET) 
   do {
         _kernel_int = __wait_io_int(ofile_lst[desc].owner_pthread_ptr_write);
      if (!__K_IS_IOINTR(_kernel_int))
         continue;
   } while(ofile_lst[desc].pfsop->fdev.fdev_isset_write(desc));
   #endif
   //to remove debug
   //usleep(/*5000*/2000);

   return cb;
}
/*--------------------------------------------
| Name:        _uip_split_output
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
#if defined (USE_TCP_HIGHSPEED_ACK)
static void _uip_split_output(desc_t desc, char* buf, int size){
   u16_t tcplen, len1, len2;
   u16_t _uip_len=size;

   struct uip_tcpip_hdr * pbuf = ((struct uip_tcpip_hdr *)&buf[UIP_LLH_LEN]);

   /* We only try to split maximum sized TCP segments. */
   if(pbuf->proto == UIP_PROTO_TCP &&
      _uip_len >= (UIP_BUFSIZE - UIP_LLH_LEN)/2) {

      tcplen = _uip_len - UIP_TCPIP_HLEN-14;
      /* Split the segment in two. If the original packet length was
         odd, we make the second packet one byte larger. */
      len1 = len2 = tcplen / 2;
      if(len1 + len2 < tcplen) {
         ++len2;
      }

      /* Create the first packet. This is done by altering the length
         field of the IP header and updating the checksums. */
      _uip_len = len1 + UIP_TCPIP_HLEN;
   #if UIP_CONF_IPV6
      /* For IPv6, the IP length field does not include the IPv6 IP header
         length. */
      pbuf->len[0] = ((_uip_len - UIP_IPH_LEN) >> 8);
      pbuf->len[1] = ((_uip_len - UIP_IPH_LEN) & 0xff);
   #else /* UIP_CONF_IPV6 */
      pbuf->len[0] = _uip_len >> 8;
      pbuf->len[1] = _uip_len & 0xff;
   #endif /* UIP_CONF_IPV6 */

      /* Recalculate the TCP checksum. */
      pbuf->tcpchksum = 0;
      pbuf->tcpchksum = ~(uip_tcpchksum());

   #if !UIP_CONF_IPV6
      /* Recalculate the IP checksum. */
      pbuf->ipchksum = 0;
      pbuf->ipchksum = ~(uip_ipchksum());
   #endif /* UIP_CONF_IPV6 */

      /* Transmit the first packet. */
      /*    uip_fw_output();*/
      //tcpip_output();
      g_pf_uip_core_send_ip_packet(desc,buf,_uip_len+14);
      //to remove: debug
      //usleep(3000);

      /* Now, create the second packet. To do this, it is not enough to
         just alter the length field, but we must also update the TCP
         sequence number and point the uip_appdata to a new place in
         memory. This place is detemined by the length of the first
         packet (len1). */
      _uip_len = len2 + UIP_TCPIP_HLEN;
   #if UIP_CONF_IPV6
      /* For IPv6, the IP length field does not include the IPv6 IP header
         length. */
      pbuf->len[0] = ((_uip_len - UIP_IPH_LEN) >> 8);
      pbuf->len[1] = ((_uip_len - UIP_IPH_LEN) & 0xff);
   #else /* UIP_CONF_IPV6 */
      pbuf->len[0] = _uip_len >> 8;
      pbuf->len[1] = _uip_len & 0xff;
   #endif /* UIP_CONF_IPV6 */

      /*    uip_appdata += len1;*/
      memcpy(uip_appdata, (u8_t *)uip_appdata + len1, len2);


      uip_add32(pbuf->seqno, len1);
      pbuf->seqno[0] = uip_acc32[0];
      pbuf->seqno[1] = uip_acc32[1];
      pbuf->seqno[2] = uip_acc32[2];
      pbuf->seqno[3] = uip_acc32[3];

      /* Recalculate the TCP checksum. */
      pbuf->tcpchksum = 0;
      pbuf->tcpchksum = ~(uip_tcpchksum());

   #if !UIP_CONF_IPV6
      /* Recalculate the IP checksum. */
      pbuf->ipchksum = 0;
      pbuf->ipchksum = ~(uip_ipchksum());
   #endif /* UIP_CONF_IPV6 */

      /* Transmit the second packet. */
      /*    uip_fw_output();*/
      //tcpip_output();
      g_pf_uip_core_send_ip_packet(desc,buf,_uip_len+14);
      //to remove: debug
      //usleep(3000);
   } else {
      /*    uip_fw_output();*/
      //tcpip_output();
      g_pf_uip_core_send_ip_packet(desc,buf,_uip_len);
   }

}
#endif
/*-------------------------------------------
| Name:_uip_core_recv
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
   #define __eth_hdr ((struct uip_eth_hdr *)&uip_buf[0])
static int _uip_core_recv(desc_t desc_r,desc_t desc_w){
   if(desc_r<0 || desc_w<0)
      return -1;

   uip_len = g_pf_uip_core_recv_ip_packet(desc_r,uip_buf, sizeof(uip_buf));
   if(uip_len > 0) {
   #if defined(USE_IF_ETHERNET)
      u16_t type = __eth_hdr->type;
      u16_t _TYPE = UIP_ETHTYPE_ARP;
         if(__eth_hdr->type == htons(UIP_ETHTYPE_IP)
            #if UIP_CONF_IPV6
            /*||*/ __eth_hdr->type == htons(UIP_ETHTYPE_IPV6)
            #endif
            ) {
            #if !UIP_CONF_IPV6
         uip_arp_ipin();
            #endif
         uip_process(UIP_DATA);
         /* If the above function invocation resulted in data that
         should be sent out on the network, the global variable
         uip_len is set to a value > 0. */
         if(uip_len > 0) {
               #if !UIP_CONF_IPV6
            uip_arp_out();
               #endif
               #if UIP_CONF_IPV6
               {
                  unsigned char eth_src_buf[6];
                  unsigned char eth_dst_buf[6];
                  memcpy(eth_dst_buf,uip_buf,6);
                  memcpy(eth_src_buf,uip_buf+6,6);
                  memcpy(uip_buf,eth_src_buf,6);//src->dst
                  memcpy(uip_buf+6,eth_dst_buf,6);//dst->src
               }
               __uip_core_send_ip_packet(desc_w,uip_buf,uip_len+UIP_LLH_LEN);//modif phlb uIP2.5 UIP_LLH_LEN must take in charge the link layer
               #else
            __uip_core_send_ip_packet(desc_w,uip_buf,uip_len);
               #endif
            uip_len=0;
         }

      } else if(__eth_hdr->type == htons(UIP_ETHTYPE_ARP)) {
            #if !UIP_CONF_IPV6
         uip_arp_arpin();
            #endif
         /* If the above function invocation resulted in data that
            should be sent out on the network, the global variable
            uip_len is set to a value > 0. */
         if(uip_len > 0) {
            __uip_core_send_ip_packet(desc_w,uip_buf,uip_len);
            uip_len=0;
         }
      }
      #elif defined(USE_IF_SLIP) || defined(USE_IF_PPP)     
      uip_process(UIP_DATA);
      /* If the above function invocation resulted in data that
      should be sent out on the network, the global variable
      uip_len is set to a value > 0. */
      if(uip_len > 0) {
         __uip_core_send_ip_packet(desc_w,uip_buf,uip_len);
         uip_len=0;
      }
   #endif

   }

   return 0;
}

/*-------------------------------------------
| Name:_uip_core_send
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _uip_core_make_connection(desc_t desc_w,desc_t desc_sock, int conn_no){

   hsock_t hsock  = 0;

   if(desc_w<0)
      return -1;
   if(desc_sock<0)
      return 0;
   if(!(hsock=ofile_lst[desc_sock].p))
      return -1;
   //
   uip_len=0;
   //
   if(((socket_t*)hsock)->protocol!=IPPROTO_TCP)
      return -1; //invalid protocol
   //
   __uip_makeconnection(conn_no);
   //
   __uip_core_send_ip_packet(desc_w,uip_buf,uip_len);

   uip_len=0;

   return 0;
}

/*-------------------------------------------
| Name:_uip_core_send
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _uip_core_send(desc_t desc_w,desc_t desc_sock, int conn_no){
   hsock_t hsock  = 0;

   if(desc_w<0)
      return -1;
   if(desc_sock<0)
      return 0;
   if(!(hsock=ofile_lst[desc_sock].p))
      return -1;
   //
   uip_len=0;

   //
   if(((socket_t*)hsock)->protocol==IPPROTO_TCP){
      __uip_sendpacket(conn_no);
   }else  if(((socket_t*)hsock)->protocol==IPPROTO_UDP){
      __uip_udp_sendpacket(conn_no,((socket_t*)hsock)->addr_in_to);
   }else 
      return -1;
   if(uip_len > 0) {
   #if defined(USE_IF_ETHERNET)
         #if !UIP_CONF_IPV6
      uip_arp_out();
   #endif
   #endif
      __uip_core_send_ip_packet(desc_w,uip_buf,uip_len);
      uip_len=0;
   }

   return 0;
}

/*-------------------------------------------
| Name:_uip_core_poll
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _uip_core_poll(desc_t desc_w){
   int conn_no;
   if(desc_w<0)
      return -1;
   uip_len=0;
   for(conn_no = 0; conn_no<UIP_CONNS; ++conn_no) {
      uip_periodic(conn_no);
      //ethernet
      if(uip_len > 0) {
   #if defined(USE_IF_ETHERNET)
         #if !UIP_CONF_IPV6
         uip_arp_out();
         #endif
   #endif
   #if UIP_LOGGING==1
         printf("send by poll\r\n");
   #endif
         __uip_core_send_ip_packet(desc_w,uip_buf,uip_len);
         uip_len=0;
      }
   }

   return 0;
}

/*-------------------------------------------
| Name:_uip_core_syscall
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/



/*-------------------------------------------
| Name:_uip_core_slip_sync
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _uip_core_slip_sync(desc_t desc_r,desc_t desc_w)
{
   char buffer[]={"CLIENTSERVER\0"};
   char rcv_buffer[25]={0};
   unsigned char c=0;
   int i =0;

   while((c=_uip_core_recv_char(desc_r))!=192) {
      rcv_buffer[i]=c;
      i++;

      if(!strcmp("CLIENTCLIENTCLIENT",rcv_buffer)) {
         i=0;
         while(buffer[i]) {
            _uip_core_send_char(desc_w,buffer[i]);
            i++;
         }
         i=0;
         memset(rcv_buffer,0,14);
      }
   }
   return 0;
}

int uip_core_queue_put(uint8_t uip_flag, desc_t desc, void* buf, int size){
   int i;
   uip_core_queue_header_t uip_core_queue_header={0};
   unsigned char * p = (unsigned char*)&uip_core_queue_header;
   if(desc<0)
      return -1;
   uip_core_queue_header.uip_flag=uip_flag;
   uip_core_queue_header.desc=desc;
   uip_core_queue_header.size=size;
   kernel_sem_wait(&uip_core_queue_sem);
   kernel_pthread_mutex_lock(&uip_core_queue_mutex);
   if((uip_core_queue_header.size+sizeof(uip_core_queue_header_t))>=uip_core_queue_size){//is full ?
      kernel_pthread_mutex_unlock(&uip_core_queue_mutex);
      return 0;
   }
   for(i=0;i<sizeof(uip_core_queue_header_t);i++){
      uip_core_queue[uip_core_queue_w++]=*p++;
      if(uip_core_queue_w>=UIPCORE_QUEUE_SZ)
         uip_core_queue_w=0;
   }
   if( uip_core_queue_w+uip_core_queue_header.size<UIPCORE_QUEUE_SZ){
      if(buf)
         memcpy(&uip_core_queue[uip_core_queue_w],buf,uip_core_queue_header.size);
      uip_core_queue_w+=uip_core_queue_header.size;
   }else{
      int l=(UIPCORE_QUEUE_SZ-uip_core_queue_w);
      if(buf)
         memcpy(&uip_core_queue[uip_core_queue_w],buf,l);
      uip_core_queue_w=uip_core_queue_header.size-(UIPCORE_QUEUE_SZ-uip_core_queue_w);
      if(buf)
         memcpy(&uip_core_queue[0],(char*)buf+l,uip_core_queue_w);  
   }
   uip_core_queue_size-=(uip_core_queue_header.size+sizeof(uip_core_queue_header_t));
   kernel_pthread_mutex_unlock(&uip_core_queue_mutex);
   __fire_io_int(((kernel_pthread_t*)&uip_core_thread));
   return uip_core_queue_header.size;
}
int uip_core_queue_get(uint8_t* uip_flag, desc_t* desc, void* buf, int size){
   int i;
   uip_core_queue_header_t uip_core_queue_header={0};
   unsigned char * p = (unsigned char*)&uip_core_queue_header;
   if(!desc)
      return -1;
   kernel_pthread_mutex_lock(&uip_core_queue_mutex);
   *desc=-1;
   if(uip_core_queue_size==UIPCORE_QUEUE_SZ){//is empty ?
      kernel_pthread_mutex_unlock(&uip_core_queue_mutex);
      return 0;
   }
   for(i=0;i<sizeof(uip_core_queue_header_t);i++){
      *p++=uip_core_queue[uip_core_queue_r++];
      if(uip_core_queue_r>=UIPCORE_QUEUE_SZ)
         uip_core_queue_r=0;
   }
   *desc = uip_core_queue_header.desc;
   if( uip_core_queue_r+uip_core_queue_header.size<UIPCORE_QUEUE_SZ){
      if(buf)
         memcpy(buf,&uip_core_queue[uip_core_queue_r],uip_core_queue_header.size);
      uip_core_queue_r+=uip_core_queue_header.size;
   }else{
      int l=(UIPCORE_QUEUE_SZ-uip_core_queue_r);
      if(buf)
       memcpy(buf,&uip_core_queue[uip_core_queue_r],l);
      uip_core_queue_r=uip_core_queue_header.size-(UIPCORE_QUEUE_SZ-uip_core_queue_r);
      if(buf)
       memcpy((char*)buf+l,&uip_core_queue[0],uip_core_queue_r);  
   }
   uip_core_queue_size+=(uip_core_queue_header.size+sizeof(uip_core_queue_header_t));
   kernel_pthread_mutex_unlock(&uip_core_queue_mutex);
   kernel_sem_post(&uip_core_queue_sem);
   return uip_core_queue_header.size;
}
int uip_core_queue_init(void){
   pthread_mutexattr_t  mutex_attr=0;
   uip_core_queue_r = 0;
   uip_core_queue_w = 0;
   if(kernel_pthread_mutex_init(&uip_core_queue_mutex,&mutex_attr)<0)
      return -1;
   if(kernel_sem_init(&uip_core_queue_sem,0,1)<0)
      return -1;
   return 0;
}
int uip_core_queue_flush(void){
  kernel_pthread_mutex_lock(&uip_core_queue_mutex);
  uip_core_queue_r = 0;
  uip_core_queue_w = 0;
  kernel_pthread_mutex_unlock(&uip_core_queue_mutex);
  return 0;
}

/*-------------------------------------------
| Name:uip_core_routine
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
#if 0
int  _poll_event=0;
void* uip_core_routine(void* arg){

   unsigned char _uip_arp_counter=0;
   unsigned int _uip_poll_counter=0;
   unsigned int _uip_poll_arp_counter=0;
   unsigned long _uip_ppp_counter=0;
   unsigned long _uip_lcp_echo_counter=0;
   iface_ppp_stat_t ppp_stat={0};
   kernel_pthread_t* pthread_ptr;

   #if defined(USE_IF_SLIP)
   desc_t desc_r = _vfs_open("/dev/ttys1",O_RDONLY,0);
   desc_t desc_w = _vfs_open("/dev/ttys1",O_WRONLY,0);
   #elif defined(USE_IF_ETHERNET)
   desc_t desc_r = _vfs_open("/dev/eth0",O_RDONLY,0);
   desc_t desc_w = _vfs_open("/dev/eth0",O_WRONLY,0);
   #elif defined(USE_IF_PPP)
      desc_t desc_r = _vfs_open("/dev/net/ppp",O_RDONLY,0);
      desc_t desc_w = _vfs_open("/dev/net/ppp",O_WRONLY,0);
      desc_t desc_ttys_r = _vfs_open("/dev/ttys0",O_RDONLY,0);
      desc_t desc_ttys_w = _vfs_open("/dev/ttys0",O_WRONLY,0);
      _vfs_ioctl(desc_r,I_LINK,desc_ttys_r);
      _vfs_ioctl(desc_w,I_LINK,desc_ttys_w);
   #endif

   pthread_ptr = kernel_pthread_self();

   ofile_lst[desc_r].owner_pthread_ptr_read  = pthread_ptr;
   ofile_lst[desc_w].owner_pthread_ptr_write = pthread_ptr;

   #if defined(USE_IF_PPP)
   ofile_lst[desc_ttys_r].owner_pthread_ptr_read  = pthread_ptr;
   ofile_lst[desc_ttys_w].owner_pthread_ptr_write = pthread_ptr;
   #endif

   g_pf_uip_core_send_ip_packet = _uip_core_ip_packet_op_list[g_uip_core_ip_packet_op_type]._pf_uip_core_send_ip_packet;
   g_pf_uip_core_recv_ip_packet = _uip_core_ip_packet_op_list[g_uip_core_ip_packet_op_type]._pf_uip_core_recv_ip_packet;
   //slip synchro: only for chimbo windows
   //_uip_core_slip_sync(desc_r,desc_w);

   for(;; ) {
      int _flg_recv_packet;
      pid_t pid;

      pid=0;
      _flg_recv_packet=-1;

      if( !(_flg_recv_packet=ofile_lst[desc_r].pfsop->fdev.fdev_isset_read(desc_r))
         /*|| (pthread_ptr = _uip_core_syscall())*/ ){

         //rcv ip packet ok!
         if(!_flg_recv_packet) {
            _uip_core_recv(desc_r,desc_w);
         }

         if(pid) { //syscall for ip stack operation
            desc_t desc_sock;
            netsend_t* netsend_dt;
            int conn_no;

            if(pthread_ptr->reg.syscall!=_SYSCALL_NET_SND) {
               __flush_syscall(pthread_ptr);
               __kernel_ret_int(pthread_ptr);
               __syscall_unlock();
               continue;
            }

            netsend_dt = (netsend_t*)pthread_ptr->reg.data;
            desc_sock = netsend_dt->desc;

            //to do:lepton: find connection
            conn_no = socksconn_no(desc_sock);

            __flush_syscall(pthread_ptr);
            __kernel_ret_int(pthread_ptr);
            __syscall_unlock();

            //
            if(conn_no>=0) {
   #if UIP_LOGGING==1
               printf("send by event\r\n");
   #endif
               _uip_core_send(desc_w,conn_no);
            }
         }

      }else{
         //to remove: test
         //OS_Delay(5);
         unsigned int timeout = UIP_CORE_POLLING_PERIOD;

         struct timespec abs_timeout;

         _uip_ppp_counter=0;
         _uip_lcp_echo_counter=0;
         abs_timeout.tv_sec   = (timeout/1000);
         abs_timeout.tv_nsec  = (timeout%1000)*1000000; //ms->ns


         while((_poll_event=__wait_io_int2(pthread_ptr,&abs_timeout/*200*/))<0){//5//10ms
            //
            if( !(_uip_poll_counter=(_uip_poll_counter+1)%5 /*%20*/) ) {
               _uip_poll_counter=0;
               _uip_core_poll(desc_w);
            }

            if( !(_uip_poll_arp_counter=(++_uip_poll_arp_counter)%5) ) {
               #if !UIP_CONF_IPV6
               uip_arp_timer();
               #endif
               // break;
            }
            #if defined(USE_IF_PPP)
            if(_vfs_ioctl(desc_w,PPPSTAT,&ppp_stat)>=0){
               if(ppp_stat.is_up==PPP_DOWN && !((++_uip_ppp_counter)%UIP_CORE_RETRY_LINK_PERIOD/*20*/)){
                  _uip_ppp_counter=0;
                  _vfs_ioctl(desc_w,PPPUP);
         }
               if(ppp_stat.is_up==PPP_UP && !((++_uip_lcp_echo_counter)%UIP_CORE_RETRY_LCP_ECHO_PERIOD)){
                  _uip_lcp_echo_counter=0;
                  _vfs_ioctl(desc_w,PPPECHO);
               }
            }
            #endif
        }
      }
   }

}
#endif

void* uip_core_routine(void* arg){
   unsigned int _uip_core_stop=0;
   int  _uip_poll_event=0;
   unsigned int _uip_poll_counter=0;
   unsigned int _uip_poll_arp_counter=0;
   unsigned long _uip_ppp_counter=0;
   unsigned long _uip_lcp_echo_counter=0;
   iface_ppp_stat_t ppp_stat={0};
   kernel_pthread_t* pthread_ptr;
   #if defined(USE_IF_SLIP)
      desc_t desc_r = _vfs_open(IFACE_COM_NAME,O_RDONLY,0);
      desc_t desc_w = _vfs_open(IFACE_COM_NAME,O_WRONLY,0);
      if(desc_r<0 || desc_w<0)
         return (void*0);//uip core panic!!!
   #elif defined(USE_IF_ETHERNET)
      desc_t desc_r = _vfs_open(uip_core_if_list[0].name,O_RDONLY,0);
      desc_t desc_w = _vfs_open(uip_core_if_list[0].name,O_WRONLY,0);
      if(desc_r<0 || desc_w<0)
         return (void*0);//uip core panic!!!
   #elif defined(USE_IF_PPP)
      desc_t desc_r = _vfs_open(uip_core_if_list[0].name,O_RDONLY,0);
      desc_t desc_w = _vfs_open(uip_core_if_list[0].name,O_WRONLY,0);
      #if __tauon_compiler_cpu_target__ == __compiler_cpu_target_win32__
      desc_t desc_ttys_r = _vfs_open("/dev/ttys1",O_RDONLY,0);
      desc_t desc_ttys_w = _vfs_open("/dev/ttys1",O_WRONLY,0);
      #else
      desc_t desc_ttys_r = _vfs_open("/dev/ttys0",O_RDONLY,0);
      desc_t desc_ttys_w = _vfs_open("/dev/ttys0",O_WRONLY,0);
      #endif
      if(   desc_r<0 || desc_w<0 
         || desc_ttys_r<0 || desc_ttys_w<0)
         return (void*)0;//uip core panic!!!
      _vfs_ioctl(desc_r,I_LINK,desc_ttys_r);
      _vfs_ioctl(desc_w,I_LINK,desc_ttys_w);
   #endif
   uip_core_if_list[0].desc_r = desc_r;
   uip_core_if_list[0].desc_w = desc_w;
   if(!(pthread_ptr = kernel_pthread_self()))
      return (void*)0;//uip core panic!!!
   ofile_lst[desc_r].owner_pthread_ptr_read  = pthread_ptr;
   ofile_lst[desc_w].owner_pthread_ptr_write = pthread_ptr;
   #if defined(USE_IF_PPP)
   ofile_lst[desc_ttys_r].owner_pthread_ptr_read  = pthread_ptr;
   ofile_lst[desc_ttys_w].owner_pthread_ptr_write = pthread_ptr;
   #endif
   g_pf_uip_core_send_ip_packet = _uip_core_ip_packet_op_list[g_uip_core_ip_packet_op_type]._pf_uip_core_send_ip_packet;
   g_pf_uip_core_recv_ip_packet = _uip_core_ip_packet_op_list[g_uip_core_ip_packet_op_type]._pf_uip_core_recv_ip_packet;
   #ifdef USE_PPP_MS_WINDOWS
   _uip_core_ms_windows_sync(desc_ttys_r,desc_ttys_w);
   #endif
   while(!_uip_core_stop){
     
      int _flg_recv_data=0;
      uint8_t uip_flag=0;
      desc_t desc_sock=-1;
      int msg_size=0;
      int conn_no=-1;
      _flg_recv_data=-1;
      desc_sock=-1;
      msg_size=0;
      conn_no=-1;
      if(!(_flg_recv_data=ofile_lst[desc_r].pfsop->fdev.fdev_isset_read(desc_r)) ){
         if(!_flg_recv_data){//processin incomming data
            _uip_core_recv(desc_r,desc_w);
         }
      }
      if( !(ofile_lst[desc_w].pfsop->fdev.fdev_isset_write(desc_w)) && (msg_size=uip_core_queue_get(&uip_flag,&desc_sock,(void*)0,0)) ){
        if(!msg_size)
            continue;
         if(desc_sock<0)
            continue;
         conn_no = socksconn_no(desc_sock);
         if(conn_no>=0){
            if(uip_flag=UIP_POLL_REQUEST){
               _uip_core_make_connection(desc_w,desc_sock,conn_no);
            }else{
               _uip_core_send(desc_w,desc_sock,conn_no);
            }
         }
      }
      if(!msg_size && _flg_recv_data<0)
      {
         unsigned int timeout = UIP_CORE_POLLING_PERIOD;
         struct timespec abs_timeout;
         abs_timeout.tv_sec   = (timeout/1000);
         abs_timeout.tv_nsec  = (timeout%1000)*1000000;//ms->ns
         _uip_ppp_counter=0;
         _uip_lcp_echo_counter=0;
         while((_uip_poll_event=__wait_io_int2(pthread_ptr,(const struct timespec*)&abs_timeout/*200*/))<0){//5//10ms
            if( !(_uip_poll_counter=(_uip_poll_counter+1)%5/*%20*/) ){
               _uip_poll_counter=0;
            }
            if( !((++_uip_poll_arp_counter)%5) ){
               #if !UIP_CONF_IPV6
               uip_arp_timer();
               #endif
            }
            #if defined(USE_IF_PPP)
            if(_vfs_ioctl(desc_w,PPPSTAT,&ppp_stat)>=0){
               if(ppp_stat.is_up==PPP_DOWN && !((++_uip_ppp_counter)%UIP_CORE_RETRY_LINK_PERIOD/*20*/)){
                  _uip_ppp_counter=0;
                  _vfs_ioctl(desc_w,PPPUP);
               }
               if(ppp_stat.is_up==PPP_UP && !((++_uip_lcp_echo_counter)%UIP_CORE_RETRY_LCP_ECHO_PERIOD)){
                  _uip_lcp_echo_counter=0;
                  //_vfs_ioctl(desc_w,PPPECHO);
               }
               if(ppp_stat.is_up==PPP_SHUTDOWN){
                 _vfs_ioctl(desc_w,PPPDWN);
                 _uip_core_stop=1;
               }
            }
            #endif
        }
      }
   } //while(_uip_core_stop)
   #if defined(USE_IF_SLIP)
      _vfs_close(desc_r);
      _vfs_close(desc_w);
   #elif defined(USE_IF_ETHERNET)
      _vfs_close(desc_r);
      _vfs_close(desc_w);
   #elif defined(USE_IF_PPP)
      _vfs_close(desc_ttys_r);
      _vfs_close(desc_ttys_w);
      _vfs_close(desc_r);
      _vfs_close(desc_w);
   #endif
      uip_core_if_list[0].desc_r = INVALID_DESC;
      uip_core_if_list[0].desc_w = INVALID_DESC;
      uip_core_queue_flush();
      return 0;
}
/*--------------------------------------------
| Name:        uip_core_run
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int uip_core_run(void){

   pthread_attr_t       thread_attr={0};

   thread_attr.stacksize = UIP_CORE_STACK_SIZE;
   thread_attr.stackaddr = (void*)&uip_core_stack;
   thread_attr.priority  = UIP_CORE_PRIORITY;
   thread_attr.timeslice = 0;
   thread_attr.name = "kernel_pthread_uip";

   uip_core_queue_init();
   kernel_pthread_create(&uip_core_thread,&thread_attr,uip_core_routine,(char*)0);

   return 0;
}

#endif

/*============================================
| End of Source  : uip_core.c
==============================================*/
