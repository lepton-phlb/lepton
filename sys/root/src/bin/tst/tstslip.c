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

#include "kernel/libstd.h"
#include "kernel/stat.h"
#include "kernel/statvfs.h"
#include "kernel/devio.h"
#include "kernel/time.h"
#include "kernel/ioctl.h"

#include "termios/termios.h"

#include "stdio/stdio.h"


/*============================================
| Global Declaration
==============================================*/

#define OPT_MSK_S 0x01   //-s start to send
#define OPT_MSK_V 0x02   //-v verbose


const unsigned char snd_buf[11]=
{0x30+0x00,0x30+0x00,0x30+0x01,0x30+0x02,0x30+0x03,0x30+0x04,0x30+0x05,0x30+0x06,0x30+0x07,0x30+
 0x08,
 0x30+0x09};

/*============================================
| Implementation
==============================================*/

/*--------------------------------------------
| Name:        tstslip_main
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int tstslip_main(int argc,char* argv[]){
   int fd1[2];
   int fd2;
   int i;
   int verbose=0;
   unsigned int opt=0;
   char* dev=(char*)0;
   int cb=0;
   int pkt=0;

   char rcv_buf[256];

   for(i=1; i<argc; i++) {
      if(argv[i][0]=='-') {
         unsigned char c;
         unsigned char l=strlen(argv[i]);
         for(c=1; c<l; c++) {
            switch(argv[i][c]) {

            //
            case 's': {
               opt |= OPT_MSK_S;
               if((i+1) == argc) {  //not enough parameter
                  printf("error: not enough parameter\r\n");
                  return 0;
               }
            }
            break;

            //
            case 'v': {
               opt |= OPT_MSK_V;
               if((i+1) == argc) {  //not enough parameter
                  printf("error: //not enough parameter\r\n");
                  return 0;
               }
               i++;
               if(!argv[i]) {
                  verbose=1;
                  break;
               }

               verbose=atoi(argv[i]);
            }
            break;
            //
            default:
               break;
            } //switch
         } //for
      }else{

         if(!argv[i]) {
            dev=(char*)0;
            break;
         }

         dev = argv[i];
      }

   } //for


   //
   if(dev!=(char*)0) {
      //
      if((fd1[0] = open("/dev/slip",O_RDWR,0))<0) {
         printf("error: cannot open device /dev/slip \r\n");
         return 0;
      }

      fd1[1]=fd1[0];

      //

      if((fd2 = open(dev,O_RDWR,0))<0) {
         printf("error: cannot open device %s\r\n",dev);
         return 0;
      }

      //link devices
      ioctl(fd1[0],I_LINK,fd2);
   }else{
      //
      if((fd1[0] = open("/dev/slip",O_RDONLY,0))<0) {
         printf("error: cannot open device /dev/slip \r\n");
         return 0;
      }

      if((fd1[1] = open("/dev/slip",O_WRONLY,0))<0) {
         printf("error: cannot open device /dev/slip \r\n");
         return 0;
      }

      //link devices
      ioctl(fd1[0],I_LINK,0);
      ioctl(fd1[1],I_LINK,1);

      verbose = 0;
   }

   //
   if(opt&OPT_MSK_S) {
      usleep(10000000);
      write(fd1[1],snd_buf,sizeof(snd_buf));
      /*write(fd1[1],snd_buf,sizeof(snd_buf));
      write(fd1[1],snd_buf,sizeof(snd_buf));
      write(fd1[1],snd_buf,sizeof(snd_buf));*/
   }

   while((cb=read(fd1[0],rcv_buf,sizeof(rcv_buf)))>0) {
      if((cb!=sizeof(snd_buf)) || memcmp(&rcv_buf[1],&snd_buf[1],sizeof(snd_buf)-1)) {
         if(verbose)
            printf("error:\r\n");
         continue;
      }

      rcv_buf[0]-=0x30;
      rcv_buf[0]=(++(rcv_buf[0]))%9;
      rcv_buf[0]+=0x30;
      if(verbose) {
         printf("(%06d)[%02x]",pkt++,rcv_buf[0]);
         for(i=1; i<cb; i++)
            printf(":%02x",rcv_buf[i]);
         printf("\r\n");
      }
      write(fd1[1],rcv_buf,cb);
      //usleep(1000);
      memset(rcv_buf,0,sizeof(rcv_buf)+2);
   }

   return 0;
}
/*============================================
| End of Source  : tstslip.c
==============================================*/
