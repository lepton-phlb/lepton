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
#ifndef _OS_RTFSCORE_H
#define _OS_RTFSCORE_H


/*===========================================
Includes
=============================================*/
#include "kernel/core/system.h"
#include "kernel/fs/vfs/vfstypes.h"

/*===========================================
Declaration
=============================================*/
#ifndef __KERNEL_RTFS_NODETBL_SIZE
   #define RTFS_NODETBL_SIZE   50//128 //50//40//30//20
#else
   #define RTFS_NODETBL_SIZE   __KERNEL_RTFS_NODETBL_SIZE
#endif

#ifndef __KERNEL_RTFS_BLKTBL_SIZE
   #define RTFS_BLKTBL_SIZE    120//256 //120//100//80//60
#else
   #define RTFS_BLKTBL_SIZE  __KERNEL_RTFS_BLKTBL_SIZE
#endif

#define RTFS_BLOCK_ALLOC_SIZE   (RTFS_BLKTBL_SIZE/8)+1 //1 bits: free(0|1);

#define RTFS_MAX_FILENAME 8 //12

typedef unsigned short rtfs_attr_t;
typedef unsigned short rtfs_size_t;
typedef unsigned short rtfs_oflags_t;

typedef blocknb_t rtfs_blocknb_t;
typedef int16_t rtfs_inodenb_t;


#define INVALID_RTFSNODE    -1
#define INVALID_RTFSBLOCK   -1

#define RTFS_ROOT_NODE   0

//
typedef struct {
   unsigned short inode;
   char name[RTFS_MAX_FILENAME];
}rtfs_block_dir_t;

//
#ifndef __KERNEL_RTFS_BLOCK_SIZE
   #define RTFS_BLOCK_SIZE 16 //16
#else
   #define RTFS_BLOCK_SIZE  __KERNEL_RTFS_BLOCK_SIZE
#endif

typedef union {
   char byte[RTFS_BLOCK_SIZE];
}rtfs_block_data_t;

//
#ifndef __KERNEL_RTFS_NODE_BLOCK_NB_MAX
   #define MAX_RTFS_BLOCK   32
#else
   #define MAX_RTFS_BLOCK   __KERNEL_RTFS_NODE_BLOCK_NB_MAX
#endif

typedef struct {
   rtfs_attr_t attr;
   rtfs_size_t size;

   ino_mod_t ino_mod;
   time_t cmtime;     ///creation/modifed time.

   rtfs_blocknb_t blk[MAX_RTFS_BLOCK];
}rtfs_block_node_t;

extern rtfs_block_node_t rtfsinode_lst[RTFS_NODETBL_SIZE];
extern rtfs_block_data_t rtfsblk_lst[RTFS_BLKTBL_SIZE];

extern volatile int _rtfs_offset;

#define __rtfsinode_lst(__inode__) rtfsinode_lst[(__inode__-_rtfs_offset)]


//
int               _rtfs_core(void);
int               _rtfs_checkfs(mntdev_t* pmntdev);

rtfs_blocknb_t    _rtfs_allocblk(void);
void              _rtfs_freeblk(rtfs_blocknb_t block);

int               _rtfs_makefs(desc_t dev_desc,struct vfs_formatopt_t* vfs_formatopt);
int               _rtfs_readfs(mntdev_t* pmntdev);
int               _rtfs_writefs(mntdev_t* pmntdev);
int               _rtfs_statfs(mntdev_t* pmntdev,struct statvfs *statvfs);

inodenb_t         _rtfs_allocnode(void);
int               _rtfs_freenode(inodenb_t inode);

#endif
