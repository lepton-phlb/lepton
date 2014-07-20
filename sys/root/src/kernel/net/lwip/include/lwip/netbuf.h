#ifndef __LWIP_NETBUF_H__
#define __LWIP_NETBUF_H__

#include "lwip/opt.h"
#include "lwip/pbuf.h"
#include "lwip/ip_addr.h"

#ifdef __cplusplus
extern "C" {
#endif

struct netbuf {
  struct pbuf *p, *ptr;
  struct ip_addr *addr;
  u16_t port;
#if LWIP_NETBUF_RECVINFO
  struct ip_addr *toaddr;
  u16_t toport;
#endif /* LWIP_NETBUF_RECVINFO */
};

/* Network buffer functions: */
struct netbuf *   netbuf_new      (void);
void              netbuf_delete   (struct netbuf *buf);
void *            netbuf_alloc    (struct netbuf *buf, u16_t size);
void              netbuf_free     (struct netbuf *buf);
err_t             netbuf_ref      (struct netbuf *buf,
           const void *dataptr, u16_t size);
void              netbuf_chain    (struct netbuf *head,
           struct netbuf *tail);

u16_t             netbuf_len      (struct netbuf *buf);
err_t             netbuf_data     (struct netbuf *buf,
           void **dataptr, u16_t *len);
s8_t              netbuf_next     (struct netbuf *buf);
void              netbuf_first    (struct netbuf *buf);


#define netbuf_copy_partial(buf, dataptr, len, offset) \
  pbuf_copy_partial((buf)->p, (dataptr), (len), (offset))
#define netbuf_copy(buf,dataptr,len) netbuf_copy_partial(buf, dataptr, len, 0)
#define netbuf_take(buf, dataptr, len) pbuf_take((buf)->p, dataptr, len)
#define netbuf_len(buf)              ((buf)->p->tot_len)
#define netbuf_fromaddr(buf)         ((buf)->addr)
#define netbuf_fromport(buf)         ((buf)->port)
#if LWIP_NETBUF_RECVINFO
#define netbuf_destaddr(buf)         ((buf)->toaddr)
#define netbuf_destport(buf)         ((buf)->toport)
#endif /* LWIP_NETBUF_RECVINFO */

#ifdef __cplusplus
}
#endif

#endif /* __LWIP_NETBUF_H__ */
