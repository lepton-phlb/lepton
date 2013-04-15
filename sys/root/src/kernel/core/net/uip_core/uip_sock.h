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
#ifndef _OS_SOCKETENGINE_H
#define _OS_SOCKETENGINE_H


/*===========================================
Includes
=============================================*/
#include "kernel/core/types.h"
#include "kernel/core/kernel_pthread.h"

//#include "kernel/net/uip0.9/net/uipopt.h"
//#include "kernel/net/uip1.0/net/uip.h"
//#include "net/uip.h"

#pragma message ("uip 1.0")
#include "kernel/net/uip1.0/net/uip.h"

#include "kernel/core/net/uip_core/uip_socket.h"
#include "kernel/fs/vfs/vfs.h"

/*===========================================
Declaration
=============================================*/

#define SOCKET_BUFFER_SIZE           UIP_TCP_MSS+4

#define RCV_SOCKET_BUFFER_SIZE       UIP_TCP_MSS
#define SND_SOCKET_BUFFER_SIZE       UIP_TCP_MSS

#define MAX_SOCKET_MSGQSIZE      2

#define STATE_SOCKET_CONNECT     0
#define STATE_SOCKET_LISTEN      1
#define STATE_SOCKET_CONNECTED   2
#define STATE_SOCKET_ACCEPTED    3
#define STATE_SOCKET_RECV        4
#define STATE_SOCKET_SEND        5
#define STATE_SOCKET_WAIT        6
#define STATE_SOCKET_CLOSE       7
#define STATE_SOCKET_CLOSED      -1
#define STATE_SOCKET_ABORTED     -2
#define STATE_SOCKET_TIMEDOUT    -3

#define MSG_SOCKET_EVENT         1

//
struct socksconn_state {

   char rcv_buffer[RCV_SOCKET_BUFFER_SIZE+5];
   char snd_buffer[SND_SOCKET_BUFFER_SIZE+5];

   int __r;
   int _r;
   int _w;

   hsock_t hsocks;
};

//
typedef struct {
   desc_t desc;
   //specific only for listen socket
   hsock_t hsocks;
   int fd; //accept socket process file descriptor

   int state; //STATE_SOCKET_LISTEN //STATE_SOCKET_CONNECTED //STATE_SOCKET_CLOSE
   struct _sockaddr_in addr_in;
   int r;
   int w;

   struct socksconn_state* socksconn;
}socket_t;

#define MAX_SOCKET 10

extern socket_t*     socketList;
extern struct socksconn_state*   socksconn_state_list;



/* UIP_APPSTATE_SIZE: The size of the application-specific state
   stored in the uip_conn structure. */
#define UIP_APPSTATE_SIZE (sizeof(struct socksconn_state))

#define FS_STATISTICS 1

extern struct socksconn_state *hs;

#define __WAIT_SOCKET_EVENT(__pthread_ptr__,hsock) __wait_io_int(__pthread_ptr__)
#define __PUT_SOCKET_EVENT(__hsock__,__oflag__){ \
      desc_t __desc__ = ((socket_t*)(__hsock__))->desc; \
      if( (__desc__>=0) && ((__oflag__)&O_RDONLY) ) \
         __fire_io_int(ofile_lst[__desc__].owner_pthread_ptr_read); \
      if( (__desc__>=0) && ((__oflag__)&O_WRONLY) ) \
         __fire_io_int(ofile_lst[__desc__].owner_pthread_ptr_write); \
}

#define __CLR_SOCKET_EVENT(hsock)

hsock_t  sock_get(void);
void     sock_put(hsock_t hsock);
int      socksconn_no(desc_t desc);
int      uip_sock_init(void);
void     uip_sock(void);

/* UIP_APPCALL: the name of the application function. This function
   must return void and take no arguments (i.e., C type "void
   appfunc(void)"). */
#define UIP_APPCALL     uip_sock

//
//to remove: debugging
/*
extern int __debug_sock_state;
extern int __debug_sock_i;
extern unsigned int __debug_sock_rport;
extern unsigned int __debug_sock_lport;
extern unsigned int __debug_wait_incomming_data;
*/

//

#endif
