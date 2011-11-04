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

#include <stdio.h>
#include <string.h>

#include <fcntl.h>

#include <sys/shm.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "virtual_screen.h"
#include "virtual_hardware.h"
#include "virtual_cpu.h"
#include "virtual_cpu_gui.h"

char screen_name[] = "lcd0\0";

int virtual_screen_load(void * data);
int virtual_screen_open(void * data);
int virtual_screen_close(void * data);
int virtual_screen_read(void * data);
int virtual_screen_write(void * data);
int virtual_screen_seek(void * data);
int virtual_screen_ioctl(void * data);

hdwr_info_t virtual_screen = {
   screen_name,
   __fdev_not_fd,
   virtual_screen_load,
   virtual_screen_open,
   virtual_screen_close,
   virtual_screen_read,
   virtual_screen_write,
   virtual_screen_seek,
   virtual_screen_ioctl
};

//
#define  VIRTUAL_SCREEN_IOCTL_LOAD_XRES   0
#define  VIRTUAL_SCREEN_IOCTL_LOAD_YRES   2
#define  VIRTUAL_SCREEN_IOCTL_LOAD_BPP    4

static virtual_screen_t * screen_0_data;
//
int virtual_screen_load(void * data) {
   virtual_cpu_t * vcpu = (virtual_cpu_t *)data;
   unsigned int x_res = SCREEN_XRES;
   unsigned int y_res = SCREEN_YRES;
   unsigned int bpp = SCREEN_BPP;

   //put data pointer in write place
   screen_0_data = (virtual_screen_t *)(vcpu->shm_base_addr + SCRN_OFFSET);

   //pass x_res, y_res and bpp
   sprintf((char *)screen_0_data->data_ioctl,"%4d%4d%4d",x_res,y_res,bpp);

   DEBUG_TRACE("(F) %s [x_rex,y_res,bpp] : %d %d %d load ok..\n", virtual_screen.name,x_res,y_res,bpp);
   return 0;
}

//
int virtual_screen_open(void * data) {
   DEBUG_TRACE("(F) %s open ok..\n", virtual_screen.name);
   return 0;
}

//
int virtual_screen_close(void * data) {
   return 0;
}

//
int virtual_screen_read(void * data) {
   return 0;
}

//
int virtual_screen_write(void * data) {
   return 0;
}

//nothing to do
int virtual_screen_seek(void * data) {
   return 0;
}

//nothing to do
int virtual_screen_ioctl(void * data) {
   return 0;
}
