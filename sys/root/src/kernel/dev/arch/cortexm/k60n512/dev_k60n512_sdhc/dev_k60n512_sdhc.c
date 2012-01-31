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
#include "kernel/core/ioctl_hd.h"

#include "lib/libc/termios/termios.h"

#include "dev_k60n512_sdhc.h"
#include "kernel/dev/arch/cortexm/k60n512/common/dev_k60n512_sim.h"

#include "kernel/dev/arch/all/sd/dev_sd.h"
/*===========================================
Global Declaration
=============================================*/
const char dev_k60n512_sdhc_name[]="sdhc\0";

static int dev_k60n512_sdhc_load(void);
static int dev_k60n512_sdhc_open(desc_t desc, int o_flag);
static int dev_k60n512_sdhc_close(desc_t desc);
static int dev_k60n512_sdhc_seek(desc_t desc,int offset,int origin);
static int dev_k60n512_sdhc_read(desc_t desc, char* buf,int cb);
static int dev_k60n512_sdhc_write(desc_t desc, const char* buf,int cb);
static int dev_k60n512_sdhc_ioctl(desc_t desc,int request,va_list ap);

dev_map_t dev_k60n512_sdhc_map={
   dev_k60n512_sdhc_name,
   S_IFCHR,
   dev_k60n512_sdhc_load,
   dev_k60n512_sdhc_open,
   dev_k60n512_sdhc_close,
   __fdev_not_implemented,
   __fdev_not_implemented,
   dev_k60n512_sdhc_read,
   dev_k60n512_sdhc_write,
   dev_k60n512_sdhc_seek,
   dev_k60n512_sdhc_ioctl
};

//
#define  KINETIS_SDHC_INIT_SPEED       400000
#define  KINETIS_SDHC_NORMAL_SPEED     15000000

//
#define  KINETIS_SDHC_IRQ_NO				81//80
#define  KINETIS_SDHC_IRQ_PRIORITY		3

//
#define  KINETIS_SDHC_ERRORS_IRQS      (REG_SDHC_IRQSTATEN_DEBESEN_MASK | \
REG_SDHC_IRQSTATEN_DCESEN_MASK | REG_SDHC_IRQSTATEN_DTOESEN_MASK | \
REG_SDHC_IRQSTATEN_CIESEN_MASK | REG_SDHC_IRQSTATEN_CEBESEN_MASK | \
REG_SDHC_IRQSTATEN_CCESEN_MASK | REG_SDHC_IRQSTATEN_CTOESEN_MASK)

/*#define  KINETIS_SDHC_ALLOWED_IRQS    (KINETIS_SDHC_ERRORS_IRQS | \
REG_SDHC_IRQSTATEN_DINTSEN_MASK | \
REG_SDHC_IRQSTATEN_CRMSEN_MASK | REG_SDHC_IRQSTATEN_TCSEN_MASK | \
REG_SDHC_IRQSTATEN_CCSEN_MASK)*/

#define  KINETIS_SDHC_ALLOWED_IRQS    (KINETIS_SDHC_ERRORS_IRQS | \
REG_SDHC_IRQSTATEN_DINTSEN_MASK | \
REG_SDHC_IRQSTATEN_TCSEN_MASK | \
REG_SDHC_IRQSTATEN_CCSEN_MASK)

//
typedef struct board_kinetis_sdhc_info_st {
	unsigned int sdhc_base;
	
	desc_t _desc_rd;
	desc_t _desc_wr;

	//for eCos IRQ
	cyg_handle_t  _irq_handle;
	cyg_interrupt _irq_it;
   
   //
   mci_cmd_t * command;
   
   //
   kernel_sem_t cmd_completed;
   unsigned char post_event;
} board_kinetis_sdhc_info_t;

board_kinetis_sdhc_info_t g_kinetis_sdhc_info;

//
typedef struct _sdhc_stat_st {
   unsigned int error;
   
   unsigned int cc;
   unsigned int tc;
   unsigned int dint;
} _sdhc_stat_t;

_sdhc_stat_t g_kinetis_sdhc_stat;

#if defined(USE_ECOS)

#define SDHC_CLK CYGHWR_HAL_KINETIS_PIN(E, 2, 4, (PORT_PCR_DSE_MASK))
#define SDHC_CMD CYGHWR_HAL_KINETIS_PIN(E, 3, 4, (PORT_PCR_PS_MASK|PORT_PCR_PE_MASK|PORT_PCR_DSE_MASK))
#define SDHC_D0 CYGHWR_HAL_KINETIS_PIN(E, 1, 4, (PORT_PCR_PS_MASK|PORT_PCR_PE_MASK|PORT_PCR_DSE_MASK))
#define SDHC_D1 CYGHWR_HAL_KINETIS_PIN(E, 0, 4, (PORT_PCR_PS_MASK|PORT_PCR_PE_MASK|PORT_PCR_DSE_MASK))
#define SDHC_D2 CYGHWR_HAL_KINETIS_PIN(E, 5, 4, (PORT_PCR_PS_MASK|PORT_PCR_PE_MASK|PORT_PCR_DSE_MASK))
#define SDHC_D3 CYGHWR_HAL_KINETIS_PIN(E, 4, 4, (PORT_PCR_PS_MASK|PORT_PCR_PE_MASK|PORT_PCR_DSE_MASK))

__externC void  hal_set_pin_function(cyg_uint32 pin);
__externC void 	hal_freescale_uart_setbaud(cyg_uint32 uart_p, cyg_uint32 baud);

static cyg_uint32 _kinetis_sdhc_isr(cyg_vector_t vector, cyg_addrword_t data);
static void _kinetis_sdhc_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data);
#endif

static void _kinetis_sdhc_configure_pins(unsigned char pin_flag);
static void _kinetis_sdhc_setbaud(board_kinetis_sdhc_info_t * p_inf_sdhc, 
unsigned int master_clock, unsigned int baudrate);
static int _kinetis_sdhc_send_command(board_kinetis_sdhc_info_t * p_inf_sdhc);

static const unsigned int _kinetis_sdhc_supported_command[] = {
   REG_SDHC_XFERTYP_CMDINX(0) | REG_SDHC_XFERTYP_RSPTYP(SDHC_XFERTYP_RSPTYP_NO),
   REG_SDHC_XFERTYP_CMDINX(1) | REG_SDHC_XFERTYP_RSPTYP(SDHC_XFERTYP_RSPTYP_NO),
   REG_SDHC_XFERTYP_CMDINX(2) | REG_SDHC_XFERTYP_CCCEN_MASK | REG_SDHC_XFERTYP_RSPTYP(SDHC_XFERTYP_RSPTYP_136),
   REG_SDHC_XFERTYP_CMDINX(3) | REG_SDHC_XFERTYP_CICEN_MASK | REG_SDHC_XFERTYP_CCCEN_MASK | REG_SDHC_XFERTYP_RSPTYP(SDHC_XFERTYP_RSPTYP_48),
   REG_SDHC_XFERTYP_CMDINX(4) | REG_SDHC_XFERTYP_RSPTYP(SDHC_XFERTYP_RSPTYP_NO),
   REG_SDHC_XFERTYP_CMDINX(5) | REG_SDHC_XFERTYP_RSPTYP(SDHC_XFERTYP_RSPTYP_48), //CMD5
   REG_SDHC_XFERTYP_CMDINX(SDHC_ACMD6) | REG_SDHC_XFERTYP_CICEN_MASK | REG_SDHC_XFERTYP_CCCEN_MASK | REG_SDHC_XFERTYP_RSPTYP(SDHC_XFERTYP_RSPTYP_48),
   //REG_SDHC_XFERTYP_CMDINX(6) | REG_SDHC_XFERTYP_CICEN_MASK | REG_SDHC_XFERTYP_CCCEN_MASK | REG_SDHC_XFERTYP_RSPTYP(SDHC_XFERTYP_RSPTYP_48),
   REG_SDHC_XFERTYP_CMDINX(7) | REG_SDHC_XFERTYP_CICEN_MASK | REG_SDHC_XFERTYP_CCCEN_MASK | REG_SDHC_XFERTYP_RSPTYP(SDHC_XFERTYP_RSPTYP_48BUSY),
   REG_SDHC_XFERTYP_CMDINX(8) | REG_SDHC_XFERTYP_CICEN_MASK | REG_SDHC_XFERTYP_CCCEN_MASK | REG_SDHC_XFERTYP_RSPTYP(SDHC_XFERTYP_RSPTYP_48),
   REG_SDHC_XFERTYP_CMDINX(9) | REG_SDHC_XFERTYP_CCCEN_MASK | REG_SDHC_XFERTYP_RSPTYP(SDHC_XFERTYP_RSPTYP_136),
   REG_SDHC_XFERTYP_CMDINX(10) | REG_SDHC_XFERTYP_CCCEN_MASK | REG_SDHC_XFERTYP_RSPTYP(SDHC_XFERTYP_RSPTYP_136), //CMD10
   REG_SDHC_XFERTYP_CMDINX(11) | REG_SDHC_XFERTYP_CICEN_MASK | REG_SDHC_XFERTYP_CCCEN_MASK | REG_SDHC_XFERTYP_RSPTYP(SDHC_XFERTYP_RSPTYP_48),
   REG_SDHC_XFERTYP_CMDINX(12) | REG_SDHC_XFERTYP_CMDTYP(SDHC_XFERTYP_CMDTYP_ABORT) | REG_SDHC_XFERTYP_CICEN_MASK | REG_SDHC_XFERTYP_CCCEN_MASK | REG_SDHC_XFERTYP_RSPTYP(SDHC_XFERTYP_RSPTYP_48BUSY),
   REG_SDHC_XFERTYP_CMDINX(13) | REG_SDHC_XFERTYP_CICEN_MASK | REG_SDHC_XFERTYP_CCCEN_MASK | REG_SDHC_XFERTYP_RSPTYP(SDHC_XFERTYP_RSPTYP_48),
   0,
   REG_SDHC_XFERTYP_CMDINX(15) | REG_SDHC_XFERTYP_RSPTYP(SDHC_XFERTYP_RSPTYP_NO), //CMD15
   REG_SDHC_XFERTYP_CMDINX(16) | REG_SDHC_XFERTYP_CICEN_MASK | REG_SDHC_XFERTYP_CCCEN_MASK | REG_SDHC_XFERTYP_RSPTYP(SDHC_XFERTYP_RSPTYP_48),
   REG_SDHC_XFERTYP_CMDINX(17) | REG_SDHC_XFERTYP_CICEN_MASK | REG_SDHC_XFERTYP_CCCEN_MASK | REG_SDHC_XFERTYP_RSPTYP(SDHC_XFERTYP_RSPTYP_48),
   REG_SDHC_XFERTYP_CMDINX(18) | REG_SDHC_XFERTYP_CICEN_MASK | REG_SDHC_XFERTYP_CCCEN_MASK | REG_SDHC_XFERTYP_RSPTYP(SDHC_XFERTYP_RSPTYP_48),
   0,
   REG_SDHC_XFERTYP_CMDINX(20) | REG_SDHC_XFERTYP_CICEN_MASK | REG_SDHC_XFERTYP_CCCEN_MASK | REG_SDHC_XFERTYP_RSPTYP(SDHC_XFERTYP_RSPTYP_48), //CMD20
   0,
   REG_SDHC_XFERTYP_CMDINX(SDHC_ACMD22) | REG_SDHC_XFERTYP_CICEN_MASK | REG_SDHC_XFERTYP_CCCEN_MASK | REG_SDHC_XFERTYP_RSPTYP(SDHC_XFERTYP_RSPTYP_48),
   REG_SDHC_XFERTYP_CMDINX(SDHC_ACMD23) | REG_SDHC_XFERTYP_CICEN_MASK | REG_SDHC_XFERTYP_CCCEN_MASK | REG_SDHC_XFERTYP_RSPTYP(SDHC_XFERTYP_RSPTYP_48),
   REG_SDHC_XFERTYP_CMDINX(24) | REG_SDHC_XFERTYP_CICEN_MASK | REG_SDHC_XFERTYP_CCCEN_MASK | REG_SDHC_XFERTYP_RSPTYP(SDHC_XFERTYP_RSPTYP_48),
   REG_SDHC_XFERTYP_CMDINX(25) | REG_SDHC_XFERTYP_CICEN_MASK | REG_SDHC_XFERTYP_CCCEN_MASK | REG_SDHC_XFERTYP_RSPTYP(SDHC_XFERTYP_RSPTYP_48), //CMD25
   REG_SDHC_XFERTYP_CMDINX(26) | REG_SDHC_XFERTYP_CICEN_MASK | REG_SDHC_XFERTYP_CCCEN_MASK | REG_SDHC_XFERTYP_RSPTYP(SDHC_XFERTYP_RSPTYP_48),
   REG_SDHC_XFERTYP_CMDINX(27) | REG_SDHC_XFERTYP_CICEN_MASK | REG_SDHC_XFERTYP_CCCEN_MASK | REG_SDHC_XFERTYP_RSPTYP(SDHC_XFERTYP_RSPTYP_48),
   REG_SDHC_XFERTYP_CMDINX(28) | REG_SDHC_XFERTYP_CICEN_MASK | REG_SDHC_XFERTYP_CCCEN_MASK | REG_SDHC_XFERTYP_RSPTYP(SDHC_XFERTYP_RSPTYP_48BUSY),
   REG_SDHC_XFERTYP_CMDINX(29) | REG_SDHC_XFERTYP_CICEN_MASK | REG_SDHC_XFERTYP_CCCEN_MASK | REG_SDHC_XFERTYP_RSPTYP(SDHC_XFERTYP_RSPTYP_48BUSY),
   REG_SDHC_XFERTYP_CMDINX(30) | REG_SDHC_XFERTYP_CICEN_MASK | REG_SDHC_XFERTYP_CCCEN_MASK | REG_SDHC_XFERTYP_RSPTYP(SDHC_XFERTYP_RSPTYP_48),//CMD30
   0,
   REG_SDHC_XFERTYP_CMDINX(32) | REG_SDHC_XFERTYP_CICEN_MASK | REG_SDHC_XFERTYP_CCCEN_MASK | REG_SDHC_XFERTYP_RSPTYP(SDHC_XFERTYP_RSPTYP_48),
   REG_SDHC_XFERTYP_CMDINX(33) | REG_SDHC_XFERTYP_CICEN_MASK | REG_SDHC_XFERTYP_CCCEN_MASK | REG_SDHC_XFERTYP_RSPTYP(SDHC_XFERTYP_RSPTYP_48),
   REG_SDHC_XFERTYP_CMDINX(34) | REG_SDHC_XFERTYP_CICEN_MASK | REG_SDHC_XFERTYP_CCCEN_MASK | REG_SDHC_XFERTYP_RSPTYP(SDHC_XFERTYP_RSPTYP_48),
   REG_SDHC_XFERTYP_CMDINX(35) | REG_SDHC_XFERTYP_CICEN_MASK | REG_SDHC_XFERTYP_CCCEN_MASK | REG_SDHC_XFERTYP_RSPTYP(SDHC_XFERTYP_RSPTYP_48),//CMD35
   REG_SDHC_XFERTYP_CMDINX(36) | REG_SDHC_XFERTYP_CICEN_MASK | REG_SDHC_XFERTYP_CCCEN_MASK | REG_SDHC_XFERTYP_RSPTYP(SDHC_XFERTYP_RSPTYP_48),
   REG_SDHC_XFERTYP_CMDINX(37) | REG_SDHC_XFERTYP_CICEN_MASK | REG_SDHC_XFERTYP_CCCEN_MASK | REG_SDHC_XFERTYP_RSPTYP(SDHC_XFERTYP_RSPTYP_48),
   REG_SDHC_XFERTYP_CMDINX(38) | REG_SDHC_XFERTYP_CICEN_MASK | REG_SDHC_XFERTYP_CCCEN_MASK | REG_SDHC_XFERTYP_RSPTYP(SDHC_XFERTYP_RSPTYP_48BUSY),
   REG_SDHC_XFERTYP_CMDINX(39) | REG_SDHC_XFERTYP_RSPTYP(SDHC_XFERTYP_RSPTYP_48),
   REG_SDHC_XFERTYP_CMDINX(40) | REG_SDHC_XFERTYP_CICEN_MASK | REG_SDHC_XFERTYP_CCCEN_MASK | REG_SDHC_XFERTYP_RSPTYP(SDHC_XFERTYP_RSPTYP_48), //CMD40
   REG_SDHC_XFERTYP_CMDINX(SDHC_ACMD41) | REG_SDHC_XFERTYP_RSPTYP(SDHC_XFERTYP_RSPTYP_48),
   REG_SDHC_XFERTYP_CMDINX(42) | REG_SDHC_XFERTYP_CICEN_MASK | REG_SDHC_XFERTYP_CCCEN_MASK | REG_SDHC_XFERTYP_RSPTYP(SDHC_XFERTYP_RSPTYP_48BUSY),
   0,
   0,
   0, //CMD45
   0,
   0,
   0,
   0,
   0,//CMD50
   REG_SDHC_XFERTYP_CMDINX(SDHC_ACMD51) | REG_SDHC_XFERTYP_CICEN_MASK | REG_SDHC_XFERTYP_CCCEN_MASK | REG_SDHC_XFERTYP_RSPTYP(SDHC_XFERTYP_RSPTYP_48),
   REG_SDHC_XFERTYP_CMDINX(52) | REG_SDHC_XFERTYP_CICEN_MASK | REG_SDHC_XFERTYP_CCCEN_MASK | REG_SDHC_XFERTYP_RSPTYP(SDHC_XFERTYP_RSPTYP_48),
   REG_SDHC_XFERTYP_CMDINX(53) | REG_SDHC_XFERTYP_CICEN_MASK | REG_SDHC_XFERTYP_CCCEN_MASK | REG_SDHC_XFERTYP_RSPTYP(SDHC_XFERTYP_RSPTYP_48),
   0,
   REG_SDHC_XFERTYP_CMDINX(55) | REG_SDHC_XFERTYP_CICEN_MASK | REG_SDHC_XFERTYP_CCCEN_MASK | REG_SDHC_XFERTYP_RSPTYP(SDHC_XFERTYP_RSPTYP_48), //CMD55
   REG_SDHC_XFERTYP_CMDINX(56) | REG_SDHC_XFERTYP_CICEN_MASK | REG_SDHC_XFERTYP_CCCEN_MASK | REG_SDHC_XFERTYP_RSPTYP(SDHC_XFERTYP_RSPTYP_48BUSY),
   0,
   0,
   0,
   REG_SDHC_XFERTYP_CMDINX(60) | REG_SDHC_XFERTYP_CICEN_MASK | REG_SDHC_XFERTYP_CCCEN_MASK | REG_SDHC_XFERTYP_RSPTYP(SDHC_XFERTYP_RSPTYP_48BUSY),//CMD60
   REG_SDHC_XFERTYP_CMDINX(61) | REG_SDHC_XFERTYP_CICEN_MASK | REG_SDHC_XFERTYP_CCCEN_MASK | REG_SDHC_XFERTYP_RSPTYP(SDHC_XFERTYP_RSPTYP_48BUSY),
   0,
   //ACMD
   0
};
/*===========================================
Implementation
=============================================*/
/*-------------------------------------------
| Name:dev_k60n512_sdhc_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_sdhc_load(void) {
   volatile unsigned int reg_val = 0;
      
   //
   g_kinetis_sdhc_info.sdhc_base = 0x400b1000;
   g_kinetis_sdhc_info._desc_rd = -1;
	g_kinetis_sdhc_info._desc_wr = -1;
   
   //enable clocking
   _kinetis_sdhc_configure_pins(0);

   //reset
   reg_val = REG_SDHC_SYSCTL_RSTA_MASK | REG_SDHC_SYSCTL_SDCLKFS(0x80);
   HAL_WRITE_UINT32(g_kinetis_sdhc_info.sdhc_base + REG_SDHC_SYSCTL, reg_val);
   do {
      HAL_READ_UINT32(g_kinetis_sdhc_info.sdhc_base + REG_SDHC_SYSCTL, reg_val);
   } while(reg_val & REG_SDHC_SYSCTL_RSTA_MASK);
   
   //initial values
   HAL_WRITE_UINT32(g_kinetis_sdhc_info.sdhc_base + REG_SDHC_VENDOR, 0);
   HAL_WRITE_UINT32(g_kinetis_sdhc_info.sdhc_base + REG_SDHC_BLKATTR, 
   REG_SDHC_BLKATTR_BLKCNT(1) | REG_SDHC_BLKATTR_BLKSIZE(512));
   HAL_WRITE_UINT32(g_kinetis_sdhc_info.sdhc_base + REG_SDHC_PROCTL,
   REG_SDHC_PROCTL_EMODE(SDHC_PROCTL_EMODE_INVARIANT) | REG_SDHC_PROCTL_D3CD_MASK);
   
   //set baudrate for initialization
   _kinetis_sdhc_setbaud(&g_kinetis_sdhc_info, 96000000, KINETIS_SDHC_INIT_SPEED);
   do {
      HAL_READ_UINT32(g_kinetis_sdhc_info.sdhc_base + REG_SDHC_PRSSTAT, reg_val);
   } while(reg_val & (REG_SDHC_PRSSTAT_CIHB_MASK | REG_SDHC_PRSSTAT_CDIHB_MASK));
   
   //configure port PINS and enable clocking again
   _kinetis_sdhc_configure_pins(1);
   
   return 0;
}

/*-------------------------------------------
| Name:dev_k60n512_sdhc_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_sdhc_open(desc_t desc, int o_flag){
   volatile unsigned int reg_val = 0;
	
	if(o_flag & O_RDONLY){
		if(g_kinetis_sdhc_info._desc_rd<0) {
			g_kinetis_sdhc_info._desc_rd = desc;
		}
		else
			return -1; //already open
	}

	if(o_flag & O_WRONLY){
		if(g_kinetis_sdhc_info._desc_wr<0) {
			g_kinetis_sdhc_info._desc_wr = desc;
		}
		else
			return -1; //already open
	}
	
	if(!ofile_lst[desc].p)
		ofile_lst[desc].p=&g_kinetis_sdhc_info;

		//unmask IRQ and enable ksz
	if(g_kinetis_sdhc_info._desc_rd>=0 && g_kinetis_sdhc_info._desc_wr>=0) {
      kernel_sem_init(&g_kinetis_sdhc_info.cmd_completed, 0, 0);
      g_kinetis_sdhc_info.post_event = 0;
      
      cyg_interrupt_create((cyg_vector_t)KINETIS_SDHC_IRQ_NO,
				KINETIS_SDHC_IRQ_PRIORITY,
				// Data item passed to interrupt handler
				(cyg_addrword_t)&g_kinetis_sdhc_info,   
				_kinetis_sdhc_isr,
				_kinetis_sdhc_dsr,
				&g_kinetis_sdhc_info._irq_handle,
				&g_kinetis_sdhc_info._irq_it);
		
		
		cyg_interrupt_attach(g_kinetis_sdhc_info._irq_handle);
      
      //enable requests
      //HAL_WRITE_UINT32(g_kinetis_sdhc_info.sdhc_base + REG_SDHC_IRQSTAT, 0xffff);
      HAL_WRITE_UINT32(g_kinetis_sdhc_info.sdhc_base + REG_SDHC_IRQSTAT, 0x0);
      HAL_WRITE_UINT32(g_kinetis_sdhc_info.sdhc_base + REG_SDHC_IRQSTATEN, KINETIS_SDHC_ALLOWED_IRQS);
      
      //80 initial clocks
      HAL_READ_UINT32(g_kinetis_sdhc_info.sdhc_base + REG_SDHC_SYSCTL, reg_val);
      reg_val |= REG_SDHC_SYSCTL_INITA_MASK;
      HAL_WRITE_UINT32(g_kinetis_sdhc_info.sdhc_base + REG_SDHC_SYSCTL, reg_val);
      
      do {
         HAL_READ_UINT32(g_kinetis_sdhc_info.sdhc_base + REG_SDHC_SYSCTL, reg_val);
      } while(reg_val & REG_SDHC_SYSCTL_INITA_MASK);

      //maybe check sdcard presence
      
      //set card removal irq
      HAL_READ_UINT32(g_kinetis_sdhc_info.sdhc_base + REG_SDHC_IRQSTAT, reg_val);
      reg_val |= REG_SDHC_IRQSTAT_CRM_MASK;
      HAL_WRITE_UINT32(g_kinetis_sdhc_info.sdhc_base + REG_SDHC_IRQSTAT, reg_val);
      
		cyg_interrupt_unmask((cyg_vector_t)KINETIS_SDHC_IRQ_NO);
	}
   
   return 0;
}

/*-------------------------------------------
| Name:dev_k60n512_sdhc_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_sdhc_close(desc_t desc){
   return 0;
}

/*-------------------------------------------
| Name:dev_k60n512_sdhc_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_sdhc_read(desc_t desc, char* buf,int size){
   return size;
}

/*-------------------------------------------
| Name:dev_k60n512_sdhc_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_sdhc_write(desc_t desc, const char* buf,int size){
   return size;
}

/*-------------------------------------------
| Name:dev_k60n512_sdhc_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_sdhc_seek(desc_t desc,int offset,int origin){
   return -1;
}

/*-------------------------------------------
| Name:dev_k60n512_sdhc_ioctl
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_sdhc_ioctl(desc_t desc,int request,va_list ap) {
   switch(request) {
      case HDSD_SENDCOMMAND: {
         mci_cmd_t * command = (mci_cmd_t *)ap;
         board_kinetis_sdhc_info_t * p_inf_sdhc = (board_kinetis_sdhc_info_t *)ofile_lst[desc].p;
         
         if(!p_inf_sdhc)
            return -1;
         
         p_inf_sdhc->command = command;
         
         if(_kinetis_sdhc_send_command(p_inf_sdhc) < 0)
            return -1;
         
         //wait for irq
         kernel_sem_wait(&p_inf_sdhc->cmd_completed);
         
         return 0;
      }
      break;

      case HDSD_CHECKBUSY: {
         return 0;
      }
      break;

      case HDSD_SETSPEED: {
            board_kinetis_sdhc_info_t * p_inf_sdhc = (board_kinetis_sdhc_info_t *)ofile_lst[desc].p;
            unsigned int reg_val = 0;
            
            _kinetis_sdhc_configure_pins(0);
            
            //increase frequency
            _kinetis_sdhc_setbaud(p_inf_sdhc, 96000000, KINETIS_SDHC_NORMAL_SPEED);
            
            //4bits width
            HAL_READ_UINT32(g_kinetis_sdhc_info.sdhc_base + REG_SDHC_PROCTL, reg_val);
            reg_val &= (~REG_SDHC_PROCTL_DTW_MASK);
            reg_val |= REG_SDHC_PROCTL_DTW(SDHC_PROCTL_DTW_4BIT);
            HAL_WRITE_UINT32(g_kinetis_sdhc_info.sdhc_base + REG_SDHC_PROCTL, reg_val);
            
            _kinetis_sdhc_configure_pins(1);
            return 0;
         }
      break;

      default:
         return -1;
      break;
   }

   return 0;
}

/*-------------------------------------------
| Name:_kinetis_send_command
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _kinetis_sdhc_send_command(board_kinetis_sdhc_info_t * p_inf_sdhc) {
   unsigned int xfertyp = _kinetis_sdhc_supported_command[p_inf_sdhc->command->cmd & 0x3f];
   unsigned int reg_val;
   //check command
   if(!xfertyp && p_inf_sdhc->command->cmd!= 0) {
      return -1;
   }
   
   //card removal check
   HAL_READ_UINT32(g_kinetis_sdhc_info.sdhc_base + REG_SDHC_IRQSTAT, reg_val);
   reg_val |= REG_SDHC_IRQSTAT_CRM_MASK;
   HAL_WRITE_UINT32(g_kinetis_sdhc_info.sdhc_base + REG_SDHC_IRQSTAT, reg_val);
   
   do {
      HAL_READ_UINT32(g_kinetis_sdhc_info.sdhc_base + REG_SDHC_PRSSTAT, reg_val);
   } while((reg_val & REG_SDHC_PRSSTAT_CIHB_MASK)
      || (reg_val & REG_SDHC_PRSSTAT_CDIHB_MASK));
   
   //setup command
   HAL_WRITE_UINT32(g_kinetis_sdhc_info.sdhc_base + REG_SDHC_CMDARG, p_inf_sdhc->command->arg);
   xfertyp &= (~REG_SDHC_XFERTYP_CMDTYP_MASK);
   xfertyp |= REG_SDHC_XFERTYP_CMDTYP(0);
   
   //block count
   HAL_READ_UINT32(g_kinetis_sdhc_info.sdhc_base + REG_SDHC_BLKATTR, reg_val);
   reg_val &= (~ REG_SDHC_BLKATTR_BLKCNT_MASK);
   HAL_WRITE_UINT32(g_kinetis_sdhc_info.sdhc_base + REG_SDHC_BLKATTR, reg_val);
   
   if(p_inf_sdhc->command->block_count > 0) {
      xfertyp |= REG_SDHC_XFERTYP_DPSEL_MASK;
      
      if(p_inf_sdhc->command->is_read) {
         xfertyp |= REG_SDHC_XFERTYP_DTDSEL_MASK;
      }
      
      if(p_inf_sdhc->command->block_count > 1) {
         xfertyp |= REG_SDHC_XFERTYP_MSBSEL_MASK | REG_SDHC_XFERTYP_AC12EN_MASK;
      }
      
      HAL_READ_UINT32(g_kinetis_sdhc_info.sdhc_base + REG_SDHC_BLKATTR, reg_val);
      reg_val &= (~REG_SDHC_BLKATTR_BLKCNT_MASK);
      reg_val |= REG_SDHC_BLKATTR_BLKCNT(p_inf_sdhc->command->block_count);
      HAL_WRITE_UINT32(g_kinetis_sdhc_info.sdhc_base + REG_SDHC_BLKATTR, reg_val);
      
      xfertyp |= REG_SDHC_XFERTYP_BCEN_MASK;
      
      //enable DMA
      HAL_WRITE_UINT32(g_kinetis_sdhc_info.sdhc_base + REG_SDHC_DSADDR, 
      (unsigned int)p_inf_sdhc->command->p_data);
      xfertyp |= REG_SDHC_XFERTYP_DMAEN_MASK;
   }
   else {
      HAL_WRITE_UINT32(g_kinetis_sdhc_info.sdhc_base + REG_SDHC_DSADDR, 0);
   }
   
   //issue command
   HAL_WRITE_UINT32(g_kinetis_sdhc_info.sdhc_base + REG_SDHC_XFERTYP, xfertyp);
   
   //allowed irq generation
   HAL_WRITE_UINT32(g_kinetis_sdhc_info.sdhc_base + REG_SDHC_IRQSIGEN, KINETIS_SDHC_ALLOWED_IRQS);
       
   return 0;
}

/*-------------------------------------------
| Name:_kinetis_sdhc_isr
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
cyg_uint32 _kinetis_sdhc_isr(cyg_vector_t vector, cyg_addrword_t data) {
   volatile unsigned int irq_stat = 0, irq_mask = 0, reg_val = 0;
   board_kinetis_sdhc_info_t * p_inf_sdhc = (board_kinetis_sdhc_info_t *)data;
   
   cyg_interrupt_mask((cyg_vector_t)KINETIS_SDHC_IRQ_NO);
   
   if(!p_inf_sdhc)
      return CYG_ISR_HANDLED;
   
   irq_stat = HAL_READ_UINT32(p_inf_sdhc->sdhc_base + REG_SDHC_IRQSTAT, irq_stat);
   irq_mask = HAL_READ_UINT32(p_inf_sdhc->sdhc_base + REG_SDHC_IRQSTATEN, irq_mask);
   
   irq_stat &= irq_mask;
   
   if((irq_stat & KINETIS_SDHC_ERRORS_IRQS)) {
      if((irq_stat & REG_SDHC_IRQSTAT_CTOE_MASK) != 0) {
         HAL_WRITE_UINT32(p_inf_sdhc->sdhc_base + REG_SDHC_IRQSTAT, REG_SDHC_IRQSTAT_CTOE_MASK);
         p_inf_sdhc->command->status = SD_LOW_LEVEL_TIMEOUT_ERROR;
      }
      if((irq_stat & REG_SDHC_IRQSTAT_CCE_MASK) != 0) {
         HAL_WRITE_UINT32(p_inf_sdhc->sdhc_base + REG_SDHC_IRQSTAT, REG_SDHC_IRQSTAT_CCE_MASK);
         p_inf_sdhc->command->status = SD_LOW_LEVEL_CRC_ERROR;
      }
      if((irq_stat & REG_SDHC_IRQSTAT_CEBE_MASK) != 0) {
         HAL_WRITE_UINT32(p_inf_sdhc->sdhc_base + REG_SDHC_IRQSTAT, REG_SDHC_IRQSTAT_CEBE_MASK);
         p_inf_sdhc->command->status = SD_LOW_LEVEL_ERROR;
      }
      if((irq_stat & REG_SDHC_IRQSTAT_CIE_MASK) != 0) {
         HAL_WRITE_UINT32(p_inf_sdhc->sdhc_base + REG_SDHC_IRQSTAT, REG_SDHC_IRQSTAT_CIE_MASK);
         p_inf_sdhc->command->status = SD_LOW_LEVEL_ERROR;
      }
      if((irq_stat & REG_SDHC_IRQSTAT_DTOE_MASK) != 0) {
         HAL_WRITE_UINT32(p_inf_sdhc->sdhc_base + REG_SDHC_IRQSTAT, REG_SDHC_IRQSTAT_DTOE_MASK);
         p_inf_sdhc->command->status = SD_LOW_LEVEL_TIMEOUT_ERROR;
      }
      if((irq_stat & REG_SDHC_IRQSTAT_DCE_MASK) != 0) {
         HAL_WRITE_UINT32(p_inf_sdhc->sdhc_base + REG_SDHC_IRQSTAT, REG_SDHC_IRQSTAT_DCE_MASK);
         p_inf_sdhc->command->status = SD_LOW_LEVEL_CRC_ERROR;
      }
      if((irq_stat & REG_SDHC_IRQSTAT_DEBE_MASK) != 0) {
         HAL_WRITE_UINT32(p_inf_sdhc->sdhc_base + REG_SDHC_IRQSTAT, REG_SDHC_IRQSTAT_DEBE_MASK);
         p_inf_sdhc->command->status = SD_LOW_LEVEL_ERROR;
      }
      if((irq_stat & REG_SDHC_IRQSTAT_AC12E_MASK) != 0) {
         HAL_WRITE_UINT32(p_inf_sdhc->sdhc_base + REG_SDHC_IRQSTAT, REG_SDHC_IRQSTAT_AC12E_MASK);
         p_inf_sdhc->command->status = SD_LOW_LEVEL_ERROR;
      }
      if((irq_stat & REG_SDHC_IRQSTAT_DMAE_MASK) != 0) {
         HAL_WRITE_UINT32(p_inf_sdhc->sdhc_base + REG_SDHC_IRQSTAT, REG_SDHC_IRQSTAT_DMAE_MASK);
         p_inf_sdhc->command->status = SD_LOW_LEVEL_ERROR;
      }
      p_inf_sdhc->post_event = 1;
      g_kinetis_sdhc_stat.error++;
   }
   else {
      //command ok
      if((irq_stat & REG_SDHC_IRQSTAT_CC_MASK) != 0) {
         HAL_WRITE_UINT32(p_inf_sdhc->sdhc_base + REG_SDHC_IRQSTAT, REG_SDHC_IRQSTAT_CC_MASK);
         
         
         if(p_inf_sdhc->command->block_count > 0) {
            HAL_WRITE_UINT32(g_kinetis_sdhc_info.sdhc_base + REG_SDHC_IRQSIGEN, KINETIS_SDHC_ALLOWED_IRQS);
         }
         else {
            //just a simple command
            p_inf_sdhc->post_event = 1;
            g_kinetis_sdhc_stat.cc++;
         }
      }
      
      //transfert ok
      if((irq_stat & REG_SDHC_IRQSTAT_TC_MASK) != 0) {
         HAL_WRITE_UINT32(p_inf_sdhc->sdhc_base + REG_SDHC_IRQSTAT, REG_SDHC_IRQSTAT_TC_MASK);
         p_inf_sdhc->post_event = 1;
         g_kinetis_sdhc_stat.tc++;
      }
      
      //dma ok
      if((irq_stat & REG_SDHC_IRQSTAT_DINT_MASK) != 0) {
         HAL_WRITE_UINT32(p_inf_sdhc->sdhc_base + REG_SDHC_IRQSTAT, REG_SDHC_IRQSTAT_DINT_MASK);
         
         //stop DMA
         #if 0
         HAL_READ_UINT32(p_inf_sdhc->sdhc_base + REG_SDHC_SYSCTL, reg_val);
         reg_val |= REG_SDHC_SYSCTL_RSTD_MASK;
         HAL_WRITE_UINT32(p_inf_sdhc->sdhc_base + REG_SDHC_SYSCTL, reg_val);
         #endif
         
         g_kinetis_sdhc_stat.dint++;
      }
      
      //card removed
      if((irq_stat & REG_SDHC_IRQSTAT_CRM_MASK) != 0) {
         HAL_WRITE_UINT32(p_inf_sdhc->sdhc_base + REG_SDHC_IRQSTAT, REG_SDHC_IRQSTAT_CRM_MASK);
      }
   }
   
	return CYG_ISR_HANDLED | CYG_ISR_CALL_DSR;
}

/*-------------------------------------------
| Name:_kinetis_sdhc_dsr
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void _kinetis_sdhc_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data) {
   board_kinetis_sdhc_info_t * p_inf_sdhc = (board_kinetis_sdhc_info_t *)data;
   unsigned int tmp_val=0;
   if(!p_inf_sdhc)
      return;
   
   if(p_inf_sdhc->post_event) {
      //read response
      if(p_inf_sdhc->command->p_resp) {
         switch (p_inf_sdhc->command->resp_type) {
            case R1:
               HAL_READ_UINT32(p_inf_sdhc->sdhc_base + REG_SDHC_CMDRSP0,
               p_inf_sdhc->command->p_resp[0]);
            break;
            
            case R2:
               //construct CSD for upper layer (aka /dev/sd) for CMD9
               if(p_inf_sdhc->command->cmd == SEND_CSD) {
                  HAL_READ_UINT32(p_inf_sdhc->sdhc_base + REG_SDHC_CMDRSP3,
                  p_inf_sdhc->command->p_resp[0]);
                  HAL_READ_UINT32(p_inf_sdhc->sdhc_base + REG_SDHC_CMDRSP2,
                  tmp_val);
                  tmp_val &= 0xff000000;
                  p_inf_sdhc->command->p_resp[0] = (p_inf_sdhc->command->p_resp[0] << 8) | (tmp_val >> 24);
                  
                  HAL_READ_UINT32(p_inf_sdhc->sdhc_base + REG_SDHC_CMDRSP2,
                  p_inf_sdhc->command->p_resp[1]);
                  HAL_READ_UINT32(p_inf_sdhc->sdhc_base + REG_SDHC_CMDRSP1,
                  tmp_val);
                  tmp_val &= 0xff000000;
                  p_inf_sdhc->command->p_resp[1] = (p_inf_sdhc->command->p_resp[1] << 8) | (tmp_val >> 24);
                  
                  HAL_READ_UINT32(p_inf_sdhc->sdhc_base + REG_SDHC_CMDRSP1,
                  p_inf_sdhc->command->p_resp[2]);
                  HAL_READ_UINT32(p_inf_sdhc->sdhc_base + REG_SDHC_CMDRSP0,
                  tmp_val);
                  tmp_val &= 0xff000000;
                  p_inf_sdhc->command->p_resp[2] = (p_inf_sdhc->command->p_resp[2] << 8) | (tmp_val >> 24);
                  
                  HAL_READ_UINT32(p_inf_sdhc->sdhc_base + REG_SDHC_CMDRSP0,
                  tmp_val);
                  p_inf_sdhc->command->p_resp[3] = (tmp_val << 8);
               }
               else {
                  HAL_READ_UINT32(p_inf_sdhc->sdhc_base + REG_SDHC_CMDRSP3,
                  p_inf_sdhc->command->p_resp[0]);
                  HAL_READ_UINT32(p_inf_sdhc->sdhc_base + REG_SDHC_CMDRSP2,
                  p_inf_sdhc->command->p_resp[1]);
                  HAL_READ_UINT32(p_inf_sdhc->sdhc_base + REG_SDHC_CMDRSP1,
                  p_inf_sdhc->command->p_resp[2]);
                  HAL_READ_UINT32(p_inf_sdhc->sdhc_base + REG_SDHC_CMDRSP0,
                  p_inf_sdhc->command->p_resp[3]);
               }
            break;

            case R3:
            case R1b:
            case R6:
            case R7:
               HAL_READ_UINT32(p_inf_sdhc->sdhc_base + REG_SDHC_CMDRSP0,
               p_inf_sdhc->command->p_resp[0]);
            break;

            default:
            break;
         }
      }
      
      //
      p_inf_sdhc->post_event = 0;
      kernel_sem_post(&p_inf_sdhc->cmd_completed);
   }
   
   cyg_interrupt_unmask((cyg_vector_t)KINETIS_SDHC_IRQ_NO);
}

/*-------------------------------------------
| Name:_kinetis_sdhc_baudrate
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void _kinetis_sdhc_setbaud(board_kinetis_sdhc_info_t * p_inf_sdhc, unsigned int master_clock, 
unsigned int baudrate) {
   unsigned int pres, min, minpres = 0x80, mindiv = 0x0F;
   int val;
   volatile unsigned int div;
   volatile int reg_val;
   
   /* Find closest setting */
   min = (unsigned int)-1;
   for(pres = 2; pres <= 256; pres <<= 1) {
      for(div = 1; div <= 16; div++) {
         val = pres * div * baudrate - master_clock;
         if(val >= 0) {
            if(min > val) {
              min = val;
              minpres = pres;
              mindiv = div;
            }
         }
      }
   }
   
   /* Disable ESDHC clocks */
   HAL_READ_UINT32(p_inf_sdhc->sdhc_base + REG_SDHC_SYSCTL, reg_val);
   reg_val &= (~REG_SDHC_SYSCTL_SDCLKEN_MASK);
   HAL_WRITE_UINT32(p_inf_sdhc->sdhc_base + REG_SDHC_SYSCTL, reg_val);

   /* Change dividers */
   HAL_READ_UINT32(p_inf_sdhc->sdhc_base + REG_SDHC_SYSCTL, div);
   div &= (~ (REG_SDHC_SYSCTL_DTOCV_MASK | REG_SDHC_SYSCTL_SDCLKFS_MASK 
   | REG_SDHC_SYSCTL_DVS_MASK));
   div |= (REG_SDHC_SYSCTL_DTOCV(0x0E) | REG_SDHC_SYSCTL_SDCLKFS(minpres >> 1) 
   | REG_SDHC_SYSCTL_DVS(mindiv - 1));
   HAL_WRITE_UINT32(p_inf_sdhc->sdhc_base + REG_SDHC_SYSCTL, div);
   
   /* Wait for stable clock */
   do {
      HAL_DELAY_US(1);
      HAL_READ_UINT32(p_inf_sdhc->sdhc_base + REG_SDHC_PRSSTAT, reg_val);
   } while((reg_val & REG_SDHC_PRSSTAT_SDSTB_MASK) == 0);

   /* Enable ESDHC clocks */
   HAL_READ_UINT32(p_inf_sdhc->sdhc_base + REG_SDHC_SYSCTL, reg_val);
   reg_val |= REG_SDHC_SYSCTL_SDCLKEN_MASK;
   HAL_WRITE_UINT32(p_inf_sdhc->sdhc_base + REG_SDHC_SYSCTL, reg_val);
   
   HAL_READ_UINT32(p_inf_sdhc->sdhc_base + REG_SDHC_IRQSTAT, reg_val);
   reg_val |= REG_SDHC_IRQSTAT_DTOE_MASK;
   HAL_WRITE_UINT32(p_inf_sdhc->sdhc_base + REG_SDHC_IRQSTAT, reg_val);
}

/*-------------------------------------------
| Name:_kinetis_sdhc_configure_pins
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void _kinetis_sdhc_configure_pins(unsigned char pin_flag) {
   unsigned int reg_val;
   
   if(pin_flag) {
      hal_set_pin_function(SDHC_CLK);
      hal_set_pin_function(SDHC_CMD);
      hal_set_pin_function(SDHC_D0);
      hal_set_pin_function(SDHC_D1);
      hal_set_pin_function(SDHC_D2);
      hal_set_pin_function(SDHC_D3);
   }
   
   HAL_READ_UINT32(REG_SIM_SCGC3_ADDR, reg_val);
   reg_val |= REG_SIM_SCGC3_SDHC_MASK;
   HAL_WRITE_UINT32(REG_SIM_SCGC3_ADDR, reg_val);
}
/*============================================
| End of Source  : dev_k60n512_sdhc.c
==============================================*/

