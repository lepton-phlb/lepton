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


/*============================================
| Compiler Directive
==============================================*/
#ifndef _UIP_CORE_H
#define _UIP_CORE_H


/*============================================
| Includes
==============================================*/
#include "kernel/core/interrupt.h"

/*============================================
| Declaration
==============================================*/

extern kernel_pthread_t uip_core_thread;

#define __make_uip_core_interrupt(__pthread_ptr__,__irq_nb__){ \
      if(__irq_nb__==KERNEL_NET_INTERRUPT_NB) { \
         /*specific for network interface and ip stack*/ \
         OS_ClearEvents(__pthread_ptr__->tcb); \
         OS_SignalEvent (KERNEL_INTERRUPT,uip_core_thread.tcb); \
      } \
}

//
#define __mk_uip_core_syscall(__syscall_nb__,__pdata__){ \
      kernel_pthread_t* __pthread_ptr__; \
      __pthread_ptr__ = kernel_pthread_self(); \
      __syscall_lock(); \
      __pthread_ptr__->reg.from_pthread_ptr = __pthread_ptr__; \
      __pthread_ptr__->reg.syscall=__syscall_nb__; \
      __pthread_ptr__->reg.data=(void*)&__pdata__; \
      __pthread_ptr__->irq_nb=KERNEL_NET_INTERRUPT_NB; \
      __pthread_ptr__->irq_prior=-1; \
      __atomic_in(); \
      __make_uip_core_interrupt(__pthread_ptr__,__pthread_ptr__->irq_nb); \
      __atomic_out(); \
      __wait_ret_int(); \
}

int uip_core_queue_put(uint8_t uip_flag, desc_t desc, void* buf, int size);

unsigned char _uip_core_recv_char(desc_t desc);
void _uip_core_send_char(desc_t desc,unsigned char c);

int uip_core_run(void);


#endif
