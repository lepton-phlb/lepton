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
#include "kernel/core/kernelconf.h"
#include "kernel/core/types.h"
#include "kernel/core/interrupt.h"
#include "kernel/core/kernel.h"
#include "kernel/core/system.h"
#include "kernel/core/fcntl.h"
#include "kernel/core/cpu.h"
#include "kernel/fs/vfs/vfsdev.h"
#include "kernel/core/ioctl_board.h"

#include "kernel/dev/arch/cortexm/stm32f4xx/target.h"
#include "kernel/dev/arch/cortexm/stm32f4xx/gpio.h"
#include "kernel/dev/arch/cortexm/stm32f4xx/dma.h"


/*===========================================
Global Declaration
=============================================*/
const char dev_stm32f4xx_olimex_board_name[]="board\0";

int dev_stm32f4xx_olimex_board_load(void);
int dev_stm32f4xx_olimex_board_open(desc_t desc, int o_flag);
int dev_stm32f4xx_olimex_board_close(desc_t desc);
int dev_stm32f4xx_olimex_board_isset_read(desc_t desc);
int dev_stm32f4xx_olimex_board_isset_write(desc_t desc);
int dev_stm32f4xx_olimex_board_read(desc_t desc, char* buf,int size);
int dev_stm32f4xx_olimex_board_write(desc_t desc, const char* buf,int size);
int dev_stm32f4xx_olimex_board_seek(desc_t desc,int offset,int origin);
int dev_stm32f4xx_olimex_board_ioctl(desc_t desc,int request,va_list ap);


dev_map_t dev_stm32f4xx_olimex_board_map={
   dev_stm32f4xx_olimex_board_name,
   S_IFBLK,
   dev_stm32f4xx_olimex_board_load,
   dev_stm32f4xx_olimex_board_open,
   dev_stm32f4xx_olimex_board_close,
   dev_stm32f4xx_olimex_board_isset_read,
   dev_stm32f4xx_olimex_board_isset_write,
   dev_stm32f4xx_olimex_board_read,
   dev_stm32f4xx_olimex_board_write,
   dev_stm32f4xx_olimex_board_seek,
   dev_stm32f4xx_olimex_board_ioctl //ioctl
};


/*===========================================
Implementation
=============================================*/


/*-------------------------------------------
| Name:dev_stm32f4xx_olimex_board_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_stm32f4xx_olimex_board_load(void){
   gpio_startup_init();
   dma_startup_init();
   return 0;
}

/*-------------------------------------------
| Name:dev_stm32f4xx_olimex_board_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_stm32f4xx_olimex_board_open(desc_t desc, int o_flag){

   //
   if(o_flag & O_RDONLY) {
   }

   if(o_flag & O_WRONLY) {
   }

   ofile_lst[desc].offset=0;

   return 0;
}

/*-------------------------------------------
| Name:dev_stm32f4xx_olimex_board_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_stm32f4xx_olimex_board_close(desc_t desc){
   return 0;
}

/*-------------------------------------------
| Name:dev_stm32f4xx_olimex_board_isset_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_stm32f4xx_olimex_board_isset_read(desc_t desc){
   return -1;
}

/*-------------------------------------------
| Name:dev_stm32f4xx_olimex_board_isset_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_stm32f4xx_olimex_board_isset_write(desc_t desc){
   return -1;
}
/*-------------------------------------------
| Name:dev_stm32f4xx_olimex_board_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_stm32f4xx_olimex_board_read(desc_t desc, char* buf,int size){
   return -1;
}

/*-------------------------------------------
| Name:dev_stm32f4xx_olimex_board_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_stm32f4xx_olimex_board_write(desc_t desc, const char* buf,int size){
   //read only mode
   return -1;
}

/*-------------------------------------------
| Name:dev_a0350_board_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_stm32f4xx_olimex_board_seek(desc_t desc,int offset,int origin){

   switch(origin) {

   case SEEK_SET:
      ofile_lst[desc].offset=offset;
      break;

   case SEEK_CUR:
      ofile_lst[desc].offset+=offset;
      break;

   case SEEK_END:
      //to do: warning in SEEK_END (+ or -)????
      ofile_lst[desc].offset-=offset;
      break;
   }

   return ofile_lst[desc].offset;
}

/*-------------------------------------------
| Name:dev_stm32f4xx_olimex_board_ioctl
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_stm32f4xx_olimex_board_ioctl(desc_t desc,int request,va_list ap){

   switch(request) {
   case BRDPWRDOWN:   //power down
      break;
   //
   case BRDRESET:   //reset
      break;

   case BRDWATCHDOG:   //start watchdog
      break;
   //
   case BRDCFGPORT: {
   }
   break;

   case BRDBEEP: {
   }
   break;

   //
   default:
      return -1;

   }

   return 0;
}

/*============================================
| End of Source  : dev_stm32f4xx_olimex_board.c
==============================================*/
