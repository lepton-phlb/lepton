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

#include "kernel/core/types.h"
#include "kernel/core/interrupt.h"
#include "kernel/core/system.h"
#include "kernel/core/fcntl.h"
#include "kernel/core/ioctl_lcd.h"
#include "kernel/core/ioctl.h"
#include "kernel/core/malloc.h"

#include "kernel/fs/vfs/vfsdev.h"
#include "kernel/fs/vfs/vfstypes.h"

#include "cyg/cpuload/cpuload.h"
/*============================================
| Global Declaration
==============================================*/
static int dev_loadavg_load(void);
static int dev_loadavg_open(desc_t desc,int o_flag);
static int dev_loadavg_close(desc_t desc);
static int dev_loadavg_isset_read(desc_t desc);
static int dev_loadavg_isset_write(desc_t desc);
static int dev_loadavg_write(desc_t desc, const char* buf,int size);
static int dev_loadavg_read(desc_t desc, char* buf,int size);
static int dev_loadavg_seek(desc_t desc,int offset,int origin);
static int dev_loadavg_ioctl(desc_t desc,int request,va_list ap);

static const char dev_loadavg_name[]="load\0";

dev_map_t dev_loadavg_map={
   dev_loadavg_name,
   S_IFBLK,
   dev_loadavg_load,
   dev_loadavg_open,
   dev_loadavg_close,
   dev_loadavg_isset_read,
   dev_loadavg_isset_write,
   dev_loadavg_read,
   dev_loadavg_write,
   dev_loadavg_seek,
   dev_loadavg_ioctl //ioctl
};

typedef struct loadavg_info_st {
   desc_t desc;
   unsigned int calib;
   cyg_handle_t handle;
   cyg_cpuload_t cpuload;

   unsigned int loadavg_100ms;
   unsigned int loadavg_1s;
   unsigned int loadavg_10s;
} loadavg_info_t;

static loadavg_info_t loadavg_info;
/*============================================
| Implementation
==============================================*/
/*--------------------------------------------
| Name:        dev_fb_load
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int dev_loadavg_load(void) {
   loadavg_info.desc = -1;
   return 0;
}

/*--------------------------------------------
| Name:        dev_loadavg_open
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int dev_loadavg_open(desc_t desc,int o_flag) {
   //already open
   if(loadavg_info.desc >= 0) {
      return -1;
   }

   //open only in readonly
   if(o_flag & O_WRONLY) {
      return -1;
   }

   if(o_flag & O_RDONLY) {
      loadavg_info.desc = desc;
      ofile_lst[desc].p = (void *)&loadavg_info;
   }

   //make calibration and start measurement
   cyg_cpuload_calibrate((cyg_uint32 *)&loadavg_info.calib);
   cyg_cpuload_create(&loadavg_info.cpuload, (cyg_uint32 )loadavg_info.calib,
                      &loadavg_info.handle);
   return 0;
}

/*--------------------------------------------
| Name:        dev_loadavg_close
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int dev_loadavg_close(desc_t desc) {
   if(ofile_lst[desc].oflag & O_RDONLY) {
      if(!ofile_lst[desc].nb_reader) {
         loadavg_info_t * p_loadavg = (loadavg_info_t *)ofile_lst[desc].p;
         cyg_cpuload_delete(p_loadavg->handle);
         ofile_lst[desc].p=NULL;
         loadavg_info.desc = -1;
      }
   }

   return 0;
}

/*--------------------------------------------
| Name:        dev_loadavg_isset_read
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int dev_loadavg_isset_read(desc_t desc) {
   return -1;
}

/*--------------------------------------------
| Name:        dev_loadavg_isset_write
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int dev_loadavg_isset_write(desc_t desc) {
   return -1;
}

/*--------------------------------------------
| Name:        dev_loadavg_write
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int dev_loadavg_write(desc_t desc, const char* buf,int size) {
   return 0;
}

/*--------------------------------------------
| Name:        dev_loadavg_read
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int dev_loadavg_read(desc_t desc, char* buf,int size) {
   loadavg_info_t * p_loadavg = (loadavg_info_t *)ofile_lst[desc].p;

   //buf to short or desc not open
   if(size < (sizeof(cyg_uint32)*3)) {
      return -1;
   }

   if(p_loadavg->desc < 0) {
      return -1;
   }

   cyg_cpuload_get(p_loadavg->handle,
                   (cyg_uint32 *)&p_loadavg->loadavg_100ms,
                   (cyg_uint32 *)&p_loadavg->loadavg_1s,
                   (cyg_uint32 *)&p_loadavg->loadavg_10s);

   memcpy((void *)buf, (void *)&p_loadavg->loadavg_100ms, sizeof(cyg_uint32));
   memcpy((void *)buf+4, (void *)&p_loadavg->loadavg_1s, sizeof(cyg_uint32));
   memcpy((void *)buf+8, (void *)&p_loadavg->loadavg_10s, sizeof(cyg_uint32));
   return size;
}

/*--------------------------------------------
| Name:        dev_loadavg_seek
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int dev_loadavg_seek(desc_t desc,int offset,int origin) {
   return 0;
}

/*--------------------------------------------
| Name:        dev_loadavg_ioctl
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int dev_loadavg_ioctl(desc_t desc,int request,va_list ap) {
   return 0;
}

/*============================================
| End of Source  : dev_loadavg.c
|---------------------------------------------
| Historic:
|---------------------------------------------
| Authors     | Date     | Comments
| $Log:$
==============================================*/


