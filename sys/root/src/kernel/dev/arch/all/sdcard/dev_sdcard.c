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
#include "kernel/core/kernel.h"
#include "kernel/core/system.h"
#include "kernel/fs/vfs/vfsdev.h"
#include "kernel/core/ioctl_hd.h"
#include "kernel/core/malloc.h"

//SPI
#include "dev_sdcard.h" // Driver lepton
#include "drv_sdcard.h" // Driver SdCard


/*===========================================
Global Declaration
=============================================*/

static const char dev_sdcard_name[]="sdcard\0sdcard\0";

int dev_sdcard_load       (void);
int dev_sdcard_isset_read (desc_t desc);
int dev_sdcard_isset_write(desc_t desc);
int dev_sdcard_open       (desc_t desc, int o_flag);
int dev_sdcard_close      (desc_t desc);
int dev_sdcard_seek       (desc_t desc,int offset,int origin);
int dev_sdcard_read       (desc_t desc, char* buf,int cb);
int dev_sdcard_write      (desc_t desc, const char* buf,int cb);
int dev_sdcard_ioctl      (desc_t desc,int request,va_list ap);

dev_map_t dev_sdcard_map={
   dev_sdcard_name,
   S_IFBLK,
   dev_sdcard_load,
   dev_sdcard_open,
   dev_sdcard_close,
   __fdev_not_implemented,
   __fdev_not_implemented,
   dev_sdcard_read,
   dev_sdcard_write,
   dev_sdcard_seek,
   dev_sdcard_ioctl
};

fdev_read_t _if_spi_master_read  = (fdev_read_t)0;
fdev_write_t _if_spi_master_write = (fdev_write_t)0;

typedef struct {
   t_media media_sdcard;
}sdcard_info_t;

/**
 *
 * SDCARD Size
 *
 */
//const int   SDCARD_SIZE =  10L*1024L*1024L;//(10 MB)
#define SDCARD_SIZE     (int)(((sdcard_info_t *)ofile_lst[desc].p)->media_sdcard.size)

static kernel_pthread_mutex_t s_sd_mutex;

#define __sd_lock()      kernel_pthread_mutex_lock  (&s_sd_mutex)
#define __sd_unlock()    kernel_pthread_mutex_unlock(&s_sd_mutex)


/*===========================================
Implementation
=============================================*/

/*-------------------------------------------
| Name:dev_sdcard_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_sdcard_load(void)
{
   // initialization
   pthread_mutexattr_t mutex_attr=0;
   kernel_pthread_mutex_init(&s_sd_mutex, &mutex_attr);

   return 0;
}

/*-------------------------------------------
| Name:dev_sdcard_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_sdcard_open(desc_t desc, int o_flag)
{
   ofile_lst[desc].offset = 0;
   return 0;
}

/*-------------------------------------------
| Name:dev_sdcard_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_sdcard_close(desc_t desc)
{
   return 0;
}

/*-------------------------------------------
| Name:dev_sdcard_seek
| Description:
| Parameters:
| Return Type:
| Comments: SDCARD size => 2Go max
| See:
---------------------------------------------*/
int dev_sdcard_seek(desc_t desc,int offset,int origin)
{
   int dev_current_addr;

   // Semaphore protect
   __sd_lock();

   dev_current_addr = (int)ofile_lst[desc].offset;

   switch(origin)
   {
   case SEEK_SET:
      dev_current_addr = (int)(offset%(SDCARD_SIZE));
      break;

   case SEEK_CUR:
      dev_current_addr += (int)(offset%(SDCARD_SIZE));
      break;

   case SEEK_END:
      dev_current_addr=(int)SDCARD_SIZE;
      break;
   }

   ofile_lst[desc].offset = dev_current_addr;

   // Semaphore protect
   __sd_unlock();

   return ofile_lst[desc].offset;
}

/*-------------------------------------------
| Name:dev_sdcard_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_sdcard_read(desc_t desc, char* buf,int cb)
{
   int result;

   // Semaphore protect
   __sd_lock();

   int dev_current_addr = (int)ofile_lst[desc].offset;

   if ((dev_current_addr + (long)cb) > SDCARD_SIZE)
   {
      if(!(cb = SDCARD_SIZE-dev_current_addr))
         return 0;  //end of device.
   }

   //use current position
   if (__get_if_spi_master_desc() < 0)
      return -1;

   // Call sdcard driver
   result = sdcard_read(desc,
                        (unsigned char *)buf,
                        (unsigned long)dev_current_addr,
                        (unsigned long)cb);

   if (result == -1)    // Test default
      return -1;

   dev_current_addr += cb;

   ofile_lst[desc].offset = dev_current_addr;

   // Semaphore protect
   __sd_unlock();

   return cb;
}

/*-------------------------------------------
| Name:dev_sdcard_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_sdcard_write(desc_t desc, const char* buf,int cb)
{
   int result;

   // Semaphore protect
   __sd_lock();

   int dev_current_addr = (int)ofile_lst[desc].offset;

   if( (dev_current_addr + (long)cb) > SDCARD_SIZE)
   {
      if(!(cb = SDCARD_SIZE-dev_current_addr))
         return 0;  //end of device.
   }

   if(__get_if_spi_master_desc() < 0)
      return -1;

   // Call sdcard driver
   result = sdcard_write(desc,(unsigned char *)buf,
                         (unsigned long)dev_current_addr,
                         (unsigned long)cb);

   if (result == -1)     // Test default
      return -1;

   dev_current_addr += cb;

   ofile_lst[desc].offset = dev_current_addr;

   // Semaphore protect
   __sd_unlock();

   return cb;
}

/*--------------------------------------------
| Name:        dev_sdcard_ioctl
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int dev_sdcard_ioctl(desc_t desc,int request,va_list ap){
   switch(request) {

   case HDGETSZ: {
      long* hdsz_p= va_arg( ap, long*);
      if(!hdsz_p)
         return -1;
      *hdsz_p= ((sdcard_info_t*)ofile_lst[desc].p)->media_sdcard.size;
      // *hdsz_p = SDCARD_SIZE;
   }
   break;

   case I_LINK: {
      if( !(ofile_lst[desc].p = _sys_malloc(sizeof(sdcard_info_t))) )
         return -1;

      sdcard_init(desc,&((sdcard_info_t*)ofile_lst[desc].p)->media_sdcard);
   }
   break;

   case I_UNLINK: {
      if(ofile_lst[desc].p)
         _sys_free(ofile_lst[desc].p);
   }
   break;

   //

   default:
      return -1;
   }

   return 0;
}

/*===========================================
End of Source dev_sdcard.c
=============================================*/
