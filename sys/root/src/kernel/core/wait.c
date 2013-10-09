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
#include "kernel/core/kernelconf.h"
#include "kernel/core/syscall.h"
#include "kernel/core/system.h"
#include "kernel/core/process.h"

/*===========================================
Global Declaration
=============================================*/


/*===========================================
Implementation
=============================================*/

/*-------------------------------------------
| Name:wait
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
pid_t wait(int *status){

   waitpid_t waitpid_dt;

   waitpid_dt.pid = -1;
   waitpid_dt.options = 0;

   __mk_syscall(_SYSCALL_WAITPID,waitpid_dt)

   *status=waitpid_dt.status;

   return waitpid_dt.ret;
}


/*-------------------------------------------
| Name:waitpid
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
pid_t waitpid(pid_t pid,int* status,int options){

   waitpid_t waitpid_dt;

   waitpid_dt.pid = pid;
   waitpid_dt.options = options;

   __mk_syscall(_SYSCALL_WAITPID,waitpid_dt)

   *status=waitpid_dt.status;

   return waitpid_dt.ret;
}

/*===========================================
End of Source wait.c
=============================================*/
