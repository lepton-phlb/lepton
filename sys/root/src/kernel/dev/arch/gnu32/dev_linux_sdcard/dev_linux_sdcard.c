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
#include "kernel/core/system.h"
#include "kernel/core/ioctl_hd.h"

#include "kernel/fs/vfs/vfsdev.h"
#include "kernel/fs/vfs/vfstypes.h"

#include "kernel/dev/arch/gnu32/common/dev_linux_stub.h"

#include <string.h>
#include <stdlib.h>

/*============================================
| Global Declaration
==============================================*/
const char dev_linux_sdcard_name[]="sdcard0\0";

int dev_linux_sdcard_load(void);
int dev_linux_sdcard_open(desc_t desc, int o_flag);
int dev_linux_sdcard_close(desc_t desc);
int dev_linux_sdcard_isset_read(desc_t desc);
int dev_linux_sdcard_isset_write(desc_t desc);
int dev_linux_sdcard_read(desc_t desc, char* buf,int size);
int dev_linux_sdcard_write(desc_t desc, const char* buf,int size);
int dev_linux_sdcard_seek(desc_t desc,int offset,int origin);
int dev_linux_sdcard_ioctl(desc_t desc,int request,va_list ap);

dev_map_t dev_linux_sdcard_map={
   dev_linux_sdcard_name,
   S_IFBLK,
   dev_linux_sdcard_load,
   dev_linux_sdcard_open,
   dev_linux_sdcard_close,
   __fdev_not_implemented,
   __fdev_not_implemented,
   dev_linux_sdcard_read,
   dev_linux_sdcard_write,
   dev_linux_sdcard_seek,
   dev_linux_sdcard_ioctl //ioctl
};


//
//#define DFLT_SDCARD_MEMORYSIZE 1024*1024/*2048*1024*/ //32*1024 //(32KB)
//2Go
#define DFLT_SDCARD_MEMORYSIZE   (2021654528)
static char* pmemory;
static unsigned long memory_size = DFLT_SDCARD_MEMORYSIZE;
static int fh=-1;

//static const char memory[DFLT_SDCARD_MEMORYSIZE]={0};
static int instance_counter=0;
//
extern int cyg_hal_sys_fsync(int desc);
/*============================================
| Implementation
==============================================*/
/*-------------------------------------------
| Name:dev_linux_sdcard_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_sdcard_load(void){
   return 0;
}

/*-------------------------------------------
| Name:dev_linux_sdcard_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_sdcard_open(desc_t desc, int o_flag){

   if(fh==-1) {

      if( (fh =
              cyg_hal_sys_open( ".sdcard0.o",_O_RDWR|_O_CREAT|_O_EXCL|_O_SYNC,_S_IREAD|
                                _S_IWRITE)) == -1 ) {
         if( (fh = cyg_hal_sys_open( ".sdcard0.o",_O_RDWR |_O_SYNC,_S_IREAD|_S_IWRITE )) == -1 )
            return -1;

         cyg_hal_sys_lseek( fh, 0, SEEK_SET );
      }else{
         int w=0;

         cyg_hal_sys_close(fh);
         if( (fh = cyg_hal_sys_open( ".sdcard0.o",_O_RDWR|_O_SYNC,_S_IREAD|_S_IWRITE)) == -1 )
            return -1;

         cyg_hal_sys_lseek(fh,0,SEEK_SET );
         cyg_hal_sys_lseek(fh,memory_size,SEEK_SET);
         //cyg_hal_sys_write(fh,memory,4);
      }
   }

   //
   if(o_flag & O_RDONLY) {
   }

   if(o_flag & O_WRONLY) {
   }

   instance_counter++;

   return 0;
}

/*-------------------------------------------
| Name:dev_linux_sdcard_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_sdcard_close(desc_t desc){

   if(fh==-1)
      return -1;

   instance_counter--;

   if(instance_counter<0) {
      instance_counter=0;
      cyg_hal_sys_close(fh);
      fh = -1;
   }

   return 0;
}

/*-------------------------------------------
| Name:dev_linux_sdcard_isset_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_sdcard_isset_read(desc_t desc){
   return -1;
}

/*-------------------------------------------
| Name:dev_linux_sdcard_isset_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_sdcard_isset_write(desc_t desc){
   return -1;
}
/*-------------------------------------------
| Name:dev_linux_sdcard_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_sdcard_read(desc_t desc, char* buf,int size){
   int r;
   if(ofile_lst[desc].offset>memory_size)
      return -1;

   cyg_hal_sys_lseek( fh, ofile_lst[desc].offset, SEEK_SET);
   r= cyg_hal_sys_read( fh,buf,size);
   ofile_lst[desc].offset = cyg_hal_sys_lseek( fh, 0, SEEK_CUR);

   return r;
}

/*-------------------------------------------
| Name:dev_linux_sdcard_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_sdcard_write(desc_t desc, const char* buf,int size){
   int w;
   if(ofile_lst[desc].offset>memory_size)
      return -1;

   cyg_hal_sys_lseek( fh, ofile_lst[desc].offset, SEEK_SET);
   w = cyg_hal_sys_write( fh,buf,size);
   cyg_hal_sys_fsync(fh);
   ofile_lst[desc].offset = cyg_hal_sys_lseek( fh, 0, SEEK_CUR);

   return w;
}

/*-------------------------------------------
| Name:dev_linux_sdcard_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_sdcard_seek(desc_t desc,int offset,int origin){
   if(ofile_lst[desc].offset>memory_size)
      return -1;

   ofile_lst[desc].offset = cyg_hal_sys_lseek( fh, offset, origin);

   return ofile_lst[desc].offset;
}

/*-------------------------------------------
| Name:dev_linux_sdcard_ioctl
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_sdcard_ioctl(desc_t desc,int request,va_list ap){
   switch(request) {

   case HDGETSZ: {
      long* hdsz_p= va_arg( ap, long*);
      if(!hdsz_p)
         return -1;

      *hdsz_p = memory_size;
   }
   break;

   case HDSETSZ: {
      int w=0;
      long hdsz= va_arg( ap, long);
      if(!hdsz)
         return -1;
      cyg_hal_sys_lseek(fh,0,SEEK_SET );
      memory_size = hdsz;

/*         if(( w =_lseek(fh,memory_size,SEEK_SET)) == -1 )
            printf( "sdcard0 creation failed" );
         else
            printf( "sdcard0 creation size %u bytes ok.\n", w );*/
   }
   break;
   //
   default:
      return -1;

   }

   return 0;
}

/*============================================
| End of Source  : dev_linux_sdcard.c
|---------------------------------------------
| Historic:
|---------------------------------------------
| Authors     | Date     | Comments
| $Log: not supported by cvs2svn $
==============================================*/

