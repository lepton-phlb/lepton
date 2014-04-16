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


/*===========================================
Includes
=============================================*/
#include "kernel/core/signal.h"
#include "kernel/core/libstd.h"
#include "kernel/core/stat.h"
#include "kernel/core/statvfs.h"
#include "kernel/core/devio.h"
#include "kernel/core/time.h"
#include "kernel/core/wait.h"
#include "kernel/core/fcntl.h"

#include "lib/libc/stdio/stdio.h"

#include <string.h>
/*===========================================
Global Declaration
=============================================*/
#define CAT_BUF_SIZE 64

const int cat_read_size = 1;
char const colon[2] = { ':', ' ' };
const char nl = '\n';


/*===========================================
Implementation
=============================================*/


/*-------------------------------------------
| Name:dumpfile
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static void dumpfile(int fd)
{
   int i,cb;
   char buf[CAT_BUF_SIZE];
   const char cr='\r';

   while ((cb=read(fd,buf,sizeof(buf))) > 0) {
      for(i=0; i<cb; i++) {
         if(buf[i]=='\n')
            write(STDOUT_FILENO,&cr,1);
         else if(buf[i]=='\x18')
            exit(0);
         write(STDOUT_FILENO,&buf[i],1);
      }

   }
}

/*-------------------------------------------
| Name:cat_main
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int cat_main(int argc,char* argv[])
{
   int i, fd;

   if(argc <= 1) {
      dumpfile(STDIN_FILENO);
   } else {
      for(i=1; i<argc; i++) {
         fd = open(argv[i], O_RDONLY,0);
         if(fd == -1) {
            write(STDERR_FILENO, argv[0], strlen(argv[0]));
            write(STDERR_FILENO, colon, 2);
            write(STDERR_FILENO, argv[i], strlen(argv[i]));
            write(STDERR_FILENO, colon, 2);
            write(STDERR_FILENO, &nl, 1);
         } else {
            dumpfile(fd);
            close(fd);
         }
      }
   }

   return 0;
}


/*===========================================
End of Source cat.c
=============================================*/
