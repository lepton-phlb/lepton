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

/*===========================================
Global Declaration
=============================================*/


static const char __lpd_dflt_path[]="/var/spool";

/*===========================================
Implementation
=============================================*/



/*-------------------------------------------
| Name:lpr_main
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int lpr_main(int argc,char* argv[]){

   int i = 0;
   unsigned int opt=0;
   char buf[64];
   int r;

   int fd;

   if(mknod("/var/spool/pipe0",S_IFIFO,0)<0)
      return -1;

   fd = open("/var/spool/pipe0",O_WRONLY,0);
   if(fd<0)
      return -1;

   while( (r=read(0,buf,sizeof(buf)))>0 ) {
      int w=0;
      while(w < r )
         w+=write(fd,buf+w,r-w);
   }

   return 0;
}


/*===========================================
End of Source lpr.c
=============================================*/
