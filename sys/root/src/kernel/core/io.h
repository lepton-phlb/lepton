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


/*============================================
| Compiler Directive   
==============================================*/
#ifndef _IO_H
#define _IO_H


/*============================================
| Includes 
==============================================*/


/*============================================
| Declaration  
==============================================*/
#if !defined(USE_IO_UNIX) && !defined(USE_IO_LINUX)
   #define  USE_IO_UNIX
#endif

//unix
#define __unix_inb(__unix_port__)   *((unsigned char*)__unix_port__)
#define __unix_inw(__unix_port__)   *((unsigned short*)__unix_port__)
#define __unix_inl(__unix_port__)   *((unsigned long*)__unix_port__)
#define __unix_outb(__unix_port__,__unix_value__) *((unsigned char*)__unix_port__)=((unsigned char)__unix_value__)
#define __unix_outw(__unix_port__,__unix_value__) *((unsigned short*)__unix_port__)=((unsigned short)__unix_value__)
#define __unix_outl(__unix_port__,__unix_value__) *((unsigned long*)__unix_port__)=((unsigned long)__unix_value__)

#define inb(__port__) __unix_inb(__port__)
#define inw(__port__) __unix_inw(__port__)
#define inl(__port__) __unix_inl(__port__)


#if defined(USE_IO_UNIX)
   #define outb(__port__,__value__) __unix_outb(__port__,__value__)
   #define outw(__port__,__value__) __unix_outw(__port__,__value__)
   #define outl(__port__,__value__) __unix_outl(__port__,__value__)
#elif defined(USE_IO_LINUX)
   #define outb(__value__,__port__) __unix_outb(__port__,__value__)
   #define outw(__value__,__port__) __unix_outw(__port__,__value__)
   #define outl(__value__,__port__) __unix_outl(__port__,__value__)
#endif


#endif
