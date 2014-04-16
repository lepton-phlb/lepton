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


/*============================================
| Compiler Directive
==============================================*/
#ifndef _UFSDRIVER_1_3_H
#define _UFSDRIVER_1_3_H


/*============================================
| Includes
==============================================*/
#include "kernel/fs/vfs/vfstypes.h"

/*============================================
| Declaration
==============================================*/
//begin 1 byte struct alignment
//force compatiblity with mklepton
#if (__tauon_compiler__!=__compiler_gnuc__)
   #pragma pack(push, 1)
#endif

//
typedef unsigned short ufs_attr_t;
typedef unsigned short ufs_oflags_t;

typedef blocknb_t ufs_blocknb_t;
typedef int16_t ufs_inodenb_t;

typedef int16_t ufs_size_1_3_t; //to do: must be modified to uint32_t to support large file over 16Kbytes.

typedef struct __attribute__((aligned(UFS_ALIGNEMENT))){
   ufs_attr_t attr;
   ufs_size_1_3_t size;

   ino_mod_t ino_mod;
   time_t cmtime;      ///creation/modifed time.

   ufs_blocknb_t blk[1];
   ufs_blocknb_t blk_smpl;
   ufs_blocknb_t blk_dbl;
} ufs_block_node_1_3_t;

#if (__tauon_compiler__!=__compiler_gnuc__)
   #pragma pack (pop)
#endif
//end 1 byte struct alignment

extern ufs_block_node_1_3_t ufs_block_node_1_3;

int   _ufs_drv_1_3_drvmakefs     (desc_t desc,superblk_t* psuperblk);
int   _ufs_drv_1_3_drvreadfs     (desc_t desc,superblk_t* psuperblk);
int   _ufs_drv_1_3_drvwritefs    (desc_t desc,superblk_t* psuperblk);
int   _ufs_drv_1_3_readnode      (desc_t desc,char* pufs_blknode,int node,int size);
int   _ufs_drv_1_3_writenode     (desc_t desc,char* pufs_blknode,int node,int size);
int   _ufs_drv_1_3_readblk       (desc_t desc,char* pufs_blk,int block,int size);
int   _ufs_drv_1_3_writeblk      (desc_t desc,char* pufs_blk,int block,int size);
int   _ufs_drv_1_3_raw_readblk   (mntdev_t* pmntdev,char* pufs_blk,int block,int size);
int   _ufs_drv_1_3_sync_sp_blk   (mntdev_t* pmntdev,char* pufs_blkalloc,int block_no);
int   _ufs_drv_1_3_sync_sp_node  (mntdev_t* pmntdev,char * pufs_nodealloc,int node_no);


#endif
