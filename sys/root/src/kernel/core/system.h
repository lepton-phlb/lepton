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

/*===========================================
Compiler Directive
=============================================*/
#ifndef _SYSTEM_H_
#define _SYSTEM_H_

/*===========================================
Includes
=============================================*/

#include "kernel/core/kernelconf.h"

#if defined (CPU_WIN32)
   //#include "kernel/core/windows.h"
   #include "kernel/core/ucore/embOSW32_100/win32/windows.h"
#elif ( defined(__IAR_SYSTEMS_ICC) && defined (__KERNEL_UCORE_EMBOS) && defined(CPU_M16C62))
   #include <icclbutl.h>
   #include <intrm16c.h>
//#include "iom16c62.h"
   #include "dev/arch/m16c/dev_m16c_62p/iom16c62p136a.h"
   #include "intm16c.h"
   #include "rtos.h"
   #include "stdlib.h"
#endif

#include "kernel/core/ver.h"
#include "kernel/core/cpu.h"


/*===========================================
Declaration
=============================================*/
//prevent multiple declaration (to do:will be moved in stat.h)
#ifdef S_IFMT
   #undef S_IFMT
#endif
#ifdef S_IFNULL
   #undef S_IFNULL
#endif
#ifdef S_IFREG
   #undef S_IFREG
#endif
#ifdef S_IFBLK
   #undef S_IFBLK
#endif
#ifdef S_IFCHR
   #undef S_IFCHR
#endif
#ifdef S_IFDIR
   #undef S_IFDIR
#endif
#ifdef S_IFIFO
   #undef S_IFIFO
#endif
#ifdef S_IFLNK
   #undef S_IFLNK
#endif

//file attribute
#define S_IFMT                  0x003F
#define S_IFNULL                0x0000
#define S_IFREG                 0x0001
#define S_IFBLK                 0x0002
#define S_IFCHR                 0x0004
#define S_IFDIR                 0x0008
#define S_IFIFO                 0x0010
#define S_IFLNK                 0x0020

//prevent multiple declaration
#ifdef O_RDONLY
   #undef O_RDONLY
#endif
#ifdef O_WRONLY
   #undef O_WRONLY
#endif
#ifdef O_RDWR
   #undef O_RDWR
#endif
#ifdef O_CREAT
   #undef O_CREAT
#endif
#ifdef O_APPEND
   #undef O_APPEND
#endif
#ifdef O_SYNC
   #undef O_SYNC
#endif
#ifdef O_NONBLOCK
   #undef O_NONBLOCK
#endif

//
#define PATH_MAX     64

//file open flag
#define O_RDONLY     0x0001
#define O_WRONLY     0x0002
#define O_RDWR       0x0003 //O_RDONLY|O_WRONLY
#define O_CREAT      0x0004
#define O_APPEND     0x0008
#define O_SYNC       0x0010
#define O_NONBLOCK   0x0020

//see fcntl.h FD_CLOEXEC 0x8000

//file descriptor
#define INVALID_DESC             -1

//file seek option
#define SEEK_SET   0
#define SEEK_CUR   1
#define SEEK_END   2

#endif
