/******************** (C) COPYRIGHT 2013 IJINUS ********************************
* File Name          : types.h
* Author             : Yoann TREGUIER
* Version            : Drivers demo
* Date               : 2013/05/24
* Description        : Redefined types
*******************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __TYPES_H
#define __TYPES_H

/* Includes ------------------------------------------------------------------*/
//#include <stdio.h>
//#include <stdlib.h>
//#include <stdarg.h>
//#include <string.h>
//#include <ctype.h>
//#include <time.h>

/* Exported define -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
#pragma anon_unions
#pragma pack(4)

typedef enum {FALSE = 0, TRUE = !FALSE} bool;
typedef signed char         s8;
typedef unsigned char       u8;
typedef short               s16;
typedef unsigned short      u16;
typedef int                 s32;
typedef unsigned int        u32;
typedef long long           s64;
typedef unsigned long long  u64;
typedef float               f32;
typedef double              f64;

typedef __packed union
{
  char c[4];
  u32 u;
  s32 s;
  f32 f;
  void *p;
  char *cp;
  u32 *up;
  s32 *sp;
  f32 *fp;
} m32;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

#endif /* __TYPES_H */

/******************* (C) COPYRIGHT 2013 IJINUS ****************END OF FILE*****/
