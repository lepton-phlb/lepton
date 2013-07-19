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
#include "kernel/core/dirent.h"
#include "kernel/core/errno.h"
#include "kernel/core/system.h"
#include "kernel/core/process.h"
#include "kernel/core/kernel.h"
#include "kernel/core/fcntl.h"
#include "kernel/core/stat.h"

#include "kernel/fs/vfs/vfstypes.h"
#include "kernel/fs/vfs/vfskernel.h"
#include "kernel/fs/vfs/vfs.h"

#include "kernel/fs/rootfs/rootfs.h"
#include "kernel/fs/kofs/kofs.h"
#include "kernel/fs/ufs/ufs.h"
#include "kernel/fs/ufs/ufsx.h"
#include "kernel/fs/fat/fat16.h"

//#include <stdio.h>
#include <string.h>
#include <stdlib.h>


/*===========================================
Global Declaration
=============================================*/
#define VFS_INODENB_ROOT   0
#define VFS_ROOT_MNT       0

#define NO_DEV_DESC     -1

//
inodenb_t current_inodenb=VFS_INODENB_ROOT;

//superblock
superblk_t superblk_lst[MAX_SUPER_BLOCK];

//open file list
ofile_t ofile_lst[MAX_OPEN_FILE]={-1};

//mounted device list
mntdev_t mntdev_lst[MAX_MOUNT_DEVICE];

#if __KERNEL_VFS_SUPPORT_EFFS==1
   extern fsop_t effs_op;
#endif

//file system supported
pfsop_t const fsop_lst[MAX_FILESYSTEM]={
#if __KERNEL_VFS_SUPPORT_ROOTFS==1 
   &rootfs_op,
#endif
#if __KERNEL_VFS_SUPPORT_UFS==1 
   &ufs_op,
#endif
#if __KERNEL_VFS_SUPPORT_UFSX==1 
   &ufsx_op,
#endif
#if __KERNEL_VFS_SUPPORT_KOFS==1
   &kofs_op,
#endif
#if __KERNEL_VFS_SUPPORT_MSDOS==1
   &fat_msdos_op,
#endif
#if __KERNEL_VFS_SUPPORT_VFAT==1
   &fat_vfat_op,
#endif
#if __KERNEL_VFS_SUPPORT_EFFS==1
   &effs_op,
#endif
   0
};

const int nb_supportedfs = (sizeof(fsop_lst)/sizeof(pfsop_t))-1;


/*===========================================
Implementation
=============================================*/

char * _vfs_token(const char* str_token,const char* delimit){
   static char token_path[PATH_MAX+1];
   if(!str_token)
      return strtok(NULL,delimit);

   strcpy(token_path,str_token);
   return strtok(token_path,delimit);
}

/*-------------------------------------------
| Name:_vfs_mnt2fsop
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
pfsop_t _vfs_mntdev2fsop(mntdev_t* pmntdev){
   fstype_t fstype =  pmntdev->fstype;
   return fsop_lst[fstype];
}

/*-------------------------------------------
| Name:_vfs_getsuperblk
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
superblk_t* _vfs_getsuperblk(void){
   int i=0;

   for(i=0; i<MAX_SUPER_BLOCK; i++) {
      if( superblk_lst[i].alloc_node_size==0)
         return &superblk_lst[i];
   }

   return (superblk_t*)0; //to do:replace by null.
}

/*-------------------------------------------
| Name:_vfs_putsuperblk
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _vfs_putsuperblk(superblk_t* psuperblk){
   psuperblk->alloc_node_size=0;
   return 0;
}

/*-------------------------------------------
| Name:_vfs_addmnt
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
mntdev_t* _vfs_addmnt(mntdev_t* pcurmntdev,inodenb_t mnt_inodenb, desc_t dev_desc,fstype_t fstype){
   int i=0;

   for(i=0; i<MAX_MOUNT_DEVICE; i++) {
      int offset;

      if(mntdev_lst[i].psuperblk_info)
         continue;
      //
      mntdev_lst[i].dev_desc     = dev_desc;
      mntdev_lst[i].fstype       = fstype;

      //
      mntdev_lst[i].mnt_inodenb  = mnt_inodenb;

      mntdev_lst[i].psuperblk_info = _vfs_getsuperblk();

      //read files system when it's mounted
      if(fsop_lst[fstype]->fs.readfs(&mntdev_lst[i])<0) {
         mntdev_lst[i].pmntdev=NULL;
         _vfs_putsuperblk(mntdev_lst[i].psuperblk_info);
         mntdev_lst[i].psuperblk_info=NULL;
         __kernel_set_errno(EIO);
         return NULL;
      }

      if(!pcurmntdev) {
         //rootfs use RAM: boot device.
         mntdev_lst[i].inodenb_offset= 0;
         mntdev_lst[i].pmntdev   = &mntdev_lst[i];
         return &mntdev_lst[i];
      }

      //
      offset=mntdev_lst[i-1].inodenb_offset + mntdev_lst[i-1].inodetbl_size;
      if((i+1)<MAX_MOUNT_DEVICE && mntdev_lst[i+1].psuperblk_info!=NULL) {
         if(offset+mntdev_lst[i].inodetbl_size>mntdev_lst[i+1].inodenb_offset) {
            _vfs_putsuperblk(mntdev_lst[i].psuperblk_info);
            mntdev_lst[i].psuperblk_info=NULL;
            continue;
         }
      }

      mntdev_lst[i].inodenb_offset = offset;
      pcurmntdev->pmntdev = &mntdev_lst[i];

      //checkfs
      //while(fsop_lst[fstype]->fs.checkfs(&mntdev_lst[i])<0);//disabled: bug on i2c device.
      //fsop_lst[fstype]->fs.checkfs(&mntdev_lst[i]);

      return &mntdev_lst[i];
   }

   return 0;
}

/*-------------------------------------------
| Name:_vfs_removemnt
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
mntdev_t* _vfs_removemnt(inodenb_t inodenb){

   int i=0;
   fstype_t fstype;

   for(i=0; i<MAX_MOUNT_DEVICE; i++) {
      if( mntdev_lst[i].inodenb_offset!=inodenb)
         continue;

      //
      fstype = mntdev_lst[i].fstype;
      //write file system block when it's unmounted
      fsop_lst[fstype]->fs.writefs(&mntdev_lst[i]);

      //
      mntdev_lst[i].pmntdev=NULL;

      _vfs_putsuperblk(mntdev_lst[i].psuperblk_info);
      mntdev_lst[i].psuperblk_info=NULL;

      return &mntdev_lst[i];
   }

   return 0;
}

/*-------------------------------------------
| Name:_vfs_getmnt
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
mntdev_t* _vfs_getmnt(desc_t dir_desc,desc_t desc){

   int i=0;
   mntdev_t* pcurmntdev =(mntdev_t*)0;
   inodenb_t inodenb;

   if(dir_desc!=INVALID_DESC)
      pcurmntdev=ofile_lst[dir_desc].pmntdev;

   if(desc==INVALID_DESC)
      return &mntdev_lst[VFS_ROOT_MNT];

   inodenb = ofile_lst[desc].inodenb;

   for(i=0; i<MAX_MOUNT_DEVICE; i++) {
      if( mntdev_lst[i].psuperblk_info==NULL /*||  mntdev_lst[i].mnt_inodenb == INVALID_INODE_NB*/)
         continue;

      if(inodenb>=mntdev_lst[i].inodenb_offset
         && inodenb<(mntdev_lst[i].inodenb_offset+mntdev_lst[i].inodetbl_size)) { //retrieve filesystem with current inode
         return &mntdev_lst[i];
      }
   }

   return pcurmntdev;
}


/*-------------------------------------------
| Name:_vfs_rootmnt
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _vfs_rootmnt(void){
   _vfs_addmnt(NULL,VFS_INODENB_ROOT,NO_DEV_DESC,fs_rootfs);
   return 0;
}

/*-------------------------------------------
| Name:_vfs_lookupmnt
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _vfs_lookupmnt(const char* ref,desc_t* desc,char* filename){

   inodenb_t _inodenb       = VFS_INODENB_ROOT;

   desc_t _desc;
   desc_t __desc;

   char sep[]="/";
   char* token;

   *desc=INVALID_DESC;

   if((_desc = _vfs_getdesc(_inodenb,INVALID_DESC))<0)
      return -1;

   token = _vfs_token(ref,sep);

   while( token !=NULL) {

      _inodenb=ofile_lst[_desc].pfsop->fs.lookupdir(_desc,token);

      if(_inodenb==INVALID_INODE_NB) {
         *desc=_desc;
         strcpy(filename,token);
         return -1;
      }

      strcpy(filename,token);
      token = _vfs_token(NULL,sep);
      if(!token) {
         ofile_lst[_desc].pfsop->fs.open(_desc);
         *desc=_desc;
         return 0;
      }

      __desc=_desc;
      _desc  = _vfs_getdesc(_inodenb,_desc);
      _vfs_putdesc(__desc);
      if(_desc<0)
         return -1;

      ofile_lst[_desc].pfsop->fs.open(_desc);
      if( (ofile_lst[_desc].attr&S_IFDIR)!=S_IFDIR) {
         ofile_lst[_desc].pfsop->fs.close(_desc);
         *desc=_desc;
         __kernel_set_errno(ENOTDIR);
         return -1;
      }

      ofile_lst[_desc].pfsop->fs.close(_desc);
   }


   *desc=_desc;
   return 0;
}

/*-------------------------------------------
| Name:_vfs_getdesc
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
desc_t _vfs_getdesc(inodenb_t inodenb,desc_t ancestor_desc){

   desc_t desc;

   for(desc=0; desc<MAX_OPEN_FILE; desc++) {
      if(!ofile_lst[desc].used) {
         ofile_lst[desc].used++;

         ofile_lst[desc].cmtime       = 0L;
         ofile_lst[desc].status       = 0x00;
         ofile_lst[desc].inodenb      = inodenb;

         //warning!!!: pointer must be free before: responsability of developper
         //to release the pointer in device driver for dev_xxx_close() operation.
         ofile_lst[desc].p= (void*)0;

         if(ancestor_desc==INVALID_DESC)
            ofile_lst[desc].dir_inodenb  = VFS_INODENB_ROOT;
         else
            ofile_lst[desc].dir_inodenb  = ofile_lst[ancestor_desc].inodenb;

         ofile_lst[desc].owner_pid               = __get_syscall_owner_pid();
         ofile_lst[desc].owner_pthread_ptr_read  = (kernel_pthread_t*)0;
         ofile_lst[desc].owner_pthread_ptr_write = (kernel_pthread_t*)0;
         ofile_lst[desc].desc         = desc;
         ofile_lst[desc].offset       = 0;
         ofile_lst[desc].pmntdev      = _vfs_getmnt(ancestor_desc,desc);
         ofile_lst[desc].pfsop        = _vfs_mntdev2fsop(ofile_lst[desc].pmntdev);
         ofile_lst[desc].oflag        = 0;

         //printf("get desc[%d]\n",_desc);
         return desc;
      }
   }

   __kernel_set_errno(ENFILE);

   return INVALID_DESC;
}

/*-------------------------------------------
| Name:_vfs_dupdesc
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
desc_t _vfs_dupdesc(desc_t desc){

   desc_t _desc;

   if(desc<0) {
      __kernel_set_errno(EINVAL);
      return INVALID_DESC;
   }

   for(_desc=0; _desc<MAX_OPEN_FILE; _desc++) {
      if(!ofile_lst[_desc].used) {
         memcpy(&ofile_lst[_desc],&ofile_lst[desc],sizeof(ofile_t));
         return _desc;
      }
   }

   __kernel_set_errno(ENFILE);

   return INVALID_DESC;
}

/*-------------------------------------------
| Name:_vfs_putdesc
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _vfs_putdesc(desc_t desc){

   desc_t _desc;
   if(desc<0) {
      __kernel_set_errno(EINVAL);
      return -1;
   }

   //read branch. only stream compatible mode in this branch  see(_vfs_link_desc() in vfs_core.c)
   _desc = desc;
   while(_desc>=0) {
      ofile_lst[_desc].nb_reader=0;
      ofile_lst[_desc].nb_writer=0;
      //protection from io interrupt
      __disable_interrupt_section_in();
      //
      ofile_lst[_desc].owner_pid=-1;
      ofile_lst[_desc].owner_pthread_ptr_read   = (kernel_pthread_t*)0;
      //
      __disable_interrupt_section_out();
      //

      ofile_lst[_desc].offset = 0;
      //is link by fattach() see I_LINK// must be fdetach() before.
      if(!S_ISLNK(ofile_lst[_desc].attr)) {
         if(ofile_lst[_desc].used)
            ofile_lst[_desc].used--;
         else
            ofile_lst[_desc].desc_nxt[0]=INVALID_DESC;
         return 0;
      }
      //
      if(!(ofile_lst[_desc].oflag&O_RDONLY))
         break;

      _desc=ofile_lst[_desc].desc_nxt[0];

   }


   //write branch. only stream compatible mode in this branch  see(_vfs_link_desc() in vfs_core.c)
   _desc = desc;
   while(_desc>=0) {
      ofile_lst[_desc].nb_reader=0;
      ofile_lst[_desc].nb_writer=0;
      //protection from io interrupt
      __disable_interrupt_section_in();
      //
      ofile_lst[_desc].owner_pid=-1;
      ofile_lst[_desc].owner_pthread_ptr_write  = (kernel_pthread_t*)0;
      //
      __disable_interrupt_section_out();
      //

      ofile_lst[_desc].offset = 0;
      //is link by fattach() see I_LINK// must be fdetach() before.
      if(!S_ISLNK(ofile_lst[_desc].attr)) {
         if(ofile_lst[_desc].used)
            ofile_lst[_desc].used--;
         else
            ofile_lst[_desc].desc_nxt[1]=INVALID_DESC;
         return 0;
      }
      //
      if(!(ofile_lst[_desc].oflag&O_WRONLY))
         break;

      _desc=ofile_lst[_desc].desc_nxt[1];
   }

   //printf("put desc[%d]\n",desc);
   return 0;
}

/*--------------------------------------------
| Name:        _vfs_link_desc
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int _vfs_link_desc(desc_t desc,desc_t desc_link){

   desc_t _desc=desc;

   if(desc<0 || desc_link<0)
      return -1;

   if(   !(ofile_lst[desc].oflag & ofile_lst[desc_link].oflag & O_RDONLY)
         && !(ofile_lst[desc].oflag & ofile_lst[desc_link].oflag & O_WRONLY) )
      return -1;  //incompatible oflag in the stream

   ofile_lst[desc_link].desc_prv = desc;

   //read mode stream
   if(ofile_lst[desc_link].oflag&O_RDONLY) {
      ofile_lst[desc].desc_nxt[0] = desc_link; //read 0
   }

   //write mode stream
   if(ofile_lst[desc_link].oflag&O_WRONLY) {
      ofile_lst[desc].desc_nxt[1] = desc_link; //write 1
   }

   //protection from io interrupt section begin
   __disable_interrupt_section_in();
   //pthread owner propagation for read branch
   _desc=desc;
   if(ofile_lst[desc].oflag&O_RDONLY) {
      while((_desc=ofile_lst[_desc].desc_nxt[0])>=0) {
         ofile_lst[_desc].owner_pthread_ptr_read=ofile_lst[desc].owner_pthread_ptr_read;
         ofile_lst[_desc].owner_pid=ofile_lst[desc].owner_pid;
      }
   }
   //pthread owner propagation for write branch
   _desc=desc;
   if(ofile_lst[desc].oflag&O_WRONLY) {
      while((_desc=ofile_lst[_desc].desc_nxt[1])>=0) {
         ofile_lst[_desc].owner_pthread_ptr_write=ofile_lst[desc].owner_pthread_ptr_write;
         ofile_lst[_desc].owner_pid=ofile_lst[desc].owner_pid;
      }
   }
   //protection from io interrupt section end
   __disable_interrupt_section_out();
   //

   return 0;
}

/*--------------------------------------------
| Name:        _vfs_unlink_desc
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int _vfs_unlink_desc(desc_t desc){

   if(desc<0)
      return -1;

   if(ofile_lst[desc].oflag&O_RDONLY) {
      desc_t desc_link;
      desc_link= ofile_lst[desc].desc_nxt[0]; //read 0
      if(desc_link<0)
         return -1;
      ofile_lst[desc_link].desc_prv = INVALID_DESC;
      ofile_lst[desc].desc_nxt[0] = INVALID_DESC;

   }

   //write mode stream
   if(ofile_lst[desc].oflag&O_WRONLY) {
      desc_t desc_link;
      desc_link= ofile_lst[desc].desc_nxt[1]; //write 1
      if(desc_link<0)
         return -1;
      ofile_lst[desc_link].desc_prv = INVALID_DESC;
      ofile_lst[desc].desc_nxt[1] = INVALID_DESC;
   }

   return 0;
}

/*-------------------------------------------
| Name:_vfs_lookup
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _vfs_lookup(const char* ref,desc_t* desc,char** filename){

   inodenb_t _inodenb       = VFS_INODENB_ROOT;

   desc_t _desc;
   desc_t __desc;

   const char sep[]="/";
   char* token;

   pid_t pid = __get_syscall_owner_pid();

   //
   *desc=INVALID_DESC;

   //
   if(ref[0]=='\0') {
      __kernel_set_errno(EINVAL);
      return -1;
   }

   //get current dir for current process
   if(ref[0]!='/' && pid)
      _inodenb = process_lst[__get_syscall_owner_pid()]->inode_curdir;


   if((_desc = _vfs_getdesc(_inodenb,INVALID_DESC))<0)
      return -1;

   token = _vfs_token(ref,sep);
   while( token !=NULL) {

      // "." and ".." on roots
      if(ofile_lst[_desc].inodenb == ofile_lst[_desc].pmntdev->inodenb_offset) {
         if(!strcmp(__dot,token))
            _inodenb = ofile_lst[_desc].inodenb;
         else if(!strcmp(__dotdot,token))
            _inodenb = ofile_lst[_desc].dir_inodenb;
         else
            _inodenb=ofile_lst[_desc].pfsop->fs.lookupdir(_desc,token);
      }else{
         _inodenb=ofile_lst[_desc].pfsop->fs.lookupdir(_desc,token);
      }

      //
      if(_inodenb==INVALID_INODE_NB) {
         *desc=_desc;
         //strcpy(filename,token);
         if(filename)
            *filename=token;
         return -1;
      }

      //strcpy(filename,token);
      if(filename)
         *filename=token;
      token = _vfs_token(NULL,sep);

      __desc=_desc;
      _desc  = _vfs_getdesc(_inodenb,_desc);
      _vfs_putdesc(__desc);
      if(_desc<0)
         return -1;

      ofile_lst[_desc].pfsop->fs.open(_desc);
      if( (ofile_lst[_desc].attr&S_IFDIR)!=S_IFDIR) {
         ofile_lst[_desc].pfsop->fs.close(_desc);
         *desc=_desc;
         return 0;
      }

      ofile_lst[_desc].pfsop->fs.close(_desc);
   }


   *desc=_desc;
   return 0;
}

/*============================================
| End of Source  : vfscore.c
==============================================*/
