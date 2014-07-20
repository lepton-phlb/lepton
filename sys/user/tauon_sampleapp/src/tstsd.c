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
//libraries provide by eCos
#include <stdlib.h>
#include <string.h>

//user libraries
#include "kernel/core/signal.h"
#include "kernel/core/wait.h"
#include "kernel/core/stat.h"
#include "kernel/core/libstd.h"
#include "kernel/core/devio.h"
#include "kernel/core/time.h"
#include "kernel/core/select.h"
#include "kernel/core/ioctl_hd.h"

#include "lib/libc/termios/termios.h"
#include "lib/libc/stdio/stdio.h"
/*============================================
| Global Declaration
==============================================*/


/*============================================
| Implementation
==============================================*/
#define BLOCK_SIZE   512
#define MAX_BUF_SIZE 4096

#define MIN(__x__, __y__)  ((__x__)>(__y__)) ? (__y__) : (__x__)
#define MAX(__x__, __y__)  ((__x__)>(__y__)) ? (__x__) : (__y__)

#if defined(CPU_GNU32)
   #define SD_DEV       "/dev/sdcard0"
#else
   #if defined(CPU_ARM9)
      #define SD_DEV       "/dev/hd/sd0"
   #else //CORTEXM
      #define SD_DEV       "/dev/hd/sdhc0"
   #endif
#endif

static char buf_wr[MAX_BUF_SIZE];
static char buf_rd[MAX_BUF_SIZE];

#define BUF_SIZE_VALUES_NB_TEST2    8
static unsigned int tstsd_buffer_size_value[BUF_SIZE_VALUES_NB_TEST2] = {
   15,
   2048,
   511,
   3051,
   46,
   127,
   728,
   30
};

static unsigned int tstsd_max_time_wr[BUF_SIZE_VALUES_NB_TEST2];
static unsigned int tstsd_min_time_wr[BUF_SIZE_VALUES_NB_TEST2];
static unsigned int tstsd_max_time_rd[BUF_SIZE_VALUES_NB_TEST2];
static unsigned int tstsd_min_time_rd[BUF_SIZE_VALUES_NB_TEST2];

static unsigned int tstsd_diff_time(unsigned int  *time_memo);

/*============================================
| Implementation
==============================================*/
/*--------------------------------------------
| Name:        tstsd_diff_time
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
unsigned int tstsd_diff_time(unsigned int  *time_memo) {
   unsigned int time_cur;
   unsigned int val;
   struct    timespec tp;

   clock_gettime(CLOCK_MONOTONIC, &tp);
   time_cur = tp.tv_sec*1000L + tp.tv_nsec/1000000L;      //ms

   // test if overflow
   if (time_cur < (*time_memo))
      val = ((1000 -  (*time_memo)) + time_cur);
   else
      val = (time_cur - (*time_memo));

   clock_gettime(CLOCK_MONOTONIC, &tp);
   *time_memo = (tp.tv_sec*1000L + tp.tv_nsec/1000000L);

   return val;
}

/*--------------------------------------------
| Name:        tstsd_main
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int tstsd_main(int argc,char* argv[]) {
   int fd=-1;
   unsigned long sd_size = 0;
   char c='e';
   int cb_wr = 0;
   int cb_rd = 0;
   unsigned int tm_wr=0, tm_rd=0;
   int r,w;
   int count_rd=0, count_wr=0;
   unsigned long total_wr = 0;
   unsigned int index = 0;
   unsigned int diff_wr = 0, diff_rd = 0;
   unsigned char ok = 1;
   time_t begin;
   unsigned int i=0;

   //
   if((fd = open(SD_DEV, O_RDWR, 0)) < 0) {
      fprintf(stderr,"(EE) Can't open %s\r\n", SD_DEV);
      return -1;
   }

   //
   ioctl(fd, HDGETSZ, &sd_size);
   fprintf(stderr, "%d bytes\r\n", sd_size);

   srand(0);
   ///
   sd_size = 20*1024*1024;
   ///

   begin = time(NULL);
   while((total_wr += cb_wr) <= sd_size) {
      index = rand() % BUF_SIZE_VALUES_NB_TEST2;

      //write
      memset((void *)buf_wr, c, tstsd_buffer_size_value[index]);
      tstsd_diff_time(&tm_wr);

      w = 0;
      cb_wr = 0;
      while((cb_wr+=w) < tstsd_buffer_size_value[index]) {
         if((w=write(fd, buf_wr+cb_wr, tstsd_buffer_size_value[index]-cb_wr))<=0) {
            close(fd);
            fprintf(stderr, "close cb_wr : %d [%d]\r\n", cb_wr, total_wr);
            ok = 0;
            break;
         }
         count_wr++;
      }

      diff_wr = tstsd_diff_time(&tm_wr);
      if(!ok) {
         break;
      }

      tstsd_max_time_wr[index] = MAX(tstsd_max_time_wr[index], diff_wr);
      tstsd_min_time_wr[index] =
         (tstsd_min_time_wr[index] == 0) ? diff_wr : tstsd_min_time_wr[index];
      tstsd_min_time_wr[index] = MIN(tstsd_min_time_wr[index], diff_wr);

      printf("%d) WR %c (%d) %d ms\r\n", count_wr, c, cb_wr, diff_wr);
      lseek(fd, -cb_wr, SEEK_CUR);

      //read
      memset((void *)buf_rd, 0, MAX_BUF_SIZE);
      tstsd_diff_time(&tm_rd);

      r=0;
      cb_rd=0;
      while((cb_rd+=r) < cb_wr) {
         if((r=read(fd, buf_rd+cb_rd, cb_wr-cb_rd)) <= 0) {
            close(fd);
            fprintf(stderr, "[close] cb_rd : %d r : %d [%d]\r\n", cb_rd, r, total_wr);
            ok = 0;
            break; //return -1;
         }
         count_rd++;
      }

      diff_rd = tstsd_diff_time(&tm_rd);
      if(!ok) {
         break;
      }

      tstsd_max_time_rd[index] = MAX(tstsd_max_time_rd[index], diff_rd);
      tstsd_min_time_rd[index] =
         (tstsd_min_time_rd[index] == 0) ? diff_rd : tstsd_min_time_rd[index];
      tstsd_min_time_rd[index] = MIN(tstsd_min_time_rd[index], diff_rd);

      printf("%d] rd (%d) %ld ms\r\n", count_rd, cb_rd, diff_rd);

      //compare buffer
      if(memcmp((const void *)buf_wr, (const void *)buf_rd, cb_rd)) {
         fprintf(stderr, "(EE) buf_wr != buf_rd [%d]\r\n", total_wr);
         ok = 0;
         break;
      }

      if(++c>'z') {
         c='a';
      }
   }

   close(fd);

   fprintf(stderr, "%d bytes written in %d sec\r\n", total_wr, (int)difftime(time(NULL), begin));

   for(; i<BUF_SIZE_VALUES_NB_TEST2; i++) {
      fprintf(stderr, "%d bytes\r\n", tstsd_buffer_size_value[i]);
      fprintf(stderr, "write min:%d - MAX:%d ms\r\n", tstsd_min_time_wr[i], tstsd_max_time_wr[i]);
      fprintf(stderr, "read  min:%d - MAX:%d ms\r\n", tstsd_min_time_rd[i], tstsd_max_time_rd[i]);
      fprintf(stderr, "-------------\r\n");
   }
   return 0;
}

/*============================================
| End of Source  : tstsd.c
|---------------------------------------------
| Historic:
|---------------------------------------------
| Authors     | Date     | Comments
| $Log:$
==============================================*/
