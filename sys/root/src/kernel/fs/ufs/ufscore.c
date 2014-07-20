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
#include "kernel/core/errno.h"
#include "kernel/core/kernel.h"
#include "kernel/core/system.h"
#include "kernel/core/systime.h"
#include "kernel/core/malloc.h"
#include "kernel/core/stat.h"


#include "ufscore.h"
#include "ufsdriver.h"

#if defined(__GNUC__)
   #include <stdlib.h>
   #include <string.h>
#endif
/*===========================================
Global Declaration
=============================================*/
#define USE_UFS_SP_SYNC 1

//
const int ufs_node_size          = sizeof(ufs_block_node_t);


/*===========================================
Implementation
=============================================*/

/*-------------------------------------------
| Name:_ufs_fpos
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _ufs_fpos(int ufs_base,int ufs_base_smpl,int* poffset_dbl,int* poffset_smpl,int* poffset,
              int pos)
{
   //
   if(pos/(ufs_base_smpl+ufs_base)) {
      //Double Indirection Needed
      pos=pos-(ufs_base_smpl+ufs_base);
      *poffset_dbl      = pos/ufs_base_smpl;
      *poffset_smpl     = (pos%ufs_base_smpl)/ufs_base;
      *poffset          = (pos%ufs_base_smpl)%ufs_base;
   }else if(pos/(ufs_base)) {
      //Simple Indirection Needed
      pos=pos-ufs_base;
      *poffset_dbl      = -1;
      *poffset_smpl     = pos/ufs_base;
      *poffset          = pos%ufs_base;
   }else{
      //Direct block
      *poffset_dbl      = -1;
      *poffset_smpl     = -1;
      *poffset          = pos;
   }
   return 0;
}

/*-------------------------------------------
| Name:_ufs_allocblk
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
ufs_blocknb_t _ufs_allocblk(mntdev_t* pmntdev)
{
   ufs_blocknb_t blk;
   char* pufs_blkalloc = pmntdev->psuperblk_info->psuperblk;
   int ufs_blkalloc_size = pmntdev->psuperblk_info->alloc_blk_size;

   for(blk=0; blk<ufs_blkalloc_size; blk++) {
      unsigned char b;
      unsigned char byte=pufs_blkalloc[blk];

      if (byte==0xFF)
         continue;

      for(b=0; b<8; b++) {
         char mask=(0x01<<b);

         if( !(byte&mask) ) {
            pufs_blkalloc[blk]=byte|mask;
            //printf("alloc blk=%d b=%d %d\n",blk,b,((blk<<3)+b));
#ifdef USE_UFS_SP_SYNC
            __ufs_sync_sp_blk(pmntdev,pufs_blkalloc,blk);
#endif
            return ( (blk<<3)+b);
         }
      }
   }
   __kernel_set_errno(ENOSPC);

   return INVALID_UFSBLOCK;
}

/*-------------------------------------------
| Name:_ufs_isfreeblk
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _ufs_isfreeblk(mntdev_t* pmntdev,ufs_blocknb_t block)
{
   ufs_blocknb_t blk;
   char offset;
   char mask;
   char* pufs_blkalloc = pmntdev->psuperblk_info->psuperblk;

   if(block==INVALID_UFSBLOCK) return 1;

   blk = block>>3;

   offset=(block-(blk<<3));

   mask=0x01<<offset;

   if( (pufs_blkalloc[blk]&mask) )
      return 0;  //block not free

   return 1; // block free;
}

/*-------------------------------------------
| Name:_ufs_checkfreeblk
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _ufs_checkfreeblk(mntdev_t* pmntdev,char* p_check_ufs_blkalloc,ufs_blocknb_t block)
{
   ufs_blocknb_t blk;
   char offset;
   char mask;
   char* pufs_blkalloc = pmntdev->psuperblk_info->psuperblk;

   if(block==INVALID_UFSBLOCK) return 1;

   blk = block>>3;

   offset=(block-(blk<<3));

   mask=0x01<<offset;

   //temporay bits vector block allocation
   p_check_ufs_blkalloc[blk]|=mask;

   if( (pufs_blkalloc[blk]&mask) )
      return 0;  //block not free

   // block free;
   //fix this block
   pufs_blkalloc[blk]|=mask;

   return -1;
}


/*-------------------------------------------
| Name:_ufs_freeblk
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void _ufs_freeblk(mntdev_t* pmntdev,ufs_blocknb_t block)
{
   char* pufs_blkalloc = pmntdev->psuperblk_info->psuperblk;

   ufs_blocknb_t blk = block>>3;

   char offset=(block-(blk<<3));

   char mask=0x01<<offset;

   //check
   if(block==INVALID_UFSBLOCK)
      return;

   //action
   pufs_blkalloc[blk]=pufs_blkalloc[blk]&(~mask);

#ifdef USE_UFS_SP_SYNC
   __ufs_sync_sp_blk(pmntdev,pufs_blkalloc,blk);
#endif

   //printf("free blk %d\n",block);
}

/*-------------------------------------------
| Name:_ufs_allocinode
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
inodenb_t _ufs_allocnode(mntdev_t* pmntdev)
{

   ufs_inodenb_t i;
   char * pufs_nodealloc = pmntdev->psuperblk_info->psuperblk
                           +pmntdev->psuperblk_info->alloc_blk_size;

   int ufs_nodealloc_size = pmntdev->psuperblk_info->alloc_node_size;

   for(i=0; i<ufs_nodealloc_size; i++) {
      unsigned char b;
      unsigned char byte=pufs_nodealloc[i];

      if (byte==0xFF)
         continue;

      for(b=0; b<8; b++) {
         char mask=(0x01<<b);
         //check limit
         if( (i<<3)+b == (pmntdev->inodetbl_size-1)) //-1 ugly path for bug fix on last node ????
            return INVALID_UFSNODE;

         if( !(byte&mask) ) {
            pufs_nodealloc[i]=byte|mask;
#ifdef USE_UFS_SP_SYNC
            __ufs_sync_sp_node(pmntdev,pufs_nodealloc,i);
#endif
            //convert to logical inode number
            return (inodenb_t)((i<<3)+b)+pmntdev->inodenb_offset;
         }
      }
   }

   __kernel_set_errno(ENOSPC);
   return INVALID_UFSNODE;
}

/*-------------------------------------------
| Name:_ufs_freenode
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _ufs_freenode(mntdev_t* pmntdev,inodenb_t inode)
{

   //convert to physical inode number
   inodenb_t phys_inode=inode-pmntdev->inodenb_offset;
   inodenb_t n = (phys_inode)>>3;
   unsigned char* pufs_nodealloc = pmntdev->psuperblk_info->psuperblk
                                   +pmntdev->psuperblk_info->alloc_blk_size;

   unsigned char offset=(unsigned char)(phys_inode-(n<<3));

   unsigned char mask=(unsigned char)(0x01<<offset);

   pufs_nodealloc[n]=pufs_nodealloc[n]&(~mask);

#ifdef USE_UFS_SP_SYNC
   __ufs_sync_sp_node(pmntdev,pufs_nodealloc,n);
#endif

   return 0;
}


/*-------------------------------------------
| Name:_ufs_statfs
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _ufs_statfs(mntdev_t* pmntdev,struct statvfs *statvfs){

   ufs_blocknb_t blk;
   char* pufs_blkalloc = pmntdev->psuperblk_info->psuperblk;
   int ufs_blkalloc_size = pmntdev->psuperblk_info->alloc_blk_size;
   //
   ufs_block_size_t ufs_block_size=pmntdev->fs_info.ufs_info.block_size;

   memset(statvfs,0,sizeof(struct statvfs));

   statvfs->f_bsize  = ufs_block_size;
   statvfs->f_frsize = ufs_block_size;

   statvfs->f_blocks = pmntdev->psuperblk_info->alloc_blk_size<<3;
   statvfs->f_namemax = UFS_MAX_FILENAME;

   //find free data block
   for(blk=0; blk<ufs_blkalloc_size; blk++) {
      unsigned char b;
      unsigned char byte=pufs_blkalloc[blk];
      if (byte==0xFF)
         continue;
      for(b=0; b<8; b++) {
         char mask=(0x01<<b);
         if( !(byte&mask) ) { //block free!:).
            statvfs->f_bfree++;
         }
      }
   }

   return 0;
}

/*-------------------------------------------
| Name:_ufs_readfs
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _ufs_readfs(mntdev_t* pmntdev)
{
   //mount information
   ufs_block_size_t ufs_block_size;
   long _l_blk_sz;

   //get ufs driver ops from signature
   if((pmntdev->fs_info.ufs_info.p_ufs_driver=_ufs_get_driver(pmntdev->dev_desc))==(void*)0)
      return -1;

   //
   if(__ufs_drvreadfs(pmntdev->fs_info.ufs_info.p_ufs_driver,pmntdev->dev_desc,
                      pmntdev->psuperblk_info)<0)
      return -1;

   pmntdev->inodetbl_size = pmntdev->psuperblk_info->nodeblk_size/
                            pmntdev->fs_info.ufs_info.p_ufs_driver->ufs_blocknode_sz;

/*
const long ufs_max_file_size =   UFS_BLOCK_SIZE
                                 +UFS_BLOCK_SIZE/2*UFS_BLOCK_SIZE
                                 +UFS_BLOCK_SIZE/2*UFS_BLOCK_SIZE/2*UFS_BLOCK_SIZE;
*/

/*
const int ufs_base        = UFS_BLOCK_SIZE;
const int ufs_base_smpl   = (UFS_BLOCK_SIZE/sizeof(ufs_blocknb_t))*UFS_BLOCK_SIZE;
const int ufs_base_dbl    = (UFS_BLOCK_SIZE/sizeof(ufs_blocknb_t))
                           *((UFS_BLOCK_SIZE/sizeof(ufs_blocknb_t))*UFS_BLOCK_SIZE);
*/
   ufs_block_size = pmntdev->psuperblk_info->blk_size;

   pmntdev->fs_info.ufs_info.block_size = ufs_block_size;

   _l_blk_sz = (long)ufs_block_size;

   pmntdev->fs_info.ufs_info.max_file_size = _l_blk_sz
                                             +((_l_blk_sz*_l_blk_sz)>>1) // div by 2
                                             +((_l_blk_sz*_l_blk_sz*_l_blk_sz)>>2); // div by 4

   if(sizeof(int)<4) { //limit file size for 16 bits cpu
      if(pmntdev->fs_info.ufs_info.max_file_size>32765)
         pmntdev->fs_info.ufs_info.max_file_size=32765;
   }

   pmntdev->fs_info.ufs_info.ufs_base = ufs_block_size;
   pmntdev->fs_info.ufs_info.ufs_base_smpl = (ufs_block_size/sizeof(ufs_blocknb_t))*ufs_block_size;

   return 0;
}

/*-------------------------------------------
| Name:_ufs_writefs
| Description:
| Parameters:
| Return Type:
| Comments:0
| See:
---------------------------------------------*/
int _ufs_writefs(mntdev_t* pmntdev){
   __ufs_drvwritefs(pmntdev->fs_info.ufs_info.p_ufs_driver,pmntdev->dev_desc,
                    pmntdev->psuperblk_info);
   return 0;
}

/*-------------------------------------------
| Name:_ufs_makefs
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _ufs_makefs(desc_t dev_desc,struct vfs_formatopt_t* vfs_formatopt){
   ufs_block_node_t blocknode;
   ufs_inodenb_t _inode;
   superblk_t superblk_info;
   char * pufs_nodealloc;
   int cb=0;
   int offset;
   int __max_blk;

   static struct __timeval tv; //common

   ufs_block_size_t ufs_block_size;

   ufs_driver_t* p_ufs_driver=(ufs_driver_t*)0;

   //
   switch(vfs_formatopt->blk_sz) {
   case 16:
   case 32:
   case 64:
   case 128:
   case 256:
   case 512:
   case 1024:
      ufs_block_size = vfs_formatopt->blk_sz;
      break;

   default:
      ufs_block_size = 32;
      break;
   }


   //get ufs driver ops from signature
   if((p_ufs_driver=_ufsdriver_set_driver(UFS_SIGNATURE_DFLT))==(void*)0)
      return -1;

   //
   superblk_info.blk_size          = ufs_block_size;
   superblk_info.datablk_size      = vfs_formatopt->max_blk*ufs_block_size;
   //protection 16bits 32bits compatibility
   if( sizeof(superblk_info.nodeblk_size)<=(sizeof(uint16_t))
       && ((uint32_t)vfs_formatopt->max_node*(uint32_t)p_ufs_driver->ufs_blocknode_sz)>=(0x0000FFFF) )
      superblk_info.nodeblk_size      =
         ((0x0000FFFF)/p_ufs_driver->ufs_blocknode_sz)*p_ufs_driver->ufs_blocknode_sz;
   else
      superblk_info.nodeblk_size      = vfs_formatopt->max_node*p_ufs_driver->ufs_blocknode_sz;
   superblk_info.alloc_blk_size    = vfs_formatopt->max_blk/8+1; //1 bits: free(0|1);
   superblk_info.alloc_node_size   = vfs_formatopt->max_node/8+1;
   superblk_info.superblk_size     = superblk_info.alloc_node_size+superblk_info.alloc_blk_size;

   //to do: get size of sotckage device instead 32*1000 constant. ugly code!!!!
   //__max_blk= ((int)(32*1000)-(superblk_info.nodeblk_size+superblk_info.alloc_node_size))/(1/8+ufs_block_size)-5;
   __max_blk=
      ((vfs_formatopt->dev_sz)-
       (superblk_info.nodeblk_size+superblk_info.alloc_node_size))/(1/8+ufs_block_size)-5;
   if(vfs_formatopt->max_blk>__max_blk)
      vfs_formatopt->max_blk=__max_blk;

   superblk_info.alloc_blk_size    = vfs_formatopt->max_blk/8+1; //1 bits: free(0|1);
   superblk_info.datablk_size      = vfs_formatopt->max_blk*ufs_block_size;
   superblk_info.superblk_size     = superblk_info.alloc_node_size+superblk_info.alloc_blk_size;
   //

   //
   __ufs_drvmakefs(p_ufs_driver,dev_desc,&superblk_info);

   //
   __ufs_drvreadfs(p_ufs_driver,dev_desc,&superblk_info);

   //warning: make root node only when reformat file system
   _inode=0;
   pufs_nodealloc = superblk_info.psuperblk+superblk_info.alloc_blk_size;
   pufs_nodealloc[0]=0x01; //alloc root node;

   _sys_gettimeofday(&tv,0L);
   blocknode.cmtime = tv.tv_sec;

   blocknode.attr = S_IFDIR;
   blocknode.size = 0;
   blocknode.blk[0]     = INVALID_UFSBLOCK;
   blocknode.blk_smpl   = INVALID_UFSBLOCK;
   blocknode.blk_dbl    = INVALID_UFSBLOCK;

   //write node
   offset= superblk_info.nodeblk_addr+_inode*p_ufs_driver->ufs_blocknode_sz;
   ofile_lst[dev_desc].pfsop->fdev.fdev_seek(dev_desc,offset,SEEK_SET);
   while(cb<p_ufs_driver->ufs_blocknode_sz) {
      cb+=ofile_lst[dev_desc].pfsop->fdev.fdev_write(dev_desc,(char*)&blocknode+cb,
                                                     p_ufs_driver->ufs_blocknode_sz-cb);
      if(cb<0)
         return -1;
   }

   __ufs_drvwritefs(p_ufs_driver,dev_desc,&superblk_info);

   return 0;
}

/*-------------------------------------------
| Name:_ufs_checkfs
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _ufs_checkfs(mntdev_t* pmntdev){

   //device
   desc_t dev_desc = pmntdev->dev_desc;
   int error=0;

   //node list read index
   ufs_inodenb_t i;
   //block list read index
   //ufs_blocknb_t blk;
   //
   int ufs_blkalloc_size = pmntdev->psuperblk_info->alloc_blk_size;
   char * pufs_nodealloc = pmntdev->psuperblk_info->psuperblk+ufs_blkalloc_size;

   int ufs_nodealloc_size = pmntdev->psuperblk_info->alloc_node_size;

   char* pufs_blkalloc = pmntdev->psuperblk_info->psuperblk;

   //
   int ufs_base = pmntdev->fs_info.ufs_info.ufs_base;
   int ufs_base_smpl = pmntdev->fs_info.ufs_info.ufs_base_smpl;

   //
   ufs_block_size_t ufs_block_size=pmntdev->fs_info.ufs_info.block_size;



   //temporay bits vector block allocation
   //retrieve and fix unused block
   char* p_check_ufs_blkalloc = _sys_malloc(ufs_blkalloc_size);
   memset(p_check_ufs_blkalloc,0,ufs_blkalloc_size);


   for(i=0; i<ufs_nodealloc_size; i++) {
      unsigned char b;
      unsigned char byte=pufs_nodealloc[i];

      if (byte==0x00)
         continue;

      for(b=0; b<8; b++) {
         ufs_inodenb_t inode=( (i<<3)+b);
         ufs_block_node_t blocknode;
         int cb=0;
         int offset=0;

         char mask=(0x01<<b);

         //is a used node
         if( !(byte&mask) )
            continue;

         //go to check!!!
         //check node and block
         //pufs_nodealloc[i]=byte|mask;

         //read node
         offset=pmntdev->psuperblk_info->nodeblk_addr+inode*ufs_node_size;

         ofile_lst[dev_desc].pfsop->fdev.fdev_seek(dev_desc,offset,SEEK_SET);

         while(cb<ufs_node_size) {
            cb+=ofile_lst[dev_desc].pfsop->fdev.fdev_read(dev_desc,(char*)&blocknode+cb,
                                                          ufs_node_size-cb);
            if(cb<0)
               return -1;
         }

         //read file
         {
            static ufs_block_byte_t blkdata;
            static char buf[sizeof(ufs_block_dir_t)];
            ufs_block_dir_t*    dir=(ufs_block_dir_t*)buf;
            //
            ufs_inodenb_t curblk_no = INVALID_UFSBLOCK;
            //
            ufs_block_indirect_t blkind;

            //
            int offset_dbl;
            int offset_smpl;
            int offset;

            int _offset_dbl;
            int _offset_smpl;
            int _offset;

            int r=0;
            int _r=0;

            /*if(!blocknode.size)
               return 0;*/

            while( r<=blocknode.size ) {

               //For test: If new indirection and block allocation requested
               _offset_dbl    = offset_dbl;
               _offset_smpl   = offset_smpl;
               _offset        = offset;

               //check node consistency
               if((blocknode.attr&S_IFDIR) && r && !(r%sizeof(ufs_block_dir_t))) {
                  char o =(dir->inode>>3);
                  char b =dir->inode - (o<<3);
                  char mask=0x01<<b;
                  char byte=pufs_nodealloc[o];

                  if(!(byte&mask)) {
                     error=-1;
                     pufs_nodealloc[o]=byte|mask;
                     //printf("->error : ino:%d name:%s fixed\r\n",dir->inode,dir->name);
                  }else{
                     //printf("->ino:%d name:%s ok\r\n",dir->inode,dir->name);
                  }
                  _r=0;
               }

               if( r>=blocknode.size)
                  break;

               _ufs_fpos(ufs_base,ufs_base_smpl,&offset_dbl,&offset_smpl,&offset,r++);


               //
               if(offset_dbl!=-1) {

                  if(!r || offset_dbl != _offset_dbl) {
                     //double indirection
                     if(_ufs_checkfreeblk(pmntdev,p_check_ufs_blkalloc,blocknode.blk_dbl)) {
                        //printf("error: on inode no:%d block:%d\r\n",inode,blocknode.blk_dbl);
                        error=-1;
                     }
                     __ufs_raw_readblk(pmntdev,(char*)&blkind, blocknode.blk_dbl,ufs_block_size);
                     //simple indirection
                     if(_ufs_checkfreeblk(pmntdev,p_check_ufs_blkalloc,blkind.blk[offset_dbl])) {
                        //printf("error: on inode no:%d block:%d\r\n",inode,blkind.blk[offset_dbl]);
                        error=-1;
                     }
                     __ufs_raw_readblk(pmntdev,(char*)&blkind, blkind.blk[offset_dbl],
                                       ufs_block_size);
                     //set new current data block
                     curblk_no = blkind.blk[offset_smpl];
                     if(_ufs_checkfreeblk(pmntdev,p_check_ufs_blkalloc,curblk_no)) {
                        //printf("error: on inode no:%d block:%d\r\n",inode,curblk_no);
                        error=-1;
                     }
                     //load current data block
                     if(blocknode.attr&S_IFDIR)
                        __ufs_raw_readblk(pmntdev,(char*)&blkdata, curblk_no,ufs_block_size);


                  }else if(offset_smpl != _offset_smpl) {
                     //current simple indirection
                     curblk_no = blkind.blk[offset_smpl];
                     //load current data block
                     if(_ufs_checkfreeblk(pmntdev,p_check_ufs_blkalloc,curblk_no)) {
                        //printf("error on inode no:%d block:%d\r\n",inode,curblk_no);
                        error=-1;
                     }
                     if(blocknode.attr&S_IFDIR)
                        __ufs_raw_readblk(pmntdev,(char*)&blkdata, curblk_no,ufs_block_size);
                  }

                  if(blocknode.attr&S_IFDIR)
                     buf[_r++]=blkdata.byte[offset];

               } else if (offset_smpl!=-1) {

                  if(!r || offset_smpl != _offset_smpl) {
                     //simple indirection
                     if(_ufs_checkfreeblk(pmntdev,p_check_ufs_blkalloc,blocknode.blk_smpl)) {
                        //printf("error on inode no:%d block:%d\r\n",inode,blocknode.blk_smpl);
                        error=-1;
                     }
                     __ufs_raw_readblk(pmntdev,(char*)&blkind, blocknode.blk_smpl,ufs_block_size);
                     //set new current data block
                     curblk_no =  blkind.blk[offset_smpl];
                     if(_ufs_checkfreeblk(pmntdev,p_check_ufs_blkalloc,curblk_no)) {
                        //printf("error on inode no:%d block:%d\r\n",inode,curblk_no);
                        error=-1;
                     }
                     //load current data block
                     if(blocknode.attr&S_IFDIR)
                        __ufs_raw_readblk(pmntdev,(char*)&blkdata,curblk_no,ufs_block_size);
                  }
                  if(blocknode.attr&S_IFDIR)
                     buf[_r++]=blkdata.byte[offset];
               }else{
                  if(r==1) {
                     //set new current data block
                     curblk_no = blocknode.blk[0];
                     if(_ufs_checkfreeblk(pmntdev,p_check_ufs_blkalloc,curblk_no)) {
                        //printf("error on inode no:%d block:%d\r\n",inode,curblk_no);
                        error=-1;
                     }
                     //load current data block
                     if(blocknode.attr&S_IFDIR)
                        __ufs_raw_readblk(pmntdev,(char*)&blkdata, curblk_no,ufs_block_size);
                  }
                  if(blocknode.attr&S_IFDIR)
                     buf[_r++]=blkdata.byte[offset];
               }
            } //end while
         } //end read file

      } //end for(b=...
   } //end for(i=...


   //retrieve and fix unused block
   /* not yet ok
   for(blk=0;blk<ufs_blkalloc_size;blk++){
      unsigned char b;
      unsigned char byte=pufs_blkalloc[blk];
      unsigned char byte_check=p_check_ufs_blkalloc[blk];

      if (byte==byte_check)
         continue;

      for(b=0;b<8;b++){
         char mask=(0x01<<b);

         if( (byte&mask)!=(byte_check&mask) )
            printf("error on blk %d...\r\n",((blk<<3)+b));

         if( (byte&mask) && !(byte_check&mask) ){
            error=-1;
            //unused block must be deallocated
            pufs_blkalloc[blk]=byte&(~mask);
            printf("error on blk %d. unused block fixed\r\n",((blk<<3)+b));

         }
      }
   }
  */
   //
   if(error<0) {
      //printf("file system fixed\r\n");
      _ufs_writefs(pmntdev);
      _ufs_readfs(pmntdev);
   }

   _sys_free(p_check_ufs_blkalloc);

   return error;
}

/*===========================================
End of Source OS_FileSystemCore.c
=============================================*/
