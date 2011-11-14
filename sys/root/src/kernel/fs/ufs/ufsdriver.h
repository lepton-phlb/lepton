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
#ifndef _UFSDRIVER_H
#define _UFSDRIVER_H


/*===========================================
Includes
=============================================*/
#include "kernel/core/kernelconf.h"

/*===========================================
Declaration
=============================================*/

//UFS VERSION 
#define UFS_SIGNATURE         "ufs x.x"
#define UFS_SIGNATURE_1_2     "ufs 1.2"
#define UFS_SIGNATURE_1_3     "ufs 1.3"
#define UFS_SIGNATURE_1_4     "ufs 1.4"
#define UFS_SIGNATURE_1_5     "ufs 1.5"


#if defined(__KERNEL_SUPPORT_UFS_DRIVER_1_3)
   #include "kernel/fs/ufs/ufsdriver_1_3.h"
   #if __KERNEL_SUPPORT_UFS_DRIVER==__KERNEL_SUPPORT_UFS_DRIVER_1_3
      #define UFS_SIGNATURE_DFLT   UFS_SIGNATURE_1_3
   #endif
#endif

#if defined(__KERNEL_SUPPORT_UFS_DRIVER_1_4)
   #include "kernel/fs/ufs/ufsdriver_1_4.h"
   #if __KERNEL_SUPPORT_UFS_DRIVER==__KERNEL_SUPPORT_UFS_DRIVER_1_4
      #define UFS_SIGNATURE_DFLT   UFS_SIGNATURE_1_4
   #endif
#endif

#if defined(__KERNEL_SUPPORT_UFS_DRIVER_1_5)
   #include "kernel/fs/ufs/ufsdriver_1_5.h"
   #if __KERNEL_SUPPORT_UFS_DRIVER==__KERNEL_SUPPORT_UFS_DRIVER_1_5
      #define UFS_SIGNATURE_DFLT   UFS_SIGNATURE_1_5
   #endif
#endif


typedef int   (*_p_ufs_drvmakefs)    (desc_t desc,superblk_t* psuperblk);
typedef int   (*_p_ufs_drvreadfs)    (desc_t desc,superblk_t* psuperblk);
typedef int   (*_p_ufs_drvwritefs)   (desc_t desc,superblk_t* psuperblk);
typedef int   (*_p_ufs_readnode)     (desc_t desc,char* pufs_blknode,int node,int size);
typedef int   (*_p_ufs_writenode)    (desc_t desc,char* pufs_blknode,int node,int size);
typedef int   (*_p_ufs_readblk)      (desc_t desc,char* pufs_blk,int block,int size);
typedef int   (*_p_ufs_writeblk)     (desc_t desc,char* pufs_blk,int block,int size);
typedef int   (*_p_ufs_raw_readblk)  (mntdev_t* pmntdev,char* pufs_blk,int block,int size);
typedef int   (*_p_ufs_sync_sp_blk)  (mntdev_t* pmntdev,char* pufs_blkalloc,int block_no);
typedef int   (*_p_ufs_sync_sp_node) (mntdev_t* pmntdev,char * pufs_nodealloc,int node_no);


typedef _p_ufs_drvmakefs      p_ufs_drvmakefs_t;
typedef _p_ufs_drvreadfs      p_ufs_drvreadfs_t;
typedef _p_ufs_drvwritefs     p_ufs_drvwritefs_t;
typedef _p_ufs_readnode       p_ufs_readnode_t;
typedef _p_ufs_writenode      p_ufs_writenode_t;
typedef _p_ufs_readblk        p_ufs_readblk_t;    
typedef _p_ufs_writeblk       p_ufs_writeblk_t;
typedef _p_ufs_raw_readblk    p_ufs_raw_readblk_t;
typedef _p_ufs_sync_sp_blk    p_ufs_sync_sp_blk_t;
typedef _p_ufs_sync_sp_node   p_ufs_sync_sp_node_t;

typedef void*    p_ufs_blocknode_t;
typedef uint16_t ufs_blocknode_sz_t;

typedef struct ufs_driver_st{
   const char*          p_ufs_drv_signature;
   ufs_blocknode_sz_t   ufs_blocknode_sz;
   p_ufs_blocknode_t    p_ufs_blocknode;
   p_ufs_drvmakefs_t    p_ufs_drvmakefs;
   p_ufs_drvreadfs_t    p_ufs_drvreadfs;
   p_ufs_drvwritefs_t   p_ufs_drvwritefs;
   p_ufs_readnode_t     p_ufs_readnode;
   p_ufs_writenode_t    p_ufs_writenode;
   p_ufs_readblk_t      p_ufs_readblk;    
   p_ufs_writeblk_t     p_ufs_writeblk;
   p_ufs_raw_readblk_t  p_ufs_raw_readblk;
   p_ufs_sync_sp_blk_t  p_ufs_sync_sp_blk;
   p_ufs_sync_sp_node_t p_ufs_sync_sp_node;
}ufs_driver_t;


ufs_driver_t* _ufs_get_driver(desc_t desc);
ufs_driver_t* _ufsdriver_set_driver(char* signature);


#define __ufs_drvmakefs(__p_ufs_driver__,__desc__,__psuperblk__)\
   __p_ufs_driver__->p_ufs_drvmakefs(__desc__,__psuperblk__)
//ok
#define __ufs_drvreadfs(__p_ufs_driver__,__desc__,__psuperblk__)\
   __p_ufs_driver__->p_ufs_drvreadfs(__desc__,__psuperblk__)
//ok
#define __ufs_drvwritefs(__p_ufs_driver__,__desc__,__psuperblk__)\
   __p_ufs_driver__->p_ufs_drvwritefs(__desc__,__psuperblk__)   
//ok
#define __ufs_readnode(__desc__,__pufs_blknode__,__node__,__size__)\
   ofile_lst[desc].pmntdev->fs_info.ufs_info.p_ufs_driver->p_ufs_readnode(__desc__,__pufs_blknode__,__node__,__size__)
//ok
#define __ufs_writenode(__desc__,__pufs_blknode__,__node__,__size__)\
   ofile_lst[desc].pmntdev->fs_info.ufs_info.p_ufs_driver->p_ufs_writenode(__desc__,__pufs_blknode__,__node__,__size__)
//ok
#define __ufs_readblk(__desc__,__pufs_blk__,__block__,__size__)\
   ofile_lst[desc].pmntdev->fs_info.ufs_info.p_ufs_driver->p_ufs_readblk(__desc__,__pufs_blk__,__block__,__size__)
//ok
#define __ufs_writeblk(__desc__,__pufs_blk__,__block__,__size__)\
   ofile_lst[desc].pmntdev->fs_info.ufs_info.p_ufs_driver->p_ufs_writeblk(__desc__,__pufs_blk__,__block__,__size__)
//ok
#define __ufs_raw_readblk(__pmntdev__,__pufs_blk__,__block__,__size__)\
   __pmntdev__->fs_info.ufs_info.p_ufs_driver->p_ufs_raw_readblk(__pmntdev__,__pufs_blk__,__block__,__size__)
//ok
#define __ufs_sync_sp_blk(__pmntdev__,__pufs_blkalloc,__block_no__)\
   __pmntdev__->fs_info.ufs_info.p_ufs_driver->p_ufs_sync_sp_blk(__pmntdev__,__pufs_blkalloc,__block_no__)
//ok
#define __ufs_sync_sp_node(__pmntdev__,__pufs_nodealloc__,__node_no__)\
   __pmntdev__->fs_info.ufs_info.p_ufs_driver->p_ufs_sync_sp_node (__pmntdev__,__pufs_nodealloc__,__node_no__)   


#endif



