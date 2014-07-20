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

#include "kernel/core/types.h"
#include "kernel/core/interrupt.h"
#include "kernel/core/syscall.h"
#include "kernel/core/kernelconf.h"
#include "kernel/core/kernel.h"
#include "kernel/core/process.h"
#include "kernel/core/kernel_pthread.h"
#include "kernel/core/errno.h"

#include "kernel/core/devio.h"

#include "kernel/fs/vfs/vfs.h"

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
#endif

#include "kernel/core/net/uip_core/uip_sock.h"
#include "kernel/core/net/uip_core/uip_socket.h"
#include "kernel/core/net/uip_core/uip_core.h"

#if UIP_LOGGING!=1
#include "lib/libc/stdio/stdio.h"
#else
#pragma message ("warning uip 1.0 logging")
#endif

#define __set_kernel_pthread_errno(__errno__) (__kernel_pthread_errno=__errno__)


/*============================================
| Global Declaration 
==============================================*/

static const char dev_sock_name[]="sock";

static int dev_sock_load(void);
static int dev_sock_open(desc_t desc,int o_flag);
static int dev_sock_close(desc_t desc);
static int dev_sock_isset_read(desc_t desc);
static int dev_sock_isset_write(desc_t desc);
static int dev_sock_read(desc_t desc,char* buffer,int nbyte);
static int dev_sock_write(desc_t desc,const char* buffer,int nbyte);
static int dev_sock_seek(desc_t desc,int offset,int origin);
static int dev_sock_ioctl(desc_t desc,int request,va_list ap);


dev_map_t dev_sock_map={
   dev_sock_name,
   S_IFCHR,
   dev_sock_load,
   dev_sock_open,
   dev_sock_close,
   dev_sock_isset_read,
   dev_sock_isset_write,
   dev_sock_read,
   dev_sock_write,
   dev_sock_seek,
   dev_sock_ioctl //ioctl
};

/*============================================
| Implementation 
==============================================*/

/*-------------------------------------------
| Name:dev_sock_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int dev_sock_load(void){
   return uip_sock_init();
}

/*-------------------------------------------
| Name:dev_sock_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int dev_sock_open(desc_t desc,int o_flag){
   hsock_t hsock = sock_get();

   if(!hsock)
      return -1;

   ((socket_t*)hsock)->desc = desc;

   ofile_lst[desc].p = hsock;
   return 0;
}

/*-------------------------------------------
| Name:dev_sock_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int dev_sock_close(desc_t desc){
   hsock_t hsock = ofile_lst[desc].p;

   if(!hsock)
      return -1;

   if( (ofile_lst[desc].oflag & O_RDONLY) 
      && ofile_lst[desc].oflag & O_WRONLY){

      if(!ofile_lst[desc].nb_reader && !ofile_lst[desc].nb_writer){
         sock_put(hsock);
         ofile_lst[desc].p = (hsock_t)0;
      }
   }
   
   return 0;
}

/*-------------------------------------------
| Name: dev_sock_isset_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int dev_sock_isset_read(desc_t desc){
   hsock_t hsock = ofile_lst[desc].p;
   int r= ((socket_t*)hsock)->r;
   int _w=((socket_t*)hsock)->socksconn->_w;
   
   //
   if( (((socket_t*)(hsock))->state!=STATE_SOCKET_WAIT))
      return 0;
   //
   if(r!=_w)//recv data ok.
      return 0;

   return -1;
}

/*-------------------------------------------
| Name: dev_sock_isset_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int dev_sock_isset_write(desc_t desc){
   hsock_t hsock = ofile_lst[desc].p;
   int w= ((socket_t*)hsock)->w;
   int _r=((socket_t*)hsock)->socksconn->_r;
   int free_size;
   //
   if( (((socket_t*)(hsock))->state!=STATE_SOCKET_WAIT)){
      return 0;
   }
   //
   if((free_size=(_r-w))<=0)
       free_size=SND_SOCKET_BUFFER_SIZE+free_size; 
   //
   if(w==_r){//send data ok.
      return 0;
   }
   //
   return -1;
}

/*-------------------------------------------
| Name:dev_sock_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int dev_sock_read(desc_t desc,char* buffer,int nbyte ){
 int cb=0;
   hsock_t hsock = ofile_lst[desc].p;
//   socket_recvfrom_header_t* p_socket_recvfrom_header;
   int len = 0;
   int* plen;
   int r = 0;
   int _w = 0;

   if(!hsock)
      return -1;

   r= ((socket_t*)hsock)->r;
   _w=((socket_t*)hsock)->socksconn->_w;
   
   //
   if( ((socket_t*)(hsock))->state==STATE_SOCKET_CLOSED){
     return -1;
   }
   if( ((socket_t*)(hsock))->state==STATE_SOCKET_ABORTED){
     return -1;
   }
   if( ((socket_t*)(hsock))->state==STATE_SOCKET_NETDOWN){
     return -1;
   }
   //
   if( (r==_w) && (((socket_t*)(hsock))->state!=STATE_SOCKET_WAIT))
      return -1;
   //if MSG_DONTWAIT and socket buffer empty return 0
   if(r==_w)
       return 0;
   // For UDP datagram, get the packet header from the ring buffer
   if(((socket_t*)hsock)->protocol==IPPROTO_UDP){  
      //begin udp: get header
      cb = sizeof(socket_recvfrom_header_t);//32bytes for v6, 20bytes for v4
      //Case where both sock_addr and packet length are in sequence
      if((r+cb)<RCV_SOCKET_BUFFER_SIZE){
         //Start with source address strcuture
         memcpy(&((socket_t*)(hsock))->addr_in_from, &((socket_t*)hsock)->socksconn->rcv_buffer[r], sizeof(((socket_t*)(hsock))->addr_in_from));
         r = r + sizeof(((socket_t*)(hsock))->addr_in_from);
         //Then go with length
         plen = (int*)&((socket_t*)hsock)->socksconn->rcv_buffer[r];
         len = *plen;
         r=r+sizeof(int);
      }else{
          cb = sizeof(((socket_t*)(hsock))->addr_in_from); //_sockaddr_in6 is 28bytes, _sockaddr_in is 16bytes
          //Then case where sock_addr fits, but not the length
          if((r+cb)<RCV_SOCKET_BUFFER_SIZE){
              //Start with source address strcuture
              memcpy(&((socket_t*)(hsock))->addr_in_from, &((socket_t*)hsock)->socksconn->rcv_buffer[r], cb);
              r=r+cb;
              //Then go with length
              memcpy(&len, &((socket_t*)hsock)->socksconn->rcv_buffer[r], RCV_SOCKET_BUFFER_SIZE-r);
              memcpy(((char*)&len)+(RCV_SOCKET_BUFFER_SIZE-r), ((socket_t*)hsock)->socksconn->rcv_buffer, sizeof(int)-(RCV_SOCKET_BUFFER_SIZE-r));
              r=sizeof(int)-(RCV_SOCKET_BUFFER_SIZE-r);
          }else{
              //Start with source address strcuture
              memcpy(&((socket_t*)(hsock))->addr_in_from, &((socket_t*)hsock)->socksconn->rcv_buffer[r], RCV_SOCKET_BUFFER_SIZE-r);
              memcpy(((char*)&((socket_t*)(hsock))->addr_in_from)+(RCV_SOCKET_BUFFER_SIZE-r), ((socket_t*)hsock)->socksconn->rcv_buffer, cb-(RCV_SOCKET_BUFFER_SIZE-r));
              r = cb-(RCV_SOCKET_BUFFER_SIZE-r);
              //Then go with length
              plen = (int*)&((socket_t*)hsock)->socksconn->rcv_buffer[r];
              len = *plen;
              r=r+sizeof(int);
          }
      }
      //get packet len
      cb = len;
      //end udp
   }else{
       if(r<=_w){
           cb = _w-r;
       }else{
           cb = (RCV_SOCKET_BUFFER_SIZE-r)+_w;
       }   
   }
   
   // Trap any internal issue from ring buffer 
   if(cb<0){
      return -1;
   }
   
   if(nbyte<=cb)
      cb=nbyte;
   //get packet
   if((r+cb)<RCV_SOCKET_BUFFER_SIZE){
      memcpy(buffer,&((socket_t*)hsock)->socksconn->rcv_buffer[r],cb);
      r=r+cb;
   }else{
      memcpy(buffer,&((socket_t*)hsock)->socksconn->rcv_buffer[r],RCV_SOCKET_BUFFER_SIZE-r);
      memcpy(buffer+(RCV_SOCKET_BUFFER_SIZE-r),((socket_t*)hsock)->socksconn->rcv_buffer, cb-(RCV_SOCKET_BUFFER_SIZE-r));
      r=cb-(RCV_SOCKET_BUFFER_SIZE-r);
   }
   //
   if( (((socket_t*)hsock)->flags&MSG_PEEK) )
     return cb;
   //go to next packet
   ((socket_t*)hsock)->r=r;
   return cb;
}

/*-------------------------------------------
| Name:dev_sock_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int dev_sock_write(desc_t desc,const char* buffer,int nbyte ){
   int cb=0;
//   netsend_t netsend;
   hsock_t hsock = ofile_lst[desc].p;
   int w;

   if(!hsock)
      return -1;

   
   #if UIP_LOGGING==1
   ANNOTATE("0 ready to send\r\n");
   #endif

   if(((socket_t*)hsock)->state!=STATE_SOCKET_WAIT)
      return -1;

   if(nbyte>=SND_SOCKET_BUFFER_SIZE)
      cb=SND_SOCKET_BUFFER_SIZE-1;
   else
      cb=nbyte;

   //lock, already lock in write().
   //__lock_io(ofile_lst[desc].owner_pthread_ptr_write,desc,O_WRONLY);
   //
   w  = ((socket_t*)hsock)->w;
   #if UIP_LOGGING==1
   ANNOTATE("1)desc=%d dev_sock_write w=%d _r=%d\r\n",desc,w,((socket_t*)hsock)->socksconn->_r);
   #endif

   if((w+cb)<SND_SOCKET_BUFFER_SIZE){
      memcpy(&((socket_t*)hsock)->socksconn->snd_buffer[w],buffer,cb);
      w=w+cb;
   }else{
      memcpy(&((socket_t*)hsock)->socksconn->snd_buffer[w],buffer,SND_SOCKET_BUFFER_SIZE-w);
      memcpy(((socket_t*)hsock)->socksconn->snd_buffer,(char*)&buffer[SND_SOCKET_BUFFER_SIZE-w],cb-(SND_SOCKET_BUFFER_SIZE-w));
      w=cb-(SND_SOCKET_BUFFER_SIZE-w);
   }

   ((socket_t*)hsock)->w = w;
   #if UIP_LOGGING==1
   ANNOTATE("2)desc=%d dev_sock_write w=%d _r=%d\r\n",desc,w,((socket_t*)hsock)->socksconn->_r);
   #endif

   //unlock
   //__unlock_io(ofile_lst[desc].owner_pthread_ptr_write,desc,O_WRONLY);


   //Wake ip_stack
   //to do:lepton: syscall kernel
   //OS_WakeTask(_OS_TASK_TCB(os_ipStack));
//   netsend.desc = desc;
   //__mk_uip_core_syscall(_SYSCALL_NET_SND,netsend);
   uip_core_queue_put(UIP_TIMER,desc,(void*)buffer,nbyte);
   //

   return cb;
}

/*-------------------------------------------
| Name:dev_sock_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int dev_sock_seek(desc_t desc,int offset,int origin){
   return 0;
}

/*-------------------------------------------
| Name:dev_sock_ioctl
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int dev_sock_ioctl(desc_t desc,int request,va_list ap){
   hsock_t hsock = ofile_lst[desc].p;
   //
   if(!hsock)
      return -1;
   //
   switch(request){
   
      case SETUIPSOCKOPT:{
         int v = va_arg( ap, int);
         switch(v){
            case IPPROTO_UDP:
            case IPPROTO_TCP:
            ((socket_t*)hsock)->protocol=v;
            return 0;
         }//
         
      }//
   
   }
    
    return -1;
}


/*============================================
| End of Source  : uip_dev_sock.c
==============================================*/