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
#include "kernel/core/ioctl_hd.h"

#include "kernel/fs/vfs/vfsdev.h"

#include "lib/libc/ctype/ctype.h"
#include "kernel/core/types.h"
#include "kernel/core/time.h"
#include "kernel/core/devio.h"
#include "kernel/core/errno.h"

#include "dev_at91_mci.h"

#include "kernel/dev/arch/all/sd/dev_sd.h"
/*============================================
| Global Declaration
==============================================*/
int dev_at91_mci_load(board_inf_mci_t * p_inf_mci, int master_clk);
int dev_at91_mci_open(desc_t desc, int o_flag);
int dev_at91_mci_close(desc_t desc);
int dev_at91_mci_read(desc_t desc, char* buf,int cb);
int dev_at91_mci_write(desc_t desc, const char* buf,int cb);
int dev_at91_mci_seek(desc_t desc,int offset,int origin);
int dev_at91_mci_ioctl(desc_t desc,int request,va_list ap);

#if defined(__KERNEL_UCORE_ECOS)
static cyg_uint32 dev_at91_mci_isr(cyg_vector_t vector, cyg_addrword_t data);
static void dev_at91_mci_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data);
static void dev_at91_mci_timer_callback(alrm_hdl_t alarm_handle, cyg_addrword_t data);
static cyg_interrupt dev_at91_intr_it;
static cyg_handle_t dev_at91_intr_handle;
#endif

//
static kernel_sem_t g_sem_mci __attribute__ ((section (".no_cache")));
board_inf_mci_t * g_p_inf_mci __attribute__ ((section (".no_cache")));

//
static int _at91_mci_send_cmd(board_inf_mci_t * p_inf_mci, mci_cmd_t * mci_cmd);
static int _at91_mci_set_speed(board_inf_mci_t * p_inf_mci, unsigned int speed);

static unsigned int mci_supported_command[] = {
      0 | AT91_MCI_CMDR_TRCMD_VAL(0) | AT91_MCI_CMDR_SPCMD_VAL(0), //GO_IDLE_STATE
      1 | AT91_MCI_CMDR_TRCMD_VAL(0) | AT91_MCI_CMDR_SPCMD_VAL(0) | AT91_MCI_CMDR_RSPTYP_VAL(0x1) | AT91_MCI_CMDR_OPDCMD, //SEND_OP_COND
      2 | AT91_MCI_CMDR_TRCMD_VAL(0) | AT91_MCI_CMDR_SPCMD_VAL(0) | AT91_MCI_CMDR_RSPTYP_VAL(0x2), //ALL_SEND_CID
      3 | AT91_MCI_CMDR_TRCMD_VAL(0) | AT91_MCI_CMDR_SPCMD_VAL(0) | AT91_MCI_CMDR_RSPTYP_VAL(0x1) | AT91_MCI_CMDR_MAXLAT, //SEND_RELATIVE_ADDRESS
      4 | AT91_MCI_CMDR_TRCMD_VAL(0) | AT91_MCI_CMDR_SPCMD_VAL(0) | AT91_MCI_CMDR_RSPTYP_VAL(0) | AT91_MCI_CMDR_MAXLAT, //SET_DSR
      UNSUPPORTED_COMMAND, //IO_SEND_OP_COND
      UNSUPPORTED_COMMAND, //SWITCH_FUNC
      7 | AT91_MCI_CMDR_TRCMD_VAL(0) | AT91_MCI_CMDR_SPCMD_VAL(0) | AT91_MCI_CMDR_RSPTYP_VAL(0x1) | AT91_MCI_CMDR_MAXLAT, //SELECT/DESELECT_CARD
      8 | AT91_MCI_CMDR_TRCMD_VAL(0) | AT91_MCI_CMDR_SPCMD_VAL(0) | AT91_MCI_CMDR_RSPTYP_VAL(0x1) | AT91_MCI_CMDR_MAXLAT, //SEND_IF_COND
      9 | AT91_MCI_CMDR_TRCMD_VAL(0) | AT91_MCI_CMDR_SPCMD_VAL(0) | AT91_MCI_CMDR_RSPTYP_VAL(0x2) | AT91_MCI_CMDR_MAXLAT, //SEND_CSD
      10 | AT91_MCI_CMDR_TRCMD_VAL(0) | AT91_MCI_CMDR_SPCMD_VAL(0) | AT91_MCI_CMDR_RSPTYP_VAL(0x2) | AT91_MCI_CMDR_MAXLAT, //SEND_CID
      UNSUPPORTED_COMMAND, //
      12 | AT91_MCI_CMDR_TRCMD_VAL(0x2) | AT91_MCI_CMDR_SPCMD_VAL(0) | AT91_MCI_CMDR_RSPTYP_VAL(0x1)  | AT91_MCI_CMDR_MAXLAT, //STOP_TRANSMISSION
      13 | AT91_MCI_CMDR_TRCMD_VAL(0) | AT91_MCI_CMDR_SPCMD_VAL(0) | AT91_MCI_CMDR_RSPTYP_VAL(0x1)  | AT91_MCI_CMDR_MAXLAT, //SEND_STATUS
      UNSUPPORTED_COMMAND, //
      15 | AT91_MCI_CMDR_RSPTYP_VAL(0) , //GO_INACTIVE_STATE
      16 | AT91_MCI_CMDR_TRCMD_VAL(0) | AT91_MCI_CMDR_SPCMD_VAL(0) | AT91_MCI_CMDR_RSPTYP_VAL(0x1) | AT91_MCI_CMDR_MAXLAT, //SET_BLOCKLEN
      17 | AT91_MCI_CMDR_TRCMD_VAL(1) | AT91_MCI_CMDR_RSPTYP_VAL(0x1) | AT91_MCI_CMDR_TRTYP_VAL(0) | AT91_MCI_CMDR_TRDIR | AT91_MCI_CMDR_MAXLAT, //READ_SINGLE_BLOCK
      18 | AT91_MCI_CMDR_TRCMD_VAL(1) | AT91_MCI_CMDR_RSPTYP_VAL(0x1) | AT91_MCI_CMDR_TRTYP_VAL(0x1)  | AT91_MCI_CMDR_TRDIR   | AT91_MCI_CMDR_MAXLAT, //READ_MULTIPLE_BLOCK
      UNSUPPORTED_COMMAND, //
      UNSUPPORTED_COMMAND, //20
      UNSUPPORTED_COMMAND, //
      UNSUPPORTED_COMMAND, //
      UNSUPPORTED_COMMAND, //
      24 | AT91_MCI_CMDR_TRCMD_VAL(0x1) | AT91_MCI_CMDR_RSPTYP_VAL(0x1) | (AT91_MCI_CMDR_TRTYP_VAL(0) & ~(AT91_MCI_CMDR_TRDIR))  | AT91_MCI_CMDR_MAXLAT, //WRITE_BLOCK
      25 | AT91_MCI_CMDR_TRCMD_VAL(0x1) | AT91_MCI_CMDR_RSPTYP_VAL(0x1) | (AT91_MCI_CMDR_TRTYP_VAL(0x1) &  ~(AT91_MCI_CMDR_TRDIR))  | AT91_MCI_CMDR_MAXLAT, //WRITE_MULTIPLE_BLOCK
      UNSUPPORTED_COMMAND, //
      UNSUPPORTED_COMMAND, //
      UNSUPPORTED_COMMAND, //
      UNSUPPORTED_COMMAND, //
      UNSUPPORTED_COMMAND, //30
      UNSUPPORTED_COMMAND, //
      UNSUPPORTED_COMMAND, //
      UNSUPPORTED_COMMAND, //
      UNSUPPORTED_COMMAND, //
      UNSUPPORTED_COMMAND, //35
      UNSUPPORTED_COMMAND, //
      UNSUPPORTED_COMMAND, //
      UNSUPPORTED_COMMAND, //
      UNSUPPORTED_COMMAND, //
      UNSUPPORTED_COMMAND, //40
      UNSUPPORTED_COMMAND, //
      UNSUPPORTED_COMMAND, //
      UNSUPPORTED_COMMAND, //
      UNSUPPORTED_COMMAND, //
      UNSUPPORTED_COMMAND, //45
      UNSUPPORTED_COMMAND, //
      UNSUPPORTED_COMMAND, //
      UNSUPPORTED_COMMAND, //
      UNSUPPORTED_COMMAND, //
      UNSUPPORTED_COMMAND, //50
      UNSUPPORTED_COMMAND, //
      UNSUPPORTED_COMMAND, //
      UNSUPPORTED_COMMAND, //
      UNSUPPORTED_COMMAND, //
      55 | AT91_MCI_CMDR_SPCMD_VAL(0) | AT91_MCI_CMDR_RSPTYP_VAL(0x1) | AT91_MCI_CMDR_TRCMD_VAL(0) | AT91_MCI_CMDR_MAXLAT, //APP_CMD
      UNSUPPORTED_COMMAND, //
      UNSUPPORTED_COMMAND, //
      58 | AT91_MCI_CMDR_SPCMD_VAL(0) | AT91_MCI_CMDR_RSPTYP_VAL(0x1) | AT91_MCI_CMDR_MAXLAT, //SPI
      59 | AT91_MCI_CMDR_SPCMD_VAL(0) | AT91_MCI_CMDR_RSPTYP_VAL(0x1) | AT91_MCI_CMDR_MAXLAT, //SPI
      UNSUPPORTED_COMMAND, //60
      UNSUPPORTED_COMMAND,
      UNSUPPORTED_COMMAND,
      UNSUPPORTED_COMMAND,
      //DUMMY FOR POWER ON INIT
      (AT91_MCI_CMDR_TRCMD_VAL(0) | AT91_MCI_CMDR_SPCMD_VAL(0x1) | AT91_MCI_CMDR_OPDCMD), //64
      //ACMD command begin
      UNSUPPORTED_COMMAND, //65 (64+1)
      UNSUPPORTED_COMMAND,
      UNSUPPORTED_COMMAND,
      UNSUPPORTED_COMMAND,
      UNSUPPORTED_COMMAND,
      6  | AT91_MCI_CMDR_SPCMD_VAL(0) | AT91_MCI_CMDR_RSPTYP_VAL(0x1) | AT91_MCI_CMDR_TRCMD_VAL(0) | AT91_MCI_CMDR_MAXLAT, //SET_BUS_WIDTH, //70
      UNSUPPORTED_COMMAND,
      UNSUPPORTED_COMMAND,
      UNSUPPORTED_COMMAND,
      UNSUPPORTED_COMMAND,
      UNSUPPORTED_COMMAND, //75
      UNSUPPORTED_COMMAND,
      UNSUPPORTED_COMMAND,
      UNSUPPORTED_COMMAND,
      UNSUPPORTED_COMMAND,
      UNSUPPORTED_COMMAND, //80
      UNSUPPORTED_COMMAND,
      UNSUPPORTED_COMMAND,
      UNSUPPORTED_COMMAND,
      UNSUPPORTED_COMMAND,
      UNSUPPORTED_COMMAND, //85
      UNSUPPORTED_COMMAND,
      UNSUPPORTED_COMMAND,
      UNSUPPORTED_COMMAND,
      UNSUPPORTED_COMMAND,
      UNSUPPORTED_COMMAND, //90
      UNSUPPORTED_COMMAND,
      UNSUPPORTED_COMMAND,
      UNSUPPORTED_COMMAND,
      UNSUPPORTED_COMMAND,
      UNSUPPORTED_COMMAND, //95
      UNSUPPORTED_COMMAND,
      UNSUPPORTED_COMMAND,
      UNSUPPORTED_COMMAND,
      UNSUPPORTED_COMMAND,
      UNSUPPORTED_COMMAND, //100
      UNSUPPORTED_COMMAND,
      UNSUPPORTED_COMMAND,
      UNSUPPORTED_COMMAND,
      UNSUPPORTED_COMMAND,
      41 | AT91_MCI_CMDR_SPCMD_VAL(0) | AT91_MCI_CMDR_RSPTYP_VAL(0x1) | AT91_MCI_CMDR_TRCMD_VAL(0), //SD_APP_OP_COND
      UNSUPPORTED_COMMAND,
      UNSUPPORTED_COMMAND,
      UNSUPPORTED_COMMAND,
      UNSUPPORTED_COMMAND,
      UNSUPPORTED_COMMAND, //110
      UNSUPPORTED_COMMAND,
      UNSUPPORTED_COMMAND,
      UNSUPPORTED_COMMAND,
      UNSUPPORTED_COMMAND,
      51 | AT91_MCI_CMDR_SPCMD_VAL(0) | AT91_MCI_CMDR_RSPTYP_VAL(0x1) | AT91_MCI_CMDR_TRCMD_VAL(0) | AT91_MCI_CMDR_MAXLAT, //SEND_SCR
      UNSUPPORTED_COMMAND
};

#define  MCI_SPEED      (15000000)//(15000000)//(10000000)
static volatile unsigned int g_mci_master_clock;

#define  __reset_dma() { \
      MCI_WRITE_UINT32((unsigned int)(p_inf_mci->base_addr + MCI_PDC_PTCR), AT91_MCI_PDC_PTCR_RXTDIS | AT91_MCI_PDC_PTCR_TXTDIS); \
}

typedef struct mci_stat_st {
   unsigned int irq_txbufe;
   unsigned int irq_blke;
   unsigned int irq_notbusy;
   unsigned int irq_endrx;
   unsigned int irq_rxbuff;

   unsigned int irq_rinde;
   unsigned int irq_rdire;
   unsigned int irq_rcrce;
   unsigned int irq_rende;
   unsigned int irq_rtoe;
   unsigned int irq_dcrce;
   unsigned int irq_dtoe;
   unsigned int irq_ovre;
   unsigned int irq_unre;

   unsigned int dsr_count;
   unsigned int cmd_count;
} mci_stat_t;

static mci_stat_t mci_stat;
/*============================================
| Implementation
==============================================*/
/*-------------------------------------------
| Name:dev_at91_mci_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91_mci_load(board_inf_mci_t * p_inf_mci, int master_clk) {
   int clk_div;

   if(g_p_inf_mci)
      return -1;

   //reset the MCI, disable MCI irq
   MCI_WRITE_UINT32((unsigned int)(p_inf_mci->base_addr + MCI_CR), AT91_MCI_CR_SWRST);
   MCI_WRITE_UINT32((unsigned int)(p_inf_mci->base_addr + MCI_CR), AT91_MCI_CR_MCIDIS | AT91_MCI_CR_PWSDIS);
   MCI_WRITE_UINT32((unsigned int)(p_inf_mci->base_addr + MCI_IDR), 0xFFFFFFFF);

   // Set the Data Timeout Register
   MCI_WRITE_UINT32((unsigned int)(p_inf_mci->base_addr + MCI_DTOR), AT91_MCI_DTOR_DTOCYC_VAL(0xf)|AT91_MCI_DTOR_DTOMUL_1048576);

   // Set the Mode Register: 400KHz for MCK = 48MHz (CLKDIV = 58)
   clk_div = (master_clk / (400000 * 2)) - 1;
   MCI_WRITE_UINT32((unsigned int)(p_inf_mci->base_addr + MCI_MR), (clk_div | (AT91_MCI_MR_PWSDIV_VAL(0x7) & (0x7 << 8))));

   // Set the SDCard Register
   MCI_WRITE_UINT32((unsigned int)(p_inf_mci->base_addr + MCI_SDCR), AT91_MCI_SDCR_4_BITS_WIDTH);

   // Enable the MCI
   MCI_WRITE_UINT32((unsigned int)(p_inf_mci->base_addr + MCI_CR), AT91_MCI_CR_MCIEN);

   //
   g_p_inf_mci = p_inf_mci;
   g_mci_master_clock = master_clk;

   //
   g_p_inf_mci->timer_attr.tm_msec = 1000;//500;//300;//5000;//500;
   g_p_inf_mci->timer_attr.func = dev_at91_mci_timer_callback;
   g_p_inf_mci->timer_attr.data = (cyg_addrword_t)g_p_inf_mci;
   g_p_inf_mci->command = NULL;
   rttmr_create(&g_p_inf_mci->timer,&g_p_inf_mci->timer_attr);
   rttmr_stop(&p_inf_mci->timer);


   return 0;
}

/*-------------------------------------------
| Name:dev_at91_mci_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91_mci_open(desc_t desc, int o_flag){
   //already open
   if(ofile_lst[desc].p)
      return -1;

   if(o_flag & O_WRONLY) {
      g_p_inf_mci->desc_wr = desc;
   }
   if(o_flag & O_RDONLY) {
      g_p_inf_mci->desc_rd = desc;
   }

   //open in read/write configure IRQ
   if(g_p_inf_mci->desc_rd >= 0 && g_p_inf_mci->desc_wr >= 0) {
      //
      ofile_lst[desc].p = (void *)g_p_inf_mci;
      kernel_sem_init(&g_sem_mci, 0, 0);
      g_p_inf_mci->post_event = 0;

      //create ISR and DSR
      cyg_interrupt_create((cyg_vector_t)g_p_inf_mci->irq_no, (cyg_priority_t)g_p_inf_mci->irq_prio, (cyg_addrword_t)g_p_inf_mci,
            &dev_at91_mci_isr, &dev_at91_mci_dsr,
            &dev_at91_intr_handle, &dev_at91_intr_it);
      //attach it
      cyg_interrupt_attach(dev_at91_intr_handle);
      cyg_interrupt_unmask((cyg_vector_t)g_p_inf_mci->irq_no);
   }
   return 0;
}

/*-------------------------------------------
| Name:dev_at91_mci_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91_mci_close(desc_t desc) {
   board_inf_mci_t * p_inf_mci = (board_inf_mci_t *) ofile_lst[desc].p;

   if(ofile_lst[desc].oflag & O_RDONLY) {
      if(!ofile_lst[desc].nb_reader) {
         p_inf_mci->desc_rd = -1;
      }
   }

   if(ofile_lst[desc].oflag & O_WRONLY) {
      if(!ofile_lst[desc].nb_writer) {
         p_inf_mci->desc_wr = -1;
      }
   }

   //close all
   if (p_inf_mci->desc_wr < 0 && p_inf_mci->desc_rd < 0) {
      //disable the MCI
      MCI_WRITE_UINT32((unsigned int)(p_inf_mci->base_addr + MCI_CR), AT91_MCI_CR_MCIDIS | AT91_MCI_CR_PWSDIS);
      MCI_WRITE_UINT32((unsigned int)(p_inf_mci->base_addr + MCI_IDR), 0xFFFFFFFF);

      //
      rttmr_stop(&p_inf_mci->timer);
   }

   return 0;
}

/*-------------------------------------------
| Name:dev_at91_mci_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91_mci_read(desc_t desc, char* buf,int cb){
   return 0;
}

/*-------------------------------------------
| Name:dev_at91_mci_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91_mci_write(desc_t desc, const char* buf,int cb){
   return cb;
}

/*-------------------------------------------
| Name:dev_at91_mci_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91_mci_seek(desc_t desc,int offset,int origin){
   return -1;
}

/*-------------------------------------------
| Name:dev_at91_mci_ioctl
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
#define  MCI_DELAY   1000//2000
int dev_at91_mci_ioctl(desc_t desc,int request,va_list ap) {
   switch(request) {
      case HDSD_SENDCOMMAND: {
         mci_cmd_t * command = (mci_cmd_t *)ap;
         board_inf_mci_t * p_inf_mci = (board_inf_mci_t *)ofile_lst[desc].p;
         int sem_val;
         p_inf_mci->command = command;
         p_inf_mci->prev_command = 0;
         mci_stat.cmd_count++;

         if(_at91_mci_send_cmd(p_inf_mci, command)<0) {
            return -1;
         }

         rttmr_start(&p_inf_mci->timer);
         //wait for irq
         kernel_sem_wait(&g_sem_mci);

         return 0;
      }
      break;

      case HDSD_CHECKBUSY: {
         board_inf_mci_t * p_inf_mci = (board_inf_mci_t *)ofile_lst[desc].p;
         return 0;
      }
      break;

      case HDSD_SETSPEED: {
            board_inf_mci_t * p_inf_mci = (board_inf_mci_t *)ofile_lst[desc].p;
            return _at91_mci_set_speed(p_inf_mci, MCI_SPEED);
         }
      break;

      default:
         return -1;
      break;
   }

   return 0;
}

/*-------------------------------------------
| Name:dev_at91_mci_timer_callback
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void dev_at91_mci_timer_callback(alrm_hdl_t alarm_handle, cyg_addrword_t data){
   board_inf_mci_t * p_inf_mci = (board_inf_mci_t *)data;
   unsigned int * resp = NULL;
   unsigned int status = 0;

   if(p_inf_mci->command != NULL && p_inf_mci->command->cmd != UNSUPPORTED_COMMAND) {
      return;
   }

   cyg_interrupt_mask((cyg_vector_t)p_inf_mci->irq_no);

   if(p_inf_mci->post_event) {
      cyg_interrupt_unmask((cyg_vector_t)p_inf_mci->irq_no);
      return;
   }

   rttmr_stop(&p_inf_mci->timer);

   //WRITE_SINGLE_BLOCK or WRITE_BLOCK
   if(p_inf_mci->command->cmd == WRITE_MULTIPLE_BLOCK
         || p_inf_mci->command->cmd == READ_MULTIPLE_BLOCK) {

      // Disable MCI clock
      MCI_WRITE_UINT32((unsigned int)(p_inf_mci->base_addr + MCI_CR), AT91_MCI_CR_MCIDIS);
      __reset_dma();
      p_inf_mci->command->status = SD_LOW_LEVEL_TIMEOUT_ERROR;
      p_inf_mci->post_event = 0;
      kernel_sem_post(&g_sem_mci);
   }
   cyg_interrupt_unmask((cyg_vector_t)p_inf_mci->irq_no);
}

/*-------------------------------------------
| Name:dev_at91_mci_isr
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
cyg_uint32 dev_at91_mci_isr(cyg_vector_t vector, cyg_addrword_t data) {
   volatile unsigned int status;
   volatile unsigned int imr;
   board_inf_mci_t * p_inf_mci = (board_inf_mci_t *)data;
   volatile unsigned int send_cmd12 = 0;

   if(!p_inf_mci)
      return CYG_ISR_HANDLED;

   cyg_interrupt_mask(vector);

   // Read the status register
   MCI_READ_UINT32((unsigned int)(p_inf_mci->base_addr + MCI_SR), status);
   MCI_READ_UINT32((unsigned int)(p_inf_mci->base_addr + MCI_IMR), imr);
   status &= imr;

   // Check if an error has occured
   if((status & AT91_MCI_ALL_ERRORS) != 0) {
      if((status & AT91_MCI_ALL_ERRORS) == AT91_MCI_IMR_RINDE) {
         p_inf_mci->command->status = SD_LOW_LEVEL_ERROR;
         p_inf_mci->command->p_resp = NULL;
         mci_stat.irq_rinde++;
      }
      else if((status & AT91_MCI_ALL_ERRORS) == AT91_MCI_IMR_RDIRE) {
         p_inf_mci->command->status = SD_LOW_LEVEL_ERROR;
         p_inf_mci->command->p_resp = NULL;
         mci_stat.irq_rdire++;
      }
      // if the command is SEND_OP_COND the CRC error flag is always present
      // (cf : R3 response)
      else if((status & AT91_MCI_ALL_ERRORS) == AT91_MCI_IMR_RCRCE) {
         p_inf_mci->command->status = SD_LOW_LEVEL_CRC_ERROR;
         mci_stat.irq_rcrce++;
      }
      else if((status & AT91_MCI_ALL_ERRORS) == AT91_MCI_IMR_RENDE) {
         p_inf_mci->command->status = SD_LOW_LEVEL_ERROR;
         p_inf_mci->command->p_resp = NULL;
         mci_stat.irq_rende++;
      }
      else if ((status & AT91_MCI_ALL_ERRORS) == AT91_MCI_IMR_RTOE) {
         MCI_WRITE_UINT32((unsigned int)(p_inf_mci->base_addr + MCI_CR), AT91_MCI_CR_MCIDIS);
         p_inf_mci->command->status = SD_LOW_LEVEL_NO_RESPONSE;
         p_inf_mci->command->p_resp = NULL;
         mci_stat.irq_rtoe++;
      }
      //
      else if((status & AT91_MCI_ALL_ERRORS) == AT91_MCI_IMR_DCRCE) {
         MCI_WRITE_UINT32((unsigned int)(p_inf_mci->base_addr + MCI_CR), AT91_MCI_CR_MCIDIS);
         __reset_dma();
         p_inf_mci->command->status = SD_LOW_LEVEL_CRC_ERROR;
         mci_stat.irq_dcrce++;

         //need to send cmd12
         if(p_inf_mci->command->cmd == WRITE_MULTIPLE_BLOCK
               || p_inf_mci->command->cmd == READ_MULTIPLE_BLOCK) {
            send_cmd12 = 1;
         }
      }
      else if((status & AT91_MCI_ALL_ERRORS) == AT91_MCI_IMR_DTOE) {
         p_inf_mci->command->status = SD_LOW_LEVEL_TIMEOUT_ERROR;
         p_inf_mci->command->p_resp = NULL;
         mci_stat.irq_dtoe++;
      }
      else if((status & AT91_MCI_ALL_ERRORS) == AT91_MCI_IMR_OVRE) {
         MCI_WRITE_UINT32((unsigned int)(p_inf_mci->base_addr + MCI_CR), AT91_MCI_CR_MCIDIS);
         __reset_dma();
         p_inf_mci->command->status = SD_LOW_LEVEL_ERROR;
         p_inf_mci->command->p_resp = NULL;
         mci_stat.irq_ovre++;
      }
      else if((status & AT91_MCI_ALL_ERRORS) == AT91_MCI_IMR_UNRE) {
         MCI_WRITE_UINT32((unsigned int)(p_inf_mci->base_addr + MCI_CR), AT91_MCI_CR_MCIDIS);
         __reset_dma();
         p_inf_mci->command->status = SD_LOW_LEVEL_ERROR;
         p_inf_mci->command->p_resp = NULL;
         mci_stat.irq_unre++;
      }
      //
      p_inf_mci->post_event = 1;

      MCI_READ_UINT32((unsigned int)(p_inf_mci->base_addr + MCI_IMR), imr);
      MCI_WRITE_UINT32((unsigned int)(p_inf_mci->base_addr + MCI_IDR), imr);
   }

   else {
      // Check if a transfer has been completed
      if(((status & AT91_MCI_IMR_CMDRDY) != 0)
            || ((status & AT91_MCI_IMR_ENDRX) != 0)
            || ((status & AT91_MCI_IMR_RXBUFF) != 0)
            || ((status & AT91_MCI_IMR_ENDTX) != 0)
            || ((status & AT91_MCI_IMR_TXBUFE) != 0)
            || ((status & AT91_MCI_IMR_BLKE) != 0)
            || ((status & AT91_MCI_IMR_NOTBUSY) != 0)) {

         if((status & AT91_MCI_IMR_CMDRDY) != 0) {
            MCI_READ_UINT32((unsigned int)(p_inf_mci->base_addr + MCI_IMR), imr);
            MCI_WRITE_UINT32((unsigned int)(p_inf_mci->base_addr + MCI_IDR), imr);

            if(!p_inf_mci->command->is_read && p_inf_mci->command->block_count > 0) {
               if(p_inf_mci->command->block_count > 1) {
                  MCI_WRITE_UINT32((unsigned int)(p_inf_mci->base_addr + MCI_IER), AT91_MCI_IER_BLKE|AT91_MCI_IMR_TXBUFE|AT91_MCI_ALL_ERRORS);
               }
               else {
                  MCI_WRITE_UINT32((unsigned int)(p_inf_mci->base_addr + MCI_IER), AT91_MCI_IMR_TXBUFE|AT91_MCI_ALL_ERRORS);
               }
               MCI_WRITE_UINT32((unsigned int)(p_inf_mci->base_addr + MCI_PDC_PTCR), AT91_MCI_PDC_PTCR_TXTEN);
            }
            else {
                  if(p_inf_mci->prev_command == WRITE_MULTIPLE_BLOCK
                        && p_inf_mci->command->cmd == STOP_TRANSMISSION) {
                     MCI_WRITE_UINT32((unsigned int)(p_inf_mci->base_addr + MCI_IER), AT91_MCI_IER_NOTBUSY);
                  }
                  else {
                     p_inf_mci->post_event = 1;
                  }
            }
         }

         if((status & AT91_MCI_IMR_ENDRX) != 0) {
            MCI_WRITE_UINT32((unsigned int)(p_inf_mci->base_addr + MCI_IDR), AT91_MCI_IDR_ENDRX);
            MCI_WRITE_UINT32((unsigned int)(p_inf_mci->base_addr + MCI_IER), AT91_MCI_IER_RXBUFF);
            mci_stat.irq_endrx++;
         }
         if((status & AT91_MCI_IMR_RXBUFF) != 0) {
            MCI_WRITE_UINT32((unsigned int)(p_inf_mci->base_addr + MCI_PDC_PTCR), AT91_MCI_PDC_PTCR_RXTDIS);
            MCI_WRITE_UINT32((unsigned int)(p_inf_mci->base_addr + MCI_IDR), AT91_MCI_IER_ENDRX|AT91_MCI_IER_RXBUFF);

            mci_stat.irq_rxbuff++;
            if(p_inf_mci->command->block_count > 1) {
               MCI_WRITE_UINT32((unsigned int)(p_inf_mci->base_addr + MCI_CR), AT91_MCI_CR_MCIDIS);
               send_cmd12 = 1;
            }
            else {
               MCI_WRITE_UINT32((unsigned int)(p_inf_mci->base_addr + MCI_CR), AT91_MCI_CR_MCIDIS);
               p_inf_mci->post_event = 1;
            }
         }

         //
         if((status & AT91_MCI_IMR_ENDTX) != 0) {
            //
         }

         if((status & AT91_MCI_IMR_TXBUFE) != 0) {
            MCI_WRITE_UINT32((unsigned int)(p_inf_mci->base_addr + MCI_PDC_PTCR), AT91_MCI_PDC_PTCR_TXTDIS);
            MCI_WRITE_UINT32((unsigned int)(p_inf_mci->base_addr + MCI_IDR), AT91_MCI_IDR_TXBUFE);

            mci_stat.irq_txbufe++;
            if(p_inf_mci->command->block_count > 1) {
               MCI_WRITE_UINT32((unsigned int)(p_inf_mci->base_addr + MCI_IER), AT91_MCI_IER_BLKE);
            }
            else {
               MCI_WRITE_UINT32((unsigned int)(p_inf_mci->base_addr + MCI_IER), AT91_MCI_IER_NOTBUSY);
            }
         }

         if((status & AT91_MCI_IMR_BLKE) != 0 && !p_inf_mci->command->is_read) {
            MCI_WRITE_UINT32((unsigned int)(p_inf_mci->base_addr + MCI_IDR), AT91_MCI_IDR_BLKE);
            //MCI_WRITE_UINT32((unsigned int)(p_inf_mci->base_addr + MCI_CR), AT91_MCI_CR_MCIDIS);
            send_cmd12 = 1;
            mci_stat.irq_blke++;
         }

         //
         if((status & AT91_MCI_IMR_NOTBUSY) != 0) {
            MCI_WRITE_UINT32((unsigned int)(p_inf_mci->base_addr + MCI_IDR), AT91_MCI_IDR_NOTBUSY);

            if(p_inf_mci->command->cmd == WRITE_SINGLE_BLOCK
                  || p_inf_mci->prev_command == WRITE_MULTIPLE_BLOCK) {
               p_inf_mci->post_event = 1;
               mci_stat.irq_notbusy++;
            }
         }
      }
   }

   if(send_cmd12 == 1) {
      unsigned int * resp = p_inf_mci->command->p_resp;
      unsigned int tmp_prev_cmd=p_inf_mci->command->cmd;
      unsigned int tmp_prev_status=p_inf_mci->command->status;

      __reset_dma();

      //send a STOP_TRANSMISSION COMMAND
      memset((void *)p_inf_mci->command, 0, sizeof(mci_cmd_t));
      p_inf_mci->command->resp_type = R1b;
      p_inf_mci->command->cmd = STOP_TRANSMISSION;
      p_inf_mci->command->p_resp = resp;
      _at91_mci_send_cmd(p_inf_mci, p_inf_mci->command);

      p_inf_mci->post_event = 0;
      p_inf_mci->prev_command = tmp_prev_cmd;
      p_inf_mci->command->status = tmp_prev_status;
   }

   cyg_interrupt_acknowledge(vector);
   return (CYG_ISR_HANDLED | CYG_ISR_CALL_DSR);
}

/*-------------------------------------------
| Name:dev_at91_mci_dsr
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void dev_at91_mci_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data) {
   board_inf_mci_t * p_inf_mci = (board_inf_mci_t *)data;

   //unlock send cmd
   if(p_inf_mci->post_event) {
      if(p_inf_mci->command->p_resp) {
         unsigned char resp_size;
         volatile unsigned int i=0;

         switch (p_inf_mci->command->resp_type) {
         case R1:
            resp_size = 1;
            break;

         case R2:
            resp_size = 4;
            break;

         case R3:
         case R1b:
         case R6:
         case R7:
            resp_size = 1;
            break;

         default:
            resp_size = 0;
            break;
         }
         for(i=0; i < resp_size; i++) {
            MCI_READ_UINT32((unsigned int)(p_inf_mci->base_addr + MCI_RSPR1), p_inf_mci->command->p_resp[i]);
         }
      }

      mci_stat.dsr_count++;
      //
      rttmr_stop(&p_inf_mci->timer);
      p_inf_mci->post_event = 0;
      kernel_sem_post(&g_sem_mci);
   }

   cyg_interrupt_unmask(vector);
}


/*-------------------------------------------
| Name:dev_at91_mci_dsr
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _at91_mci_set_speed(board_inf_mci_t * p_inf_mci, unsigned int speed) {
   unsigned int mciMr;
   unsigned int clkdiv;

   // Set the Mode Register: 400KHz for MCK = 48MHz (CLKDIV = 58)
   MCI_READ_UINT32((unsigned int)(p_inf_mci->base_addr + MCI_MR), mciMr);
   mciMr &= ~AT91_MCI_MR_CLKDIV_VAL(0xff);

   // Multimedia Card Interface clock (MCCK or MCI_CK) is Master Clock (MCK)
   // divided by (2*(CLKDIV+1))
   if (speed > 0) {
      clkdiv = (g_mci_master_clock / (speed * 2));
      if (clkdiv > 0) {
         clkdiv -= 1;
      }
   }
   else {
      clkdiv = 0;
   }

   mciMr |= clkdiv;
   MCI_WRITE_UINT32((unsigned int)(p_inf_mci->base_addr + MCI_MR), mciMr);
   return 0;
}

/*-------------------------------------------
| Name:_at91_mci_send_cmd
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _at91_mci_send_cmd(board_inf_mci_t * p_inf_mci, mci_cmd_t * mci_cmd) {
   unsigned int mciIer, mciMr;

   // Command is now being executed
   mci_cmd->status = 0;

   // Disable MCI clock
   MCI_WRITE_UINT32((unsigned int)(p_inf_mci->base_addr + MCI_CR), AT91_MCI_CR_MCIDIS);

   // Disable transmitter and receiver
   __reset_dma();

   MCI_READ_UINT32((unsigned int)(p_inf_mci->base_addr + MCI_MR), mciMr);
   mciMr &= (~( AT91_MCI_MR_BLKLEN_VAL(0xfff) | AT91_MCI_MR_PDCMODE));


   // Command with DATA stage
   if(mci_cmd->block_size > 0) {
      // Enable PDC mode and set block size
      MCI_WRITE_UINT32((unsigned int)(p_inf_mci->base_addr + MCI_MR), mciMr | AT91_MCI_MR_PDCMODE |(mci_cmd->block_size << 16));

      // DATA transfer from card to host
      if(mci_cmd->is_read) {
         MCI_WRITE_UINT32((unsigned int)(p_inf_mci->base_addr + MCI_PDC_RPR), (int) mci_cmd->p_data);

         if ((mci_cmd->block_size & 0x3) != 0) {
            MCI_WRITE_UINT32((unsigned int)(p_inf_mci->base_addr + MCI_PDC_RCR), (mci_cmd->block_count * mci_cmd->block_size) / 4 + 1);
         }
         else {
            MCI_WRITE_UINT32((unsigned int)(p_inf_mci->base_addr + MCI_PDC_RCR), (mci_cmd->block_count * mci_cmd->block_size) / 4);
         }

         MCI_WRITE_UINT32((unsigned int)(p_inf_mci->base_addr + MCI_PDC_PTCR), AT91_MCI_PDC_PTCR_RXTEN);
         mciIer = AT91_MCI_IER_ENDRX | AT91_MCI_ALL_ERRORS;
      }

      // DATA transfer from host to card
      else {
         MCI_WRITE_UINT32((unsigned int)(p_inf_mci->base_addr + MCI_PDC_TPR), (int) mci_cmd->p_data);
         // Update the PDC counter
         if ((mci_cmd->block_size & 0x3) != 0) {
            MCI_WRITE_UINT32((unsigned int)(p_inf_mci->base_addr + MCI_PDC_TCR), (mci_cmd->block_count * mci_cmd->block_size) / 4 + 1);
         }
         else {
            MCI_WRITE_UINT32((unsigned int)(p_inf_mci->base_addr + MCI_PDC_TCR), (mci_cmd->block_count * mci_cmd->block_size) / 4);
         }
         // MCI_BLKE notifies the end of Multiblock command
         mciIer = AT91_MCI_IER_CMDRDY | AT91_MCI_ALL_ERRORS;
      }
   }
   // No data transfer: stop at the end of the command
   else {
      MCI_WRITE_UINT32((unsigned int)(p_inf_mci->base_addr + MCI_MR), mciMr);
      mciIer = AT91_MCI_IER_CMDRDY | AT91_MCI_ALL_ERRORS;
      //
      if(mci_cmd->cmd == STOP_TRANSMISSION) {
         mciIer |= AT91_MCI_IER_NOTBUSY;
      }
   }

   // Enable MCI clock
   MCI_WRITE_UINT32((unsigned int)(p_inf_mci->base_addr + MCI_CR), AT91_MCI_CR_MCIEN);

   // Send the command
   if(mci_supported_command[mci_cmd->cmd] != UNSUPPORTED_COMMAND) {
      MCI_WRITE_UINT32((unsigned int)(p_inf_mci->base_addr + MCI_ARGR), mci_cmd->arg);
      MCI_WRITE_UINT32((unsigned int)(p_inf_mci->base_addr + MCI_CMDR), mci_supported_command[mci_cmd->cmd]);
   }
   else {
      return -1;
   }

   // In case of transmit, the PDC shall be enabled after sending the command
   if ((mci_cmd->block_size > 0) && !(mci_cmd->is_read)) {
      MCI_WRITE_UINT32((unsigned int)(p_inf_mci->base_addr + MCI_PDC_PTCR), AT91_MCI_PDC_PTCR_TXTEN);
   }

   // Interrupt enable shall be done after PDC TXTEN and RXTEN
   MCI_WRITE_UINT32((unsigned int)(p_inf_mci->base_addr + MCI_IER), mciIer);

   return 0;
}

/*============================================
| End of Source  : dev_at91_mci.c
|---------------------------------------------
| Historic:
|---------------------------------------------
| Authors     | Date     | Comments
| $Log:$
==============================================*/
