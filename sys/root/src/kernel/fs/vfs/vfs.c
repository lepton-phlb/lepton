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
#include "kernel/core/kernel.h"
#include "kernel/core/errno.h"
#include "kernel/core/process.h"
#include "kernel/core/system.h"
#include "kernel/core/ioctl.h"
#include "kernel/core/fcntl.h"
#include "kernel/core/stat.h"
#include "kernel/core/statvfs.h"
#include "kernel/fs/vfs/vfstypes.h"
#include "kernel/fs/vfs/vfscore.h"
#include "kernel/fs/vfs/vfs.h"


#if defined(__GNUC__)
   #include <string.h>
   #include <stdlib.h>
#endif

/*============================================
| Global Declaration
==============================================*/


/*============================================
| Implementation
==============================================*/


/*-------------------------------------------
| Name:_vfs_mount
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _vfs_mount(fstype_t fstype,const char* dev_path,const char* mount_path){

   char filename[32]; //to remove: Only for test
   desc_t dev_desc=-1;
   desc_t desc;
   struct dirent dirent;
   mntdev_t* pmntdev;

   if(dev_path) {
      dev_desc = _vfs_open(dev_path,O_RDWR,0);
      if(dev_desc<0 || !(ofile_lst[dev_desc].attr&(S_IFBLK|S_IFCHR))) {
         __kernel_set_errno(ENODEV);
         return -1; //to do: put desc
      }
   }

   if(_vfs_lookupmnt(mount_path,&desc,filename)<0) {
      _vfs_close(dev_desc); //close device
      _vfs_putdesc(desc);  //invalid path
      __kernel_set_errno(ENOENT);
      return -1;
   }

   //
   if(desc<0)
      return -1;

   //to do: verify if mount point is a directory.
   _vfs_rewinddir(desc);
   while(_vfs_readdir(desc,&dirent)) {
      //printf("[%d] %s\n",dirent.inodenb,dirent.d_name);
      if(!strcmp(filename,dirent.d_name))
         break;
   }

   if(!(pmntdev = _vfs_addmnt(ofile_lst[desc].pmntdev,dirent.inodenb, dev_desc,fstype))) {
      __kernel_set_errno(ENOENT);
      _vfs_close(dev_desc);
      _vfs_putdesc(desc);  //invalid path
      return -1;
   }

   ofile_lst[desc].pfsop->fs.mountdir(desc,pmntdev->mnt_inodenb,pmntdev->inodenb_offset);


   _vfs_putdesc(desc);
   return 0;
}

/*-------------------------------------------
| Name:_vfs_umount
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _vfs_umount(const char* mount_path){

   char filename[32]; //to remove: Only for test
   desc_t desc;
   struct dirent dirent;
   mntdev_t* pmntdev;


   if(_vfs_lookupmnt(mount_path,&desc,filename)<0) {
      _vfs_putdesc(desc);  //invalid path
      __kernel_set_errno(ENOENT);
      return -1;
   }

   //
   if(desc<0)
      return -1;

   _vfs_rewinddir(desc);
   while(_vfs_readdir(desc,&dirent)) {
      //printf("[%d] %s\n",dirent.inodenb,dirent.d_name);
      if(!strcmp(filename,dirent.d_name))
         break;
   }

   //
   if(!(pmntdev = _vfs_removemnt(dirent.inodenb)))
      return -1;

   _vfs_close(pmntdev->dev_desc); //close device

   //restore mount : target become original dir
   ofile_lst[desc].pfsop->fs.mountdir(desc,dirent.inodenb,pmntdev->mnt_inodenb);

   _vfs_putdesc(desc);
   return 0;
}

/*-------------------------------------------
| Name:_vfs_sync
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _vfs_sync(void){

   int i=0;
   fstype_t fstype;

   for(i=0; i<MAX_MOUNT_DEVICE; i++) {
      if(!mntdev_lst[i].psuperblk_info || mntdev_lst[i].dev_desc<0)
         continue;
      //
      fstype = mntdev_lst[i].fstype;
      //write file system
      fsop_lst[fstype]->fs.writefs(&mntdev_lst[i]);
      fsop_lst[fstype]->fs.readfs(&mntdev_lst[i]);
   }

   return 0;
}

/*-------------------------------------------
| Name:_vfs_makefs
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _vfs_makefs(fstype_t fstype,const char* dev_path,struct vfs_formatopt_t* vfs_formatopt){

   desc_t dev_desc;
   int error;

   dev_desc = _vfs_open(dev_path,O_RDWR,0);
   if(dev_desc<0 || !(ofile_lst[dev_desc].attr&(S_IFBLK|S_IFCHR))) {
      if(dev_desc>=0)
         _vfs_putdesc(dev_desc);
      __kernel_set_errno(ENOENT);
      return -1;
   }

   error=fsop_lst[fstype]->fs.makefs(dev_desc,vfs_formatopt);

   _vfs_putdesc(dev_desc);

   return error;
}

/*-------------------------------------------
| Name:_vfs_mknod
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _vfs_mknod(const char *path, mode_t mode, dev_t dev){
   char* filename;
   inodenb_t _inode;
   desc_t _desc;

   if(_vfs_lookup(path,&_desc,&filename)==0) {
      _vfs_putdesc(_desc);  //already exist
      __kernel_set_errno(EEXIST);
      return -1;
   }

   //
   if(_desc<0)
      return -1;

   //
   _inode = ofile_lst[_desc].pfsop->fs.create(_desc,filename,mode);
   if(_inode==INVALID_INODE_NB) {
      _vfs_putdesc(_desc);
      return -1;
   }

   if(ofile_lst[_desc].pfsop->fs.mknod(_desc,_inode,dev)<0) {
      _vfs_putdesc(_desc);
      return -1;
   }

   _vfs_putdesc(_desc);
   return 0;
}

/*--------------------------------------------
| Name:        _vfs_fattach
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int _vfs_fattach(desc_t desc, const char* path){
   dev_t dev = (dev_t)((dev_t)max_dev+(dev_t)desc);
   desc_t _desc;

   if(ofile_lst[desc].desc_nxt[0]<0 && ofile_lst[desc].desc_nxt[1]<0)
      return -1;

   if(_vfs_mknod(path,(mode_t)(ofile_lst[desc].attr|S_IFLNK),dev)<0)
      return -1;

   //read branch. only stream compatible mode in this branch  see(_vfs_link_desc() in vfs_core.c)
   _desc=desc;
   if(ofile_lst[_desc].oflag&O_RDONLY) {
      do {
         ofile_lst[_desc].attr|=S_IFLNK;
      } while((_desc=ofile_lst[_desc].desc_nxt[0])>=0);
   }

   //write branch. only stream compatible mode in this branch  see(_vfs_link_desc() in vfs_core.c)
   _desc=desc;
   if(ofile_lst[_desc].oflag&O_WRONLY) {
      do {
         ofile_lst[_desc].attr|=S_IFLNK;
      } while((_desc=ofile_lst[_desc].desc_nxt[1])>=0);
   }

   return 0;
}

/*--------------------------------------------
| Name:        _vfs_fdetach
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int _vfs_fdetach(const char* path){
   desc_t desc;
   desc_t _desc;

   if((desc= _vfs_open(path,O_RDWR,0))<0)
      return -1;

   //it's a stream
   if(!S_ISLNK(ofile_lst[desc].attr)) {
      _vfs_close(desc);  //already exist
      __kernel_set_errno(EEXIST);
      return -1; //no :(
   }
   //yes :)
   _vfs_close(desc); //(dec nb reader and write);

   //now remove S_IFLNK attribute

   //read branch. only stream compatible mode in this branch  see(_vfs_link_desc() in vfs_core.c)
   _desc=desc;
   if(ofile_lst[_desc].oflag&O_RDONLY) {
      do {
         ofile_lst[_desc].attr&=~(S_IFLNK);
         _vfs_close(_desc); //close definitly driver (nb reader and write goes to 0).
      } while((_desc=ofile_lst[_desc].desc_nxt[0])>=0);
   }

   //write branch. only stream compatible mode in this branch  see(_vfs_link_desc() in vfs_core.c)
   _desc=desc;
   if(ofile_lst[_desc].oflag&O_WRONLY) {
      do {
         ofile_lst[_desc].attr&=~(S_IFLNK);
         _vfs_close(_desc); //close definitly driver (nb reader and write goes to 0).
      } while((_desc=ofile_lst[_desc].desc_nxt[1])>=0);
   }

   //remove path
   if(_vfs_rm(path)<0)
      return -1;

   return 0;
}


/*-------------------------------------------
| Name:_vfs_create
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _vfs_creat(const char* ref, int attr,mode_t mode){
   char* filename;
   desc_t _desc;

   if(_vfs_lookup(ref,&_desc,&filename)==0) {
      _vfs_putdesc(_desc);  //already exist
      __kernel_set_errno(EEXIST);
      return -1;
   }

   //
   if(_desc<0)
      return -1;

   //
   if(ofile_lst[_desc].pfsop->fs.create(_desc,filename,attr)<0) {
      _vfs_putdesc(_desc); //cannot create (disk full)
      return -1;
   }
   //
   _vfs_putdesc(_desc);
   return 0;
}

/*-------------------------------------------
| Name:_vfs_mkdir
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _vfs_mkdir(const char* ref,mode_t mode){
   return _vfs_creat(ref,S_IFDIR,mode);
}

/*-------------------------------------------
| Name:_vfs_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
desc_t _vfs_open(const char* ref, int oflag, mode_t mode){

   char* filename; //to remove: Only for test.

   desc_t _desc;

   if(ref[0]=='\0')
      return -1;

   if(oflag&O_CREAT) {
      if(_vfs_creat(ref,S_IFREG,mode)<0 && (oflag&O_EXCL)) {
         __kernel_set_errno(EEXIST);
         return -1;
      }
   }
   //
   if(_vfs_lookup(ref,&_desc,&filename)<0) {
      _vfs_putdesc(_desc);  //file not exist.
      __kernel_set_errno(ENOENT);
      return INVALID_DESC;
   }

   //
   if(_desc<0) {
      __kernel_set_errno(ENFILE);
      return -1;
   }

   //set oflag
   ofile_lst[_desc].oflag=oflag;

   //assign owner thread
   if(oflag&O_RDONLY) {
      ofile_lst[_desc].owner_pthread_ptr_read  = __get_syscall_owner_pthread_ptr();
   }

   if(oflag&O_WRONLY) {
      ofile_lst[_desc].owner_pthread_ptr_write  = __get_syscall_owner_pthread_ptr();
   }

   //open file on physical disk device
   // and ofile_lst[_desc] information is filled by inode information.
   ofile_lst[_desc].pfsop->fs.open(_desc);

   //if attr == S_IFBLK or S_IFCHR (block or character device)
   if( ofile_lst[_desc].attr&S_IFCHR
       || ofile_lst[_desc].attr&S_IFBLK) {
      // open device
      dev_t dev;
      //1) get device id
      dev = ofile_lst[_desc].ext.dev;

      //it's a stream! open operation on attached file
      if(S_ISLNK(ofile_lst[_desc].attr)) {
         //yes
         desc_t _attach_desc = (desc_t)(dev-max_dev);
         pid_t _owner_pid   = ofile_lst[_desc].owner_pid;
         kernel_pthread_t*  _owner_pthread_ptr_read  = ofile_lst[_desc].owner_pthread_ptr_read;
         kernel_pthread_t*  _owner_pthread_ptr_write = ofile_lst[_desc].owner_pthread_ptr_write;
         //to do: make copy for dev->desc) in _desc
         // or return dev-desc???
         // _vfs_open() note 1: be aware: S_IFLNK flag is copied on new structure of desc from _attach_desc
         // see _vfs_close to remove this flag when device is closed definitly (nb_reader and nb_writer reach 0)
         memcpy(&ofile_lst[_desc],&ofile_lst[_attach_desc],sizeof(ofile_t));
         ofile_lst[_desc].owner_pid = _owner_pid;
         ofile_lst[_desc].owner_pthread_ptr_read  = _owner_pthread_ptr_read;
         ofile_lst[_desc].owner_pthread_ptr_write = _owner_pthread_ptr_write;
         //keep original descriptor of attached devices (important se in devio.c for read() write() used by __io_lock() __io_unlock() ).
         ofile_lst[_desc].desc = _attach_desc; //keep implicit with previous memcpy but i prefer explicit copy here :-)
         //
         ofile_lst[_desc].nb_reader = 0;
         ofile_lst[_desc].nb_writer = 0;
         ofile_lst[_desc].oflag = oflag;

         //read branch. only stream compatible mode in this branch  see(_vfs_link_desc() in vfs_core.c)
         _attach_desc=_desc;
         if(ofile_lst[_attach_desc].oflag&O_RDONLY) {
            while((_attach_desc=ofile_lst[_attach_desc].desc_nxt[0])>=0) {
               ofile_lst[_attach_desc].owner_pid = _owner_pid;
               ofile_lst[_attach_desc].owner_pthread_ptr_read = _owner_pthread_ptr_read;
               ofile_lst[_attach_desc].nb_reader++;
            }
         }

         //write branch. only stream compatible mode in this branch  see(_vfs_link_desc() in vfs_core.c)
         _attach_desc=_desc;
         if(ofile_lst[_attach_desc].oflag&O_WRONLY) {
            while((_attach_desc=ofile_lst[_attach_desc].desc_nxt[1])>=0) {
               ofile_lst[_attach_desc].owner_pid = _owner_pid;
               ofile_lst[_attach_desc].owner_pthread_ptr_write = _owner_pthread_ptr_write;
               ofile_lst[_attach_desc].nb_writer++;
            }
         }

         //_vfs_putdesc(_desc);

         //get desc

      }else{
         //no
         //2) remap pfsop
         if(!pdev_lst[dev]) {
            ofile_lst[_desc].pfsop->fs.close(_desc);
            _vfs_putdesc(_desc);
            __kernel_set_errno(ENODEV);
            return -1;
         }

         //3) call pfsop->dev.open: ofile_lst[_desc].pfsop->fdev.fdev_open();
         if(((pfsop_t)pdev_lst[dev])->fdev.fdev_open(_desc,oflag)<0) {
            ofile_lst[_desc].pfsop->fs.close(_desc);
            _vfs_putdesc(_desc);
            __kernel_set_errno(EIO);
            return -1;
         }

         //4) swap filesystem operation to device operation
         ofile_lst[_desc].pfsop = (pfsop_t)pdev_lst[dev];
      }
   }else if(ofile_lst[_desc].attr&S_IFIFO) {


      if(ofile_lst[_desc].ext.pipe_desc==-1) {
         const char fifo_path[]="/dev/fifo";

         desc_t desc_fifo_r=_vfs_open(fifo_path,O_RDONLY,mode);
         desc_t desc_fifo_w=_vfs_open(fifo_path,O_WRONLY,mode);
         ofile_lst[_desc].ext.pipe_desc = ofile_lst[desc_fifo_r].ext.pipe_desc;
         ofile_lst[_desc].pfsop->fs.close(_desc); //update pipe_desc;

         ofile_lst[desc_fifo_r].attr|=S_IFIFO;
         ofile_lst[desc_fifo_w].attr|=S_IFIFO;

         _vfs_putdesc(_desc); //


         if(oflag&O_RDONLY) {
            ofile_lst[desc_fifo_w].nb_writer=0;
            return desc_fifo_r;
         }else if(oflag&O_WRONLY) {
            ofile_lst[desc_fifo_r].nb_reader=0;
            return desc_fifo_w;
         }

      }else{
         pipe_desc_t pipe_desc = ofile_lst[_desc].ext.pipe_desc;
         desc_t desc_fifo_r = opipe_lst[pipe_desc].desc_r;
         desc_t desc_fifo_w = opipe_lst[pipe_desc].desc_w;

         _vfs_putdesc(_desc); //
         //
         if((oflag&O_RDONLY) && !(oflag&O_NONBLOCK)) {
            _desc=desc_fifo_r;
            if(!ofile_lst[_desc].nb_reader) {
               __fire_io(ofile_lst[desc_fifo_w].owner_pthread_ptr_write);
            }

         }else if((oflag&O_WRONLY) && !(oflag&O_NONBLOCK)) {
            _desc=desc_fifo_w;
            if(!ofile_lst[_desc].nb_writer) {
               __fire_io(ofile_lst[desc_fifo_r].owner_pthread_ptr_read);
            }
         }


      }

   }else{
      //Regular file or directory
      if(oflag&O_APPEND)
         ofile_lst[_desc].pfsop->fs.seek(_desc,0,SEEK_END);
      else
         ofile_lst[_desc].pfsop->fs.seek(_desc,0,SEEK_SET);

      if(oflag&O_TRUNC)
         ofile_lst[_desc].pfsop->fs.truncate(_desc,0);
   }

   if(oflag&O_RDONLY) {
      ofile_lst[_desc].nb_reader++;
   }
   if(oflag&O_WRONLY) {
      ofile_lst[_desc].nb_writer++;
   }

   return _desc;
}

/*-------------------------------------------
| Name:_vfs_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _vfs_close(desc_t desc){

   //printf("->close [%d] oflag=0x%x rd=%d wr=%d\n",desc,ofile_lst[desc].oflag,ofile_lst[desc].nb_reader,ofile_lst[desc].nb_writer);
   if(desc<0)
      return -1;

   if((ofile_lst[desc].oflag&O_RDONLY) && (ofile_lst[desc].nb_reader>0)) {
      ofile_lst[desc].nb_reader--;
   }
   if((ofile_lst[desc].oflag&O_WRONLY) && (ofile_lst[desc].nb_writer>0)) {
      ofile_lst[desc].nb_writer--;
   }

   if( ofile_lst[desc].attr&S_IFCHR
       || ofile_lst[desc].attr&S_IFBLK) {

      //only for linked or attached device
      if(S_ISLNK(ofile_lst[desc].attr)) {
         desc_t _attach_desc;
         //read branch. only stream compatible mode in this branch  see(_vfs_link_desc() in vfs_core.c)
         _attach_desc=desc;
         if(ofile_lst[_attach_desc].oflag&O_RDONLY) {
            while((_attach_desc=ofile_lst[_attach_desc].desc_nxt[0])>=0) {
               ofile_lst[_attach_desc].nb_reader--;
            }
         }

         //write branch. only stream compatible mode in this branch  see(_vfs_link_desc() in vfs_core.c)
         _attach_desc=desc;
         if(ofile_lst[_attach_desc].oflag&O_WRONLY) {
            while((_attach_desc=ofile_lst[_attach_desc].desc_nxt[1])>=0) {
               ofile_lst[_attach_desc].nb_writer--;
            }
         }

         //it's close operation from open operation on attached file (see _vfs_open() note 1 )
         if((desc!=ofile_lst[desc].desc)) {
            if(!ofile_lst[desc].nb_reader && !ofile_lst[desc].nb_writer)
               ofile_lst[desc].attr&=~(S_IFLNK);
         }
      }else if(!(S_ISLNK(ofile_lst[desc].attr)) ) {
         //not linked or attached device
         if(ofile_lst[desc].pfsop->fdev.fdev_close)
            ofile_lst[desc].pfsop->fdev.fdev_close(desc);
      }
   }else{
      if(   !ofile_lst[desc].nb_reader
            && !ofile_lst[desc].nb_writer
            && ofile_lst[desc].used)
         ofile_lst[desc].pfsop->fs.close(desc);
   }

   if(!ofile_lst[desc].nb_reader
      && !ofile_lst[desc].nb_writer)
      _vfs_putdesc(desc);


   return 0;
}

/*-------------------------------------------
| Name:_vfs_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _vfs_read(desc_t desc,char* buf, size_t size){
   if((ofile_lst[desc].attr&(S_IFCHR|S_IFBLK)))
      if(ofile_lst[desc].pfsop->fdev.fdev_read) {
         return ofile_lst[desc].pfsop->fdev.fdev_read(desc,buf,size);
      }else{
         __kernel_set_errno(ENOTSUP);
         return -1;
      }
   return ofile_lst[desc].pfsop->fs.read(desc,buf,size);
}

/*-------------------------------------------
| Name:_vfs_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _vfs_write(desc_t desc,char* buf, size_t size){
   int cb;
   static char _buf[32]={0};
   if((ofile_lst[desc].attr&(S_IFCHR|S_IFBLK)))
      if(ofile_lst[desc].pfsop->fdev.fdev_write) {
         return ofile_lst[desc].pfsop->fdev.fdev_write(desc,buf,size);
      }else{
         __kernel_set_errno(ENOTSUP);
         return -1;
      }
   if((cb=ofile_lst[desc].offset-ofile_lst[desc].size)>0) {
      ofile_lst[desc].offset-=cb;
      for(; cb>0; ) {
         if(((cb>>5)<<5))
            ofile_lst[desc].pfsop->fs.write(desc,_buf,sizeof(_buf));
         else
            ofile_lst[desc].pfsop->fs.write(desc,_buf,cb);

         cb-=sizeof(_buf);
      }
   }

   return ofile_lst[desc].pfsop->fs.write(desc,buf,size);
}

/*-------------------------------------------
| Name:_vfs_lseek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _vfs_lseek(desc_t desc, int offset, int origin){
   if((ofile_lst[desc].attr&S_IFBLK))
      if (ofile_lst[desc].pfsop->fdev.fdev_seek) {
         return ofile_lst[desc].pfsop->fdev.fdev_seek(desc,offset,origin);
      }else{
         __kernel_set_errno(ENOTSUP);
         return -1;
      }
   return ofile_lst[desc].pfsop->fs.seek(desc,offset,origin);
}

/*--------------------------------------------
| Name:        _vfs_ioctl2
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int _vfs_ioctl2(desc_t desc, int request, va_list ap){
   va_list _ap;
   va_list __ap;
   int ret;

   __va_list_copy(_ap,ap);

   if(!(ofile_lst[desc].attr&(S_IFCHR|S_IFBLK))) {
      __kernel_set_errno(ENOTSUP);
      return -1;
   }
   if(!ofile_lst[desc].pfsop->fdev.fdev_ioctl) {
      __kernel_set_errno(ENOTSUP);
      return -1;
   }

   switch(request) {
   case I_LINK: {
      desc_t desc_link;
#if defined(__GNUC__)
      desc_link = va_arg(_ap, int);
#else
      desc_link = va_arg(_ap, desc_t);
#endif
      if(desc_link<0)
         return -1;
      if(_vfs_link_desc(desc,desc_link)<0)
         return -1;
      //
      __ap = va_arg(_ap, va_list);
      //
      if((ret = ofile_lst[desc].pfsop->fdev.fdev_ioctl(desc,request,__ap))<0) {
         _vfs_unlink_desc(desc);
         return -1;
      }

   }
   break;

   case I_UNLINK: {
      //
      ret = ofile_lst[desc].pfsop->fdev.fdev_ioctl(desc,request,ap);
      //
      _vfs_unlink_desc(desc);
   }
   break;

   default:
      ret = ofile_lst[desc].pfsop->fdev.fdev_ioctl(desc,request,ap);
      break;
   }

   return ret;
}

/*-------------------------------------------
| Name:_vfs_ioctl
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _vfs_ioctl(desc_t desc, int request, ... ){
   va_list ap;
   int ret;
   va_start(ap, request);
   ret = _vfs_ioctl2(desc,request,ap);
   va_end(ap);
   return ret;
}

/*-------------------------------------------
| Name:_vfs_ftruncate
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _vfs_ftruncate(desc_t desc,off_t length){
   ofile_lst[desc].pfsop->fs.truncate(desc,length);
   return 0;
}

/*-------------------------------------------
| Name:_vfs_truncate
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _vfs_truncate(const char *ref,off_t length){
   char* filename;
   desc_t desc;
   int r;

   //to do: only for S_IFREG
   if(_vfs_lookup(ref,&desc,&filename)<0) {
      _vfs_putdesc(desc);  //no exist
      __kernel_set_errno(ENOENT);
      return -1;
   }

   //
   if(desc<0)
      return -1;

   //
   r = _vfs_ftruncate(desc,length);

   _vfs_putdesc(desc);

   return r;
}

/*-------------------------------------------
| Name:_vfs_remove
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _vfs_remove(const char *ref){
   char* filename;
   desc_t desc;

   if(_vfs_lookup(ref,&desc,&filename)<0) {
      _vfs_putdesc(desc);  //file not exist.
      __kernel_set_errno(ENOENT);
      return -1;
   }

   //
   if(desc<0)
      return -1;

   //
   ofile_lst[desc].pfsop->fs.open(desc);

   //to do: implement unlink() for S_IFCHR,S_IFBLK,S_IFIFO
   if((ofile_lst[desc].attr&(S_IFREG|S_IFIFO))) {
      ofile_lst[desc].pfsop->fs.close(desc);
      _vfs_putdesc(desc);
      return _vfs_rm(ref);
   }else if((ofile_lst[desc].attr&S_IFDIR)) {
      ofile_lst[desc].pfsop->fs.close(desc);
      _vfs_putdesc(desc);
      return _vfs_rmdir(ref);
   }

   //note : unlink can be used for S_IFDIR.
   ofile_lst[desc].pfsop->fs.close(desc);
   _vfs_putdesc(desc);

   __kernel_set_errno(EXDEV);
   return -1;
}

/*-------------------------------------------
| Name:_vfs_unlink
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _vfs_unlink(const char *ref){
   return 0;
}

/*-------------------------------------------
| Name:_vfs_rm
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _vfs_rm(const char *ref){

   char* filename;
   desc_t desc;
   desc_t desc_ancst;
   int r;

   if(!strcmp(ref,__dot) || !strcmp(ref,__dotdot))
      return -1;  //ref="." or ".." cannot be removed

   if(_vfs_lookup(ref,&desc,&filename)<0) {
      _vfs_putdesc(desc);  //file not exist.
      __kernel_set_errno(ENOENT);
      return -1;
   }

   //
   if(desc<0)
      return -1;

   ofile_lst[desc].pfsop->fs.open(desc);

   //to do: verify if this file is not used by anoter process


   //
   if(!(ofile_lst[desc].attr&(S_IFREG|S_IFIFO)) ) { //attr is not supported: error
      ofile_lst[desc].pfsop->fs.close(desc);
      _vfs_putdesc(desc);
      __kernel_set_errno(EISDIR);
      return -1;
   }

   //
   desc_ancst = _vfs_getdesc(ofile_lst[desc].dir_inodenb,INVALID_DESC);
   if(desc_ancst<0) {
      ofile_lst[desc].pfsop->fs.close(desc);
      _vfs_putdesc(desc); //no descriptor available
      return -1;
   }

   r= ofile_lst[desc].pfsop->fs.remove(desc_ancst,desc);

   ofile_lst[desc].pfsop->fs.close(desc);
   _vfs_putdesc(desc);
   _vfs_putdesc(desc_ancst);

   return r;
}

/*-------------------------------------------
| Name:_vfs_rmdir
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _vfs_rmdir(const char *ref){

   char* filename;
   desc_t desc;
   desc_t desc_ancst;
   struct dirent dirent;
   int r;
   int i;

   if(!strcmp(ref,__dot) || !strcmp(ref,__dotdot))
      return -1;  //ref="." or ".." cannot be removed!


   if(_vfs_lookup(ref,&desc,&filename)<0) {
      _vfs_putdesc(desc);  //file not exist.
      return -1;
   }

   //
   if(desc<0)
      return -1;

   ofile_lst[desc].pfsop->fs.open(desc);

   //to do: verify if this file is not used by anoter process

   //test if ref is a directory
   if(!(ofile_lst[desc].attr&S_IFDIR)) {
      ofile_lst[desc].pfsop->fs.close(desc);
      _vfs_putdesc(desc);  //is not directory.
      __kernel_set_errno(ENOTDIR);
      return -1;
   }

   //test if ref directory is empty
   while(_vfs_readdir(desc,&dirent)) {
      if(strcmp(dirent.d_name,__dot) && strcmp(dirent.d_name,__dotdot)) {
         ofile_lst[desc].pfsop->fs.close(desc);
         _vfs_putdesc(desc);  //directory is not empty:cannot be removed!.
         __kernel_set_errno(ENOTEMPTY);
         return -1;
      }
   }

   //test if ref is a mount point
   for(i=0; i<MAX_MOUNT_DEVICE; i++) {
      if(mntdev_lst[i].psuperblk_info==NULL)
         continue;

      if(ofile_lst[desc].inodenb == mntdev_lst[i].inodenb_offset) {
         ofile_lst[desc].pfsop->fs.close(desc);
         _vfs_putdesc(desc);  //ref is a mount point: cannot be removed!.
         __kernel_set_errno(EPERM);
         return -1;
      }
   }

   //
   desc_ancst = _vfs_getdesc(ofile_lst[desc].dir_inodenb,INVALID_DESC);
   if(desc_ancst<0) {
      ofile_lst[desc].pfsop->fs.close(desc);
      _vfs_putdesc(desc); //no descriptor available
      return -1;
   }

   r= ofile_lst[desc].pfsop->fs.remove(desc_ancst,desc);

   ofile_lst[desc].pfsop->fs.close(desc);
   _vfs_putdesc(desc);
   _vfs_putdesc(desc_ancst);

   return r;
}

/*-------------------------------------------
| Name:_vfs_rename
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _vfs_rename(const char *old_name, const char *new_name ){

   char* new_filename;
   char* old_filename;
   desc_t desc;
   desc_t desc_ancst;
   int r;
   int i=0;

   inodenb_t old_dir_inodenb;
   inodenb_t new_dir_inodenb;

   if(!strcmp(old_name,__dot) || !strcmp(old_name,__dotdot)) {
      __kernel_set_errno(EPERM);
      return -1; //ref="." or ".." cannot be renamed
   }

   if(!strcmp(new_name,__dot) || !strcmp(new_name,__dotdot)) {
      __kernel_set_errno(EPERM);
      return -1; //ref="." or ".." cannot be renamed
   }

   //
   for(i=strlen(new_name); i>0; --i) {
      if(new_name[i]=='/') break;
      new_filename=(char*)&new_name[i];
   }
   //
   for(i=strlen(old_name); i>0; --i) {
      if(old_name[i]=='/') break;
      old_filename=(char*)&old_name[i];
   }

   //new name
   //check if new name already exist
   if(_vfs_lookup(new_name,&desc,(void*)0)>=0) {
      _vfs_putdesc(desc);  //file already exist.
      __kernel_set_errno(EEXIST);
      return -1;
   }
   //
   if(desc<0)
      return -1;  //kernel panic!!! (desc not available)

   //
   ofile_lst[desc].pfsop->fs.open(desc);
   //
   if(!(ofile_lst[desc].attr&S_IFDIR)) {
      _vfs_putdesc(desc);
      __kernel_set_errno(ENOTDIR);
      return -1; //the parent node it's not a directory
   }
   ofile_lst[desc].pfsop->fs.close(desc);

   //it's a directory. good:)
   new_dir_inodenb=ofile_lst[desc].inodenb;
   //
   _vfs_putdesc(desc);


   //previous name entry
   //find the file to be renamed
   if(_vfs_lookup(old_name,&desc,(void*)0)<0) {
      _vfs_putdesc(desc);  //file not exist.
      __kernel_set_errno(ENOENT);
      return -1;
   }
   //
   if(desc<0)
      return -1;  //kernel panic!!! (desc not available)
   //
   old_dir_inodenb=ofile_lst[desc].dir_inodenb;

   //it's on the same path
   if(old_dir_inodenb!=new_dir_inodenb) {
      _vfs_putdesc(desc);  //operation supported only on the same path.(limitation)
      __kernel_set_errno(ENOTSUP);
      return -1;
   }

   //check if this file is not used by anoter process or pthread?
   for(i=0; i<MAX_OPEN_FILE; i++) {
      if(    (i==desc) //don't check the current descriptor. it's used.
             ||  (ofile_lst[i].inodenb!=ofile_lst[desc].inodenb) //it's not the current file. inode are not the same.
             ||  (!ofile_lst[i].owner_pthread_ptr_read && !ofile_lst[i].owner_pthread_ptr_write) //don't used by any process and pthread.
             )
         continue;

      _vfs_putdesc(desc);  //file is already used by another process or pthread.
      //
      __kernel_set_errno(EINPROGRESS);
      return -1;
   }

   //start to rename operation
   ofile_lst[desc].pfsop->fs.open(desc);
   //
   if( !(ofile_lst[desc].attr&(S_IFREG)) && !(ofile_lst[desc].attr&(S_IFDIR))) { //attr is not supported: error
      ofile_lst[desc].pfsop->fs.close(desc);
      _vfs_putdesc(desc);
      __kernel_set_errno(ENOTSUP);
      return -1;
   }

   //
   desc_ancst = _vfs_getdesc(ofile_lst[desc].dir_inodenb,INVALID_DESC);
   if(desc_ancst<0) {
      ofile_lst[desc].pfsop->fs.close(desc);
      _vfs_putdesc(desc); //no descriptor available
      return -1;
   }

   //
   r= ofile_lst[desc].pfsop->fs.open(desc_ancst);
   //
   r= ofile_lst[desc].pfsop->fs.rename(desc_ancst,old_filename,new_filename);
   //
   ofile_lst[desc].pfsop->fs.close(desc);
   _vfs_putdesc(desc);
   _vfs_putdesc(desc_ancst);

   return r;
}

/*-------------------------------------------
| Name:_vfs_chdir
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _vfs_chdir(const char* path){
   char* filename;
   desc_t desc;

   //to do:check pid value: must be > 0.
   pid_t pid = __get_syscall_owner_pid();

   if(_vfs_lookup(path,&desc,&filename)<0) {
      _vfs_putdesc(desc);  //no exist
      __kernel_set_errno(ENOENT);
      return -1;
   }

   //
   if(desc<0)
      return -1;

   ofile_lst[desc].pfsop->fs.open(desc);

   if(!(ofile_lst[desc].attr&S_IFDIR)) {
      ofile_lst[desc].pfsop->fs.close(desc);
      _vfs_putdesc(desc);
      __kernel_set_errno(ENOTDIR);
      return -1;
   }

   //to do:check pid value: must be > 0.
   process_lst[__get_syscall_owner_pid()]->inode_curdir = ofile_lst[desc].inodenb;

   ofile_lst[desc].pfsop->fs.close(desc);
   _vfs_putdesc(desc);

   return 0;
}

/*-------------------------------------------
| Name:_vfs_opendir
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
desc_t _vfs_opendir(const char* path){
   desc_t desc;
   desc = _vfs_open(path,O_RDONLY,0);

   if(desc<0) {
      __kernel_set_errno(ENOENT);
      return -1;
   }

   if(!(ofile_lst[desc].attr&S_IFDIR)) {
      _vfs_putdesc(desc);
      __kernel_set_errno(ENOTDIR);
      return -1;
   }

   return desc;
}

/*-------------------------------------------
| Name:_vfs_closedir
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _vfs_closedir(desc_t desc){

   if(!(ofile_lst[desc].attr&S_IFDIR)) {
      __kernel_set_errno(ENOTDIR);
      return -1;
   }
   _vfs_putdesc(desc);
   return 0;
}

/*-------------------------------------------
| Name:_vfs_readdir
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
struct dirent* _vfs_readdir(desc_t desc,struct dirent* dirent){

   if(!(ofile_lst[desc].attr&S_IFDIR)) {
      __kernel_set_errno(ENOTDIR);
      return (struct dirent*)0;
   }

   if(ofile_lst[desc].pfsop->fs.readdir(desc,dirent)<0)
      return (struct dirent*)0;

   return dirent;
}

/*-------------------------------------------
| Name:_vfs_rewinddir
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void _vfs_rewinddir(desc_t desc){

   if(!(ofile_lst[desc].attr&S_IFDIR)) {
      __kernel_set_errno(ENOTDIR);
      return;
   }

   ofile_lst[desc].pfsop->fs.seek(desc,0,SEEK_SET);
}

/*-------------------------------------------
| Name:_vfs_telldir
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _vfs_telldir(desc_t desc){

   if(!(ofile_lst[desc].attr&S_IFDIR)) {
      __kernel_set_errno(ENOTDIR);
      return -1;
   }

   return ofile_lst[desc].pfsop->fs.telldir(desc);
}

/*-------------------------------------------
| Name:_vfs_seekdir
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _vfs_seekdir(desc_t desc, int loc){

   if(!(ofile_lst[desc].attr&S_IFDIR)) {
      __kernel_set_errno(ENOTDIR);
      return -1;
   }

   return ofile_lst[desc].pfsop->fs.seekdir(desc,loc);
}

/*-------------------------------------------
| Name:_vfs_getcwd
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _vfs_getcwd(char* buf,size_t size){
   //to do:check pid value: must be > 0.
   inodenb_t inodenb = process_lst[__get_syscall_owner_pid()]->inode_curdir;
   inodenb_t parent_inodenb;
   desc_t desc;
   struct dirent dirent;
   int err;
   size_t len=0;
   size_t _len=0;
   buf[0]='\0';


   memset(buf,0,size);

   if(!inodenb) {
      buf[0]='/';
      buf[1]='\0';
      return 0;
   }

   // "/" root
   while(1) {

      if(!inodenb)
         return 0;

      len =strlen(buf);
      if(len==size)
         return -1;  //buf too small

      //
      desc = _vfs_getdesc(inodenb,INVALID_DESC);
      err=0;
      ofile_lst[desc].pfsop->fs.open(desc);
      if(ofile_lst[desc].inodenb != ofile_lst[desc].pmntdev->inodenb_offset) {
         while((err=ofile_lst[desc].pfsop->fs.readdir(desc,&dirent))>=0) {
            if(!strcmp(dirent.d_name,__dotdot))
               break;
         }
         parent_inodenb = dirent.inodenb;
      }else{
         ofile_lst[desc].pfsop->fs.close(desc);
         _vfs_putdesc(desc);

         parent_inodenb = ofile_lst[desc].pmntdev->mnt_inodenb;
         desc = _vfs_getdesc(parent_inodenb,INVALID_DESC);
         ofile_lst[desc].pfsop->fs.open(desc);
         while((err=ofile_lst[desc].pfsop->fs.readdir(desc,&dirent))>=0) {
            if(!strcmp(dirent.d_name,__dotdot))
               break;
         }
         parent_inodenb = dirent.inodenb;
      }
      ofile_lst[desc].pfsop->fs.close(desc);
      _vfs_putdesc(desc);

      if(err<0)
         break;

      //
      desc = _vfs_getdesc(parent_inodenb,INVALID_DESC);
      ofile_lst[desc].pfsop->fs.open(desc);
      while((err=ofile_lst[desc].pfsop->fs.readdir(desc,&dirent))>=0) {
         if(inodenb==dirent.inodenb)
            break;
      }
      ofile_lst[desc].pfsop->fs.close(desc);
      _vfs_putdesc(desc);

      if(err<0)
         break;

      _len = strlen(dirent.d_name);
      //memcpy(&buf[_len+1],buf,len+1);
      memmove(&buf[_len+1],buf,len+1);
      memcpy(&buf[1],dirent.d_name,_len);
      buf[0]='/';

      inodenb = parent_inodenb;
   }

   return 0;
}

/*-------------------------------------------
| Name:_vfs_fstatvfs
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _vfs_fstatvfs(desc_t desc, struct statvfs *buf){

   fsop_t* pfsop        = _vfs_mntdev2fsop(ofile_lst[desc].pmntdev);

   return pfsop->fs.statfs(ofile_lst[desc].pmntdev,buf);
}

/*-------------------------------------------
| Name:_vfs_statvfs
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _vfs_statvfs(const char *path, struct statvfs *buf){
   char* filename;
   desc_t desc;
   int r;

   if(_vfs_lookup(path,&desc,&filename)<0) {
      __kernel_set_errno(ENOENT);
      _vfs_putdesc(desc);  //no exist
      return -1;
   }

   //
   if(desc<0)
      return -1;


   r = _vfs_fstatvfs(desc,buf);

   _vfs_putdesc(desc);

   return r;
}

/*-------------------------------------------
| Name:_vfs_fstat
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _vfs_fstat(desc_t desc, struct stat * stat){

   stat->st_ino  = ofile_lst[desc].inodenb;
   stat->st_size = ofile_lst[desc].size;
   stat->st_mode = ofile_lst[desc].attr;

   stat->st_atime = ofile_lst[desc].cmtime;
   stat->st_ctime = ofile_lst[desc].cmtime;
   stat->st_mtime = ofile_lst[desc].cmtime;

   stat->st_dev  = -1;

   if( ofile_lst[desc].attr&S_IFCHR
       || ofile_lst[desc].attr&S_IFBLK) {
      stat->st_dev = ofile_lst[desc].ext.dev;
   }

   return 0;
}

/*-------------------------------------------
| Name:_vfs_stat
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _vfs_stat(const char * ref, struct stat * stat){
   char* filename;
   desc_t desc;
   int r;

   if(_vfs_lookup(ref,&desc,&filename)<0) {
      _vfs_putdesc(desc);  //no exist
      __kernel_set_errno(ENOENT);
      return -1;
   }

   //
   if(desc<0)
      return -1;

   //
   ofile_lst[desc].pfsop->fs.open(desc);
   r = _vfs_fstat(desc,stat);
   ofile_lst[desc].pfsop->fs.close(desc);
   _vfs_putdesc(desc);

   return r;
}

/*-------------------------------------------
| Name:_vfs
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _vfs(void){

   int i=0;
   desc_t desc;

   //Init mount device list
   for(i=0; i<MAX_MOUNT_DEVICE; i++) {
      mntdev_lst[i].mnt_inodenb=INVALID_INODE_NB;
   }

   //Init open file list
   for(desc=0; desc<MAX_OPEN_FILE; desc++) {
      pthread_mutexattr_t mutex_attr=0;
      ofile_lst[desc].desc = -1;
      ofile_lst[desc].desc_nxt[0] = -1; //read stream
      ofile_lst[desc].desc_nxt[1] = -1; //write stream
      ofile_lst[desc].desc_prv = -1;
      ofile_lst[desc].inodenb=INVALID_INODE_NB;

      //kernel_pthread_mutex_init(&ofile_lst[desc].mutex,&mutex_attr);
      kernel_sem_init(&ofile_lst[desc].sem_read,0,1);
      kernel_sem_init(&ofile_lst[desc].sem_write,0,1);
   }

   //load file system
   for(i=0; i<nb_supportedfs; i++) {
      if(fsop_lst[i])
         fsop_lst[i]->fs.loadfs();
   }

   return 0;
}

/*-------------------------------------------
| Name:_vfs_ls
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
#if defined(__KERNEL_UCORE_EMBOS)
   #include <stdio.h>
#endif

int _vfs_ls(char* ref){
   desc_t desc;
   struct dirent dirent;

#if defined(__KERNEL_UCORE_EMBOS)
   printf("ls %s\n",ref);
#elif defined(__KERNEL_UCORE_ECOS)
   int i=0;
#endif

   if((desc=_vfs_opendir(ref))<0)
      return -1;

   while(_vfs_readdir(desc,&dirent)) {
#if defined(__KERNEL_UCORE_EMBOS)
      printf("[%d] %s\n",dirent.inodenb,dirent.d_name);
#elif defined(__KERNEL_UCORE_ECOS)
      i++;
#endif
   }

   _vfs_closedir(desc);


   return 0;
}

/*===========================================
End of Sourcevfsroutine.c
=============================================*/
