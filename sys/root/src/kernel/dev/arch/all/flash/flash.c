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


/*============================================
| Includes
==============================================*/
#include "kernel/dev/arch/all/flash/flash.h"
#include "kernel/dev/arch/all/flash/amd/lldapi/lld.h"

/*============================================
| Global Declaration
==============================================*/
const flash_sector_t am29dl640d_sector_map []={
   {0x00000000,      8*1024},
   {0x00010000,     64*1024},
   {0x007F0000,      8*1024},
   {0xFFFFFFFF,      0}
};


const flash_sector_t am29lv320dt_sector_map []={
   {0x00000000,      64*1024},
   {0x003F0000,      8*1024},
   {0xFFFFFFFF,      0}
};

const flash_sector_t s29gl256p_sector_map []={
   {0x00000000,      128*1024},
   {0xFFFFFFFF,      0}
};


const flash_type_t flash_type_lst[]={
   {(const flash_sector_t*)&am29dl640d_sector_map,    0x00800000/sizeof(FLASHDATA),
    sizeof(FLASHDATA)},
   {(const flash_sector_t*)&am29lv320dt_sector_map,   0x00400000/sizeof(FLASHDATA),
    sizeof(FLASHDATA)},
   {(const flash_sector_t*)&s29gl256p_sector_map,     0x02000000/sizeof(FLASHDATA),
    sizeof(FLASHDATA)}
};


/*============================================
| Implementation
==============================================*/


/*============================================
| End of Source  : flash.c
==============================================*/
