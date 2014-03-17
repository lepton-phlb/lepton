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

#ifdef USE_ECOS
   #include <cyg/kernel/kapi.h>
#endif

#include "kernel/core/kernel.h"
/*============================================
| Global Declaration
==============================================*/

#ifdef USE_ECOS
//add constructor priority behind last ecos priority constructor (strtok priority's 56000)
//see cyg_hal_invoke_constructors() in file $(ECOS_REPOSITORY)/hal/arm/arch/current/src/hal_misc.c
//see  sys/root/src//kernel/core/core-ecos/process.c (first tauon processus (initd) call tauon constructor chained list).
//tauon posix syscalls in constructor are supported.
void tauon_cplusplus_ctor(void) __attribute__((constructor(56001)));
#endif

//dummy add for compilation with arm-elf
#if defined(CPU_ARM9)
void * _impure_ptr;
#endif
/*============================================
| Implementation
==============================================*/
void cyg_user_start(void)
{
   //lancement du noyau
   _start_kernel(NULL);
}

//dummy cplusplus constructor to locate c++ apps global variale constructor
#ifdef USE_ECOS
void tauon_cplusplus_ctor(void) {
}
#endif
/*============================================
| End of Source  : tauon.c
|---------------------------------------------
| Historic:
|---------------------------------------------
| Authors     | Date     | Comments
| $Log: not supported by cvs2svn $
| Revision 1.3  2009/08/25 14:01:44  jjp
| bug fix for static target
|
| Revision 1.2  2009/07/21 08:03:52  jjp
| improve c++ constructor
|
| Revision 1.1  2009/03/30 15:48:57  jjp
| first import of tauon
|
| Revision 1.1  2009/03/30 11:18:54  jjp
| First import of tauon
|
|---------------------------------------------
==============================================*/
