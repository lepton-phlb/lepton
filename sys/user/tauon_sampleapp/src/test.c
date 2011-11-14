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
/*============================================
| Global Declaration
==============================================*/


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
int test_main(int argc,char* argv[]) {
   //   //do vfork and exec
   int pid,_pid;
   int status = -1;
   char cmd[] = "/usr/bin/test2";
   int i=0;

   pid = vfork();
   switch(pid) {
      case -1://error
	 printf("(EE) Can't vfork\r\n");
	 exit(0);
      break;
      //son
      case 0:
	 printf("Son before execl\r\n");
	 execl(cmd , NULL);
      break;

      default :
	 while((_pid=waitpid((pid_t)-1,&status,0))>0) {
	    printf("(D) in waitpid _pid:%d status:%d\r\n", _pid, status);
	 }
	 printf("(D) After waitpid\r\n");
      break;
   }
   return 0;
}

/*-------------------------------------------
| Name:test2_main
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int test2_main(int argc, char * argv) {
   int a=1, b=2;
   printf("(S)%d\r\n", a+b);
   return 0;
}

/*============================================
| End of Source  : test.c
|---------------------------------------------
| Historic:
|---------------------------------------------
| Authors     | Date     | Comments
| $Log:$
==============================================*/
