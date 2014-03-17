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

/*!
   \option
      -i use input file.
      -b binary file.
      -f filter definition file

   \brief
      get or set value in binary file.
      by default use .btb filter file in /usr/share/btb.

   \use
      btb adj_loop.i_trip
      get value of adj_loop.i_trip parameter

      btb adj_loop.i_trip=12
      set value 12 to adj_loop.i_trip parameter.
*/

/*============================================
| Includes
==============================================*/
#include <stdlib.h>

#include "kernel/core/libstd.h"
#include "kernel/core/stat.h"
#include "kernel/core/statvfs.h"
#include "kernel/core/devio.h"
#include "kernel/core/time.h"
#include "kernel/core/ioctl_board.h"

#include "lib/libc/stdio/stdio.h"


/*============================================
| Global Declaration
==============================================*/

#define  OPT_MSK_R 0x01 //-r reset board
#define  OPT_MSK_S 0x02 //-s shutdown board
#define  OPT_MSK_P 0x04 //-p powersave board setting


/*============================================
| Implementation
==============================================*/
int shutdown_main(int argc,char* argv[]){
   int i;
   unsigned int opt=0;

   int fd = -1;
   if((fd=open("/dev/board",O_RDWR,0))<0) {
      printf("error: cannot open board\r\n");
      return -1;
   }

   // if no option
   if(argc<2) {
      if(ioctl(fd,BRDPWRDOWN,(void*)0)<0) {
         printf("error: force shutdown option not supported\r\n");
         return -1;
      }
      for(;; )
         usleep(1000);

      printf("error: force shutdown option not supported\r\n");
      return -1;
   }

   //get option
   for(i=1; i<argc; i++) {
      if(argv[i][0]=='-') {
         unsigned char c;
         unsigned char l=strlen(argv[i]);
         for(c=1; c<l; c++) {
            switch(argv[i][c]) {
            case 'r':
               opt |= OPT_MSK_R;
               //reset system
               if(ioctl(fd,BRDRESET,(void*)0)<0) {
                  printf("error: reset option not supported\r\n");
                  return -1;
               }
               for(;; )
                  usleep(1000);

               break;

            case 's': {
               int shtdn_opt=0;
               opt |= OPT_MSK_S;
               if(argv[i+1][0]=='-')
                  break;
               i++;
               if(argv[i]) {
                  shtdn_opt = atoi(argv[i]);
                  //
                  switch(shtdn_opt) {
                  case 0:     // Power Down Mode
                     if(ioctl(fd,BRDPWRDOWN,(void*)0)<0) {
                        printf("error: force shutdown option not supported\r\n");
                        return -1;
                     }
                     for(;; )
                        usleep(1000);
                     break;
                  case 1:     // Standby Mode
                     if(ioctl(fd,BRDSTANDBY,(void*)0)<0) {
                        printf("error: force standby option not supported\r\n");
                        return -1;
                     }
                     printf("standby mode on ok\r\n");
                     break;
                  default:
                     if(ioctl(fd,BRDPWRDOWN,(void*)0)<0) {
                        printf("error: force shutdown option not supported\r\n");
                        return -1;
                     }
                     for(;; )
                        usleep(1000);
                     break;
                  }
               }
            }
            break;

            case 'p': {  //power save
               int pwrsv_timeout=0;    //unit minute
               opt |= OPT_MSK_P;
               if(argv[i+1][0]=='-')
                  break;
               i++;
               if(argv[i]) {
                  pwrsv_timeout = atoi(argv[i]);
                  //
                  if(pwrsv_timeout>0) {
                     if(ioctl(fd,BRDPWRSAVEOFF,(void*)0)<0) {
                        printf("error: power save off not supported\r\n");
                        return -1;
                     }

                     if(ioctl(fd,BRDPWRSAVESET,pwrsv_timeout)<0) {
                        printf("error: power save set timeout not supported\r\n");
                        return -1;
                     }

                     if(ioctl(fd,BRDPWRSAVEON,(void*)0)<0) {
                        printf("error: power save on not supported\r\n");
                        return -1;
                     }
                     printf("power save on ok\r\n");
                  }else if(!pwrsv_timeout) {
                     if(ioctl(fd,BRDPWRSAVEOFF,(void*)0)<0) {
                        printf("error: power save off not supported\r\n");
                        return -1;
                     }
                     printf("power save off ok\r\n");
                  }else
                     return -1;
               }else{
                  if(ioctl(fd,BRDPWRSAVEON,(void*)0)<0) {
                     printf("error: power save on not supported\r\n");
                     return -1;
                  }
                  printf("power save on ok\r\n");
               }
            }
            break;

            default:
               printf("error: unknown option\r\n");
               break;


            }
         }
      }else{
         if(!argv[i])
            return -1;
      }
   }
   return 0;
}


/*============================================
| End of Source  : shutdown.c
==============================================*/
