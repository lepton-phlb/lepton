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
#ifndef _FAT_CORE_VFAT_H
#define _FAT_CORE_VFAT_H
/*============================================
| Includes
==============================================*/
#include "fatcore.h"

/*============================================
| Declaration
==============================================*/

#define  __vfat_get_nb_long_entries(__fat_vfat_entry__) \
   (__fat_vfat_entry__.LDIR_Ord&~FAT16_LAST_LONGENTRY_MSK)

int _fat16_vfat_lookupname_ud(fat16_core_info_t * fat_info, desc_t desc_new, char* filename);
int _fat16_vfat_lookupname_rd(fat16_core_info_t * fat_info, desc_t desc_new, char* filename);
//
int _fat16_vfat_get_infos_addr(desc_t desc_new, fat16_u16_t cluster, int nb_entries, fat16_u32_t *current_addr);
//
int _fat16_vfat_build_name(desc_t dev_desc, unsigned char * dst_longname, int nb_ent);
int _fat16_vfat_extract_name(unsigned char * dst, const fat_vfat_dir_entry_t * pvfat_entry);
int _fat16_vfat_build_oob_name(fat16_core_info_t * fat_info, desc_t dev_desc, unsigned char * dst_longname, int nb_ent, fat16_u16_t cluster);
//
fat16_u32_t _fat16_vfat_get_addr_dot_items(fat16_core_info_t * fat_info, desc_t desc, fat16_u16_t parent_cluster, fat16_u16_t pattern_cluster);
//
unsigned char _fat16_vfat_chksum(unsigned char * short_name);
int _fat16_vfat_create_short_name(unsigned char * short_name);
int _fat16_vfat_create_long_name(unsigned char * long_name, fat_vfat_dir_entry_t *ptab_entry, unsigned char chksum);

//
int _fat16_vfat_get_next_long_entry(desc_t desc_new, fat16_u16_t *cluster, int nb_entries, fat16_u32_t *current_addr);
fat16_u32_t _fat16_vfat_write_long_entries(desc_t dev_desc, fat16_u32_t addr, fat_vfat_dir_entry_t *ptab_entry, int pos, int count);
int _fat16_vfat_request_entries(desc_t desc, fat16_u32_t addr_begin, fat16_u32_t addr_end, int nb_ent);
int _fat16_vfat_remove_entries(fat16_core_info_t * fat_info, desc_t dev_desc);
//
inodenb_t _fat16_vfat_request_free_rd(desc_t desc, fat16_u32_t * addr, int req_ent);
inodenb_t _fat16_vfat_request_free_ud(desc_t desc, fat16_u32_t * addr, fat16_u16_t * cluster, fat16_u32_t *prev_addr, fat16_u16_t * prev_cluster, int req_ent, int * rem_ent);
#endif //_FAT_CORE_VFAT_H
