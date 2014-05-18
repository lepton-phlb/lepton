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
#ifndef __LWIP_RAW_H__
#define __LWIP_RAW_H__

#include "lwip/opt.h"

#if LWIP_RAW /* don't build if not configured for use in lwipopts.h */

#include "lwip/pbuf.h"
#include "lwip/inet.h"
#include "lwip/ip.h"
#include "lwip/ip_addr.h"

#ifdef __cplusplus
extern "C" {
#endif

struct raw_pcb {
/* Common members of all PCB types */
  IP_PCB;

  struct raw_pcb *next;

  u8_t protocol;

  /* receive callback function
   * @param arg user supplied argument (raw_pcb.recv_arg)
   * @param pcb the raw_pcb which received data
   * @param p the packet buffer that was received
   * @param addr the remote IP address from which the packet was received
   * @return 1 if the packet was 'eaten' (aka. deleted),
   *         0 if the packet lives on
   * If returning 1, the callback is responsible for freeing the pbuf
   * if it's not used any more.
   */
  u8_t (* recv)(void *arg, struct raw_pcb *pcb, struct pbuf *p,
    struct ip_addr *addr);
  /* user-supplied argument for the recv callback */
  void *recv_arg;
};

/* The following functions is the application layer interface to the
   RAW code. */
struct raw_pcb * raw_new        (u8_t proto);
void             raw_remove     (struct raw_pcb *pcb);
err_t            raw_bind       (struct raw_pcb *pcb, struct ip_addr *ipaddr);
err_t            raw_connect    (struct raw_pcb *pcb, struct ip_addr *ipaddr);

void             raw_recv       (struct raw_pcb *pcb,
                                 u8_t (* recv)(void *arg, struct raw_pcb *pcb,
                                              struct pbuf *p,
                                              struct ip_addr *addr),
                                 void *recv_arg);
err_t            raw_sendto     (struct raw_pcb *pcb, struct pbuf *p, struct ip_addr *ipaddr);
err_t            raw_send       (struct raw_pcb *pcb, struct pbuf *p);

/* The following functions are the lower layer interface to RAW. */
u8_t             raw_input      (struct pbuf *p, struct netif *inp);
#define raw_init() /* Compatibility define, not init needed. */

#ifdef __cplusplus
}
#endif

#endif /* LWIP_RAW */

#endif /* __LWIP_RAW_H__ */
