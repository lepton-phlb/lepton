/*--------------------------------------------
| Copyright(C) 2005 CHAUVIN-ARNOUX
|---------------------------------------------
| Project:
| Project Manager:
| Source:          test.c
| Path:
| Authors:
| Plateform:   	GNU/Linux
| Created:
| Revision/Date: 	$Revision:$ / $Date:$
| Description:
|---------------------------------------------*/


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
