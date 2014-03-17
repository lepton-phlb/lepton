#ifndef __NETIF_SLIPIF_H__
#define __NETIF_SLIPIF_H__

#include "lwip/netif.h"

#ifdef __cplusplus
extern "C" {
#endif

err_t slipif_init(struct netif * netif);
void slipif_poll(struct netif *netif);

#ifdef __cplusplus
}
#endif
 
#endif 

