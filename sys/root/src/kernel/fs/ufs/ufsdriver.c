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
#include <stdlib.h>
#include <string.h>
#include "kernel/core/system.h"
#include "kernel/fs/vfs/vfstypes.h"
#include "kernel/fs/vfs/vfskernel.h"
#include "kernel/fs/vfs/vfs.h"

#include "kernel/fs/ufs/ufsdriver.h"
#include "kernel/fs/ufs/ufscore.h"


enum {
#if defined(__KERNEL_SUPPORT_UFS_DRIVER_1_3)
   UFS_DRIVER_1_3,
#endif
#if defined(__KERNEL_SUPPORT_UFS_DRIVER_1_4)
   UFS_DRIVER_1_4,
#endif
#if defined(__KERNEL_SUPPORT_UFS_DRIVER_1_5)
   UFS_DRIVER_1_5,
#endif
   UFS_DRIVER_X_X
} ufs_driver_id_t;



/*===========================================
Global Declaration
=============================================*/

ufs_driver_t ufs_driver_list[]={
#if defined(__KERNEL_SUPPORT_UFS_DRIVER_1_3)
   {
      UFS_SIGNATURE_1_3,
      sizeof(ufs_block_node_1_3_t),
      &ufs_block_node_1_3,
      _ufs_drv_1_3_drvmakefs,
      _ufs_drv_1_3_drvreadfs,
      _ufs_drv_1_3_drvwritefs,
      _ufs_drv_1_3_readnode,
      _ufs_drv_1_3_writenode,
      _ufs_drv_1_3_readblk,
      _ufs_drv_1_3_writeblk,
      _ufs_drv_1_3_raw_readblk,
      _ufs_drv_1_3_sync_sp_blk,
      _ufs_drv_1_3_sync_sp_node
   },
#endif
#if defined(__KERNEL_SUPPORT_UFS_DRIVER_1_4)
   {
      UFS_SIGNATURE_1_4,
      sizeof(ufs_block_node_1_4_t),
      &ufs_block_node_1_4,
      _ufs_drv_1_4_drvmakefs,
      _ufs_drv_1_4_drvreadfs,
      _ufs_drv_1_4_drvwritefs,
      _ufs_drv_1_4_readnode,
      _ufs_drv_1_4_writenode,
      _ufs_drv_1_4_readblk,
      _ufs_drv_1_4_writeblk,
      _ufs_drv_1_4_raw_readblk,
      _ufs_drv_1_4_sync_sp_blk,
      _ufs_drv_1_4_sync_sp_node
   },
#endif
#if defined(__KERNEL_SUPPORT_UFS_DRIVER_1_5)
   {
      UFS_SIGNATURE_1_5,
      sizeof(ufs_block_node_1_4_t),
      &ufs_block_node_1_4,
      _ufs_drv_1_5_drvmakefs,
      _ufs_drv_1_5_drvreadfs,
      _ufs_drv_1_5_drvwritefs,
      _ufs_drv_1_4_readnode,
      _ufs_drv_1_4_writenode,
      _ufs_drv_1_4_readblk,
      _ufs_drv_1_4_writeblk,
      _ufs_drv_1_4_raw_readblk,
      _ufs_drv_1_4_sync_sp_blk,
      _ufs_drv_1_4_sync_sp_node
   },
#endif
};

const int ufs_driver_list_sz=sizeof(ufs_driver_list)/sizeof(ufs_driver_t);

/*===========================================
Implementation
=============================================*/

/*--------------------------------------------
| Name:        _ufs_get_driver
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
ufs_driver_t* _ufs_get_driver(desc_t desc){
   int i;
   int cb=0;

   char buffer[16]={0};

   ofile_lst[desc].pfsop->fdev.fdev_seek( desc, 0L, SEEK_SET );
   //UFS signature instead magic number
   if(( cb = ofile_lst[desc].pfsop->fdev.fdev_read(desc,
                                                   buffer,
                                                   strlen(UFS_SIGNATURE)))<0)
      return (ufs_driver_t*)0;
   ofile_lst[desc].pfsop->fdev.fdev_seek( desc, 0L, SEEK_SET );

   //look up signature and get driver
   for(i=0; i<ufs_driver_list_sz; i++) {
      if(!strcmp(ufs_driver_list[i].p_ufs_drv_signature,buffer))
         return &ufs_driver_list[i];
   }

   //not found
   return (ufs_driver_t*)0;
}

/*--------------------------------------------
| Name:        _ufsdriver_set_driver
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
ufs_driver_t* _ufsdriver_set_driver(char* signature){
   int i;
   //look up signature and get driver
   for(i=0; i<ufs_driver_list_sz; i++) {
      if(!strcmp(ufs_driver_list[i].p_ufs_drv_signature,signature))
         return &ufs_driver_list[i];
   }

   //not found
   return (ufs_driver_t*)0;

}






/*============================================
| End of Source  : ufsdriver.c
==============================================*/
