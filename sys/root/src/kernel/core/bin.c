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
#include "kernel/core/types.h"
#include "kernel/core/interrupt.h"
#include "kernel/core/kernel.h"
#include "kernel/core/syscall.h"
#include "kernel/core/process.h"
#include "kernel/core/statvfs.h"

#include "kernel/fs/vfs/vfs.h"

#include "kernel/core/bin.h"

#include <string.h>
#include <stdlib.h>

/*===========================================
Global Declaration
=============================================*/


/*===========================================
Implementation
=============================================*/

/*-------------------------------------------
| Name:_mkbin
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _mkbin(const char* ref,const char* name){
   int i;
   int cb;
   desc_t desc;
   exec_file_t exec_file;
   
   for(i=0;i<bin_lst_size;i++){
      if( !strcmp(bin_lst[i].name,name))break;
   }

   if(i==bin_lst_size)
      return -1;
   exec_file.signature  = EXEC_SIGNT;
   exec_file.priority   = bin_lst[i].priority;//KERNEL_PRIORITY;//101;/*KERNEL_PRIORITY-10*/;
   exec_file.stacksize  = bin_lst[i].stacksize;
   exec_file.timeslice  = bin_lst[i].timeslice;
   exec_file.index      = i; 

   //binary init
   desc= _vfs_open(ref,O_CREAT|O_WRONLY,0);
   cb=sizeof(exec_file);
   cb=_vfs_write(desc,(char*)&exec_file,cb);
   _vfs_close(desc);

   //test verif
   desc= _vfs_open(ref,O_RDONLY,0);
   cb=sizeof(exec_file);
   cb=_vfs_read(desc,(char*)&exec_file,cb);
   _vfs_close(desc);
   
   
   return 0;
}


/*===========================================
End of Sourcebin.c
=============================================*/
