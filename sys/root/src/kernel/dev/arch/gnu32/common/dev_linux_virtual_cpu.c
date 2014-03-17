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
#include <stdlib.h>
#include <string.h>
#include "cyg/hal/hal_io.h"

#include "kernel/core/types.h"
#include "kernel/core/interrupt.h"
#include "kernel/core/system.h"
#include "kernel/core/fcntl.h"

#include "kernel/fs/vfs/vfsdev.h"
#include "kernel/fs/vfs/vfstypes.h"


#include "linux_hdwr_ops.h"

/*============================================
| Global Declaration
==============================================*/

int dev_linux_virtual_cpu_load(void);
const char dev_linux_virtual_cpu_name[] = "vcpu\0";

dev_map_t dev_linux_virtual_cpu_map={
   dev_linux_virtual_cpu_name,
   S_IFCHR,
   dev_linux_virtual_cpu_load,
   __fdev_not_implemented,
   __fdev_not_implemented,
   __fdev_not_implemented,
   __fdev_not_implemented,
   __fdev_not_implemented,
   __fdev_not_implemented,
   __fdev_not_implemented,
   __fdev_not_implemented
};

//use by all virtual device
void * shared_dev_addr;

/*============================================
| Implementation
==============================================*/
//open shared memory segment
int dev_linux_virtual_cpu_load(void) {
   int key;
   key = cyg_hal_sys_shmget(VIRTUAL_SHM_KEY, VIRTUAL_SHM_SIZE, CYG_HAL_SYS_IPC_CREAT|0777);
   if(key<0) {
      return -1;
   }
   //
   shared_dev_addr = cyg_hal_sys_shmat(key, 0, 0);
   if(shared_dev_addr == (void *)-1) {
      return -1;
   }

   return 0;
}

/*============================================
| End of Source  : dev_linux_virtual_cpu.c
|---------------------------------------------
| Historic:
|---------------------------------------------
| Authors     | Date     | Comments
| $Log: not supported by cvs2svn $
==============================================*/
