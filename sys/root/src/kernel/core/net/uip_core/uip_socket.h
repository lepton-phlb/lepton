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
#ifndef _OS_SOCKET_H
#define _OS_SOCKET_H



/*===========================================
Includes
=============================================*/
#include "kernel/core/kernelconf.h"
#include "kernel/fs/vfs/vfsdev.h"

#pragma message ("uip 1.0")
#include "kernel/net/uip1.0/net/uipopt.h"

//
//


#include "kernel/core//net/socks.h"
//#include "uip.h"

/*===========================================
Declaration
=============================================*/

//to do: must be defined correctly/compatibility
#define SOCK_STREAM     1
#define SOCK_DGRAM      2
#define SOCK_RAW        3

#define AF_UNSPEC       0
#define AF_INET         2
#define PF_INET         AF_INET
#define PF_UNSPEC       AF_UNSPEC

#define IPPROTO_IP      0
#define IPPROTO_TCP     6
#define IPPROTO_UDP     17

#define INADDR_ANY      0
#define INADDR_BROADCAST 0xffffffff

/* Flags we can use with send and recv. */
#define MSG_DONTWAIT    0x40            /* Nonblocking i/o for this operation only */


//
#ifndef htons
   #if UIP_BYTE_ORDER == UIP_BIG_ENDIAN
//#pragma message ("htons UIP_BIG_ENDIAN")
      #define htons(n) (n)
   #else /* BYTE_ORDER == BIG_ENDIAN */
         //#pragma message ("htons UIP_LITTLE_ENDIAN")
      #define htons(n) ((((u16_t)((n) & 0xff)) << 8) | (((n) & 0xff00) >> 8))
   #endif /* BYTE_ORDER == BIG_ENDIAN */
#else
   #pragma message ("htons already defined")
#endif /* htons */

#define ntohs(n) htons(n)

//
struct _in_addr {
   union {
      struct { unsigned char s_b1,s_b2,s_b3,s_b4; }   S_un_b;
      struct { unsigned short s_w1,s_w2; }            S_un_w;
      unsigned long S_addr;
   } S_un;

#define s_addr  S_un.S_addr
   /* can be used for most tcp & ip code */
#define s_host  S_un.S_un_b.s_b2
   /* host on imp */
#define s_net   S_un.S_un_b.s_b1
   /* network */
#define s_imp   S_un.S_un_w.s_w2
   /* imp */
#define s_impno S_un.S_un_b.s_b4
   /* imp # */
#define s_lh    S_un.S_un_b.s_b3
   /* logical host */
};

//
struct _sockaddr {
   unsigned short sa_family;
   char sa_data[14];
};
#define sockaddr _sockaddr

//
struct _sockaddr_in {
   short sin_family;
   unsigned short sin_port;
   struct   _in_addr sin_addr;
   char sin_zero[8];
};
#define sockaddr_in _sockaddr_in

//
unsigned long  _uip_inet_addr(char *cp);
char*          _uip_inet_ntoa(char*cp, struct _in_addr in);

//
int   _sys_sock_socket     (int af,int type,int protocol);
int   _sys_sock_bind       (int sock, struct _sockaddr *address,int len);
int   _sys_sock_connect    (int sock, struct _sockaddr *address,int len);
int   _sys_sock_listen     (int sock, int maxcon);
int   _sys_sock_accept     (int sock, struct _sockaddr *address,int* len);
int   _sys_sock_shutdown   (int sock, int how);

#define inet_addr(cp) _uip_inet_addr(cp)
#define inet_ntoa(cp,in) _uip_inet_ntoa(cp,in)

#define socket(af,type,protocol) _sys_sock_socket(af,type,protocol)
#define bind(sock,address,len) _sys_sock_bind(sock,address,len)
#define connect(sock,address,len) _sys_sock_connect(sock,address,len)
#define listen(sock,maxcon) _sys_sock_listen(sock,maxcon)
#define accept(sock,address,len) _sys_sock_accept(sock,address,len)
#define shutdown(sock,how) _sys_sock_shutdown   (sock,how)

#define recv(sock,buf,len,flg) read(sock,buf,len)
#define send(sock,buf,len,flg) write(sock,buf,len)


/*
#define inet_addr _inet_addr
#define inet_ntoa _inet_ntoa

#define socket _sys_sock_socket
#define bind _sys_sock_bind
#define connect _sys_sock_connect
#define listen _sys_sock_listen
#define accept _sys_sock_accept
#define shutdown _sys_sock_shutdown

#define recv(sock,buf,len,flg) read(sock,buf,len)
#define send(sock,buf,len,flg) write(sock,buf,len)
*/

extern dev_map_t dev_sock_map;

#endif
