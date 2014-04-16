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
#ifndef __CTYPE_H__
#define __CTYPE_H__


/*===========================================
Includes
=============================================*/
#include "kernel/core/kernelconf.h"

/*===========================================
Declaration
=============================================*/
//win32 patch
#ifdef CPU_WIN32
   #define _INC_CTYPE
#endif

//
#ifndef __KERNEL_UCORE_ECOS
   //
   #ifdef toascii
      #undef toascii
   #endif 
   #define toascii(c) ((c) & 0x7F)

   int __l_isdigit   ( int ch );
   int __l_isalnum   ( int ch );
   int __l_isalpha   ( int ch );
   int __l_isblank   ( int ch );
   int __l_iscntrl   ( int ch );
   int __l_isgraph   ( int ch );
   int __l_islower   ( int ch );
   int __l_isprint   ( int ch );
   int __l_isspace   ( int ch );
   int __l_ispunct   ( int ch );
   int __l_isupper   ( int ch );
   int __l_isxdigit  ( int ch );
   int __l_tolower   ( int ch );
   int __l_toupper   ( int ch );

   //
   #ifdef isdigit
      #undef isdigit
   #endif
   #define isdigit   __l_isdigit
   //
   #ifdef isalnum
      #undef isalnum
   #endif
   #define isalnum   __l_isalnum
   //
   #ifdef isalpha
      #undef isalpha
   #endif
   #define isalpha   __l_isalpha
   //
   #ifdef isblank
      #undef isblank
   #endif
   #define isblank   __l_isblank
   //
   #ifdef iscntrl
      #undef iscntrl
   #endif
   #define iscntrl   __l_iscntrl
   //
   #ifdef isgraph
      #undef isgraph
   #endif
   #define isgraph   __l_isgraph
   //
   #ifdef islower
      #undef islower
   #endif
   #define islower   __l_islower
   //
   #ifdef isprint
      #undef isprint
   #endif
   #define isprint   __l_isprint
   //
   #ifdef isspace
      #undef isspace
   #endif
   #define isspace   __l_isspace
   //
   #ifdef ispunct
      #undef ispunct
   #endif
   #define ispunct   __l_ispunct
   //
   #ifdef isupper
      #undef isupper
   #endif
   #define isupper   __l_isupper
   //
   #ifdef isxdigit
      #undef isxdigit
   #endif
   #define isxdigit   __l_isxdigit
   //
   #ifdef tolower
      #undef tolower
   #endif
   #define tolower   __l_tolower
   //
   #ifdef toupper
      #undef toupper
   #endif
   #define toupper   __l_toupper

#endif //ifndef __KERNEL_UCORE_ECOS


//
int __l_isascii   ( int ch );

#ifdef isascii
   #undef isascii
#endif
#define isascii   __l_isascii

//
#endif
