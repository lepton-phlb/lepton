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
// lepton
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

// drivers
#include "kernel/dev/arch/arm9/at91sam9260/common/dev_at91sam9260_common.h"

// eCos
#include "cyg/hal/at91sam9260.h"
#include <string.h>
#include <stdlib.h>


/*===========================================
Global Declaration
=============================================*/

static const char dev_at91sam9260_rtt_name[]="rtt0\0\0";

static int dev_at91sam9260_rtt_load(void);
static int dev_at91sam9260_rtt_isset_read(desc_t desc);
static int dev_at91sam9260_rtt_isset_write(desc_t desc);
static int dev_at91sam9260_rtt_open(desc_t desc, int o_flag);
static int dev_at91sam9260_rtt_close(desc_t desc);
static int dev_at91sam9260_rtt_seek(desc_t desc,int offset,int origin);
static int dev_at91sam9260_rtt_read(desc_t desc, char* buf,int cb);
static int dev_at91sam9260_rtt_write(desc_t desc, const char* buf,int cb);



//
dev_map_t dev_at91sam9260_rtt_map={
   dev_at91sam9260_rtt_name,
   S_IFBLK,
   dev_at91sam9260_rtt_load,
   dev_at91sam9260_rtt_open,
   dev_at91sam9260_rtt_close,
   __fdev_not_implemented,
   __fdev_not_implemented,
   dev_at91sam9260_rtt_read,
   dev_at91sam9260_rtt_write,
   dev_at91sam9260_rtt_seek,
   __fdev_not_implemented, //ioctl
};


/*===========================================
Implementation
=============================================*/
/*-------------------------------------------
| Name:dev_at91sam9260_rtt_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int dev_at91sam9260_rtt_load(void)
{
   AT91C_BASE_RTTC->RTTC_RTMR = (AT91_REG)0x00008000;
   return 0;
}

/*-------------------------------------------
| Name:dev_at91sam9260_rtt_isset_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int dev_at91sam9260_rtt_isset_read(desc_t desc)
{
   return -1;
}

/*-------------------------------------------
| Name:dev_at91sam9260_rtt_isset_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int dev_at91sam9260_rtt_isset_write(desc_t desc)
{
   return -1;
}

/*-------------------------------------------
| Name:dev_at91sam9260_rtt_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int dev_at91sam9260_rtt_open(desc_t desc, int o_flag)
{
   //
   if(o_flag & O_RDONLY) {
   }

   if(o_flag & O_WRONLY) {
   }

   ofile_lst[desc].offset = 0;
   return 0;
}

/*-------------------------------------------
| Name:dev_at91sam9260_rtt_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int dev_at91sam9260_rtt_close(desc_t desc)
{
   return 0;
}

/*-------------------------------------------
| Name:dev_at91sam9260_rtt_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int dev_at91sam9260_rtt_read(desc_t desc, char* buf,int cb)
{
   time_t time=0;

   if(cb!=sizeof(time_t))
      return -1;

   time = AT91C_BASE_SYS->SYS_GPBR[0]+AT91C_BASE_RTTC->RTTC_RTVR;

   memcpy(buf,&time,cb);
   return 0;
}

/*-------------------------------------------
| Name:dev_at91sam9260_rtt_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int dev_at91sam9260_rtt_write(desc_t desc, const char* buf,int cb)
{
   time_t time=0;
   if(cb!=sizeof(time_t))
      return -1;

   memcpy(&time,buf,cb);

   AT91C_BASE_SYS->SYS_GPBR[0] = time - AT91C_BASE_RTTC->RTTC_RTVR;
   return cb;
}

/*-------------------------------------------
| Name:dev_at91sam9260_rtt_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int dev_at91sam9260_rtt_seek(desc_t desc,int offset,int origin)
{
   return -1;
}

/*============================================
| End of Source  : dev_at91sam9260_rtt.c
==============================================*/
