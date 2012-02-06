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
//libraries provide by eCos
#include <stdlib.h>
#include <string.h>

//user libraries
#include "kernel/core/signal.h"
#include "kernel/core/wait.h"
#include "kernel/core/stat.h"
#include "kernel/core/libstd.h"
#include "kernel/core/devio.h"
#include "lib/libc/stdio/stdio.h"
#include "kernel/core/time.h"
#include "kernel/core/select.h"
#include "kernel/core/ioctl_fb.h"

#include "lib/libc/termios/termios.h"
#include "kernel/core/kernel_sem.h"

/*============================================
| Global Declaration
==============================================*/
//see KINETIS512_SC/src/projects/i2c from freescale website
static signed char i2c_MMA7660_convert(signed char input);
#define MMA7660_XOUT    0x0
#define MMA7660_YOUT    0x1
#define MMA7660_ZOUT    0x2
#define MMA7660_TILT    0x3
#define MMA7660_SRST    0x4
#define MMA7660_SPCNT   0x5
#define MMA7660_INTSU   0x6
#define MMA7660_MODE    0x7
#define MMA7660_SR      0x8
#define MMA7660_PDET    0x9
#define MMA7660_PD      0xa

/*============================================
| Implementation
==============================================*/
/*-------------------------------------------
| Name:test_main
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int tsti2c_main(int argc,char* argv[]) {
   int fd=-1;
   signed char buf[2] = {0, 0};
   signed char xout, yout, zout;
   
   //note we name it i2c1 because i2c0 device name is already use
   //BUT it's really i2c0 (PTD8, PTD9)
   if((fd = open("/dev/i2c1", O_RDWR | O_NONBLOCK, 0)) < 0) {
      fprintf(stderr, "can't open dev/i2c1\r\n");
      return -1;
   }
   
   //init
   //disable tap detection
   buf[0] = MMA7660_PDET;
   buf[1] = 0xe0;
   write(fd, buf, 2);
   
   //enable auto-sleep, auto-wake, and put in active mode
   buf[0] = MMA7660_MODE;
   buf[1] = 0x19;
   write(fd, buf, 2);
   
   xout = yout = zout = 0;
   //read values
   while(1) {
      //read x
      buf[0] = MMA7660_XOUT;
      buf[1] = 0;
      read(fd, buf, 2);
      xout = i2c_MMA7660_convert(buf[1]);
      
      //read y
      buf[0] = MMA7660_YOUT;
      buf[1] = 0;
      read(fd, buf, 2);
      yout = i2c_MMA7660_convert(buf[1]);
      
      //read z
      buf[0] = MMA7660_ZOUT;
      buf[1] = 0;
      read(fd, buf, 2);
      yout = i2c_MMA7660_convert(buf[1]);

      printf("[X/Y/Z] : %d - %d - %d\r\n", xout, yout, zout);
      //250 ms
      usleep(250000);
   }
   
   close(fd);
   return 0;
}

//
signed char i2c_MMA7660_convert(signed char input) {
   input &=~0x40; //Turn off alert bit
   
   if(input & 0x20)    // update 6-bit signed format to 8-bit signed format
      input |= 0xE0;
   
   return (signed char) input;
}

/*============================================
| End of Source  : tsti2c.c
|---------------------------------------------
| Historic:
|---------------------------------------------
| Authors     | Date     | Comments
| $Log:$
==============================================*/
