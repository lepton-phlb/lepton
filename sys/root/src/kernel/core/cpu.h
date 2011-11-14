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
#ifndef _CPU_H
#define _CPU_H

#include "kernel/core/kernelconf.h"
#include "kernel/core/ioctl_cpu.h"

/*===========================================
Includes
=============================================*/
#ifdef TEST_PLL
//#define OS_FSYS 20000000L
#define OS_FSYS __KERNEL_CPU_FREQ
#else
#define OS_FSYS __KERNEL_CPU_FREQ
#endif
#define __cpu_clock_hz OS_FSYS

/*===========================================
Declaration
=============================================*/

int _kernel_cpu_irq(int request, ... );

//
#if defined(__KERNEL_ARCH_DELAY_1US) 
 #if __KERNEL_ARCH_DELAY_1US>0
   /**
    * delai static en micro secondes
    * \hideinitializer
    */
   #define __cpu_static_delay_us(__cpu_delay_us_value__){\
     int volatile __i__, __j__;\
     for (__j__ = 0; __j__ < ((__cpu_delay_us_value__>>2)); __j__++)\
       for(__i__ = 0; __i__ < (__KERNEL_ARCH_DELAY_1US); __i__++) __i__ = __i__;\
   }

   /**
    * delai static en millisecondes 
    * \hideinitializer
    */
   #define __cpu_static_delay_ms(__cpu_delay_ms_value__){\
     int __i_ms__;\
     int volatile __i__, __j__;\
     for (__i_ms__ = 0; __i_ms__ < (__cpu_delay_ms_value__); __i_ms__++)\
       for (__j__ = 0; __j__ < ((__KERNEL_ARCH_DELAY_1US)*330); __j__++)\
         for(__i__ = 0; __i__ < (__KERNEL_ARCH_DELAY_1US); __i__++) __i__ = __i__;\
   }
 #else
   #define __cpu_static_delay_us(__cpu_delay_us_value__)
   #define __cpu_static_delay_ms(__cpu_delay_ms_value__)
 #endif
#else
   #define __cpu_static_delay_us(__cpu_delay_us_value__)
   #define __cpu_static_delay_ms(__cpu_delay_ms_value__)
#endif

#endif
