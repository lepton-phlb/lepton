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
#ifndef _LIB_H
#define _LIB_H


/*============================================
| Includes
==============================================*/
#include "kernel/core/kernelconf.h"
#include "kernel/core/process.h"

/*============================================
| Declaration
==============================================*/
#ifdef __KERNEL_LOAD_LIB
   #pragma message ("use load lib")
#endif

#define LIB_INVALID_OFFSET 0xffffffff

#define __declare_lib_data_offset(__lib_name__) lib_data_offset_t __lib_name__ ## \
   lib_data_offset =LIB_INVALID_OFFSET
#define __extern_lib_data_offset(__lib_name__) extern lib_data_offset_t __lib_name__ ## \
   lib_data_offset
#define __implement_lib_data_offset(__lib_name__) __lib_name__ ## lib_data_offset

#define __lib_data_alloc(__pthread_ptr__, \
                         __size__) (kernel_pthread_alloca(__pthread_ptr__,__size__))
#define __lib_data(__lib_name__) (void*)((char*)kernel_pthread_self()->attr.stackaddr+ \
                                         __implement_lib_data_offset(libc))

#define __declare_fct_lib_entrypoint(__lib_name__) extern __lib_name__ ## _lib_entrypoint( \
      kernel_pthread_t* pthread_ptr)
#define __implement_fct_lib_entrypoint(__lib_name__) __lib_name__ ## _lib_entrypoint



typedef void* plib_t;
typedef unsigned long lib_data_offset_t;

typedef int (*LIB_ENTRY_POINT)(kernel_pthread_t* pthread_ptr);
typedef LIB_ENTRY_POINT lib_entry_point_t;

typedef struct lib_st {
   lib_entry_point_t lib_entry_point;
   lib_data_offset_t* p_lib_data_offset;
}lib_t;

int load_lib(kernel_pthread_t* pthread_ptr);



#endif
