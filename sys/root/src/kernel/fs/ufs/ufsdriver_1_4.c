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

#include "kernel/core/system.h"
#include "kernel/core/malloc.h"

#include "kernel/fs/vfs/vfstypes.h"
#include "kernel/fs/vfs/vfskernel.h"
#include "kernel/fs/vfs/vfs.h"

#include "kernel/fs/ufs/ufscore.h"
#include "kernel/fs/ufs/ufsdriver.h"

#if defined(__KERNEL_SUPPORT_UFS_DRIVER_1_4)

/*===========================================
Global Declaration
=============================================*/

ufs_block_node_1_4_t ufs_block_node_1_4;
typedef struct {
   uint16_t superblk_size;
   uint16_t alloc_blk_size;
   uint16_t alloc_node_size;

   uint16_t nodeblk_size;
   uint16_t datablk_size;

   uint16_t blk_size;

   char* psuperblk;

   uint32_t superblk_addr;
   uint32_t nodeblk_addr;
   uint32_t datablk_addr;
}superblk_16_32_bits_t;

static const superblk_16_32_bits_t superblk_16_32_bits;

/*===========================================
Implementation
=============================================*/


/*-------------------------------------------
| Name:_ufs_drv_1_4_creatfs
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _ufs_drv_1_4_drvmakefs(desc_t desc,superblk_t* superblk)
{
   int cb=0;
   int _offset=0;

   ofile_lst[desc].pfsop->fdev.fdev_seek( desc, 0L, SEEK_SET );
   //UFS signature instead magic number
   if(( cb = ofile_lst[desc].pfsop->fdev.fdev_write(desc,
                                                    UFS_SIGNATURE_1_4,
                                                    strlen(UFS_SIGNATURE)))<0)
      return -1;

   //block size
   if(( cb = ofile_lst[desc].pfsop->fdev.fdev_write(desc,
                                                    (char*)&superblk->blk_size,
                                                    sizeof(superblk_16_32_bits.blk_size)))<0)
      return -1;

   //
   if(( cb = ofile_lst[desc].pfsop->fdev.fdev_write(desc,
                                                    (char*)&superblk->superblk_size,
                                                    sizeof(superblk_16_32_bits.superblk_size)))<0)
      return -1;

   if(( cb = ofile_lst[desc].pfsop->fdev.fdev_write(desc,
                                                    (char*)&superblk->alloc_blk_size,
                                                    sizeof(superblk_16_32_bits.alloc_blk_size)))<0)
      return -1;

   if(( cb = ofile_lst[desc].pfsop->fdev.fdev_write(desc,
                                                    (char*)&superblk->alloc_node_size,
                                                    sizeof(superblk_16_32_bits.alloc_node_size)))<0)
      return -1;

   if(( cb = ofile_lst[desc].pfsop->fdev.fdev_write(desc,
                                                    (char*)&superblk->nodeblk_size,
                                                    sizeof(superblk_16_32_bits.nodeblk_size)))<0 )
      return -1;

   if(( cb = ofile_lst[desc].pfsop->fdev.fdev_write(desc,
                                                    (char*)&superblk->datablk_size,
                                                    sizeof(superblk_16_32_bits.datablk_size)))<0)
      return -1;

   //dynamic allocation:alloc
   superblk->psuperblk         = _sys_malloc(superblk->superblk_size);

   _offset = ofile_lst[desc].pfsop->fdev.fdev_seek(desc,0L,SEEK_CUR);

   ofile_lst[desc].pfsop->fdev.fdev_seek(desc,0L,SEEK_SET);

   superblk->superblk_addr   = _offset;
   superblk->nodeblk_addr    = superblk->superblk_addr+superblk->superblk_size;
   superblk->datablk_addr    = superblk->nodeblk_addr+superblk->nodeblk_size;

   ofile_lst[desc].pfsop->fdev.fdev_seek(desc,superblk->superblk_addr,SEEK_SET);

   memset(superblk->psuperblk,0,superblk->superblk_size);
   cb=0;
   while(cb<(int)superblk->superblk_size) {
      cb+=ofile_lst[desc].pfsop->fdev.fdev_write(desc,superblk->psuperblk+cb,
                                                 superblk->superblk_size-cb);
      if(cb<0) {
         //dynamic allocation:free
         _sys_free(superblk->psuperblk);
         return -1;
      }
   }

   //dynamic allocation:free
   _sys_free(superblk->psuperblk);

   //ofile_lst[desc].pfsop->fdev.fdev_seek(desc,_superblk_addr,SEEK_SET);

   return 0;
}

/*-------------------------------------------
| Name:_ufs_drv_1_4_readfs
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _ufs_drv_1_4_drvreadfs(desc_t desc, superblk_t* superblk)
{
   int cb=0;
   int _offset=0;

   char buffer[16]={0};

   ofile_lst[desc].pfsop->fdev.fdev_seek( desc, 0L, SEEK_SET );
   //UFS signature instead magic number
   if(( cb = ofile_lst[desc].pfsop->fdev.fdev_read(desc,
                                                   buffer,
                                                   strlen(UFS_SIGNATURE)))<0)
      return -1;

   //check ufs signature and read block size
   if(!strcmp(buffer,UFS_SIGNATURE_1_2)) {
      //force block size
      superblk->blk_size = 32;
   }else if(!strcmp(buffer,UFS_SIGNATURE_1_4)) {
      //read block size
      superblk->blk_size = 32;
      //
      if(( cb = ofile_lst[desc].pfsop->fdev.fdev_read(desc,
                                                      (char*)&superblk->blk_size,
                                                      sizeof(superblk_16_32_bits.blk_size)))<0)
         return -1;
      //
      if(superblk->blk_size>UFS_BLOCK_SIZE_MAX)
         return -1;  // cannot allocate block, block size too big :( .

   }else{
      return -1;
   }


   //
   if(( cb = ofile_lst[desc].pfsop->fdev.fdev_read(desc,
                                                   (char*)&superblk->superblk_size,
                                                   sizeof(superblk_16_32_bits.superblk_size)))<0)
      return -1;

   if(( cb = ofile_lst[desc].pfsop->fdev.fdev_read(desc,
                                                   (char*)&superblk->alloc_blk_size,
                                                   sizeof(superblk_16_32_bits.alloc_blk_size)))<0)
      return -1;

   if(( cb = ofile_lst[desc].pfsop->fdev.fdev_read(desc,
                                                   (char*)&superblk->alloc_node_size,
                                                   sizeof(superblk_16_32_bits.alloc_node_size)))<0)
      return -1;

   if(( cb = ofile_lst[desc].pfsop->fdev.fdev_read(desc,
                                                   (char*)&superblk->nodeblk_size,
                                                   sizeof(superblk_16_32_bits.nodeblk_size)))<0 )
      return -1;

   if(( cb = ofile_lst[desc].pfsop->fdev.fdev_read(desc,
                                                   (char*)&superblk->datablk_size,
                                                   sizeof(superblk_16_32_bits.datablk_size)))<0)
      return -1;


   //dynamic allocation:alloc
   superblk->psuperblk         = _sys_malloc(superblk->superblk_size);

   //
   _offset = ofile_lst[desc].pfsop->fdev.fdev_seek(desc,0L,SEEK_CUR);

   ofile_lst[desc].pfsop->fdev.fdev_seek(desc,0L,SEEK_SET);

   superblk->superblk_addr   = _offset;
   superblk->nodeblk_addr    = superblk->superblk_addr+superblk->superblk_size;
   superblk->datablk_addr    = superblk->nodeblk_addr+superblk->nodeblk_size;

   ofile_lst[desc].pfsop->fdev.fdev_seek(desc,superblk->superblk_addr,SEEK_SET);

   memset(superblk->psuperblk,0,superblk->superblk_size);
   cb=0;
   while(cb<(int)superblk->superblk_size) {
      cb+=ofile_lst[desc].pfsop->fdev.fdev_read(desc,superblk->psuperblk+cb,
                                                superblk->superblk_size-cb);
      if(cb<0) {
         //dynamic allocation:free
         _sys_free(superblk->psuperblk);
         return -1;
      }
   }



   return 0;
}

/*-------------------------------------------
| Name:_ufs_drv_1_4_writefs
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _ufs_drv_1_4_drvwritefs(desc_t desc,superblk_t* superblk)
{
   int cb=0;
   char update=1; //default case: update.

   //dynamic allocation:alloc
   char* psuperblk         = _sys_malloc(superblk->superblk_size);

   ofile_lst[desc].pfsop->fdev.fdev_seek(desc,superblk->superblk_addr,SEEK_SET);

   //compare super block
   if(psuperblk) {
      memset(psuperblk,0,superblk->superblk_size);

      //read device superblock
      while(cb<(int)superblk->superblk_size) {
         cb+=ofile_lst[desc].pfsop->fdev.fdev_read(desc,psuperblk+cb,superblk->superblk_size-cb);
         if(cb<0) {
            //dynamic allocation:free
            _sys_free(psuperblk);
            break;
         }
      }

      //compare current superblock with device superblock
      if(!memcmp(superblk->psuperblk,psuperblk,superblk->superblk_size))
         update = 0;  //current superblock = device superblock: no update required.

      _sys_free(psuperblk);
   }

   if(!update) {
      _sys_free(superblk->psuperblk);
      //_dbg_printf("update superblock not required\n");
      return 0;
   }


   //
   ofile_lst[desc].pfsop->fdev.fdev_seek(desc,superblk->superblk_addr,SEEK_SET);
   //write superblock
   cb=0;
   while(cb<(int)superblk->superblk_size) {
      cb+=ofile_lst[desc].pfsop->fdev.fdev_write(desc,superblk->psuperblk+cb,
                                                 superblk->superblk_size-cb);
      if(cb<0) {
         //dynamic allocation:free
         _sys_free(superblk->psuperblk);
         return -1;
      }
   }

   //dynamic allocation:free
   _sys_free(superblk->psuperblk);

   //_dbg_printf("update superblock required\n");

   return 0;
}

/*-------------------------------------------
| Name:_ufs_drv_1_4_readnode
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _ufs_drv_1_4_readnode(desc_t desc,char* pufs_blknode,int node,int size)
{
   int cb=0;
   int offset;
   desc_t dev_desc = __get_dev_desc(desc);

   offset=ofile_lst[desc].pmntdev->psuperblk_info->nodeblk_addr
           +(ufs_inodenb_t)__cvt2physnode(desc,node)*size;

   ofile_lst[dev_desc].pfsop->fdev.fdev_seek(dev_desc,offset,SEEK_SET);

   while(cb<size) {
      cb+=ofile_lst[dev_desc].pfsop->fdev.fdev_read(dev_desc,pufs_blknode+cb,size-cb);
      if(cb<0)
         return -1;
   }

   return cb;
}

/*-------------------------------------------
| Name:_ufs_drv_1_4_writenode
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _ufs_drv_1_4_writenode(desc_t desc,char* pufs_blknode,int node,int size)
{
   int cb=0;
   int offset;
   desc_t dev_desc = __get_dev_desc(desc);

   offset= ofile_lst[desc].pmntdev->psuperblk_info->nodeblk_addr
           +(ufs_inodenb_t)__cvt2physnode(desc,node)*size;

   ofile_lst[dev_desc].pfsop->fdev.fdev_seek(dev_desc,offset,SEEK_SET);

   while(cb<size) {
      cb+=ofile_lst[dev_desc].pfsop->fdev.fdev_write(dev_desc,pufs_blknode+cb,size-cb);
      if(cb<0)
         return -1;
   }

   return cb;
}

/*-------------------------------------------
| Name:_ufs_drv_1_4_readblk
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _ufs_drv_1_4_readblk(desc_t desc,char* pufs_blk,int block,int size)
{
   int cb=0;
   int offset;
   desc_t dev_desc = __get_dev_desc(desc);

   offset=ofile_lst[desc].pmntdev->psuperblk_info->datablk_addr+block*size;

   ofile_lst[dev_desc].pfsop->fdev.fdev_seek(dev_desc,offset,SEEK_SET);

   while(cb<size) {
      cb+=ofile_lst[dev_desc].pfsop->fdev.fdev_read(dev_desc,pufs_blk+cb,size-cb);
      if(cb<0)
         return -1;
   }

   return cb;
}

/*-------------------------------------------
| Name:writeDataBlock
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _ufs_drv_1_4_writeblk(desc_t desc,char* pufs_blk,int block,int size)
{
   int cb=0;
   int offset;
   desc_t dev_desc = __get_dev_desc(desc);

   offset=ofile_lst[desc].pmntdev->psuperblk_info->datablk_addr+block*size;

   ofile_lst[dev_desc].pfsop->fdev.fdev_seek(dev_desc,offset,SEEK_SET);

   //to remove test
   //printf("_ufs_drv_1_4_writeblk size=%d\n",size);

   while(cb<size) {
      //to remove test
      //PD2|=(0x01<<6);
      //P2.6=1;
      cb+=ofile_lst[dev_desc].pfsop->fdev.fdev_write(dev_desc,pufs_blk+cb,size-cb);
      //P2.6=0;
      if(cb<0)
         return -1;
   }

   return cb;
}

/*-------------------------------------------
| Name:_ufs_drv_1_4_raw_readblk
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _ufs_drv_1_4_raw_readblk(mntdev_t* pmntdev,char* pufs_blk,int block,int size)
{
   int cb=0;
   int offset;

   desc_t dev_desc = pmntdev->dev_desc;

   offset=pmntdev->psuperblk_info->datablk_addr+block*size;

   ofile_lst[dev_desc].pfsop->fdev.fdev_seek(dev_desc,offset,SEEK_SET);

   while(cb<size) {
      cb+=ofile_lst[dev_desc].pfsop->fdev.fdev_read(dev_desc,pufs_blk+cb,size-cb);
      if(cb<0)
         return -1;
   }

   return cb;
}

/*--------------------------------------------
| Name:        _ufs_drv_1_4_drv_1_4_sync_sp_block
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int _ufs_drv_1_4_sync_sp_blk(mntdev_t* pmntdev,char* pufs_blkalloc,int block_byte){
   desc_t desc = pmntdev->dev_desc;
   //
   ofile_lst[desc].pfsop->fdev.fdev_seek( desc,
                                          pmntdev->psuperblk_info->superblk_addr+block_byte,
                                          SEEK_SET);
   //write superblock
   return ofile_lst[desc].pfsop->fdev.fdev_write(  desc,
                                                   pufs_blkalloc+block_byte,
                                                   1);
}

/*--------------------------------------------
| Name:        _ufs_drv_1_4_sync_sp_node
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int _ufs_drv_1_4_sync_sp_node(mntdev_t* pmntdev,char * pufs_nodealloc,int node_byte){
   desc_t desc = pmntdev->dev_desc;
   //
   ofile_lst[desc].pfsop->fdev.fdev_seek(
      desc,
      pmntdev->psuperblk_info->superblk_addr+
      pmntdev->psuperblk_info->alloc_blk_size+node_byte,
      SEEK_SET);
   //write superblock
   return ofile_lst[desc].pfsop->fdev.fdev_write(desc,pufs_nodealloc+node_byte,1);
}

#endif //#if __KERNEL_SUPPORT_UFS_DRIVER==__KERNEL_SUPPORT_UFS_DRIVER_1_4

/*============================================
| End of Source  : ufsdriver_1_4.c
==============================================*/
