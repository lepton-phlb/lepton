/*
The contents of this file are subject to the Mozilla Public License Version 1.1
(the "License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License for the
specific language governing rights and limitations under the License.

The Original Code is Lepton.

The Initial Developer of the Original Code is Philippe Le Boulanger.
Portions created by Philippe Le Boulanger are Copyright (C) 2014 <lepton.phlb@gmail.com>.
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
