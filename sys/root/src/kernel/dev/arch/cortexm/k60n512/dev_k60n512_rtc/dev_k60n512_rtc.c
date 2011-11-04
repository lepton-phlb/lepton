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

/*===========================================
Includes
=============================================*/
#include "kernel/core/types.h"
#include "kernel/core/interrupt.h"
#include "kernel/core/kernelconf.h"
#include "kernel/core/kernel.h"
#include "kernel/core/system.h"
#include "kernel/core/fcntl.h"
#include "kernel/core/cpu.h"
#include "kernel/core/core_rttimer.h"
#include "kernel/fs/vfs/vfsdev.h"

#include "lib/libc/termios/termios.h"

/*===========================================
Global Declaration
=============================================*/
const char dev_k60n512_rtc_name[]="rtc0\0\0";

static int dev_k60n512_rtc_load(void);
static int dev_k60n512_rtc_open(desc_t desc, int o_flag);
static int dev_k60n512_rtc_close(desc_t desc);
static int dev_k60n512_rtc_seek(desc_t desc,int offset,int origin);
static int dev_k60n512_rtc_read(desc_t desc, char* buf,int cb);
static int dev_k60n512_rtc_write(desc_t desc, const char* buf,int cb);

dev_map_t dev_k60n512_rtc_map={
   dev_k60n512_rtc_name,
   S_IFCHR,
   dev_k60n512_rtc_load,
   dev_k60n512_rtc_open,
   dev_k60n512_rtc_close,
   __fdev_not_implemented,
   __fdev_not_implemented,
   dev_k60n512_rtc_read,
   dev_k60n512_rtc_write,
   dev_k60n512_rtc_seek,
   __fdev_not_implemented
};

/*===========================================
Implementation
=============================================*/
/*-------------------------------------------
| Name:dev_k60n512_rtc_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_rtc_load(void){
   return 0;
}

/*-------------------------------------------
| Name:dev_k60n512_rtc_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_rtc_open(desc_t desc, int o_flag){
   return 0;
}

/*-------------------------------------------
| Name:dev_k60n512_rtc_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_rtc_close(desc_t desc){
   return 0;
}

/*-------------------------------------------
| Name:dev_k60n512_rtc_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_rtc_read(desc_t desc, char* buf,int size){
   return size;
}

/*-------------------------------------------
| Name:dev_k60n512_rtc_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_rtc_write(desc_t desc, const char* buf,int size){
   return size;
}

/*-------------------------------------------
| Name:dev_k60n512_rtc_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_rtc_seek(desc_t desc,int offset,int origin){
   return 0;
}

/*============================================
| End of Source  : dev_k60n512_rtc.c
==============================================*/
