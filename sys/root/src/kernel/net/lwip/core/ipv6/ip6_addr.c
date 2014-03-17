
#include "lwip/opt.h"
#include "lwip/ip_addr.h"
#include "lwip/inet.h"

u8_t
ip_addr_netcmp(struct ip_addr *addr1, struct ip_addr *addr2,
                struct ip_addr *mask)
{
  return((addr1->addr[0] & mask->addr[0]) == (addr2->addr[0] & mask->addr[0]) &&
         (addr1->addr[1] & mask->addr[1]) == (addr2->addr[1] & mask->addr[1]) &&
         (addr1->addr[2] & mask->addr[2]) == (addr2->addr[2] & mask->addr[2]) &&
         (addr1->addr[3] & mask->addr[3]) == (addr2->addr[3] & mask->addr[3]));
        
}

u8_t
ip_addr_cmp(struct ip_addr *addr1, struct ip_addr *addr2)
{
  return(addr1->addr[0] == addr2->addr[0] &&
         addr1->addr[1] == addr2->addr[1] &&
         addr1->addr[2] == addr2->addr[2] &&
         addr1->addr[3] == addr2->addr[3]);
}

void
ip_addr_set(struct ip_addr *dest, struct ip_addr *src)
{
  SMEMCPY(dest, src, sizeof(struct ip_addr));
  /*  dest->addr[0] = src->addr[0];
  dest->addr[1] = src->addr[1];
  dest->addr[2] = src->addr[2];
  dest->addr[3] = src->addr[3];*/
}

u8_t
ip_addr_isany(struct ip_addr *addr)
{
  if (addr == NULL) return 1;
  return((addr->addr[0] | addr->addr[1] | addr->addr[2] | addr->addr[3]) == 0);
}
