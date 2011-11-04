/*
The contents of this file are subject to the Mozilla Public License Version 1.1 
(the "License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS" basis, 
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License for the 
specific language governing rights and limitations under the License.

The Original Code is ______________________________________.

The Initial Developer of the Original Code is ________________________.
Portions created by ______________________ are Copyright (C) ______ _______________________.
All Rights Reserved.

Contributor(s): ______________________________________.

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
| Compiler Directive
==============================================*/
#ifndef _LWIP_SOCKET_H
#define _LWIP_SOCKET_H


/*============================================
| Includes
==============================================*/
#include "lwip/ip_addr.h"
#include "lwip/tcpip.h"

/*============================================
| Declaration
==============================================*/


/* members are in network byte order */
struct sockaddr_in {
  u8_t sin_len;
  u8_t sin_family;
  u16_t sin_port;
  struct in_addr sin_addr;
  char sin_zero[8];
};

struct sockaddr {
  u8_t sa_len;
  u8_t sa_family;
  char sa_data[14];
};

#ifndef socklen_t
   #define socklen_t u32_t
#endif

/* Socket protocol types (TCP/UDP/RAW) */
#define SOCK_STREAM     1
#define SOCK_DGRAM      2
#define SOCK_RAW        3

/*
 * Option flags per-socket. These must match the SOF_ flags in ip.h!
 */
#define  SO_DEBUG       0x0001 /* Unimplemented: turn on debugging info recording */
#define  SO_ACCEPTCONN  0x0002 /* socket has had listen() */
#define  SO_REUSEADDR   0x0004 /* Unimplemented: allow local address reuse */
#define  SO_KEEPALIVE   0x0008 /* keep connections alive */
#define  SO_DONTROUTE   0x0010 /* Unimplemented: just use interface addresses */
#define  SO_BROADCAST   0x0020 /* Unimplemented: permit sending of broadcast msgs */
#define  SO_USELOOPBACK 0x0040 /* Unimplemented: bypass hardware when possible */
#define  SO_LINGER      0x0080 /* linger on close if data present */
#define  SO_OOBINLINE   0x0100 /* Unimplemented: leave received OOB data in line */
#define  SO_REUSEPORT   0x0200 /* Unimplemented: allow local address & port reuse */

#define SO_DONTLINGER   ((int)(~SO_LINGER))

/*
 * Additional options, not kept in so_options.
 */
#define SO_SNDBUF    0x1001    /* Unimplemented: send buffer size */
#define SO_RCVBUF    0x1002    /* receive buffer size */
#define SO_SNDLOWAT  0x1003    /* Unimplemented: send low-water mark */
#define SO_RCVLOWAT  0x1004    /* Unimplemented: receive low-water mark */
#define SO_SNDTIMEO  0x1005    /* Unimplemented: send timeout */
#define SO_RCVTIMEO  0x1006    /* receive timeout */
#define SO_ERROR     0x1007    /* get error status and clear */
#define SO_TYPE      0x1008    /* get socket type */
#define SO_CONTIMEO  0x1009    /* Unimplemented: connect timeout */
#define SO_NO_CHECK  0x100a    /* don't create UDP checksum */


/*
 * Structure used for manipulating linger option.
 */
struct linger {
       int l_onoff;                /* option on/off */
       int l_linger;               /* linger time */
};

/*
 * Level number for (get/set)sockopt() to apply to socket itself.
 */
#define  SOL_SOCKET  0xfff    /* options for socket level */


#define AF_UNSPEC       0
#define AF_INET         2
#define PF_INET         AF_INET
#define PF_UNSPEC       AF_UNSPEC

#define IPPROTO_IP      0
#define IPPROTO_TCP     6
#define IPPROTO_UDP     17
#define IPPROTO_UDPLITE 136

//#define INADDR_ANY       0
//#define INADDR_BROADCAST 0xffffffff

/* Flags we can use with send and recv. */
#define MSG_PEEK       0x01    /* Peeks at an incoming message */
#define MSG_WAITALL    0x02    /* Unimplemented: Requests that the function block until the full amount of data requested can be returned */
#define MSG_OOB        0x04    /* Unimplemented: Requests out-of-band data. The significance and semantics of out-of-band data are protocol-specific */
#define MSG_DONTWAIT   0x08    /* Nonblocking i/o for this operation only */
#define MSG_MORE       0x10    /* Sender will send more */


/*
 * Options for level IPPROTO_IP
 */
#define IP_TOS             1
#define IP_TTL             2

#if LWIP_TCP
/*
 * Options for level IPPROTO_TCP
 */
#define TCP_NODELAY    0x01    /* don't delay send to coalesce packets */
#define TCP_KEEPALIVE  0x02    /* send KEEPALIVE probes when idle for pcb->keep_idle milliseconds */
#define TCP_KEEPIDLE   0x03    /* set pcb->keep_idle  - Same as TCP_KEEPALIVE, but use seconds for get/setsockopt */
#define TCP_KEEPINTVL  0x04    /* set pcb->keep_intvl - Use seconds for get/setsockopt */
#define TCP_KEEPCNT    0x05    /* set pcb->keep_cnt   - Use number of probes sent for get/setsockopt */
#endif /* LWIP_TCP */

#if LWIP_UDP && LWIP_UDPLITE
/*
 * Options for level IPPROTO_UDPLITE
 */
#define UDPLITE_SEND_CSCOV 0x01 /* sender checksum coverage */
#define UDPLITE_RECV_CSCOV 0x02 /* minimal receiver checksum coverage */
#endif /* LWIP_UDP && LWIP_UDPLITE*/


#if LWIP_IGMP
/*
 * Options and types for UDP multicast traffic handling
 */
#define IP_ADD_MEMBERSHIP  3
#define IP_DROP_MEMBERSHIP 4
#define IP_MULTICAST_TTL   5
#define IP_MULTICAST_IF    6
#define IP_MULTICAST_LOOP  7

typedef struct ip_mreq {
    struct in_addr imr_multiaddr; /* IP multicast address of group */
    struct in_addr imr_interface; /* local IP address of interface */
} ip_mreq;
#endif /* LWIP_IGMP */

/* Unimplemented for now... */
#define IPTOS_TOS_MASK          0x1E
#define IPTOS_TOS(tos)          ((tos) & IPTOS_TOS_MASK)
#define IPTOS_LOWDELAY          0x10
#define IPTOS_THROUGHPUT        0x08
#define IPTOS_RELIABILITY       0x04
#define IPTOS_LOWCOST           0x02
#define IPTOS_MINCOST           IPTOS_LOWCOST

/*
 * Definitions for IP precedence (also in ip_tos) (Unimplemented)
 */
#define IPTOS_PREC_MASK                 0xe0
#define IPTOS_PREC(tos)                ((tos) & IPTOS_PREC_MASK)
#define IPTOS_PREC_NETCONTROL           0xe0
#define IPTOS_PREC_INTERNETCONTROL      0xc0
#define IPTOS_PREC_CRITIC_ECP           0xa0
#define IPTOS_PREC_FLASHOVERRIDE        0x80
#define IPTOS_PREC_FLASH                0x60
#define IPTOS_PREC_IMMEDIATE            0x40
#define IPTOS_PREC_PRIORITY             0x20
#define IPTOS_PREC_ROUTINE              0x00


/*
 * Commands for ioctlsocket(),  taken from the BSD file fcntl.h.
 * lwip_ioctl only supports FIONREAD and FIONBIO, for now
 *
 * Ioctl's have the command encoded in the lower word,
 * and the size of any in or out parameters in the upper
 * word.  The high 2 bits of the upper word are used
 * to encode the in/out status of the parameter; for now
 * we restrict parameters to at most 128 bytes.
 */
#if !defined(FIONREAD) || !defined(FIONBIO)
#define IOCPARM_MASK    0x7fU           /* parameters must be < 128 bytes */
#define IOC_VOID        0x20000000UL    /* no parameters */
#define IOC_OUT         0x40000000UL    /* copy out parameters */
#define IOC_IN          0x80000000UL    /* copy in parameters */
#define IOC_INOUT       (IOC_IN|IOC_OUT)
                                        /* 0x20000000 distinguishes new &
                                           old ioctl's */
#define _IO(x,y)        (IOC_VOID|((x)<<8)|(y))

#define _IOR(x,y,t)     (IOC_OUT|(((long)sizeof(t)&IOCPARM_MASK)<<16)|((x)<<8)|(y))

#define _IOW(x,y,t)     (IOC_IN|(((long)sizeof(t)&IOCPARM_MASK)<<16)|((x)<<8)|(y))
#endif /* !defined(FIONREAD) || !defined(FIONBIO) */

//see kernel/core/ioctl.h
#if 0
   #ifndef FIONREAD
   #define FIONREAD    _IOR('f', 127, unsigned long) /* get # bytes to read */
   #endif
   #ifndef FIONBIO
   #define FIONBIO     _IOW('f', 126, unsigned long) /* set/clear non-blocking i/o */
   #endif
#endif

/* Socket I/O Controls: unimplemented */
#ifndef SIOCSHIWAT
#define SIOCSHIWAT  _IOW('s',  0, unsigned long)  /* set high watermark */
#define SIOCGHIWAT  _IOR('s',  1, unsigned long)  /* get high watermark */
#define SIOCSLOWAT  _IOW('s',  2, unsigned long)  /* set low watermark */
#define SIOCGLOWAT  _IOR('s',  3, unsigned long)  /* get low watermark */
#define SIOCATMARK  _IOR('s',  7, unsigned long)  /* at oob mark? */
#endif

/*Struture for gethostbyname*/
struct hostent {
    char  *h_name;      /* Official name of the host. */
    char **h_aliases;   /* A pointer to an array of pointers to alternative host names,
                           terminated by a null pointer. */
    int    h_addrtype;  /* Address type. */
    int    h_length;    /* The length, in bytes, of the address. */
    char **h_addr_list; /* A pointer to an array of pointers to network addresses (in
                           network byte order) for the host, terminated by a null pointer. */
#define h_addr h_addr_list[0] /* for backward compatibility */
};



int _sys_lwip_accept(int fd, struct sockaddr *addr, socklen_t *addrlen);
int _sys_lwip_bind(int fd, struct sockaddr *name, socklen_t namelen);
int _sys_lwip_shutdown(int fd, int how);
int _sys_lwip_getpeername (int fd, struct sockaddr *name, socklen_t *namelen);
int _sys_lwip_getsockname (int fd, struct sockaddr *name, socklen_t *namelen);
int _sys_lwip_getsockopt (int fd, int level, int optname, void *optval, socklen_t *optlen);
int _sys_lwip_setsockopt (int fd, int level, int optname, const void *optval, socklen_t optlen);
int _sys_lwip_connect(int fd, struct sockaddr *name, socklen_t namelen);
int _sys_lwip_listen(int fd, int backlog);
int _sys_lwip_recv(int fd, void *mem, int len, unsigned int flags);
int _sys_lwip_recvfrom(int fd, void *mem, int len, unsigned int flags,
      struct sockaddr *from, socklen_t *fromlen);
int _sys_lwip_send(int fd, const void *dataptr, int size, unsigned int flags);
int _sys_lwip_sendto(int fd, void *dataptr, int size, unsigned int flags,
    struct sockaddr *to, socklen_t tolen);
int _sys_lwip_socket(int domain, int type, int protocol);
int _sys_lwip_select(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset,
                struct timeval *timeout);
int _sys_lwip_ioctl(int fd, long cmd, void *argp);
struct hostent* _sys_lwip_gethostbyname(const char *name);


#if LWIP_COMPAT_SOCKETS
#define accept(a,b,c)         _sys_lwip_accept(a,b,c)
#define bind(a,b,c)           _sys_lwip_bind(a,b,c)
#define shutdown(a,b)         _sys_lwip_shutdown(a,b)
//#define close(s)              _sys_lwip_close(s)
#define connect(a,b,c)        _sys_lwip_connect(a,b,c)
#define getsockname(a,b,c)    _sys_lwip_getsockname(a,b,c)
#define getpeername(a,b,c)    _sys_lwip_getpeername(a,b,c)
#define setsockopt(a,b,c,d,e) _sys_lwip_setsockopt(a,b,c,d,e)
#define getsockopt(a,b,c,d,e) _sys_lwip_getsockopt(a,b,c,d,e)
#define listen(a,b)           _sys_lwip_listen(a,b)
#define recv(a,b,c,d)         _sys_lwip_recv(a,b,c,d)
//#define read(a,b,c)           _sys_lwip_read(a,b,c)
#define recvfrom(a,b,c,d,e,f) _sys_lwip_recvfrom(a,b,c,d,e,f)
#define send(a,b,c,d)         _sys_lwip_send(a,b,c,d)
#define sendto(a,b,c,d,e,f)   _sys_lwip_sendto(a,b,c,d,e,f)
#define socket(a,b,c)         _sys_lwip_socket(a,b,c)
//#define write(a,b,c)          _sys_lwip_write(a,b,c)
//#define select(a,b,c,d,e)     _sys_lwip_select(a,b,c,d,e)
#define ioctlsocket(a,b,c)    _sys_lwip_ioctl(a,b,c)
#define gethostbyname(a)      _sys_lwip_gethostbyname(a)
#endif /* LWIP_COMPAT_SOCKETS */


#endif
