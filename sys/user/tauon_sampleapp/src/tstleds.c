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
#include "lib/libc/stdio/stdio.h"
#include "kernel/core/time.h"
#include "kernel/core/select.h"
#include "kernel/core/ioctl_fb.h"

#include "lib/libc/termios/termios.h"
#include "kernel/core/kernel_sem.h"

/*============================================
| Global Declaration
==============================================*/

#define NUM_LEDS  4
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
int tstleds_main(int argc,char* argv[]) {
   int fd=-1;
   unsigned char buf[NUM_LEDS];
   int i=0;

   if((fd = open("/dev/gleds", O_RDWR, 0)) < 0) {
      fprintf(stderr, "(EE) can't open /dev/gleds\r\n");
      return -1;
   }

   while(1) {
      memset(buf, 0, NUM_LEDS);

      buf[i++] = 1;
      write(fd, buf, NUM_LEDS);

      usleep(100000);
      i = i%NUM_LEDS;
   }

   close(fd);
   return 0;
}

/*============================================
| End of Source  : tstleds.c
|---------------------------------------------
| Historic:
|---------------------------------------------
| Authors     | Date     | Comments
| $Log:$
==============================================*/
