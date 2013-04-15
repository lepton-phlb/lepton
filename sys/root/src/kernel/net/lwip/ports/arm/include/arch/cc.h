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
#ifndef __ARCH_CC_H__
#define __ARCH_CC_H__

/* Include some files for defining library routines */
//#include <string.h>
//#include <ctype.h>

/* Define platform endianness */
#ifndef BYTE_ORDER
   #define BYTE_ORDER LITTLE_ENDIAN
#endif /* BYTE_ORDER */

/* Define generic types used in lwIP */
typedef unsigned char u8_t;
typedef signed char s8_t;
typedef unsigned short u16_t;
typedef signed short s16_t;
typedef unsigned long u32_t;
typedef signed long s32_t;

#define U16_F "hu"
#define S16_F "hd"
#define X16_F "hx"
#define U32_F "lu"
#define S32_F "ld"
#define X32_F "lx"

typedef u32_t mem_ptr_t;

/* Compiler hints for packing structures */
#define PACK_STRUCT_FIELD(x) x
#define PACK_STRUCT_STRUCT
#pragma warning(disable: 4103)
#define PACK_STRUCT_USE_INCLUDES

//#pragma warning(disable: 4103)
//#pragma warning(disable:4761;disable:4244)

/* prototypes for printf() and abort() */

//#include <stdio.h>
//#include <stdlib.h>

/* Plaform specific diagnostic output */

//#define LWIP_PLATFORM_DIAG(x)	do {printf x;} while(0)
/*
#define LWIP_PLATFORM_ASSERT(x) do {printf("Assertion \"%s\" failed at line %d in %s\n", \
                                     x, __LINE__, __FILE__); fflush(NULL); abort();} while(0)
*/


#define LWIP_PLATFORM_DIAG(x)
#define LWIP_PLATFORM_ASSERT(x)

#define INT_MAX ((int)(2147483647))


#endif /* __ARCH_CC_H__ */
