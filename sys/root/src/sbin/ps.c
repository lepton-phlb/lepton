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
#include <stdlib.h>
#include <string.h>
#include "kernel/core/kernel.h"
#include "kernel/core/process.h"
#include "kernel/core/libstd.h"
#include "kernel/core/stat.h"
#include "kernel/core/statvfs.h"
#include "kernel/core/devio.h"
#include "kernel/core/ioctl_proc.h"
#include "kernel/core/time.h"

#include "lib/libc/stdio/stdio.h"


/*===========================================
Global Declaration
=============================================*/

#define OPT_MSK_V       0x01   //-v verbose
#define OPT_MSK_VSYS    0x02   //-v sys
#define OPT_MSK_VIO     0x04   //-v io

const char __STAT_SYS[] ="sys";
const char __STAT_IO[]  ="io";

//
const char __PID[]      ="PID";
const char __PPID[]     ="PPID";
const char __PGID[]     ="PGID";
const char __STIME[]    ="STIME";
const char __COMMAND[]  ="COMMAND";


/*===========================================
Implementation
=============================================*/
int ps_main(int argc,char* argv[]){
   process_t process_info;

#ifdef KERNEL_PROFILER
   kernel_profiler_result_t kernel_profiler_result;
   io_profiler_result_t io_profiler_result;
#endif
   int fd;
   int cb;
   int i;
   unsigned int opt=0;
#if defined(__KERNEL_UCORE_ECOS)
   int pid, ppid, pgid;
#endif
   //get option
   for(i=1; i<argc; i++) {
      if(argv[i][0]=='-') {
         unsigned char c;
         unsigned char l=strlen(argv[i]);
         for(c=1; c<l; c++) {
            switch(argv[i][c]) {
            case 'v':
               opt |= OPT_MSK_V;
               break;
            }
         }
      }else{
         if(!argv[i])
            return -1;

         if(opt&OPT_MSK_V) {
            if(!strcmp(argv[i],__STAT_SYS))
               opt |= OPT_MSK_VSYS;
            else if(!strcmp(argv[i],__STAT_IO))
               opt |= OPT_MSK_VIO;
            continue;
         }
      }
   }

   if((opt&OPT_MSK_V) && !(opt&(OPT_MSK_VIO|OPT_MSK_VSYS)) )
      opt |= (OPT_MSK_VIO|OPT_MSK_VSYS);

   if( (fd = open("/dev/proc",O_RDONLY,0))<0)
      return -1;

   if(ioctl(fd,PROCSTAT,(void*)0)<0)
      return 0;

   printf("  %4s %4s %4s  %8s   %.32s\r\n",__PID,__PPID,__PGID,__STIME,__COMMAND);

   while( (cb=read(fd,&process_info,sizeof(process_info)))>=0) {
      char cbuf[26];
      if(!cb) continue;

#if defined(__KERNEL_UCORE_ECOS)
      pid = (int)process_info.pid;
      ppid = (int)process_info.ppid;
      pgid = (int)process_info.pgid;
      printf("%4d %4d %4d  %.8s   %.32s\r\n",
             pid,
             ppid,
             pgid,
             ctime_r(&process_info.start_time,cbuf)+ 11,
             process_info.argv[0]);
#else
      printf("%c %4d %4d %4d  %.8s   %.32s\r\n", ((process_info.pthread_ptr->stat & PTHREAD_STATUS_ZOMBI)? 'Z' : ' '),
             process_info.pid,
             process_info.ppid,
             process_info.pgid,
             ctime_r(&process_info.start_time,cbuf)+ 11,
             process_info.argv[0]);
#endif
   }

#ifdef KERNEL_PROFILER
   #if defined(EVAL_BOARD) //eval board just for display all ps informations on small screen.
   int nb_syscall=5;
   #endif
   // kernel syscall statistics
   if(opt&(OPT_MSK_VSYS)) {
      if(ioctl(fd,PROCSCHEDSTAT,(void*)0)<0)
         return 0;

      printf("\r\nkernel syscall statistics\r\n");

   #if defined(EVAL_BOARD)
      printf("%-12s %-5s\r\n","syscall name","time (s)");
      while( nb_syscall-- &&(cb=read(fd,&kernel_profiler_result,sizeof(kernel_profiler_result_t)))>=0){
         unsigned int counter =kernel_profiler_result.counter;
      	printf("%-12s %10f\r\n",kernel_profiler_result.pname,(float)(kernel_profiler_result.counter)*PROFILER_PERIOD);
      }
   #else
      printf("%-24s %-10s\r\n","syscall name","time (s)");
      while( (cb=read(fd,&kernel_profiler_result,sizeof(kernel_profiler_result_t)))>=0) {
         unsigned int counter =kernel_profiler_result.counter;
            printf("%-24s %10f\r\n",kernel_profiler_result.pname,(float)(kernel_profiler_result.counter)*PROFILER_PERIOD);
      }
   #endif
   }

   //io statistics
   if(opt&(OPT_MSK_VIO)) {
      if(ioctl(fd,PROCIOSTAT,(void*)0)<0)
         return 0;

      printf("\r\nio statistics\r\n");

      printf("%-8s io %10s %-6s %-10s %-10s %-10s %-10s\r\n","dev","nb","size","time (s)","rate avg","rate max ","rate min");
      while( (cb=read(fd,&io_profiler_result,sizeof(io_profiler_result_t)))>=0) {
         int l = (sizeof(int)==sizeof(long));

         if(io_profiler_result.pname) {
            unsigned int counter    = io_profiler_result.counter[O_RDONLY];
            long nbacces  = io_profiler_result.nbacces[O_RDONLY];
            unsigned int size     = io_profiler_result.size[O_RDONLY];
            printf("%-8s RD %10.0f %6u %10f %10.1f %10.1f %10.1f\r\n",io_profiler_result.pname,
                   (float)nbacces,
                   size,
                   (counter)*PROFILER_PERIOD,
                   io_profiler_result.avg[O_RDONLY]/(float)1024.0,
                   io_profiler_result.rate_max[O_RDONLY]/(float)1024.0,
                   io_profiler_result.rate_min[O_RDONLY]/(float)1024.0);
         }
         if(io_profiler_result.pname) {
            unsigned int counter    = io_profiler_result.counter[O_WRONLY];
            long nbacces  = io_profiler_result.nbacces[O_WRONLY];
            unsigned int size     = io_profiler_result.size[O_WRONLY];
            printf("%-8s WR %10.0f %6u %10f %10.1f %10.1f %10.1f\r\n",io_profiler_result.pname,
                   (float)nbacces,
                   size,
                   (counter)*PROFILER_PERIOD,
                   io_profiler_result.avg[O_WRONLY]/(float)1024.0,
                   io_profiler_result.rate_max[O_WRONLY]/(float)1024.0,
                   io_profiler_result.rate_min[O_WRONLY]/(float)1024.0);
         }
      }
      printf("\r\n size: Bytes\r\n rate: KBytes/s\r\n");
   }
#endif



   return 0;
}


/*===========================================
End of Sourceps.c
=============================================*/
