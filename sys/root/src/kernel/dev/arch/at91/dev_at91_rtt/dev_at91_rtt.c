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

#include "dev_at91_rtt.h"
/*============================================
| Global Declaration
==============================================*/
int dev_at91_rtt_load(board_inf_rtt_t * p_inf_rtt);
int dev_at91_rtt_open(desc_t desc, int o_flag);
int dev_at91_rtt_close(desc_t desc);
int dev_at91_rtt_read(desc_t desc, char* buf,int cb);
int dev_at91_rtt_write(desc_t desc, const char* buf,int cb);
int dev_at91_rtt_seek(desc_t desc,int offset,int origin);

static board_inf_rtt_t * p_g_board_rtt = NULL;
/*============================================
| Implementation
==============================================*/
/*-------------------------------------------
| Name:dev_at91_rtt_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91_rtt_load(board_inf_rtt_t * p_inf_rtt){
   if(!p_inf_rtt)
      return -1;

   p_g_board_rtt = p_inf_rtt;
   //AT91C_BASE_RTTC->RTTC_RTMR = (AT91_REG)0x00008000;
   RTT_WRITE_UINT32((unsigned int)(p_g_board_rtt->base_addr+RTT_MR), (AT91_REG)0x00008000);
   return 0;
}

/*-------------------------------------------
| Name:dev_at91_rtt_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91_rtt_open(desc_t desc, int o_flag){
   //
   if(o_flag & O_RDONLY) {
   }

   if(o_flag & O_WRONLY) {
   }

   ofile_lst[desc].offset = 0;
   return 0;
}

/*-------------------------------------------
| Name:dev_at91_rtt_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91_rtt_close(desc_t desc){
   return 0;
}

/*-------------------------------------------
| Name:dev_at91_rtt_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91_rtt_read(desc_t desc, char* buf,int cb){
   time_t time=0;
   register volatile unsigned int val1,val2;

   if(cb!=sizeof(time_t))
      return -1;

   do {
      //val1 = AT91C_BASE_RTTC->RTTC_RTVR;
      //val2 = AT91C_BASE_RTTC->RTTC_RTVR;
      RTT_READ_UINT32((unsigned int)(p_g_board_rtt->base_addr+RTT_VR), val1);
      RTT_READ_UINT32((unsigned int)(p_g_board_rtt->base_addr+RTT_VR), val2);
   }
   while(val1 != val2);

   RTT_READ_UINT32((unsigned int)(p_g_board_rtt->bckup_reg_addr), val2);
   //time = AT91_SYS->SYS_GPBR0 + val1;
   time = val2 + val1;

   memcpy(buf,&time,cb);

   return 0;
}

/*-------------------------------------------
| Name:dev_at91_rtt_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91_rtt_write(desc_t desc, const char* buf,int cb){
   time_t time=0;
   volatile unsigned int rtt_vr;
   if(cb!=sizeof(time_t))
      return -1;

   memcpy(&time,buf,cb);
   RTT_READ_UINT32((unsigned int)(p_g_board_rtt->base_addr+RTT_VR), rtt_vr);

   //AT91_SYS->SYS_GPBR0 = time - AT91C_BASE_RTTC->RTTC_RTVR;
   RTT_WRITE_UINT32((unsigned int)(p_g_board_rtt->bckup_reg_addr), (time-(time_t)rtt_vr));
   return cb;
}

/*-------------------------------------------
| Name:dev_at91_rtt_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91_rtt_seek(desc_t desc,int offset,int origin){
   return -1;
}

/*============================================
| End of Source  : dev_at91_rtt.c
|---------------------------------------------
| Historic:
|---------------------------------------------
| Authors     | Date     | Comments
| $Log:$
==============================================*/
