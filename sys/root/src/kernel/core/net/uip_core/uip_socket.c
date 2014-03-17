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
#include <stdlib.h>

#include "kernel/core/types.h"
#include "kernel/core/interrupt.h"
#include "kernel/core/syscall.h"
#include "kernel/core/kernelconf.h"
#include "kernel/core/kernel.h"
#include "kernel/core/process.h"

#include "kernel/core/devio.h"

#include "kernel/fs/vfs/vfs.h"

#pragma message ("uip 1.0")
#include "kernel/net/uip1.0/net/uip.h"

#include "kernel/core/net/uip_core/uip_sock.h"

#include "kernel/core/net/uip_core/uip_socket.h"

#include "kernel/core/net/uip_core/uip_core.h"

#if UIP_LOGGING!=1
   #include "lib/libc/stdio/stdio.h"
#else
   #pragma message ("warning uip 1.0 logging")
#endif


/*===========================================
Global Declaration
=============================================*/
int _sys_sock_load(void);
int _sys_sock_open(desc_t desc,int o_flag);
int _sys_sock_close(desc_t desc);
int _sys_sock_isset_read(desc_t desc);
int _sys_sock_isset_write(desc_t desc);
int _sys_sock_read(desc_t desc,char* buffer,int nbyte );
int _sys_sock_write(desc_t desc,const char* buffer,int nbyte );
int _sys_sock_seek(desc_t desc,int offset,int origin);


const char _sys_sock_name[]="sock";

dev_map_t dev_sock_map={
   _sys_sock_name,
   S_IFCHR,
   _sys_sock_load,
   _sys_sock_open,
   _sys_sock_close,
   _sys_sock_isset_read,
   _sys_sock_isset_write,
   _sys_sock_read,
   _sys_sock_write,
   _sys_sock_seek,
   __fdev_not_implemented //ioctl
};


/*===========================================
Implementation
=============================================*/
/*-------------------------------------------
| Name:_uip_inet_addr
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
unsigned long _uip_inet_addr(char *cp){

   unsigned long laddr=0;
   u16_t* paddr=(u16_t*)&laddr;
   char saddr[14]={0};
   unsigned char baddr[4]={0};
   char* token;
   int i=-1;

   if(!cp) return 0;

   strcpy(saddr,cp);

   token= strtok(saddr,".");

   while(token && i++<4) {

      if(!token)
         return -1;
      baddr[i] = atoi(token);
      token= strtok(NULL,".");
   }

   uip_ipaddr(paddr, baddr[0],baddr[1],baddr[2],baddr[3]);

   return laddr;
}

/*-------------------------------------------
| Name:_uip_inet_ntoa
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
char* _uip_inet_ntoa(char*cp, struct _in_addr in){

   sprintf(cp,"%d.%d.%d.%d",in.S_un.S_un_b.s_b1,
           in.S_un.S_un_b.s_b2,
           in.S_un.S_un_b.s_b3,
           in.S_un.S_un_b.s_b4);

   return cp;
}

/*-------------------------------------------
| Name:_sys_socket
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _sys_sock_socket(int af,int type,int protocol){ //socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
   int fd = open("/dev/sock",O_RDWR,0);
   //to do:lepton: for option PF_INET,SOCK_STREAM,IPPROTO_TCP use ioctl(). not yet implemented.

   return fd;
}

/*-------------------------------------------
| Name:_sys_sock_bind
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _sys_sock_bind(int sock, struct _sockaddr *address,int len){
#if defined (__KERNEL_NET_IPSTACK)
   kernel_pthread_t* pthread_ptr;
   pid_t pid      = 0;
   desc_t desc    = -1;
   hsock_t hsock  = 0;

   if(sock<0)
      return -1;

   if(!(pthread_ptr = kernel_pthread_self()))
      return -1;

   if((pid= pthread_ptr->pid)<1)
      return -1;

   desc    = process_lst[pid]->desc_tbl[sock];
   if(desc<0)
      return -1;

   if(!(hsock  = ofile_lst[desc].ext.hsock) )
      return -1;

   //Enter Semaphore
   __lock_io(pthread_ptr,((socket_t*)hsock)->desc,O_RDONLY);

   ((socket_t*)hsock)->addr_in.sin_port = ((struct _sockaddr_in*)address)->sin_port;

   //Leave Semaphore
   __unlock_io(pthread_ptr,((socket_t*)hsock)->desc,O_RDONLY);
#endif
   return 0;
}

/*-------------------------------------------
| Name:_sys_sock_connect
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _sys_sock_connect(int sock, struct _sockaddr *address,int len){
#if defined (__KERNEL_NET_IPSTACK)
   struct uip_conn * uip_conn;

   kernel_pthread_t* pthread_ptr;
   pid_t pid      = 0;
   desc_t desc    = -1;
   hsock_t hsock  = 0;

   if(sock<0)
      return -1;

   if(!(pthread_ptr = kernel_pthread_self()))
      return -1;

   if((pid= pthread_ptr->pid)<1)
      return -1;

   desc    = process_lst[pid]->desc_tbl[sock];
   if(desc<0)
      return -1;

   if(!(hsock  = ofile_lst[desc].ext.hsock) )
      return -1;


   __lock_io(pthread_ptr,((socket_t*)hsock)->desc,O_RDONLY);
   ((socket_t*)hsock)->state = STATE_SOCKET_CONNECT;

   ((socket_t*)hsock)->addr_in.sin_port = ((struct _sockaddr_in*)address)->sin_port;
   ((socket_t*)hsock)->addr_in.sin_addr.s_addr = ((struct _sockaddr_in*)address)->sin_addr.s_addr;

   uip_conn = uip_connect((uip_ipaddr_t*)(&((socket_t*)hsock)->addr_in.sin_addr.s_addr),
                          (u16_t)( ((socket_t*)hsock)->addr_in.sin_port));

   ((socket_t*)hsock)->r =0;
   ((socket_t*)hsock)->w =0;
   ((socket_t*)hsock)->socksconn = (struct socksconn_state *)(uip_conn->appstate);
   ((socket_t*)hsock)->socksconn->__r  = 0;
   ((socket_t*)hsock)->socksconn->_r   = 0;
   ((socket_t*)hsock)->socksconn->_w   = 0;

   ((socket_t*)hsock)->socksconn->hsocks = hsock;

   //Leave Semaphore
   __unlock_io(pthread_ptr,((socket_t*)hsock)->desc,O_RDONLY);

   //to do:lepton
   //Wake ip_stack????
   //to do:lepton: syscall kernel
   //OS_WakeTask(_OS_TASK_TCB(os_ipStack));

   //Wait uip_connected();
   __WAIT_SOCKET_EVENT(pthread_ptr,hsock);
   if(((socket_t*)hsock)->state != STATE_SOCKET_WAIT)
      return -1;
#endif
   return 0;
}

/*-------------------------------------------
| Name:_sys_sock_listen
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _sys_sock_listen(int sock,int maxcon){
#if defined (__KERNEL_NET_IPSTACK)
   pid_t pid      = 0;
   desc_t desc    = -1;
   hsock_t hsock  = 0;

   kernel_pthread_t* pthread_ptr;

   if(sock<0)
      return -1;

   if(!(pthread_ptr = kernel_pthread_self()))
      return -1;

   if((pid= pthread_ptr->pid)<1)
      return -1;

   desc    = process_lst[pid]->desc_tbl[sock];
   if(desc<0)
      return -1;

   if(!(hsock  = ofile_lst[desc].ext.hsock) )
      return -1;

   //Enter Semaphore
   __lock_io(pthread_ptr,((socket_t*)hsock)->desc,O_RDONLY);
   ((socket_t*)hsock)->state = STATE_SOCKET_LISTEN;
   uip_listen( (u16_t)( ((socket_t*)hsock)->addr_in.sin_port) );
   //Leave Semaphore
   __unlock_io(pthread_ptr,((socket_t*)hsock)->desc,O_RDONLY);
#endif
   return 0;
}

/*-------------------------------------------
| Name:_sys_sock_accept
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _sys_sock_accept(int sock, struct _sockaddr *address,int* len){
#if defined (__KERNEL_NET_IPSTACK)
   hsock_t hsock  = 0;
   hsock_t hcsock = 0;
   kernel_pthread_t* pthread_ptr;
   pid_t pid      = 0;
   desc_t desc    = -1;

   if(sock<0)
      return -1;

   if(!(pthread_ptr = kernel_pthread_self()))
      return -1;

   if((pid= pthread_ptr->pid)<1)
      return -1;

   desc    = process_lst[pid]->desc_tbl[sock];

   if(desc<0)
      return -1;

   if(!(hsock  = ofile_lst[desc].ext.hsock) )
      return -1;

   //Make ready to accept connection
   ((socket_t*)hsock)->state = STATE_SOCKET_LISTEN;

   //wait connected operation from socketEngine
   __WAIT_SOCKET_EVENT(pthread_ptr,hsock);
   if(((socket_t*)hsock)->state!=STATE_SOCKET_CONNECTED)
      return -1;

   //((socket_t*)hsock)->state=STATE_SOCKET_ACCEPTED;

   //wait Accepted operation from socketEngine
   //__WAIT_SOCKET_EVENT(hsock);

   hcsock   = ((socket_t*)hsock)->hsocks;


   ((struct _sockaddr_in*)address)->sin_port = ((socket_t*)hcsock)->addr_in.sin_port;
   ((struct _sockaddr_in*)address)->sin_addr.s_addr = ((socket_t*)hcsock)->addr_in.sin_addr.s_addr;

   *len=sizeof(struct _sockaddr_in);

   /*if(((socket_t*)hsock)->state!=STATE_SOCKET_LISTEN)
      return NULL;*/
   return ((socket_t*)hcsock)->fd;
#else
   return -1;
#endif

}

/*-------------------------------------------
| Name:_sys_sock_shutdown
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _sys_sock_shutdown(int sock, int how){
#if defined (__KERNEL_NET_IPSTACK)
   netsend_t netsend;
   pid_t pid      = 0;
   kernel_pthread_t* pthread_ptr;
   desc_t desc    = -1;
   hsock_t hsock  = 0;

   if(sock<0)
      return -1;

   if(!(pthread_ptr = kernel_pthread_self()))
      return -1;

   if((pid= pthread_ptr->pid)<1)
      return -1;

   desc    = process_lst[pid]->desc_tbl[sock];

   if(desc<0)
      return -1;

   if(!(hsock  = ofile_lst[desc].ext.hsock) )
      return -1;

   if( ((socket_t*)hsock)->socksconn!=NULL) {
      /*
      while( ((socket_t*)hsock)->w != ((socket_t*)hsock)->socksconn->_r )
         __WAIT_SOCKET_EVENT(hsock);
      */
   }

   if( ((socket_t*)(hsock))->state ==STATE_SOCKET_CLOSED ||
       ((socket_t*)(hsock))->state == STATE_SOCKET_ACCEPTED) {
      __CLR_SOCKET_EVENT(hsock);

      close(sock);

      return 0;
   }

   ((socket_t*)(hsock))->state=STATE_SOCKET_CLOSE;

   //Wake ip_stack
   //to do:lepton: syscall kernel
   //OS_WakeTask(_OS_TASK_TCB(os_ipStack));
   netsend.desc = desc;
   __mk_uip_core_syscall(_SYSCALL_NET_SND,netsend);

   while( ((socket_t*)(hsock))->state==STATE_SOCKET_WAIT )
      __WAIT_SOCKET_EVENT(pthread_ptr,hsock);

   __CLR_SOCKET_EVENT(hsock);

   close(sock);
#endif
   return 0;
}

/*-------------------------------------------
| Name:_sys_sock_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _sys_sock_load(void){
   return uip_sock_init();
}

/*-------------------------------------------
| Name:_sys_sock_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _sys_sock_open(desc_t desc,int o_flag){
   hsock_t hsock = sock_get();

   if(!hsock)
      return -1;

   ((socket_t*)hsock)->desc = desc;

   ofile_lst[desc].ext.hsock = hsock;
   return 0;
}

/*-------------------------------------------
| Name:_pipe_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _sys_sock_close(desc_t desc){
   hsock_t hsock = ofile_lst[desc].ext.hsock;

   if(!hsock)
      return -1;

   if( (ofile_lst[desc].oflag & O_RDONLY)
       && ofile_lst[desc].oflag & O_WRONLY) {

      if(!ofile_lst[desc].nb_reader && !ofile_lst[desc].nb_writer) {
         sock_put(hsock);
         ofile_lst[desc].ext.hsock = (hsock_t)0;
      }

   }

   return 0;
}

/*-------------------------------------------
| Name:_sys_sock_isset_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _sys_sock_isset_read(desc_t desc){
   hsock_t hsock = ofile_lst[desc].ext.hsock;
   int r= ((socket_t*)hsock)->r;
   int _w=((socket_t*)hsock)->socksconn->_w;

   //
   if( (((socket_t*)(hsock))->state!=STATE_SOCKET_WAIT))
      return 0;
   //
   if(r!=_w) //recv data ok.
      return 0;

   return -1;
}

/*-------------------------------------------
| Name:_sys_sock_isset_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _sys_sock_isset_write(desc_t desc){
   hsock_t hsock = ofile_lst[desc].ext.hsock;
   int w= ((socket_t*)hsock)->w;
   int _r=((socket_t*)hsock)->socksconn->_r;

   //
#if UIP_LOGGING==1
   {
      printf("desc=%d isset_write\n",desc);
   }
#endif

   if( (((socket_t*)(hsock))->state!=STATE_SOCKET_WAIT)) {
#if UIP_LOGGING==1
      {
         printf("state!= STATE_SOCKET_WAIT w=%d r=%d\n",w,_r);
      }
#endif
      return 0;
   }
   //
   if(w==_r) { //send data ok.
#if UIP_LOGGING==1
      {
         printf("w==r  w=%d r=%d\n",w,_r);
      }
#endif
      return 0;
   }

   return -1;
}

/*-------------------------------------------
| Name:_sys_sock_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _sys_sock_read(desc_t desc,char* buffer,int nbyte ){
   int cb=0;
#if defined (__KERNEL_NET_IPSTACK)
   hsock_t hsock = ofile_lst[desc].ext.hsock;
   int r = 0;
   int _w = 0;

   if(!hsock)
      return -1;

   r= ((socket_t*)hsock)->r;
   _w=((socket_t*)hsock)->socksconn->_w;

   if( (r==_w) && (((socket_t*)(hsock))->state!=STATE_SOCKET_WAIT))
      return -1;

   //
   if(r<=_w) {
      cb = _w-r;
   }else{
      cb = (RCV_SOCKET_BUFFER_SIZE-r)+_w;
   }

   if(nbyte<=cb)
      cb=nbyte;

   if((r+cb)<RCV_SOCKET_BUFFER_SIZE) {
      memcpy(buffer,&((socket_t*)hsock)->socksconn->rcv_buffer[r],cb);
      r=r+cb;
   }else{
      memcpy(buffer,&((socket_t*)hsock)->socksconn->rcv_buffer[r],RCV_SOCKET_BUFFER_SIZE-r);
      memcpy(buffer+(RCV_SOCKET_BUFFER_SIZE-r),((socket_t*)hsock)->socksconn->rcv_buffer,_w);
      r=cb-(RCV_SOCKET_BUFFER_SIZE-r);
   }

   ((socket_t*)hsock)->r=r;
#endif
   return cb;
}

/*-------------------------------------------
| Name:_sys_sock_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _sys_sock_write(desc_t desc,const char* buffer,int nbyte ){
   int cb=0;
#if defined (__KERNEL_NET_IPSTACK)
   netsend_t netsend;
   hsock_t hsock = ofile_lst[desc].ext.hsock;
   int w;

   if(!hsock)
      return -1;


   #if UIP_LOGGING==1
   printf("0 ready to send\r\n");
   #endif

   if(((socket_t*)hsock)->state!=STATE_SOCKET_WAIT)
      return -1;

   if(nbyte>=SND_SOCKET_BUFFER_SIZE)
      cb=SND_SOCKET_BUFFER_SIZE-1;
   else
      cb=nbyte;

   //Enter semaphore
   //__lock_io(ofile_lst[desc].owner_pthread_ptr,((socket_t*)hsock)->desc);

   w  = ((socket_t*)hsock)->w;
   #if UIP_LOGGING==1
   printf("1)desc=%d _sys_sock_write w=%d _r=%d\r\n",desc,w,((socket_t*)hsock)->socksconn->_r);
   #endif

   if((w+cb)<SND_SOCKET_BUFFER_SIZE) {
      memcpy(&((socket_t*)hsock)->socksconn->snd_buffer[w],buffer,cb);
      w=w+cb;
   }else{
      memcpy(&((socket_t*)hsock)->socksconn->snd_buffer[w],buffer,SND_SOCKET_BUFFER_SIZE-w);
      memcpy(((socket_t*)hsock)->socksconn->snd_buffer,
             (char*)&buffer[SND_SOCKET_BUFFER_SIZE-w],cb-(SND_SOCKET_BUFFER_SIZE-w));
      w=cb-(SND_SOCKET_BUFFER_SIZE-w);
   }

   ((socket_t*)hsock)->w = w;
   #if UIP_LOGGING==1
   printf("2)desc=%d _sys_sock_write w=%d _r=%d\r\n",desc,w,((socket_t*)hsock)->socksconn->_r);
   #endif


   //Leave semaphore
   //__unlock_io(ofile_lst[desc].owner_pthread_ptr,((socket_t*)hsock)->desc);

   //Wake ip_stack
   //to do:lepton: syscall kernel
   //OS_WakeTask(_OS_TASK_TCB(os_ipStack));
   netsend.desc = desc;
   __mk_uip_core_syscall(_SYSCALL_NET_SND,netsend);
#endif
   return cb;
}

/*-------------------------------------------
| Name:_sys_sock_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _sys_sock_seek(desc_t desc,int offset,int origin){
   return 0;
}





/*===========================================
End of SourceOS_Socket.c
=============================================*/
