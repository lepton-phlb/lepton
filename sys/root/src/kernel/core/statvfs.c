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
#include "kernel/core/types.h"
#include "kernel/core/interrupt.h"
#include "kernel/core/syscall.h"
#include "kernel/core/kernel.h"
#include "kernel/core/process.h"
#include "kernel/fs/vfs/vfskernel.h"
#include "kernel/fs/vfs/vfstypes.h"
#include "kernel/core/statvfs.h"


/*===========================================
Global Declaration
=============================================*/


/*===========================================
Implementation
=============================================*/

/*-------------------------------------------
| Name:fstatvfs
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int fstatvfs(int fildes, struct statvfs *buf){
   fstatvfs_t fstatvfs_dt;
   fstatvfs_dt.fildes = fildes;
   fstatvfs_dt.statvfs = buf;

   __mk_syscall(_SYSCALL_FSTATVFS,fstatvfs_dt)

   return fstatvfs_dt.ret;
}

/*-------------------------------------------
| Name:statvfs
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int statvfs(const char *path, struct statvfs *buf){
   statvfs_t statvfs_dt;
   statvfs_dt.path = path;
   statvfs_dt.statvfs = buf;

   __mk_syscall(_SYSCALL_STATVFS,statvfs_dt)

   return statvfs_dt.ret;
}

/*-------------------------------------------
| Name:mkfs
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int mkfs(fstype_t fstype,const char* dev_path, struct vfs_formatopt_t * vfs_formatopt){

   mkfs_t mkfs_dt;

   mkfs_dt.fstype = fstype;
   mkfs_dt.dev_path = dev_path;
   mkfs_dt.vfs_formatopt = vfs_formatopt;

   __mk_syscall(_SYSCALL_MKFS,mkfs_dt)

   return mkfs_dt.ret;
}

/*===========================================
End of Sourcestatvfs.c
=============================================*/
