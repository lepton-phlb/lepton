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


#ifdef _TRACE_DEBUG
#define _START_TRACE(s)\
        printf("START ");printf(s);printf("\n");\
        resetMemoryAccessCounter();

#define _END_TRACE(s){\
   printf("END ");printf(s);traceMemoryAccessCounter();printf("\n");}\

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
fsop_t ufs_op={
   _ufs_loadfs,
   _ufs_checkfs,
   _ufs_makefs,
   _ufs_readfs,
   _ufs_writefs,
   _ufs_statfs,
   _ufs_mountdir,
   _ufs_readdir,
   _ufs_telldir,
   _ufs_seekdir,
   _ufs_lookupdir,
   _ufs_mknod,
   _ufs_create,
   _ufs_open,    //open
   _ufs_close,   //close
   _ufs_read,    //read
   _ufs_write,   //write
   _ufs_seek,    //seek
   _ufs_truncate,
   _ufs_remove,
   _ufs_rename
};


/*===========================================
Implementation
=============================================*/


/*-------------------------------------------
| Name:_ufs_mountdir
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _ufs_mountdir(desc_t desc,inodenb_t original_root_node,inodenb_t target_root_node){
   int r;
   ufs_block_dir_t      dir;
   _ufs_seek(desc,0,SEEK_SET);

   // to do: use readdir function.
   while( (r=_ufs_read(desc,(char*)&dir,sizeof(ufs_block_dir_t)))>0){
      if(original_root_node == __cvt2logicnode(desc,dir.inode)){
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
| Name:_ufs_readdir
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _ufs_readdir(desc_t desc,dirent_t* dirent){

   ufs_block_dir_t      dir;
   int r;

   r  = _ufs_read(desc,(char*)&dir,sizeof(ufs_block_dir_t));
   if(r<=0)
      return -1;

   strcpy(dirent->d_name,dir.name);
   dirent->inodenb = __cvt2logicnode(desc,dir.inode);

   return 0;
}

/*-------------------------------------------
| Name:_ufs_telldir
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _ufs_telldir(desc_t desc){
   int loc;
   loc = _ufs_seek(desc,0,SEEK_CUR)/sizeof(ufs_block_dir_t);
   return loc;
}

/*-------------------------------------------
| Name:_ufs_seekdir
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _ufs_seekdir(desc_t desc,int loc){

   int pos = loc * sizeof(ufs_block_dir_t);
   if(_ufs_seek(desc,pos,SEEK_SET)<0)
      return -1;

   return loc;
}

/*-------------------------------------------
| Name:_ufs_lookupdir
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
#if defined(CPU_ARM7) || defined(CPU_WIN32)
static char direntry[__KERNEL_UFS_BLOCK_SIZE_MAX];

inodenb_t _ufs_lookupdir(desc_t desc,char* filename){
   ufs_inodenb_t          _inode=INVALID_INODE_NB;
   int r;
   int s = sizeof(ufs_block_dir_t);
   //
   ufs_block_dir_t * pdir = NULL;
   
   //
   _ufs_open(desc);
   //read a max of entry in a block
   
   while((r = _ufs_read(desc, (void*)direntry, __KERNEL_UFS_BLOCK_SIZE_MAX))) {
      r=r/s-1;
      pdir = (ufs_block_dir_t *)(direntry+r*s);
      while(r>=0){
         if(!strcmp(pdir->name,filename)) {
            _inode=(ufs_inodenb_t)__cvt2logicnode(desc,pdir->inode);
            //break;
            _ufs_close(desc);
            return _inode;
         }

         pdir = (ufs_block_dir_t *)(direntry+(--r)*s);//r--;
      }
   }

   _ufs_close(desc);

   return _inode;
}

#else
inodenb_t _ufs_lookupdir(desc_t desc,char* filename){
   ufs_inodenb_t          _inode=INVALID_INODE_NB;
   ufs_block_dir_t         dir;
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
| Name:_rtfs_mknod
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _ufs_mknod(desc_t desc,inodenb_t inodenb,dev_t dev){

   ufs_block_node_t    blocknode;

   __ufs_readnode(desc,(char*)&blocknode,inodenb,ufs_node_size);

   blocknode.blk[0] = dev;

   __ufs_writenode(desc,(char*)&blocknode,inodenb,ufs_node_size);

   return 0;
}

/*-------------------------------------------
| Name:_ufs_create
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
inodenb_t _ufs_create(desc_t desc,char* filename, int attr){

   ufs_block_node_t    blocknode;
   ufs_inodenb_t       _dir_inode;
   inodenb_t           _inode;
   ufs_block_dir_t    dir;

   static struct __timeval tv;//common
   static struct statvfs __statvfs;

   if(strlen(filename)>UFS_MAX_FILENAME){
      __kernel_set_errno(ENAMETOOLONG); 
      return -1; //file name is too long
   }

   _dir_inode = (ufs_inodenb_t)ofile_lst[desc].inodenb;

   _ufs_statfs(ofile_lst[desc].pmntdev,&__statvfs);
   if(__statvfs.f_bfree<1){
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
| Name:_ufs_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _ufs_open(desc_t desc)
{
   
   ufs_block_node_t    blocknode;
   ufs_inodenb_t  _inode   = (ufs_inodenb_t)ofile_lst[desc].inodenb;
   
   __ufs_readnode(desc,(char*)&blocknode,_inode,ufs_node_size);
   
   ofile_lst[desc].attr     = blocknode.attr;
   ofile_lst[desc].size     = blocknode.size;
   ofile_lst[desc].offset   = 0;//-1; //Open at begin of file //bug fix.
   ofile_lst[desc].cmtime   = blocknode.cmtime;

    if((ofile_lst[desc].attr & (S_IFCHR | S_IFBLK )) ){
      ofile_lst[desc].ext.dev = blocknode.blk[0];
   }else if(ofile_lst[desc].attr & S_IFIFO){
      ofile_lst[desc].ext.pipe_desc = blocknode.blk[0];
   }
   
   return 0;
}

/*-------------------------------------------
| Name:_ufs_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _ufs_close(desc_t desc){

   ufs_block_node_t    blocknode;
   ufs_inodenb_t  _inode   = (ufs_inodenb_t)ofile_lst[desc].inodenb;

   __ufs_readnode(desc,(char*)&blocknode,_inode,ufs_node_size);

   if(IS_FSTATUS_MODIFIED(ofile_lst[desc].status)){
      static struct __timeval tv;//common
      //set modification date
      _sys_gettimeofday(&tv,0L);
      blocknode.cmtime = tv.tv_sec;
      __ufs_writenode(desc,(char*)&blocknode,_inode,ufs_node_size);
   }

   blocknode.size=ofile_lst[desc].size;
   if((ofile_lst[desc].attr & (S_IFCHR | S_IFBLK )) ){
      if(blocknode.blk[0] != ofile_lst[desc].ext.dev){
         blocknode.blk[0] = ofile_lst[desc].ext.dev;
         __ufs_writenode(desc,(char*)&blocknode,_inode,ufs_node_size);
      }
   }else if(ofile_lst[desc].attr & S_IFIFO){
      if(blocknode.blk[0] != ofile_lst[desc].ext.pipe_desc){
         blocknode.blk[0] = ofile_lst[desc].ext.pipe_desc;
         __ufs_writenode(desc,(char*)&blocknode,_inode,ufs_node_size);
      }
   }

   ofile_lst[desc].offset  =  0;
   ofile_lst[desc].attr    =  S_IFNULL;
   ofile_lst[desc].size    =  0;

   return 0;
}


/*-------------------------------------------
| Name:_ufs_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _ufs_write(desc_t desc, char* buf, int size)
{
   ufs_block_node_t  blocknode;

   //warning: use static, implicit cache.
   //no simultaneous access in this code part (kernel).
   static ufs_block_byte_t  blkdata;
   //
   ufs_inodenb_t curblk_no = INVALID_UFSBLOCK;

   //
   ufs_block_size_t ufs_block_size=ofile_lst[desc].pmntdev->fs_info.ufs_info.block_size;
   long ufs_max_file_size=ofile_lst[desc].pmntdev->fs_info.ufs_info.max_file_size;

   int ufs_base = ofile_lst[desc].pmntdev->fs_info.ufs_info.ufs_base;
   int ufs_base_smpl = ofile_lst[desc].pmntdev->fs_info.ufs_info.ufs_base_smpl;

   //
   int offset_dbl;
   int offset_smpl;
   int offset;

   int _offset_dbl;
   int _offset_smpl;
   int _offset;

   int w=-1;
   
   ufs_inodenb_t inode=(ufs_inodenb_t)ofile_lst[desc].inodenb;

   _ufs_fpos(ufs_base,ufs_base_smpl,&offset_dbl,&offset_smpl,&offset,ofile_lst[desc].offset-1 );

   __ufs_readnode(desc,(char*)&blocknode,inode,ufs_node_size);

   while(++w<size){

      //For test: If new indirection and block allocation requested 
      _offset_dbl    = offset_dbl;
      _offset_smpl   = offset_smpl;
      _offset        = offset;

      if(ofile_lst[desc].offset>=(ufs_max_file_size-1))
         break;

      _ufs_fpos(ufs_base,ufs_base_smpl,&offset_dbl,&offset_smpl,&offset,ofile_lst[desc].offset++);

      if(offset_dbl!=-1){
         //BlockNode double indirection
         static ufs_block_indirect_t   blkind;
         static ufs_block_indirect_t   blkind2;
         static ufs_inodenb_t            nodedbl;

         //Check if double indirection is requested
         if(ofile_lst[desc].offset>blocknode.size && blocknode.blk_dbl==INVALID_UFSBLOCK){
            blocknode.blk_dbl =_ufs_allocblk(ofile_lst[desc].pmntdev);
            if( blocknode.blk_dbl==INVALID_UFSBLOCK){//error
                ofile_lst[desc].offset--;
                break;
            }
         }

         //load double indirection block speed optimization
         if(!w ||_offset_dbl!=offset_dbl)
            __ufs_readblk(desc,(char*)&blkind2, blocknode.blk_dbl,ufs_block_size);

         //Check if new indirection requested
         if( ofile_lst[desc].offset>blocknode.size && _offset_dbl!=offset_dbl ){
             blkind2.blk[offset_dbl] = _ufs_allocblk(ofile_lst[desc].pmntdev);
             if(blkind2.blk[offset_dbl]==INVALID_UFSBLOCK){//error
                ofile_lst[desc].offset--;
                break;
             }
            //to do: bug fix
            if( (offset_dbl+1)%(ufs_block_size/sizeof(ufs_blocknb_t)) )
               blkind2.blk[offset_dbl+1]=INVALID_UFSBLOCK;

            __ufs_writeblk(desc,(char*)&blkind2, blocknode.blk_dbl,ufs_block_size);
         }

         
         //load simple indirection block speed optimization
         if(!w || (_offset_dbl!=offset_dbl) || (_offset_smpl!=offset_smpl) ){
            nodedbl=blkind2.blk[offset_dbl];
            __ufs_readblk(desc,(char*)&blkind, nodedbl,ufs_block_size);
         }

         if( ofile_lst[desc].offset>blocknode.size && _offset_smpl!=offset_smpl ){
             blkind.blk[offset_smpl] =_ufs_allocblk(ofile_lst[desc].pmntdev);
             if( blkind.blk[offset_smpl]==INVALID_UFSBLOCK){//error
                ofile_lst[desc].offset--;
                break;
             }
            //to do: bug fix
            if( (offset_smpl+1)%(ufs_block_size/sizeof(ufs_blocknb_t)) )
               blkind.blk[offset_smpl+1]=INVALID_UFSBLOCK;

             blocknode.size=(int)ofile_lst[desc].offset;
             __ufs_writeblk(desc,(char*)&blkind, nodedbl,ufs_block_size);
         }

         //write current block
         if(w && _offset_smpl!=offset_smpl ){
             //write current data block
            __ufs_writeblk(desc,(char*)&blkdata, curblk_no,ufs_block_size);
            //set new current data block
            curblk_no=blkind.blk[offset_smpl];
            //load current data block
            __ufs_readblk(desc,(char*)&blkdata,curblk_no ,ufs_block_size);
         }

         //first byte
         if(!w){
            //set new current data block
            curblk_no=blkind.blk[offset_smpl];
            //load current data block
            __ufs_readblk(desc,(char*)&blkdata,curblk_no ,ufs_block_size);
         }

         //write byte data
         blkdata.byte[offset]=buf[w];
      }else if (offset_smpl!=-1){
         //BlockNode simple indirection
         static ufs_block_indirect_t   blkind;

         //Check if simple indirection is requested
         if(ofile_lst[desc].offset>blocknode.size && blocknode.blk_smpl==INVALID_UFSBLOCK ){
            blocknode.blk_smpl = _ufs_allocblk(ofile_lst[desc].pmntdev);
            if( blocknode.blk_smpl == INVALID_UFSBLOCK){//error
                ofile_lst[desc].offset--;
                break;
            }
         }

         //load simple indirection block speed optimization
         if(!w || _offset_smpl!=offset_smpl)
            __ufs_readblk(desc,(char*)&blkind, blocknode.blk_smpl,ufs_block_size);

         //Check if new indirection requested
         if( ofile_lst[desc].offset>blocknode.size && _offset_smpl!=offset_smpl ){
            blkind.blk[offset_smpl] =_ufs_allocblk(ofile_lst[desc].pmntdev);
            if( blkind.blk[offset_smpl] == INVALID_UFSBLOCK){//error
               ofile_lst[desc].offset--;
               break;
            }
            //to do: bug fix
            if( (offset_smpl+1)%(ufs_block_size/sizeof(ufs_blocknb_t)) )
               blkind.blk[offset_smpl+1]=INVALID_UFSBLOCK;
            
            blocknode.size=(int)ofile_lst[desc].offset;
            __ufs_writeblk(desc,(char*)&blkind, blocknode.blk_smpl,ufs_block_size);

         }

         //write current block
         if(w && _offset_smpl!=offset_smpl ){
            //write current data block
            __ufs_writeblk(desc,(char*)&blkdata, curblk_no,ufs_block_size);
            //set new current data block
            curblk_no = blkind.blk[offset_smpl];
            //load current data block
            __ufs_readblk(desc,(char*)&blkdata,curblk_no ,ufs_block_size);
         }

         //first byte
         if(!w){
            //set new current data block
            curblk_no = blkind.blk[offset_smpl];
            //load current data block
            __ufs_readblk(desc,(char*)&blkdata,curblk_no ,ufs_block_size);
         }
         
         //write byte data
         blkdata.byte[offset]=buf[w];
      }else{

         if( (ofile_lst[desc].offset>blocknode.size) && !(blocknode.size%ufs_block_size) ){
            blocknode.blk[0] =_ufs_allocblk(ofile_lst[desc].pmntdev);
            if( blocknode.blk[0]  == INVALID_UFSBLOCK){//error
                ofile_lst[desc].offset--;
                break;
            }

            //write current data block
            __ufs_writeblk(desc,(char*)&blkdata, curblk_no,ufs_block_size);
            //set new current data block
            curblk_no = blocknode.blk[0];
            //load current data block
            __ufs_readblk(desc,(char*)&blkdata,curblk_no ,ufs_block_size);
         }

         //first byte
         if(!w){
            //set new current data block
            curblk_no = blocknode.blk[0];
            //load current data block
            __ufs_readblk(desc,(char*)&blkdata,curblk_no ,ufs_block_size);
         }
       
         //write byte data
         blkdata.byte[offset]=buf[w];
      }

      if(ofile_lst[desc].offset>blocknode.size)
         blocknode.size=(int)ofile_lst[desc].offset;
   }

   //write current data block
   if(curblk_no!=INVALID_UFSBLOCK){
      __ufs_writeblk(desc,(char*)&blkdata, curblk_no,ufs_block_size);
   }

   if(ofile_lst[desc].offset>ofile_lst[desc].size){
      blocknode.size=(int)ofile_lst[desc].offset;
      ofile_lst[desc].size=blocknode.size;
      __ufs_writenode(desc,(char*)&blocknode,inode,ufs_node_size);
   }

   //set modified status
   ofile_lst[desc].status|=MSK_FSTATUS_MODIFIED;

   return w;
}

/*-------------------------------------------
| Name:_ufs_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _ufs_read(desc_t desc, char* buf, int size){
  
   //warning: use static, implicit cache.
   //no simultaneous access in this code part (kernel).
   static ufs_block_byte_t  blkdata;
   //
   ufs_block_size_t ufs_block_size=ofile_lst[desc].pmntdev->fs_info.ufs_info.block_size;
   //
   int ufs_base = ofile_lst[desc].pmntdev->fs_info.ufs_info.ufs_base;
   int ufs_base_smpl = ofile_lst[desc].pmntdev->fs_info.ufs_info.ufs_base_smpl;
   //
   ufs_inodenb_t curblk_no = INVALID_UFSBLOCK;
   //
   ufs_block_indirect_t blkind;

   //
   int offset_dbl=-1;
   int offset_smpl=-1;
   int offset=-1;

   int _offset_dbl;
   int _offset_smpl;
   int _offset;

   int r=-1;

   ufs_block_node_t  blocknode;

   ufs_inodenb_t inode=(ufs_inodenb_t)ofile_lst[desc].inodenb;;

   __ufs_readnode(desc,(char*)&blocknode,inode,ufs_node_size);
   if(!blocknode.size)
      return 0;

   while( ++r<size && ofile_lst[desc].offset<blocknode.size ){

      //For test: If new indirection and block allocation requested 
      _offset_dbl    = offset_dbl;
      _offset_smpl   = offset_smpl;
      _offset        = offset;

      _ufs_fpos(ufs_base,ufs_base_smpl,&offset_dbl,&offset_smpl,&offset,ofile_lst[desc].offset++);

      //
      if(offset_dbl!=-1){

         if(!r || offset_dbl != _offset_dbl){
            //double indirection
            __ufs_readblk(desc,(char*)&blkind, blocknode.blk_dbl,ufs_block_size);
            //simple indirection
            __ufs_readblk(desc,(char*)&blkind, blkind.blk[offset_dbl],ufs_block_size);
            //set new current data block
            curblk_no = blkind.blk[offset_smpl];
            //load current data block
            __ufs_readblk(desc,(char*)&blkdata, curblk_no ,ufs_block_size);
         }else if(offset_smpl != _offset_smpl){
            curblk_no = blkind.blk[offset_smpl];
            //load current data block
            __ufs_readblk(desc,(char*)&blkdata, curblk_no ,ufs_block_size);
         }

         buf[r]=blkdata.byte[offset];

      } else if (offset_smpl!=-1){

         if(!r || offset_smpl != _offset_smpl){
            //simple indirection
            __ufs_readblk(desc,(char*)&blkind, blocknode.blk_smpl,ufs_block_size);
            //set new current data block
            curblk_no =  blkind.blk[offset_smpl];
            //load current data block
            __ufs_readblk(desc,(char*)&blkdata,curblk_no,ufs_block_size);
         }
         buf[r]=blkdata.byte[offset];
      }else{
         if(!r){
            //set new current data block
            curblk_no = blocknode.blk[0];
            //load current data block
            __ufs_readblk(desc,(char*)&blkdata, curblk_no,ufs_block_size);
         }
         buf[r]=blkdata.byte[offset];
      }
   }

   return r;
}

/*-------------------------------------------
| Name:_ufs_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _ufs_seek(desc_t desc, int offset, int origin)
{
   ufs_block_node_t  blocknode;
   ufs_inodenb_t        _inode=(ufs_inodenb_t)ofile_lst[desc].inodenb;

   __ufs_readnode(desc,(char*)&blocknode,_inode,ufs_node_size);

   switch(origin)
   {
      case SEEK_SET:
         //Begin of the File
         ofile_lst[desc].offset=offset;
      break;

      case SEEK_CUR:
         //Current position of the file
         ofile_lst[desc].offset=ofile_lst[desc].offset+offset;
      break;

      case SEEK_END:
         //End of the File
         ofile_lst[desc].offset = blocknode.size+offset;
      break;

      default:
      return -1;
   }

   return ofile_lst[desc].offset;
}

/*-------------------------------------------
| Name:_ufs_truncate
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _ufs_truncate(desc_t desc, off_t length)
{
   ufs_block_node_t  blocknode;
   static ufs_block_indirect_t   blkind;

   int offset_dbl;
   int offset_smpl;
   int offset;

   int _offset_dbl;
   int _offset_smpl;
   int _offset;

   int pos=-1;

   
   ufs_inodenb_t inode = (ufs_inodenb_t)ofile_lst[desc].inodenb;
   //
   ufs_block_size_t ufs_block_size=ofile_lst[desc].pmntdev->fs_info.ufs_info.block_size;
   //
   int ufs_base = ofile_lst[desc].pmntdev->fs_info.ufs_info.ufs_base;
   int ufs_base_smpl = ofile_lst[desc].pmntdev->fs_info.ufs_info.ufs_base_smpl;
   
   __ufs_readnode(desc,(char*)&blocknode,inode,ufs_node_size);
   if(!blocknode.size)
      return 0;

   pos   =  blocknode.size;
   // node -> offset_dbl -> offset_smpl -> offset
   _ufs_fpos(ufs_base,ufs_base_smpl,&offset_dbl,&offset_smpl,&offset,pos);

   __ufs_readnode(desc,(char*)&blocknode,inode,ufs_node_size);

   //printf("truncate length=%d\n",length);
   while(--pos>=length){
      //printf("pos=%d\n",pos);

      //For test: If indirection and block delete;
      _offset_dbl    = offset_dbl;
      _offset_smpl   = offset_smpl;
      _offset        = offset;

      _ufs_fpos(ufs_base,ufs_base_smpl,&offset_dbl,&offset_smpl,&offset,pos);

      if(offset_dbl!=-1){
         //dbl indirection requested
         static ufs_block_indirect_t   blkind_dbl;

         if( _offset_dbl!=offset_dbl ){
            //load first indirection block
            if(blocknode.blk_dbl==INVALID_UFSBLOCK)
               continue;
            __ufs_readblk(desc,(char*)&blkind_dbl, blocknode.blk_dbl,ufs_block_size);

            if(blkind_dbl.blk[_offset_dbl]==INVALID_UFSBLOCK)
               continue;
            _ufs_freeblk(ofile_lst[desc].pmntdev,blkind_dbl.blk[_offset_dbl]);
         }

         //
         if(_offset_smpl!=offset_smpl ){
            //load first indirection block
            if(blocknode.blk_dbl==INVALID_UFSBLOCK)
               continue;
            __ufs_readblk(desc,(char*)&blkind_dbl, blocknode.blk_dbl,ufs_block_size);
            if( blkind_dbl.blk[_offset_dbl]==INVALID_UFSBLOCK)
               continue;
            __ufs_readblk(desc,(char*)&blkind, blkind_dbl.blk[_offset_dbl],ufs_block_size);
            _ufs_freeblk(ofile_lst[desc].pmntdev,blkind.blk[_offset_smpl]);
         }

      } else if (offset_dbl==-1 && offset_smpl!=-1){
         //indirection requested
         if( _offset_dbl!=offset_dbl ){
            //load first indirection block
            if(blocknode.blk_dbl==INVALID_UFSBLOCK)
               continue;
            __ufs_readblk(desc,(char*)&blkind, blocknode.blk_dbl,ufs_block_size);
            if(blkind.blk[_offset_dbl]==INVALID_UFSBLOCK)
               continue;
            _ufs_freeblk(ofile_lst[desc].pmntdev,blkind.blk[_offset_dbl]);

            if(blkind.blk[_offset_smpl]==INVALID_UFSBLOCK)
               continue;
            __ufs_readblk(desc,(char*)&blkind, blkind.blk[_offset_smpl],ufs_block_size);
            _ufs_freeblk(ofile_lst[desc].pmntdev,blkind.blk[_offset_smpl]);

             //double indirection is requested
            if(blocknode.blk_dbl!=INVALID_UFSBLOCK){
               _ufs_freeblk(ofile_lst[desc].pmntdev,blocknode.blk_dbl);
               blocknode.blk_dbl=INVALID_UFSBLOCK;
            }
         }
         
         if(_offset_smpl!=offset_smpl ){
            //load simple indirection block
            if( blocknode.blk_smpl==INVALID_UFSBLOCK)
               continue;
            __ufs_readblk(desc,(char*)&blkind, blocknode.blk_smpl,ufs_block_size);

            if(blkind.blk[_offset_smpl]==INVALID_UFSBLOCK)
               continue;
            _ufs_freeblk(ofile_lst[desc].pmntdev,blkind.blk[_offset_smpl]);
         }

      }else if( offset_smpl==-1 && pos>=0){
         if(_offset_smpl!=offset_smpl ){
            if( blocknode.blk_smpl==INVALID_UFSBLOCK)
               continue;
            __ufs_readblk(desc,(char*)&blkind, blocknode.blk_smpl,ufs_block_size);

            if(blkind.blk[_offset_smpl]==INVALID_UFSBLOCK)
               continue;
            _ufs_freeblk(ofile_lst[desc].pmntdev,blkind.blk[_offset_smpl]);
             //simple indirection is requested
            if(blocknode.blk_smpl!=INVALID_UFSBLOCK ){
               _ufs_freeblk(ofile_lst[desc].pmntdev,blocknode.blk_smpl);
               blocknode.blk_smpl=INVALID_UFSBLOCK;
            }
         }

         if(!pos){
            if(blocknode.blk[0]==INVALID_UFSBLOCK)
               continue;

            _ufs_freeblk(ofile_lst[desc].pmntdev,blocknode.blk[0]);
            blocknode.blk[0]=INVALID_UFSBLOCK;
         }
      }
   }

   //set inode.size
   blocknode.size=pos+1;
   ofile_lst[desc].size   = blocknode.size;
   ofile_lst[desc].offset = blocknode.size;
   __ufs_writenode(desc,(char*)&blocknode,inode,ufs_node_size);
   
   return 0;
}

/*-------------------------------------------
| Name:_ufs_remove
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _ufs_remove(desc_t desc_ancst,desc_t desc){

   //remove regular file
   ufs_block_dir_t   dir;
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

   pos = _ufs_seek(desc_ancst,offset , SEEK_CUR);

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
| Name:_ufs_rename
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _ufs_rename(desc_t desc,const char*  old_name, char* new_name){

   //remove regular file
   ufs_block_dir_t   dir;
   int r;
   int pos;
   int offset;
   
   //
   while( (r=_ufs_read(desc,(char*)&dir,sizeof(ufs_block_dir_t))) ){
      if(!strcmp(dir.name,old_name))
         break;
   }

   if(!r)
      return -1;
   offset=-1*(int)(sizeof(ufs_block_dir_t));

   //rewind on the entry matched
   pos = _ufs_seek(desc,offset , SEEK_CUR);

   //change the entry name
   strcpy(dir.name,new_name);

   //write the new entry name
   _ufs_write(desc,(char*)&dir,sizeof(ufs_block_dir_t));

   return 0;
}

/*-------------------------------------------
| Name:_ufs_loadfs
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _ufs_loadfs(void){
   return 0;
}

/*===========================================
End of Source OS_FileSystemRoutine.c
=============================================*/
