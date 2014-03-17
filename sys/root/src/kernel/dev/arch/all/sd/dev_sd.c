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
| Includes
==============================================*/
#include "kernel/core/types.h"
#include "kernel/core/interrupt.h"
#include "kernel/core/kernelconf.h"
#include "kernel/core/kernel.h"
#include "kernel/core/system.h"
#include "kernel/core/fcntl.h"
#include "kernel/core/ioctl_hd.h"

#include "kernel/fs/vfs/vfsdev.h"

#include "lib/libc/ctype/ctype.h"
#include "kernel/core/types.h"
#include "kernel/core/time.h"
#include "kernel/core/devio.h"
#include "kernel/core/errno.h"

#include "dev_sd.h"
/*============================================
| Global Declaration
==============================================*/
static const char dev_sd_name[]="sd\0";

int dev_sd_load(void);
int dev_sd_open(desc_t desc, int o_flag);
int dev_sd_close(desc_t desc);
int dev_sd_read(desc_t desc, char* buf,int cb);
int dev_sd_write(desc_t desc, const char* buf,int cb);
int dev_sd_seek(desc_t desc,int offset,int origin);
int dev_sd_ioctl(desc_t desc,int request,va_list ap);

dev_map_t dev_sd_map={
   dev_sd_name,
   S_IFBLK,
   dev_sd_load,
   dev_sd_open,
   dev_sd_close,
   __fdev_not_implemented,
   __fdev_not_implemented,
   dev_sd_read,
   dev_sd_write,
   dev_sd_seek,
   dev_sd_ioctl,    //ioctl
};

//
static board_inf_sd_t g_inf_sd
#if defined(CPU_ARM9)
__attribute__ ((section (".no_cache")))
#endif
;
static sd_cmd_t g_sd_cmd
#if defined(CPU_ARM9)
__attribute__ ((section (".no_cache")))
#endif
;

static kernel_pthread_mutex_t g_sd_mutex;
/*============================================
| Implementation
==============================================*/
/*-------------------------------------------
| Name:dev_sd_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_sd_load(void) {
   g_inf_sd.command = &g_sd_cmd;
   return 0;
}

/*-------------------------------------------
| Name:dev_sd_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_sd_open(desc_t desc, int o_flag){
   return 0;
}

/*-------------------------------------------
| Name:dev_sd_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_sd_close(desc_t desc) {
   return 0;
}

/*-------------------------------------------
| Name:dev_sd_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_sd_read(desc_t desc, char* buf,int cb) {
   int dev_current_addr = 0;

   kernel_pthread_mutex_lock(&g_sd_mutex);

   dev_current_addr = (int)ofile_lst[desc].offset;

   if(cb > SD_BUFFER_SIZE_NO_CACHE_RCV) {
      cb = SD_BUFFER_SIZE_NO_CACHE_RCV;
   }

   if ((dev_current_addr + (long)cb) > ((board_inf_sd_t *)ofile_lst[desc].p)->total_size) {
      if(!(cb = ((board_inf_sd_t *)ofile_lst[desc].p)->total_size-dev_current_addr))
         kernel_pthread_mutex_unlock(&g_sd_mutex);
      return 0;   //end of device.
   }

   // Call sdcard driver
   if(_sd_read(desc, buf, cb, 0) != 0) {
      kernel_pthread_mutex_unlock(&g_sd_mutex);
      return -1;
   }

   dev_current_addr += cb;

   ofile_lst[desc].offset = dev_current_addr;

   kernel_pthread_mutex_unlock(&g_sd_mutex);

   return cb;
}

/*-------------------------------------------
| Name:dev_sd_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_sd_write(desc_t desc, const char* buf, int cb) {
   int dev_current_addr = 0;

   kernel_pthread_mutex_lock(&g_sd_mutex);

   dev_current_addr = (int)ofile_lst[desc].offset;

   if(cb > SD_BUFFER_SIZE_NO_CACHE_SND) {
      cb = SD_BUFFER_SIZE_NO_CACHE_SND;
   }

   if ((dev_current_addr + (long)cb) > ((board_inf_sd_t *)ofile_lst[desc].p)->total_size) {
      if(!(cb = ((board_inf_sd_t *)ofile_lst[desc].p)->total_size-dev_current_addr))
         kernel_pthread_mutex_unlock(&g_sd_mutex);
      return 0;   //end of device.
   }

   // Call sdcard driver
   if(_sd_write(desc, buf, cb) != 0) {
      kernel_pthread_mutex_unlock(&g_sd_mutex);
      return -1;
   }

   dev_current_addr += cb;

   ofile_lst[desc].offset = dev_current_addr;

   kernel_pthread_mutex_unlock(&g_sd_mutex);

   return cb;
}

/*-------------------------------------------
| Name:dev_sd_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_sd_seek(desc_t desc,int offset,int origin){
   int dev_current_addr;
   long sd_size = ((board_inf_sd_t *)ofile_lst[desc].p)->total_size;

   kernel_pthread_mutex_lock(&g_sd_mutex);

   dev_current_addr = (int)ofile_lst[desc].offset;

   switch(origin) {
   case SEEK_SET:
      dev_current_addr = (int)(offset%(sd_size));
      break;

   case SEEK_CUR:
      dev_current_addr += (int)(offset%(sd_size));
      break;

   case SEEK_END:
      dev_current_addr=(int)sd_size;
      break;
   }

   ofile_lst[desc].offset = dev_current_addr;

   kernel_pthread_mutex_unlock(&g_sd_mutex);

   return ofile_lst[desc].offset;
}

/*-------------------------------------------
| Name:dev_sd_ioctl
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_sd_ioctl(desc_t desc,int request,va_list ap) {
   switch(request) {
   case HDGETSZ: {
      long* hdsz_p= va_arg( ap, long*);
      if(!hdsz_p)
         return -1;
      *hdsz_p= ((board_inf_sd_t *)ofile_lst[desc].p)->total_size;
   }
   break;

   case I_LINK: {
      //already link
      if(ofile_lst[desc].p)
         return -1;

      kernel_pthread_mutex_init(&g_sd_mutex, NULL);
      ofile_lst[desc].p = (void *)&g_inf_sd;

      return _sd_init((board_inf_sd_t *) ofile_lst[desc].p, ofile_lst[desc].desc_nxt[0]);
   }
   break;

   case I_UNLINK: {
      if(ofile_lst[desc].p)
         ofile_lst[desc].p = NULL;
   }
   break;

   default:
      return -1;
   }

   return 0;
}

/*============================================
| End of Source  : dev_sd.c
|---------------------------------------------
| Historic:
|---------------------------------------------
| Authors     | Date     | Comments
| $Log:$
==============================================*/
