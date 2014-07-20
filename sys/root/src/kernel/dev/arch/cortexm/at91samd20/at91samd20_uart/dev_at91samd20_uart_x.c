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
#include "kernel/core/kernelconf.h"
#include "kernel/core/types.h"
#include "kernel/core/interrupt.h"
#include "kernel/core/kernel.h"
#include "kernel/core/system.h"
#include "kernel/core/fcntl.h"
#include "kernel/core/cpu.h"
#include "kernel/core/core_rttimer.h"
#include "kernel/fs/vfs/vfsdev.h"

#include "kernel/dev/arch/at91/asf/sam0/drivers/sercom/usart/usart.h"
#include "kernel/dev/arch/at91/asf/sam0/drivers/sercom/usart/usart_interrupt.h"
#include "dev_at91samd20_uart_x.h"


/*===========================================
Global Declaration
=============================================*/
int dev_at91samd20_uart_x_load(board_at91samd20_uart_info_t * uart_info);
int dev_at91samd20_uart_x_open(desc_t desc, int o_flag, board_at91samd20_uart_info_t * uart_info);

//
int dev_at91samd20_uart_x_isset_read(desc_t desc);
int dev_at91samd20_uart_x_isset_write(desc_t desc);
int dev_at91samd20_uart_x_close(desc_t desc);
int dev_at91samd20_uart_x_seek(desc_t desc,int offset,int origin);
int dev_at91samd20_uart_x_read(desc_t desc, char* buf,int cb);
int dev_at91samd20_uart_x_write(desc_t desc, const char* buf,int cb);
int dev_at91samd20_uart_x_ioctl(desc_t desc,int request,va_list ap);
int dev_at91samd20_uart_x_seek(desc_t desc,int offset,int origin);


/*===========================================
Implementation
=============================================*/

/**
 * \internal
 * Handles interrupts as they occur, and it will run callback functions
 * which are registered and enabled.
 *
 * \param[in]  instance  ID of the SERCOM instance calling the interrupt
 *                       handler.
 */
static void _at91samd20_uart_x_interrupt_handler(
		uint8_t instance)
{
	/* Temporary variables */
	uint16_t interrupt_status;
	uint16_t callback_status;
	uint8_t error_code;
   //
  __hw_enter_interrupt();
  //

   //lepton
   board_at91samd20_uart_info_t * p_board_at91samd20_uart_info= (board_at91samd20_uart_info_t *)_sercom_instances[instance];
	/* Get device instance from the look-up table */
	struct usart_module *module
		= (struct usart_module *)_sercom_instances[instance];

	/* Pointer to the hardware module instance */
	SercomUsart *const usart_hw
		= &(module->hw->USART);

	/* Wait for the synchronization to complete */
	_usart_wait_for_sync(module);

	/* Read and mask interrupt flag register */
	interrupt_status = usart_hw->INTFLAG.reg;
	interrupt_status &= usart_hw->INTENSET.reg;
	callback_status = module->callback_reg_mask &
			module->callback_enable_mask;

	/* Check if a DATA READY interrupt has occurred,
	 * and if there is more to transfer */
	if (interrupt_status & SERCOM_USART_INTFLAG_DRE) {
		if (module->remaining_tx_buffer_length) {
			/* Write value will be at least 8-bits long */
			uint16_t data_to_send = *(module->tx_buffer_ptr);
			/* Increment 8-bit pointer */
			(module->tx_buffer_ptr)++;

			if (module->character_size == USART_CHARACTER_SIZE_9BIT) {
				data_to_send |= (*(module->tx_buffer_ptr) << 8);
				/* Increment 8-bit pointer */
				(module->tx_buffer_ptr)++;
			}
			/* Write the data to send */
			usart_hw->DATA.reg = (data_to_send & SERCOM_USART_DATA_MASK);

			if (--(module->remaining_tx_buffer_length) == 0) {
				/* Disable the Data Register Empty Interrupt */
				usart_hw->INTENCLR.reg = SERCOM_USART_INTFLAG_DRE;
				/* Enable Transmission Complete interrupt */
				usart_hw->INTENSET.reg = SERCOM_USART_INTFLAG_TXC;

			}
		} else {
			usart_hw->INTENCLR.reg = SERCOM_USART_INTFLAG_DRE;
		}

	/* Check if the Transmission Complete interrupt has occurred and
	 * that the transmit buffer is empty */
	}

	if (interrupt_status & SERCOM_USART_INTFLAG_TXC) {

		/* Disable TX Complete Interrupt, and set STATUS_OK */
		usart_hw->INTENCLR.reg = SERCOM_USART_INTFLAG_TXC;
		module->tx_status = STATUS_OK;
      
      //
      __fire_io_int(ofile_lst[p_board_at91samd20_uart_info->desc_w].owner_pthread_ptr_write);
		
      /* Run callback if registered and enabled */
		if (callback_status & (1 << USART_CALLBACK_BUFFER_TRANSMITTED)) {
			(*(module->callback[USART_CALLBACK_BUFFER_TRANSMITTED]))(module);
		}

	/* Check if the Receive Complete interrupt has occurred, and that
	 * there's more data to receive */
	}

	if (interrupt_status & SERCOM_USART_INTFLAG_RXC) {

			/* Read out the status code and mask away all but the 4 LSBs*/
			error_code = (uint8_t)(usart_hw->STATUS.reg & SERCOM_USART_STATUS_MASK);

			/* Check if an error has occurred during the receiving */
			if (error_code) {
				/* Check which error occurred */
				if (error_code & SERCOM_USART_STATUS_FERR) {
					/* Store the error code and clear flag by writing 1 to it */
					module->rx_status = STATUS_ERR_BAD_FORMAT;
					usart_hw->STATUS.reg |= SERCOM_USART_STATUS_FERR;
				} else if (error_code & SERCOM_USART_STATUS_BUFOVF) {
					/* Store the error code and clear flag by writing 1 to it */
					module->rx_status = STATUS_ERR_OVERFLOW;
					usart_hw->STATUS.reg |= SERCOM_USART_STATUS_BUFOVF;
				} else if (error_code & SERCOM_USART_STATUS_PERR) {
					/* Store the error code and clear flag by writing 1 to it */
					module->rx_status = STATUS_ERR_BAD_DATA;
					usart_hw->STATUS.reg |= SERCOM_USART_STATUS_PERR;
				}
#ifdef FEATURE_USART_LIN_SLAVE
				else if (error_code & SERCOM_USART_STATUS_ISF) {
					/* Store the error code and clear flag by writing 1 to it */
					module->rx_status = STATUS_ERR_PROTOCOL;
					usart_hw->STATUS.reg |= SERCOM_USART_STATUS_ISF;
				}
#endif
#ifdef FEATURE_USART_COLLISION_DECTION
				else if (error_code & SERCOM_USART_STATUS_COLL) {
					/* Store the error code and clear flag by writing 1 to it */
					module->rx_status = STATUS_ERR_PACKET_COLLISION;
					usart_hw->STATUS.reg |= SERCOM_USART_STATUS_COLL;
				}
#endif

				/* Run callback if registered and enabled */
				if (callback_status
						& (1 << USART_CALLBACK_ERROR)) {
					(*(module->callback[USART_CALLBACK_ERROR]))(module);
				}

			} else {

				/* Read current packet from DATA register,
				 * increment buffer pointer and decrement buffer length */
				uint16_t received_data = (usart_hw->DATA.reg & SERCOM_USART_DATA_MASK);

				/* Read value will be at least 8-bits long */
				module->rx_buffer_ptr[p_board_at91samd20_uart_info->rx_buffer_w]  = received_data;
            //
            if(p_board_at91samd20_uart_info->rx_buffer_w==p_board_at91samd20_uart_info->rx_buffer_r){
               __fire_io_int(ofile_lst[p_board_at91samd20_uart_info->desc_r].owner_pthread_ptr_read);
            }
				/* Increment 8-bit pointer */
            if(p_board_at91samd20_uart_info->rx_buffer_w<MAX_AT91SAMD20_RX_BUFFER){
               p_board_at91samd20_uart_info->rx_buffer_w++;
            }else{
              p_board_at91samd20_uart_info->rx_buffer_w = 0;
            }
           
            //
				if (module->character_size == USART_CHARACTER_SIZE_9BIT) {
					/* 9-bit data, write next received byte to the buffer */
					module->rx_buffer_ptr[p_board_at91samd20_uart_info->rx_buffer_w]  = (received_data >> 8);
					/* Increment 8-bit pointer */
					if(p_board_at91samd20_uart_info->rx_buffer_w<MAX_AT91SAMD20_RX_BUFFER){
                  p_board_at91samd20_uart_info->rx_buffer_w++;
               }else{
                  p_board_at91samd20_uart_info->rx_buffer_w = 0;
               }
      
				}
            //
            module->rx_status = STATUS_OK;
            
			}
		/*} else {
			// This should not happen. Disable Receive Complete interrupt.
			usart_hw->INTENCLR.reg = SERCOM_USART_INTFLAG_RXC;
		}*/
	}

#ifdef FEATURE_USART_HARDWARE_FLOW_CONTROL
	if (interrupt_status & SERCOM_USART_INTFLAG_CTSIC) {
		/* Disable interrupts */
		usart_hw->INTENCLR.reg = SERCOM_USART_INTENCLR_CTSIC;
		/* Clear interrupt flag */
		usart_hw->INTFLAG.reg = SERCOM_USART_INTFLAG_CTSIC;

		/* Run callback if registered and enabled */
		if (callback_status & (1 << USART_CALLBACK_CTS_INPUT_CHANGE)) {
			(*(module->callback[USART_CALLBACK_CTS_INPUT_CHANGE]))(module);
		}
	}
#endif

#ifdef FEATURE_USART_LIN_SLAVE
	if (interrupt_status & SERCOM_USART_INTFLAG_RXBRK) {
		/* Disable interrupts */
		usart_hw->INTENCLR.reg = SERCOM_USART_INTENCLR_RXBRK;
		/* Clear interrupt flag */
		usart_hw->INTFLAG.reg = SERCOM_USART_INTFLAG_RXBRK;

		/* Run callback if registered and enabled */
		if (callback_status & (1 << USART_CALLBACK_BREAK_RECEIVED)) {
			(*(module->callback[USART_CALLBACK_BREAK_RECEIVED]))(module);
		}
	}
#endif

#ifdef FEATURE_USART_START_FRAME_DECTION
	if (interrupt_status & SERCOM_USART_INTFLAG_RXS) {
		/* Disable interrupts */
		usart_hw->INTENCLR.reg = SERCOM_USART_INTENCLR_RXS;
		/* Clear interrupt flag */
		usart_hw->INTFLAG.reg = SERCOM_USART_INTFLAG_RXS;

		/* Run callback if registered and enabled */
		if (callback_status & (1 << USART_CALLBACK_START_RECEIVED)) {
			(*(module->callback[USART_CALLBACK_START_RECEIVED]))(module);
		}
	}
#endif
   //
   __hw_leave_interrupt();
   //
}


void _at91samd20_uart_x_prepare_rx(struct usart_module *module){
   /* Get a pointer to the hardware module instance */
	SercomUsart *const usart_hw = &(module->hw->USART);

	/* Enable the RX Complete Interrupt */
	usart_hw->INTENSET.reg = SERCOM_USART_INTFLAG_RXC;

#ifdef FEATURE_USART_LIN_SLAVE
	/* Enable the break character is received Interrupt */
	if(module->lin_slave_enabled) {
		usart_hw->INTENSET.reg = SERCOM_USART_INTFLAG_RXBRK;
	}
#endif

#ifdef FEATURE_USART_START_FRAME_DECTION
	/* Enable a start condition is detected Interrupt */
	if(module->start_frame_detection_enabled) {
		usart_hw->INTENSET.reg = SERCOM_USART_INTFLAG_RXS;
	}
#endif
}

/*-------------------------------------------
| Name:dev_at91samd20_uart_x_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91samd20_uart_x_load(board_at91samd20_uart_info_t * uart_info){
   
   return 0;
}

/*-------------------------------------------
| Name:dev_at91samd20_uart_x_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91samd20_uart_x_open(desc_t desc, int o_flag,
                            board_at91samd20_uart_info_t * uart_info){
                          
   //
   if(uart_info->desc_r<0 && uart_info->desc_w<0) {
      uint8_t instance_index = _sercom_get_sercom_inst_index(uart_info->usart_instance.hw);
      //
      uart_info->usart_instance.rx_buffer_ptr=uart_info->internal_buffer_rx;
      //
      _sercom_set_handler(instance_index, _at91samd20_uart_x_interrupt_handler);
      _sercom_instances[instance_index] = uart_info;
     //! [setup_enable]
      usart_enable(&uart_info->usart_instance);
   }
   //
   if(o_flag & O_RDONLY) {
      if(uart_info->desc_r<0) {
         uart_info->desc_r = desc;
         //
         uart_info->rx_buffer_r = 0;
         uart_info->rx_buffer_w = 0;
         _at91samd20_uart_x_prepare_rx(&uart_info->usart_instance);
      }
      else
         return -1;                //already open
   }
   //
   if(o_flag & O_WRONLY) {
      if(uart_info->desc_w<0) {
         uart_info->desc_w = desc;
         //
         uart_info->tx_buffer_r = 0;
         uart_info->tx_buffer_w = 0;
      }
      else
         return -1;                //already open
   }
   //
   if(!ofile_lst[desc].p)
      ofile_lst[desc].p=uart_info;

   //unmask IRQ
   if(uart_info->desc_r>=0 && uart_info->desc_w>=0) {
   }
   return 0;
}

/*-------------------------------------------
| Name:dev_at91samd20_uart_x_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91samd20_uart_x_close(desc_t desc){
  board_at91samd20_uart_info_t * p_uart_info = (board_at91samd20_uart_info_t*)ofile_lst[desc].p;
  //
  if(!p_uart_info)
   return -1;
  // 
  if(ofile_lst[desc].oflag & O_RDONLY) {
      if(!ofile_lst[desc].nb_reader) {
         p_uart_info->desc_r = -1;
      }
   }
   //
   if(ofile_lst[desc].oflag & O_WRONLY) {
      if(!ofile_lst[desc].nb_writer) {
         p_uart_info->desc_w = -1;
      }
   }
   //
   if(p_uart_info->desc_r<0 && p_uart_info->desc_w<0) {
   }

   return 0;
}

/*-------------------------------------------
| Name:dev_at91samd20_uart_x_isset_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91samd20_uart_x_isset_read(desc_t desc){
   board_at91samd20_uart_info_t * p_board_at91samd20_uart_info = (board_at91samd20_uart_info_t*)ofile_lst[desc].p;
   //
   if(!p_board_at91samd20_uart_info)
      return -1;
   //
   if(p_board_at91samd20_uart_info->rx_buffer_r!=p_board_at91samd20_uart_info->rx_buffer_w){
      return 0;
   }
   //
   return -1;
}

/*-------------------------------------------
| Name:dev_at91samd20_uart_x_isset_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91samd20_uart_x_isset_write(desc_t desc){
  board_at91samd20_uart_info_t * p_board_at91samd20_uart_info = (board_at91samd20_uart_info_t*)ofile_lst[desc].p;
   //
   if(!p_board_at91samd20_uart_info)
      return -1;
   //
   if(p_board_at91samd20_uart_info->usart_instance.tx_status==STATUS_OK){
     return 0;
   }
   //
   return -1;
}

/*-------------------------------------------
| Name:dev_at91samd20_uart_x_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91samd20_uart_x_read(desc_t desc, char* buf,int size){
   int cb=0;
   board_at91samd20_uart_info_t * p_board_at91samd20_uart_info = (board_at91samd20_uart_info_t*)ofile_lst[desc].p;
   uint16_t r = p_board_at91samd20_uart_info->rx_buffer_r;
   uint16_t w = p_board_at91samd20_uart_info->rx_buffer_w;
   uchar8_t* p_rx_buffer = p_board_at91samd20_uart_info->internal_buffer_rx;
   //
   if(!p_board_at91samd20_uart_info)
      return -1;
   //
   while(r!=w){
     buf[cb] = p_rx_buffer[r];
     cb++;
     if(cb==size)
       break;
     if(r<MAX_AT91SAMD20_RX_BUFFER)
       r++;
     else
       r=0;
   }
   //
   p_board_at91samd20_uart_info->rx_buffer_r=r;
   //
   return cb;
}

/*-------------------------------------------
| Name:dev_at91samd20_uart_x_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91samd20_uart_x_write(desc_t desc, const char* buf,int size){
   int cb;
   board_at91samd20_uart_info_t * p_board_at91samd20_uart_info = (board_at91samd20_uart_info_t*)ofile_lst[desc].p;
   //
   if(!p_board_at91samd20_uart_info)
      return -1;
   //
   if(size<MAX_AT91SAMD20_TX_BUFFER){
      cb=size;
   }else{
      cb=MAX_AT91SAMD20_TX_BUFFER;
   }
   //
   memcpy(p_board_at91samd20_uart_info->internal_buffer_tx,buf,cb);
   // 
   usart_write_buffer_job(&p_board_at91samd20_uart_info->usart_instance, p_board_at91samd20_uart_info->internal_buffer_tx, cb);
   //
   return cb;
}

/*-------------------------------------------
| Name:dev_at91samd20_uart_x_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91samd20_uart_x_seek(desc_t desc,int offset,int origin){
   return 0;
}

/*-------------------------------------------
| Name:dev_at91samd20_uart_x_ioctl
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91samd20_uart_x_ioctl(desc_t desc,int request,va_list ap) {
   return 0;
}


/*============================================
| End of Source  : dev_at91samd20_uart_x.c
==============================================*/