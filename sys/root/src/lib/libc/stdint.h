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


/*============================================
| Compiler Directive   
==============================================*/
#ifndef _STDINT_H
#define _STDINT_H


/*============================================
| Includes 
==============================================*/
#include "kernel/core/types.h"
#include "kernel/core/kernelconf.h"

/*============================================
| Declaration  
==============================================*/
#define CHAR_BIT      8         /* number of bits in a char */
#define SCHAR_MIN   (-128)      /* minimum signed char value */
#define SCHAR_MAX     127       /* maximum signed char value */
#define UCHAR_MAX     0xff      /* maximum unsigned char value */

#ifndef _CHAR_UNSIGNED
#define CHAR_MIN    SCHAR_MIN   /* mimimum char value */
#define CHAR_MAX    SCHAR_MAX   /* maximum char value */
#else
#define CHAR_MIN      0
#define CHAR_MAX    UCHAR_MAX
#endif  /* _CHAR_UNSIGNED */

#if __KERNEL_CPU_ARCH == CPU_ARCH_16 
   #define SHRT_MIN    (-32768)           /* minimum (signed) short value */
   #define SHRT_MAX      (32767)          /* maximum (signed) short value */
   #define USHRT_MAX     (0xffff)         /* maximum unsigned short value */
   #define INT_MIN     (-32767 - 1)       /* minimum (signed) int value */
   #define INT_MAX       (32767)          /* maximum (signed) int value */
   #define UINT_MAX      0xffffffff       /* maximum unsigned int value */
   #define LONG_MIN    (-2147483647L - 1) /* minimum (signed) long value */
   #define LONG_MAX      (2147483647L)    /* maximum (signed) long value */
   #define ULONG_MAX     (0xffffffffUL)   /* maximum unsigned long value */
#else
   #define SHRT_MIN    (-32768)           /* minimum (signed) short value */
   #define SHRT_MAX      (32767)          /* maximum (signed) short value */
   #define USHRT_MAX     (0xffff)         /* maximum unsigned short value */
   #define INT_MIN     (-2147483647 - 1)  /* minimum (signed) int value */
   #define INT_MAX       (2147483647)     /* maximum (signed) int value */
   #define UINT_MAX      (0xffffffff)     /* maximum unsigned int value */
   #define LONG_MIN    (-2147483647L - 1) /* minimum (signed) long value */
   #define LONG_MAX      (2147483647L)    /* maximum (signed) long value */
   #define ULONG_MAX     (0xffffffffUL)   /* maximum unsigned long value */
#endif


typedef short int 	int_least8_t;
typedef int 	      int_least16_t;
typedef long int 	   int_least32_t;

typedef unsigned short int uint_least8_t;
typedef unsigned int 	   uint_least16_t;
typedef unsigned long int 	uint_least32_t;

typedef short int 	   int_fast8_t;
typedef int 	         int_fast16_t;
typedef long int 	      int_fast32_t;

typedef unsigned short int 	   uint_fast8_t;
typedef unsigned int 	         uint_fast16_t;
typedef unsigned long int 	      uint_fast32_t;


#if (__KERNEL_CPU_ARCH_SUPPORT_FORMAT>32)
typedef long long int 	         int_least64_t;
typedef long long int 	         int64_t;
typedef unsigned long long int 	uint64_t;
typedef unsigned long long int 	uint_least64_t;
typedef long long int 	         int_fast64_t;
typedef unsigned long long int 	uint_fast64_t;


typedef int64_t 	intptr_t;
typedef uint64_t 	uintptr_t;
typedef int64_t 	intmax_t;
typedef uint64_t 	uintmax_t;

#else

typedef int32_t 	intptr_t;
typedef uint32_t 	uintptr_t;
typedef int32_t 	intmax_t;
typedef uint32_t 	uintmax_t;

#endif

//from $(TOOLCHAIN)/lib/gcc/arm-elf/4.3.3/include/stddef.h
#if defined (__PTRDIFF_TYPE__)
#undef __PTRDIFF_TYPE__
#define __PTRDIFF_TYPE__   int
typedef __PTRDIFF_TYPE__ 	ptrdiff_t;
#endif


#endif
