#ifndef __NETIF_LOOPIF_H__
#define __NETIF_LOOPIF_H__

#include "lwip/opt.h"
#include "lwip/netif.h"
#include "lwip/err.h"

#ifdef __cplusplus
extern "C" {
#endif

#if !LWIP_NETIF_LOOPBACK_MULTITHREADING
#define loopif_poll netif_poll
#endif /* !LWIP_NETIF_LOOPBACK_MULTITHREADING */

err_t loopif_init(struct netif *netif);

#ifdef __cplusplus
}
#endif

#endif /* __NETIF_LOOPIF_H__ */
