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
#ifndef _VFSKERNEL_H
#define _VFSKERNEL_H


/*===========================================
Includes
=============================================*/
#include "kernel/core/dirent.h"

/*===========================================
Declaration
=============================================*/

int _syscall_mount      (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
int _syscall_umount     (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
int _syscall_sync       (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
int _syscall_opendir    (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
int _syscall_closedir   (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
int _syscall_readdir    (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
int _syscall_rewinddir  (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
int _syscall_telldir    (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
int _syscall_seekdir    (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
int _syscall_creat      (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
int _syscall_open       (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
int _syscall_close      (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
int _syscall_read       (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
int _syscall_write      (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
int _syscall_lseek      (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
int _syscall_stat       (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
int _syscall_mkdir      (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
int _syscall_chdir      (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
int _syscall_mknod      (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
int _syscall_fattach    (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
int _syscall_fdetach    (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
int _syscall_ftruncate  (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
int _syscall_truncate   (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
int _syscall_rm         (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
int _syscall_rmdir      (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
int _syscall_fstatvfs   (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
int _syscall_statvfs    (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
int _syscall_remove     (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
int _syscall_rename     (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
int _syscall_getcwd     (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
int _syscall_stat       (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
int _syscall_fstat      (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
int _syscall_mkfs       (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);



//
typedef struct{
   int fstype;
   const char * dev_path;
   const char * mount_path;
   int ret;
}mount_t;

//
typedef struct{
   const char * mount_path;
   int ret;
}umount_t;

//
typedef struct{
   int ret;
}sync_t;

//
typedef struct{
   const char * dirname;
   DIR* dir;
}opendir_t;

//
typedef struct{
   DIR* dir;
   int ret;
}closedir_t;

//
typedef struct{
   DIR* dir;
   struct dirent* dirent;
}readdir_t;

//
typedef struct{
   DIR* dir;
}rewinddir_t;

//
typedef struct{
   DIR* dir;
   int loc;
}telldir_t,seekdir_t;

//
typedef struct{
   const char * path;
   int oflag;
   int attr;
   mode_t mode;
   int fildes;
}open_t,creat_t;

//
typedef struct{
   int fildes;
   int ret;
}close_t;

typedef struct{
   int fildes;
   void* buf;
   size_t nbyte;
}read_t,write_t;

//
typedef struct{
   int fildes;
   off_t offset;
   int whence;
}lseek_t;

//
typedef struct{
   const char * path;
   mode_t mode;
   union{
      dev_t    dev; 
      pipe_desc_t pipe_desc;
   }ext;
   int ret;
}mknod_t;

//
typedef struct{
   int fd;
   const char* path;
   int ret;
}fattach_t;

//
typedef struct{
   const char* path;
   int ret;
}fdetach_t;

//
typedef struct{
   const char * path;
   mode_t mode;
   int ret;
}mkdir_t;

//
typedef struct{
   int fildes;
   off_t length;
   int ret;
}ftruncate_t;

//
typedef struct{
   const char *ref;
   off_t length;
   int ret;
}truncate_t;

//
typedef struct{
   const char *ref;
   int ret;
}rm_t,rmdir_t,remove_t;

//
typedef struct{
   const char *old_name;
   const char *new_name;
   int ret;
}rename_t;

//
typedef struct{
   int fildes;
   struct statvfs *statvfs;
   int ret;
}fstatvfs_t;

//
typedef struct{
   const char *path;
   struct statvfs *statvfs;
   int ret;
}statvfs_t;

//
typedef struct{
    const char *path;
    int ret;
}chdir_t;

//
typedef struct{
    char *buf;
    size_t size;
    int ret;
}getcwd_t;

//
typedef struct{
   const char * ref;
   struct stat *stat;
   int ret;
}stat_t;

//
typedef struct{
   int fd;
   struct stat *stat;
   int ret;
}fstat_t;

//
typedef struct{
   fstype_t fstype;
   const char* dev_path;
   struct vfs_formatopt_t* vfs_formatopt;
   int ret;
}mkfs_t;


#endif
