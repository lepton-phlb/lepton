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


#pragma message ("uip 1.0")
#include "kernel/net/uip1.0/net/uip.h"

#include "kernel/core/net/uip_core/uip_sock.h"

#if defined(USE_IF_ETHERNET)
   #pragma message ("uip 1.0")
   #include "kernel/net/uip1.0/net/uip_arp.h"
#endif

/*===========================================
Global Declaration
=============================================*/
#if defined (__KERNEL_NET_IPSTACK)
socket_t _socketList[MAX_SOCKET];
struct socksconn_state _socksconn_state_list[UIP_CONNS];

socket_t*   socketList=NULL;
struct socksconn_state*    socksconn_state_list=NULL;
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
#if defined (__KERNEL_NET_IPSTACK)
   int i;
   socketList = (socket_t*)&_socketList;
   socksconn_state_list = (struct socksconn_state*)&_socksconn_state_list;
   uip_init();

   //ethernet
   #if defined(USE_IF_ETHERNET)
   uip_arp_init();
   #endif
#else
   return -1;
#endif
#if defined (__KERNEL_NET_IPSTACK)
   for(i=0; i<MAX_SOCKET; i++) {
      socketList[i].addr_in.sin_port=0;
      socketList[i].socksconn=NULL;
      socketList[i].desc = -1;
   }

   for(i=0; i<UIP_CONNS; i++) {
      socksconn_state_list[i].hsocks=NULL;
      uip_conns[i].appstate = &socksconn_state_list[i];
   }

   return 0;
#endif
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
#if defined (__KERNEL_NET_IPSTACK)
   int i;

   for(i=0; i<MAX_SOCKET; i++) {
      if(!socketList[i].addr_in.sin_port) {

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
#if defined (__KERNEL_NET_IPSTACK)
   if(!hsock) return;
   ((socket_t*)hsock)->addr_in.sin_port=0;
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
#if defined (__KERNEL_NET_IPSTACK)
   int i;

   for(i = 0; i < UIP_CONNS; ++i) {
      struct socksconn_state * socksconn = (struct socksconn_state *)(uip_conns[i].appstate);
      if( !(socket_t*)socksconn->hsocks
          || (((socket_t*)socksconn->hsocks)->desc<0)
          || (((socket_t*)(socksconn->hsocks))->state==STATE_SOCKET_CLOSED)) continue;

      if( (struct socksconn_state *)socksconn==
          (struct socksconn_state *)((socket_t*)(ofile_lst[desc].ext.hsock))->socksconn)
         return i;
   }
#endif
   return -1;
}

/*-------------------------------------------
| Name:uip_sock
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void uip_sock(void){
#if defined (__KERNEL_NET_IPSTACK)
   int i;
   struct socksconn_state * socksconn = (struct socksconn_state *)(uip_conn->appstate);

   #if UIP_LOGGING==1
   static clock_t start;
   static clock_t finish;
   #endif


   //
   #if UIP_LOGGING==1
   if(socksconn && socksconn->hsocks && uip_flags!=UIP_POLL) {
      printf("\nuip_sock() desc=%d fd=%d",((socket_t*)(socksconn->hsocks))->desc,
             ((socket_t*)(socksconn->hsocks))->fd);
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
      printf("connect\n");
   #endif
      for(i=0; i<MAX_SOCKET; i++) {
         //
         if((socketList[i].state == STATE_SOCKET_LISTEN)
            && uip_conn->lport == socketList[i].addr_in.sin_port) {
            u16_t* paddr;

            //printf("connection request on port : %d\n",ntohs(uip_conn->lport));

            socksconn->hsocks=(hsock_t)&socketList[i];

            paddr=(u16_t*)&((socket_t*)(socksconn->hsocks))->addr_in.sin_addr.s_addr;

            paddr[0]=uip_conn->ripaddr[0];
            paddr[1]=uip_conn->ripaddr[1];

            ((socket_t*)(socksconn->hsocks))->state=STATE_SOCKET_CONNECTED;
            //post event SOCKS_CONNECTED to application data on listen socket
            //__PUT_SOCKET_EVENT(socksconn->hsocks);
            {
               hsock_t hsocks_listen = socksconn->hsocks;
               desc_t desc = ((socket_t*)hsocks_listen)->desc;
               pid_t pid   = ofile_lst[desc].owner_pid;
               int fd      = -1;

               //printf("connection accepted on port : %d\n",uip_conn->lport);

               //get new socket from list
               __set_syscall_owner_pid(pid);
               desc = _vfs_open("/dev/sock",O_RDWR,0);
               fd = _get_fd(pid,0);

               if(fd>=0) {
                  ((socket_t*)ofile_lst[desc].ext.hsock)->fd = fd;
                  process_lst[pid]->desc_tbl[fd]  = desc;
                  socksconn->hsocks = ofile_lst[desc].ext.hsock;
               }else{
                  socksconn->hsocks = 0;
               }

               //
               ((socket_t*)(hsocks_listen))->hsocks=socksconn->hsocks;

               ((socket_t*)(socksconn->hsocks))->socksconn = socksconn;

               ((socket_t*)(socksconn->hsocks))->socksconn->_r=0;
               ((socket_t*)(socksconn->hsocks))->socksconn->_w=0;

               //send hsocks
               ((socket_t*)(socksconn->hsocks))->addr_in.sin_addr.s_addr
                  =((socket_t*)(hsocks_listen))->addr_in.sin_addr.s_addr;

               //
               //((socket_t*)(socksconn->hsocks))->addr_in.sin_port=((socket_t*)(hsocks_listen))->addr_in.sin_port;
               ((socket_t*)(socksconn->hsocks))->addr_in.sin_port=uip_conn->rport;

               ((socket_t*)(socksconn->hsocks))->state=STATE_SOCKET_WAIT;
               //((socket_t*)(hsocks_listen))->state=STATE_SOCKET_LISTEN;//set by accept function
   #if UIP_LOGGING==1
               printf("accepted() i=%d desc=%d on port:%d\n",i,
                      ((socket_t*)(socksconn->hsocks))->desc,
                      ntohs(uip_conn->lport));
   #endif

               __PUT_SOCKET_EVENT(hsocks_listen,O_RDONLY);
            }

            uip_send(uip_appdata, 0);
            break;
         }else if((socketList[i].state == STATE_SOCKET_CONNECT)
                  && uip_conn->rport == socketList[i].addr_in.sin_port) {

            //printf("connection ok on port : %d\n",ntohs(uip_conn->lport));

            socksconn->hsocks=(hsock_t)&socketList[i];
            socksconn->__r = 0;
            socksconn->_r  = 0;
            socksconn->_w  = 0;

            ((socket_t*)(socksconn->hsocks))->state=STATE_SOCKET_WAIT;
            //post event STATE_SOCKET_WAIT ready to send or recv data from remote host
   #if UIP_LOGGING==1
            printf("connected() i=%d desc=%d on port:%d\n",i,
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
      printf(" uip_flags: ");
      switch(uip_flags) {
      case UIP_ACKDATA:
         printf("UIP_ACKDATA");
         break;
      case UIP_NEWDATA:
         printf("UIP_NEWDATA");
         break;
      case UIP_REXMIT:
         printf("UIP_REXMIT");
         break;
      case UIP_POLL:
         printf("UIP_POLL");
         break;
      case UIP_CLOSE:
         printf("UIP_CLOSE");
         break;
      case UIP_ABORT:
         printf("UIP_ABORT");
         break;
      case UIP_CONNECTED:
         printf("UIP_CONNECTED");
         break;
      case UIP_TIMEDOUT:
         printf("UIP_TIMEDOUT");
         break;
      default:
         printf("WARNING UNKNOW FLAG:0x%x",uip_flags);
         break;

      }
      printf("\n");
   }
   #endif


   if(!socksconn->hsocks) {
      uip_send(uip_appdata, 0);
      return;
   }

   //
   if(((socket_t*)(socksconn->hsocks))->state==STATE_SOCKET_ACCEPTED) {
      //to do:lepton: allocate process file descriptor (process_lst[pid]->desc_tbl[fildes])
   }

   if(uip_acked()) {
      //socket buffer size = uip_mss
   #if UIP_LOGGING==1
      {
         double duration;
         desc_t __desc = ((socket_t*)(socksconn->hsocks))->desc;
         finish = clock();
         duration = (double)(finish - start) / CLOCKS_PER_SEC;
         printf( "acked time= %2.1f s\n", duration );
         printf("ackeddata() desc=%d len=%d pid=%d\n",__desc,
                uip_datalen(),ofile_lst[__desc].owner_pid);
         printf("...ackeddata() _r=%d __r=%d w=%d\n",socksconn->_r,socksconn->__r,
                ((socket_t*)(socksconn->hsocks))->w);
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
         printf("newdata() desc=%d len=%d\n",((socket_t*)(socksconn->hsocks))->desc,cb);
   #endif

         if(uip_datalen()<(RCV_SOCKET_BUFFER_SIZE-_w)) {
            memcpy(&socksconn->rcv_buffer[_w],(char*)uip_appdata,uip_datalen());
            _w=_w+uip_datalen();
         }else{
            memcpy(&socksconn->rcv_buffer[_w],(char*)uip_appdata,(RCV_SOCKET_BUFFER_SIZE-_w));
            memcpy(socksconn->rcv_buffer,(char*)uip_appdata+(RCV_SOCKET_BUFFER_SIZE-_w),uip_datalen(
                      )-(RCV_SOCKET_BUFFER_SIZE-_w));
            _w=uip_datalen()-(RCV_SOCKET_BUFFER_SIZE-_w);
         }

         socksconn->_w=_w;

         ((char*)uip_appdata)[cb]=0x00;

   #if UIP_LOGGING==1
         {
            desc_t __desc = ((socket_t*)(socksconn->hsocks))->desc;
            printf("uip_appdata = %s pid=%d\n",uip_appdata,ofile_lst[__desc].owner_pid);
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
         //__lock_io(ofile_lst[desc].owner_pthread_ptr,desc);

         if( uip_rexmit() )
            _r=0;

         _r   = socksconn->_r;
         w    = ((socket_t*)(socksconn->hsocks))->w;

         if(_r==w) {
            //Leave semaphore
            //__unlock_io(ofile_lst[desc].owner_pthread_ptr,desc);
            return;
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

   #if UIP_LOGGING==1
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
         //__unlock_io(ofile_lst[desc].owner_pthread_ptr,desc);
      }

   }

   //
   if(((socket_t*)(socksconn->hsocks))->state==STATE_SOCKET_CLOSE) {
      ((socket_t*)(socksconn->hsocks))->state=STATE_SOCKET_CLOSED;
   #if UIP_LOGGING==1
      printf("close request desc=%d\n",((socket_t*)(socksconn->hsocks))->desc);
   #endif
      uip_close();
      uip_send(uip_appdata,0);
      //__PUT_SOCKET_EVENT(socksconn->hsocks);
   }else if(uip_closed()) {
   #if UIP_LOGGING==1
      printf("uip_closed() desc=%d\n",((socket_t*)(socksconn->hsocks))->desc);
   #endif
      ((socket_t*)(socksconn->hsocks))->state=STATE_SOCKET_CLOSED;
      //__PUT_SOCKET_EVENT(socksconn->hsocks);
      uip_send(uip_appdata,0);
   }else if(uip_aborted()) {
   #if UIP_LOGGING==1
      printf("uip_aborted() desc=%d\n",((socket_t*)(socksconn->hsocks))->desc);
   #endif
      ((socket_t*)(socksconn->hsocks))->state=STATE_SOCKET_ABORTED;
      uip_close();
      uip_send(uip_appdata,0);
      if(socksconn->hsocks)
         __PUT_SOCKET_EVENT(socksconn->hsocks,O_WRONLY);
   }else if(uip_timedout()) {
   #if UIP_LOGGING==1
      printf("uip_timedout() desc=%d\n",((socket_t*)(socksconn->hsocks))->desc);
   #endif
      ((socket_t*)(socksconn->hsocks))->state=STATE_SOCKET_TIMEDOUT;
      uip_close();
      uip_send(uip_appdata,0);
      if(socksconn->hsocks)
         __PUT_SOCKET_EVENT(socksconn->hsocks,O_WRONLY);
   }
#endif
}


/*===========================================
End of SourceOS_SocketEngine.c
=============================================*/
