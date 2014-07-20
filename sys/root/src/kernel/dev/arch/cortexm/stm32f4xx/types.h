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
