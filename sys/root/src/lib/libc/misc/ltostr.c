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

/*===========================================
Includes
=============================================*/
#include <stdlib.h>
#include "kernel/core/kernelconf.h"
#include "lib/libc/misc/ltostr.h"

/*===========================================
Global Declaration
=============================================*/
//static char buf[34];

//extern char * ultostr(unsigned long val,int radix);


/*===========================================
Implementation
=============================================*/

/*-------------------------------------------
| Name:ltostr
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
char * ltostr(char* buf,int len,long val,int radix, int uppercase){
   char *p=buf;
   int flg = 0;
   if( val < 0 ) { flg++; val= -val; }
   p = ultostr(buf,len,val, radix,uppercase);
   if(p && flg) *--p = '-';
   return p;
}

/*-------------------------------------------
| Name:ultostr
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
char * ultostr(char* buf,int len,unsigned long val,int radix,int uppercase){
   register char *p=buf;
   register int c;

   if( radix > 36 || radix < 2 ) return 0;

   p = buf+len;
   *--p = '\0';

   do
   {
#if __KERNEL_CPU_ARCH == CPU_ARCH_32
      c = val%radix;
      val/=radix;
#else
      ldiv_t lr = ldiv(val,radix);
      c = abs(lr.rem);   //abs fix problem with 0x80000000
      val= lr.quot;
#endif

      if( c > 9 ) *--p = (uppercase ? 'A' : 'a')-10+c; else *--p = '0'+c;
   }
   while(val);
   return p;
}


/*
 * Copyright (C) 2000 Manuel Novoa III
 *
 * Note: buf is a pointer to the END of the buffer passed.
 * Call like this;
 *     char buf[SIZE], *p;
 *     p = __ultostr(buf + sizeof(buf) - 1, ...)
 *
 * For longs of 32 bits, appropriate buffer sizes are:
 *     base =  2      33  = 32 digits + 1 nul
 *     base = 10      11  = 10 digits + 1 nul
 *     base = 16       9  = 8 hex digits + 1 nul
 */

char *__ultostr(char *buf, unsigned long uval, int base, int uppercase)
{
   int digit;

   if ((base < 2) || (base > 36)) {
      return 0;
   }

   *buf = '\0';

   do {
      digit = uval % base;
      uval /= base;

      /* note: slightly slower but generates less code */
      *--buf = '0' + digit;
      if (digit > 9) {
         *buf = (uppercase ? 'A' : 'a') + digit - 10;
      }
   } while (uval);

   return buf;
}

/*
 * Copyright (C) 2000 Manuel Novoa III
 *
 * Note: buf is a pointer to the END of the buffer passed.
 * Call like this:
 *     char buf[SIZE], *p;
 *     p = __ltostr(buf + sizeof(buf) - 1, ...)
 *
 * For longs of 32 bits, appropriate buffer sizes are:
 *     base =  2      34  = 1 (possible -) sign + 32 digits + 1 nul
 *     base = 10      12  = 1 (possible -) sign + 10 digits + 1 nul
 *     base = 16      10  = 1 (possible -) sign + 8 hex digits + 1 nul
 */

extern char *__ultostr(char *buf, unsigned long uval, int base, int uppercase);


char *__ltostr(char *buf, long val, int base, int uppercase)
{
   unsigned long uval;
   char *pos;
   int negative;

   negative = 0;
   if (val < 0) {
      negative = 1;
      uval = ((unsigned long)(-(1+val))) + 1;
   } else {
      uval = val;
   }


   pos = __ultostr(buf, uval, base, uppercase);

   if (pos && negative) {
      *--pos = '-';
   }

   return pos;
}

//only for 64 bits format support (compiler).
#if (__KERNEL_COMPILER_SUPPORT_TYPE>__KERNEL_COMPILER_SUPPORT_32_BITS_TYPE)
/*
 * Copyright (C) 2000 Manuel Novoa III
 *
 * Note: buf is a pointer to the END of the buffer passed.
 * Call like this:
 *     char buf[SIZE], *p;
 *     p = __ulltostr(buf + sizeof(buf) - 1, ...)
 *
 * For long longs of 64 bits, appropriate buffer sizes are:
 *     base =  2      65  = 64 digits + 1 nul
 *     base = 10      20  = 19 digits + 1 nul
 *     base = 16      17  = 16 hex digits + 1 nul
 */

char *__ulltostr(char *buf, unsigned long long uval, int base, int uppercase)
{
   int digit;

   if ((base < 2) || (base > 36)) {
      return 0;
   }

   *buf = '\0';

   do {
      digit = uval % base;
      uval /= base;

      /* note: slightly slower but generates less code */
      *--buf = '0' + digit;
      if (digit > 9) {
         *buf = (uppercase ? 'A' : 'a') + digit - 10;
      }
   } while (uval);

   return buf;
}

/*
 * Copyright (C) 2000 Manuel Novoa III
 *
 * Note: buf is a pointer to the END of the buffer passed.
 * Call like this:
 *     char buf[SIZE], *p;
 *     p = __lltostr(buf + sizeof(buf) - 1, ...)
 * For long longs of 64 bits, appropriate buffer sizes are:
 *     base =  2      66  = 1 (possible -) sign + 64 digits + 1 nul
 *     base = 10      21  = 1 (possible -) sign + 19 digits + 1 nul
 *     base = 16      18  = 1 (possible -) sign + 16 hex digits + 1 nul
 */

extern char *__ulltostr(char *buf, unsigned long long uval, int base,
                        int uppercase);

char *__lltostr(char *buf, long long val, int base, int uppercase)
{
   unsigned long long uval;
   char *pos;
   int negative;

   negative = 0;
   if (val < 0) {
      negative = 1;
      uval = ((unsigned long long)(-(1+val))) + 1;
   } else {
      uval = val;
   }


   pos = __ulltostr(buf, uval, base, uppercase);

   if (pos && negative) {
      *--pos = '-';
   }

   return pos;
}
#endif //(__KERNEL_CPU_ARCH_SUPPORT_FORMAT>32)


/*===========================================
End of Sourceltostr.c
=============================================*/
