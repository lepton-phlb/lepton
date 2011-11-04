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
#include <stdlib.h>

#include "kernel/core/types.h"
#include "kernel/core/interrupt.h"
#include "kernel/core/system.h"
#include "kernel/core/fcntl.h"
#include "kernel/core/time.h"

#include "kernel/fs/vfs/vfsdev.h"
#include "kernel/fs/vfs/vfstypes.h"

#include "lib/libc/stdio/stdio.h"

#include "dev_linux_rtc.h"
#include "kernel/dev/arch/gnu32/common/dev_linux_stub.h"

/*============================================
| Global Declaration
==============================================*/
const char dev_linux_rtc_name[]="rtc0\0";

int dev_linux_rtc_load(void);
int dev_linux_rtc_open(desc_t desc, int o_flag);
int dev_linux_rtc_close(desc_t desc);
int dev_linux_rtc_isset_read(desc_t desc);
int dev_linux_rtc_isset_write(desc_t desc);
int dev_linux_rtc_read(desc_t desc, char* buf,int size);
int dev_linux_rtc_write(desc_t desc, const char* buf,int size);
int dev_linux_rtc_seek(desc_t desc,int offset,int origin);

//specific rtc device function
int dev_linux_rtc_settime(desc_t desc,char* buf,int size);  
int dev_linux_rtc_gettime(desc_t desc,char* buf,int size);  

dev_rtc_t dev_linux_rtc_ext={
   dev_linux_rtc_settime,
   dev_linux_rtc_gettime
};


dev_map_t dev_linux_rtc_map={
   dev_linux_rtc_name,
   S_IFBLK,
   dev_linux_rtc_load,
   dev_linux_rtc_open,
   dev_linux_rtc_close,
   __fdev_not_implemented,
   __fdev_not_implemented,
   dev_linux_rtc_read,
   dev_linux_rtc_write,
   dev_linux_rtc_seek,
   __fdev_not_implemented, //ioctl
   (pfdev_ext_t)&dev_linux_rtc_ext
};


/*============================================
| Implementation
==============================================*/
/*-------------------------------------------
| Name:dev_linux_rtc_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_rtc_load(void){
   return 0;
}

/*-------------------------------------------
| Name:dev_linux_rtc_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_rtc_open(desc_t desc, int o_flag){

   //
   if(o_flag & O_RDONLY){
   }

   if(o_flag & O_WRONLY){      
   }

   ofile_lst[desc].offset=0;

   return 0;
}

/*-------------------------------------------
| Name:dev_linux_rtc_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_rtc_close(desc_t desc){

   if(ofile_lst[desc].oflag & O_RDONLY){
      if(!ofile_lst[desc].nb_reader){
      }
   }
   
   if(ofile_lst[desc].oflag & O_WRONLY){
      if(!ofile_lst[desc].nb_writer){
      }
   }
   
   return 0;
}

/*-------------------------------------------
| Name:dev_linux_rtc_isset_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_rtc_isset_read(desc_t desc){
   return -1;
}

/*-------------------------------------------
| Name:dev_linux_rtc_isset_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_rtc_isset_write(desc_t desc){
   return -1;
}

/*-------------------------------------------
| Name:dev_linux_rtc_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_rtc_read(desc_t desc, char* buf,int size){
   struct tm *newtime;
   long ltime;
   int cb;

#if defined(USE_ECOS)
   struct cyg_hal_sys_timeval cyg_tv;
   cyg_hal_sys_gettimeofday(&cyg_tv, NULL);
   //sous-cast (voir cyg/hal/hal_io.h)
   cyg_hal_sys_time( &ltime );
#else
   _sys_time(&ltime);
#endif
   /* Obtain coordinated universal time: */
   newtime = gmtime( &ltime );
   //
   for( cb=0; cb<size; cb++){
   
      //
      switch (ofile_lst[desc].offset){
         //sec
         case 0:
            buf[cb]=(char)newtime->tm_sec;
         break;

         //min
         case 1:
            buf[cb]=(char)newtime->tm_min;
         break;

         //hour
         case 2:
            buf[cb]=(char)newtime->tm_hour;
         break;

         //day
         case 3:
            buf[cb]=(char)newtime->tm_mday;
         break;

         //month
         case 4:
            buf[cb]=(char)newtime->tm_mon;
         break;

         //year
         case 5:
            buf[cb]=(char)newtime->tm_year;
         break;

         default:
            buf[cb]=0x00;
         break;
      }

      ofile_lst[desc].offset++;
   }

   return cb;
}

/*-------------------------------------------
| Name:dev_linux_rtc_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_rtc_write(desc_t desc, const char* buf,int size){
   //offset (0): sec 0..59
   //offset (1): min 0.59
   //offset (2): hour 0..23
   //offset (3): day 1..31
   //offset (4): month 0..11
   //offset (5): year 00 .. 99

   return 0;
}

/*-------------------------------------------
| Name:dev_linux_rtc_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_rtc_seek(desc_t desc,int offset,int origin){
   switch(origin){

      case SEEK_SET:
         ofile_lst[desc].offset=offset;
      break;

      case SEEK_CUR:
         ofile_lst[desc].offset+=offset;
      break;

      case SEEK_END:
         //to do: warning in SEEK_END (+ or -)????
         ofile_lst[desc].offset+=offset;
      break;
   }

   return ofile_lst[desc].offset;
}

/*-------------------------------------------
| Name:dev_linux_rtc_settime
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_rtc_settime(desc_t desc,char* buf,int size){
   return 0;
}

/*-------------------------------------------
| Name:dev_win32_rtc_gettime
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_rtc_gettime(desc_t desc,char* buf,int size){
   return dev_linux_rtc_read(desc,buf,size);
}

/*============================================
| End of Source  : dev_linux_rtc.c
|---------------------------------------------
| Historic:
|---------------------------------------------
| Authors     | Date     | Comments
| $Log: not supported by cvs2svn $
| Revision 1.1  2009/03/30 15:49:23  jjp
| first import of tauon
|
| Revision 1.1  2009/03/30 11:16:11  jjp
| First import of tauon
|
|---------------------------------------------
==============================================*/
