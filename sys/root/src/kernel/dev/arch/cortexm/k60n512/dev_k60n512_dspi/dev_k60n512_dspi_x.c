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

#include "dev_k60n512_dspi_x.h"

/*===========================================
Global Declaration
=============================================*/
#define DSPI_X_FRAME_SIZE     7

#define DSPI_X_ALLOWED_IRQS    (REG_SPI_RSER_TCF_RE_MASK | \
                                REG_SPI_RSER_RFOF_RE_MASK | \
                                REG_SPI_RSER_RFDF_RE_MASK)

//
int dev_k60n512_dspi_x_load(board_kinetis_dspi_info_t * kinetis_dspi_info);
int dev_k60n512_dspi_x_open(desc_t desc, int o_flag, board_kinetis_dspi_info_t * kinetis_dspi_info);
int dev_k60n512_dspi_x_close(desc_t desc);
int dev_k60n512_dspi_x_read(desc_t desc, char* buf,int cb);
int dev_k60n512_dspi_x_write(desc_t desc, const char* buf,int cb);
int dev_k60n512_dspi_x_isset_read(desc_t desc);
int dev_k60n512_dspi_x_isset_write(desc_t desc);
int dev_k60n512_dspi_x_ioctl(desc_t desc,int request,va_list ap);

#if defined(USE_ECOS)
static cyg_uint32 _kinetis_dspi_x_isr(cyg_vector_t vector, cyg_addrword_t data);
static void _kinetis_dspi_x_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data);
#endif

//
static unsigned int _kinetis_dspi_x_find_baudrate(unsigned int bus_clk,
                                                  unsigned int baudrate);
static int _kinetis_dspi_x_set_polarity(board_kinetis_dspi_info_t * kinetis_dspi_info);

//
static const unsigned int _kinetis_dspi_baudrate_prescaler[] = { 2, 3, 5, 7 };
static const unsigned int _kinetis_dspi_baudrate_scaler[] =
{ 2, 4, 6, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768 };
static const unsigned int _kinetis_dspi_delay_prescaler[] = { 1, 3, 5, 7 };
/*===========================================
Implementation
=============================================*/
/*-------------------------------------------
| Name:dev_k60n512_dspi_x_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_dspi_x_load(board_kinetis_dspi_info_t * kinetis_dspi_info) {
   volatile unsigned int reg_val = 0;

   //disable and clear spi
   HAL_READ_UINT32(kinetis_dspi_info->dspi_base + REG_SPI_MCR, reg_val);
   reg_val &= ~REG_SPI_MCR_MDIS_MASK;
   HAL_WRITE_UINT32(kinetis_dspi_info->dspi_base + REG_SPI_MCR, reg_val);

   HAL_READ_UINT32(kinetis_dspi_info->dspi_base + REG_SPI_MCR, reg_val);
   reg_val |= REG_SPI_MCR_HALT_MASK | REG_SPI_MCR_CLR_RXF_MASK | REG_SPI_MCR_CLR_TXF_MASK
              | REG_SPI_MCR_DIS_RXF_MASK | REG_SPI_MCR_DIS_TXF_MASK;
   HAL_WRITE_UINT32(kinetis_dspi_info->dspi_base + REG_SPI_MCR, reg_val);

   //define number of bytes transferred by frame and spi baudrate
   HAL_WRITE_UINT32(kinetis_dspi_info->dspi_base + REG_SPI_CTAR0,
                    REG_SPI_CTAR_FMSZ(DSPI_X_FRAME_SIZE));

   HAL_READ_UINT32(kinetis_dspi_info->dspi_base + REG_SPI_CTAR0, reg_val);
   reg_val |= _kinetis_dspi_x_find_baudrate(/*48000000*/ CYGHWR_HAL_CORTEXM_KINETIS_CLK_PER_BUS,
                                                         kinetis_dspi_info->clk);
   HAL_WRITE_UINT32(kinetis_dspi_info->dspi_base + REG_SPI_CTAR0, reg_val);

   //
   _kinetis_dspi_x_set_polarity(kinetis_dspi_info);

   //disable receive FIFO overflow
   HAL_READ_UINT32(kinetis_dspi_info->dspi_base + REG_SPI_MCR, reg_val);
   reg_val &= ~REG_SPI_MCR_ROOE_MASK;
   HAL_WRITE_UINT32(kinetis_dspi_info->dspi_base + REG_SPI_MCR, reg_val);

   //set CS0-7 inactive high
   //!TODO inactif niveau bas
   HAL_READ_UINT32(kinetis_dspi_info->dspi_base + REG_SPI_MCR, reg_val);
   reg_val |= REG_SPI_MCR_PCSIS(0xff);
   HAL_WRITE_UINT32(kinetis_dspi_info->dspi_base + REG_SPI_MCR, reg_val);

   //set mode
   HAL_READ_UINT32(kinetis_dspi_info->dspi_base + REG_SPI_MCR, reg_val);
   reg_val |= (kinetis_dspi_info->mode << REG_SPI_MCR_MSTR_SHIFT);
   HAL_WRITE_UINT32(kinetis_dspi_info->dspi_base + REG_SPI_MCR, reg_val);

   //disable all IRQs
   HAL_WRITE_UINT32(kinetis_dspi_info->dspi_base + REG_SPI_RSER, 0);

   return 0;
}

/*-------------------------------------------
| Name:dev_k60n512_dspi_x_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_dspi_x_open(desc_t desc, int o_flag,
                            board_kinetis_dspi_info_t * kinetis_dspi_info) {
   volatile unsigned int reg_val = 0;
   cyg_handle_t irq_handle;
   cyg_interrupt irq_it;

   if((o_flag & O_RDONLY)) {
      if(kinetis_dspi_info->desc_r<0) {
         kinetis_dspi_info->desc_r = desc;
         kinetis_dspi_info->input_r = 0;
         kinetis_dspi_info->input_w = 0;
      }
      else
         return -1;                //already open
   }

   if((o_flag & O_WRONLY)) {
      if(kinetis_dspi_info->desc_w<0) {
         kinetis_dspi_info->desc_w = desc;
         kinetis_dspi_info->output_r = 0;
         kinetis_dspi_info->output_w = 0;
      }
      else
         return -1;                //already open
   }

   if(!ofile_lst[desc].p)
      ofile_lst[desc].p=kinetis_dspi_info;

   //unmask IRQ and enable ksz
   if(kinetis_dspi_info->desc_r >=0 || kinetis_dspi_info->desc_w >=0) {

#if 0
      cyg_interrupt_create((cyg_vector_t)kinetis_dspi_info->irq_no,
                           kinetis_dspi_info->irq_prio,
                           // Data item passed to interrupt handler
                           (cyg_addrword_t)kinetis_dspi_info,
                           _kinetis_dspi_x_isr,
                           _kinetis_dspi_x_dsr,
                           &kinetis_dspi_info->irq_handle,
                           &kinetis_dspi_info->irq_it);
      cyg_interrupt_attach(kinetis_dspi_info->irq_handle);

      //allow IRQ
      HAL_READ_UINT32(kinetis_dspi_info->dspi_base + REG_SPI_RSER, reg_val);
      reg_val |= DSPI_X_ALLOWED_IRQS;
      HAL_WRITE_UINT32(kinetis_dspi_info->dspi_base + REG_SPI_RSER, reg_val);
#endif

      //clear TFF (TX FIFO not full)
      HAL_READ_UINT32(kinetis_dspi_info->dspi_base + REG_SPI_SR, reg_val);
      reg_val &= ~REG_SPI_SR_TFFF_MASK;
      HAL_WRITE_UINT32(kinetis_dspi_info->dspi_base + REG_SPI_SR, reg_val);

      //start SPI transfer
      HAL_READ_UINT32(kinetis_dspi_info->dspi_base + REG_SPI_MCR, reg_val);
      reg_val &= ~REG_SPI_MCR_HALT_MASK;
      HAL_WRITE_UINT32(kinetis_dspi_info->dspi_base + REG_SPI_MCR, reg_val);

#if 0
      cyg_interrupt_unmask((cyg_vector_t)kinetis_dspi_info->irq_no);
#endif
   }

   return 0;
}

/*-------------------------------------------
| Name:dev_k60n512_dspi_x_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_dspi_x_close(desc_t desc) {
   board_kinetis_dspi_info_t * p_inf_dspi_x = (board_kinetis_dspi_info_t *)ofile_lst[desc].p;
   volatile unsigned int reg_val = 0;

   if(ofile_lst[desc].oflag & O_RDONLY) {
      if(!ofile_lst[desc].nb_reader) {
         p_inf_dspi_x->desc_r = -1;
         p_inf_dspi_x->input_r = -1;
         p_inf_dspi_x->input_w = -1;
      }
   }

   if(ofile_lst[desc].oflag & O_WRONLY) {
      if(!ofile_lst[desc].nb_writer) {
         p_inf_dspi_x->desc_w = -1;
         p_inf_dspi_x->output_r = 0;
         p_inf_dspi_x->output_w = 0;
      }
   }

   //close all
   if ((p_inf_dspi_x->desc_w < 0) && (p_inf_dspi_x->desc_r < 0)) {
      HAL_READ_UINT32(p_inf_dspi_x->dspi_base + REG_SPI_MCR, reg_val);
      reg_val |= REG_SPI_MCR_HALT_MASK;
      HAL_WRITE_UINT32(p_inf_dspi_x->dspi_base + REG_SPI_MCR, reg_val);

      HAL_WRITE_UINT32(p_inf_dspi_x->dspi_base + REG_SPI_RSER, 0);
      ofile_lst[desc].p = NULL;
   }

   return 0;
}

/*-------------------------------------------
| Name:dev_k60n512_dspi_x_isset_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_dspi_x_isset_read(desc_t desc) {
   return 0;
}

/*-------------------------------------------
| Name:dev_k60n512_dspi_x_isset_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_dspi_x_isset_write(desc_t desc) {
#if 0
   board_kinetis_dspi_info_t * p_inf_dspi_x = (board_kinetis_dspi_info_t *)ofile_lst[desc].p;

   if(!p_inf_dspi_x)
      return -1;

   if(p_inf_dspi_x->output_r == p_inf_dspi_x->output_w) {
      //!TODO maybe need a special flag
      return 0;
   }

   return -1;
#endif
   return -1;
}

/*-------------------------------------------
| Name:dev_k60n512_dspi_x_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_dspi_x_read(desc_t desc, char* buf,int size){
   return size;
}

/*-------------------------------------------
| Name:dev_k60n512_dspi_x_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_dspi_x_write(desc_t desc, const char* buf,int size){
   board_kinetis_dspi_info_t * p_inf_dspi_x = (board_kinetis_dspi_info_t *)ofile_lst[desc].p;
   volatile unsigned int reg_val = 0;
   unsigned int i=0;
   unsigned short data=0;

   //cyg_interrupt_mask((cyg_vector_t)p_inf_dspi_x->irq_no);

   //
   if(size > (DSPI_X_FRAME_SIZE + 1)) {
      size = DSPI_X_FRAME_SIZE + 1;
   }

   //fill buffer with data
   for(; i<size; i++,
       p_inf_dspi_x->output_w=(p_inf_dspi_x->output_w+1) & (SPI_TX_BUFFER_SIZE-1)) {
      p_inf_dspi_x->output_buffer[p_inf_dspi_x->output_w] = buf[i];
   }

   //check master mode
   while(i--) {
      HAL_READ_UINT32(p_inf_dspi_x->dspi_base + REG_SPI_MCR, reg_val);

      if(reg_val & REG_SPI_MCR_MSTR_MASK) {
         //check FIFO is not empty
         do {
            HAL_DELAY_US(100);   //50 ko
            HAL_READ_UINT32(p_inf_dspi_x->dspi_base + REG_SPI_SR, reg_val);
         } while(!(reg_val & REG_SPI_SR_TFFF_MASK));

         reg_val = p_inf_dspi_x->output_buffer[p_inf_dspi_x->output_r];
         reg_val |= REG_SPI_PUSHR_PCS(p_inf_dspi_x->cs+1) | REG_SPI_PUSHR_CTAS(0);

         //last
         if(!i) {
            reg_val |= REG_SPI_PUSHR_EOQ_MASK;
         }
         else {
            reg_val |= REG_SPI_PUSHR_CONT_MASK;
         }
         HAL_WRITE_UINT32(p_inf_dspi_x->dspi_base + REG_SPI_PUSHR, reg_val);
      }

      //wait read buffer not empty flag
      do {
         HAL_READ_UINT32(p_inf_dspi_x->dspi_base + REG_SPI_SR, reg_val);
      } while( !(reg_val & REG_SPI_SR_RFDF_MASK));

      data = REG_SPI_POPR_RXDATA(p_inf_dspi_x->dspi_base + REG_SPI_POPR);

      HAL_READ_UINT32(p_inf_dspi_x->dspi_base + REG_SPI_SR, reg_val);
      reg_val |= REG_SPI_SR_RFDF_MASK;
      HAL_WRITE_UINT32(p_inf_dspi_x->dspi_base + REG_SPI_SR, reg_val);

      //
      HAL_READ_UINT32(p_inf_dspi_x->dspi_base + REG_SPI_SR, reg_val);

      if(reg_val & REG_SPI_SR_EOQF_MASK) {
         reg_val |= REG_SPI_SR_EOQF_MASK;
      }
      if(reg_val & REG_SPI_SR_TCF_MASK) {
         reg_val |= REG_SPI_SR_TCF_MASK;
      }
      if(reg_val & REG_SPI_SR_RFOF_MASK) {
         reg_val |= REG_SPI_SR_RFOF_MASK;
      }
      HAL_WRITE_UINT32(p_inf_dspi_x->dspi_base + REG_SPI_SR, reg_val);

      //
      p_inf_dspi_x->output_r=(p_inf_dspi_x->output_r+1) & (SPI_TX_BUFFER_SIZE-1);
   }

   //cyg_interrupt_unmask((cyg_vector_t)p_inf_dspi_x->irq_no);
   return size;
}

/*-------------------------------------------
| Name:dev_k60n512_dspi_x_ioctl
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_dspi_x_ioctl(desc_t desc,int request,va_list ap) {
   //change baudrate

   //change cs (aka slave)

   //...
   return 0;
}

/*-------------------------------------------
| Name:_kinetis_dspi_x_isr
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
cyg_uint32 _kinetis_dspi_x_isr(cyg_vector_t vector, cyg_addrword_t data) {
   volatile unsigned int reg_val = 0, irq_sr = 0;
   board_kinetis_dspi_info_t * p_inf_dspi_x = (board_kinetis_dspi_info_t *)data;

   cyg_interrupt_mask((cyg_vector_t)p_inf_dspi_x->irq_no);


   return CYG_ISR_HANDLED | CYG_ISR_CALL_DSR;
}

/*-------------------------------------------
| Name:_kinetis_dspi_x_dsr
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void _kinetis_dspi_x_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data) {
   board_kinetis_dspi_info_t * p_inf_dspi_x = (board_kinetis_dspi_info_t *)data;

   cyg_interrupt_unmask((cyg_vector_t)p_inf_dspi_x->irq_no);
}

/*-------------------------------------------
| Name:_kinetis_dspi_x_find_baudrate
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
unsigned int _kinetis_dspi_x_find_baudrate(unsigned int bus_clk,
                                           unsigned int baudrate) {
   volatile unsigned int pres, scaler, dbr, mindbr = 1;
   unsigned int best_pres = 0;
   unsigned int best_scaler = 0;
   unsigned int delayrate;

   unsigned int diff;
   unsigned int min_diff;

   unsigned int result;

   /* find combination of prescaler and scaler resulting in baudrate closest to the requested value */
   min_diff = (unsigned int)-1;
   for(pres = 0; pres < 4; pres++) {
      for(scaler = 0; scaler < 16; scaler++) {
         for(dbr = 1; dbr < 3; dbr++) {
            diff = baudrate -
                   ((bus_clk *
                     dbr) /
                    (_kinetis_dspi_baudrate_prescaler[pres] *
                     (_kinetis_dspi_baudrate_scaler[scaler])));
            if(diff < 0)
               diff = -diff;

            if(min_diff > diff) {
               /* a better match found */
               min_diff = diff;
               best_pres = pres;
               best_scaler = scaler;
               mindbr = dbr;
            }
         }
      }
   }

   /* store baudrate scaler and prescaler settings to the result */
   result = REG_SPI_CTAR_PBR(best_pres) | REG_SPI_CTAR_BR(best_scaler);
   result |= ((mindbr - 1) * REG_SPI_CTAR_DBR_MASK);

   /* similar lookup for delay prescalers */
   min_diff = (unsigned int)-1;
   delayrate = baudrate * 4; /* double the baudrate (half period delay is sufficient) and divisor is (2<<scaler), thus times 4 */
   for(pres = 0; pres < 4; pres++) {
      for(scaler = 0; scaler < 16; scaler++) {
         diff = ((_kinetis_dspi_delay_prescaler[pres] * delayrate) << scaler) - bus_clk;
         if(diff < 0)
            diff = -diff;

         if(min_diff > diff) {
            /* a better match found */
            min_diff = diff;
            best_pres = pres;
            best_scaler = scaler;
         }
      }
   }

   /* add delay scalers and prescaler settings to the result */
   result |= REG_SPI_CTAR_CSSCK(best_scaler) | REG_SPI_CTAR_PCSSCK(best_pres);
   result |= REG_SPI_CTAR_DT(best_scaler) | REG_SPI_CTAR_PDT(best_pres);

   return result;
}

/*-------------------------------------------
| Name:_kinetis_dspi_x_find_baudrate
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int _kinetis_dspi_x_set_polarity(board_kinetis_dspi_info_t * kinetis_dspi_info) {
   volatile unsigned int reg_val = 0;

   HAL_READ_UINT32(kinetis_dspi_info->dspi_base + REG_SPI_CTAR0, reg_val);

   switch(kinetis_dspi_info->clk_phase) {
   case SPI_CTAR_CLK_POL_PHA_MODE0:
      /* Inactive state of SPI_CLK = logic 0 */
      reg_val &= ~REG_SPI_CTAR_CPOL_MASK;
      /* SPI_CLK transitions middle of bit timing */
      reg_val &= ~REG_SPI_CTAR_CPHA_MASK;
      break;

   case SPI_CTAR_CLK_POL_PHA_MODE1:
      /* Inactive state of SPI_CLK = logic 0 */
      reg_val &= ~REG_SPI_CTAR_CPOL_MASK;
      /* SPI_CLK transitions begining of bit timing */
      reg_val |= REG_SPI_CTAR_CPHA_MASK;
      break;

   case SPI_CTAR_CLK_POL_PHA_MODE2:
      /* Inactive state of SPI_CLK = logic 1 */
      reg_val |= REG_SPI_CTAR_CPOL_MASK;
      /* SPI_CLK transitions middle of bit timing */
      reg_val &= (~REG_SPI_CTAR_CPHA_MASK);
      break;

   case SPI_CTAR_CLK_POL_PHA_MODE3:
      /* Inactive state of SPI_CLK = logic 1 */
      reg_val |= REG_SPI_CTAR_CPOL_MASK;
      /* SPI_CLK transitions begining of bit timing */
      reg_val |= REG_SPI_CTAR_CPHA_MASK;
      break;

   default:
      return -1;
   }

   HAL_WRITE_UINT32(kinetis_dspi_info->dspi_base + REG_SPI_CTAR0, reg_val);
   return 0;
}
/*============================================
| End of Source  : dev_k60n512_dspi_x.c
==============================================*/

