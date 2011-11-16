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
#ifndef __LWIP_TCPIP_H__
#define __LWIP_TCPIP_H__

#include "lwip/opt.h"

#if !NO_SYS /* don't build if not configured for use in lwipopts.h */

#include "lwip/api_msg.h"
#include "lwip/netifapi.h"
#include "lwip/pbuf.h"
#include "lwip/api.h"
#include "lwip/sys.h"
#include "lwip/netif.h"

#ifdef __cplusplus
extern "C" {
#endif

#if LWIP_TCPIP_CORE_LOCKING
/** The global semaphore to lock the stack. */
extern sys_sem_t lock_tcpip_core;
#define LOCK_TCPIP_CORE()     sys_sem_wait(lock_tcpip_core)
#define UNLOCK_TCPIP_CORE()   sys_sem_signal(lock_tcpip_core)
#define TCPIP_APIMSG(m)       tcpip_apimsg_lock(m)
#define TCPIP_APIMSG_ACK(m)
#define TCPIP_NETIFAPI(m)     tcpip_netifapi_lock(m)
#define TCPIP_NETIFAPI_ACK(m)
#else
#define LOCK_TCPIP_CORE()
#define UNLOCK_TCPIP_CORE()
#define TCPIP_APIMSG(m)       tcpip_apimsg(m)
#define TCPIP_APIMSG_ACK(m)   sys_sem_signal(m->conn->op_completed)
#define TCPIP_NETIFAPI(m)     tcpip_netifapi(m)
#define TCPIP_NETIFAPI_ACK(m) sys_sem_signal(m->sem)
#endif /* LWIP_TCPIP_CORE_LOCKING */

void tcpip_init(void (* tcpip_init_done)(void *), void *arg);

#if LWIP_NETCONN
err_t tcpip_apimsg(struct api_msg *apimsg);
#if LWIP_TCPIP_CORE_LOCKING
err_t tcpip_apimsg_lock(struct api_msg *apimsg);
#endif /* LWIP_TCPIP_CORE_LOCKING */
#endif /* LWIP_NETCONN */

err_t tcpip_input(struct pbuf *p, struct netif *inp);

#if LWIP_NETIF_API
err_t tcpip_netifapi(struct netifapi_msg *netifapimsg);
#if LWIP_TCPIP_CORE_LOCKING
err_t tcpip_netifapi_lock(struct netifapi_msg *netifapimsg);
#endif /* LWIP_TCPIP_CORE_LOCKING */
#endif /* LWIP_NETIF_API */

err_t tcpip_callback_with_block(void (*f)(void *ctx), void *ctx, u8_t block);
#define tcpip_callback(f, ctx)              tcpip_callback_with_block(f, ctx, 1)

/* free pbufs or heap memory from another context without blocking */
err_t pbuf_free_callback(struct pbuf *p);
err_t mem_free_callback(void *m);

err_t tcpip_timeout(u32_t msecs, sys_timeout_handler h, void *arg);
err_t tcpip_untimeout(sys_timeout_handler h, void *arg);

enum tcpip_msg_type {
#if LWIP_NETCONN
  TCPIP_MSG_API,
#endif /* LWIP_NETCONN */
  TCPIP_MSG_INPKT,
#if LWIP_NETIF_API
  TCPIP_MSG_NETIFAPI,
#endif /* LWIP_NETIF_API */
  TCPIP_MSG_CALLBACK,
  TCPIP_MSG_TIMEOUT,
  TCPIP_MSG_UNTIMEOUT
};

struct tcpip_msg {
  enum tcpip_msg_type type;
  sys_sem_t *sem;
  union {
#if LWIP_NETCONN
    struct api_msg *apimsg;
#endif /* LWIP_NETCONN */
#if LWIP_NETIF_API
    struct netifapi_msg *netifapimsg;
#endif /* LWIP_NETIF_API */
    struct {
      struct pbuf *p;
      struct netif *netif;
    } inp;
    struct {
      void (*f)(void *ctx);
      void *ctx;
    } cb;
    struct {
      u32_t msecs;
      sys_timeout_handler h;
      void *arg;
    } tmo;
  } msg;
};

#ifdef __cplusplus
}
#endif

#endif /* !NO_SYS */

#endif /* __LWIP_TCPIP_H__ */
