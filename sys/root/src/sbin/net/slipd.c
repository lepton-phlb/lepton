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

#include "kernel/core/libstd.h"
#include "kernel/core/stat.h"
#include "kernel/core/statvfs.h"
#include "kernel/core/devio.h"
#include "kernel/core/time.h"
#include "kernel/core/ioctl.h"
#include "kernel/core/ioctl_if.h"
#include "kernel/core/stropts.h"

#include "lib/libc/termios/termios.h"
#include "lib/libc/stdio/stdio.h"

/*============================================
| Global Declaration
==============================================*/
#define OPT_MSK_W 0x01   //-w wait windows synchro "CLIENTCLIENTCLIENT"
#define OPT_MSK_V 0x02   //-v verbose
#define OPT_MSK_S 0x02   //-s speed

#define DFLT_SPEED 9600L

const char str_synchro_from_pc[]="CLIENT";
const char str_synchro_to_pc[]="CLIENTSERVER\0";

/*============================================
| Implementation
==============================================*/
/*--------------------------------------------
| Name:        slipd_windows_synchro
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int slipd_windows_synchro(int fdin,int fdout, int verbose){
   char rcv_buf[16];
   char c='0';
   int j=0;
   int cb=sizeof(str_synchro_from_pc);
   while((cb=read(fdin,rcv_buf,sizeof(rcv_buf)))>0) {
      int i;

      for(i=0; i<cb; i++) {
         if( (j=(rcv_buf[i]==str_synchro_from_pc[j]?(j+1):0))==(sizeof(str_synchro_from_pc)-1) ){
            return ((write(fdout,str_synchro_to_pc,(sizeof(str_synchro_to_pc)-1))>0 ? 0 : -1));
         }
      }
   }

   return -1;
}

/*--------------------------------------------
| Name:        slipd_main
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int slipd_main(int argc,char* argv[]){
   int fd1[2];
   int fd2;
   int i;
   int verbose=0;
   unsigned int opt=0;
   char* dev=(char*)0;
   int cb=0;
   int pkt=0;

   struct if_config_st if_config;
   struct termios tty_termios;
   speed_t speed=DFLT_SPEED;


   for(i=1; i<argc; i++) {
      if(argv[i][0]=='-') {
         unsigned char c;
         unsigned char l=strlen(argv[i]);
         for(c=1; c<l; c++) {
            switch(argv[i][c]) {

            //
            case 'w': {
               opt |= OPT_MSK_W;
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
            case 's': {
               opt |= OPT_MSK_S;
               if((i+1) == argc) {  //not enough parameter
                  printf("error: //not enough parameter\r\n");
                  return 0;
               }
               i++;
               if(!argv[i]) {
                  verbose=1;
                  break;
               }

               speed=atol(argv[i]);

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
      if((fd1[0] = open("/dev/net/slip",O_RDWR,0))<0) {
         printf("error: cannot open device /dev/slip \r\n");
         return 0;
      }

      fd1[1]=fd1[0];

      //
      if((fd2 = open(dev,O_RDWR,0))<0) {
         printf("error: cannot open device %s\r\n",dev);
         return 0;
      }

      //set speed
      if(opt&OPT_MSK_S) {
         tcgetattr(fd2, &tty_termios);

         /* set baudrate */
         cfsetspeed(&tty_termios, speed);

         /* apply the options back to the port */
         tcsetattr(fd2, TCSANOW, &tty_termios);
      }

      //wait windows slip synchro
      if(opt&OPT_MSK_W)
         if(slipd_windows_synchro(fd2,fd2,verbose)<0)
            return -1;

      //link devices
      ioctl(fd1[0],I_LINK,fd2);

      fattach(fd1[0],"/dev/net/sl0");

      close(fd1[0]);
      close(fd1[1]);
      close(fd2);

      if((fd2=open("/dev/net/ip",O_RDWR,0))<0) {
         printf("error: cannot open ip stack\r\n");
         return 0;
      }

      if((fd1[0]=open("/dev/net/sl0",O_RDONLY,0))<0) {
         printf("error: cannot slip device in read mode\r\n");
         return 0;
      }

      if((fd1[1]=open("/dev/net/sl0",O_WRONLY,0))<0) {
         printf("error: cannot slip device in write mode\r\n");
         return 0;
      }

      //link devices
      ioctl(fd2,I_LINK,fd1[0]);
      //link devices
      ioctl(fd2,I_LINK,fd1[1]);

      fattach(fd2,"/dev/net/s0");

      close(fd1[0]);
      close(fd1[1]);
      close(fd2);

      if((fd2=open("/dev/net/s0",O_RDWR,0))<0) {
         printf("error: cannot open pseudo net device\r\n");
         return 0;
      }

      strcpy(if_config.gw_ip_addr,"128.1.1.2");
      strcpy(if_config.if_ip_addr,"128.1.1.3");
      strcpy(if_config.if_net_msk,"255.255.0.0");
      if_config.if_flags=IFF_POINTTOPOINT|IFF_UP;

      ioctl(fd2,IFSETCFG,&if_config);

   }else{
      //
      verbose = 0;
      //
      if((fd1[0] = open("/dev/slip",O_RDWR,0))<0) {
         printf("error: cannot open device /dev/slip \r\n");
         return 0;
      }

      if(slipd_windows_synchro(0,1,verbose)<0)
         return -1;

      fd1[1]=fd1[0];


      //link devices
      ioctl(fd1[0],I_LINK,0);
      ioctl(fd1[1],I_LINK,1);

   }


   return 0;
}

/*============================================
| End of Source  : slipd.c
==============================================*/
