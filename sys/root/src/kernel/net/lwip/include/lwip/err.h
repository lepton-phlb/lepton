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
