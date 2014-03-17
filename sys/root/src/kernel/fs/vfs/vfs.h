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
#ifndef _VFS_H
#define _VFS_H


/*===========================================
Includes
=============================================*/
#include "kernel/fs/vfs/vfstypes.h"
#include "kernel/fs/vfs/vfsdev.h"

#if defined(__GNUC__)
   #include "kernel/core/stat.h"
#endif
/*===========================================
Declaration
=============================================*/
//typedef int _size_t;

int      _vfs(void);
int      _vfs_rootmnt(void);
int      _vfs_makefs(fstype_t fstype,const char* dev_path,struct vfs_formatopt_t* vfs_formatopt);
int      _vfs_mount(fstype_t fstype,const char* dev_path,const char* mount_path);
int      _vfs_umount(const char* mount_path);
int      _vfs_sync(void);
int      _vfs_mknod(const char *path, mode_t mode, dev_t dev);
int      _vfs_fattach(desc_t desc, const char* path);
int      _vfs_fdetach(const char* path);
int      _vfs_mkdir(const char* ref,mode_t mode);
desc_t   _vfs_open(const char* ref, int oflag,mode_t mode);
int      _vfs_close(desc_t desc);
int      _vfs_read(desc_t desc,char* buf, size_t size);
int      _vfs_write(desc_t desc,char* buf, size_t size);
int      _vfs_lseek(desc_t desc, int offset, int origin);
int      _vfs_ioctl(desc_t desc, int request, ... );
int      _vfs_ioctl2(desc_t desc, int request, va_list ap);
int      _vfs_ftruncate(desc_t desc,off_t length);
int      _vfs_truncate(const char *ref,off_t length);
int      _vfs_remove(const char *ref);
int      _vfs_rename(const char *old_name, const char *new_name );
int      _vfs_rm(const char *ref);
int      _vfs_rmdir(const char *ref);
int      _vfs_chdir(const char* path);

int      _vfs_fstatvfs(desc_t desc, struct statvfs *buf);
int      _vfs_statvfs(const char *path, struct statvfs *buf);
int      _vfs_fstat(desc_t desc, struct stat * stat);
int      _vfs_stat(const char * ref, struct stat * stat);

desc_t   _vfs_opendir(const char* path);
int      _vfs_closedir(desc_t desc);
struct dirent* _vfs_readdir(desc_t desc,struct dirent* dirent);
void     _vfs_rewinddir(desc_t desc);
int      _vfs_telldir(desc_t desc);
int      _vfs_seekdir(desc_t desc, int loc);
int      _vfs_getcwd(char* buf,size_t size);

//
desc_t   _vfs_getdesc(inodenb_t inodenb,desc_t desc);
int      _vfs_putdesc(desc_t desc);

//
int _vfs_ls(char* ref);

#endif
