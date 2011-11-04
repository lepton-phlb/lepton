/*
The contents of this file are subject to the Mozilla Public License Version 1.1 
(the "License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS" basis, 
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License for the 
specific language governing rights and limitations under the License.

The Original Code is ______________________________________.

The Initial Developer of the Original Code is ________________________.
Portions created by ______________________ are Copyright (C) ______ _______________________.
All Rights Reserved.

Contributor(s): ______________________________________.

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
Compiler Directive
=============================================*/
#ifndef _UFSCORE_H
#define _UFSCORE_H


/*===========================================
Includes
=============================================*/
#include "kernel/fs/vfs/vfstypes.h"
#include "kernel/fs/ufs/ufsdriver.h"


//begin 1 byte struct alignment 
//force compatiblity with mklepton
#if (__tauon_compiler__!=__compiler_gnuc__)
   #pragma pack(push, 1)
#endif

/*===========================================
Declaration
=============================================*/

//
#define UFS_BLOCK_SIZE_MAX     __KERNEL_UFS_BLOCK_SIZE_MAX //64//128///256//128//16//4//64       //Byte

#define INVALID_UFSNODE    -1
#define INVALID_UFSBLOCK   -1

//File
#if defined(USE_UFSX)
   #define UFS_MAX_FILENAME      30//32
#else
   #define UFS_MAX_FILENAME      14//8
#endif

#if __KERNEL_SUPPORT_UFS_DRIVER==13
   typedef ufs_block_node_1_3_t ufs_block_node_t;
#endif

#if __KERNEL_SUPPORT_UFS_DRIVER==14
   typedef ufs_block_node_1_4_t ufs_block_node_t;
#endif

#if __KERNEL_SUPPORT_UFS_DRIVER==15
   typedef ufs_block_node_1_4_t ufs_block_node_t;
#endif
//
typedef struct __attribute__((aligned(UFS_ALIGNEMENT))){
   ufs_blocknb_t blk[UFS_BLOCK_SIZE_MAX/sizeof(ufs_blocknb_t)];
}ufs_block_indirect_t;

typedef struct __attribute__((aligned(UFS_ALIGNEMENT))){
   char byte[UFS_BLOCK_SIZE_MAX];
}ufs_block_byte_t;

typedef struct __attribute__((aligned(UFS_ALIGNEMENT))){
   ufs_inodenb_t inode;
   char name[UFS_MAX_FILENAME];
}ufs_block_dir_t;

typedef union __attribute__((aligned(UFS_ALIGNEMENT))){
   ufs_block_byte_t  data;
}ufs_block_data_t;

//
extern const int ufs_node_size;

//
int  _ufs_fpos(int ufs_base,int ufs_base_smpl,int* poffset_dbl,int* poffset_smpl,int* poffset,int pos);

//
ufs_blocknb_t  _ufs_allocblk(mntdev_t* pmntdev);
int            _ufs_isfreeblk(mntdev_t* pmntdev,ufs_blocknb_t block);
void           _ufs_freeblk(mntdev_t* pmntdev,ufs_blocknb_t block);

//to do: modify alloc node, write node en read node to use physical node number
inodenb_t  _ufs_allocnode(mntdev_t* pmntdev);
int        _ufs_freenode(mntdev_t* pmntdev,inodenb_t inode);

void  _ufs_mem(void);
int   _ufs_statfs(mntdev_t* pmntdev,struct statvfs *statvfs);
int   _ufs_makefs(desc_t dev_desc,struct vfs_formatopt_t* vfs_formatopt);
int   _ufs_readfs(mntdev_t* pmntdev);
int   _ufs_writefs(mntdev_t* pmntdev);
int   _ufs_checkfs(mntdev_t* pmntdev);

#if (__tauon_compiler__!=__compiler_gnuc__)
   #pragma pack (pop)
#endif
//end 1 byte struct alignment 


#endif
