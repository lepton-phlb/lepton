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
#include "kernel/core/dirent.h"

#include "lib/libc/stdio/stdio.h"


/*===========================================
Global Declaration
=============================================*/

static const char __lpd_dflt_path[]="/var/spool";

/*===========================================
Implementation
=============================================*/



/*-------------------------------------------
| Name:lpd_main
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int lpd_main(int argc,char* argv[]){

   DIR* dir;
   struct dirent dirent;
   unsigned int opt=0;
   char* ref = (char*)__lpd_dflt_path;
   char buf[64];
   char path[PATH_MAX];

   //
   mkdir(__lpd_dflt_path,0);

   //
   for(;; ) {
      int fd =-1;
      int r=0;
      int w=0;

      //
      usleep(1000000); //1 second polling period

      //find file to print, in spool directory
      if(!(dir=opendir(ref)))
         return -1;

      while(readdir2(dir,&dirent)) {
         if(dirent.d_name[0]=='.')
            continue;

         strcpy(path,__lpd_dflt_path);
         strcat(path,"/");
         strcat(path,dirent.d_name);

         if( (fd=open(path,O_RDONLY,0))>=0)
            break;
      }

      closedir(dir);

      if(fd<0)
         continue;


      //to do lock file

      //print file
      while ( (r=read(fd,buf,sizeof(buf))) ) {
         w=0;
         while(w < r )
            w+=write(STDOUT_FILENO,buf+w,r-w);
      }

      close(fd);

      //delete file
      remove(path);

   }

   return 0;
}


/*===========================================
End of Source lpd.c
=============================================*/
