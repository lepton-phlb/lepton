
#include "lwip/opt.h"

#if PPP_SUPPORT

#include "ppp.h"
#include "randm.h"
#include "magic.h"

/***********************************/
/*** PUBLIC FUNCTION DEFINITIONS ***/
/***********************************/
/*
 * magicInit - Initialize the magic number generator.
 *
 * Since we use another random number generator that has its own
 * initialization, we do nothing here.
 */
void magicInit()
{
  return;
}

/*
 * magic - Returns the next magic number.
 */
u32_t magic()
{
  return avRandom();
}

#endif /* PPP_SUPPORT */
