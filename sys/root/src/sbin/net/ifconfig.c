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
#include <string.h>

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



/*============================================
| Implementation
==============================================*/
/*--------------------------------------------
| Name:        chk_ipv4_addr
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
static int chk_ipv4_addr(char* a_ip_addr){
   int ip_addr_1=-1;
   int ip_addr_2=-1;
   int ip_addr_3=-1;
   int ip_addr_4=-1;
   //interface gateway ip address
   sscanf(a_ip_addr,"%d.%d.%d.%d",&ip_addr_1,&ip_addr_2,&ip_addr_3,&ip_addr_4);

   if(ip_addr_1> -1 && ip_addr_1<256
      && ip_addr_2> -1 && ip_addr_2<256
      && ip_addr_3> -1 && ip_addr_3<256
      && ip_addr_4> -1 && ip_addr_4<256)
      return 0;

   return -1;
}

/*--------------------------------------------
| Name:        ifconfig_main
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int ifconfig_main(int argc,char* argv[]){
   int i;
   unsigned int opt=0;
   struct if_config_st if_config;
   char* if_name = (char*)0;

   int fd1[2];
   int fd2;

   //
   for(i=1; i<argc; i++) {
      /*if(i==1){
         if_name = argv[i];
         continue;
      }else*/if( !strcmp(argv[i],"addif") ) {
         //get if ip addr
         if(++i==argc)
            return -1;
         if(chk_ipv4_addr(argv[i])<0)
            return -1;
         strcpy(if_config.if_ip_addr,argv[i]);

         //get if gateway ip addr
         if(++i<argc) {
            if(chk_ipv4_addr(argv[i])<0)
               return -1;
            strcpy(if_config.gw_ip_addr,argv[i]);
         }else{
            strcpy(if_config.gw_ip_addr,if_config.if_ip_addr);
         }

         //default netmask
         strcpy(if_config.if_net_msk,"255.255.0.0");
         //
         if_config.if_flags=IFF_BROADCAST|IFF_ADD|IFF_UP;
      }else if( !strcmp(argv[i],"netmask") ) {
         //get if netmask
         if(++i==argc)
            return -1;
         if(chk_ipv4_addr(argv[i])<0)
            return -1;
         strcpy(if_config.if_net_msk,argv[i]);
      }


   } //for

   //
   if(if_config.if_flags&IFF_ADD) {
      if((fd2=open("/dev/net/ip",O_RDWR,0))<0) {
         printf("error: cannot open ip stack\r\n");
         return 0;
      }

      if((fd1[0]=open("/dev/eth0",O_RDONLY,0))<0) {
         printf("error: cannot open ip stack\r\n");
         return 0;
      }

      if((fd1[1]=open("/dev/eth0",O_WRONLY,0))<0) {
         printf("error: cannot open ip stack\r\n");
         return 0;
      }

      //link devices
      ioctl(fd2,I_LINK,fd1[0]);
      //link devices
      ioctl(fd2,I_LINK,fd1[1]);

      fattach(fd2,"/dev/net/e0");

      close(fd1[0]);
      close(fd1[1]);
      close(fd2);

      if((fd2=open("/dev/net/e0",O_RDWR,0))<0) {
         printf("error: cannot open net device\r\n");
         return 0;
      }

      ioctl(fd2,IFSETCFG,&if_config);
   }


   return 0;
}

/*============================================
| End of Source  : ifconfig.c
==============================================*/
