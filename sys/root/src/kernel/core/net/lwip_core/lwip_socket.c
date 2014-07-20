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
#include <string.h>

#include "lwip/api.h"
#include "lwip/sys.h"
#include "lwip/igmp.h"
#include "lwip/inet.h"
#include "lwip/tcp.h"
#include "lwip/raw.h"
#include "lwip/udp.h"
#include "lwip/tcpip.h"


#include "kernel/core/types.h"
#include "kernel/core/interrupt.h"
#include "kernel/core/syscall.h"
#include "kernel/core/kernelconf.h"
#include "kernel/core/kernel.h"
#include "kernel/core/process.h"
#include "kernel/core/devio.h"
#include "kernel/core/ioctl.h"
#include "kernel/fs/vfs/vfs.h"

#include "kernel/core/net/lwip_core/lwip_socket.h"


/*============================================
| Global Declaration
==============================================*/
#define NUM_SOCKETS 10 //MEMP_NUM_NETCONN

typedef unsigned long uuid_sock_conn_t;

typedef struct {
   struct netconn *conn;
   struct netbuf *lastdata;
   u16_t lastoffset;
   u16_t rcvevent;
   u16_t sendevent;
   u16_t flags;
   int err;
   desc_t desc;
   uuid_sock_conn_t uuid_sock_conn;
}lwip_sock_t;

static lwip_sock_t _lwip_sock_list[NUM_SOCKETS];

/** This struct is used to pass data to the set/getsockopt_internal
 * functions running in tcpip_thread context (only a void* is allowed) */
struct lwip_setgetsockopt_data_st {
   /** socket struct for which to change options */
   lwip_sock_t *sock;
   /** socket index for which to change options */
   int fd;
   /** level of the option to process */
   int level;
   /** name of the option to process */
   int optname;
   /** set: value to set the option to
     * get: value of the option is stored here */
   void *optval;
   /** size of *optval */
   socklen_t *optlen;
   /** if an error occures, it is temporarily stored here */
   err_t err;
} lwip_setgetsockopt_data_t;


static sys_sem_t socksem = 0;
static sys_sem_t selectsem = 0;

static void _sys_lwip_sock_event_callback(struct netconn *conn, enum netconn_evt evt, u16_t len);
static int  _sys_lwip_lowlevel_write(lwip_sock_t *sock, const void *data, int size,
                                     unsigned int flags);
static int  _sys_lwip_lowlevel_writeto(lwip_sock_t *sock, void *data, int size, unsigned int flags,
                                       struct sockaddr *to,
                                       socklen_t tolen);



//*----------------------
static const int err_to_errno_table[15] = {
   0,            // ERR_OK          0      No error, everything OK.
   ENOMEM,       // ERR_MEM        -1      Out of memory error.
   ENOBUFS,      // ERR_BUF        -2      Buffer error.
   EHOSTUNREACH, // ERR_RTE        -3      Routing problem.
   ECONNABORTED, // ERR_ABRT       -4      Connection aborted.
   ECONNRESET,   // ERR_RST        -5      Connection reset.
   ESHUTDOWN,    // ERR_CLSD       -6      Connection closed.
   ENOTCONN,     // ERR_CONN       -7      Not connected.
   EINVAL,       // ERR_VAL        -8      Illegal value.
   EIO,          // ERR_ARG        -9      Illegal argument.
   EADDRINUSE,   // ERR_USE        -10     Address in use.
   -1,           // ERR_IF         -11     Low-level netif error.
   -1,           // ERR_ISCONN     -12     Already connected.
   ETIMEDOUT,    // ERR_TIMEOUT    -13     Timeout.
   EINPROGRESS   // ERR_INPROGRESS -14     Operation in progress.
};

#define ERR_TO_ERRNO_TABLE_SIZE \
   (sizeof(err_to_errno_table)/sizeof(err_to_errno_table[0]))

#define err_to_errno(err) \
   ((unsigned)(-(err)) < ERR_TO_ERRNO_TABLE_SIZE ? \
    err_to_errno_table[-(err)] : EIO)

#ifdef ERRNO
   #define set_errno(err) errno = (err)
#else
   #define set_errno(err)
#endif

#define sock_set_errno(sk, e) do { \
      sk->err = (e); \
      set_errno(sk->err); \
} while (0)

int dev_lwip_sock_load(void);
int dev_lwip_sock_open(desc_t desc,int o_flag);
int dev_lwip_sock_close(desc_t desc);
int dev_lwip_sock_isset_read(desc_t desc);
int dev_lwip_sock_isset_write(desc_t desc);
int dev_lwip_sock_read(desc_t desc,char* buffer,int nbyte );
int dev_lwip_sock_write(desc_t desc,const char* buffer,int nbyte );
int dev_lwip_sock_seek(desc_t desc,int offset,int origin);
int dev_lwip_sock_ioctl(desc_t desc,int request,va_list ap);


const char dev_lwip_sock_name[]="net/sock";

dev_map_t dev_lwip_sock_map={
   dev_lwip_sock_name,
   S_IFCHR,
   dev_lwip_sock_load,
   dev_lwip_sock_open,
   dev_lwip_sock_close,
   dev_lwip_sock_isset_read,
   dev_lwip_sock_isset_write,
   dev_lwip_sock_read,
   dev_lwip_sock_write,
   dev_lwip_sock_seek,
   dev_lwip_sock_ioctl //ioctl
};

#define __sys_lwip_desc2sock(__desc__) ((lwip_sock_t *)(ofile_lst[__desc__].ext.hsock))

static uuid_sock_conn_t g_uuid_sock_conn=0;

/*============================================
| Implementation
==============================================*/

/*--------------------------------------------
| Name:        _sys_lwip_socket_init
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int _sys_lwip_socket_init(void){
   int i;
   for(i = 0; i < NUM_SOCKETS; ++i) {
      _lwip_sock_list[i].desc    = INVALID_DESC;
      _lwip_sock_list[i].conn    = (struct netconn *)0;
   }

   return 0;
}

/*--------------------------------------------
| Name:        _lwip_alloc_socket
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
static hsock_t _sys_lwip_alloc_socket(desc_t desc){
   int i;

   if (!socksem)
      socksem = sys_sem_new(1);

   /* Protect socket array */
   sys_sem_wait(socksem);

   /* allocate a new socket identifier */
   for(i = 0; i < NUM_SOCKETS; ++i) {
      if (_lwip_sock_list[i].desc == INVALID_DESC) {
         _lwip_sock_list[i].desc = desc;
         _lwip_sock_list[i].lastdata = NULL;
         _lwip_sock_list[i].lastoffset = 0;
         _lwip_sock_list[i].rcvevent = 0;
         _lwip_sock_list[i].sendevent = 1; /* TCP send buf is empty */
         _lwip_sock_list[i].flags = 0;
         _lwip_sock_list[i].err = 0;

         //to remove:debug
         _lwip_sock_list[i].uuid_sock_conn=++g_uuid_sock_conn;
         //

         sys_sem_signal(socksem);
         return (hsock_t)&_lwip_sock_list[i];
      }
   }
   sys_sem_signal(socksem);
   return (hsock_t)0;
}

/*--------------------------------------------
| Name:        _sys_lwip_free_socket
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
static int _sys_lwip_free_socket(desc_t desc){

   lwip_sock_t* sock;

   /* Protect socket array */
   sys_sem_wait(socksem);
   sock = (lwip_sock_t *)(ofile_lst[desc].ext.hsock);
   if(!sock)
      return -1;
   //
   sock->desc = INVALID_DESC;
   sock->conn = (struct netconn *)0;

   sys_sem_signal(socksem);
   return 0;
}

/*--------------------------------------------
| Name:        _sys_lwip_attach_sock2conn
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int _sys_lwip_attach_sock2conn(desc_t desc,struct netconn * conn){

   lwip_sock_t* sock;

   if(desc<0)
      return -1;

   sock = (lwip_sock_t *)(ofile_lst[desc].ext.hsock);
   if(sock==(lwip_sock_t*)0)
      return -1;

   sock->conn = conn;
   conn->socket = desc;

   return 0;
}

/*--------------------------------------------
| Name:        _sys_lwip_get_sock
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
lwip_sock_t* _sys_lwip_get_sock(int fd){

   pid_t pid;
   kernel_pthread_t* pthread_ptr;
   desc_t desc;

   lwip_sock_t* sock;

   if(!(pthread_ptr = kernel_pthread_self()))
      return (lwip_sock_t*)0;

   if((pid= pthread_ptr->pid)<1)
      return (lwip_sock_t*)0;

   if((desc = process_lst[pid]->desc_tbl[fd])<0)
      return (lwip_sock_t*)0;

   if( !(sock = (lwip_sock_t *)(ofile_lst[desc].ext.hsock)) )
      return (lwip_sock_t*)0;

   //update oflag
   if(ofile_lst[desc].oflag&O_NONBLOCK)
      sock->flags |= O_NONBLOCK;
   else
      sock->flags &= (~O_NONBLOCK);


   return sock;
}

/*--------------------------------------------
| Name:        _sys_lwip_socket
| Description:
| Parameters:  none
| Return Type: none
| Comments:    lwip 1.2.0
| See:
----------------------------------------------*/
int _sys_lwip_socket(int domain, int type, int protocol){

   struct netconn *conn;
   int fd;
   pid_t pid;
   kernel_pthread_t* pthread_ptr;
   desc_t desc;

   if(!(pthread_ptr = kernel_pthread_self()))
      return -1;

   if((pid= pthread_ptr->pid)<1)
      return -1;

   /* create a netconn */
   switch (type) {
   case SOCK_RAW:
      conn = netconn_new_with_proto_and_callback(NETCONN_RAW, protocol,
                                                 _sys_lwip_sock_event_callback);
      LWIP_DEBUGF(SOCKETS_DEBUG,
                  ("lwip_socket(%s, SOCK_RAW, %d) = ", domain == PF_INET ? "PF_INET" : "UNKNOWN",
                   protocol));
      break;
   case SOCK_DGRAM:
      conn = netconn_new_with_callback(NETCONN_UDP, _sys_lwip_sock_event_callback);
      LWIP_DEBUGF(SOCKETS_DEBUG,
                  ("lwip_socket(%s, SOCK_DGRAM, %d) = ", domain == PF_INET ? "PF_INET" : "UNKNOWN",
                   protocol));
      break;
   case SOCK_STREAM:
      conn = netconn_new_with_callback(NETCONN_TCP, _sys_lwip_sock_event_callback);
      LWIP_DEBUGF(SOCKETS_DEBUG,
                  ("lwip_socket(%s, SOCK_STREAM, %d) = ", domain == PF_INET ? "PF_INET" : "UNKNOWN",
                   protocol));
      break;
   default:
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_socket(%d, %d/UNKNOWN, %d) = -1\n", domain, type, protocol));
      set_errno(EINVAL);
      return -1;
   }

   if (!conn) {
      LWIP_DEBUGF(SOCKETS_DEBUG, ("-1 / ENOBUFS (could not create netconn)\n"));
      set_errno(ENOBUFS);
      return -1;
   }

   //alloc a newsock structure
   if( (fd=open("/dev/net/sock",O_RDWR,0)) <0) {
      netconn_delete(conn);
      set_errno(ENOBUFS);
      return -1;
   }

   //get internal desc
   if((desc = process_lst[pid]->desc_tbl[fd])<0) {
      netconn_delete(conn);
      set_errno(ENOBUFS);
      return -1;
   }
   conn->socket = desc;
   _sys_lwip_attach_sock2conn(desc,conn);

   //

   LWIP_DEBUGF(SOCKETS_DEBUG, ("%d\n", fd));
   set_errno(0);
   return fd;
}

/*--------------------------------------------
| Name:        _sys_lwip_bind
| Description:
| Parameters:  none
| Return Type: none
| Comments:    lwip 1.3.0
| See:
----------------------------------------------*/
int _sys_lwip_bind(int fd, struct sockaddr *name, socklen_t namelen){

   lwip_sock_t *sock;
   struct ip_addr local_addr;
   u16_t local_port;
   err_t err;

   sock = _sys_lwip_get_sock(fd);
   if (!sock) {
      set_errno(EBADF);
      return -1;
   }

   LWIP_ERROR("lwip_bind: invalid address", ((namelen == sizeof(struct sockaddr_in)) &&
                                             ((((struct sockaddr_in *)name)->sin_family) == AF_INET)),
              sock_set_errno(sock, err_to_errno(ERR_ARG)); return -1; );

   local_addr.addr = ((struct sockaddr_in *)name)->sin_addr.s_addr;
   local_port = ((struct sockaddr_in *)name)->sin_port;

   LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_bind(%d, addr=", sock));
   ip_addr_debug_print(SOCKETS_DEBUG, &local_addr);
   LWIP_DEBUGF(SOCKETS_DEBUG, (" port=%u)\n", ntohs(local_port)));

   err = netconn_bind(sock->conn, &local_addr, ntohs(local_port));

   if (err != ERR_OK) {
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_bind(%d) failed, err=%d\n", sock, err));
      sock_set_errno(sock, err_to_errno(err));
      return -1;
   }

   LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_bind(%d) succeeded\n", sock));
   sock_set_errno(sock, 0);
   return 0;

   return 0;
}

/*--------------------------------------------
| Name:        _sys_lwip_connect
| Description:
| Parameters:  none
| Return Type: none
| Comments:    lwip 1.3.0
| See:
----------------------------------------------*/
int _sys_lwip_connect(int fd, struct sockaddr *name, socklen_t namelen){

   lwip_sock_t *sock;
   err_t err;

   sock = _sys_lwip_get_sock(fd);
   if (!sock) {
      set_errno(EBADF);
      return -1;
   }

   LWIP_ERROR("lwip_connect: invalid address", ((namelen == sizeof(struct sockaddr_in)) &&
                                                ((((struct sockaddr_in *)name)->sin_family) ==
                                                 AF_INET)),
              sock_set_errno(sock, err_to_errno(ERR_ARG)); return -1; );

   if (((struct sockaddr_in *)name)->sin_family == AF_UNSPEC) {
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_connect(%d, AF_UNSPEC)\n", sock));
      err = netconn_disconnect(sock->conn);
   } else {
      struct ip_addr remote_addr;
      u16_t remote_port;

      remote_addr.addr = ((struct sockaddr_in *)name)->sin_addr.s_addr;
      remote_port = ((struct sockaddr_in *)name)->sin_port;

      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_connect(%d, addr=", sock));
      ip_addr_debug_print(SOCKETS_DEBUG, &remote_addr);
      LWIP_DEBUGF(SOCKETS_DEBUG, (" port=%u)\n", ntohs(remote_port)));

      err = netconn_connect(sock->conn, &remote_addr, ntohs(remote_port));
   }

   if (err != ERR_OK) {
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_connect(%d) failed, err=%d\n", sock, err));
      sock_set_errno(sock, err_to_errno(err));
      return -1;
   }

   LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_connect(%d) succeeded\n", sock));
   sock_set_errno(sock, 0);
   return 0;
}

/*--------------------------------------------
| Name:        _sys_lwip_listen
| Description:
| Parameters:  none
| Return Type: none
| Comments:    lwip 1.3.0
| See:
----------------------------------------------*/
int _sys_lwip_listen(int fd, int backlog){

   lwip_sock_t *sock;
   err_t err;

   sock = _sys_lwip_get_sock(fd);
   if (!sock) {
      set_errno(EBADF);
      return -1;
   }

   LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_listen(%d, backlog=%d)\n", sock, backlog));

   /* limit the "backlog" parameter to fit in an u8_t */
   if (backlog < 0) {
      backlog = 0;
   }
   if (backlog > 0xff) {
      backlog = 0xff;
   }

   err = netconn_listen_with_backlog(sock->conn, backlog);

   if (err != ERR_OK) {
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_listen(%d) failed, err=%d\n", sock, err));
      sock_set_errno(sock, err_to_errno(err));
      return -1;
   }

   sock_set_errno(sock, 0);
   return 0;
   return 0;
}
/*--------------------------------------------
| Name:        _sys_lwip_accept
| Description:
| Parameters:  none
| Return Type: none
| Comments:    lwip 1.3.0
| See:
----------------------------------------------*/
int _sys_lwip_accept(int fd, struct sockaddr *addr, socklen_t *addrlen){

   lwip_sock_t *sock, *nsock;
   struct netconn *newconn;
   struct ip_addr naddr;
   u16_t port;
   struct sockaddr_in sin;
   err_t err;

   int new_fd;
   pid_t pid;
   kernel_pthread_t* pthread_ptr;
   desc_t new_desc;

   if(!(pthread_ptr = kernel_pthread_self()))
      return -1;

   if((pid= pthread_ptr->pid)<1)
      return -1;

   LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_accept(%d)...\n", sock));
   sock = _sys_lwip_get_sock(fd);
   if (!sock) {
      set_errno(EBADF);
      return -1;
   }

   //non blocking call
   if ( (sock->flags & O_NONBLOCK) &&  !sock->rcvevent) {
      sock_set_errno(sock, EWOULDBLOCK);
      return -1;
   }

   newconn = netconn_accept(sock->conn);
   if (!newconn) {
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_accept(%d) failed, err=%d\n", sock, sock->conn->err));
      sock_set_errno(sock, err_to_errno(sock->conn->err));
      return -1;
   }

   // get the IP address and port of the remote host
   err = netconn_peer(newconn, &naddr, &port);
   if (err != ERR_OK) {
      netconn_delete(newconn);
      sock_set_errno(sock, err_to_errno(err));
      return -1;
   }

   memset(&sin, 0, sizeof(sin));
   sin.sin_len = sizeof(sin);
   sin.sin_family = AF_INET;
   sin.sin_port = htons(port);
   sin.sin_addr.s_addr = naddr.addr;

   if (*addrlen > sizeof(sin))
      *addrlen = sizeof(sin);

   SMEMCPY(addr, &sin, *addrlen);

   //lepton: new socket allocation
   /*
   newsock = alloc_socket(newconn);
   if (newsock == -1) {
    netconn_delete(newconn);
    sock_set_errno(sock, ENFILE);
    return -1;
   }
   LWIP_ASSERT("invalid socket index", (newsock >= 0) && (newsock < NUM_SOCKETS));
   newconn->callback = event_callback;
   nsock = &sockets[newsock];
   LWIP_ASSERT("invalid socket pointer", nsock != NULL);
   */
   //alloc a new sock structure
   if( (new_fd=open("/dev/net/sock",O_RDWR,0)) <0) {
      netconn_delete(newconn);
      set_errno(ENOBUFS);
      return -1;
   }
   //get internal desc
   if((new_desc = process_lst[pid]->desc_tbl[new_fd])<0) {
      netconn_delete(newconn);
      set_errno(ENOBUFS);
      return -1;
   }
   //
   sys_sem_wait(socksem);
   {
      u16_t l_rcv_event=-1 - newconn->socket;
      //
      _sys_lwip_attach_sock2conn(new_desc,newconn);
      //
      newconn->callback = _sys_lwip_sock_event_callback;
      nsock = _sys_lwip_get_sock(new_fd);

      /* See event_callback: If data comes in right away after an accept, even
      * though the server task might not have created a new socket yet.
      * In that case, newconn->socket is counted down (newconn->socket--),
      * so nsock->rcvevent is >= 1 here!
      */
      //original calcul from lwip nsock->rcvevent += -1 - newconn->socket;
      nsock->rcvevent=l_rcv_event;
      //nsock->rcvevent = sock->rcvevent+1;
      //sock->rcvevent=0;
      //newconn->socket = new_desc;//this operation already made in _sys_lwip_attach_sock2conn()
   }
   sys_sem_signal(socksem);
   //
   LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_accept(%d) returning new sock=%d addr=", sock, nsock));
   ip_addr_debug_print(SOCKETS_DEBUG, &naddr);
   LWIP_DEBUGF(SOCKETS_DEBUG, (" port=%u\n", port));

   sock_set_errno(sock, 0);

   return new_fd;
}


/*--------------------------------------------
| Name:        _sys_lwip_lowlevel_close
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
static int _sys_lwip_lowlevel_close(lwip_sock_t *sock){
   LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_close(%d)\n", sock));
   //
   netconn_delete(sock->conn);
   //
   sys_sem_wait(socksem);
   if (sock->lastdata) {
      netbuf_delete(sock->lastdata);
   }
   sock->lastdata = NULL;
   sock->lastoffset = 0;
   sock->conn = NULL;
   sock_set_errno(sock, 0);
   sys_sem_signal(socksem);

   return 0;
}

/*--------------------------------------------
| Name:        _sys_lwip_shutdown
| Description:
| Parameters:  none
| Return Type: none
| Comments:    lwip 1.3.0
| See:
----------------------------------------------*/
int _sys_lwip_shutdown(int fd, int how)
{
   LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_shutdown(%d, how=%d)\n", fd, how));
   return close(fd); /* XXX temporary hack until proper implementation */
}



/*--------------------------------------------
| Name:        _sys_lwip_lowlevel_read
| Description:
| Parameters:  none
| Return Type: none
| Comments:    lwip 1.3.0
| See:         recvfrom in lwip stack
----------------------------------------------*/
int _sys_lwip_lowlevel_read(lwip_sock_t *sock, void *mem, int len, unsigned int flags,
                            struct sockaddr *from, socklen_t *fromlen){


   struct netbuf      *buf;
   u16_t buflen, copylen, off = 0;
   struct ip_addr     *addr;
   u16_t port;
   u8_t done = 0;

   if (!sock) {
      set_errno(EBADF);
      return -1;
   }

   LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_recvfrom(%d, %p, %d, 0x%x, ..)\n", sock, mem, len, flags));

   do {
      LWIP_DEBUGF(SOCKETS_DEBUG,
                  ("lwip_recvfrom: top while sock->lastdata=%p\n", (void*)sock->lastdata));
      /* Check if there is data left from the last recv operation. */
      if (sock->lastdata) {
         buf = sock->lastdata;
      } else {
         /* If this is non-blocking call, then check first */
         if (((flags & MSG_DONTWAIT) || (sock->flags & O_NONBLOCK)) && !sock->rcvevent) {
            LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_recvfrom(%d): returning EWOULDBLOCK\n", sock));
            sock_set_errno(sock, EWOULDBLOCK);
            return -1;
         }

         /* No data was left from the previous operation, so we try to get
         some from the network. */
         sock->lastdata = buf = netconn_recv(sock->conn);
         LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_recvfrom: netconn_recv netbuf=%p\n", (void*)buf));

         if (!buf) {
            /* We should really do some error checking here. */
            LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_recvfrom(%d): buf == NULL!\n", sock));
            sock_set_errno(sock,
                           (((sock->conn->pcb.ip!=NULL) &&
                             (sock->conn->err==ERR_OK)) ? ETIMEDOUT : err_to_errno(sock->conn->err)));
            return 0;
         }
      }

      buflen = netbuf_len(buf);
      LWIP_DEBUGF(SOCKETS_DEBUG,
                  ("lwip_recvfrom: buflen=%d len=%d off=%d sock->lastoffset=%d\n", buflen, len, off,
                   sock->lastoffset));

      buflen -= sock->lastoffset;

      if (len > buflen) {
         copylen = buflen;
      } else {
         copylen = len;
      }

      /* copy the contents of the received buffer into
      the supplied memory pointer mem */
      netbuf_copy_partial(buf, (u8_t*)mem + off, copylen, sock->lastoffset);

      off += copylen;

      if (netconn_type(sock->conn) == NETCONN_TCP) {
         len -= copylen;
         if ( (len <= 0) || (buf->p->flags & PBUF_FLAG_PUSH) || !sock->rcvevent) {
            done = 1;
         }
      } else {
         done = 1;
      }

      /* If we don't peek the incoming message... */
      if ((flags & MSG_PEEK)==0) {
         /* If this is a TCP socket, check if there is data left in the
            buffer. If so, it should be saved in the sock structure for next
            time around. */
         if ((sock->conn->type == NETCONN_TCP) && (buflen - copylen > 0)) {
            sock->lastdata = buf;
            sock->lastoffset += copylen;
            LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_recvfrom: lastdata now netbuf=%p\n", (void*)buf));
         } else {
            sock->lastdata = NULL;
            sock->lastoffset = 0;
            LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_recvfrom: deleting netbuf=%p\n", (void*)buf));
            netbuf_delete(buf);
         }
      } else {
         done = 1;
      }
   } while (!done);

   /* Check to see from where the data was.*/
   if (from && fromlen) {
      struct sockaddr_in sin;

      if (netconn_type(sock->conn) == NETCONN_TCP) {
         addr = (struct ip_addr*)&(sin.sin_addr.s_addr);
         netconn_getaddr(sock->conn, addr, &port, 0);
      } else {
         addr = netbuf_fromaddr(buf);
         port = netbuf_fromport(buf);
      }

      memset(&sin, 0, sizeof(sin));
      sin.sin_len = sizeof(sin);
      sin.sin_family = AF_INET;
      sin.sin_port = htons(port);
      sin.sin_addr.s_addr = addr->addr;

      if (*fromlen > sizeof(sin))
         *fromlen = sizeof(sin);

      SMEMCPY(from, &sin, *fromlen);

      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_recvfrom(%d): addr=", sock));
      ip_addr_debug_print(SOCKETS_DEBUG, addr);
      LWIP_DEBUGF(SOCKETS_DEBUG, (" port=%u len=%u\n", port, off));
   } else {
#if SOCKETS_DEBUG
      struct sockaddr_in sin;

      if (netconn_type(sock->conn) == NETCONN_TCP) {
         addr = (struct ip_addr*)&(sin.sin_addr.s_addr);
         netconn_getaddr(sock->conn, addr, &port, 0);
      } else {
         addr = netbuf_fromaddr(buf);
         port = netbuf_fromport(buf);
      }

      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_recvfrom(%d): addr=", sock));
      ip_addr_debug_print(SOCKETS_DEBUG, addr);
      LWIP_DEBUGF(SOCKETS_DEBUG, (" port=%u len=%u\n", port, off));
#endif /*  SOCKETS_DEBUG */
   }

   sock_set_errno(sock, 0);
   return off;
}

/*--------------------------------------------
| Name:        _sys_lwip_recv
| Description:
| Parameters:  none
| Return Type: none
| Comments:    lwip 1.3.0
| See:
----------------------------------------------*/
int _sys_lwip_recv(int fd, void *mem, int len, unsigned int flags){

   lwip_sock_t *sock;
   pid_t pid;
   kernel_pthread_t* pthread_ptr;
   desc_t desc;
   int cb;

   //begin devio section
   if(fd<0)
      return -1;

   if(len<=0)
      return -1;

   if(!(pthread_ptr = kernel_pthread_self()))
      return -1;

   if((pid= pthread_ptr->pid)<1)
      return -1;

   desc = process_lst[pid]->desc_tbl[fd];

   if(desc<0)
      return -1;

   if(!(ofile_lst[desc].oflag&O_RDONLY))
      return -1;

   if(ofile_lst[desc].attr&(S_IFREG|S_IFDIR))
      return -1;  //not device file

   //get sock
   sock = _sys_lwip_get_sock(fd);
   if (!sock) {
      set_errno(EBADF);
      return -1;
   }

   //
   __lock_io(pthread_ptr,ofile_lst[desc].desc,O_RDONLY);
   //check thread owner
   __atomic_in();
   {
      desc_t _desc=ofile_lst[desc].desc;
      //
      if(ofile_lst[_desc].owner_pthread_ptr_read!=pthread_ptr) {
         do {
            //check
            if(ofile_lst[_desc].used<=0) {
               __atomic_out();
               __unlock_io(pthread_ptr,ofile_lst[desc].desc,O_RDONLY);
               return -1; //error, stream not coherent :(
            }
            //
            //begin of section: protection from io interrupt
            __disable_interrupt_section_in();
            //
            ofile_lst[_desc].owner_pthread_ptr_read=pthread_ptr;
            ofile_lst[_desc].owner_pid=pid;
            //end of section: protection from io interrupt
            __disable_interrupt_section_out();
            //
            //aware: continue operation on original desc (see fattach() and _vfs_open() note 1)
         } while((_desc=ofile_lst[_desc].desc_nxt[0])>=0);
      }
   }
   __atomic_out();
   //end devio section

   //
   cb=_sys_lwip_lowlevel_read(sock, mem, len, flags, NULL, NULL);
   //
   __unlock_io(pthread_ptr,ofile_lst[desc].desc,O_RDONLY);
   //
   return cb;
}

/*--------------------------------------------
| Name:        _sys_lwip_recvfrom
| Description:
| Parameters:  none
| Return Type: none
| Comments:    lwip 1.3.0
| See:
----------------------------------------------*/
int _sys_lwip_recvfrom(int fd, void *mem, int len, unsigned int flags,struct sockaddr *from,
                       socklen_t *fromlen){

   lwip_sock_t *sock;

   pid_t pid;
   kernel_pthread_t* pthread_ptr;
   desc_t desc;
   int cb;

   //begin devio section
   if(fd<0)
      return -1;

   if(len<=0)
      return -1;

   if(!(pthread_ptr = kernel_pthread_self()))
      return -1;

   if((pid= pthread_ptr->pid)<1)
      return -1;

   desc = process_lst[pid]->desc_tbl[fd];

   if(desc<0)
      return -1;

   if(!(ofile_lst[desc].oflag&O_RDONLY))
      return -1;

   if(ofile_lst[desc].attr&(S_IFREG|S_IFDIR))
      return -1;  //not device file

   //get sock
   sock = _sys_lwip_get_sock(fd);
   if (!sock) {
      set_errno(EBADF);
      return -1;
   }

   //
   __lock_io(pthread_ptr,ofile_lst[desc].desc,O_RDONLY);
   //check thread owner
   __atomic_in();
   {
      desc_t _desc=ofile_lst[desc].desc;
      //
      if(ofile_lst[_desc].owner_pthread_ptr_read!=pthread_ptr) {
         do {
            //check
            if(ofile_lst[_desc].used<=0) {
               __atomic_out();
               __unlock_io(pthread_ptr,ofile_lst[desc].desc,O_RDONLY);
               return -1; //error, stream not coherent :(
            }
            //
            //begin of section: protection from io interrupt
            __disable_interrupt_section_in();
            //
            ofile_lst[_desc].owner_pthread_ptr_read=pthread_ptr;
            ofile_lst[_desc].owner_pid=pid;
            //end of section: protection from io interrupt
            __disable_interrupt_section_out();
            //
            //aware: continue operation on original desc (see fattach() and _vfs_open() note 1)
         } while((_desc=ofile_lst[_desc].desc_nxt[0])>=0);
      }
   }
   __atomic_out();
   //end devio section

   //
   cb=_sys_lwip_lowlevel_read(sock, mem, len, flags, from, fromlen);
   //
   __unlock_io(pthread_ptr,ofile_lst[desc].desc,O_RDONLY);
   //
   return cb;
}

/*--------------------------------------------
| Name:        _sys_lwip_lowlevel_write
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int _sys_lwip_lowlevel_write(lwip_sock_t *sock, const void *data, int size, unsigned int flags){

   err_t err;

   LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_send(%d, data=%p, size=%d, flags=0x%x)\n",
                               sock, data, size, flags));
   if (!sock) {
      set_errno(EBADF);
      return -1;
   }

   if (sock->conn->type!=NETCONN_TCP) {
#if (LWIP_UDP || LWIP_RAW)
      return _sys_lwip_lowlevel_writeto(sock, (void*)data, size, flags, NULL, 0);
#else
      sock_set_errno(sock, err_to_errno(ERR_ARG));
      return -1;
#endif /* (LWIP_UDP || LWIP_RAW) */
   }

   err =
      netconn_write(sock->conn, data, size, NETCONN_COPY | ((flags & MSG_MORE) ? NETCONN_MORE : 0));
   //phlb modif 05/03/2008
   if( !(sock->flags & O_NONBLOCK) && (sock->desc>=0) ) {
      __fire_io_int(ofile_lst[sock->desc].owner_pthread_ptr_write);
   }

   LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_send(%d) err=%d size=%d\n", sock, err, size));
   sock_set_errno(sock, err_to_errno(err));
   return (err==ERR_OK ? size : -1);
}

/*--------------------------------------------
| Name:        _sys_lwip_lowlevel_writeto
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int _sys_lwip_lowlevel_writeto( lwip_sock_t *sock, void *data, int size, unsigned int flags,
                                struct sockaddr *to, socklen_t tolen){

   struct ip_addr remote_addr;
   int err;

#if !LWIP_TCPIP_CORE_LOCKING
   struct netbuf buf;
   u16_t remote_port;
#endif

   if (!sock) {
      set_errno(EBADF);
      return -1;
   }

   if (sock->conn->type==NETCONN_TCP) {
#if LWIP_TCP
      return _sys_lwip_lowlevel_write(sock, data, size, flags);
#else
      sock_set_errno(sock, err_to_errno(ERR_ARG));
      return -1;
#endif /* LWIP_TCP */
   }

   LWIP_ASSERT("lwip_sendto: size must fit in u16_t",
               ((size >= 0) && (size <= 0xffff)));
   LWIP_ERROR("lwip_sendto: invalid address", (((to == NULL) && (tolen == 0)) ||
                                               ((tolen == sizeof(struct sockaddr_in)) &&
                                                ((((struct sockaddr_in *)to)->sin_family) ==
                                                 AF_INET))),
              sock_set_errno(sock, err_to_errno(ERR_ARG)); return -1; );

#if LWIP_TCPIP_CORE_LOCKING
   /* Should only be consider like a sample or a simple way to experiment this option (no check of "to" field...) */
   { struct pbuf* p;

     p = pbuf_alloc(PBUF_TRANSPORT, 0, PBUF_REF);
     if (p == NULL) {
        err = ERR_MEM;
     } else {
        p->payload = (void*)data;
        p->len = p->tot_len = size;

        remote_addr.addr = ((struct sockaddr_in *)to)->sin_addr.s_addr;

        LOCK_TCPIP_CORE();
        if (sock->conn->type==NETCONN_RAW) {
           err = sock->conn->err = raw_sendto(sock->conn->pcb.raw, p, &remote_addr);
        } else {
           err = sock->conn->err =
                    udp_sendto(sock->conn->pcb.udp, p, &remote_addr,
                               ntohs(((struct sockaddr_in *)to)->sin_port));
        }
        UNLOCK_TCPIP_CORE();

        pbuf_free(p);
     }}
#else
   /* initialize a buffer */
   buf.p = buf.ptr = NULL;
   if (to) {
      remote_addr.addr = ((struct sockaddr_in *)to)->sin_addr.s_addr;
      remote_port      = ntohs(((struct sockaddr_in *)to)->sin_port);
      buf.addr         = &remote_addr;
      buf.port         = remote_port;
   } else {
      remote_addr.addr = 0;
      remote_port      = 0;
      buf.addr         = NULL;
      buf.port         = 0;
   }

   LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_sendto(%d, data=%p, size=%d, flags=0x%x to=",
                               sock, data, size, flags));
   ip_addr_debug_print(SOCKETS_DEBUG, &remote_addr);
   LWIP_DEBUGF(SOCKETS_DEBUG, (" port=%u\n", remote_port));

   /* make the buffer point to the data that should be sent */
   if ((err = netbuf_ref(&buf, data, size)) == ERR_OK) {
      /* send the data */
      err = netconn_send(sock->conn, &buf);
   }

   /* deallocated the buffer */
   if (buf.p != NULL) {
      pbuf_free(buf.p);
   }
#endif /* LWIP_TCPIP_CORE_LOCKING */
   sock_set_errno(sock, err_to_errno(err));
   return (err==ERR_OK ? size : -1);
}

/*--------------------------------------------
| Name:        _sys_lwip_send
| Description:
| Parameters:  none
| Return Type: none
| Comments:    lwip 1.3.0
| See:
----------------------------------------------*/
int _sys_lwip_send(int fd, const void *data, int size, unsigned int flags){

   lwip_sock_t *sock;

   pid_t pid;
   kernel_pthread_t* pthread_ptr;
   desc_t desc;
   int cb;

   //begin devio section
   if(fd<0)
      return -1;

   if(size<0)
      return -1;

   if(size==0)
      return 0;

   if(!(pthread_ptr = kernel_pthread_self()))
      return -1;

   if((pid= pthread_ptr->pid)<1)
      return -1;

   desc = process_lst[pid]->desc_tbl[fd];

   if(desc<0)
      return -1;

   if(!(ofile_lst[desc].oflag&O_WRONLY))
      return -1;

   if(ofile_lst[desc].attr&(S_IFREG|S_IFDIR))
      return -1;  //not device file

   //get sock
   sock = _sys_lwip_get_sock(fd);
   if (!sock) {
      set_errno(EBADF);
      return -1;
   }

   //
   __lock_io(pthread_ptr,ofile_lst[desc].desc,O_WRONLY);
   //check thread owner
   __atomic_in();
   {
      desc_t _desc=ofile_lst[desc].desc;
      //
      if(ofile_lst[_desc].owner_pthread_ptr_write!=pthread_ptr) {
         do {
            //check
            if(ofile_lst[_desc].used<=0) {
               __atomic_out();
               __unlock_io(pthread_ptr,ofile_lst[desc].desc,O_WRONLY);
               return -1; //error, stream not coherent :(
            }
            //
            //begin of section: protection from io interrupt
            __disable_interrupt_section_in();
            //
            ofile_lst[_desc].owner_pthread_ptr_write=pthread_ptr;
            ofile_lst[_desc].owner_pid=pid;
            //end of section: protection from io interrupt
            __disable_interrupt_section_out();
            //
            //aware: continue operation on original desc (see fattach() and _vfs_open() note 1)
         } while((_desc=ofile_lst[_desc].desc_nxt[1])>=0);
      }
   }
   __atomic_out();
   //end devio section

   //
   cb=_sys_lwip_lowlevel_write(sock,data,size,flags);
   //
   __unlock_io(pthread_ptr,ofile_lst[desc].desc,O_WRONLY);
   //
   return cb;
}

/*--------------------------------------------
| Name:        _sys_lwip_sendto
| Description:
| Parameters:  none
| Return Type: none
| Comments:    lwip 1.3.0
| See:
----------------------------------------------*/
int _sys_lwip_sendto(int fd,void *data, int size, unsigned int flags,struct sockaddr *to,
                     socklen_t tolen){

   lwip_sock_t *sock;

   pid_t pid;
   kernel_pthread_t* pthread_ptr;
   desc_t desc;
   int cb;

   //begin devio section
   if(fd<0)
      return -1;

   if(size<0)
      return -1;

   if(size==0)
      return 0;

   if(!(pthread_ptr = kernel_pthread_self()))
      return -1;

   if((pid= pthread_ptr->pid)<1)
      return -1;

   desc = process_lst[pid]->desc_tbl[fd];

   if(desc<0)
      return -1;

   if(!(ofile_lst[desc].oflag&O_WRONLY))
      return -1;

   if(ofile_lst[desc].attr&(S_IFREG|S_IFDIR))
      return -1;  //not device file

   //get sock
   sock = _sys_lwip_get_sock(fd);
   if (!sock) {
      set_errno(EBADF);
      return -1;
   }

   //
   __lock_io(pthread_ptr,ofile_lst[desc].desc,O_WRONLY);
   //check thread owner
   __atomic_in();
   {
      desc_t _desc=ofile_lst[desc].desc;
      //
      if(ofile_lst[_desc].owner_pthread_ptr_write!=pthread_ptr) {
         do {
            //check
            if(ofile_lst[_desc].used<=0) {
               __atomic_out();
               __unlock_io(pthread_ptr,ofile_lst[desc].desc,O_WRONLY);
               return -1; //error, stream not coherent :(
            }
            //
            //begin of section: protection from io interrupt
            __disable_interrupt_section_in();
            //
            ofile_lst[_desc].owner_pthread_ptr_write=pthread_ptr;
            ofile_lst[_desc].owner_pid=pid;
            //end of section: protection from io interrupt
            __disable_interrupt_section_out();
            //
            //aware: continue operation on original desc (see fattach() and _vfs_open() note 1)
         } while((_desc=ofile_lst[_desc].desc_nxt[1])>=0);
      }
   }
   __atomic_out();
   //end devio section

   //
   cb=_sys_lwip_lowlevel_writeto(sock,data,size,flags,to,tolen);
   //
   __unlock_io(pthread_ptr,ofile_lst[desc].desc,O_WRONLY);
   //
   return cb;
}

/*--------------------------------------------
| Name:        _sys_lwip_getaddrname
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
static int _sys_lwip_getaddrname(int fd, struct sockaddr *name, socklen_t *namelen, u8_t local)
{
   struct sockaddr_in sin;
   struct ip_addr naddr;
   err_t res;

   lwip_sock_t *sock;

   sock = _sys_lwip_get_sock(fd);
   if (!sock) {
      set_errno(EBADF);
      return -1;
   }

   memset(&sin, 0, sizeof(sin));
   sin.sin_len = sizeof(sin);
   sin.sin_family = AF_INET;

   /* get the IP address and port */
   res = netconn_getaddr(sock->conn, &naddr, &sin.sin_port, local);

   if (res != ERR_OK) { // ERR_CONN for invalid connections
      set_errno(ECONNRESET);
      return -1;
   }

   LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getaddrname(%d, addr=", sock));
   ip_addr_debug_print(SOCKETS_DEBUG, &naddr);
   LWIP_DEBUGF(SOCKETS_DEBUG, (" port=%d)\n", sin.sin_port));

   sin.sin_port = htons(sin.sin_port);
   sin.sin_addr.s_addr = naddr.addr;

   if (*namelen > sizeof(sin))
      *namelen = sizeof(sin);

   SMEMCPY(name, &sin, *namelen);
   sock_set_errno(sock, 0);
   return 0;
}

/*--------------------------------------------
| Name:        _sys_lwip_getpeername
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int _sys_lwip_getpeername(int fd, struct sockaddr *name, socklen_t *namelen){
   return _sys_lwip_getaddrname(fd, name, namelen, 0);
}

/*--------------------------------------------
| Name:        _sys_lwip_getsockname
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int _sys_lwip_getsockname(int fd, struct sockaddr *name, socklen_t *namelen){
   return _sys_lwip_getaddrname(fd, name, namelen, 1);
}

/*--------------------------------------------
| Name:        _sys_lwip_getsockopt_internal
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
static void _sys_lwip_getsockopt_internal(void *arg)
{
   lwip_sock_t *sock;
#ifdef LWIP_DEBUG
   int s;
#endif /* LWIP_DEBUG */
   int level, optname;
   void *optval;
   struct lwip_setgetsockopt_data_st *data;

   LWIP_ASSERT("arg != NULL", arg != NULL);

   data = (struct lwip_setgetsockopt_data_st*)arg;
   sock = data->sock;
#ifdef LWIP_DEBUG
   s = data->fd;
#endif /* LWIP_DEBUG */
   level = data->level;
   optname = data->optname;
   optval = data->optval;

   switch (level) {

/* Level: SOL_SOCKET */
   case SOL_SOCKET:
      switch (optname) {

      /* The option flags */
      case SO_ACCEPTCONN:
      case SO_BROADCAST:
      /* UNIMPL case SO_DEBUG: */
      /* UNIMPL case SO_DONTROUTE: */
      case SO_KEEPALIVE:
         /* UNIMPL case SO_OOBINCLUDE: */
#if SO_REUSE
      case SO_REUSEADDR:
      case SO_REUSEPORT:
#endif /* SO_REUSE */
       /*case SO_USELOOPBACK: UNIMPL */
         *(int*)optval = sock->conn->pcb.ip->so_options & optname;
         LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, SOL_SOCKET, optname=0x%x, ..) = %s\n",
                                     sock, optname, (*(int*)optval ? "on" : "off")));
         break;

      case SO_TYPE:
         switch (NETCONNTYPE_GROUP(sock->conn->type)) {
         case NETCONN_RAW:
            *(int*)optval = SOCK_RAW;
            break;
         case NETCONN_TCP:
            *(int*)optval = SOCK_STREAM;
            break;
         case NETCONN_UDP:
            *(int*)optval = SOCK_DGRAM;
            break;
         default: /* unrecognized socket type */
            *(int*)optval = sock->conn->type;
            LWIP_DEBUGF(SOCKETS_DEBUG,
                        ("lwip_getsockopt(%d, SOL_SOCKET, SO_TYPE): unrecognized socket type %d\n",
                         sock, *(int *)optval));
         } /* switch (sock->conn->type) */
         LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, SOL_SOCKET, SO_TYPE) = %d\n",
                                     sock, *(int *)optval));
         break;

      case SO_ERROR:
         if (sock->err == 0) {
            sock_set_errno(sock, err_to_errno(sock->conn->err));
         }
         *(int *)optval = sock->err;
         sock->err = 0;
         LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, SOL_SOCKET, SO_ERROR) = %d\n",
                                     sock, *(int *)optval));
         break;

#if LWIP_SO_RCVTIMEO
      case SO_RCVTIMEO:
         *(int *)optval = sock->conn->recv_timeout;
         break;
#endif /* LWIP_SO_RCVTIMEO */
#if LWIP_SO_RCVBUF
      case SO_RCVBUF:
         *(int *)optval = sock->conn->recv_bufsize;
         break;
#endif /* LWIP_SO_RCVBUF */
#if LWIP_UDP
      case SO_NO_CHECK:
         *(int*)optval = (udp_flags(sock->conn->pcb.udp) & UDP_FLAGS_NOCHKSUM) ? 1 : 0;
         break;
#endif /* LWIP_UDP*/
      } /* switch (optname) */
      break;

/* Level: IPPROTO_IP */
   case IPPROTO_IP:
      switch (optname) {
      case IP_TTL:
         *(int*)optval = sock->conn->pcb.ip->ttl;
         LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, IPPROTO_IP, IP_TTL) = %d\n",
                                     sock, *(int *)optval));
         break;
      case IP_TOS:
         *(int*)optval = sock->conn->pcb.ip->tos;
         LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, IPPROTO_IP, IP_TOS) = %d\n",
                                     sock, *(int *)optval));
         break;
#if LWIP_IGMP
      case IP_MULTICAST_TTL:
         *(u8_t*)optval = sock->conn->pcb.ip->ttl;
         LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, IPPROTO_IP, IP_MULTICAST_TTL) = %d\n",
                                     sock, *(int *)optval));
         break;
      case IP_MULTICAST_IF:
         ((struct in_addr*) optval)->s_addr = sock->conn->pcb.udp->multicast_ip.addr;
         LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, IPPROTO_IP, IP_MULTICAST_IF) = 0x%x\n",
                                     sock, *(u32_t *)optval));
         break;
#endif /* LWIP_IGMP */
      } /* switch (optname) */
      break;

#if LWIP_TCP
/* Level: IPPROTO_TCP */
   case IPPROTO_TCP:
      switch (optname) {
      case TCP_NODELAY:
         *(int*)optval = (sock->conn->pcb.tcp->flags & TF_NODELAY);
         LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, IPPROTO_TCP, TCP_NODELAY) = %s\n",
                                     sock, (*(int*)optval) ? "on" : "off") );
         break;
      case TCP_KEEPALIVE:
         *(int*)optval = (int)sock->conn->pcb.tcp->keep_idle;
         LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, IPPROTO_IP, TCP_KEEPALIVE) = %d\n",
                                     sock, *(int *)optval));
         break;

   #if LWIP_TCP_KEEPALIVE
      case TCP_KEEPIDLE:
         *(int*)optval = (int)(sock->conn->pcb.tcp->keep_idle/1000);
         LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, IPPROTO_IP, TCP_KEEPIDLE) = %d\n",
                                     sock, *(int *)optval));
         break;
      case TCP_KEEPINTVL:
         *(int*)optval = (int)(sock->conn->pcb.tcp->keep_intvl/1000);
         LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, IPPROTO_IP, TCP_KEEPINTVL) = %d\n",
                                     sock, *(int *)optval));
         break;
      case TCP_KEEPCNT:
         *(int*)optval = (int)sock->conn->pcb.tcp->keep_cnt;
         LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, IPPROTO_IP, TCP_KEEPCNT) = %d\n",
                                     sock, *(int *)optval));
         break;
   #endif /* LWIP_TCP_KEEPALIVE */

      } /* switch (optname) */
      break;
#endif /* LWIP_TCP */
#if LWIP_UDP && LWIP_UDPLITE
   /* Level: IPPROTO_UDPLITE */
   case IPPROTO_UDPLITE:
      switch (optname) {
      case UDPLITE_SEND_CSCOV:
         *(int*)optval = sock->conn->pcb.udp->chksum_len_tx;
         LWIP_DEBUGF(SOCKETS_DEBUG,
                     ("lwip_getsockopt(%d, IPPROTO_UDPLITE, UDPLITE_SEND_CSCOV) = %d\n",
                      sock, (*(int*)optval)) );
         break;
      case UDPLITE_RECV_CSCOV:
         *(int*)optval = sock->conn->pcb.udp->chksum_len_rx;
         LWIP_DEBUGF(SOCKETS_DEBUG,
                     ("lwip_getsockopt(%d, IPPROTO_UDPLITE, UDPLITE_RECV_CSCOV) = %d\n",
                      sock, (*(int*)optval)) );
         break;
      } /* switch (optname) */
      break;
#endif /* LWIP_UDP */
   } /* switch (level) */
   sys_sem_signal(sock->conn->op_completed);
}

/*--------------------------------------------
| Name:        _sys_lwip_getsockopt
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int _sys_lwip_getsockopt(int fd, int level, int optname, void *optval, socklen_t *optlen)
{
   err_t err = ERR_OK;
   lwip_sock_t *sock;
   struct lwip_setgetsockopt_data_st data;

   sock = _sys_lwip_get_sock(fd);
   if (!sock) {
      set_errno(EBADF);
      return -1;
   }

   if ((NULL == optval) || (NULL == optlen)) {
      sock_set_errno(sock, EFAULT);
      return -1;
   }

   /* Do length and type checks for the various options first, to keep it readable. */
   switch (level) {

/* Level: SOL_SOCKET */
   case SOL_SOCKET:
      switch (optname) {

      case SO_ACCEPTCONN:
      case SO_BROADCAST:
      /* UNIMPL case SO_DEBUG: */
      /* UNIMPL case SO_DONTROUTE: */
      case SO_ERROR:
      case SO_KEEPALIVE:
         /* UNIMPL case SO_CONTIMEO: */
         /* UNIMPL case SO_SNDTIMEO: */
#if LWIP_SO_RCVTIMEO
      case SO_RCVTIMEO:
#endif /* LWIP_SO_RCVTIMEO */
#if LWIP_SO_RCVBUF
      case SO_RCVBUF:
#endif /* LWIP_SO_RCVBUF */
       /* UNIMPL case SO_OOBINLINE: */
       /* UNIMPL case SO_SNDBUF: */
       /* UNIMPL case SO_RCVLOWAT: */
       /* UNIMPL case SO_SNDLOWAT: */
#if SO_REUSE
      case SO_REUSEADDR:
      case SO_REUSEPORT:
#endif /* SO_REUSE */
      case SO_TYPE:
         /* UNIMPL case SO_USELOOPBACK: */
         if (*optlen < sizeof(int)) {
            err = EINVAL;
         }
         break;

      case SO_NO_CHECK:
         if (*optlen < sizeof(int)) {
            err = EINVAL;
         }
#if LWIP_UDP
         if ((sock->conn->type != NETCONN_UDP) ||
             ((udp_flags(sock->conn->pcb.udp) & UDP_FLAGS_UDPLITE) != 0)) {
            /* this flag is only available for UDP, not for UDP lite */
            err = EAFNOSUPPORT;
         }
#endif /* LWIP_UDP */
         break;

      default:
         LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, SOL_SOCKET, UNIMPL: optname=0x%x, ..)\n",
                                     sock, optname));
         err = ENOPROTOOPT;
      } /* switch (optname) */
      break;

/* Level: IPPROTO_IP */
   case IPPROTO_IP:
      switch (optname) {
      /* UNIMPL case IP_HDRINCL: */
      /* UNIMPL case IP_RCVDSTADDR: */
      /* UNIMPL case IP_RCVIF: */
      case IP_TTL:
      case IP_TOS:
         if (*optlen < sizeof(int)) {
            err = EINVAL;
         }
         break;
#if LWIP_IGMP
      case IP_MULTICAST_TTL:
         if (*optlen < sizeof(u8_t)) {
            err = EINVAL;
         }
         break;
      case IP_MULTICAST_IF:
         if (*optlen < sizeof(struct in_addr)) {
            err = EINVAL;
         }
         break;
#endif /* LWIP_IGMP */

      default:
         LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, IPPROTO_IP, UNIMPL: optname=0x%x, ..)\n",
                                     sock, optname));
         err = ENOPROTOOPT;
      } /* switch (optname) */
      break;

#if LWIP_TCP
/* Level: IPPROTO_TCP */
   case IPPROTO_TCP:
      if (*optlen < sizeof(int)) {
         err = EINVAL;
         break;
      }

      /* If this is no TCP socket, ignore any options. */
      if (sock->conn->type != NETCONN_TCP)
         return 0;

      switch (optname) {
      case TCP_NODELAY:
      case TCP_KEEPALIVE:
   #if LWIP_TCP_KEEPALIVE
      case TCP_KEEPIDLE:
      case TCP_KEEPINTVL:
      case TCP_KEEPCNT:
   #endif /* LWIP_TCP_KEEPALIVE */
         break;

      default:
         LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, IPPROTO_TCP, UNIMPL: optname=0x%x, ..)\n",
                                     sock, optname));
         err = ENOPROTOOPT;
      } /* switch (optname) */
      break;
#endif /* LWIP_TCP */
#if LWIP_UDP && LWIP_UDPLITE
/* Level: IPPROTO_UDPLITE */
   case IPPROTO_UDPLITE:
      if (*optlen < sizeof(int)) {
         err = EINVAL;
         break;
      }

      /* If this is no UDP lite socket, ignore any options. */
      if (sock->conn->type != NETCONN_UDPLITE)
         return 0;

      switch (optname) {
      case UDPLITE_SEND_CSCOV:
      case UDPLITE_RECV_CSCOV:
         break;

      default:
         LWIP_DEBUGF(SOCKETS_DEBUG,
                     ("lwip_getsockopt(%d, IPPROTO_UDPLITE, UNIMPL: optname=0x%x, ..)\n",
                      sock, optname));
         err = ENOPROTOOPT;
      } /* switch (optname) */
      break;
#endif /* LWIP_UDP && LWIP_UDPLITE*/
/* UNDEFINED LEVEL */
   default:
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, level=0x%x, UNIMPL: optname=0x%x, ..)\n",
                                  sock, level, optname));
      err = ENOPROTOOPT;
   } /* switch */


   if (err != ERR_OK) {
      sock_set_errno(sock, err);
      return -1;
   }

   /* Now do the actual option processing */
   data.sock = sock;
   data.level = level;
   data.optname = optname;
   data.optval = optval;
   data.optlen = optlen;
   data.err = err;
   tcpip_callback(_sys_lwip_getsockopt_internal, &data);
   sys_arch_sem_wait(sock->conn->op_completed, 0);
   /* maybe _sys_lwip_getsockopt_internal has changed err */
   err = data.err;

   sock_set_errno(sock, err);
   return err ? -1 : 0;
}

/*--------------------------------------------
| Name:        _sys_lwip_setsockopt_internal
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
static void _sys_lwip_setsockopt_internal(void *arg)
{
   lwip_sock_t *sock;
#ifdef LWIP_DEBUG
   int s;
#endif /* LWIP_DEBUG */
   int level, optname;
   const void *optval;
   struct lwip_setgetsockopt_data_st *data;

   LWIP_ASSERT("arg != NULL", arg != NULL);

   data = (struct lwip_setgetsockopt_data_st*)arg;
   sock = data->sock;
#ifdef LWIP_DEBUG
   s = data->fd;
#endif /* LWIP_DEBUG */
   level = data->level;
   optname = data->optname;
   optval = data->optval;

   switch (level) {

/* Level: SOL_SOCKET */
   case SOL_SOCKET:
      switch (optname) {

      /* The option flags */
      case SO_BROADCAST:
      /* UNIMPL case SO_DEBUG: */
      /* UNIMPL case SO_DONTROUTE: */
      case SO_KEEPALIVE:
         /* UNIMPL case SO_OOBINCLUDE: */
#if SO_REUSE
      case SO_REUSEADDR:
      case SO_REUSEPORT:
#endif /* SO_REUSE */
       /* UNIMPL case SO_USELOOPBACK: */
         if (*(int*)optval) {
            sock->conn->pcb.ip->so_options |= optname;
         } else {
            sock->conn->pcb.ip->so_options &= ~optname;
         }
         LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, SOL_SOCKET, optname=0x%x, ..) -> %s\n",
                                     sock, optname, (*(int*)optval ? "on" : "off")));
         break;
#if LWIP_SO_RCVTIMEO
      case SO_RCVTIMEO:
         sock->conn->recv_timeout = ( *(int*)optval );
         break;
#endif /* LWIP_SO_RCVTIMEO */
#if LWIP_SO_RCVBUF
      case SO_RCVBUF:
         sock->conn->recv_bufsize = ( *(int*)optval );
         break;
#endif /* LWIP_SO_RCVBUF */
#if LWIP_UDP
      case SO_NO_CHECK:
         if (*(int*)optval) {
            udp_setflags(sock->conn->pcb.udp, udp_flags(sock->conn->pcb.udp) | UDP_FLAGS_NOCHKSUM);
         } else {
            udp_setflags(sock->conn->pcb.udp, udp_flags(sock->conn->pcb.udp) & ~UDP_FLAGS_NOCHKSUM);
         }
         break;
#endif /* LWIP_UDP */
      } /* switch (optname) */
      break;

/* Level: IPPROTO_IP */
   case IPPROTO_IP:
      switch (optname) {
      case IP_TTL:
         sock->conn->pcb.ip->ttl = (u8_t)(*(int*)optval);
         LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, IPPROTO_IP, IP_TTL, ..) -> %u\n",
                                     sock, sock->conn->pcb.ip->ttl));
         break;
      case IP_TOS:
         sock->conn->pcb.ip->tos = (u8_t)(*(int*)optval);
         LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, IPPROTO_IP, IP_TOS, ..)-> %u\n",
                                     sock, sock->conn->pcb.ip->tos));
         break;
#if LWIP_IGMP
      case IP_MULTICAST_TTL:
         sock->conn->pcb.udp->ttl = (u8_t)(*(u8_t*)optval);
         break;
      case IP_MULTICAST_IF:
         sock->conn->pcb.udp->multicast_ip.addr = ((struct in_addr*) optval)->s_addr;
         break;
      case IP_ADD_MEMBERSHIP:
      case IP_DROP_MEMBERSHIP:
      {
         /* If this is a TCP or a RAW socket, ignore these options. */
         struct ip_mreq *imr = (struct ip_mreq *)optval;
         if(optname == IP_ADD_MEMBERSHIP) {
            data->err =
               igmp_joingroup((struct ip_addr*)&(imr->imr_interface.s_addr),
                              (struct ip_addr*)&(imr->imr_multiaddr.s_addr));
         } else {
            data->err =
               igmp_leavegroup((struct ip_addr*)&(imr->imr_interface.s_addr),
                               (struct ip_addr*)&(imr->imr_multiaddr.s_addr));
         }
         if(data->err != ERR_OK) {
            data->err = EADDRNOTAVAIL;
         }
      }
      break;
#endif /* LWIP_IGMP */
      } /* switch (optname) */
      break;

#if LWIP_TCP
/* Level: IPPROTO_TCP */
   case IPPROTO_TCP:
      switch (optname) {
      case TCP_NODELAY:
         if (*(int*)optval) {
            sock->conn->pcb.tcp->flags |= TF_NODELAY;
         } else {
            sock->conn->pcb.tcp->flags &= ~TF_NODELAY;
         }
         LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, IPPROTO_TCP, TCP_NODELAY) -> %s\n",
                                     sock, (*(int *)optval) ? "on" : "off") );
         break;
      case TCP_KEEPALIVE:
         sock->conn->pcb.tcp->keep_idle = (u32_t)(*(int*)optval);
         LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, IPPROTO_TCP, TCP_KEEPALIVE) -> %lu\n",
                                     sock, sock->conn->pcb.tcp->keep_idle));
         break;

   #if LWIP_TCP_KEEPALIVE
      case TCP_KEEPIDLE:
         sock->conn->pcb.tcp->keep_idle = 1000*(u32_t)(*(int*)optval);
         LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, IPPROTO_TCP, TCP_KEEPIDLE) -> %lu\n",
                                     sock, sock->conn->pcb.tcp->keep_idle));
         break;
      case TCP_KEEPINTVL:
         sock->conn->pcb.tcp->keep_intvl = 1000*(u32_t)(*(int*)optval);
         LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, IPPROTO_TCP, TCP_KEEPINTVL) -> %lu\n",
                                     sock, sock->conn->pcb.tcp->keep_intvl));
         break;
      case TCP_KEEPCNT:
         sock->conn->pcb.tcp->keep_cnt = (u32_t)(*(int*)optval);
         LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, IPPROTO_TCP, TCP_KEEPCNT) -> %lu\n",
                                     sock, sock->conn->pcb.tcp->keep_cnt));
         break;
   #endif /* LWIP_TCP_KEEPALIVE */

      } /* switch (optname) */
      break;
#endif /* LWIP_TCP*/
#if LWIP_UDP && LWIP_UDPLITE
   /* Level: IPPROTO_UDPLITE */
   case IPPROTO_UDPLITE:
      switch (optname) {
      case UDPLITE_SEND_CSCOV:
         if ((*(int*)optval != 0) && (*(int*)optval < 8)) {
            /* don't allow illegal values! */
            sock->conn->pcb.udp->chksum_len_tx = 8;
         } else {
            sock->conn->pcb.udp->chksum_len_tx = *(int*)optval;
         }
         LWIP_DEBUGF(SOCKETS_DEBUG,
                     ("lwip_setsockopt(%d, IPPROTO_UDPLITE, UDPLITE_SEND_CSCOV) -> %d\n",
                      sock, (*(int*)optval)) );
         break;
      case UDPLITE_RECV_CSCOV:
         if ((*(int*)optval != 0) && (*(int*)optval < 8)) {
            /* don't allow illegal values! */
            sock->conn->pcb.udp->chksum_len_rx = 8;
         } else {
            sock->conn->pcb.udp->chksum_len_rx = *(int*)optval;
         }
         LWIP_DEBUGF(SOCKETS_DEBUG,
                     ("lwip_setsockopt(%d, IPPROTO_UDPLITE, UDPLITE_RECV_CSCOV) -> %d\n",
                      sock, (*(int*)optval)) );
         break;
      } /* switch (optname) */
      break;
#endif /* LWIP_UDP */
   } /* switch (level) */
   sys_sem_signal(sock->conn->op_completed);
}

/*--------------------------------------------
| Name:        _sys_lwip_setsockopt
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int _sys_lwip_setsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen)
{
   lwip_sock_t *sock;
   int err = ERR_OK;
   struct lwip_setgetsockopt_data_st data;

   sock = _sys_lwip_get_sock(fd);
   if (!sock) {
      set_errno(EBADF);
      return -1;
   }

   if (NULL == optval) {
      sock_set_errno(sock, EFAULT);
      return -1;
   }

   /* Do length and type checks for the various options first, to keep it readable. */
   switch (level) {

/* Level: SOL_SOCKET */
   case SOL_SOCKET:
      switch (optname) {

      case SO_BROADCAST:
      /* UNIMPL case SO_DEBUG: */
      /* UNIMPL case SO_DONTROUTE: */
      case SO_KEEPALIVE:
         /* UNIMPL case case SO_CONTIMEO: */
         /* UNIMPL case case SO_SNDTIMEO: */
#if LWIP_SO_RCVTIMEO
      case SO_RCVTIMEO:
#endif /* LWIP_SO_RCVTIMEO */
#if LWIP_SO_RCVBUF
      case SO_RCVBUF:
#endif /* LWIP_SO_RCVBUF */
       /* UNIMPL case SO_OOBINLINE: */
       /* UNIMPL case SO_SNDBUF: */
       /* UNIMPL case SO_RCVLOWAT: */
       /* UNIMPL case SO_SNDLOWAT: */
#if SO_REUSE
      case SO_REUSEADDR:
      case SO_REUSEPORT:
#endif /* SO_REUSE */
       /* UNIMPL case SO_USELOOPBACK: */
         if (optlen < sizeof(int)) {
            err = EINVAL;
         }
         break;
      case SO_NO_CHECK:
         if (optlen < sizeof(int)) {
            err = EINVAL;
         }
#if LWIP_UDP
         if ((sock->conn->type != NETCONN_UDP) ||
             ((udp_flags(sock->conn->pcb.udp) & UDP_FLAGS_UDPLITE) != 0)) {
            /* this flag is only available for UDP, not for UDP lite */
            err = EAFNOSUPPORT;
         }
#endif /* LWIP_UDP */
         break;
      default:
         LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, SOL_SOCKET, UNIMPL: optname=0x%x, ..)\n",
                                     sock, optname));
         err = ENOPROTOOPT;
      } /* switch (optname) */
      break;

/* Level: IPPROTO_IP */
   case IPPROTO_IP:
      switch (optname) {
      /* UNIMPL case IP_HDRINCL: */
      /* UNIMPL case IP_RCVDSTADDR: */
      /* UNIMPL case IP_RCVIF: */
      case IP_TTL:
      case IP_TOS:
         if (optlen < sizeof(int)) {
            err = EINVAL;
         }
         break;
#if LWIP_IGMP
      case IP_MULTICAST_TTL:
         if (optlen < sizeof(u8_t)) {
            err = EINVAL;
         }
         if (NETCONNTYPE_GROUP(sock->conn->type) != NETCONN_UDP) {
            err = EAFNOSUPPORT;
         }
         break;
      case IP_MULTICAST_IF:
         if (optlen < sizeof(struct in_addr)) {
            err = EINVAL;
         }
         if (NETCONNTYPE_GROUP(sock->conn->type) != NETCONN_UDP) {
            err = EAFNOSUPPORT;
         }
         break;
      case IP_ADD_MEMBERSHIP:
      case IP_DROP_MEMBERSHIP:
         if (optlen < sizeof(struct ip_mreq)) {
            err = EINVAL;
         }
         if (NETCONNTYPE_GROUP(sock->conn->type) != NETCONN_UDP) {
            err = EAFNOSUPPORT;
         }
         break;
#endif /* LWIP_IGMP */
      default:
         LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, IPPROTO_IP, UNIMPL: optname=0x%x, ..)\n",
                                     sock, optname));
         err = ENOPROTOOPT;
      } /* switch (optname) */
      break;

#if LWIP_TCP
/* Level: IPPROTO_TCP */
   case IPPROTO_TCP:
      if (optlen < sizeof(int)) {
         err = EINVAL;
         break;
      }

      /* If this is no TCP socket, ignore any options. */
      if (sock->conn->type != NETCONN_TCP)
         return 0;

      switch (optname) {
      case TCP_NODELAY:
      case TCP_KEEPALIVE:
   #if LWIP_TCP_KEEPALIVE
      case TCP_KEEPIDLE:
      case TCP_KEEPINTVL:
      case TCP_KEEPCNT:
   #endif /* LWIP_TCP_KEEPALIVE */
         break;

      default:
         LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, IPPROTO_TCP, UNIMPL: optname=0x%x, ..)\n",
                                     sock, optname));
         err = ENOPROTOOPT;
      } /* switch (optname) */
      break;
#endif /* LWIP_TCP */
#if LWIP_UDP && LWIP_UDPLITE
/* Level: IPPROTO_UDPLITE */
   case IPPROTO_UDPLITE:
      if (optlen < sizeof(int)) {
         err = EINVAL;
         break;
      }

      /* If this is no UDP lite socket, ignore any options. */
      if (sock->conn->type != NETCONN_UDPLITE)
         return 0;

      switch (optname) {
      case UDPLITE_SEND_CSCOV:
      case UDPLITE_RECV_CSCOV:
         break;

      default:
         LWIP_DEBUGF(SOCKETS_DEBUG,
                     ("lwip_setsockopt(%d, IPPROTO_UDPLITE, UNIMPL: optname=0x%x, ..)\n",
                      sock, optname));
         err = ENOPROTOOPT;
      } /* switch (optname) */
      break;
#endif /* LWIP_UDP && LWIP_UDPLITE */
/* UNDEFINED LEVEL */
   default:
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, level=0x%x, UNIMPL: optname=0x%x, ..)\n",
                                  sock, level, optname));
      err = ENOPROTOOPT;
   } /* switch (level) */


   if (err != ERR_OK) {
      sock_set_errno(sock, err);
      return -1;
   }


   /* Now do the actual option processing */
   data.sock = sock;
   data.level = level;
   data.optname = optname;
   data.optval = (void*)optval;
   data.optlen = &optlen;
   data.err = err;
   tcpip_callback(_sys_lwip_setsockopt_internal, &data);
   sys_arch_sem_wait(sock->conn->op_completed, 0);
   /* maybe _sys_lwip_setsockopt_internal has changed err */
   err = data.err;

   sock_set_errno(sock, err);
   return err ? -1 : 0;
}

/*-------------------------------------------
| Name:dev_lwip_sock_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_lwip_sock_load(void){
   return _sys_lwip_socket_init();
}

/*-------------------------------------------
| Name:dev_lwip_sock_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_lwip_sock_open(desc_t desc,int o_flag){
   hsock_t hsock = _sys_lwip_alloc_socket(desc);
   if(!hsock)
      return -1;
   ofile_lst[desc].ext.hsock = hsock;
   return 0;
}

/*-------------------------------------------
| Name:dev_lwip_sock_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_lwip_sock_close(desc_t desc){
   hsock_t hsock = ofile_lst[desc].ext.hsock;
   if(!hsock)
      return -1;

   if( (ofile_lst[desc].oflag & O_RDONLY)
       && ofile_lst[desc].oflag & O_WRONLY) {

      if(!ofile_lst[desc].nb_reader && !ofile_lst[desc].nb_writer) {
         lwip_sock_t *sock;
         sock = __sys_lwip_desc2sock(desc);
         if(sock)
            _sys_lwip_lowlevel_close(sock);
         _sys_lwip_free_socket(desc);
         ofile_lst[desc].ext.hsock =(hsock_t)0;
      }

   }

   return 0;
}

/*-------------------------------------------
| Name:dev_lwip_sock_isset_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_lwip_sock_isset_read(desc_t desc){
   lwip_sock_t* sock;
   sock = (lwip_sock_t *)(ofile_lst[desc].ext.hsock);
   if(!sock)
      return 0;  //not block devio read
   if(sock->rcvevent>0)
      return 0;

   return -1;
}

/*-------------------------------------------
| Name:dev_lwip_sock_isset_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_lwip_sock_isset_write(desc_t desc){
   lwip_sock_t* sock;
   sock = (lwip_sock_t *)(ofile_lst[desc].ext.hsock);
   if(!sock)
      return 0;  //not block devio write
   if(sock->sendevent>0)
      return 0;
   return -1;
}

/*-------------------------------------------
| Name:dev_lwip_sock_read
| Description:
| Parameters:
| Return Type:
| Comments:    lwip 1.3.0
| See:
---------------------------------------------*/
int dev_lwip_sock_read(desc_t desc,char* mem,int len ){
   //to do: fix ofile_lst[desc]->flags and sock flags with O_NONBLOCK
   return _sys_lwip_lowlevel_read(__sys_lwip_desc2sock(desc),mem, len, 0, NULL, NULL);
}

/*-------------------------------------------
| Name:dev_lwip_sock_write
| Description:
| Parameters:
| Return Type:
| Comments:    lwip 1.3.0
| See:
---------------------------------------------*/
int dev_lwip_sock_write(desc_t desc,const char *data, int size){
   lwip_sock_t *sock;
   int cb;

   sock = __sys_lwip_desc2sock(desc);
   sock->sendevent=1;
   if((cb = _sys_lwip_lowlevel_write(sock,data,size, 0))<0) {
      sock->sendevent=0;
      return -1;
   }
   //__fire_io_int(ofile_lst[desc].owner_pthread_ptr_write);

   return cb;
}

/*-------------------------------------------
| Name:dev_lwip_sock_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_lwip_sock_seek(desc_t desc,int offset,int origin){
   return -1;
}

/*--------------------------------------------
| Name:        dev_lwip_sock_ioctl
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int dev_lwip_sock_ioctl(desc_t desc,int request,va_list ap){

   lwip_sock_t *sock;

   sock = (lwip_sock_t *)(ofile_lst[desc].ext.hsock);
   if (!sock) {
      set_errno(EBADF);
      return -1;
   }
   //
   switch(request) {
   //
   case FIONREAD: {
      int buflen = 0;
      int recv_avail;
      int* p_val=va_arg( ap, int*);

      if (!p_val) {
         sock_set_errno(sock, EINVAL);
         set_errno(EINVAL);
         return -1;
      }

      SYS_ARCH_GET(sock->conn->recv_avail, recv_avail);
      if (recv_avail < 0)
         recv_avail = 0;
      *p_val = recv_avail;

      /* Check if there is data left from the last recv operation. /maq 041215 */
      if (sock->lastdata) {
         buflen = netbuf_len(sock->lastdata);
         buflen -= sock->lastoffset;
         *p_val += buflen;
      }
   }
   break;

   case FIONBIO: {
      int* p_on = va_arg( ap, int*);
      if (p_on && *p_on)
         sock->flags |= O_NONBLOCK;
      else
         sock->flags &= ~O_NONBLOCK;
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_ioctl(%d, FIONBIO, %d)\n", s, !!(sock->flags & O_NONBLOCK)));
      return 0;
   }
   }
   //
   return 0;
}

/*--------------------------------------------
| Name:        _sys_lwip_sock_event_callback
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
static void _sys_lwip_sock_event_callback(struct netconn *conn, enum netconn_evt evt, u16_t len){
   desc_t desc;
   lwip_sock_t* sock;


   /* Get socket */
   if (conn) {
      desc = conn->socket;
      if (desc < 0) {
         /* Data comes in right away after an accept, even though
          * the server task might not have created a new socket yet.
          * Just count down (or up) if that's the case and we
          * will use the data later. Note that only receive events
          * can happen before the new socket is set up. */
         /*
          if (evt == NETCONN_EVT_RCVPLUS)
             conn->socket--;
          */

         sys_sem_wait(socksem);
         if (conn->socket < 0) {
            if (evt == NETCONN_EVT_RCVPLUS) {
               conn->socket--;
            }
            sys_sem_signal(socksem);
            return;
         }
         sys_sem_signal(socksem);
         return;
      }

      sock = (lwip_sock_t *)(ofile_lst[desc].ext.hsock);
      if (!sock)
         return;
   }else{
      return;
   }

   /* Set event as required */
   switch (evt) {
   case NETCONN_EVT_RCVPLUS:
      sock->rcvevent++;
      __fire_io_int(ofile_lst[desc].owner_pthread_ptr_read);
      break;
   case NETCONN_EVT_RCVMINUS:
      sock->rcvevent--;
      break;
   case NETCONN_EVT_SENDPLUS:
      //phlb modif 05/03/2009
      if((sock->flags & O_NONBLOCK)) {
         __fire_io_int(ofile_lst[desc].owner_pthread_ptr_write);
      }
      sock->sendevent = 1;
      break;
   case NETCONN_EVT_SENDMINUS:
      sock->sendevent = 0;
      break;
   }
}


/*--------------------------------------------
| Name:        _sys_lwip_gethostbyname
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
struct hostent* _sys_lwip_gethostbyname(const char *name) {
   err_t err;
   struct ip_addr addr;

   /* buffer variables for lwip_gethostbyname() */
   static struct hostent s_hostent;
   char *s_aliases;
   struct ip_addr s_hostent_addr;
   struct ip_addr *s_phostent_addr;

   /* query host IP address */
   err = netconn_gethostbyname(name, &addr);
   if (err != ERR_OK) {
      LWIP_DEBUGF(DNS_DEBUG, ("lwip_gethostbyname(%s) failed, err=%d\n", name, err));
      return NULL;
   }

   /* fill hostent */
   s_hostent_addr = addr;
   s_phostent_addr = &s_hostent_addr;
   s_hostent.h_name = (char*)name;
   s_hostent.h_aliases = &s_aliases;
   s_hostent.h_addrtype = AF_INET;
   s_hostent.h_length = sizeof(struct ip_addr);
   s_hostent.h_addr_list = (char**)&s_phostent_addr;


#if DNS_DEBUG
   /* dump hostent */
   LWIP_DEBUGF(DNS_DEBUG, ("hostent.h_name           == %s\n",      s_hostent.h_name));
   LWIP_DEBUGF(DNS_DEBUG, ("hostent.h_aliases        == 0x%08lX\n",(u32_t)(s_hostent.h_aliases)));
   if (s_hostent.h_aliases != NULL) {
      u8_t idx;
      for ( idx=0; s_hostent.h_aliases[idx]; idx++) {
         LWIP_DEBUGF(DNS_DEBUG,
                     ("hostent.h_aliases[%i]->   == 0x%08lX\n", idx, s_hostent.h_aliases[idx]));
         LWIP_DEBUGF(DNS_DEBUG,
                     ("hostent.h_aliases[%i]->   == %s\n",      idx, s_hostent.h_aliases[idx]));
      }
   }
   LWIP_DEBUGF(DNS_DEBUG, ("hostent.h_addrtype       == %lu\n",    (u32_t)(s_hostent.h_addrtype)));
   LWIP_DEBUGF(DNS_DEBUG, ("hostent.h_length         == %lu\n",    (u32_t)(s_hostent.h_length)));
   LWIP_DEBUGF(DNS_DEBUG, ("hostent.h_addr_list      == 0x%08lX\n", s_hostent.h_addr_list));
   if (s_hostent.h_addr_list != NULL) {
      u8_t idx;
      for ( idx=0; s_hostent.h_addr_list[idx]; idx++) {
         LWIP_DEBUGF(DNS_DEBUG,
                     ("hostent.h_addr_list[%i]   == 0x%08lX\n", idx, s_hostent.h_addr_list[idx]));
         LWIP_DEBUGF(DNS_DEBUG,
                     ("hostent.h_addr_list[%i]-> == %s\n",      idx,
                      inet_ntoa(*((struct in_addr*)(s_hostent.h_addr_list[idx])))));
      }
   }
#endif /* DNS_DEBUG */

   return &s_hostent;

}

/*============================================
| End of Source  : lwip_socket.c
==============================================*/
