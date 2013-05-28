/*
The contents of this file are subject to the Mozilla Public License Version 1.1
(the "License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License for the
specific language governing rights and limitations under the License.

The Original Code is Lepton.

The Initial Developer of the Original Code is Philippe Le Boulanger.
Portions created by Philippe Le Boulanger are Copyright (C) 2011 <lepton.phlb@gmail.com>.
All Rights Reserved.

Contributor(s): Jean-Jacques Pitrolle <lepton.jjp@gmail.com>.

Alternatively, the contents of this file may be used under the terms of the eCos GPL license
(the  [eCos GPL] License), in which case the provisions of [eCos GPL] License are applicable
instead of those above. If you wish to allow use of your version of this file only under the
terms of the [eCos GPL] License and not to allow others to use your version of this file under
the MPL, indicate your decision by deleting  the provisions above and replace
them with the notice and other provisions required by the [eCos GPL] License.
If you do not delete the provisions above, a recipient may use your version of this file under
either the MPL or the [eCos GPL] License."
*/

/**
 * \addtogroup lepton_kernel
 * @{
 */

/**
 * \addtogroup kernel_sem les kernel semaphore
 * @{
 *
 */


/*============================================
| Compiler Directive
==============================================*/
#ifndef _KERNEL_SEM_H
#define _KERNEL_SEM_H


/*============================================
| Includes
==============================================*/

#include "kernel/core/kernelconf.h"
#include "kernel/core/kal.h"

#if (__tauon_compiler__==__compiler_keil_arm__) || (__tauon_compiler__==__compiler_gnuc__)
   #include "kernel/core/timer.h"
#endif

/*============================================
| Declaration
==============================================*/

typedef struct kernel_sem_st {
#ifdef __KERNEL_UCORE_EMBOS
   OS_CSEMA sem;
#endif
#ifdef __KERNEL_UCORE_ECOS
   cyg_sem_t sem;
#endif
}kernel_sem_t;

int            kernel_sem_close(kernel_sem_t * );
int            kernel_sem_destroy(kernel_sem_t *);
int            kernel_sem_getvalue(kernel_sem_t *, int *);
int            kernel_sem_init(kernel_sem_t *, int, unsigned int);
kernel_sem_t * kernel_sem_open(const char *, int, ...);
int            kernel_sem_post(kernel_sem_t *);

int            kernel_sem_trywait(kernel_sem_t *);
int            kernel_sem_unlink(const char *);
int            kernel_sem_wait(kernel_sem_t *);
int            kernel_sem_timedwait(kernel_sem_t *, int, const struct timespec *);

/** @} */
/** @} */

#endif
