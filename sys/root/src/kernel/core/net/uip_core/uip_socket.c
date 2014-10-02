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

/*===========================================
Global Declaration
=============================================*/

extern desc_t uip_core_if_indextodesc(int ifindex, unsigned long oflag);

const struct in6_addr in6addr_any={
   {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}
};

/*===========================================
Implementation
=============================================*/


/**
 * \brief Convert IPv4 address from dotted string to unsigned long. Standard BSD inet_addr() with internal name.
 * \note IPv4 function only.
 * \param[in] cp dotted string IPv4 address.
 * \return returns the IPv4 address is stored in network byte order. On failure, return (unsigned long)-1.
 * \details Example 
    \code  
        struct sockaddr_in addr;
        addr.sin_addr.s_addr = inet_addr("192.168.0.10"); 
    \endcode
 * \callgraph
 */
unsigned long _uip_inet_addr(char *cp){

   unsigned long laddr=0;
   
   uip_ipaddr_t* paddr=(uip_ipaddr_t*)&laddr;

   char saddr[14]={0};
   unsigned char baddr[4]={0};
   char* token;
   int i=-1;

   if(!cp) return 0;

   strcpy(saddr,cp);

   token= strtok(saddr,".");
   
   while(token && i++<4){
      baddr[i] = atoi(token);
      token= strtok(NULL,".");
   }

   uip_ipaddr(paddr, baddr[0],baddr[1],baddr[2],baddr[3]);

   return laddr;
} //end of _uip_inet_addr()

/**
 * \brief Convert a IPv4 socket address to the IPv4 "h1.h2.h3.h4\0" format. Standard BSD inet_ntoa() with internal name.
 * \note IPv4 function only.
 * \param[out] cp string buffer to copy the resulting convertion.
 * \param[in] in address structure containing the IPv4 address.
 * \return \p cp, string with resulting convertion.
 * \callgraph
 */
char* _uip_inet_ntoa(char*cp, struct _in_addr in){

   sprintf(cp,"%d.%d.%d.%d",in.S_un.S_un_b.s_b1,
                            in.S_un.S_un_b.s_b2,
                            in.S_un.S_un_b.s_b3,
                            in.S_un.S_un_b.s_b4);

   return cp;
} //end of _uip_inet_ntoa()

/**
 * \brief Creates a socket. Standard BSD socket() with internal name.
 * \param[in]  af ("familly" or "domain") should only be \ref AF_INET6 (IPv4 support has been disabled so no AF_INET support). Parameter is ignore in current implementation, AF_INET6 is forced.
 * \param[in]  type can be SOCK_STREAM for TCP sockets or SOCK_DGRAM for UDP sockets.
 * \param[in]  protocol can be IPPROTO_TCP with \ref SOCK_STREAM type or IPPROTO_UDP with \ref SOCK_DGRAM with 
 * \details UDP Example \code int desc = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP); \endcode \n
            TCP Example \code int desc = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP); \endcode
 * \return socket() returns a socket file descriptor (\ref desc_t) which is a small non-negative integer. This file descriptor number should be used for all other socket operations on that socket. If socket() encounters an error, it will return -1.
 * \callgraph
 */
int _sys_sock_socket(int af,int type,int protocol){
#if defined (__KERNEL_NET_IPSTACK) && defined (USE_UIP_CORE)
   pid_t pid;
   kernel_pthread_t* pthread_ptr;
   int fd;
   desc_t desc = INVALID_DESC;
   int _protocol;
   hsock_t hsock  = 0;

   //
   if(protocol==IPPROTO_TCP){
      _protocol = IPPROTO_TCP;
   }else if(protocol==IPPROTO_UDP){
      _protocol = IPPROTO_UDP;
   }else if(!protocol && type==SOCK_STREAM){
      _protocol = IPPROTO_TCP;
   }else if(!protocol && type==SOCK_DGRAM){
      _protocol = IPPROTO_UDP;
   }else{
      __set_kernel_pthread_errno(EPROTONOSUPPORT);
      return -1;
   }
   //
   if((fd = open("/dev/sock",O_RDWR,0))<0)
      return -1;
   //
   if(!(pthread_ptr = kernel_pthread_self())){
      __set_kernel_pthread_errno(ESRCH);
      return -1;
   }
   if((pid=pthread_ptr->pid)<=0){
      __set_kernel_pthread_errno(ESRCH);
      return -1;
   }
   //
   desc = process_lst[pid]->desc_tbl[fd];
   if(desc<0)
      return -1;
   //    
   _vfs_ioctl(desc,SETUIPSOCKOPT,_protocol);
   //
   if(! (hsock  = ofile_lst[desc].p) ){
      __set_kernel_pthread_errno (ENOTSOCK);
      return -1;
   }
   //
   if(((socket_t*)hsock)->protocol==IPPROTO_TCP)
      return fd;
   //
   ((socket_t*)hsock)->r =0;
   ((socket_t*)hsock)->w =0;
   //
   ((socket_t*)hsock)->state = STATE_SOCKET_OPENED;
   // 
   ((socket_t*)hsock)->uip_udp_conn = uip_udp_new((void*)0,0);
   //
   if( !((socket_t*)hsock)->uip_udp_conn ){
      close(desc);
      return -1;
   }
      
   //
   ((socket_t*)hsock)->socksconn = (struct socksconn_state *)(((socket_t*)hsock)->uip_udp_conn->appstate.state);
   //
   ((socket_t*)hsock)->socksconn->__r  = 0;
   ((socket_t*)hsock)->socksconn->_r   = 0;
   ((socket_t*)hsock)->socksconn->_w   = 0;
   //
   ((socket_t*)hsock)->socksconn->hsocks = hsock;
   //
   ((socket_t*)hsock)->state = STATE_SOCKET_WAIT;
   //
   __set_kernel_pthread_errno (0);
   //
   return fd;
#else
   return -1;
#endif
} //end of _sys_sock_socket()

/**
 * \brief Binds a socket to a local port and IP address. Standard BSD bind() with internal name.
 * \param[in] desc socket descriptor created with socket()
 * \param[in] address must contain the address and port for the socket to bind to. Must be in IPv6 format with current implementation. Address to zero means port will be openned on every interfaces.
 * \param[in] len is the length of \p address structure. (Unused parameter with current implementation. Assumes IPv6 structures length.)
 * \details bin() must be used in a \b server application that waits for incomming connection/packets.
 * \return On success, zero is returned. On error, -1 is returned.
 * \callgraph
 */
int _sys_sock_bind(int fd, struct _sockaddr *address,int len){
#if defined (__KERNEL_NET_IPSTACK) && defined (USE_UIP_CORE)
   pid_t pid;
   kernel_pthread_t* pthread_ptr;
   hsock_t hsock  = 0;

   #if UIP_CONF_IPV6
   desc_t desc_if;
   #endif

   desc_t desc;
   //
   //
   if(!(pthread_ptr = kernel_pthread_self())){
      __set_kernel_pthread_errno(ESRCH);
      return -1;
   }
   if((pid=pthread_ptr->pid)<=0){
      __set_kernel_pthread_errno(ESRCH);
      return -1;
   }
   //
   if(fd<0){
      __set_kernel_pthread_errno (EBADF);
      return -1;
   }
   //
   desc = process_lst[pid]->desc_tbl[fd];
   if(desc<0){
      __set_kernel_pthread_errno (EBADF);
      return -1;
   }
   if(!ofile_lst[desc].used){
      __set_kernel_pthread_errno (EBADF);
      return -1;
   }
   if(! (hsock  = ofile_lst[desc].p) ){
      __set_kernel_pthread_errno (ENOTSOCK);
      return -1;
   }
   //lock
   __lock_io(ofile_lst[desc].owner_pthread_ptr_write,desc,O_WRONLY);
   __lock_io(ofile_lst[desc].owner_pthread_ptr_read,desc,O_RDONLY);
   //
   #if UIP_CONF_IPV6
      ((socket_t*)hsock)->addr_in.sin6_port = ((struct _sockaddr_in6*)address)->sin6_port; 
      //interface index in list (see uip_core interface list)
      if( ((struct _sockaddr_in6*)address)->sin6_scope_id>=IF_LIST_MAX){
        //unlock
        __set_kernel_pthread_errno (EINVAL);
        //unlock
        __unlock_io(ofile_lst[desc].owner_pthread_ptr_write,desc,O_WRONLY);
        __unlock_io(ofile_lst[desc].owner_pthread_ptr_read,desc,O_RDONLY);
        return -1;
      } 
      //
      ((socket_t*)hsock)->addr_in.sin6_scope_id=((struct _sockaddr_in6*)address)->sin6_scope_id;
      //
      desc_if = uip_core_if_indextodesc(((socket_t*)hsock)->addr_in.sin6_scope_id,O_RDONLY);
      if(desc_if<0){
        __set_kernel_pthread_errno (EINVAL);
        //unlock
        __unlock_io(ofile_lst[desc].owner_pthread_ptr_write,desc,O_WRONLY);
        __unlock_io(ofile_lst[desc].owner_pthread_ptr_read,desc,O_RDONLY);
        return -1;
      }
      /* to do
      if(dev_core_ioctl(desc_if,NETUP)<0){
         __set_kernel_pthread_errno (ENETDOWN);
         //unlock
         __unlock_io(ofile_lst[desc].owner_pthread_ptr_write,desc,O_WRONLY);
         __unlock_io(ofile_lst[desc].owner_pthread_ptr_read,desc,O_RDONLY);
         return -1;
      }
      */
   #else
      ((socket_t*)hsock)->addr_in.sin_port = ((struct _sockaddr_in*)address)->sin_port;
   #endif
   //
   if(((socket_t*)hsock)->protocol==IPPROTO_UDP){
      ((socket_t*)hsock)->uip_udp_conn->rport = 0;//to do: check that
      uip_udp_bind(((socket_t*)hsock)->uip_udp_conn,((struct _sockaddr_in*)address)->sin_port);
   }
   //
   //unlock
   __unlock_io(ofile_lst[desc].owner_pthread_ptr_write,desc,O_WRONLY);
   __unlock_io(ofile_lst[desc].owner_pthread_ptr_read,desc,O_RDONLY);
   //
   __set_kernel_pthread_errno (0);
   //
   return 0;
#else
   return -1;
#endif
} //end of _sys_sock_bind()

/**
 * \brief Connects a socket to a remote host on a given address and port. Standard BSD connect() with internal name.
 * \param[in] desc socket descriptor created with socket()
 * \param[in] address must contain the address and port of the remote host. Must be in IPv6 format with current implementation.
 * \param[in] len is the length of \p address structure. (Unused parameter with current implementation. Assumes IPv6 structures length.)
 * \details connect() must be used in a TCP \b client application. The function call blocks until the connection is established.
 * \return On connection success, zero is returned. On error, -1 is returned.
 * \callgraph
 */
int _sys_sock_connect(int fd, struct _sockaddr *address,int len){
#if defined (__KERNEL_NET_IPSTACK) && defined (USE_UIP_CORE)
   pid_t pid;
   kernel_pthread_t* pthread_ptr;
   struct uip_conn * uip_conn;
   desc_t desc;
   hsock_t hsock  = 0;


   if(!(pthread_ptr = kernel_pthread_self())){
      __set_kernel_pthread_errno(ESRCH);
      return -1;
   }
   if((pid=pthread_ptr->pid)<=0){
      __set_kernel_pthread_errno(ESRCH);
      return -1;
   }
   //
   if(fd<0){
      __set_kernel_pthread_errno (EBADF);
      return -1;
   }
   //
   desc = process_lst[pid]->desc_tbl[fd];
   if(desc<0){
      __set_kernel_pthread_errno (EBADF);
      return -1;
   }
   if(!ofile_lst[desc].used){
      __set_kernel_pthread_errno (EBADF);
      return -1;
   }
   if(! (hsock  = ofile_lst[desc].p) ){
      __set_kernel_pthread_errno (ENOTSOCK);
      return -1;
   }
   if( ((socket_t*)(hsock))->state==STATE_SOCKET_CLOSED){
     __set_kernel_pthread_errno (ENOTCONN);
     return -1;
   }
   if( ((socket_t*)(hsock))->state==STATE_SOCKET_ABORTED){
     __set_kernel_pthread_errno (ECONNABORTED);
     return -1;
   }
   if(((socket_t*)hsock)->protocol!=IPPROTO_TCP){
      __set_kernel_pthread_errno (EPROTONOSUPPORT);
      return -1;
   }
   //lock
   __lock_io(ofile_lst[desc].owner_pthread_ptr_write,desc,O_WRONLY);
   __lock_io(ofile_lst[desc].owner_pthread_ptr_read,desc,O_RDONLY);
   //
   ((socket_t*)hsock)->state = STATE_SOCKET_CONNECT;
   //
   #if UIP_CONF_IPV6
   memcpy(&((socket_t*)hsock)->addr_in,address,sizeof(struct _sockaddr_in6));
   //
   uip_conn = uip_connect((uip_ipaddr_t*)(&((socket_t*)hsock)->addr_in.sin6_addr.s6_addr),
                  (u16_t)( ((socket_t*)hsock)->addr_in.sin6_port));
   #else
   ((socket_t*)hsock)->addr_in.sin_port = ((struct _sockaddr_in*)address)->sin_port;
   ((socket_t*)hsock)->addr_in.sin_addr.s_addr = ((struct _sockaddr_in*)address)->sin_addr.s_addr;
   //
   uip_conn = uip_connect((uip_ipaddr_t*)(&((socket_t*)hsock)->addr_in.sin_addr.s_addr),
                  (u16_t)( ((socket_t*)hsock)->addr_in.sin_port));
   #endif
   
   //
   if(!uip_conn){
      //unlock
      __unlock_io(ofile_lst[desc].owner_pthread_ptr_write,desc,O_WRONLY);
      __unlock_io(ofile_lst[desc].owner_pthread_ptr_read,desc,O_RDONLY);
      return -1;
   }
   //
   ((socket_t*)hsock)->r =0;
   ((socket_t*)hsock)->w =0;
   // 
   ((socket_t*)hsock)->socksconn = (struct socksconn_state *)(uip_conn->appstate.state);
   //
   ((socket_t*)hsock)->socksconn->__r  = 0;
   ((socket_t*)hsock)->socksconn->_r   = 0;
   ((socket_t*)hsock)->socksconn->_w   = 0;
   //
   ((socket_t*)hsock)->socksconn->hsocks = hsock;
   //unlock
   __unlock_io(ofile_lst[desc].owner_pthread_ptr_write,desc,O_WRONLY);
   __unlock_io(ofile_lst[desc].owner_pthread_ptr_read,desc,O_RDONLY);
   
   //to do:lepton
   //Wake ip_stack????
   //to do:lepton: syscall kernel
   //OS_WakeTask(_OS_TASK_TCB(os_ipStack));
   //Wait uip_connected();
    uip_core_queue_put(UIP_POLL_REQUEST,desc,(void*)address,len);
    //
   __WAIT_SOCKET_EVENT(pthread_ptr,hsock);
   if(((socket_t*)hsock)->state != STATE_SOCKET_WAIT)
      return -1;
   //
   __set_kernel_pthread_errno (0);
   //
   return 0;
#else
   return -1;
#endif
} //end of _sys_sock_connect()

/**
 * \brief Start listening to the specified port. Standard BSD listen() with internal name.
 * \param[in] desc socket descriptor to start listening on.
 * \param[in] maxcon is be the maximum TCP connections to accept on that socket. Unsued parameter with current implementation.
 * \details To accept connections, a socket is first created with socket(), 
            a willingness to accept incoming connections is specified with listen(), 
            and then the connections are accepted with accept(). The listen() call applies only to sockets of type \ref SOCK_STREAM (TCP).
 * \note Default maximum number of connections unknown?
 * \return On connection success, zero is returned. On error, -1 is returned.
 * \callgraph
 */
int _sys_sock_listen(int fd,int maxcon){
#if defined (__KERNEL_NET_IPSTACK) && defined (USE_UIP_CORE)
   pid_t pid;
   kernel_pthread_t* pthread_ptr;
   hsock_t hsock  = 0;
   desc_t desc;
   //
   //
   if(!(pthread_ptr = kernel_pthread_self())){
      __set_kernel_pthread_errno(ESRCH);
      return -1;
   }
   if((pid=pthread_ptr->pid)<=0){
      __set_kernel_pthread_errno(ESRCH);
      return -1;
   }
   //
   if(fd<0){
      __set_kernel_pthread_errno (EBADF);
      return -1;
   }
   //
   desc = process_lst[pid]->desc_tbl[fd];
   if(desc<0){
      __set_kernel_pthread_errno (EBADF);
      return -1;
   }
   if(!ofile_lst[desc].used){
      __set_kernel_pthread_errno (EBADF);
      return -1;
   }
   if(! (hsock  = ofile_lst[desc].p) ){
      __set_kernel_pthread_errno (ENOTSOCK);
      return -1;
   }
   if(((socket_t*)hsock)->protocol!=IPPROTO_TCP){
      __set_kernel_pthread_errno (EPROTONOSUPPORT);
      return -1;
   }
   //lock
   __lock_io(ofile_lst[desc].owner_pthread_ptr_write,desc,O_WRONLY);
   __lock_io(ofile_lst[desc].owner_pthread_ptr_read,desc,O_RDONLY);
   //
   ((socket_t*)hsock)->state = STATE_SOCKET_LISTEN;
   #if UIP_CONF_IPV6
   uip_listen( (u16_t)( ((socket_t*)hsock)->addr_in.sin6_port) );
   #else
   uip_listen( (u16_t)( ((socket_t*)hsock)->addr_in.sin_port) );
   #endif
   //unlock
   __unlock_io(ofile_lst[desc].owner_pthread_ptr_write,desc,O_WRONLY);
   __unlock_io(ofile_lst[desc].owner_pthread_ptr_read,desc,O_RDONLY);
   //
   __set_kernel_pthread_errno (0);
   //
   return 0;
#else
   return -1;
#endif
} //end of _sys_sock_listen()

/**
 * \brief Start listening to the specified port. Standard BSD listen() with internal name.
 * \param[in] desc is a socket that has been created with socket(), bound to a local address with bind(), and is listening for connections after a listen(). 
 * \param[out] address is a pointer to a \ref _sockaddr structure. This structure is filled in with the address of the peer socket, as known to the communications layer. The format is always IPv6 in this implementation.
 * \param[in,out] len is a value-result argument: it should initially contain the size of the structure pointed to by \p address; on return it will contain the actual length (in bytes) of the address returned.
 * \details The accept() system call is used with connection-based socket types (\ref SOCK_STREAM only in this implementation). 
            It extracts the first connection request on the queue of pending connections, 
            creates a new connected socket, and returns a new file descriptor referring to that socket. 
            The newly created socket is not in the listening state. The original socket \p desc is unaffected by this call. 
 * \note Details may be added on blocking/non-blocking behavior.
 * \return On connection success, zero is returned. On error, -1 is returned.
 * \callgraph
 */
int _sys_sock_accept(int fd, struct _sockaddr *address,int* len){
#if defined (__KERNEL_NET_IPSTACK) && defined (USE_UIP_CORE)
   pid_t pid;
   kernel_pthread_t* pthread_ptr;
   hsock_t hsock  = 0;
   hsock_t hcsock = 0;
   desc_t desc;
   
   if(!(pthread_ptr = kernel_pthread_self())){
      __set_kernel_pthread_errno(ESRCH);
      return -1;
   }
   if((pid=pthread_ptr->pid)<=0){
      __set_kernel_pthread_errno(ESRCH);
      return -1;
   }
   //
   if(fd<0){
      __set_kernel_pthread_errno (EBADF);
      return -1;
   }
   //
   desc = process_lst[pid]->desc_tbl[fd];
   if(desc<0){
      __set_kernel_pthread_errno (EBADF);
      return -1;
   }
   if(!ofile_lst[desc].used){
      __set_kernel_pthread_errno (EBADF);
      return -1;
   }
   if(! (hsock  = ofile_lst[desc].p) ){
      __set_kernel_pthread_errno (ENOTSOCK);
      return -1;
   }
   if( ((socket_t*)(hsock))->state==STATE_SOCKET_CLOSED){
     __set_kernel_pthread_errno (ENOTCONN);
     return -1;
   }
   if( ((socket_t*)(hsock))->state==STATE_SOCKET_ABORTED){
     __set_kernel_pthread_errno (ECONNABORTED);
     return -1;
   }
   if(((socket_t*)hsock)->protocol!=IPPROTO_TCP){
      __set_kernel_pthread_errno (EPROTONOSUPPORT);
      return -1;
   }
   //Make ready to accept connection
   ((socket_t*)hsock)->state = STATE_SOCKET_LISTEN;
   //wait connected operation from socketEngine
   __WAIT_SOCKET_EVENT(pthread_ptr,hsock);
    if(!ofile_lst[desc].used){
      __set_kernel_pthread_errno (EINVAL);
      return -1;
   }
   if(! (hsock  = ofile_lst[desc].p) ){
      __set_kernel_pthread_errno (ENOTSOCK);
      return -1;
   }
   if(((socket_t*)hsock)->state!=STATE_SOCKET_CONNECTED){
     __set_kernel_pthread_errno (ENOTCONN);
     return -1;
   }
   //((socket_t*)hsock)->state=STATE_SOCKET_ACCEPTED;
   //wait Accepted operation from socketEngine
   //__WAIT_SOCKET_EVENT(hsock);
   //
   hcsock   = ((socket_t*)hsock)->hsocks;
   //
   #if UIP_CONF_IPV6
   memcpy(address,&((socket_t*)hcsock)->addr_in,sizeof(struct _sockaddr_in6));
   //
   *len=sizeof(struct _sockaddr_in6);
   //
   #else
   ((struct _sockaddr_in*)address)->sin_port = ((socket_t*)hcsock)->addr_in.sin_port;
   ((struct _sockaddr_in*)address)->sin_addr.s_addr = ((socket_t*)hcsock)->addr_in.sin_addr.s_addr;
   //
   *len=sizeof(struct _sockaddr_in);
   //
   #endif
   
   /*if(((socket_t*)hsock)->state!=STATE_SOCKET_LISTEN)
      return NULL;*/
   //
   __set_kernel_pthread_errno (0);
   //
   return ((socket_t*)hcsock)->fd;
#else
   return -1;
#endif
} //end of _sys_sock_accept()


/**
 * \brief Receives a packet on a openned socket. Standard BSD recvfrom() with internal name.
 * \param[in]  desc socket descriptor created with socket()
 * \param[out] buf buffer to copy received data
 * \param[in]  length length in bytes of the \p buf buffer
 * \param[in]  flags is formed by OR'ing options. \ref MSG_DONTWAIT and \ref MSG_PEEK supported in current implementation.
 * \param[out] address to get the source address of the received packet
 * \param[out] address_len is unused parameter with current implementation
 * \details Reads data from the remote host specified by \p address into \p buf. 
            The socket must be a \ref SOCK_DGRAM (UDP) socket. \p length should be the size of the buffer.\n
            recvfrom() may not fill the entire buffer.\n\n
            \ref MSG_DONTWAIT This flag enables non-blocking operation (recvfrom() will not block waiting for data).\n
            \ref MSG_PEEK This flag causes the receive operation to return data from the beginning of the receive queue without removing that data from the queue. 
            Thus, a subsequent receive call will return the same data.\n
            In case of multiple sources received on the same socket 
            the \p address parameter will contain the source of the actual 
            dequeued packet (the one in \p buf buffer).
 * \return recvfrom() returns the amount of data which was read. Even with \ref MSG_PEEK flag, only actual amount of data read is returned which may be smaller than packet size (maxed to \p length parameter value). If there is an error, -1 is returned.
 * \callgraph
 */
int _sys_sock_recvfrom(int fd, void *buf, int length, int flags, struct sockaddr *address,socklen_t* address_len){
#if defined (__KERNEL_NET_IPSTACK) && defined (USE_UIP_CORE)
   pid_t pid;
   kernel_pthread_t* pthread_ptr;
   hsock_t hsock  = 0;
   desc_t desc;
   
   #if UIP_CONF_IPV6
   desc_t desc_if;
   #endif

   int r;
   //
   if(!(pthread_ptr = kernel_pthread_self())){
      __set_kernel_pthread_errno(ESRCH);
      return -1;
   }
   if((pid=pthread_ptr->pid)<=0){
      __set_kernel_pthread_errno(ESRCH);
      return -1;
   }
   //
   if(fd<0){
      __set_kernel_pthread_errno (EBADF);
      return -1;
   }
   //
   desc = process_lst[pid]->desc_tbl[fd];
   if(desc<0){
      __set_kernel_pthread_errno (EBADF);
      return -1;
   }
   //
   if(!ofile_lst[desc].used){
      __set_kernel_pthread_errno (EBADF);
      return -1;
   }
   if(! (hsock  = ofile_lst[desc].p) ){
      __set_kernel_pthread_errno (ENOTSOCK);
      return -1;
   }
   if( ((socket_t*)(hsock))->state==STATE_SOCKET_CLOSED){
     __set_kernel_pthread_errno (ENOTCONN);
     return -1;
   }
   if( ((socket_t*)(hsock))->state==STATE_SOCKET_ABORTED){
     __set_kernel_pthread_errno (ECONNABORTED);
     return -1;
   }
   if( ((socket_t*)(hsock))->state==STATE_SOCKET_NETDOWN){
     __set_kernel_pthread_errno (ENETDOWN);
     return -1;
   }
   if(((socket_t*)hsock)->protocol!=IPPROTO_UDP){
      __set_kernel_pthread_errno (EPROTONOSUPPORT);
      return -1;
   }
   //
   #if UIP_CONF_IPV6
   desc_if = uip_core_if_indextodesc(((socket_t*)hsock)->addr_in.sin6_scope_id,O_RDONLY);
   if(desc_if<0){
     __set_kernel_pthread_errno (EINVAL);
     return -1;
   }
   #endif
   /* to do
   if(dev_core_ioctl(desc_if,NETUP)<0){
     __set_kernel_pthread_errno (ENETDOWN);
     return -1;
   }
   */
   //lock
   __lock_io(ofile_lst[desc].owner_pthread_ptr_read,desc,O_RDONLY);
   //Prepare flags
   if(flags & MSG_DONTWAIT)
       ofile_lst[desc].oflag |= O_NONBLOCK; //GD 2011/05/24 //GD 2011/07/05: standard MSG_DONTWAIT flag compliancy (but still a workaround to pass the flag to dev_core_read())
   if(flags & MSG_PEEK)
     ((socket_t*)hsock)->flags |=  MSG_PEEK;
   //unlock
   __unlock_io(ofile_lst[desc].owner_pthread_ptr_read,desc,O_RDONLY);
   //
   if( r=read(fd,buf,length) )
   {
       //
       #if UIP_CONF_IPV6
       memcpy(address,&((socket_t*)hsock)->addr_in_from,sizeof(struct _sockaddr_in6));
       #else
       memcpy( address,&((socket_t*)(hsock))->addr_in_from,sizeof(struct _sockaddr_in));
       #endif   
   }
   if(r<0){
      if( ((socket_t*)(hsock))->state==STATE_SOCKET_CLOSED){
        __set_kernel_pthread_errno (ENOTCONN);
        return -1;
      }
      if( ((socket_t*)(hsock))->state==STATE_SOCKET_ABORTED){
        __set_kernel_pthread_errno (ECONNABORTED);
        return -1;
      }
      if( ((socket_t*)(hsock))->state==STATE_SOCKET_NETDOWN){
        __set_kernel_pthread_errno (ENETDOWN);
        return -1;
      }
      //
      /*UIP_EVENT(EVT_UIP_SOCK + EVT_LVL_ERR + EVT_UIP_SOCK_RECVFROM_FAIL, 
                 desc, &flags, sizeof(flags),
                 "recvfrom() failure (return %d)", r);*/
   }else{
       __set_kernel_pthread_errno(0);
   }
   //Enter Semaphore
   //lock
   __lock_io(ofile_lst[desc].owner_pthread_ptr_read,desc,O_RDONLY);
   //Restore flags
   if(flags & MSG_DONTWAIT)
       ofile_lst[desc].oflag &= ~(O_NONBLOCK);
   if(flags & MSG_PEEK)
     ((socket_t*)hsock)->flags &=  ~(MSG_PEEK);
   //unlock
   __unlock_io(ofile_lst[desc].owner_pthread_ptr_read,desc,O_RDONLY);
   //
   return r;  
#else
   return -1;
#endif
} // end of _sys_sock_recvfrom()

/**
 * \brief Sends a packet to a remote host. Standard BSD sendto() with internal name.
 * \param[in]  desc socket descriptor created with socket()
 * \param[in]  buf buffer with payload to send
 * \param[in]  length length in bytes of the \p buf buffer
 * \param[in]  flags is formed by OR'ing options. No options are supported with current implementation.
 * \param[in]  dest_addr to destination address and port.
 * \param[in]  dest_len should be the length of \p dest_addr structure. Unused parameter with current implementation.
 * \details Writes data to the remote host specified by \p dest_addr into \p buf. 
            The socket must be a \ref SOCK_DGRAM (UDP) socket. 
            \p length should be the amount of data in the buffer. 
 * \return sendto() returns the amount of data which was written. If there is an error, -1 is returned.
 * \callgraph
 */
int _sys_sock_sendto(int fd, const void *buf, int length, int flags, const struct sockaddr *dest_addr,socklen_t dest_len){
#if defined (__KERNEL_NET_IPSTACK) && defined (USE_UIP_CORE)
   pid_t pid;
   kernel_pthread_t* pthread_ptr;
   hsock_t hsock  = 0;
   desc_t desc;

   #if UIP_CONF_IPV6
   desc_t desc_if;
   #endif

   int r;
   //
   if(!(pthread_ptr = kernel_pthread_self())){
      __set_kernel_pthread_errno(ESRCH);
      return -1;
   }
   if((pid=pthread_ptr->pid)<=0){
      __set_kernel_pthread_errno(ESRCH);
      return -1;
   }
   //
   if(fd<0){
      __set_kernel_pthread_errno (EBADF);
      return -1;
   }
   //
   desc = process_lst[pid]->desc_tbl[fd];
   if(desc<0){
      __set_kernel_pthread_errno (EBADF);
      return -1;
   }
   if(!ofile_lst[desc].used){
      __set_kernel_pthread_errno (EBADF);
      return -1;
   }
   if(! (hsock  = ofile_lst[desc].p) ){
      __set_kernel_pthread_errno (ENOTSOCK);
      return -1;
   }
   if( ((socket_t*)(hsock))->state==STATE_SOCKET_CLOSED){
     __set_kernel_pthread_errno (ENOTCONN);
     return -1;
   }
   if( ((socket_t*)(hsock))->state==STATE_SOCKET_ABORTED){
     __set_kernel_pthread_errno (ECONNABORTED);
     return -1;
   }
   if(((socket_t*)hsock)->protocol!=IPPROTO_UDP){
      __set_kernel_pthread_errno (EPROTONOSUPPORT);
      return -1;
   }
   //
   #if UIP_CONF_IPV6
   desc_if = uip_core_if_indextodesc(((socket_t*)hsock)->addr_in.sin6_scope_id,O_RDONLY);
   if(desc_if<0){
     __set_kernel_pthread_errno (EINVAL);
     return -1;
   }
   #endif
   /*to do
   if(dev_core_ioctl(desc_if,NETUP)<0){
     __set_kernel_pthread_errno (ENETDOWN);
     return -1;
   }
   */
   //lock
   __lock_io(ofile_lst[desc].owner_pthread_ptr_write,desc,O_WRONLY);
   //
   #if UIP_CONF_IPV6
   memcpy(&((socket_t*)hsock)->addr_in_to,dest_addr,sizeof(struct _sockaddr_in6));
   #else
   memcpy(&((socket_t*)(hsock))->addr_in_to,dest_addr,sizeof(struct _sockaddr_in));
   #endif
   //unlock
   __unlock_io(ofile_lst[desc].owner_pthread_ptr_write,desc,O_WRONLY);
   //
   r=write(fd,(void*)buf,length);
   if(r<0){
      if( ((socket_t*)(hsock))->state==STATE_SOCKET_CLOSED){
        __set_kernel_pthread_errno (ENOTCONN);
        return -1;
      }
      if( ((socket_t*)(hsock))->state==STATE_SOCKET_ABORTED){
        __set_kernel_pthread_errno (ECONNABORTED);
        return -1;
      }
      if( ((socket_t*)(hsock))->state==STATE_SOCKET_NETDOWN){
        __set_kernel_pthread_errno (ENETDOWN);
        return -1;
      }
      //
      /*UIP_EVENT(EVT_UIP_SOCK + EVT_LVL_ERR + EVT_UIP_SOCK_SENDTO_FAIL, 
                 desc, &length, sizeof(length),
                 "sendto() failure (return %d)(flags %d)", r, flags);*/
   }else{
       __set_kernel_pthread_errno(0);
   }
   return r;
#else
   return -1;
#endif
} //end of _sys_sock_sendto()


/**
 * \brief Closes a socket. Standard BSD close() with internal name.
 * \param[in]  desc socket descriptor to close.
 * \return close() returns 0 on success and -1 on failure.
 * \callgraph
 */
int _sys_sock_close(int fd){
#if defined (__KERNEL_NET_IPSTACK) && defined (USE_UIP_CORE)
   pid_t pid;
   kernel_pthread_t* pthread_ptr;
   hsock_t hsock  = 0;
   desc_t desc;
   int r;
   //
   if(!(pthread_ptr = kernel_pthread_self())){
      __set_kernel_pthread_errno(ESRCH);
      return -1;
   }
   if((pid=pthread_ptr->pid)<=0){
      __set_kernel_pthread_errno(ESRCH);
      return -1;
   }
   //
   if(fd<0){
      __set_kernel_pthread_errno (EBADF);
      return -1;
   }
   //
   desc = process_lst[pid]->desc_tbl[fd];
   if(desc<0){
      __set_kernel_pthread_errno (EBADF);
      return -1;
   }
   if(!ofile_lst[desc].used){
      __set_kernel_pthread_errno (EBADF);
      return -1;
   }
   if(! (hsock  = ofile_lst[desc].p) ){
      __set_kernel_pthread_errno (ENOTSOCK);
      return -1;
   }
   if( ((socket_t*)(hsock))->state==STATE_SOCKET_CLOSED){
     __set_kernel_pthread_errno (ENOTCONN);
     return -1;
   }
   if( ((socket_t*)(hsock))->state==STATE_SOCKET_ABORTED){
     __set_kernel_pthread_errno (ECONNABORTED);
     return -1;
   }
   //
   if( ((socket_t*)hsock)->socksconn!=NULL){
      /*
      while( ((socket_t*)hsock)->w != ((socket_t*)hsock)->socksconn->_r )
         __WAIT_SOCKET_EVENT(hsock);
      */
   }
   //
   if( ((socket_t*)(hsock))->state == STATE_SOCKET_CLOSED 
      || ((socket_t*)(hsock))->state == STATE_SOCKET_ABORTED
      || ((socket_t*)(hsock))->state == STATE_SOCKET_ACCEPTED){
      __CLR_SOCKET_EVENT(hsock);

      close(fd);

      return -1;
   }
   //
   ((socket_t*)(hsock))->state=STATE_SOCKET_CLOSE;
   //
   while( ((socket_t*)(hsock))->state==STATE_SOCKET_WAIT )
      __WAIT_SOCKET_EVENT(pthread_ptr,hsock);
   //
   if(!ofile_lst[desc].used){
      __set_kernel_pthread_errno (EINVAL);
      return -1;
   }
   if(! (hsock  = ofile_lst[desc].p) ){
      __set_kernel_pthread_errno (ENOTSOCK);
      return -1;
   }
   //
   __CLR_SOCKET_EVENT(hsock);
   //
   if(((socket_t*)hsock)->protocol==IPPROTO_UDP){
     //free udp connection
     ((socket_t*)hsock)->uip_udp_conn->lport = 0;
   }
   //
   r=close(fd);
   //
   if(!r)
     __set_kernel_pthread_errno(0);
   //
   return r;
#else
   return -1;
#endif
} //end of '_sys_sock_close'

/**
 * \brief Closes a socket. Standard BSD shutdown() with internal name.
 * \param[in]  desc socket descriptor to close.
 * \param[in]  how Shutdown method : \ref SHUT_RD, \ref SHUT_WR or \ref SHUT_RDWR. 
 * \note Similar to "close()" BSD function when using \ref SHUT_RDWR method (no need to call close()).
 * \note Only \ref SHUT_RDWR is supported with current implementation.
 * \return shutdown() returns 0 on success and -1 on failure.
 * \callgraph
 */
int _sys_sock_shutdown(int fd, int how){
#if defined (__KERNEL_NET_IPSTACK) && defined (USE_UIP_CORE)
   kernel_pthread_t* pthread_ptr;
   //
   if(!(pthread_ptr = kernel_pthread_self())){
      __set_kernel_pthread_errno(ESRCH);
      return -1;
   }
   //
   if(how != SHUT_RDWR){
      __set_kernel_pthread_errno (EOPNOTSUPP);
      return -1; //only SHUT_RDWR is supported with current implementation
   }
   //
   return(_sys_sock_close(fd));
#else
   return -1;
#endif
} //end of _sys_sock_shutdown()


/** @} */

/*===========================================
End of SourceOS_Socket.c
=============================================*/