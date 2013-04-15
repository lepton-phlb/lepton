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

/*===========================================
Compiler Directive
=============================================*/
#ifndef _UIP_SOCKET_H__
#define _UIP_SOCKET_H__



/*===========================================
Includes
=============================================*/
#include "kernel/core/kernelconf.h"


/*===========================================
Declaration
=============================================*/

/** \brief TCP socket type */
#define SOCK_STREAM     1
/** \brief UDP socket type */
#define SOCK_DGRAM      2
/** \brief Provides raw network protocol access */
#define SOCK_RAW        3

/** \brief Unspecified communication domain */
#define AF_UNSPEC       0
/** \brief IPv4 Internet protocols */
#define AF_INET         2
/** \brief IPv6 Internet protocols */
#define AF_INET6        28
/** \brief IPv4 Internet protocols */
#define PF_INET         AF_INET
/** \brief IPv6 internet protocols */
#define PF_INET6        AF_INET6
/** \brief Unspecified domain */
#define PF_UNSPEC       AF_UNSPEC

#define IPPROTO_IP      0
/** \brief TCP protocol to use with \ref SOCK_STREAM */
#define IPPROTO_TCP     6
/** \brief UDP protocol to use with \ref SOCK_DGRAM */
#define IPPROTO_UDP     17

#define INADDR_ANY      0   
#define INADDR_BROADCAST 0xffffffff

/** \brief Flag we can use with send(), recv() and recvfrom(). Nonblocking i/o for this operation only. */
#define MSG_DONTWAIT    0x40
/** \brief Flag we can use with recvfrom(). Peeks at an incoming message, for this operation only. The data is treated as unread and the next recvfrom() or similar function shall still return this data. */
#define MSG_PEEK        0x80

/** \brief shutdown() flag - Disables further receive operations. */
#define SHUT_RD         0
/** \brief shutdown() flag - Disables further send operations. */
#define SHUT_WR         1
/** \brief shutdown() flag - Disables further send and receive operations. */
#define SHUT_RDWR       2


#ifndef UIP_LITTLE_ENDIAN
#define UIP_LITTLE_ENDIAN  3412
#endif /* UIP_LITTLE_ENDIAN */
#ifndef UIP_BIG_ENDIAN
#define UIP_BIG_ENDIAN     1234
#endif /* UIP_BIG_ENDIAN */

//
#ifndef UIP_BYTE_ORDER
#define UIP_BYTE_ORDER UIP_LITTLE_ENDIAN /*see contiki-conf.h*/
#endif

//
#ifndef htons
   #if UIP_BYTE_ORDER == UIP_BIG_ENDIAN
      #pragma message ("htons UIP_BIG_ENDIAN")
      #define htons(n) (n)
   #else /* BYTE_ORDER == BIG_ENDIAN */
      #pragma message ("htons UIP_LITTLE_ENDIAN")
      #define htons(n) ((((uint16_t)((n) & 0xff)) << 8) | (((n) & 0xff00) >> 8))
   #endif /* BYTE_ORDER == BIG_ENDIAN */
#else
#pragma message ("htons already defined")
#endif /* htons */

#define ntohs(n) htons(n)

#define SETUIPSOCKOPT            0x00001

//
struct _in_addr {
   union {
          struct { unsigned char s_b1,s_b2,s_b3,s_b4; }   S_un_b;
          struct { unsigned short s_w1,s_w2; }            S_un_w;
          unsigned long                                   S_addr;
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
    char           sa_data[14];
};
#define sockaddr _sockaddr

//
struct _sockaddr_in{
   short               sin_family;
   unsigned short      sin_port;
   struct   _in_addr   sin_addr;
   char                sin_zero[8];
};
#define sockaddr_in _sockaddr_in

/*
uint8_t s6_addr[16]
This array is used to contain a 128-bit IPv6 address, stored in network byte order.
The <netinet/in.h> header shall define the sockaddr_in6 structure that includes at least the following members:

sa_family_t      sin6_family    AF_INET6. 
in_port_t        sin6_port      Port number. 
uint32_t         sin6_flowinfo  IPv6 traffic class and flow information. 
struct in6_addr  sin6_addr      IPv6 address. 
uint32_t         sin6_scope_id  Set of interfaces for a scope. 

for ipv6
const char  *inet_ntop(int, const void *restrict, char *restrict, socklen_t);
int          inet_pton(int, const char *restrict, void *restrict);
*/

struct _in6_addr{
   uint8_t s6_addr[16];
};
#define in6_addr _in6_addr

struct _sockaddr_in6{
   short               sin6_family;
   unsigned short      sin6_port;
   unsigned long       sin6_flowinfo;  
   struct   _in6_addr  sin6_addr;
   uint32_t            sin6_scope_id;
};
#define sockaddr_in6 _sockaddr_in6

//
extern const struct in6_addr in6addr_any;

//
#ifndef socklen_t
typedef int32_t socklen_t;
#endif

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

int   _sys_sock_recvfrom   (int sock, void *buf, int length, int flags, struct sockaddr *address, socklen_t* address_len);
int   _sys_sock_sendto     (int sock, const void *buf, int length, int flags, const struct sockaddr *dest_addr, socklen_t dest_len);

/** \brief BSD name for internal _uip_inet_addr() */
#define inet_addr(cp) _uip_inet_addr(cp)
/** \brief BSD name for internal _uip_inet_ntoa() */
#define inet_ntoa(cp,in) _uip_inet_ntoa(cp,in)

/** \brief BSD name for internal _sys_sock_socket() */
#define socket(af,type,protocol) _sys_sock_socket(af,type,protocol)
/** \brief BSD name for internal _sys_sock_bind() */
#define bind(sock,address,len) _sys_sock_bind(sock,address,len)
/** \brief BSD name for internal _sys_sock_connect() */
#define connect(sock,address,len) _sys_sock_connect(sock,address,len)
/** \brief BSD name for internal _sys_sock_listen() */
#define listen(sock,maxcon) _sys_sock_listen(sock,maxcon)
/** \brief BSD name for internal _sys_sock_accept() */
#define accept(sock,address,len) _sys_sock_accept(sock,address,len)
/** \brief BSD name for internal _sys_sock_shutdown() */
#define shutdown(sock,how) _sys_sock_shutdown   (sock,how)

#define recv(sock,buf,len,flg) read(sock,buf,len)
#define send(sock,buf,len,flg) write(sock,buf,len)

/** \brief BSD name for internal _sys_sock_recvfrom() */
#define recvfrom(sock,buf,length,flags,address,address_len) _sys_sock_recvfrom(sock,buf,length,flags,address,address_len)
/** \brief BSD name for internal _sys_sock_sendto() */
#define sendto(sock,buf,length,flags,dest_addr,dest_len)    _sys_sock_sendto(sock,buf,length,flags,dest_addr,dest_len)


#endif