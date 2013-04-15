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

/**
 * \file
 *         Header file for database of link-local neighbors, used by
 *         IPv6 code and to be used by future ARP code.
 * \author
 *         Adam Dunkels <adam@sics.se>
 */

#ifndef __UIP_NEIGHBOR_H__
#define __UIP_NEIGHBOR_H__

#include "uip.h"

struct uip_neighbor_addr {
#if UIP_NEIGHBOR_CONF_ADDRTYPE
  UIP_NEIGHBOR_CONF_ADDRTYPE addr;
#else
  struct uip_eth_addr addr;
#endif
};

void uip_neighbor_init(void);
void uip_neighbor_add(uip_ipaddr_t ipaddr, struct uip_neighbor_addr *addr);
void uip_neighbor_update(uip_ipaddr_t ipaddr);
struct uip_neighbor_addr *uip_neighbor_lookup(uip_ipaddr_t ipaddr);
void uip_neighbor_periodic(void);

#endif /* __UIP-NEIGHBOR_H__ */
