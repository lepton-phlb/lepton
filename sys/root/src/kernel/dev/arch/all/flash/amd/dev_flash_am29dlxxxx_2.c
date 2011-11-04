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
| Includes    
==============================================*/
#include "kernel/core/kernel.h"
#include "kernel/core/system.h"
#include "kernel/core/stat.h"
#include "kernel/core/ioctl_hd.h"
#include "kernel/fs/vfs/vfsdev.h"

#include "dev_flash_nor_amd.h"

/*============================================
| Global Declaration 
==============================================*/

static const char dev_am29dlxxxx_2_name[]="hde\0am29dlxxxx\0";

static int dev_am29dlxxxx_2_load(void);
static int dev_am29dlxxxx_2_open(desc_t desc, int o_flag);

dev_map_t dev_am29dlxxxx_2_map={
   dev_am29dlxxxx_2_name,
   S_IFBLK,
   dev_am29dlxxxx_2_load,
   dev_am29dlxxxx_2_open,
   dev_flash_nor_amd_close,
   __fdev_not_implemented,
   __fdev_not_implemented,
   dev_flash_nor_amd_read,
   dev_flash_nor_amd_write,
   dev_flash_nor_amd_seek,
   dev_flash_nor_amd_ioctl //ioctl
};

static const dev_flash_t dev_flash_am29dlxxxx_2={
   (unsigned short *)0x01000000L,// chip base address
   (flash_type_t*)&flash_type_lst[FLASH_AM29DL640D]
};

/*============================================
| Implementation 
==============================================*/
/*--------------------------------------------
| Name:        dev_flash_nor_amd_1_load
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
static int dev_am29dlxxxx_2_load(void){
      return 0;
}

/*-------------------------------------------
| Name:dev_am29dlxxxx_1_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int dev_am29dlxxxx_2_open(desc_t desc, int o_flag){

   //
   ofile_lst[desc].p=(void*)&dev_flash_am29dlxxxx_2;

   if(o_flag & O_RDONLY){
   }

   if(o_flag & O_WRONLY){
   }

   ofile_lst[desc].offset=0;

   return 0;
}

/*============================================
| End of Source  : dev_flash_am29dlxxxx_2.c
==============================================*/
