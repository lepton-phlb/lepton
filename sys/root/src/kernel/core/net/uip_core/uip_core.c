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


#include "kernel/core/time.h"
#include "kernel/core/kernel_clock.h"



#if defined (__KERNEL_NET_IPSTACK)
   #include "kernel/core/net/uip_core/uip_slip.h"
   #include "kernel/core/net/uip_core/uip_sock.h"

   #pragma message ("uip 1.0")
   #include "kernel/net/uip1.0/net/uip.h"
   #include "kernel/net/uip1.0/net/uip_arch.h"

//
//

   #include "kernel/core/net/uip_core/uip_core.h"
   #include "kernel/core/net/uip_core/uip_socket.h"
   #if defined(USE_IF_ETHERNET)
//#include "kernel/net/uip0.9/net/uip_arp.h"
//#include "kernel/net/uip1.0/net/uip_arp.h"
      #pragma message ("uip 1.0")
      #include "kernel/net/uip1.0/net/uip_arp.h"

   #endif
#endif


#include "kernel/fs/vfs/vfskernel.h"

/*============================================
| Global Declaration
==============================================*/

#define UIP_CORE_POLLING_PERIOD  50 //ms 200 10
#define UIP_CORE_STACK_SIZE  2048 //1024//1024
#define UIP_CORE_PRIORITY    140

//specific for network interface and ip stack
#if defined (__KERNEL_NET_IPSTACK)
_macro_stack_addr char uip_core_stack[UIP_CORE_STACK_SIZE];
#endif

kernel_pthread_t uip_core_thread;

typedef int (*_pf_uip_core_send_ip_packet)(desc_t desc, const unsigned char* buf, int size);
typedef _pf_uip_core_send_ip_packet _pf_uip_core_send_ip_packet_t;

typedef int (*_pf_uip_core_recv_ip_packet)(desc_t desc, unsigned char* buf, int size);
typedef _pf_uip_core_recv_ip_packet _pf_uip_core_recv_ip_packet_t;

typedef struct {
   _pf_uip_core_send_ip_packet_t _pf_uip_core_send_ip_packet;
   _pf_uip_core_recv_ip_packet_t _pf_uip_core_recv_ip_packet;
}_uip_core_ip_packet_op_t;


int _uip_core_send_frame(desc_t desc, const unsigned char* buf, int size);
int _uip_core_recv_frame(desc_t desc,unsigned char* buf, int size);

const enum _uip_core_ip_packet_op_type_t {
   NET_IP_PACKET_SLIP,
   NET_IP_PACKET_ETHERNET
};

#if defined (__KERNEL_NET_IPSTACK)
_uip_core_ip_packet_op_t _uip_core_ip_packet_op_list[]={
   {_slip_send_packet,_slip_recv_packet},
   {_uip_core_send_frame,_uip_core_recv_frame}
};
#endif

#if defined(USE_IF_SLIP)
const char g_uip_core_ip_packet_op_type=NET_IP_PACKET_SLIP;
#elif defined(USE_IF_ETHERNET)
const char g_uip_core_ip_packet_op_type=NET_IP_PACKET_ETHERNET;
#endif

static _pf_uip_core_send_ip_packet_t g_pf_uip_core_send_ip_packet;
static _pf_uip_core_recv_ip_packet_t g_pf_uip_core_recv_ip_packet;

#define USE_TCP_HIGHSPEED_ACK

#if defined (USE_TCP_HIGHSPEED_ACK)
   #define __uip_core_send_ip_packet(__uip_desc__,__uip_buf__,__uip_len__) _uip_split_output( \
      __uip_desc__,__uip_buf__,__uip_len__)
#else
   #define __uip_core_send_ip_packet(__uip_desc__,__uip_buf__, \
                                     __uip_len__) g_pf_uip_core_send_ip_packet(__uip_desc__, \
                                                                               __uip_buf__, \
                                                                               __uip_len__)
#endif

/*============================================
| Implementation
==============================================*/

#if defined (__KERNEL_NET_IPSTACK) && defined (USE_UIP_CORE)
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
      _kernel_int = __wait_io_int();
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
   ofile_lst[desc].pfsop->fdev.fdev_write(desc,&c,1);
   do {
      _kernel_int = __wait_io_int();
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
int _uip_core_recv_frame(desc_t desc, unsigned char* buf, int size){
   uchar8_t _kernel_int;
   while(ofile_lst[desc].pfsop->fdev.fdev_isset_read(desc)) {
      _kernel_int = __wait_io_int();
      if (!__K_IS_IOINTR(_kernel_int))
         continue;
   }
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
int _uip_core_send_frame(desc_t desc, const unsigned char* buf, int size){
   uchar8_t _kernel_int;
   int cb;
   cb=ofile_lst[desc].pfsop->fdev.fdev_write(desc,buf,size);
   do {
      _kernel_int = __wait_io_int();
      if (!__K_IS_IOINTR(_kernel_int))
         continue;
   } while(ofile_lst[desc].pfsop->fdev.fdev_isset_write(desc));
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
void _uip_split_output(desc_t desc, char* buf, int size){
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

/*-------------------------------------------
| Name:_uip_core_recv
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
   #define __eth_hdr ((struct uip_eth_hdr *)&uip_buf[0])
int _uip_core_recv(desc_t desc_r,desc_t desc_w){
   if(desc_r<0 || desc_w<0)
      return -1;

   uip_len = g_pf_uip_core_recv_ip_packet(desc_r,uip_buf, sizeof(uip_buf));
   if(uip_len > 0) {
   #if defined(USE_IF_ETHERNET)
      u16_t type = __eth_hdr->type;
      u16_t _TYPE = UIP_ETHTYPE_ARP;
      if(__eth_hdr->type == htons(UIP_ETHTYPE_IP)) {
         uip_arp_ipin();
         uip_process(UIP_DATA);
         /* If the above function invocation resulted in data that
         should be sent out on the network, the global variable
         uip_len is set to a value > 0. */
         if(uip_len > 0) {
            uip_arp_out();
            __uip_core_send_ip_packet(desc_w,uip_buf,uip_len);
            uip_len=0;
         }

      } else if(__eth_hdr->type == htons(UIP_ETHTYPE_ARP)) {
         uip_arp_arpin();
         /* If the above function invocation resulted in data that
            should be sent out on the network, the global variable
            uip_len is set to a value > 0. */
         if(uip_len > 0) {
            __uip_core_send_ip_packet(desc_w,uip_buf,uip_len);
            uip_len=0;
         }
      }
   #elif defined(USE_IF_SLIP)
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
int _uip_core_send(desc_t desc_w,int conn_no){
   struct socksconn_state * socksconn =(struct socksconn_state *)0;
   #if UIP_LOGGING==1
   struct uip_conn* my_uip_conn;
   int cb,_r,w;
   #endif

   if(desc_w<0)
      return -1;
   uip_len=0;

   //to remove
   #if UIP_LOGGING==1
   my_uip_conn = (struct uip_conn*)&uip_conns[conn_no];
   socksconn = (struct socksconn_state *)(my_uip_conn->appstate);

   _r   = socksconn->_r;
   w    = ((socket_t*)(socksconn->hsocks))->w;
   if(_r==w) {
      cb=0;
   }else if(w>_r) {
      cb = w-_r;
   }else{
      cb=(SND_SOCKET_BUFFER_SIZE-_r)+w;
   }
   printf("send by event desc=%d w=%d _r=%d cb=%d\r\n",((socket_t*)(socksconn->hsocks))->desc,w,_r,
          cb);
   #endif

   //
   uip_sendpacket(conn_no);
   if(uip_len > 0) {
   #if defined(USE_IF_ETHERNET)
      uip_arp_out();
   #endif
   #if UIP_LOGGING==1
      printf("send by event OK\r\n");
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
         uip_arp_out();
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
kernel_pthread_t* _uip_core_syscall(void){

   kernel_pthread_t* pthread_ptr;

   __atomic_in();
   pthread_ptr = g_pthread_lst;
   while(pthread_ptr) {
      if( (pthread_ptr->pid<=0) || (pthread_ptr->irq_nb==KERNEL_NET_INTERRUPT_NB)) {
         __atomic_out();
         return pthread_ptr;
      }
      pthread_ptr = pthread_ptr->gnext;
   }
   __atomic_out();
   return (kernel_pthread_t*)0;
}

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

/*-------------------------------------------
| Name:uip_core_routine
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
unsigned int _poll_event=0;
void* uip_core_routine(void* arg){

   unsigned char _uip_arp_counter=0;
   unsigned int _uip_poll_counter=0;
   unsigned int _uip_poll_arp_counter=0;
   kernel_pthread_t* pthread_ptr;

   #if defined(USE_IF_SLIP)
   desc_t desc_r = _vfs_open("/dev/ttys1",O_RDONLY,0);
   desc_t desc_w = _vfs_open("/dev/ttys1",O_WRONLY,0);
   #elif defined(USE_IF_ETHERNET)
   desc_t desc_r = _vfs_open("/dev/eth0",O_RDONLY,0);
   desc_t desc_w = _vfs_open("/dev/eth0",O_WRONLY,0);
   #endif

   pthread_ptr = kernel_pthread_self();

   ofile_lst[desc_r].owner_pthread_ptr_read  = pthread_ptr;
   ofile_lst[desc_w].owner_pthread_ptr_write = pthread_ptr;

   g_pf_uip_core_send_ip_packet =
      _uip_core_ip_packet_op_list[g_uip_core_ip_packet_op_type]._pf_uip_core_send_ip_packet;
   g_pf_uip_core_recv_ip_packet =
      _uip_core_ip_packet_op_list[g_uip_core_ip_packet_op_type]._pf_uip_core_recv_ip_packet;

   //slip synchro: only for chimbo windows
   //_uip_core_slip_sync(desc_r,desc_w);

   for(;; ) {
      int _flg_recv_packet;
      pid_t pid;

      pid=0;
      _flg_recv_packet=-1;

      if( !(_flg_recv_packet=ofile_lst[desc_r].pfsop->fdev.fdev_isset_read(desc_r))
          || (pthread_ptr = _uip_core_syscall()) ) {

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

         abs_timeout.tv_sec   = (timeout/1000);
         abs_timeout.tv_nsec  = (timeout%1000)*1000000; //ms->ns


         while(!(_poll_event=__wait_io_int3(pthread_ptr,&abs_timeout /*200*/))) { //5//10ms
            //
            if( !(_uip_poll_counter=(_uip_poll_counter+1)%5 /*%20*/) ) {
               _uip_poll_counter=0;
               _uip_core_poll(desc_w);
            }

            if( !(_uip_poll_arp_counter=(++_uip_poll_arp_counter)%5) ) {
               uip_arp_timer();
               // break;
            }
         }
      }
   }

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

   pthread_attr_t thread_attr;

   thread_attr.stacksize = UIP_CORE_STACK_SIZE;
   thread_attr.stackaddr = (void*)&uip_core_stack;
   thread_attr.priority  = UIP_CORE_PRIORITY;
   thread_attr.timeslice = 0;

   kernel_pthread_create(&uip_core_thread,&thread_attr,uip_core_routine,(char*)0);

   return 0;
}

#endif

/*============================================
| End of Source  : uip_core.c
==============================================*/
