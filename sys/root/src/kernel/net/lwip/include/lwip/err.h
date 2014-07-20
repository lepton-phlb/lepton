#ifndef __LWIP_ERR_H__
#define __LWIP_ERR_H__

#include "lwip/opt.h"
#include "lwip/arch.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Define LWIP_ERR_T in cc.h if you want to use
 *  a different type for your platform (must be signed). */
#ifdef LWIP_ERR_T
typedef LWIP_ERR_T err_t;
#else /* LWIP_ERR_T */
 typedef s8_t err_t;
#endif /* LWIP_ERR_T*/

/* Definitions for error constants. */

#define ERR_OK          0    /* No error, everything OK. */
#define ERR_MEM        -1    /* Out of memory error.     */
#define ERR_BUF        -2    /* Buffer error.            */
#define ERR_TIMEOUT    -3    /* Timeout.                 */
#define ERR_RTE        -4    /* Routing problem.         */

#define ERR_IS_FATAL(e) ((e) < ERR_RTE)

#define ERR_ABRT       -5    /* Connection aborted.      */
#define ERR_RST        -6    /* Connection reset.        */
#define ERR_CLSD       -7    /* Connection closed.       */
#define ERR_CONN       -8    /* Not connected.           */

#define ERR_VAL        -9    /* Illegal value.           */

#define ERR_ARG        -10   /* Illegal argument.        */

#define ERR_USE        -11   /* Address in use.          */

#define ERR_IF         -12   /* Low-level netif error    */
#define ERR_ISCONN     -13   /* Already connected.       */

#define ERR_INPROGRESS -14   /* Operation in progress    */


#ifdef LWIP_DEBUG
extern const char *lwip_strerr(err_t err);
#else
#define lwip_strerr(x) ""
#endif /* LWIP_DEBUG */

#ifdef __cplusplus
}
#endif

#endif /* __LWIP_ERR_H__ */
