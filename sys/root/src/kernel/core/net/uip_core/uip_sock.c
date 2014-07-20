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


/*===========================================
Includes
=============================================*/

#include "kernel/core/kernelconf.h"
#include "kernel/core/kernel.h"
#include "kernel/core/process.h"
#include "kernel/core/system.h"
//#include <time.h>


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
#include "kernel/net/uip2.5/net/uip-ds6.h"
#endif
#include "kernel/core/net/uip_core/uip_core.h"
#include "kernel/core/net/uip_core/uip_sock.h"

#if defined(USE_IF_ETHERNET)
   #if USE_UIP_VER == 1000 
   #pragma message ("uip 1.0")
   #include "kernel/net/uip1.0/net/uip_arp.h"
   #endif
   #if USE_UIP_VER == 2500 
      #pragma message ("uip 2.5")
      #include "kernel/net/uip2.5/net/uip_arp.h"
   #endif
#endif

/*===========================================
Global Declaration
=============================================*/
#if defined (USE_UIP)
socket_t _socketList[MAX_SOCKET];
struct socksconn_state _socksconn_state_list[UIP_CONNS+UIP_UDP_CONNS];

socket_t*   socketList=NULL;
struct socksconn_state*    socksconn_state_list=NULL;
uip_core_statistics_t _uip_core_statistics;
#define BUF ((struct uip_tcpip_hdr *)&uip_buf[UIP_LLH_LEN])
#define FBUF ((struct uip_tcpip_hdr *)&uip_reassbuf[0])
#define ICMPBUF ((struct uip_icmpip_hdr *)&uip_buf[UIP_LLH_LEN])
#define UDPBUF ((struct uip_udpip_hdr *)&uip_buf[UIP_LLH_LEN])
#endif

#if UIP_CONF_IPV6
uip_ipaddr_t uip_hostaddr, uip_draddr, uip_netmask;
#endif
/*===========================================
Implementation
=============================================*/

/*-------------------------------------------
| Name:uip_sock_init
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int uip_sock_init(void) {
#if defined (USE_UIP)
   int i;
   socketList = (socket_t*)&_socketList;
   socksconn_state_list = (struct socksconn_state*)&_socksconn_state_list;
   uip_init();
   #if UIP_CONF_IPV6
   {
      uip_ip6addr_t ip6addr;
      memcpy(uip_lladdr.addr, "\x00\x0c\x29\x28\x23\x45", 6); // Set in zigbeestack.cpp (UID)    
      uip_ip6addr(&ip6addr, 0xfe80,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0002);
      uip_sethostaddr(&ip6addr);
      uip_ipaddr_copy(&uip_hostaddr, &ip6addr);

      tcpip_init();  // rpl_init() is called inside tcpip_init()
      clock_init();
      uip_ds6_addr_add(&ip6addr, 0, ADDR_MANUAL);
   }
   #endif
   //ethernet
   #if defined(USE_IF_ETHERNET)
      #if !UIP_CONF_IPV6
   uip_arp_init();
   #endif
#endif
 
   for(i=0; i<MAX_SOCKET; i++) {
      #if UIP_CONF_IPV6
      socketList[i].addr_in.sin6_port=0;
      #else
      socketList[i].addr_in.sin_port=0;
      #endif
      socketList[i].socksconn=NULL;
      socketList[i].desc = -1;
   }

   for(i=0; i<UIP_CONNS; i++) {
      socksconn_state_list[i].hsocks=NULL;
      uip_conns[i].appstate.state = &socksconn_state_list[i];
   }
   for(;i<(UIP_CONNS+UIP_UDP_CONNS);i++){
      socksconn_state_list[i].hsocks=NULL;
      uip_udp_conns[i-UIP_CONNS].appstate.state = &socksconn_state_list[i];
   }
#else
   return -1;
#endif
   return 0;
}

/*-------------------------------------------
| Name:sock_get
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
hsock_t sock_get(void){
#if defined (USE_UIP)
   int i;

   for(i=0; i<MAX_SOCKET; i++) {
      #if UIP_CONF_IPV6
      if(!socketList[i].addr_in.sin6_port){
         socketList[i].addr_in.sin6_scope_id=0;
      #else
      if(!socketList[i].addr_in.sin_port) {
      #endif

         socketList[i].r=0;
         socketList[i].w=0;

         socketList[i].socksconn=(hsock_t)0;
         //printf("alloc socket=%d\n",i);
         return (hsock_t)&socketList[i];
      }
   }
#endif
   return (hsock_t)0;
}

/*-------------------------------------------
| Name:sock_put
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void sock_put(hsock_t hsock){
#if defined (USE_UIP)
   if(!hsock) return;
   #if UIP_CONF_IPV6
   ((socket_t*)hsock)->addr_in.sin6_port=0;
   #else
   ((socket_t*)hsock)->addr_in.sin_port=0;
   #endif
#endif
}

/*-------------------------------------------
| Name:socksconn_no
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int socksconn_no(desc_t desc){
#if defined (USE_UIP)
   int i;

   for(i = 0; i < UIP_CONNS; ++i) {
      struct socksconn_state * socksconn = (struct socksconn_state *)(uip_conns[i].appstate.state);
      if( !(socket_t*)socksconn->hsocks
          || (((socket_t*)socksconn->hsocks)->desc<0)
          || (((socket_t*)(socksconn->hsocks))->state==STATE_SOCKET_CLOSED)) continue;
      if( (struct socksconn_state *)socksconn==(struct socksconn_state *)((socket_t*)(ofile_lst[desc].p))->socksconn)
         return i;
   }
   for(i=0;i < UIP_UDP_CONNS; ++i) {
      struct socksconn_state * socksconn = (struct socksconn_state *)(uip_udp_conns[i].appstate.state);

      if( !(socket_t*)socksconn->hsocks 
         || (((socket_t*)socksconn->hsocks)->desc<0)
         || (((socket_t*)(socksconn->hsocks))->state==STATE_SOCKET_CLOSED)) continue;
      if( (struct socksconn_state *)socksconn==(struct socksconn_state *)((socket_t*)(ofile_lst[desc].p))->socksconn)
         return i;
   }
#endif
   return -1;
}

#if defined (USE_UIP)
extern u16_t uip_slen;
void uip_sock_udp_callback(void){
   struct socksconn_state * socksconn = (struct socksconn_state *)(uip_udp_conn->appstate.state);
   if(uip_newdata()){
      int _w = socksconn->_w;
      int _r = ((socket_t*)(socksconn->hsocks))->r;
      int free_size=0;
      int cb=uip_datalen()+sizeof(socket_recvfrom_header_t);
      socket_recvfrom_header_t socket_recvfrom_header={0};
      hsock_t hsock = socksconn->hsocks;
      #if UIP_LOGGING==1
      UIP_EVENT(EVT_UIP_SOCK + EVT_LVL_DEBUG + EVT_UIP_SOCK_CORE_UDP_CALLBACK, 
                ((socket_t*)(socksconn->hsocks))->desc, &cb, sizeof(cb),
                "uip_sock_udp_callback() desc=%d len=%d\n",((socket_t*)(socksconn->hsocks))->desc,cb);
      #endif
      if((free_size=(_r-_w))<=0)
        free_size=RCV_SOCKET_BUFFER_SIZE+free_size;
      _uip_core_statistics.sock_rcv_buffer_free_size = free_size;
      if(cb>(free_size-2)){//no
        _uip_core_statistics.sock_drop_packet++;
        return;//drop packet
      }
      #if UIP_CONF_IPV6
      ((socket_t*)hsock)->addr_in.sin6_port=0;
      socket_recvfrom_header.addr_in_from.sin6_port = UDPBUF->srcport; 
      uip_ipaddr_copy( (uip_ipaddr_t*)&socket_recvfrom_header.addr_in_from.sin6_addr.s6_addr,
                       &BUF->srcipaddr);
      socket_recvfrom_header.len=cb-sizeof(socket_recvfrom_header_t);
      cb = sizeof(socket_recvfrom_header_t);
      if(cb<(RCV_SOCKET_BUFFER_SIZE-_w)){
         memcpy(&socksconn->rcv_buffer[_w],(char*)&socket_recvfrom_header,cb);
         _w=_w+cb;
      }else{
         memcpy(&socksconn->rcv_buffer[_w],(char*)&socket_recvfrom_header,(RCV_SOCKET_BUFFER_SIZE-_w));
         memcpy(socksconn->rcv_buffer, (char*)&socket_recvfrom_header+(RCV_SOCKET_BUFFER_SIZE-_w), cb-(RCV_SOCKET_BUFFER_SIZE-_w));
         _w=cb-(RCV_SOCKET_BUFFER_SIZE-_w);
      }  
      cb = socket_recvfrom_header.len;
      #else
      ((socket_t*)hsock)->addr_in.sin_port=0;
      ((socket_t*)(socksconn->hsocks))->addr_in_from.sin_port = UDPBUF->srcport; 
      uip_ipaddr_copy( (uip_ipaddr_t*)&((socket_t*)(socksconn->hsocks))->addr_in_from.sin_addr.s_addr,
                       &BUF->srcipaddr);
      #endif
      if(cb<(RCV_SOCKET_BUFFER_SIZE-_w)){
         memcpy(&socksconn->rcv_buffer[_w],(char*)uip_appdata,uip_datalen());
         _w=_w+uip_datalen();
      }else{
         memcpy(&socksconn->rcv_buffer[_w],(char*)uip_appdata,(RCV_SOCKET_BUFFER_SIZE-_w));
         memcpy(socksconn->rcv_buffer,(char*)uip_appdata+(RCV_SOCKET_BUFFER_SIZE-_w),uip_datalen()-(RCV_SOCKET_BUFFER_SIZE-_w));
         _w=uip_datalen()-(RCV_SOCKET_BUFFER_SIZE-_w);
      }  
      socksconn->_w=_w;
      ((char*)uip_appdata)[cb]=0x00;
      #if UIP_LOGGING==1
      {
         ANNOTATE("uip_appdata = %s\n",uip_appdata);
      }
      #endif
      uip_send(uip_appdata, 0);
      __PUT_SOCKET_EVENT(socksconn->hsocks,O_RDONLY);
   }
   if(uip_poll()){
      int _r;
      int w;
      int cb;
      desc_t desc = ((socket_t*)(socksconn->hsocks))->desc;
      _r   = socksconn->_r;
      w    = ((socket_t*)(socksconn->hsocks))->w;
      if(_r==w){
         return;
      }
      if(w>_r){
         cb = w-_r;
         memcpy((char*)uip_appdata,&socksconn->snd_buffer[_r],w-_r);
         _r=w;
      }else{
         cb=(SND_SOCKET_BUFFER_SIZE-_r)+w;
         memcpy((char*)uip_appdata,&socksconn->snd_buffer[_r],SND_SOCKET_BUFFER_SIZE-_r);
         memcpy((char*)uip_appdata+(SND_SOCKET_BUFFER_SIZE-_r),socksconn->snd_buffer,w);
         _r=w;
      }
      socksconn->__r = _r;
      uip_slen = cb;
      memcpy(&uip_buf[UIP_LLH_LEN + UIP_IPUDPH_LEN], uip_appdata, cb > UIP_BUFSIZE? UIP_BUFSIZE: cb);
      socksconn->_r = socksconn->__r;
      ((socket_t*)(socksconn->hsocks))->state=STATE_SOCKET_WAIT;
      __PUT_SOCKET_EVENT(socksconn->hsocks,O_WRONLY);
   }
}
/*-------------------------------------------
| Name:uip_sock
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int uip_sock_tcp_callback(void){
 struct socksconn_state * socksconn = (struct socksconn_state *)(uip_conn->appstate.state);
   int i;

#if defined(LEPTON) && UIP_LOGGING==1
   static clock_t start;
   static clock_t finish;
   #endif


   //
#if defined(LEPTON) && UIP_LOGGING==1
   if(socksconn && socksconn->hsocks && uip_flags!=UIP_POLL) {
      printf("\nuip_sock() desc=%d fd=%d",((socket_t*)(socksconn->hsocks))->desc,((socket_t*)(socksconn->hsocks))->fd);
      //
      printf(" state: ");
      switch(((socket_t*)(socksconn->hsocks))->state) {
      case STATE_SOCKET_CONNECT:
         printf("STATE_SOCKET_CONNECT");
         break;
      case  STATE_SOCKET_LISTEN:
         printf("STATE_SOCKET_LISTEN");
         break;
      case STATE_SOCKET_CONNECTED:
         printf("STATE_SOCKET_CONNECTED");
         break;
      case STATE_SOCKET_ACCEPTED:
         printf("STATE_SOCKET_ACCEPTED");
         break;
      case STATE_SOCKET_RECV:
         printf("STATE_SOCKET_RECV");
         break;
      case STATE_SOCKET_SEND:
         printf("STATE_SOCKET_SEND");
         break;
      case STATE_SOCKET_WAIT:
         printf("STATE_SOCKET_WAIT");
         break;
      case STATE_SOCKET_CLOSE:
         printf("STATE_SOCKET_CLOSE");
         break;
      case STATE_SOCKET_CLOSED:
         printf("STATE_SOCKET_CLOSED");
         break;
      case STATE_SOCKET_ABORTED:
         printf("STATE_SOCKET_ABORTED");
         break;
      case STATE_SOCKET_TIMEDOUT:
         printf("STATE_SOCKET_TIMEDOUT");
         break;
      default:
         printf("ERROR INVALID STATE");
         break;
      }
      printf("\n");
   }
   #endif //#UIP_LOGGING
   if(uip_connected()) {
      //retrieve socks
   #if UIP_LOGGING==1
      ANNOTATE("connect\n");
   #endif
      for(i=0; i<MAX_SOCKET; i++) {
         //
         if((socketList[i].state == STATE_SOCKET_LISTEN)
            &&
            #if UIP_CONF_IPV6
            uip_conn->lport == socketList[i].addr_in.sin6_port
            #else
            uip_conn->lport == socketList[i].addr_in.sin_port
            #endif
            //printf("connection request on port : %d\n",ntohs(uip_conn->lport));
            ){
            socksconn->hsocks=(hsock_t)&socketList[i];



            ((socket_t*)(socksconn->hsocks))->state=STATE_SOCKET_CONNECTED;
            //post event SOCKS_CONNECTED to application data on listen socket
            //__PUT_SOCKET_EVENT(socksconn->hsocks);
            {
               hsock_t hsocks_listen = socksconn->hsocks;
               desc_t desc_listen = ((socket_t*)hsocks_listen)->desc ;
               desc_t desc;

               //printf("connection accepted on port : %d\n",uip_conn->lport);

               //get new socket from list
               desc = _vfs_open("/dev/sock",O_RDWR,0);

               if(desc>=0){
                  ((socket_t*)ofile_lst[desc].p)->accept_desc = desc ;
                  //set pthread owner with desc of listen sock
                  socksconn->hsocks = ofile_lst[desc].p;
                  ofile_lst[desc].owner_pthread_ptr_read=ofile_lst[desc_listen].owner_pthread_ptr_read;
                  ofile_lst[desc].owner_pthread_ptr_write=ofile_lst[desc_listen].owner_pthread_ptr_write;
               }else{
                  socksconn->hsocks = 0;
                  continue;
               }

               //
               ((socket_t*)(hsocks_listen))->hsocks=socksconn->hsocks;

               ((socket_t*)(socksconn->hsocks))->socksconn = socksconn;

               ((socket_t*)(socksconn->hsocks))->socksconn->_r=0;
               ((socket_t*)(socksconn->hsocks))->socksconn->_w=0;

               //send hsocks
               #if UIP_CONF_IPV6
               memcpy( &((socket_t*)(socksconn->hsocks))->addr_in.sin6_addr,
                       &((socket_t*)(hsocks_listen))->addr_in.sin6_addr, sizeof(struct _in6_addr));
               ((socket_t*)(socksconn->hsocks))->addr_in.sin6_port=uip_conn->rport;
               #else
               ((socket_t*)(socksconn->hsocks))->addr_in.sin_addr.s_addr
                  =((socket_t*)(hsocks_listen))->addr_in.sin_addr.s_addr;

               //
               //((socket_t*)(socksconn->hsocks))->addr_in.sin_port=((socket_t*)(hsocks_listen))->addr_in.sin_port;
               ((socket_t*)(socksconn->hsocks))->addr_in.sin_port=uip_conn->rport;
               #endif

               ((socket_t*)(socksconn->hsocks))->state=STATE_SOCKET_WAIT;
               //((socket_t*)(hsocks_listen))->state=STATE_SOCKET_LISTEN;//set by accept function
   #if UIP_LOGGING==1
               ANNOTATE("accepted() i=%d desc=%d on port:%d\n",i,
                      ((socket_t*)(socksconn->hsocks))->desc,
                      ntohs(uip_conn->lport));
   #endif

               __PUT_SOCKET_EVENT(hsocks_listen,O_RDONLY);
            }

            uip_send(uip_appdata, 0);
            break;
         }else if((socketList[i].state == STATE_SOCKET_CONNECT)
            &&
            #if UIP_CONF_IPV6
            uip_conn->rport == socketList[i].addr_in.sin6_port
            #else
            uip_conn->rport == socketList[i].addr_in.sin_port
            #endif
            ){

            //printf("connection ok on port : %d\n",ntohs(uip_conn->lport));

            socksconn->hsocks=(hsock_t)&socketList[i];
            socksconn->__r = 0;
            socksconn->_r  = 0;
            socksconn->_w  = 0;

            ((socket_t*)(socksconn->hsocks))->state=STATE_SOCKET_WAIT;
            //post event STATE_SOCKET_WAIT ready to send or recv data from remote host
   #if UIP_LOGGING==1
            ANNOTATE("connected() i=%d desc=%d on port:%d\n",i,
                   ((socket_t*)(socksconn->hsocks))->desc,
                   ntohs(uip_conn->rport));
   #endif
            __PUT_SOCKET_EVENT(socksconn->hsocks,O_RDONLY);
            uip_send(uip_appdata, 0);
            break;
         }
      }

   } //end connected


   //trace flags
   #if UIP_LOGGING==1
   if(uip_flags!=UIP_POLL) {
      ANNOTATE(" uip_flags: ");
      switch(uip_flags) {
      case UIP_ACKDATA:
            ANNOTATE("UIP_ACKDATA");
         break;
      case UIP_NEWDATA:
            ANNOTATE("UIP_NEWDATA");
         break;
      case UIP_REXMIT:
            ANNOTATE("UIP_REXMIT");
         break;
      case UIP_POLL:
            ANNOTATE("UIP_POLL");
         break;
      case UIP_CLOSE:
            ANNOTATE("UIP_CLOSE");
         break;
      case UIP_ABORT:
            ANNOTATE("UIP_ABORT");
         break;
      case UIP_CONNECTED:
            ANNOTATE("UIP_CONNECTED");
         break;
      case UIP_TIMEDOUT:
            ANNOTATE("UIP_TIMEDOUT");
         break;
      default:
            ANNOTATE("WARNING UNKNOW FLAG:0x%x",uip_flags);
         break;

      }
      ANNOTATE("\n");
   }
   #endif


   if(!socksconn->hsocks) {
      uip_send(uip_appdata, 0);
      return 0;
   }

   //
   if(((socket_t*)(socksconn->hsocks))->state==STATE_SOCKET_ACCEPTED) {
      //to do:lepton: allocate process file descriptor (process_lst[pid]->desc_tbl[fildes])
   }

   if(uip_acked()) {
      //socket buffer size = uip_mss
      #if defined(LEPTON) && UIP_LOGGING==1
      {
         double duration;
         desc_t __desc = ((socket_t*)(socksconn->hsocks))->desc;
         finish = clock();
         duration = (double)(finish - start) / CLOCKS_PER_SEC;
         printf( "acked time= %2.1f s\n", duration );
         printf("ackeddata() desc=%d len=%d pid=%d\n",__desc,uip_datalen(),dev_open_list[__desc].owner_pid);
         printf("...ackeddata() _r=%d __r=%d w=%d\n",socksconn->_r,socksconn->__r,((socket_t*)(socksconn->hsocks))->w);
         printf("\n--- end send ---\n");
      }
   #endif

      socksconn->_r = socksconn->__r;
      ((socket_t*)(socksconn->hsocks))->state=STATE_SOCKET_WAIT;
      __PUT_SOCKET_EVENT(socksconn->hsocks,O_RDONLY);

   }

   if( ((socket_t*)(socksconn->hsocks))->state==STATE_SOCKET_WAIT ) {

      if(uip_newdata()) {

         int _w = socksconn->_w;
         int cb=uip_datalen();

   #if UIP_LOGGING==1
         ANNOTATE("newdata() desc=%d len=%d\n",((socket_t*)(socksconn->hsocks))->desc,cb);
   #endif

         if(uip_datalen()<(RCV_SOCKET_BUFFER_SIZE-_w)) {
            memcpy(&socksconn->rcv_buffer[_w],(char*)uip_appdata,uip_datalen());
            _w=_w+uip_datalen();
         }else{
            memcpy(&socksconn->rcv_buffer[_w],(char*)uip_appdata,(RCV_SOCKET_BUFFER_SIZE-_w));
            memcpy(socksconn->rcv_buffer,(char*)uip_appdata+(RCV_SOCKET_BUFFER_SIZE-_w),uip_datalen()-(RCV_SOCKET_BUFFER_SIZE-_w));
            _w=uip_datalen()-(RCV_SOCKET_BUFFER_SIZE-_w);
         }

         socksconn->_w=_w;

         ((char*)uip_appdata)[cb]=0x00;

   #if UIP_LOGGING==1
         {
//            desc_t __desc = ((socket_t*)(socksconn->hsocks))->desc;
             ANNOTATE("uip_appdata = %s pid=%d\n",uip_appdata,dev_open_list[__desc].owner_pid);
         }
   #endif

         uip_send(uip_appdata, 0);

         //((socket_t*)(socksconn->hsocks))->state=STATE_SOCKET_RECV;
         __PUT_SOCKET_EVENT(socksconn->hsocks,O_RDONLY);

      }


      if(uip_poll() || uip_rexmit()) {
         int _r;
         int w;
         int cb;
         desc_t desc = ((socket_t*)(socksconn->hsocks))->desc;

         //printf("poll()\n");

         //to do:lepton use __trylock_io instead __lock_io??? : speed optimization
         //Enter semaphore
         __lock_io(ofile_lst[desc].owner_pthread_ptr_write,desc,O_WRONLY);

         if( uip_rexmit() )
            _r=0;

         _r   = socksconn->_r;
         w    = ((socket_t*)(socksconn->hsocks))->w;

         if(_r==w) {
            //Leave semaphore
            __unlock_io(ofile_lst[desc].owner_pthread_ptr_write,desc,O_WRONLY);
            return 0;
         }

         if(w>_r) {
            cb = w-_r;
            memcpy((char*)uip_appdata,&socksconn->snd_buffer[_r],w-_r);
            _r=w;
         }else{
            cb=(SND_SOCKET_BUFFER_SIZE-_r)+w;
            memcpy((char*)uip_appdata,&socksconn->snd_buffer[_r],SND_SOCKET_BUFFER_SIZE-_r);
            memcpy((char*)uip_appdata+(SND_SOCKET_BUFFER_SIZE-_r),socksconn->snd_buffer,w);
            _r=w;
         }

         socksconn->__r = _r;
         //((socket_t*)(socksconn->hsocks))->state=STATE_SOCKET_SEND;
         uip_send(uip_appdata,cb);

         #if defined(LEPTON) && UIP_LOGGING==1
         if(uip_poll()) {
            printf("\n--- poll() begin send ---\n");
            start = clock();
         }else if(uip_rexmit()) {
            printf("\n--- rexmit() begin send ---\n");
         }
         ((char*)uip_appdata)[cb]=0x00;
         printf("uip_appdata= %s\n",uip_appdata);
         printf("senddata() desc=%d len=%d\n",((socket_t*)(socksconn->hsocks))->desc,cb);
   #endif

         //Leave semaphore
         __unlock_io(ofile_lst[desc].owner_pthread_ptr_write,desc,O_WRONLY);
      }

   }

   //
   if(((socket_t*)(socksconn->hsocks))->state==STATE_SOCKET_CLOSE) {
      ((socket_t*)(socksconn->hsocks))->state=STATE_SOCKET_CLOSED;
   #if UIP_LOGGING==1
      PRINTF("close request desc=%d\n",((socket_t*)(socksconn->hsocks))->desc);
   #endif
      uip_close();
      uip_send(uip_appdata,0);
      //__PUT_SOCKET_EVENT(socksconn->hsocks);
   }else if(uip_closed()) {
   #if UIP_LOGGING==1
      PRINTF("uip_closed() desc=%d\n",((socket_t*)(socksconn->hsocks))->desc);
   #endif
      ((socket_t*)(socksconn->hsocks))->state=STATE_SOCKET_CLOSED;
      //__PUT_SOCKET_EVENT(socksconn->hsocks);
      uip_send(uip_appdata,0);
   }else if(uip_aborted()) {
   #if UIP_LOGGING==1
      PRINTF("uip_aborted() desc=%d\n",((socket_t*)(socksconn->hsocks))->desc);
   #endif
      ((socket_t*)(socksconn->hsocks))->state=STATE_SOCKET_ABORTED;
      uip_close();
      uip_send(uip_appdata,0);
      if(socksconn->hsocks)
         __PUT_SOCKET_EVENT(socksconn->hsocks,O_WRONLY);
   }else if(uip_timedout()) {
   #if UIP_LOGGING==1
      PRINTF("uip_timedout() desc=%d\n",((socket_t*)(socksconn->hsocks))->desc);
   #endif
      ((socket_t*)(socksconn->hsocks))->state=STATE_SOCKET_TIMEDOUT;
      uip_close();
      uip_send(uip_appdata,0);
      if(socksconn->hsocks)
         __PUT_SOCKET_EVENT(socksconn->hsocks,O_WRONLY);
   }
   return 0;
}
#endif //defined (USE_UIP)

/*===========================================
End of SourceOS_SocketEngine.c
=============================================*/
