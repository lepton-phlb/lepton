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

#include "lib/libc/termios/termios.h"
#include "dev_k60n512_uart_x.h"

#if defined(USE_ECOS)
#endif
/*===========================================
Global Declaration
=============================================*/
int dev_k60n512_uart_x_load(board_kinetis_uart_info_t * kinetis_uart_info);
int dev_k60n512_uart_x_open(desc_t desc, int o_flag, board_kinetis_uart_info_t * kinetis_uart_info);

//
int dev_k60n512_uart_x_isset_read(desc_t desc);
int dev_k60n512_uart_x_isset_write(desc_t desc);
int dev_k60n512_uart_x_close(desc_t desc);
int dev_k60n512_uart_x_seek(desc_t desc,int offset,int origin);
int dev_k60n512_uart_x_read(desc_t desc, char* buf,int cb);
int dev_k60n512_uart_x_write(desc_t desc, const char* buf,int cb);
int dev_k60n512_uart_x_ioctl(desc_t desc,int request,va_list ap);
int dev_k60n512_uart_x_seek(desc_t desc,int offset,int origin);

#if defined(USE_ECOS)
static cyg_uint32 _kinetis_uart_x_isr(cyg_vector_t vector, cyg_addrword_t data);
static void _kinetis_uart_x_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data);
#endif

//Receiver On + Receiver Full Interrupt On
#define UART_X_ALLOWED_IRQS_R    (REG_UART_C2_RIE | REG_UART_C2_RE)
//Transmitter On
#define UART_X_ALLOWED_IRQS_W    (REG_UART_C2_TE)

#define UART_X_ALLOWED_IRQS    (UART_X_ALLOWED_IRQS_R | \
                                UART_X_ALLOWED_IRQS_W)


typedef struct board_kinetis_uart_stat_st {
   unsigned int isr;
   unsigned int dsr_tx;
   unsigned int dsr_rx;
   unsigned int dsr_err;
}board_kinetis_uart_stat_t;

board_kinetis_uart_stat_t _kinetis_uart_stat;

/*===========================================
Implementation
=============================================*/
/*-------------------------------------------
| Name:dev_k60n512_uart_x_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_uart_x_load(board_kinetis_uart_info_t * kinetis_uart_info){
   volatile unsigned char regval;
   pthread_mutexattr_t mutex_attr=0;

   kernel_pthread_mutex_init(&kinetis_uart_info->mutex,&mutex_attr);

   //configure baud rate
   hal_freescale_uart_setbaud(kinetis_uart_info->uart_base,
                              kinetis_uart_info->speed);

   //no parity + 8 bits data
   regval = 0;
   HAL_WRITE_UINT8(kinetis_uart_info->uart_base + REG_UART_C1, regval);

   return 0;
}

/*-------------------------------------------
| Name:dev_k60n512_uart_x_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_uart_x_open(desc_t desc, int o_flag,
                            board_kinetis_uart_info_t * kinetis_uart_info){

   if(o_flag & O_RDONLY) {
      if(kinetis_uart_info->desc_r<0) {
         kinetis_uart_info->desc_r = desc;
      }
      else
         return -1;                //already open
   }

   if(o_flag & O_WRONLY) {
      if(kinetis_uart_info->desc_w<0) {
         kinetis_uart_info->desc_w = desc;
         kinetis_uart_info->output_r = -1;
      }
      else
         return -1;                //already open
   }

   if(!ofile_lst[desc].p)
      ofile_lst[desc].p=kinetis_uart_info;

   //unmask IRQ
   if(kinetis_uart_info->desc_r>=0 && kinetis_uart_info->desc_w>=0) {
      cyg_interrupt_create((cyg_vector_t)kinetis_uart_info->irq_no,
                           kinetis_uart_info->irq_prio,
                           // Data item passed to interrupt handler
                           (cyg_addrword_t)kinetis_uart_info,
                           _kinetis_uart_x_isr,
                           _kinetis_uart_x_dsr,
                           &kinetis_uart_info->irq_handle,
                           &kinetis_uart_info->irq_it);


      cyg_interrupt_attach(kinetis_uart_info->irq_handle);
      cyg_interrupt_unmask((cyg_vector_t)kinetis_uart_info->irq_no);
      HAL_WRITE_UINT8(kinetis_uart_info->uart_base + REG_UART_C2, UART_X_ALLOWED_IRQS);
   }
   return 0;
}

/*-------------------------------------------
| Name:dev_k60n512_uart_x_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_uart_x_close(desc_t desc){
   //nothing to do
   //save code space :)
   return 0;
}

/*-------------------------------------------
| Name:dev_k60n512_uart_x_isset_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_uart_x_isset_read(desc_t desc){
   board_kinetis_uart_info_t * p_uart_info = (board_kinetis_uart_info_t*)ofile_lst[desc].p;
   if(!p_uart_info)
      return -1;

   if(p_uart_info->input_r != p_uart_info->input_w)
      return 0;

   return -1;
}

/*-------------------------------------------
| Name:dev_k60n512_uart_x_isset_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_uart_x_isset_write(desc_t desc){
   board_kinetis_uart_info_t * p_uart_info = (board_kinetis_uart_info_t*)ofile_lst[desc].p;
   if(!p_uart_info)
      return -1;

   if(p_uart_info->output_r==p_uart_info->output_w) {
      p_uart_info->output_r = -1;
      return 0;
   }

   return -1;
}

/*-------------------------------------------
| Name:dev_k60n512_uart_x_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_uart_x_read(desc_t desc, char* buf,int size){
   board_kinetis_uart_info_t * p_uart_info = (board_kinetis_uart_info_t*)ofile_lst[desc].p;
   int cb = 0;
   int count = 0;
   cb = (size>=UART_RX_BUFFER_SIZE) ? UART_RX_BUFFER_SIZE : size;

   cyg_interrupt_mask((cyg_vector_t)p_uart_info->irq_no);
   //
   while(p_uart_info->input_r != p_uart_info->input_w && count < cb) {
      buf[count++] = p_uart_info->input_buffer[p_uart_info->input_r];
      p_uart_info->input_r = (p_uart_info->input_r+1) & (UART_RX_BUFFER_SIZE-1);
   }
   //
   cyg_interrupt_unmask((cyg_vector_t)p_uart_info->irq_no);
   return count;
}

/*-------------------------------------------
| Name:dev_k60n512_uart_x_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_uart_x_write(desc_t desc, const char* buf,int size){
   board_kinetis_uart_info_t * p_uart_info = (board_kinetis_uart_info_t*)ofile_lst[desc].p;
   cyg_uint32 uart_reg;

   if(!p_uart_info) {
      return -1;
   }

   kernel_pthread_mutex_lock(&p_uart_info->mutex);

   p_uart_info->output_buffer = (unsigned char *)buf;
   p_uart_info->output_r = 0;
   p_uart_info->output_w = size;

   do {
      //HAL_DELAY_US(1000);
      //waiting for empty fifo
      HAL_READ_UINT8(p_uart_info->uart_base + REG_UART_S1, uart_reg);
   } while(!(uart_reg & REG_UART_S1_TDRE));


   __clr_irq();
   //write first byte in buf
   HAL_WRITE_UINT8(p_uart_info->uart_base + REG_UART_D,
                   p_uart_info->output_buffer[p_uart_info->output_r++]);
   p_uart_info->xmit=1;

   //transmitter irq enable
   HAL_READ_UINT8(p_uart_info->uart_base + REG_UART_C2, uart_reg);
   uart_reg |= REG_UART_C2_TIE;
   HAL_WRITE_UINT8(p_uart_info->uart_base + REG_UART_C2, uart_reg);

   __set_irq();

   kernel_pthread_mutex_unlock(&p_uart_info->mutex);

   return size;
}

/*-------------------------------------------
| Name:dev_k60n512_uart_x_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_uart_x_seek(desc_t desc,int offset,int origin){
   return 0;
}

/*-------------------------------------------
| Name:dev_k60n512_uart_x_ioctl
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_uart_x_ioctl(desc_t desc,int request,va_list ap) {
   return 0;
}

/*-------------------------------------------
| Name:_kinetis_uart_isr
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
cyg_uint32 _kinetis_uart_x_isr(cyg_vector_t vector, cyg_addrword_t data) {
   cyg_interrupt_mask(vector);
   cyg_interrupt_acknowledge(vector);

   _kinetis_uart_stat.isr++;

   return CYG_ISR_HANDLED | CYG_ISR_CALL_DSR;
}

/*-------------------------------------------
| Name:_kinetis_uart_dsr
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void _kinetis_uart_x_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data) {
   board_kinetis_uart_info_t * p_uart_info = (board_kinetis_uart_info_t*)data;
   volatile unsigned char uart_sr;

   //read status
   HAL_READ_UINT8(p_uart_info->uart_base + REG_UART_S1, uart_sr);

   //transmit
   if(p_uart_info->xmit && (uart_sr & REG_UART_S1_TDRE)) {
      if(p_uart_info->output_r != p_uart_info->output_w) {
         //finish buffer
         HAL_WRITE_UINT8(p_uart_info->uart_base + REG_UART_D,
                         p_uart_info->output_buffer[p_uart_info->output_r++]);
      }
      else {
         //all data sent
         volatile unsigned char uart_reg;
         HAL_READ_UINT8(p_uart_info->uart_base + REG_UART_C2, uart_reg);
         uart_reg &= ~REG_UART_C2_TIE;
         HAL_WRITE_UINT8(p_uart_info->uart_base + REG_UART_C2, uart_reg);
         p_uart_info->xmit=0;

         __fire_io_int(ofile_lst[p_uart_info->desc_w].owner_pthread_ptr_write);
      }
      _kinetis_uart_stat.dsr_tx++;
   }
   //receive
   else if(uart_sr & REG_UART_S1_RDRF) {
      HAL_READ_UINT8(p_uart_info->uart_base + REG_UART_D,
                     p_uart_info->input_buffer[p_uart_info->input_w]);
      p_uart_info->input_w = (p_uart_info->input_w+1) & (UART_RX_BUFFER_SIZE-1);

      __fire_io_int(ofile_lst[p_uart_info->desc_r].owner_pthread_ptr_read);
   }

   cyg_interrupt_unmask(vector);
}

/*============================================
| End of Source  : dev_k60n512_uart_x.c
==============================================*/
