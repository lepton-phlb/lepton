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

#include "kernel/core/kernel.h"
#include "kernel/core/libstd.h"
#include "kernel/core/stat.h"
#include "kernel/core/statvfs.h"
#include "kernel/core/devio.h"
#include "kernel/core/kernel_clock.h"
#include "kernel/core/time.h"
#include "kernel/core/fcntl.h"
#include "kernel/core/ioctl_eth.h"
#include "kernel/core/ioctl_if.h"

#include "lib/libc/termios/termios.h"
#include "lib/pthread/pthread.h"

#include "lib/libc/stdio/stdio.h"


/*============================================
| Global Declaration 
==============================================*/
#define NONE    0
#define ODD     1
#define EVEN    2


static char buf[1500];

/*============================================
| Implementation 
==============================================*/

/*-------------------------------------------
| Name:setparity
| Description:Set the parity stored in *TERMIOS_P
| Parameters: par
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int setparity(struct termios *termios_p, int par)
{termios_p->c_cflag &= ~PARODD;
 switch(par)
    {case NONE : termios_p->c_cflag &= ~PARENB;
                 break;
     case ODD  : termios_p->c_cflag |= PARODD;
     case EVEN : termios_p->c_cflag |= PARENB;
                 break;
    }
 return 0;
}


/*-------------------------------------------
| Name:setdatasize
| Description:Set the data size stored in *TERMIOS_P
| Parameters: nbbit
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int setdatasize(struct termios *termios_p, int nbbit)
{termios_p->c_cflag &= ~CSIZE;
 termios_p->c_cflag |= nbbit;
 return 0;
}

/*-------------------------------------------
| Name:setstopbit
| Description:Set the number of stop bit stored in *TERMIOS_P
| Parameters: nbbit
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int setstopbit(struct termios *termios_p, unsigned char nbbit)
{if(nbbit == 2)
    termios_p->c_cflag |= CSTOPB;   // 2 stop bit
 else
    termios_p->c_cflag &= ~CSTOPB;  // 1 stop bit
 return 0;
}

/*--------------------------------------------
| Name:        tsteth_main
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int tsteth_main(int argc,char* argv[]){
   int fd;
   int cb;
   int v;
   int vit = B38400;
   struct termios _termconf;
   unsigned int pckt_nb=0;
   
   //put standard output on /dev/rtu2 rs485
#if 0
   close(1);
   fd=open("/dev/ttys0",O_WRONLY,0);
   fd=dup(fd);
   tcgetattr(1,&_termconf);                // Permet d'obtenir les paramètres courants de la liaison ttys1
   cfsetispeed(&_termconf, B38400);
   cfsetospeed(&_termconf, B38400);
   setparity(&_termconf, NONE);
   setdatasize(&_termconf, CS8);
   setstopbit(&_termconf, 1);
   tcsetattr(1,TCSANOW,&_termconf);  
#endif   
   
   /*
   v = TIOCM_DTR;// #Define  normalisé pour DTR
   ioctl(1,TIOCMSET,&v); 
   */
   
   printf("tsteth....\r\n");
   
   //
   fd = open("/dev/eth0",O_RDONLY,0);
   if(fd<0){
      printf("error: cannot open ethernet device\r\n");
      return 0;
   }

   //
   while((cb=read(fd,buf,sizeof(buf)))>0){
      int i;
      int l=0;
      printf("pckt[%d]=%d\r\n",pckt_nb++,cb);
      for(i=0;i<cb;i++){
         printf("%02x ",buf[i]);
	 if(!((++l)%16))
	    printf("\r\n");
      }
      printf("\r\n");
  }
   
   // pdl = 0
   /*
   v = 0;
   ioctl(1,TIOCMSET,&v); 
   */

   return 0;
}

/*============================================
| End of Source  : tsteth.c
==============================================*/