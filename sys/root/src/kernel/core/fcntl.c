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
#include "kernel/core/errno.h"
#include "kernel/core/kernelconf.h"
#include "kernel/core/syscall.h"
#include "kernel/core/system.h"
#include "kernel/core/fcntl.h"


/*===========================================
Global Declaration
=============================================*/


/*===========================================
Implementation
=============================================*/

/*-------------------------------------------
| Name:_sys_fcntl
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int fcntl(unsigned int fd, unsigned int cmd,...){
   fcntl_t fcntl_dt;
   int argc=0;

   va_list ptr;
   va_start(ptr, cmd);

   if(fd<0)
      return -1;

   for(argc=0; argc<FCNTL_ARG_MAX; argc++) {
      switch(cmd) {
      case F_DUPFD:   //Duplicate file descriptor.
      case F_GETFD:   //Get file descriptor flags.
      case F_SETFD:   //Set file descriptor flags.
      case F_GETFL:   //Get file status flags and file access modes.
      case F_SETFL:   //Set file status flags:
      case F_SETLK:   //set file lock
      case F_SETLKW:   //set file lock blocking call
      case F_GETLK:   //get file lock
      {
         int i;
         fcntl_dt.argv[argc]=(void*)0;
         i = va_arg( ptr, int);
         if(i<0)
            goto end;
         fcntl_dt.argv[argc] = (void*)i;
      }
      break;

      default:
         //cmd not yet supported
         return -1;
      }
   }
end:
   va_end(ptr);

   fcntl_dt.fd     = fd;
   fcntl_dt.cmd    = cmd;
   fcntl_dt.argc   = argc;

   __mk_syscall(_SYSCALL_FCNTL,fcntl_dt);

   return fcntl_dt.ret;
}



/*===========================================
End of Sourcefcntl.c
=============================================*/
