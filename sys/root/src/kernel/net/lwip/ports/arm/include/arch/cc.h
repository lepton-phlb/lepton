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
