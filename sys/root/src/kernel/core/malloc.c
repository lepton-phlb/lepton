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


/*===========================================
Includes
=============================================*/
#include <stdlib.h>
#include "kernel/core/kernel_pthread.h"
#include "kernel/core/kernel.h"
#include "kernel/core/process.h"
#include "kernel/core/fcntl.h"
#include "kernel/core/bin.h"
#include "kernel/fs/vfs/vfs.h"

/*===========================================
Global Declaration
=============================================*/


/*===========================================
Implementation
=============================================*/

/*-------------------------------------------
| Name:_sys_malloc
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void* _sys_malloc(size_t size){
   void* p;

#if !defined(GNU_GCC)
   __atomic_in();
   __disable_interrupt_section_in();
#endif

   p=malloc(size);
   if(!p)
      p=(void*)0;

#if !defined(GNU_GCC)
   __disable_interrupt_section_out();
   __atomic_out();
#endif
   return p;
}

/*-------------------------------------------
| Name:_sys_calloc
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void *_sys_calloc(size_t nelem, size_t elsize){
   void* p;

#if !defined(GNU_GCC)
   __atomic_in();
   __disable_interrupt_section_in();
#endif

   p=calloc(nelem,elsize);
   if(!p)
      p=(void*)0;

#if !defined(GNU_GCC)
   __disable_interrupt_section_out();
   __atomic_out();
#endif

   return p;
}

/*-------------------------------------------
| Name:__realloc
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void *_sys_realloc(void *p, size_t size){

#if !defined(GNU_GCC)
   __atomic_in();
   __disable_interrupt_section_in();
#endif

   p=realloc(p,size);
   if(!p)
      p=(void*)0;

#if !defined(GNU_GCC)
   __disable_interrupt_section_out();
   __atomic_out();
#endif

   return p;
}

/*-------------------------------------------
| Name:_sys_free
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void _sys_free (void* p){

#if !defined(GNU_GCC)
   __atomic_in();
   __disable_interrupt_section_in();
#endif

   free(p);

#if !defined(GNU_GCC)
   __disable_interrupt_section_out();
   __atomic_out();
#endif
}


/*===========================================
End of Source malloc.c
=============================================*/
