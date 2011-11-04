/*
The contents of this file are subject to the Mozilla Public License Version 1.1 
(the "License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS" basis, 
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License for the 
specific language governing rights and limitations under the License.

The Original Code is ______________________________________.

The Initial Developer of the Original Code is ________________________.
Portions created by ______________________ are Copyright (C) ______ _______________________.
All Rights Reserved.

Contributor(s): ______________________________________.

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
#include "kernel/core//types.h"
#include "kernel/core/interrupt.h"
#include "kernel/core/syscall.h"
#include "kernel/core/kernel.h"
#include "kernel/core/process.h"
#include "kernel/fs/vfs/vfskernel.h"
#include "kernel/core//stat.h"


/*===========================================
Global Declaration
=============================================*/




/*===========================================
Implementation
=============================================*/

/*-------------------------------------------
| Name:mknod
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int mknod(const char *path, mode_t mode, dev_t dev){
   mknod_t mknod_dt;

   mknod_dt.path=path;
   mknod_dt.mode=mode;
   if(mode&S_IFIFO)
      mknod_dt.ext.pipe_desc=-1;
   else
      mknod_dt.ext.dev=dev;
   __mk_syscall(_SYSCALL_MKNOD,mknod_dt);

   return mknod_dt.ret;
}

/*-------------------------------------------
| Name:mkfifo
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int mkfifo(const char *path, mode_t mode){
   mknod_t mknod_dt;

   mknod_dt.path=path;
   mknod_dt.mode=mode|S_IFIFO;
   mknod_dt.ext.pipe_desc=-1;
   __mk_syscall(_SYSCALL_MKNOD,mknod_dt);

   return mknod_dt.ret;
}

/*-------------------------------------------
| Name:mkdir
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int mkdir(const char * path, mode_t mode){
   mkdir_t mkdir_dt;
   mkdir_dt.path  = path;
   mkdir_dt.mode  = mode;
   __mk_syscall(_SYSCALL_MKDIR,mkdir_dt);
   return mkdir_dt.ret;
}

/*-------------------------------------------
| Name:stat
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
#if defined(GNU_GCC)
int __attribute__ ((visibility("hidden")))
stat(const char * ref, struct stat * stat){
#else
int stat(const char * ref, struct stat * stat){
#endif
   stat_t stat_dt;
   stat_dt.ref = ref;
   stat_dt.stat = stat;
   __mk_syscall(_SYSCALL_STAT,stat_dt);
   return stat_dt.ret;
}

/*-------------------------------------------
| Name:fstat
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int fstat(int fd, struct stat * stat){
   fstat_t fstat_dt;
   fstat_dt.fd = fd;
   fstat_dt.stat = stat;
   __mk_syscall(_SYSCALL_FSTAT,fstat_dt);
   return fstat_dt.ret;
}


/*-------------------------------------------
| Name:access
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int access(const char * name, int type) {
   return 0;
}

/*-------------------------------------------
| Name:chmod
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int chmod(const char *path, mode_t mode) {
   return 0;
}

/*===========================================
End of Source stat.c
=============================================*/
