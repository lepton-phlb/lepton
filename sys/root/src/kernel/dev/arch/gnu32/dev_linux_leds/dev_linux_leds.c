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
#include <stdlib.h>
#include <string.h>
#include "cyg/hal/hal_io.h"

#include "kernel/core/types.h"
#include "kernel/core/interrupt.h"
#include "kernel/core/system.h"
#include "kernel/core/fcntl.h"

#include "kernel/fs/vfs/vfsdev.h"
#include "kernel/fs/vfs/vfstypes.h"


#include "dev_linux_leds.h"
#include "kernel/dev/arch/gnu32/common/linux_hdwr_ops.h"

/*============================================
| Global Declaration
==============================================*/
int dev_linux_leds_load(void);
int dev_linux_leds_open(desc_t desc, int o_flag);
int dev_linux_leds_close(desc_t desc);
int dev_linux_leds_read(desc_t desc, char* buf,int size);
int dev_linux_leds_write(desc_t desc, const char* buf,int size);
int dev_linux_leds_ioctl(desc_t desc,int request,va_list ap);

//
const char dev_linux_leds_name[]="leds0\0";

dev_map_t dev_linux_leds_map={
   dev_linux_leds_name,
   S_IFBLK,
   dev_linux_leds_load,
   dev_linux_leds_open,
   dev_linux_leds_close,
   __fdev_not_implemented,
   __fdev_not_implemented,
   dev_linux_leds_read,
   dev_linux_leds_write,
   __fdev_not_implemented,
   dev_linux_leds_ioctl
};

//buffer et variables de lecture
static volatile int _linux_leds_desc_wr = -1; //flag O_WRONLY

//
static virtual_cmd_t leds_cmd;
static virtual_leds_t * leds_0_data;
extern void * shared_dev_addr;

/*============================================
| Implementation
==============================================*/
/*-------------------------------------------
| Name:dev_linux_leds_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_leds_load(void) {
   //
   leds_0_data = (virtual_leds_t *)(shared_dev_addr + LEDS_OFFSET);
   return 0;
}

/*-------------------------------------------
| Name:dev_linux_leds_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_leds_open(desc_t desc, int o_flag) {
   if(o_flag & O_WRONLY) {
      if(_linux_leds_desc_wr>=0) //already open
         return -1;
   }
   //
   leds_cmd.hdwr_id = LEDS;
   leds_cmd.cmd = OPS_OPEN;
   //
   //disable IT
   __clr_irq();
   while(cyg_hal_sys_write(1, (void *)&leds_cmd, sizeof(virtual_cmd_t)) !=sizeof(virtual_cmd_t));
   while(cyg_hal_sys_read(0, (void *)&leds_cmd, sizeof(virtual_cmd_t)) !=sizeof(virtual_cmd_t));
   //enable IT
   __set_irq();

   return 0;
}

/*-------------------------------------------
| Name:dev_linux_leds_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_leds_close(desc_t desc) {
   if(ofile_lst[desc].oflag & O_WRONLY){
      if(!ofile_lst[desc].nb_writer){
         _linux_leds_desc_wr = -1;
         leds_cmd.hdwr_id = LEDS;
         leds_cmd.cmd = OPS_CLOSE;
         //
         //disable IT
         __clr_irq();
         while(cyg_hal_sys_write(1, (void *)&leds_cmd, sizeof(virtual_cmd_t)) !=sizeof(virtual_cmd_t));
         while(cyg_hal_sys_read(0, (void *)&leds_cmd, sizeof(virtual_cmd_t)) !=sizeof(virtual_cmd_t));
         //enable IT
         __set_irq();
      }
   }

   return 0;
}

/*-------------------------------------------
| Name:dev_linux_leds_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_leds_read(desc_t desc, char* buf,int size) {
   return 0;
}

/*-------------------------------------------
| Name:dev_linux_leds_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_leds_write(desc_t desc, const char* buf,int size) {
   if(size>SHM_LEDS_MAX)
      size = SHM_LEDS_MAX;

   //copy data directly on shared memory
   memcpy(leds_0_data->data_out, buf, size);

   //send order to write data on hardware serial port
   leds_cmd.hdwr_id = LEDS;
   leds_cmd.cmd = OPS_WRITE;

   //disable IT
   __clr_irq();
   //
   while(cyg_hal_sys_write(1, (void *)&leds_cmd, sizeof(virtual_cmd_t)) !=sizeof(virtual_cmd_t));
   while(cyg_hal_sys_read(0, (void *)&leds_cmd, sizeof(virtual_cmd_t)) !=sizeof(virtual_cmd_t));

   //Enable all IT
   __set_irq();
   return size;
}

/*-------------------------------------------
| Name:dev_linux_leds_ioctl
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_leds_ioctl(desc_t desc,int request,va_list ap) {
   return 0;
}

/*============================================
| End of Source  : dev_linux_leds.c
|---------------------------------------------
| Historic:
|---------------------------------------------
| Authors     | Date     | Comments
| $Log:$
==============================================*/
