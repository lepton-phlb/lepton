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
#include "kernel/core/system.h"
#include "kernel/core/statvfs.h"
#include "kernel/core/stat.h"
#include "kernel/fs/vfs/vfstypes.h"
#include "kernel/fs/rootfs/rootfscore.h"

#if defined(__GNUC__)
   #include <string.h>
#endif

/*===========================================
Global Declaration
=============================================*/

//
//patch: optimization for code memory occupation.
#if defined (WIN32) || defined(CPU_GNU32)
rtfs_block_node_t rtfsinode_lst[RTFS_NODETBL_SIZE]={S_IFNULL};
#elif defined (__IAR_SYSTEMS_ICC) || defined (__IAR_SYSTEMS_ICC__)
rtfs_block_node_t rtfsinode_lst[RTFS_NODETBL_SIZE];
#elif defined(__GNUC__)
rtfs_block_node_t rtfsinode_lst[RTFS_NODETBL_SIZE];
#endif

//
//patch: optimization for code memory occupation.
#if defined (WIN32) || defined(CPU_GNU32)
rtfs_block_data_t rtfsblk_lst[RTFS_BLKTBL_SIZE]={0};
#elif defined (__IAR_SYSTEMS_ICC) || defined (__IAR_SYSTEMS_ICC__)
rtfs_block_data_t rtfsblk_lst[RTFS_BLKTBL_SIZE];
#elif defined(__GNUC__)
rtfs_block_data_t rtfsblk_lst[RTFS_BLKTBL_SIZE];
#endif

//
const int rtfs_blkalloc_size      = RTFS_BLOCK_ALLOC_SIZE;
const int rtfs_superblk_size      = RTFS_BLOCK_ALLOC_SIZE;

//patch: optimization for code memory occupation.
#if defined (WIN32) || defined(CPU_GNU32)
char rtfs_superblk[RTFS_BLOCK_ALLOC_SIZE]={0};
#elif defined (__IAR_SYSTEMS_ICC) || defined (__IAR_SYSTEMS_ICC__)
char rtfs_superblk[RTFS_BLOCK_ALLOC_SIZE];
#elif defined(__GNUC__)
char rtfs_superblk[RTFS_BLOCK_ALLOC_SIZE];
#endif


//
char*          prtfs_superblk;
char*          prtfs_blkalloc;

volatile int _rtfs_offset=0;

/*===========================================
Implementation
=============================================*/


/*-------------------------------------------
| Name:_rtfsmem
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void _rtfs_mem(void)
{
   prtfs_superblk  = (char*)&rtfs_superblk;
   prtfs_blkalloc  = prtfs_superblk;
}

/*-------------------------------------------
| Name:_rtfs_allocblk
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
rtfs_blocknb_t _rtfs_allocblk(void)
{

   rtfs_blocknb_t blk;

   for(blk=0; blk<rtfs_blkalloc_size; blk++) {
      unsigned char b;
      unsigned char byte=prtfs_blkalloc[blk];

      if (byte==0xFF)
         continue;

      for(b=0; b<8; b++) {
         char mask=(0x01<<b);

         if( !(byte&mask) ) {
            prtfs_blkalloc[blk]=byte|mask;
            return ( (blk<<3)+b);
         }
      }
   }

   return INVALID_RTFSBLOCK;
}

/*-------------------------------------------
| Name:_rtfs_freeblk
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void _rtfs_freeblk(rtfs_blocknb_t block){

   rtfs_blocknb_t blk = block>>3;
   char offset=(block-(blk<<3));
   char mask=0x01<<offset;

   prtfs_blkalloc[blk]=prtfs_blkalloc[blk]&(~mask);
}


/*-------------------------------------------
| Name:_rtfs_allocnode
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
inodenb_t _rtfs_allocnode(void)
{
   int inode;
   for(inode=_rtfs_offset; inode<(RTFS_NODETBL_SIZE+_rtfs_offset); inode++)
   {
      if(__rtfsinode_lst(inode).attr==S_IFNULL) {
         return inode; //nodeoffset
      }
   }

   return INVALID_RTFSNODE;
}

/*-------------------------------------------
| Name:_rtfs_freenode
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _rtfs_freenode(inodenb_t inode){
   __rtfsinode_lst(inode).attr=S_IFNULL; //nodeoffset
   return 0;
}

/*-------------------------------------------
| Name:_rtfs_statfs
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _rtfs_statfs(mntdev_t* pmntdev,struct statvfs *statvfs){

   rtfs_blocknb_t blk;

   memset(statvfs,0,sizeof(struct statvfs));

   statvfs->f_bsize  = RTFS_BLOCK_SIZE;
   statvfs->f_frsize = RTFS_BLOCK_SIZE;

   statvfs->f_blocks = rtfs_blkalloc_size<<3; //*8
   statvfs->f_namemax = RTFS_MAX_FILENAME;

   for(blk=0; blk<rtfs_blkalloc_size; blk++) {
      unsigned char b;
      unsigned char byte=prtfs_blkalloc[blk];
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
| Name:_rtfs_makefs
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _rtfs_makefs(desc_t dev_desc,struct vfs_formatopt_t* vfs_formatopt){

   return 0;
}

/*-------------------------------------------
| Name:_rtfs_readfs
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _rtfs_readfs(mntdev_t* pmntdev){

   pmntdev->inodetbl_size = RTFS_NODETBL_SIZE;
   _rtfs_offset = pmntdev->inodenb_offset;

   return 0;
}

/*-------------------------------------------
| Name:_rtfs_writefs
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _rtfs_writefs(mntdev_t* pmntdev){
   return 0;
}

/*-------------------------------------------
| Name:_rtfs_checkfs
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _rtfs_checkfs(mntdev_t* pmntdev){
   return 0;
}

/*-------------------------------------------
| Name:_rtfs_core
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _rtfs_core(void){
   int inode;

   _rtfs_mem();

   for(inode=_rtfs_offset; inode<(RTFS_NODETBL_SIZE+_rtfs_offset); inode++)
   {
      __rtfsinode_lst(inode).attr=S_IFNULL;
   }

   return 0;
}


/*===========================================
End of Source rootfscore.c
=============================================*/
