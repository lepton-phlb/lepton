/******************** (C) COPYRIGHT 2013 IJINUS ********************************
* File Name          : types.h
* Author             : Yoann TREGUIER
* Version            : 0.3.0
* Date               : 2013/04/12
* Description        : Redefined types
*******************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SYS_TYPES_H
#define __SYS_TYPES_H

/* Includes ------------------------------------------------------------------*/
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

typedef __packed union
{
  __packed struct
  {
    u32 company_cde1  :8;
    u32 company_cde2  :8;
    u32 product_cde1  :8;
    u32 product_cde2  :8;
    u32 product_cde3  :8;
    u32 product_cnt   :24;
  };
  char tab[8];
} _Id;

#define _Uid_Ver  3
typedef __packed struct
{
  u64 ver       :3;
  u64 ref_cde1  :7;
  u64 ref_cde2  :4;
  u64 ref_cde3  :7;
  u64 ref_cnt   :19;
  u64 bom_id    :8;
  u64 bom_ind1  :8;
  u64 bom_ind2  :8;
  u32 prod_year :8;
  u32 prod_week :8;
  u32 prod_cnt  :16;
} _Uid;

#define _Ver_Ver  2
typedef __packed union
{
  __packed struct
  {
    u32 sw_min  :4;
    u32 sw_maj  :8;
    u32 sw_typ  :2;
    u32 bom_id  :6;
    u32 pcb_id  :10;
    u32 ver     :2;
  };
  u32 val;
} _Ver;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

#endif /* __SYS_TYPES_H */

/******************* (C) COPYRIGHT 2013 IJINUS ****************END OF FILE*****/
