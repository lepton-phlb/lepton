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

#if defined(__KERNEL_SUPPORT_UFS_DRIVER_1_5)

/*===========================================
Global Declaration
=============================================*/



/*===========================================
Implementation
=============================================*/


/*-------------------------------------------
| Name:_ufs_drv_1_5_creatfs
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _ufs_drv_1_5_drvmakefs(desc_t desc,superblk_t* superblk)
{
   int cb=0;
   int _offset=0;
   
   ofile_lst[desc].pfsop->fdev.fdev_seek( desc, 0L, SEEK_SET );
   //UFS signature instead magic number
   if(( cb = ofile_lst[desc].pfsop->fdev.fdev_write(desc,
                                                   UFS_SIGNATURE_1_5,
                                                   strlen(UFS_SIGNATURE)))<0)
      return -1;

   //block size
   if(( cb = ofile_lst[desc].pfsop->fdev.fdev_write(desc,
                                                   (char*)&superblk->blk_size,
                                                   sizeof(superblk->blk_size)))<0)
      return -1;

   //
   if(( cb = ofile_lst[desc].pfsop->fdev.fdev_write(desc,
                                                   (char*)&superblk->superblk_size,
                                                   sizeof(superblk->superblk_size)))<0)
      return -1;

   if(( cb = ofile_lst[desc].pfsop->fdev.fdev_write(desc,
                                                   (char*)&superblk->alloc_blk_size,
                                                   sizeof(superblk->alloc_blk_size)))<0)
      return -1;

   if(( cb = ofile_lst[desc].pfsop->fdev.fdev_write(desc,
                                                   (char*)&superblk->alloc_node_size,
                                                   sizeof(superblk->alloc_node_size)))<0)
      return -1;
   
   if(( cb = ofile_lst[desc].pfsop->fdev.fdev_write(desc,
                                                   (char*)&superblk->nodeblk_size,
                                                   sizeof(superblk->nodeblk_size)))<0 )
      return -1;

   if(( cb = ofile_lst[desc].pfsop->fdev.fdev_write(desc,
                                                   (char*)&superblk->datablk_size,
                                                   sizeof(superblk->datablk_size)))<0)
      return -1;
   
   //dynamic allocation:alloc
   superblk->psuperblk         = malloc(superblk->superblk_size);
   
   _offset = ofile_lst[desc].pfsop->fdev.fdev_seek(desc,0L,SEEK_CUR);

   ofile_lst[desc].pfsop->fdev.fdev_seek(desc,0L,SEEK_SET);
   
   superblk->superblk_addr   = _offset;
   superblk->nodeblk_addr    = superblk->superblk_addr+superblk->superblk_size;
   superblk->datablk_addr    = superblk->nodeblk_addr+superblk->nodeblk_size;

   ofile_lst[desc].pfsop->fdev.fdev_seek(desc,superblk->superblk_addr,SEEK_SET);
   
   memset(superblk->psuperblk,0,superblk->superblk_size);
   cb=0;
   while(cb<(int)superblk->superblk_size){
      cb+=ofile_lst[desc].pfsop->fdev.fdev_write(desc,superblk->psuperblk+cb,superblk->superblk_size-cb);
      if(cb<0){
         //dynamic allocation:free
         free(superblk->psuperblk);
         return -1;
      }
   }

   //dynamic allocation:free
   free(superblk->psuperblk);
   
   //ofile_lst[desc].pfsop->fdev.fdev_seek(desc,_superblk_addr,SEEK_SET);

   return 0;
}

/*-------------------------------------------
| Name:_ufs_drv_1_5_readfs
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _ufs_drv_1_5_drvreadfs(desc_t desc, superblk_t* superblk)
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
   if(!strcmp(buffer,UFS_SIGNATURE_1_2)){
      //force block size
      superblk->blk_size = 32;
   }else if(!strcmp(buffer,UFS_SIGNATURE_1_5)){
      //read block size
      superblk->blk_size = 32;
      //
      if(( cb = ofile_lst[desc].pfsop->fdev.fdev_read(desc,
                                                   (char*)&superblk->blk_size,
                                                   sizeof(superblk->blk_size)))<0)
      return -1;
      //
      if(superblk->blk_size>UFS_BLOCK_SIZE_MAX)
         return -1;// cannot allocate block, block size too big :( .

   }else{
      return -1;
   }

   
   //
   if(( cb = ofile_lst[desc].pfsop->fdev.fdev_read(desc,
                                                   (char*)&superblk->superblk_size,
                                                   sizeof(superblk->superblk_size)))<0)
      return -1;

   if(( cb = ofile_lst[desc].pfsop->fdev.fdev_read(desc,
                                                   (char*)&superblk->alloc_blk_size,
                                                   sizeof(superblk->alloc_blk_size)))<0)
      return -1;

   if(( cb = ofile_lst[desc].pfsop->fdev.fdev_read(desc,
                                                   (char*)&superblk->alloc_node_size,
                                                   sizeof(superblk->alloc_node_size)))<0)
      return -1;
   
   if(( cb = ofile_lst[desc].pfsop->fdev.fdev_read(desc,
                                                   (char*)&superblk->nodeblk_size,
                                                   sizeof(superblk->nodeblk_size)))<0 )
      return -1;

   if(( cb = ofile_lst[desc].pfsop->fdev.fdev_read(desc,
                                                   (char*)&superblk->datablk_size,
                                                   sizeof(superblk->datablk_size)))<0)
      return -1;
  

   //dynamic allocation:alloc
   superblk->psuperblk         = malloc(superblk->superblk_size);

   //
   _offset = ofile_lst[desc].pfsop->fdev.fdev_seek(desc,0L,SEEK_CUR);

   ofile_lst[desc].pfsop->fdev.fdev_seek(desc,0L,SEEK_SET);
   
   superblk->superblk_addr   = _offset;
   superblk->nodeblk_addr    = superblk->superblk_addr+superblk->superblk_size;
   superblk->datablk_addr    = superblk->nodeblk_addr+superblk->nodeblk_size;

   ofile_lst[desc].pfsop->fdev.fdev_seek(desc,superblk->superblk_addr,SEEK_SET);
   
   memset(superblk->psuperblk,0,superblk->superblk_size);
   cb=0;
   while(cb<(int)superblk->superblk_size){
      cb+=ofile_lst[desc].pfsop->fdev.fdev_read(desc,superblk->psuperblk+cb,superblk->superblk_size-cb);
      if(cb<0){
        //dynamic allocation:free
         free(superblk->psuperblk);
         return -1;
      }
   }



   return 0;
}

/*-------------------------------------------
| Name:_ufs_drv_1_5_writefs
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _ufs_drv_1_5_drvwritefs(desc_t desc,superblk_t* superblk)
{
   int cb=0;
   char update=1;//default case: update.
 
   //dynamic allocation:alloc
   char* psuperblk         = malloc(superblk->superblk_size);

    ofile_lst[desc].pfsop->fdev.fdev_seek(desc,superblk->superblk_addr,SEEK_SET);

   //compare super block
   if(psuperblk){
      memset(psuperblk,0,superblk->superblk_size);

      //read device superblock
      while(cb<(int)superblk->superblk_size){
         cb+=ofile_lst[desc].pfsop->fdev.fdev_read(desc,psuperblk+cb,superblk->superblk_size-cb);
         if(cb<0){
           //dynamic allocation:free
            free(psuperblk);
            break;
         }
      }

      //compare current superblock with device superblock
      if(!memcmp(superblk->psuperblk,psuperblk,superblk->superblk_size))
         update = 0;//current superblock = device superblock: no update required.

      free(psuperblk);
   }

   if(!update){
      free(superblk->psuperblk);
      //_dbg_printf("update superblock not required\n");
      return 0;
   }

   
   //
   ofile_lst[desc].pfsop->fdev.fdev_seek(desc,superblk->superblk_addr,SEEK_SET);
   //write superblock
   cb=0;
   while(cb<(int)superblk->superblk_size){
      cb+=ofile_lst[desc].pfsop->fdev.fdev_write(desc,superblk->psuperblk+cb,superblk->superblk_size-cb);
      if(cb<0){
         //dynamic allocation:free
         free(superblk->psuperblk);
         return -1;
      }
   }

   //dynamic allocation:free
   free(superblk->psuperblk);

   //_dbg_printf("update superblock required\n");

   return 0;
}

#endif //#if __KERNEL_SUPPORT_UFS_DRIVER==__KERNEL_SUPPORT_UFS_DRIVER_1_5

/*============================================
| End of Source  : ufsdriver_1_5.c
==============================================*/
