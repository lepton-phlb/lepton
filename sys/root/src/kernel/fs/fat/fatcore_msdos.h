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
#ifndef _FAT_CORE_MSDOS_H
#define _FAT_CORE_MSDOS_H
/*============================================
| Includes
==============================================*/
#include "fatcore.h"

/*============================================
| Declaration
==============================================*/
int _fat16_msdos_lookupname_ud(fat16_core_info_t * fat_info, desc_t desc_new, char* filename);
int _fat16_msdos_lookupname_rd(fat16_core_info_t * fat_info, desc_t desc_new, char* filename);
//
int _fat16_msdos_getname(const char *filename, char *fat_name);
int _fat16_msdos_fillname(unsigned char *dst, unsigned char *src);
//
fat16_u32_t _fat16_msdos_get_addr_dot_items(fat16_core_info_t * fat_info, desc_t desc,
                                            fat16_u16_t parent_cluster,
                                            fat16_u16_t pattern_cluster);

#endif //_FAT_CORE_MSDOS_H
