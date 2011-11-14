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
#include "kernel/core/process.h"
#include "kernel/core/syscall.h"
#include "kernel/fs/vfs/vfskernel.h"
#include "kernel/fs/vfs/vfs.h"
#include "kernel/core/flock.h"
#include "kernel/core/stat.h"

#if defined(GNU_GCC)
#include <stdlib.h>
#endif
/*===========================================
Global Declaration
=============================================*/

/*===========================================
Implementation
=============================================*/

/*-------------------------------------------
| Name:_syscall_mount
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _syscall_mount(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   mount_t* mount_dt = (mount_t*)data;
   mount_dt->ret = _vfs_mount((fstype_t)mount_dt->fstype,mount_dt->dev_path,mount_dt->mount_path);
   __flush_syscall(pthread_ptr);
   __kernel_ret_int(pthread_ptr);
   return 0;
}

/*-------------------------------------------
| Name:_syscall_umount
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _syscall_umount(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   umount_t* umount_dt = (umount_t*)data;
   umount_dt->ret = _vfs_umount(umount_dt->mount_path);
   __flush_syscall(pthread_ptr);
   __kernel_ret_int(pthread_ptr);
   return 0;
}

/*-------------------------------------------
| Name:_syscall_sync
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _syscall_sync(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   sync_t* sync_dt = (sync_t*)data;
   sync_dt->ret = _vfs_sync();
   __flush_syscall(pthread_ptr);
   __kernel_ret_int(pthread_ptr);
   return 0;
}

/*-------------------------------------------
| Name:_syscall_opendir
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _syscall_opendir(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   desc_t desc=-1;
   opendir_t* opendir_dt = (opendir_t*)data;

   opendir_dt->dir = (void*)0;

   //
   if((desc = _vfs_opendir(opendir_dt->dirname))>=0){
         opendir_dt->dir = &ofile_lst[desc];
   }

   __flush_syscall(pthread_ptr);
   __kernel_ret_int(pthread_ptr);
   return 0;
}

/*-------------------------------------------
| Name:_syscall_closedir
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _syscall_closedir(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   closedir_t* closedir_dt = (closedir_t*)data;
   desc_t desc = closedir_dt->dir->desc;

   closedir_dt->ret = _vfs_closedir(closedir_dt->dir->desc);

   //free struct dirent (see _syscall_readdir() )
   if(ofile_lst[desc].p)
      _sys_free(ofile_lst[desc].p);

   ofile_lst[desc].p=(void*)0;
   
   __flush_syscall(pthread_ptr);
   __kernel_ret_int(pthread_ptr);
   return 0;
}

/*-------------------------------------------
| Name:_syscall_readdir
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _syscall_readdir(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   readdir_t* readdir_dt = (readdir_t*)data;
   desc_t desc = readdir_dt->dir->desc;

   //alloc dirent structure (snif :'( but it's for posix conformance for readdir )
   if(ofile_lst[desc].p==(void*)0)
      ofile_lst[desc].p = _sys_malloc(sizeof(struct dirent));

   if(!readdir_dt->dirent)
      readdir_dt->dirent = ofile_lst[desc].p;
   
   if(readdir_dt->dirent)
      readdir_dt->dirent = _vfs_readdir(readdir_dt->dir->desc,readdir_dt->dirent);
   
   __flush_syscall(pthread_ptr);
   __kernel_ret_int(pthread_ptr);
   return 0;
}

/*-------------------------------------------
| Name:_syscall_rewinddir
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _syscall_rewinddir(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   rewinddir_t* rewinddir_dt = (rewinddir_t*)data;

   _vfs_rewinddir(rewinddir_dt->dir->desc);
   
   __flush_syscall(pthread_ptr);
   __kernel_ret_int(pthread_ptr);
   return 0;
}

/*-------------------------------------------
| Name:_syscall_telldir
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _syscall_telldir(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   telldir_t* telldir_dt = (telldir_t*)data;

   telldir_dt->loc = _vfs_telldir(telldir_dt->dir->desc);
   
   __flush_syscall(pthread_ptr);
   __kernel_ret_int(pthread_ptr);
   return 0;
}

/*-------------------------------------------
| Name:_syscall_seekdir
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _syscall_seekdir(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   seekdir_t* seekdir_dt = (seekdir_t*)data;

   _vfs_seekdir(seekdir_dt->dir->desc,seekdir_dt->loc);
   
   __flush_syscall(pthread_ptr);
   __kernel_ret_int(pthread_ptr);
   return 0;
}

/*-------------------------------------------
| Name:_syscall_creat
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _syscall_creat(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   creat_t* creat_dt = (creat_t*)data;
   creat_dt->oflag|=O_CREAT;
   return _syscall_open(pthread_ptr,pid,data);
}

/*-------------------------------------------
| Name:_syscall_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _syscall_open(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   desc_t desc=-1;
   open_t* open_dt = (open_t*)data;
   open_dt->fildes = -1;
   open_dt->attr = S_IFNULL;

   //to remove: profiling test
   /*__kernel_profiler_start();
   {      
      int testfd=0;
      int Nbcalcule=10000;
      float resultat;
      float cal1=10.23454646E3;
      float cal2=0.231452424E-5;
      for(testfd=2;testfd<Nbcalcule;testfd++)
	   {
 	      cal2=cal1+cal2;
 	      resultat=cal1-cal2;
 	      cal1=cal1/cal2;
 	      cal2=cal1*cal2;
 	   }
      
      //OS_Delay(50);
      __kernel_profiler_stop(__get_syscall_owner_pthread_ptr());
      __profiler_add_result(__get_syscall_owner_pthread_ptr(),_SYSCALL_CREAT,__kernel_profiler_get_counter(__get_syscall_owner_pthread_ptr()));
   }*/
   //

   desc = _vfs_open(open_dt->path,open_dt->oflag,open_dt->mode);
   if(desc>=0)
      open_dt->fildes = _get_fd(pid,0);

   if(open_dt->fildes>=0){
      process_lst[pid]->desc_tbl[open_dt->fildes]  = desc;
      open_dt->attr = ofile_lst[desc].attr;
      //ofile_lst[desc].pid = pid;//see _vfs_getdesc() in vfs.c
   }else if(desc>=0){
      _vfs_close(desc);
   }

   __flush_syscall(pthread_ptr);
   __kernel_ret_int(pthread_ptr);
   return 0;
}

/*-------------------------------------------
| Name:_syscall_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _syscall_close(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   desc_t desc=-1;
   int ilck;
   close_t* close_dt = (close_t*)data;
      
   desc = process_lst[pid]->desc_tbl[close_dt->fildes];

   if(desc>=0)
      close_dt->ret = _vfs_close(desc);//be aware ;-) :_sys_kill() called in _sys_pipe_close() use __atomic_in() __atomic_out().
   //
   close_dt->ret=_put_fd(pid,close_dt->fildes);
   //see close on exec in kernel/core/core-segger/process.c
   _unset_cloexec(pid,close_dt->fildes);
   //
   if(close_dt->fildes>=0)
      process_lst[pid]->desc_tbl[close_dt->fildes] = -1;

   //unlock file
   #ifdef __KERNEL_USE_FILE_LOCK
   __atomic_in();
   if((ilck=_is_locked(ofile_lst[desc].inodenb))>=0)//is locked?
      if( flock_lst[ilck]._flock.l_pid == pid){//locked by this process?
         _put_flock(ofile_lst[desc].inodenb);//now, is unlocked
         //wakeup locked process
         _sys_unlockw();
      }
   __atomic_out();
   #endif

   __flush_syscall(pthread_ptr);
   __kernel_ret_int(pthread_ptr);


   return 0;
}

/*-------------------------------------------
| Name:_syscall_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _syscall_read(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   desc_t desc=-1;
   read_t* read_dt = (read_t*)data;
  
   if(read_dt->fildes>=0)
      desc = process_lst[pid]->desc_tbl[read_dt->fildes];

   if(desc>=0)
      read_dt->nbyte = _vfs_read(desc,read_dt->buf,read_dt->nbyte);

   __flush_syscall(pthread_ptr);
   __kernel_ret_int(pthread_ptr);
   return 0;
}

/*-------------------------------------------
| Name:_syscall_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _syscall_write(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   desc_t desc=-1;
   write_t* write_dt = (write_t*)data;
  
   if(write_dt->fildes>=0)
      desc = process_lst[pid]->desc_tbl[write_dt->fildes];

   if(desc>=0)
      write_dt->nbyte = _vfs_write(desc,write_dt->buf,write_dt->nbyte);

   __flush_syscall(pthread_ptr);
   __kernel_ret_int(pthread_ptr);
   return 0;
}

/*-------------------------------------------
| Name:_syscall_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _syscall_lseek(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   desc_t desc=-1;
   lseek_t* lseek_dt=(lseek_t*)data;

   if(lseek_dt->fildes>=0)
      desc = process_lst[pid]->desc_tbl[lseek_dt->fildes];

   if(desc>=0)
      lseek_dt->offset = _vfs_lseek(desc,lseek_dt->offset,lseek_dt->whence);
   
   __flush_syscall(pthread_ptr);
   __kernel_ret_int(pthread_ptr);
   return 0;
}

/*-------------------------------------------
| Name:_syscall_mkdir
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _syscall_mkdir(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   mkdir_t* mkdir_dt = (mkdir_t*)data;
   mkdir_dt->ret = _vfs_mkdir(mkdir_dt->path,mkdir_dt->mode);
   __flush_syscall(pthread_ptr);
   __kernel_ret_int(pthread_ptr);
   return 0;
}

/*-------------------------------------------
| Name:_syscall_mknod
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _syscall_mknod(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   mknod_t* mknod_dt = (mknod_t*)data;
   if( (mknod_dt->mode&S_IFCHR) ||  (mknod_dt->mode&S_IFBLK)){
      mknod_dt->ret = _vfs_mknod(mknod_dt->path,mknod_dt->mode,mknod_dt->ext.dev);
   }else if(mknod_dt->mode&S_IFIFO){
      mknod_dt->ext.pipe_desc = -1;
      mknod_dt->ret = _vfs_mknod(mknod_dt->path,mknod_dt->mode,mknod_dt->ext.pipe_desc);
   }else if(mknod_dt->mode&S_IFREG){
      desc_t desc = _vfs_open(mknod_dt->path,O_CREAT|O_WRONLY,mknod_dt->mode);
      if(desc>=0){
         mknod_dt->ret = 0;
         _vfs_close(desc);
      }else
         mknod_dt->ret = -1;
   }else if(mknod_dt->mode&S_IFDIR) {
      mknod_dt->ret = _vfs_mkdir(mknod_dt->path,mknod_dt->mode);
   }else
      mknod_dt->ret= -1;

   __flush_syscall(pthread_ptr);
   __kernel_ret_int(pthread_ptr);
   return 0;
}

/*--------------------------------------------
| Name:        _syscall_fattach
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int _syscall_fattach(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   fattach_t* fattach_dt = (fattach_t*)data;
   desc_t desc;

   fattach_dt->ret=0;

   if(fattach_dt->fd<0)
      fattach_dt->ret = -1;
   
   if(fattach_dt->path==(const char *)0)
      fattach_dt->ret = -1;

   if(fattach_dt->ret != -1){
      if((desc = process_lst[pid]->desc_tbl[fattach_dt->fd])>=0){
         fattach_dt->ret = _vfs_fattach(desc,fattach_dt->path);
      }
   }

   __flush_syscall(pthread_ptr);
   __kernel_ret_int(pthread_ptr);
   return 0;
}

/*--------------------------------------------
| Name:        _syscall_fdetach
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int _syscall_fdetach(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   fdetach_t* fdetach_dt = (fdetach_t*)data;

   fdetach_dt->ret=0;

   if(fdetach_dt->path==(const char *)0)
      fdetach_dt->ret = -1;

   if(fdetach_dt->ret != -1){
         fdetach_dt->ret = _vfs_fdetach(fdetach_dt->path);
   }
   __flush_syscall(pthread_ptr);
   __kernel_ret_int(pthread_ptr);
   return 0;
}

/*-------------------------------------------
| Name:_syscall_ftruncate
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _syscall_ftruncate(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   desc_t desc=-1;
   ftruncate_t* ftruncate_dt=(ftruncate_t*)data;

   ftruncate_dt->ret=-1;
  
   if(ftruncate_dt->fildes>=0)
      desc = process_lst[pid]->desc_tbl[ftruncate_dt->fildes];

   if(desc>=0)
      ftruncate_dt->ret = _vfs_ftruncate(desc,ftruncate_dt->length);

   __flush_syscall(pthread_ptr);
   __kernel_ret_int(pthread_ptr);
   return 0;
}

/*-------------------------------------------
| Name:_syscall_truncate
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _syscall_truncate(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   truncate_t* truncate_dt=(truncate_t*)data;

   truncate_dt->ret = _vfs_truncate(truncate_dt->ref,truncate_dt->length);

   __flush_syscall(pthread_ptr);
   __kernel_ret_int(pthread_ptr);
   return 0;
}

/*-------------------------------------------
| Name:_syscall_rm
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _syscall_rm(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   rm_t* rm_dt = (rm_t*)data;
   rm_dt->ret = _vfs_rm(rm_dt->ref);
   __flush_syscall(pthread_ptr);
   __kernel_ret_int(pthread_ptr);
   return 0;
}

/*-------------------------------------------
| Name:_syscall_remove
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _syscall_remove(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   remove_t* remove_dt = (remove_t*)data;
   remove_dt->ret = _vfs_remove(remove_dt->ref);
   __flush_syscall(pthread_ptr);
   __kernel_ret_int(pthread_ptr);
   return 0;
}

/*-------------------------------------------
| Name:_syscall_rename
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _syscall_rename(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   rename_t* rename_dt = (rename_t*)data;
   rename_dt->ret = _vfs_rename(rename_dt->old_name,rename_dt->new_name);
   __flush_syscall(pthread_ptr);
   __kernel_ret_int(pthread_ptr);
   return 0;
}

/*-------------------------------------------
| Name:_syscall_rmdir
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _syscall_rmdir(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   rmdir_t* rmdir_dt = (rmdir_t*)data;
   rmdir_dt->ret = _vfs_rmdir(rmdir_dt->ref);
   __flush_syscall(pthread_ptr);
   __kernel_ret_int(pthread_ptr);
   return 0;
}

/*-------------------------------------------
| Name:_syscall_chdir
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _syscall_chdir(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   chdir_t* chdir_dt = (chdir_t*)data;
   chdir_dt->ret  = _vfs_chdir(chdir_dt->path);
   __flush_syscall(pthread_ptr);
   __kernel_ret_int(pthread_ptr);
   return 0;
}

/*-------------------------------------------
| Name:_syscall_fstatvfs
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _syscall_fstatvfs(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   desc_t desc=-1;
   fstatvfs_t* fstatvfs_dt=(fstatvfs_t*)data;

   fstatvfs_dt->ret=-1;
  
   if(fstatvfs_dt->fildes>=0)
      desc = process_lst[pid]->desc_tbl[fstatvfs_dt->fildes];

   if(desc>=0)
      fstatvfs_dt->ret = _vfs_fstatvfs(desc,fstatvfs_dt->statvfs);

   __flush_syscall(pthread_ptr);
   __kernel_ret_int(pthread_ptr);
   return 0;
}

/*-------------------------------------------
| Name:_syscall_statvfs
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _syscall_statvfs(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   
   statvfs_t* statvfs_dt=(statvfs_t*)data;

   statvfs_dt->ret = _vfs_statvfs(statvfs_dt->path,statvfs_dt->statvfs);

   __flush_syscall(pthread_ptr);
   __kernel_ret_int(pthread_ptr);
   return 0;
}

/*-------------------------------------------
| Name:_syscall_getcwd
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _syscall_getcwd(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){

   getcwd_t* getcwd_dt=(getcwd_t*)data;
   
   getcwd_dt->ret=_vfs_getcwd(getcwd_dt->buf,getcwd_dt->size);

   __flush_syscall(pthread_ptr);
   __kernel_ret_int(pthread_ptr);
   return 0;
}

/*-------------------------------------------
| Name:_syscall_stat
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _syscall_stat(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   stat_t* stat_dt=(stat_t*)data;
   stat_dt->ret = _vfs_stat(stat_dt->ref,stat_dt->stat);
   __flush_syscall(pthread_ptr);
   __kernel_ret_int(pthread_ptr);
   return 0;
}

/*-------------------------------------------
| Name:_syscall_fstat
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _syscall_fstat(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   desc_t desc;
   fstat_t* fstat_dt=(fstat_t*)data;

   if(fstat_dt->fd>=0)
      desc = process_lst[pid]->desc_tbl[fstat_dt->fd];

   if(desc>=0)
      fstat_dt->ret = _vfs_fstat(desc,fstat_dt->stat);

   __flush_syscall(pthread_ptr);
   __kernel_ret_int(pthread_ptr);
   return 0;
}

/*-------------------------------------------
| Name:_syscall_mkfs
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _syscall_mkfs(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   mkfs_t* mkfs_dt=(mkfs_t*)data;

   mkfs_dt->ret = _vfs_makefs(mkfs_dt->fstype,mkfs_dt->dev_path,mkfs_dt->vfs_formatopt);

   __flush_syscall(pthread_ptr);
   __kernel_ret_int(pthread_ptr);
   return 0;
}

/*===========================================
End of Sourcevfskernel.c
=============================================*/
