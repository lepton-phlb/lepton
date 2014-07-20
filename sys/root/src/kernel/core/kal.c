/*
The contents of this file are subject to the Mozilla Public License Version 1.1
(the "License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License for the
specific language governing rights and limitations under the License.

The Original Code is Lepton.

The Initial Developer of the Original Code is Chauvin-Arnoux.
Portions created by Chauvin-Arnoux are Copyright (C) 2011. All Rights Reserved.

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
| Includes
==============================================*/
#include <stdlib.h>
#include "kernel/core/kernel_pthread.h"

/*============================================
| Global Declaration
==============================================*/


/*============================================
| Implementation
==============================================*/

/*--------------------------------------------
| Name:        _debug_bckup_thread_start_context
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
#ifdef CPU_WIN32
void _debug_bckup_thread_start_context(CONTEXT* __context__,struct kernel_pthread_st *__pthread_ptr__){
   CONTEXT __local_context__;
   __local_context__.ContextFlags=PTHREAD_CONTEXT_DUMP;
   GetThreadContext(__pthread_ptr__->tcb->hTask,&__local_context__);
   memcpy(__context__,&__local_context__,sizeof(CONTEXT));
   _dbg_printf("start_context.Esp=0x%08x\r\n",__pthread_ptr__->start_context.Esp);
}
#endif
/*--------------------------------------------
| Name:        _debug_bckup_context
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
#ifdef CPU_WIN32
void _debug_bckup_context(CONTEXT* __context__,kernel_pthread_t *__pthread_ptr__){
   CONTEXT __local_context__;
   __local_context__.ContextFlags=PTHREAD_CONTEXT_DUMP;
   GetThreadContext(__pthread_ptr__->tcb->hTask,&__local_context__);
   memcpy(__context__,&__local_context__,sizeof(CONTEXT));
}
#endif
/*--------------------------------------------
| Name:        _debug_rstr_context
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
#ifdef CPU_WIN32
void _debug_rstr_context(CONTEXT* __context__,kernel_pthread_t *__pthread_ptr__){
   CONTEXT __local_context__;
   __context__->ContextFlags=PTHREAD_CONTEXT_DUMP;
   memcpy(&__local_context__,__context__,sizeof(CONTEXT));
   SetThreadContext(__pthread_ptr__->tcb->hTask, &__local_context__);
}
#endif
/*--------------------------------------------
| Name:        _debug_bckup_stack
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
#ifdef CPU_WIN32
int _debug_bckup_stack(kernel_pthread_t *__pthread_ptr__){
   int offsetEsp;
   void* src_stack_ptr;
   _dbg_printf("backup.Esp=0x%08x  start_context.Esp=0x%08x\r\n",__pthread_ptr__->bckup_context.Esp,__pthread_ptr__->start_context.Esp);
   offsetEsp = __pthread_ptr__->bckup_context.Esp - __pthread_ptr__->start_context.Esp;
   src_stack_ptr = (PDWORD)(__pthread_ptr__->start_context.Esp+offsetEsp);
   __pthread_ptr__->bckup_stack = (char*)malloc( abs(offsetEsp) );
   if(!__pthread_ptr__->bckup_stack)
      return -1;
   _dbg_printf("bckup_stack:0x%08x src_stack_ptr:0x%08x , offsetEsp=%d \r\n",__pthread_ptr__->bckup_stack,src_stack_ptr,offsetEsp);
   memcpy(__pthread_ptr__->bckup_stack,src_stack_ptr,abs(offsetEsp));
   return 0;
}
#endif
/*--------------------------------------------
| Name:        _debug_rstr_stack
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
#ifdef CPU_WIN32
void _debug_rstr_stack(kernel_pthread_t *__pthread_ptr__){
   int offsetEsp;
   void* src_stack_ptr;
   offsetEsp = __pthread_ptr__->bckup_context.Esp - __pthread_ptr__->start_context.Esp;
   src_stack_ptr = (PDWORD)(__pthread_ptr__->start_context.Esp+offsetEsp);
   memcpy(src_stack_ptr,__pthread_ptr__->bckup_stack,abs(offsetEsp));
   free(__pthread_ptr__->bckup_stack);
}
#endif

/*--------------------------------------------
| Name:        _debug_swap_signal_handler
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
#ifdef CPU_WIN32
void _debug_swap_signal_handler(kernel_pthread_t *__pthread_ptr__,void* sig_handler){
   context_t context;
   _debug_bckup_context(&context,__pthread_ptr__);
   context.Eip=(unsigned long)(sig_handler);
   _debug_rstr_context(&context,__pthread_ptr__);
   if(__pthread_ptr__->tcb->hCurrentWaitObject)
      SetEvent(__pthread_ptr__->tcb->hCurrentWaitObject);
}
#endif

/*--------------------------------------------
| Name:        _debug_inline_exit_signal_handler
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
#ifdef CPU_WIN32
void _debug_inline_exit_signal_handler(kernel_pthread_t *__pthread_ptr__){
   _debug_rstr_context(&__pthread_ptr__->bckup_context,__pthread_ptr__);
}
#endif

/*============================================
| End of Source  : kal.c
==============================================*/
