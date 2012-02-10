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

#include "kernel/errno.h"
#include "kernel/types.h"
#include "kernel/interrupt.h"
#include "kernel/kernel.h"
#include "kernel/syscall.h"
#include "kernel/process.h"
#include "kernel/signal.h"
#include "kernel/statvfs.h"
#include "kernel/bin.h"
#include "kernel/rttimer.h"
#include "kernel/time.h"
#include "kernel/flock.h"

#include "kernel/kernelconf.h"

#include "kernel/net.h"

#if defined (__KERNEL_NET_UIP)
   #include "kernel/net/uip_slip.h"
   #include "kernel/net/uip_sock.h"
   #include "kernel/net/uip0.9/net/uip.h"
   #if defined(USE_IF_ETHERNET)
      #include "kernel/net/uip0.9/net/uip_arp.h"
   #endif
#endif

//distrib: must be remove
#include "kernel/ioctl_board.h"
#include "kernel/ioctl_keyb.h"
#include "kernel/ioctl_hd.h"


#include "vfs/vfskernel.h"

/*============================================
| Global Declaration
==============================================*/

pthread_t kernel_net_thread;

typedef int (*_pf_net_send_ip_packet)(desc_t desc, const unsigned char* buf, int size);
typedef _pf_net_send_ip_packet _pf_net_send_ip_packet_t;

typedef int (*_pf_net_recv_ip_packet)(desc_t desc, unsigned char* buf, int size);
typedef _pf_net_recv_ip_packet _pf_net_recv_ip_packet_t;

typedef struct {
   _pf_net_send_ip_packet_t _pf_net_send_ip_packet;
   _pf_net_recv_ip_packet_t _pf_net_recv_ip_packet;
}_net_ip_packet_op_t;


int _kernel_netsend_frame(desc_t desc, const unsigned char* buf, int size);
int _kernel_netrecv_frame(desc_t desc,unsigned char* buf, int size);

const enum _net_ip_packet_op_type_t {
   NET_IP_PACKET_SLIP,
   NET_IP_PACKET_ETHERNET
};

#if defined (__KERNEL_NET_UIP)
_net_ip_packet_op_t _net_ip_packet_op_list[]={
   {_slip_send_packet,_slip_recv_packet},
   {_kernel_netsend_frame,_kernel_netrecv_frame}
};
#endif

#if defined(USE_IF_SLIP)
const char g_net_ip_packet_op_type=NET_IP_PACKET_SLIP;
#elif defined(USE_IF_ETHERNET)
const char g_net_ip_packet_op_type=NET_IP_PACKET_ETHERNET;
#endif

static _pf_net_send_ip_packet_t g_pf_net_send_ip_packet;
static _pf_net_recv_ip_packet_t g_pf_net_recv_ip_packet;

/*============================================
| Implementation
==============================================*/

#if defined (__KERNEL_NET_UIP)
/*-------------------------------------------
| Name:_kernel_recv_char
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
unsigned char _kernel_netrecv_char(desc_t desc){
   unsigned char c;
   uchar8_t _kernel_int;
   while(ofile_lst[desc].pfsop->fdev.fdev_isset_read(desc)) {
      _kernel_int = __net_wait_int();
      if (!__K_IS_IOINTR(_kernel_int))
         continue;
   }
   ofile_lst[desc].pfsop->fdev.fdev_read(desc,&c,1);
   return c;
}

/*-------------------------------------------
| Name:_kernel_send_char
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void _kernel_netsend_char(desc_t desc,unsigned char c){
   uchar8_t _kernel_int;
   ofile_lst[desc].pfsop->fdev.fdev_write(desc,&c,1);
   do {
      _kernel_int = __net_wait_int();
      if (!__K_IS_IOINTR(_kernel_int))
         continue;
   } while(ofile_lst[desc].pfsop->fdev.fdev_isset_write(desc));
}

/*-------------------------------------------
| Name:_kernel_netrecv_frame
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _kernel_netrecv_frame(desc_t desc, unsigned char* buf, int size){
   uchar8_t _kernel_int;
   while(ofile_lst[desc].pfsop->fdev.fdev_isset_read(desc)) {
      _kernel_int = __net_wait_int();
      if (!__K_IS_IOINTR(_kernel_int))
         continue;
   }
   return ofile_lst[desc].pfsop->fdev.fdev_read(desc,buf,size);
}

/*-------------------------------------------
| Name:_kernel_netsend_frame
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _kernel_netsend_frame(desc_t desc, const unsigned char* buf, int size){
   uchar8_t _kernel_int;
   int cb;
   cb=ofile_lst[desc].pfsop->fdev.fdev_write(desc,buf,size);
   do {
      _kernel_int = __net_wait_int();
      if (!__K_IS_IOINTR(_kernel_int))
         continue;
   } while(ofile_lst[desc].pfsop->fdev.fdev_isset_write(desc));

   return cb;
}

/*-------------------------------------------
| Name:_kernel_net_uip_recv
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
   #define __eth_hdr ((struct uip_eth_hdr *)&uip_buf[0])
int _kernel_net_uip_recv(desc_t desc_r,desc_t desc_w){
   if(desc_r<0 || desc_w<0)
      return -1;

   uip_len = g_pf_net_recv_ip_packet(desc_r,uip_buf, sizeof(uip_buf));
   if(uip_len > 0) {
   #if defined(USE_IF_ETHERNET)
      if(__eth_hdr->type == htons(UIP_ETHTYPE_IP)) {
         uip_arp_ipin();
         uip_process(UIP_DATA);
         /* If the above function invocation resulted in data that
         should be sent out on the network, the global variable
         uip_len is set to a value > 0. */
         if(uip_len > 0) {
            uip_arp_out();
            g_pf_net_send_ip_packet(desc_w,uip_buf,uip_len);
            uip_len=0;
         }

      } else if(__eth_hdr->type == htons(UIP_ETHTYPE_ARP)) {
         uip_arp_arpin();
         /* If the above function invocation resulted in data that
            should be sent out on the network, the global variable
            uip_len is set to a value > 0. */
         if(uip_len > 0) {
            g_pf_net_send_ip_packet(desc_w,uip_buf,uip_len);
            uip_len=0;
         }
      }
   #elif defined(USE_IF_SLIP)
      uip_process(UIP_DATA);
      /* If the above function invocation resulted in data that
      should be sent out on the network, the global variable
      uip_len is set to a value > 0. */
      if(uip_len > 0) {
         g_pf_net_send_ip_packet(desc_w,uip_buf,uip_len);
         uip_len=0;
      }
   #endif

   }

   return 0;
}

/*-------------------------------------------
| Name:_kernel_net_uip_send
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _kernel_net_uip_send(desc_t desc_w,int conn_no){
   if(desc_w<0)
      return -1;
   uip_len=0;
   uip_sendpacket(conn_no);
   if(uip_len > 0) {
   #if defined(USE_IF_ETHERNET)
      uip_arp_out();
   #endif
      g_pf_net_send_ip_packet(desc_w,uip_buf,uip_len);
      uip_len=0;
   }

   return 0;
}

/*-------------------------------------------
| Name:_kernel_net_poll
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _kernel_net_poll(desc_t desc_w){
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
         g_pf_net_send_ip_packet(desc_w,uip_buf,uip_len);
         uip_len=0;
      }
   }

   return 0;
}

/*-------------------------------------------
| Name:_kernel_net_syscall
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
pid_t _kernel_net_syscall(void){

   pid_t _pid;

   for(_pid=1; _pid<=PROCESS_MAX; _pid++) {
      if(!process_lst[_pid] || process_lst[_pid]->irq_nb!=KERNEL_NET_INTERRUPT_NB)
         continue;
      //
      return _pid;
   }

   return (pid_t)0;
}

/*-------------------------------------------
| Name:_kernel_slip_sync
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _kernel_slip_sync(desc_t desc_r,desc_t desc_w)
{
   char buffer[]={"CLIENTSERVER\0"};
   char rcv_buffer[25]={0};
   unsigned char c=0;
   int i =0;

   while((c=_kernel_netrecv_char(desc_r))!=192) {
      rcv_buffer[i]=c;
      i++;

      if(!strcmp("CLIENTCLIENTCLIENT",rcv_buffer)) {
         i=0;
         while(buffer[i]) {
            _kernel_netsend_char(desc_w,buffer[i]);
            i++;
         }
         i=0;
         memset(rcv_buffer,0,14);
      }
   }

   return 0;
}

/*-------------------------------------------
| Name:kernel_net_routine
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
unsigned int _poll_event=0;
void* kernel_net_routine(void* arg){

   unsigned char _uip_arp_counter=0;
   unsigned int _uip_poll_counter=0;

   #if defined(USE_IF_SLIP)
   desc_t desc_r = _vfs_open("/dev/ttys1",O_RDONLY,0);
   desc_t desc_w = _vfs_open("/dev/ttys1",O_WRONLY,0);
   #elif defined(USE_IF_ETHERNET)
   desc_t desc_r = _vfs_open("/dev/eth0",O_RDONLY,0);
   desc_t desc_w = _vfs_open("/dev/eth0",O_WRONLY,0);
   #endif


   g_pf_net_send_ip_packet = _net_ip_packet_op_list[g_net_ip_packet_op_type]._pf_net_send_ip_packet;
   g_pf_net_recv_ip_packet = _net_ip_packet_op_list[g_net_ip_packet_op_type]._pf_net_recv_ip_packet;

   //slip synchro
   //_kernel_slip_sync(desc_r,desc_w);

   for(;; ) {
      int _flg_recv_packet;
      pid_t pid;

      pid=0;
      _flg_recv_packet=-1;

      if( !(_flg_recv_packet=ofile_lst[desc_r].pfsop->fdev.fdev_isset_read(desc_r))
          || (pid = _kernel_net_syscall()) ) {

         //rcv ip packet ok!
         if(!_flg_recv_packet) {
            _kernel_net_uip_recv(desc_r,desc_w);
         }

         if(pid) { //syscall for ip stack operation
            desc_t desc_sock;
            netsend_t* netsend_dt;
            int conn_no;

            if(process_lst[pid]->reg.syscall!=_SYSCALL_NET_SND) {
               __flush_syscall(pid);
               __kernel_ret_int(pid);
               __syscall_unlock();
               continue;
            }

            netsend_dt = (netsend_t*)process_lst[pid]->reg.data;
            desc_sock = netsend_dt->desc;

            //to do:lepton: find connection
            conn_no = socksconn_no(desc_sock);

            __flush_syscall(pid);
            __kernel_ret_int(pid);
            __syscall_unlock();

            //
            if(conn_no>=0) {
               _kernel_net_uip_send(desc_w,conn_no);
            }
         }

      }else{
         //to remove: test
         //OS_Delay(5);
         while(!(_poll_event=__net_wait_int_timed(KERNEL_NET_POLLING_PERIOD /*200*/))) { //5//10ms
            //
            if( !(_uip_poll_counter=(_uip_poll_counter+1)%5 /*%20*/) ) {
               _kernel_net_poll(desc_w);
            }
            break;
         }
      }
   }

}
#endif

/*============================================
| End of Source  : net.c
==============================================*/
