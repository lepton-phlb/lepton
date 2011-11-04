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


/*============================================
| Compiler Directive
==============================================*/
#ifndef _FAT_INFO_H
#define _FAT_INFO_H

/*============================================
| Includes
==============================================*/
typedef struct fat_info_st {
   struct fat16_boot_core_info_st*  fat_boot_info;
   struct fat16_core_info_st*       fat_core_info;
}fat_info_t;

//for fat_boot_info
#define  __get_BPB_SecPerClus(__pmntdev__) \
      __pmntdev__->fs_info.fat_info.fat_boot_info->bpb.BPB_SecPerClust

#define  __get_BPB_BytesPerSec(__pmntdev__) \
      __pmntdev__->fs_info.fat_info.fat_boot_info->bpb.BPB_BytesPerSec

#define  __get_BPB_RootEntCnt(__pmntdev__) \
      __pmntdev__->fs_info.fat_info.fat_boot_info->bpb.BPB_RootEntCnt

#define  __get_BPB_TotSec16(__pmntdev__) \
      __pmntdev__->fs_info.fat_info.fat_boot_info->bpb.BPB_TotSec16

#define  __get_BPB_TotSec32(__pmntdev__) \
      __pmntdev__->fs_info.fat_info.fat_boot_info->bpb.BPB_TotSec32

//for fat_core_info
#define  __get_nbsec_per_clus(__pmntdev__) \
      __pmntdev__->fs_info.fat_info.fat_core_info->nbsec_per_clus

#define  __get_fat_addr(__pmntdev__) \
      __pmntdev__->fs_info.fat_info.fat_core_info->fat_addr

#define  __get_fat_size(__pmntdev__) \
      __pmntdev__->fs_info.fat_info.fat_core_info->fat_size

#define  __get_rd_addr(__pmntdev__) \
      __pmntdev__->fs_info.fat_info.fat_core_info->rd_addr

#define  __get_rd_size(__pmntdev__) \
      __pmntdev__->fs_info.fat_info.fat_core_info->rd_size

#define  __get_ud_addr(__pmntdev__) \
      __pmntdev__->fs_info.fat_info.fat_core_info->ud_addr

#define  __get_ud_size(__pmntdev__) \
      __pmntdev__->fs_info.fat_info.fat_core_info->ud_size

/*============================================
| Declaration
==============================================*/
#endif //_FAT_INFO_H
