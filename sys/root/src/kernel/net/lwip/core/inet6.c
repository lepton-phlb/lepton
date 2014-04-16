

/* inet6.c
 *
 * Functions common to all TCP/IP modules, such as the Internet checksum and the
 * byte order functions.
 *
 */


#include "lwip/opt.h"

#include "lwip/def.h"
#include "lwip/inet.h"



/* chksum:
 *
 * Sums up all 16 bit words in a memory portion. Also includes any odd byte.
 * This function is used by the other checksum functions.
 *
 * For now, this is not optimized. Must be optimized for the particular processor
 * arcitecture on which it is to run. Preferebly coded in assembler.
 */

static u32_t
chksum(void *dataptr, u16_t len)
{
  u16_t *sdataptr = dataptr;
  u32_t acc;
  
  
  for(acc = 0; len > 1; len -= 2) {
    acc += *sdataptr++;
  }

  /* add up any odd byte */
  if (len == 1) {
    acc += htons((u16_t)(*(u8_t *)dataptr) << 8);
  }

  return acc;

}

/* inet_chksum_pseudo:
 *
 * Calculates the pseudo Internet checksum used by TCP and UDP for a pbuf chain.
 */

u16_t
inet_chksum_pseudo(struct pbuf *p,
       struct ip_addr *src, struct ip_addr *dest,
       u8_t proto, u32_t proto_len)
{
  u32_t acc;
  struct pbuf *q;
  u8_t swapped, i;

  acc = 0;
  swapped = 0;
  for(q = p; q != NULL; q = q->next) {    
    acc += chksum(q->payload, q->len);
    while (acc >> 16) {
      acc = (acc & 0xffff) + (acc >> 16);
    }
    if (q->len % 2 != 0) {
      swapped = 1 - swapped;
      acc = ((acc & 0xff) << 8) | ((acc & 0xff00) >> 8);
    }
  }

  if (swapped) {
    acc = ((acc & 0xff) << 8) | ((acc & 0xff00) >> 8);
  }
  
  for(i = 0; i < 8; i++) {
    acc += ((u16_t *)src->addr)[i] & 0xffff;
    acc += ((u16_t *)dest->addr)[i] & 0xffff;
    while (acc >> 16) {
      acc = (acc & 0xffff) + (acc >> 16);
    }
  }
  acc += (u16_t)htons((u16_t)proto);
  acc += ((u16_t *)&proto_len)[0] & 0xffff;
  acc += ((u16_t *)&proto_len)[1] & 0xffff;

  while (acc >> 16) {
    acc = (acc & 0xffff) + (acc >> 16);
  }
  return ~(acc & 0xffff);
}

/* inet_chksum:
 *
 * Calculates the Internet checksum over a portion of memory. Used primarely for IP
 * and ICMP.
 */

u16_t
inet_chksum(void *dataptr, u16_t len)
{
  u32_t acc, sum;

  acc = chksum(dataptr, len);
  sum = (acc & 0xffff) + (acc >> 16);
  sum += (sum >> 16);
  return ~(sum & 0xffff);
}

u16_t
inet_chksum_pbuf(struct pbuf *p)
{
  u32_t acc;
  struct pbuf *q;
  u8_t swapped;
  
  acc = 0;
  swapped = 0;
  for(q = p; q != NULL; q = q->next) {
    acc += chksum(q->payload, q->len);
    while (acc >> 16) {
      acc = (acc & 0xffff) + (acc >> 16);
    }    
    if (q->len % 2 != 0) {
      swapped = 1 - swapped;
      acc = (acc & 0xff << 8) | (acc & 0xff00 >> 8);
    }
  }
 
  if (swapped) {
    acc = ((acc & 0xff) << 8) | ((acc & 0xff00) >> 8);
  }
  return ~(acc & 0xffff);
}

