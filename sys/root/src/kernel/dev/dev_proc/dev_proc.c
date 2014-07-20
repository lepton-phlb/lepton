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
#include <string.h>
#include "kernel/core/system.h"
#include "kernel/core/kernel.h"
#include "kernel/core/process.h"
#include "kernel/core/stat.h"
#include "kernel/fs/vfs/vfsdev.h"
#include "kernel/core/ioctl_proc.h"

#include "kernel/dev/dev_proc/dev_proc.h"

/*===========================================
Global Declaration
=============================================*/
const char dev_proc_name[]="proc\0";

int dev_proc_load(void);
int dev_proc_open(desc_t desc, int o_flag);
int dev_proc_close(desc_t desc);
int dev_proc_isset_read(desc_t desc);
int dev_proc_isset_write(desc_t desc);
int dev_proc_read(desc_t desc, char* buf,int size);
int dev_proc_write(desc_t desc, const char* buf,int size);
int dev_proc_seek(desc_t desc,int offset,int origin);
int dev_proc_ioctl(desc_t desc,int request,va_list ap);

dev_map_t dev_proc_map={
   dev_proc_name,
   S_IFBLK,
   dev_proc_load,
   dev_proc_open,
   dev_proc_close,
   __fdev_not_implemented,
   __fdev_not_implemented,
   dev_proc_read,
   dev_proc_write,
   dev_proc_seek,
   dev_proc_ioctl //ioctl
};

// idle task stat.
#define ISTATSIZE (1<<2)
typedef struct {
   unsigned long min;
   unsigned long max;
   unsigned long store[ISTATSIZE];
   char rd,wr;
} dev_proc_istat_t;

dev_proc_istat_t dev_proc_istat = {   // global val init @ reset with max =0 & min = MAX long Value
   0xffffffff,
};
// idle task stat.

#define PROC_BUF_MAX  sizeof(process_t)

#define LIMIT_PROCESS_STAT ( PROCESS_MAX * PROC_BUF_MAX )
#define LIMIT_KERNEL_STAT  ( LIMIT_PROCESS_STAT + 1 + \
                             (_SYSCALL_TOTAL_NB*sizeof(kernel_profiler_result_t)) )
#define LIMIT_IO_STAT      ( LIMIT_KERNEL_STAT+1+(sizeof(io_profiler_result_t)*max_dev) )

/*===========================================
Implementation
=============================================*/
/*-------------------------------------------
| Name:dev_proc_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_proc_load(void){
   return 0;
}

/*-------------------------------------------
| Name:dev_proc_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_proc_open(desc_t desc, int o_flag){

   //
   if(o_flag & O_RDONLY) {
      ofile_lst[desc].size = LIMIT_PROCESS_STAT;
   }

   if(o_flag & O_WRONLY) {
   }

   ofile_lst[desc].offset=0;


   return 0;
}

/*-------------------------------------------
| Name:dev_proc_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_proc_close(desc_t desc){
   return 0;
}

/*-------------------------------------------
| Name:dev_proc_isset_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_proc_isset_read(desc_t desc){
   return -1;
}

/*-------------------------------------------
| Name:dev_proc_isset_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_proc_isset_write(desc_t desc){
   return -1;
}
/*-------------------------------------------
| Name:dev_proc_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_proc_read(desc_t desc, char* buf,int size){
   int i;


   if(ofile_lst[desc].offset<LIMIT_PROCESS_STAT) {
      if(size<PROC_BUF_MAX)
         return -1;

      i = (ofile_lst[desc].offset / PROC_BUF_MAX)+1;

      if(i>PROCESS_MAX)
         return -1;

      if(process_lst[i]) {
         memcpy(buf,process_lst[i],PROC_BUF_MAX);
         size = PROC_BUF_MAX;
      }else{
         size = 0;
      }

      //
      i++;
      ofile_lst[desc].offset = (i-1)*PROC_BUF_MAX;
      return size;
   }
#ifdef KERNEL_PROFILER
   else if( (ofile_lst[desc].offset>LIMIT_PROCESS_STAT)
            && ofile_lst[desc].offset< LIMIT_KERNEL_STAT) {

      if(size<sizeof(kernel_profiler_result_t))
         return -1;

      i = (ofile_lst[desc].offset-(LIMIT_PROCESS_STAT+1)) / sizeof(kernel_profiler_result_t);

      memcpy(buf,&kernel_profiler_result_lst[i],size);

      ofile_lst[desc].offset+=sizeof(kernel_profiler_result_t);
      return size;

   }else if( (ofile_lst[desc].offset>LIMIT_KERNEL_STAT)
             && ofile_lst[desc].offset< LIMIT_IO_STAT) {

      if(size<sizeof(io_profiler_result_t))
         return -1;

      i = ( ofile_lst[desc].offset- (LIMIT_KERNEL_STAT+1) ) / sizeof(io_profiler_result_t);

      memcpy(buf,&io_profiler_result_lst[i],size);

      ofile_lst[desc].offset+=sizeof(io_profiler_result_t);
      return size;

   }
#endif

   return -1;
}

/*-------------------------------------------
| Name:dev_proc_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_proc_write(desc_t desc, const char* buf,int size){
   //read only mode
   return -1;
}

/*-------------------------------------------
| Name:dev_proc_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_proc_seek(desc_t desc,int offset,int origin){

   switch(origin) {

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
| Name:dev_ttys1_ioctl
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_proc_ioctl(desc_t desc,int request,va_list ap){

   switch(request) {
   case PROCSTAT:
      ofile_lst[desc].offset=0;

      break;

#ifdef KERNEL_PROFILER
   case PROCSCHEDSTAT:
      ofile_lst[desc].offset=(LIMIT_PROCESS_STAT+1);
      break;

   case PROCIOSTAT:
      ofile_lst[desc].offset=(LIMIT_KERNEL_STAT+1);
      break;
#endif

   //
   default:
      return -1;

   }

   return 0;
}

/*-------------------------------------------
| Name:dev_proc_istat_wr
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void dev_proc_istat_wr (unsigned long cycles)
{
   unsigned long last;
   last = dev_proc_istat.store[dev_proc_istat.wr];          // read old val

   dev_proc_istat.wr = (dev_proc_istat.wr+1) & ((char)(ISTATSIZE-1)); // pr� increment
   dev_proc_istat.store[dev_proc_istat.wr] = cycles;         // store
   cycles -= last;
   if (cycles < dev_proc_istat.min ) dev_proc_istat.min = cycles;
   if (cycles > dev_proc_istat.max ) dev_proc_istat.max = cycles;
}

/*===========================================
End of Source dev_proc.c
=============================================*/

