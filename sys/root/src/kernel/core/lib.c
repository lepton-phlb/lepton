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
#include "kernel/core/errno.h"
#include "kernel/core/kernel_pthread.h"
#include "kernel/core/kernel.h"
#include "kernel/core/process.h"
#include "kernel/core/system.h"
#include "kernel/core/lib.h"

/*============================================
| Global Declaration
==============================================*/

#ifdef __KERNEL_LOAD_LIB

   #pragma message ("use load lib")
__declare_fct_lib_entrypoint(libc);
__declare_lib_data_offset(libc);

lib_t const lib_lst[]={
   {__implement_fct_lib_entrypoint(libc),&__implement_lib_data_offset(libc)}
};

#endif


/*============================================
| Implementation
==============================================*/

/*--------------------------------------------
| Name:        load_lib
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
#ifdef __KERNEL_LOAD_LIB
int load_lib(kernel_pthread_t* pthread_ptr){
   int i=0;
   lib_data_offset_t lib_data_offset=0;
   //get current pthread stack addr
   for(i=0; i<(sizeof(lib_lst)/sizeof(lib_t)); i++) {

      //only of first library loaded
      if((*lib_lst[i].p_lib_data_offset)==LIB_INVALID_OFFSET) {
         //calc data offset
         lib_data_offset = (unsigned long)(pthread_ptr->heap_top) - (unsigned long)(pthread_ptr->attr.stackaddr);
         *lib_lst[i].p_lib_data_offset=lib_data_offset;
      }
      lib_lst[i].lib_entry_point(pthread_ptr);
   }
   return 0;
}
#endif

/*============================================
| End of Source  : lib.c
==============================================*/
