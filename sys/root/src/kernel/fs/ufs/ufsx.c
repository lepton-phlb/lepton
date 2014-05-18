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


/**
 * \addtogroup lepton_vfs
 * @{
 */

/**
 * \addtogroup ufs les fonctions de base de l'ufs
 * @{
 *
 * Ce système de fichiers est utilisé sur les supports comme la eeprom série ou la flash interne du m16c.
 * Il est donc associé aux périphériques par la fonction ou la commande shell mount.
 *
 */

/**
 * \defgroup ufsx_c les fonctions de base de l'ufs
 * @{
 *
 * les opérations de bases supportées par l'ufsx :
 *
 */

/**
 * \file
 * implementation du système de fichiers ufsx
 * \author philippe le boulanger
 */

/*===========================================
Includes
=============================================*/
#include <stdlib.h>
#include <string.h>

#include "kernel/core/errno.h"
#include "kernel/core/kernel.h"
#include "kernel/core/system.h"
#include "kernel/core/systime.h"
#include "kernel/core/stat.h"
#include "kernel/fs/vfs/vfstypes.h"

#include "ufsinfo.h"
#include "ufsdriver.h"
#include "ufscore.h"
#include "ufs.h"
#include "ufsx.h"





#ifdef _TRACE_DEBUG
   #define _START_TRACE(s) \
   printf("START "); printf(s); printf("\n"); \
   resetMemoryAccessCounter();

   #define _END_TRACE(s){ \
      printf("END "); printf(s); traceMemoryAccessCounter(); printf("\n"); } \

#else
   #define _START_TRACE(s)
   #define _END_TRACE(s)
#endif


/*===========================================
Global Declaration
=============================================*/

static const int ufs_addr_size   = sizeof(ufs_blocknb_t);
//


//
static const char _ufsstr[]=".";
static const char __ufsstr[]="..";

//
fsop_t ufsx_op={
   _ufs_loadfs,
   _ufs_checkfs,
   _ufs_makefs,
   _ufs_readfs,
   _ufs_writefs,
   _ufsx_statfs,
   _ufsx_mountdir,
   _ufsx_readdir,
   _ufsx_telldir,
   _ufsx_seekdir,
   _ufsx_lookupdir,
   _ufs_mknod,
   _ufsx_create,
   _ufs_open,    //open
   _ufs_close,   //close
   _ufs_read,    //read
   _ufs_write,   //write
   _ufs_seek,    //seek
   _ufs_truncate,
   _ufsx_remove,
   _ufsx_rename
};


/*===========================================
Implementation
=============================================*/


/*-------------------------------------------
| Name:_ufsx_mountdir
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _ufsx_mountdir(desc_t desc,inodenb_t original_root_node,inodenb_t target_root_node){
   int r;
   ufs_block_dir_t dir;
   _ufs_seek(desc,0,SEEK_SET);

   // to do: use readdir function.
   while( (r=_ufs_read(desc,(char*)&dir,sizeof(ufs_block_dir_t)))>0) {
      if(original_root_node == __cvt2logicnode(desc,dir.inode)) {
         int offset = sizeof(ufs_block_dir_t);
         _ufs_seek(desc,-1*offset,SEEK_CUR);
         dir.inode = (ufs_inodenb_t)__cvt2physnode(desc,target_root_node);
         _ufs_write(desc,(char*)&dir,sizeof(ufs_block_dir_t));
         return 0;
      }
   }

   return -1;
}

/*-------------------------------------------
| Name:_ufsx_readdir
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _ufsx_readdir(desc_t desc,dirent_t* dirent){

   ufs_block_dir_t dir;
   int r;

   r  = _ufs_read(desc,(char*)&dir,sizeof(ufs_block_dir_t));
   if(r<=0)
      return -1;

   strcpy(dirent->d_name,dir.name);
   dirent->inodenb = __cvt2logicnode(desc,dir.inode);

   return 0;
}

/*-------------------------------------------
| Name:_ufsx_telldir
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _ufsx_telldir(desc_t desc){
   int loc;
   loc = _ufs_seek(desc,0,SEEK_CUR)/sizeof(ufs_block_dir_t);
   return loc;
}

/*-------------------------------------------
| Name:_ufsx_seekdir
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _ufsx_seekdir(desc_t desc,int loc){

   int pos = loc * sizeof(ufs_block_dir_t);
   if(_ufs_seek(desc,pos,SEEK_SET)<0)
      return -1;

   return loc;
}

/*-------------------------------------------
| Name:_ufsx_lookupdir
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
#if defined(CPU_ARM7) || defined(CPU_WIN32)
static char direntry[__KERNEL_UFS_BLOCK_SIZE_MAX];

inodenb_t _ufsx_lookupdir(desc_t desc,char* filename){
   ufs_inodenb_t _inode=INVALID_INODE_NB;
   int r;
   int s = sizeof(ufs_block_dir_t);
   ufs_block_dir_t * pdir = NULL;

   //
   _ufs_open(desc);

   //read a max of entry in a block
   while((r = _ufs_read(desc, (void*)direntry, __KERNEL_UFS_BLOCK_SIZE_MAX))) {
      r=r/s-1;
      pdir = (ufs_block_dir_t *)(direntry+r*s);
      while(r>=0) {
         if(!strcmp(pdir->name,filename)) {
            _inode=(ufs_inodenb_t)__cvt2logicnode(desc,pdir->inode);
            //break;
            _ufs_close(desc);
            return _inode;
         }
         pdir = (ufs_block_dir_t *)(direntry+(--r)*s);
      }
   }
   //
   _ufs_close(desc);
   return _inode;
}

#else
inodenb_t _ufsx_lookupdir(desc_t desc,char* filename){
   ufs_inodenb_t _inode=INVALID_INODE_NB;
   ufs_block_dir_t dir;
   int r;

   //
   _ufs_open(desc);
   while( (r=_ufs_read(desc,(char*)&dir,sizeof(ufs_block_dir_t))) )
   {
      //Match token and one entry in catalog
      if(strcmp(dir.name,filename))
         continue;

      _inode=(ufs_inodenb_t)__cvt2logicnode(desc,dir.inode);
      break;

   }
   _ufs_close(desc);

   return _inode;
}
#endif

/*-------------------------------------------
| Name:_ufsx_create
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
inodenb_t _ufsx_create(desc_t desc,char* filename, int attr){

   ufs_block_node_t blocknode;
   ufs_inodenb_t _dir_inode;
   inodenb_t _inode;
   ufs_block_dir_t dir;

   static struct __timeval tv; //common
   static struct statvfs __statvfs;

   if(strlen(filename)>UFS_MAX_FILENAME) {
      __kernel_set_errno(ENAMETOOLONG);
      return -1; //file name is too long
   }

   _dir_inode = (ufs_inodenb_t)ofile_lst[desc].inodenb;

   _ufs_statfs(ofile_lst[desc].pmntdev,&__statvfs);
   if(__statvfs.f_bfree<1) {
      //safe: not enough available space.
      __kernel_set_errno(ENOSPC);
      return -1;
   }

   _inode=_ufs_allocnode(ofile_lst[desc].pmntdev);
   if(_inode==INVALID_UFSNODE)
      return -1;

   //set creation date
   _sys_gettimeofday(&tv,0L);
   blocknode.cmtime = tv.tv_sec;
   //
   blocknode.attr = attr;
   blocknode.size = 0;
   blocknode.blk[0]     = INVALID_UFSBLOCK;
   blocknode.blk_smpl   = INVALID_UFSBLOCK;
   blocknode.blk_dbl    = INVALID_UFSBLOCK;
   __ufs_writenode(desc,(char*)&blocknode,_inode,ufs_node_size);

   //Add entry;
   _ufs_open(desc);
   dir.inode=(ufs_inodenb_t)__cvt2physnode(desc,_inode);
   strcpy(dir.name,filename);
   _ufs_seek(desc,0,SEEK_END);
   _ufs_write(desc,(char*)&dir,sizeof(ufs_block_dir_t));
   _ufs_close(desc);

   if((attr&S_IFDIR)!=S_IFDIR)
      return _inode;

   //Add in new directory "." an ".."
   ofile_lst[desc].inodenb=_inode;
   _ufs_open(desc);
   _ufs_seek(desc,0,SEEK_END);

   //"."
   dir.inode=(ufs_inodenb_t)__cvt2physnode(desc,_inode);
   strcpy(dir.name,_ufsstr);
   _ufs_write(desc,(char*)&dir,sizeof(ufs_block_dir_t));

   //".."
   dir.inode=(ufs_inodenb_t)__cvt2physnode(desc,_dir_inode);
   strcpy(dir.name,__ufsstr);
   _ufs_write(desc,(char*)&dir,sizeof(ufs_block_dir_t));

   _ufs_close(desc);

   return _inode;
}

/*-------------------------------------------
| Name:_ufsx_remove
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _ufsx_remove(desc_t desc_ancst,desc_t desc){

   //remove regular file
   ufs_block_dir_t dir;
   int r;
   int pos;
   int offset;

   //
   _ufs_open(desc_ancst);
   while( (r=_ufs_read(desc_ancst,(char*)&dir,sizeof(ufs_block_dir_t))) )
   {
      if(ofile_lst[desc].inodenb == __cvt2logicnode(desc_ancst,dir.inode))
         break;
   }

   if(!r)
      return -1;

   offset=-1*(int)(sizeof(ufs_block_dir_t));

   pos = _ufs_seek(desc_ancst,offset, SEEK_CUR);

   _ufs_seek(desc_ancst, offset, SEEK_END);

   _ufs_read(desc_ancst,(char*)&dir,sizeof(ufs_block_dir_t));

   _ufs_seek(desc_ancst, pos, SEEK_SET);

   _ufs_write(desc_ancst,(char*)&dir,sizeof(ufs_block_dir_t));

   //free all block of desc;
   _ufs_truncate(desc,0);
   //free inode of desc
   _ufs_freenode(ofile_lst[desc].pmntdev,ofile_lst[desc].inodenb);

   //truncate directory desc_ancst
   _ufs_truncate(desc_ancst,(off_t)(ofile_lst[desc_ancst].size-sizeof(ufs_block_dir_t)));


   _ufs_close(desc_ancst);


   return 0;
}

/*-------------------------------------------
| Name:_ufsx_rename
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _ufsx_rename(desc_t desc,const char*  old_name, char* new_name){

   //remove regular file
   ufs_block_dir_t dir;
   int r;
   int pos;
   int offset;

   //
   while( (r=_ufs_read(desc,(char*)&dir,sizeof(ufs_block_dir_t))) ) {
      if(!strcmp(dir.name,old_name))
         break;
   }

   if(!r)
      return -1;
   offset=-1*(int)(sizeof(ufs_block_dir_t));

   //rewind on the entry matched
   pos = _ufs_seek(desc,offset, SEEK_CUR);

   //change the entry name
   strcpy(dir.name,new_name);

   //write the new entry name
   _ufs_write(desc,(char*)&dir,sizeof(ufs_block_dir_t));

   return 0;
}

/*--------------------------------------------
| Name:        _ufsx_statfs
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int _ufsx_statfs(mntdev_t* pmntdev,struct statvfs *statvfs){
   //overload ufs statfs function in ufscore.c: modify UFS_MAX_FILENAME
   if(_ufs_statfs(pmntdev,statvfs)<0)
      return -1;

   statvfs->f_namemax = UFS_MAX_FILENAME;

   return 0;
}
/*-------------------------------------------
| Name:_ufsx_loadfs
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _ufsx_loadfs(void){
   return 0;
}

/** @} */
/** @} */
/** @} */

/*============================================
| End of Source  : ufsx.c
==============================================*/
