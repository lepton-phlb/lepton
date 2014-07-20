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


/*===========================================
Includes
=============================================*/
#include "kernel/core/types.h"
#include "kernel/core/interrupt.h"
#include "kernel/core/kernelconf.h"
#include "kernel/core/kernel.h"
#include "kernel/core/system.h"
#include "kernel/core/stat.h"
#include "kernel/core/fcntl.h"

#include "lib/libc/termios/termios.h"

#include "kernel/fs/vfs/vfsdev.h"

#include "win32_core_pkt.h"

/*===========================================
Global Declaration
=============================================*/
int dev_win32_eth_load(void);
int dev_win32_eth_open(desc_t desc, int o_flag);
int dev_win32_eth_close(desc_t desc);
int dev_win32_eth_isset_read(desc_t desc);
int dev_win32_eth_isset_write(desc_t desc);
int dev_win32_eth_read(desc_t desc, char* buf,int size);
int dev_win32_eth_write(desc_t desc, const char* buf,int size);
int dev_win32_eth_seek(desc_t desc,int offset,int origin);
int dev_win32_eth_ioctl(desc_t desc,int request,va_list ap);

const char dev_win32_eth_name[]="eth0\0";

dev_map_t dev_win32_eth_map={
   dev_win32_eth_name,
   S_IFCHR,
   dev_win32_eth_load,
   dev_win32_eth_open,
   dev_win32_eth_close,
   dev_win32_eth_isset_read,
   dev_win32_eth_isset_write,
   dev_win32_eth_read,
   dev_win32_eth_write,
   dev_win32_eth_seek,
   dev_win32_eth_ioctl
};


desc_t _win32_eth_desc_rd=-1;
desc_t _win32_eth_desc_wr=-1;

unsigned int __win32_eth_input_r = 0;
unsigned int __win32_eth_input_w = 0;


/*===========================================
Implementation
=============================================*/

/*-------------------------------------------
| Name:dev_win32_eth_rcv_interrupt
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
__hw_interrupt(120,dev_win32_eth_rcv_interrupt){
   __hw_enter_interrupt();
   __win32_eth_input_w++;
   if(_win32_eth_desc_rd>=0 && ofile_lst[_win32_eth_desc_rd].owner_pthread_ptr_read)
      __fire_io_int(ofile_lst[_win32_eth_desc_rd].owner_pthread_ptr_read);
   __hw_leave_interrupt();
}

/*-------------------------------------------
| Name:dev_win32_eth_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_eth_load(void){
   win32_eth_start();
   return 0;
}

/*-------------------------------------------
| Name:dev_win32_eth_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_eth_open(desc_t desc, int o_flag){
   //
   if(o_flag & O_RDONLY) {
      if(_win32_eth_desc_rd>=0)  //already open: exclusive resource.
         return -1;
      _win32_eth_desc_rd = desc;
      __win32_eth_input_r = 0;
      __win32_eth_input_w = 0;
      //
      win32_eth_enable_interrupt();
   }

   if(o_flag & O_WRONLY) {
      if(_win32_eth_desc_wr>=0) //already open: exclusive resource.
         return -1;
      _win32_eth_desc_wr = desc;

   }

   return 0;
}

/*-------------------------------------------
| Name:dev_win32_eth_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_eth_close(desc_t desc){

   if(ofile_lst[desc].oflag & O_RDONLY) {
      if(!ofile_lst[desc].nb_reader) {
         win32_eth_disable_interrupt();
      }
   }

   if(ofile_lst[desc].oflag & O_WRONLY) {
      if(!ofile_lst[desc].nb_writer) {
      }
   }

   return 0;
}

/*-------------------------------------------
| Name:dev_win32_eth_isset_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_eth_isset_read(desc_t desc){
   if(win32_eth_pkt_available()==1)
      return 0;
   //
   return -1;
}

/*-------------------------------------------
| Name:dev_win32_eth_isset_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_eth_isset_write(desc_t desc){
   return 0;
}

/*-------------------------------------------
| Name:dev_win32_eth_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_eth_read(desc_t desc, char* buf,int size){
   int cb;
   cb = win32_eth_getpkt(buf,size);
   if(cb>0)
      __win32_eth_input_r++;
   return cb;
}

/*-------------------------------------------
| Name:dev_win32_eth_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_eth_write(desc_t desc, const char* buf,int size){
   int r;
   r=win32_eth_putpkt(buf,size);
   __fire_io_int(ofile_lst[_win32_eth_desc_wr].owner_pthread_ptr_write);
   return r;
}

/*-------------------------------------------
| Name:dev_win32_eth_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_eth_seek(desc_t desc,int offset,int origin){
   return 0;
}

/*-------------------------------------------
| Name:dev_win32_eth_ioctl
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_eth_ioctl(desc_t desc,int request,va_list ap){
   return 0;
}



/*============================================
| End of Source  : dev_win32_eth.c
==============================================*/
