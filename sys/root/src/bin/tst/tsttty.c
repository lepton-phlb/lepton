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
#include "stdlib.h"

#include "kernel/libstd.h"
#include "kernel/devio.h"
#include "kernel/fcntl.h"
#include "kernel/devio.h"
#include "libc/termios/termios.h"


/*============================================
| Global Declaration
==============================================*/
static unsigned char buffer[1024];
#define TSTTTY_DEFAULT_SPEED  (115200L)

/*============================================
| Implementation
==============================================*/

/*--------------------------------------------
| Name:        tsttty_main
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int tsttty_main(int argc,char* argv[]){
   unsigned long speed=TSTTTY_DEFAULT_SPEED;
   int i;

   if(argc>1)
      speed=atol(argv[1]);

   for(i=0; i<512; i++) {
      buffer[i]=(unsigned char)(i%10)+'0';
   }

   ioctl(1,TIOCSSERIAL,speed);

   write(1,buffer,512);
   write(1,buffer,512);
   write(1,buffer,512);

   ioctl(1,TIOCSSERIAL,TSTTTY_DEFAULT_SPEED);


   return 0;
}

/*============================================
| End of Source  : tsttty.c
==============================================*/
