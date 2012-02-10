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
Includes
=============================================*/
#include "kernel/core/types.h"
#include "kernel/core/interrupt.h"
#include "kernel/core/syscall.h"
#include "kernel/core/kernel.h"
#include "kernel/core/process.h"
#include "kernel/core/dirent.h"
#include "kernel/fs/vfs/vfskernel.h"


/*===========================================
Global Declaration
=============================================*/


/*===========================================
Implementation
=============================================*/

/*-------------------------------------------
| Name:opendir
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
#if defined(GNU_GCC)
__attribute__ ((visibility("hidden")))
DIR* opendir(char * dirname) {
#else
DIR* opendir(char * dirname){
#endif
   opendir_t opendir_dt;

   opendir_dt.dirname= dirname;

   __mk_syscall(_SYSCALL_OPENDIR,opendir_dt)

   return opendir_dt.dir;
}

/*-------------------------------------------
| Name:closedir
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
#if defined(GNU_GCC)
int __attribute__ ((visibility("hidden")))
closedir(DIR* dir){
#else
int closedir(DIR* dir){
#endif
   closedir_t closedir_dt;

   closedir_dt.dir = dir;

   __mk_syscall(_SYSCALL_CLOSEDIR,closedir_dt)

   return closedir_dt.ret;
}

/*-------------------------------------------
| Name:readdir
| Description:
| Parameters:
| Return Type:
| Comments: now conform with posix 1003.1
| See:
---------------------------------------------*/
#if defined(GNU_GCC)
__attribute__ ((visibility("hidden")))
struct dirent * readdir(DIR *dir){
#else
struct dirent *readdir(DIR *dir){
#endif

   readdir_t readdir_dt;

   readdir_dt.dir = dir;
   readdir_dt.dirent = (struct dirent*)0;

   __mk_syscall(_SYSCALL_READDIR,readdir_dt)

   return readdir_dt.dirent;
}

/*-------------------------------------------
| Name:readdir2
| Description:
| Parameters:
| Return Type:
| Comments: to preserve compatibility wtih previous version of lepton
| See:
---------------------------------------------*/
struct dirent *readdir2(DIR *dir,struct dirent* dirent){

   readdir_t readdir_dt;

   readdir_dt.dir = dir;
   readdir_dt.dirent = dirent;

   __mk_syscall(_SYSCALL_READDIR,readdir_dt)

   return readdir_dt.dirent;
}


/*-------------------------------------------
| Name:readdir_r
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
//int readdir_r(DIR *, struct dirent *, struct dirent **);//not yet implemented

/*-------------------------------------------
| Name:rewinddir
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void rewinddir(DIR* dir){
   rewinddir_t rewinddir_dt;

   rewinddir_dt.dir = dir;

   __mk_syscall(_SYSCALL_REWINDDIR,rewinddir_dt)
}

/*-------------------------------------------
| Name:seekdir
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void seekdir(DIR * dir, int loc){
   seekdir_t seekdir_dt;

   seekdir_dt.dir = dir;
   seekdir_dt.loc = loc;

   __mk_syscall(_SYSCALL_SEEKDIR,seekdir_dt)
}

/*-------------------------------------------
| Name:telldir
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int telldir(DIR * dir){
   telldir_t telldir_dt;

   telldir_dt.dir = dir;

   __mk_syscall(_SYSCALL_TELLDIR,telldir_dt)

   return telldir_dt.loc;
}

/*===========================================
End of Sourcedirent.c
=============================================*/
