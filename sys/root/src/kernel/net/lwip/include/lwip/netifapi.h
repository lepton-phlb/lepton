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
 
#ifndef __LWIP_NETIFAPI_H__
#define __LWIP_NETIFAPI_H__

#include "lwip/opt.h"

#if LWIP_NETIF_API /* don't build if not configured for use in lwipopts.h */

#include "lwip/sys.h"
#include "lwip/netif.h"
#include "lwip/dhcp.h"
#include "lwip/autoip.h"

#ifdef __cplusplus
extern "C" {
#endif

struct netifapi_msg_msg {
#if !LWIP_TCPIP_CORE_LOCKING
  sys_sem_t sem;
#endif /* !LWIP_TCPIP_CORE_LOCKING */
  err_t err;
  struct netif *netif;
  union {
    struct {
      struct ip_addr *ipaddr;
      struct ip_addr *netmask;
      struct ip_addr *gw;
      void *state;
      err_t (* init) (struct netif *netif);
      err_t (* input)(struct pbuf *p, struct netif *netif);
    } add;
    struct {
      void  (* voidfunc)(struct netif *netif);
      err_t (* errtfunc)(struct netif *netif);
    } common;
  } msg;
};

struct netifapi_msg {
  void (* function)(struct netifapi_msg_msg *msg);
  struct netifapi_msg_msg msg;
};


/* API for application */
err_t netifapi_netif_add       ( struct netif *netif,
                                 struct ip_addr *ipaddr,
                                 struct ip_addr *netmask,
                                 struct ip_addr *gw,
                                 void *state,
                                 err_t (* init)(struct netif *netif),
                                 err_t (* input)(struct pbuf *p, struct netif *netif) );

err_t netifapi_netif_set_addr  ( struct netif *netif,
                                 struct ip_addr *ipaddr,
                                 struct ip_addr *netmask,
                                 struct ip_addr *gw );

err_t netifapi_netif_common    ( struct netif *netif,
                                 void  (* voidfunc)(struct netif *netif),
                                 err_t (* errtfunc)(struct netif *netif) );

#define netifapi_netif_remove(n)      netifapi_netif_common(n, netif_remove, NULL)
#define netifapi_netif_set_up(n)      netifapi_netif_common(n, netif_set_up, NULL)
#define netifapi_netif_set_down(n)    netifapi_netif_common(n, netif_set_down, NULL)
#define netifapi_netif_set_default(n) netifapi_netif_common(n, netif_set_default, NULL)
#define netifapi_dhcp_start(n)        netifapi_netif_common(n, NULL, dhcp_start)
#define netifapi_dhcp_stop(n)         netifapi_netif_common(n, dhcp_stop, NULL)
#define netifapi_autoip_start(n)      netifapi_netif_common(n, NULL, autoip_start)
#define netifapi_autoip_stop(n)       netifapi_netif_common(n, NULL, autoip_stop)

#ifdef __cplusplus
}
#endif

#endif /* LWIP_NETIF_API */

#endif /* __LWIP_NETIFAPI_H__ */
