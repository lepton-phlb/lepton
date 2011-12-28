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
#include <string.h>
#include "kernel/core/kernel.h"
#include "kernel/core/system.h"
#include "kernel/core/stat.h"
#include "kernel/core/ioctl_hd.h"
#include "kernel/fs/vfs/vfsdev.h"

#include "kernel/dev/dev_cpufs/dev_cpufs.h"

const char dev_cpufs_name[]="hda\0cpufs\0";

int dev_cpufs_load(void);
int dev_cpufs_open(desc_t desc, int o_flag);
int dev_cpufs_close(desc_t desc);
int dev_cpufs_isset_read(desc_t desc);
int dev_cpufs_isset_write(desc_t desc);
int dev_cpufs_read(desc_t desc, char* buf,int size);
int dev_cpufs_write(desc_t desc, const char* buf,int size);
int dev_cpufs_seek(desc_t desc,int offset,int origin);

dev_map_t dev_cpufs_map={
   dev_cpufs_name,
   S_IFBLK,
   dev_cpufs_load,
   dev_cpufs_open,
   dev_cpufs_close,
   __fdev_not_implemented,
   __fdev_not_implemented,
   dev_cpufs_read,
   dev_cpufs_write,
   dev_cpufs_seek,
   __fdev_not_implemented //ioctl
};





/*===========================================
Implementation
=============================================*/

/*-------------------------------------------
| Name:dev_cpufs_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_cpufs_load(void){
   return 0;
}

/*-------------------------------------------
| Name:dev_cpufs_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_cpufs_open(desc_t desc, int o_flag){

   //
   if(o_flag & O_RDONLY){
   }

   if(o_flag & O_WRONLY){
   }

   ofile_lst[desc].offset=0;

   return 0;
}

/*-------------------------------------------
| Name:dev_cpufs_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_cpufs_close(desc_t desc){
   return 0;
}

/*-------------------------------------------
| Name:dev_cpufs_isset_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_cpufs_isset_read(desc_t desc){
  return -1;
}

/*-------------------------------------------
| Name:dev_cpufs_isset_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_cpufs_isset_write(desc_t desc){
      return -1;
}
/*-------------------------------------------
| Name:dev_cpufs_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_cpufs_read(desc_t desc, char* buf,int size){
   unsigned int pos = ofile_lst[desc].offset;

   //profiler
   __io_profiler_start(desc);

   if((pos+size)>=filecpu_memory_size)
      size -= (pos+size)-filecpu_memory_size;

   memcpy(buf,&filecpu_memory[pos],size);

   ofile_lst[desc].offset+=size;

   //profiler
   __io_profiler_stop(desc);
   __io_profiler_add_result(desc,O_RDONLY,size,__io_profiler_get_counter(desc));
   
   return size;
}

/*-------------------------------------------
| Name:dev_cpufs_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_cpufs_write(desc_t desc, const char* buf,int size){
   //read only mode
   return -1;
}

/*-------------------------------------------
| Name:dev_cpufs_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_cpufs_seek(desc_t desc,int offset,int origin){

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
| Name:dev_fileflash_ioctl
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_cpufs_ioctl(desc_t desc,int request,va_list ap){
   switch(request){

      case HDGETSZ:{
         long* hdsz_p= va_arg( ap, long*);
         if(!hdsz_p)
            return -1;
         *hdsz_p = filecpu_memory_size;
      }
      case HDSETSZ:{
        return -1;
      }
      break;
      //
      default:
         return -1;

   }

   return 0;
}

/*===========================================
End of Source dev_cpufs.c
=============================================*/
