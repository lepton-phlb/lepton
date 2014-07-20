
#include "uip-neighbor.h"

#include <string.h>

#define MAX_TIME 128

#ifdef UIP_NEIGHBOR_CONF_ENTRIES
#define ENTRIES UIP_NEIGHBOR_CONF_ENTRIES
#else /* UIP_NEIGHBOR_CONF_ENTRIES */
#define ENTRIES 8
#endif /* UIP_NEIGHBOR_CONF_ENTRIES */

struct neighbor_entry {
  uip_ipaddr_t ipaddr;
  struct uip_neighbor_addr addr;
  u8_t time;
};
static struct neighbor_entry entries[ENTRIES];

/*---------------------------------------------------------------------------*/
void
uip_neighbor_init(void)
{
  int i;

  for(i = 0; i < ENTRIES; ++i) {
    entries[i].time = MAX_TIME;
  }
}
/*---------------------------------------------------------------------------*/
void
uip_neighbor_periodic(void)
{
  int i;

  for(i = 0; i < ENTRIES; ++i) {
    if(entries[i].time < MAX_TIME) {
      entries[i].time++;
    }
  }
}
/*---------------------------------------------------------------------------*/
void
uip_neighbor_add(uip_ipaddr_t ipaddr, struct uip_neighbor_addr *addr)
{
  int i, oldest;
  u8_t oldest_time;
#if UIP_LOGGING==1
  printf("Adding neighbor with link address %02x:%02x:%02x:%02x:%02x:%02x\n",
	 addr->addr.addr[0], addr->addr.addr[1], addr->addr.addr[2], addr->addr.addr[3],
	 addr->addr.addr[4], addr->addr.addr[5]);
#endif
  
  /* Find the first unused entry or the oldest used entry. */
  oldest_time = 0;
  oldest = 0;
  for(i = 0; i < ENTRIES; ++i) {
    if(entries[i].time == MAX_TIME) {
      oldest = i;
      break;
    }
    if(uip_ipaddr_cmp(entries[i].ipaddr, addr)) {
      oldest = i;
      break;
    }
    if(entries[i].time > oldest_time) {
      oldest = i;
      oldest_time = entries[i].time;
    }
  }

  /* Use the oldest or first free entry (either pointed to by the
     "oldest" variable). */
  entries[oldest].time = 0;
  uip_ipaddr_copy(entries[oldest].ipaddr, ipaddr);
  memcpy(&entries[oldest].addr, addr, sizeof(struct uip_neighbor_addr));
}
/*---------------------------------------------------------------------------*/
static struct neighbor_entry *
find_entry(uip_ipaddr_t ipaddr)
{
  int i;
  
  for(i = 0; i < ENTRIES; ++i) {
    if(uip_ipaddr_cmp(entries[i].ipaddr, ipaddr)) {
      return &entries[i];
    }
  }
  return NULL;
}
/*---------------------------------------------------------------------------*/
void
uip_neighbor_update(uip_ipaddr_t ipaddr)
{
  struct neighbor_entry *e;

  e = find_entry(ipaddr);
  if(e != NULL) {
    e->time = 0;
  }
}
/*---------------------------------------------------------------------------*/
struct uip_neighbor_addr *
uip_neighbor_lookup(uip_ipaddr_t ipaddr)
{
  struct neighbor_entry *e;

  e = find_entry(ipaddr);
  if(e != NULL) {
    /*    printf("Lookup neighbor with link address %02x:%02x:%02x:%02x:%02x:%02x\n",
	   e->addr.addr.addr[0], e->addr.addr.addr[1], e->addr.addr.addr[2], e->addr.addr.addr[3],
	   e->addr.addr.addr[4], e->addr.addr.addr[5]);*/

    return &e->addr;
  }
  return NULL;
}
/*---------------------------------------------------------------------------*/
