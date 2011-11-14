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
#include "kernel/types.h"
#include "kernel/interrupt.h"
#include "kernel/kernel.h"
#include "kernel/system.h"
#include "kernel/fcntl.h"
#include "kernel/ioctl_lcd.h"
#include "kernel/ioctl_board.h"

#include "vfs/vfsdev.h"


#include <ioat91sam7se512.h>
#include <intrinsic.h>

/*============================================
| Global Declaration 
==============================================*/


/*============================================
| Implementation 
==============================================*/
const char dev_at91sam7se_cpu_name[]="cpu\0";

int dev_at91sam7se_cpu_load(void);
int dev_at91sam7se_cpu_open(desc_t desc, int o_flag);
int dev_at91sam7se_cpu_close(desc_t desc);
int dev_at91sam7se_cpu_isset_read(desc_t desc);
int dev_at91sam7se_cpu_isset_write(desc_t desc);
int dev_at91sam7se_cpu_read(desc_t desc, char* buf,int size);
int dev_at91sam7se_cpu_write(desc_t desc, const char* buf,int size);
int dev_at91sam7se_cpu_seek(desc_t desc,int offset,int origin);
int dev_at91sam7se_cpu_ioctl(desc_t desc,int request,va_list ap);


dev_map_t dev_at91sam7se_cpu_map={
   dev_at91sam7se_cpu_name,
   S_IFBLK,
   dev_at91sam7se_cpu_load,
   dev_at91sam7se_cpu_open,
   dev_at91sam7se_cpu_close,
   dev_at91sam7se_cpu_isset_read,
   dev_at91sam7se_cpu_isset_write,
   dev_at91sam7se_cpu_read,
   dev_at91sam7se_cpu_write,
   dev_at91sam7se_cpu_seek,
   dev_at91sam7se_cpu_ioctl //ioctl
};


/*===========================================
Implementation
=============================================*/


/*-------------------------------------------
| Name:dev_at91sam7se_cpu_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91sam7se_cpu_load(void){
   //AT91F_InitSdram(); modif now in low level init for use external sdram

   AT91F_InitDM9000A();
   //
   /*
   AT91C_MC_PUIA[4]=0x00000003;
   AT91C_SMC_CSR[2]=0x00003180;
   */

   return 0;
}

/*-------------------------------------------
| Name:dev_at91sam7se_cpu_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91sam7se_cpu_open(desc_t desc, int o_flag){

   //
   if(o_flag & O_RDONLY){
   }

   if(o_flag & O_WRONLY){
   }

   ofile_lst[desc].offset=0;

   return 0;
}

/*-------------------------------------------
| Name:dev_at91sam7se_cpu_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91sam7se_cpu_close(desc_t desc){
   return 0;
}

/*-------------------------------------------
| Name:dev_at91sam7se_cpu_isset_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91sam7se_cpu_isset_read(desc_t desc){
  return -1;
}

/*-------------------------------------------
| Name:dev_at91sam7se_cpu_isset_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91sam7se_cpu_isset_write(desc_t desc){
      return -1;
}
/*-------------------------------------------
| Name:dev_at91sam7se_cpu_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91sam7se_cpu_read(desc_t desc, char* buf,int size){
   return -1;
}

/*-------------------------------------------
| Name:dev_at91sam7se_cpu_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91sam7se_cpu_write(desc_t desc, const char* buf,int size){
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
int dev_at91sam7se_cpu_seek(desc_t desc,int offset,int origin){

   switch(origin){

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
| Name:dev_at91sam7se_cpu_ioctl
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91sam7se_cpu_ioctl(desc_t desc,int request,va_list ap){
   return -1;
}

/*============================================
| End of Source  : dev_at91sam7se_cpu.c
==============================================*/
