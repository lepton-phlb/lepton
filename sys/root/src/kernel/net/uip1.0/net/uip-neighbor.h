
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
