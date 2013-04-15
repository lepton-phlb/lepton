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
#include "kernel/core/kernelconf.h"
#include "kernel/core/kernel.h"
#include "kernel/core/system.h"
#include "kernel/core/fcntl.h"
#include "kernel/core/cpu.h"
#include "kernel/core/core_rttimer.h"
#include "kernel/fs/vfs/vfsdev.h"

/*===========================================
Global Declaration
=============================================*/
const char dev_k60n512_enet_name[]="eth0\0";

static int dev_twrk60n512_enet_load(void);

extern int dev_k60n512_enet_load(void);
extern int dev_k60n512_enet_open(desc_t desc, int o_flag);
extern int dev_k60n512_enet_close(desc_t desc);
extern int dev_k60n512_enet_isset_read(desc_t desc);
extern int dev_k60n512_enet_isset_write(desc_t desc);
extern int dev_k60n512_enet_seek(desc_t desc,int offset,int origin);
extern int dev_k60n512_enet_read(desc_t desc, char* buf,int cb);
extern int dev_k60n512_enet_write(desc_t desc, const char* buf,int cb);
extern int dev_k60n512_enet_ioctl(desc_t desc,int request,va_list ap);

dev_map_t dev_twrk60n512_enet_map={
   dev_k60n512_enet_name,
   S_IFCHR,
   dev_twrk60n512_enet_load,
   dev_k60n512_enet_open,
   dev_k60n512_enet_close,
   dev_k60n512_enet_isset_read,
   dev_k60n512_enet_isset_write,
   dev_k60n512_enet_read,
   dev_k60n512_enet_write,
   dev_k60n512_enet_seek,
   dev_k60n512_enet_ioctl
};

//
#define ENET_RMMI_MDIO     CYGHWR_HAL_KINETIS_PIN(B, 0, 4, 0)
#define ENET_RMMI_MDC      CYGHWR_HAL_KINETIS_PIN(B, 1, 4, 0)
#define ENET_RMII_CRS_DV   CYGHWR_HAL_KINETIS_PIN(A, 14, 4, 0)
#define ENET_RMMI_RXD1     CYGHWR_HAL_KINETIS_PIN(A, 12, 4, 0)
#define ENET_RMMI_RXD0     CYGHWR_HAL_KINETIS_PIN(A, 13, 4, 0)
#define ENET_RMMI_TXEN     CYGHWR_HAL_KINETIS_PIN(A, 15, 4, 0)
#define ENET_RMMI_TXD0     CYGHWR_HAL_KINETIS_PIN(A, 16, 4, 0)
#define ENET_RMMI_TXD1     CYGHWR_HAL_KINETIS_PIN(A, 17, 4, 0)

#ifdef RXERR //
#define ENET_RMMI_RXER     CYGHWR_HAL_KINETIS_PIN(A, 5, 4, 0)
#endif

/*===========================================
Implementation
=============================================*/
/*-------------------------------------------
| Name:dev_k60n512_dspi_2_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_twrk60n512_enet_load(void) {
   volatile unsigned int reg_val = 0;
   
   //for tower
	hal_set_pin_function(ENET_RMMI_MDIO);
	hal_set_pin_function(ENET_RMMI_MDC);
	hal_set_pin_function(ENET_RMII_CRS_DV);
   hal_set_pin_function(ENET_RMMI_RXD1);
   hal_set_pin_function(ENET_RMMI_RXD0);
   hal_set_pin_function(ENET_RMMI_TXEN);
   hal_set_pin_function(ENET_RMMI_TXD0);
	hal_set_pin_function(ENET_RMMI_TXD1);
   
   #ifdef RXERR
   hal_set_pin_function(ENET_RMMI_RXER);
   #endif
         
   return dev_k60n512_enet_load();
}

/*============================================
| End of Source  : dev_k60n512_dspi_2.c
==============================================*/
