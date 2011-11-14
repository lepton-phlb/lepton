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
#include "kernel/core/types.h"
#include "kernel/core/interrupt.h"
#include "kernel/core/kernelconf.h"
#include "kernel/core/kernel.h"
#include "kernel/core/system.h"
#include "kernel/core/fcntl.h"

#include "kernel/fs/vfs/vfsdev.h"

#include "lib/libc/ctype/ctype.h"
#include "kernel/core/types.h"
#include "kernel/core/time.h"
#include "kernel/core/devio.h"
#include "kernel/core/errno.h"

#include "kernel/dev/arch/at91/dev_at91_rtt/dev_at91_rtt.h"

/*===========================================
Global Declaration
=============================================*/

static const char dev_at91sam9261_rtt_name[]="rtt0\0\0";

static int dev_at91sam9261_rtt_load(void);
extern int dev_at91_rtt_load(board_inf_rtt_t * p_inf_rtt);
extern int dev_at91_rtt_open(desc_t desc, int o_flag);
extern int dev_at91_rtt_close(desc_t desc);
extern int dev_at91_rtt_read(desc_t desc, char* buf,int cb);
extern int dev_at91_rtt_write(desc_t desc, const char* buf,int cb);
extern int dev_at91_rtt_seek(desc_t desc,int offset,int origin);

//
dev_map_t dev_at91sam9261_rtt_map={
   dev_at91sam9261_rtt_name,
   S_IFBLK,
   dev_at91sam9261_rtt_load,
   dev_at91_rtt_open,
   dev_at91_rtt_close,
   __fdev_not_implemented,
   __fdev_not_implemented,
   dev_at91_rtt_read,
   dev_at91_rtt_write,
   dev_at91_rtt_seek,
   __fdev_not_implemented, //ioctl
};

static board_inf_rtt_t g_inf_board_rtt __attribute__ ((section (".no_cache"))) = {
      base_addr : (volatile unsigned int *)0xfffffd20,
      bckup_reg_addr : (volatile unsigned int *)0xfffffd50
};

/*===========================================
Implementation
=============================================*/
/*-------------------------------------------
| Name:dev_at91sam9261_rtt_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91sam9261_rtt_load(void){
   return dev_at91_rtt_load(&g_inf_board_rtt);
}

/*============================================
| End of Source  : dev_at91sam9261_rtt.c
==============================================*/
