/*
The contents of this file are subject to the Mozilla Public License Version 1.1 
(the "License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS" basis, 
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License for the 
specific language governing rights and limitations under the License.

The Original Code is ______________________________________.

The Initial Developer of the Original Code is ________________________.
Portions created by ______________________ are Copyright (C) ______ _______________________.
All Rights Reserved.

Contributor(s): ______________________________________.

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
#include "dev_k60n512_uart_s3.h"

#if defined(USE_ECOS)
#endif
/*===========================================
Global Declaration
=============================================*/
const char dev_k60n512_uart_s3_name[]="ttys0\0\0";

static int dev_k60n512_uart_s3_load(void);
static int dev_k60n512_uart_s3_isset_read(desc_t desc);
static int dev_k60n512_uart_s3_isset_write(desc_t desc);
static int dev_k60n512_uart_s3_open(desc_t desc, int o_flag);
static int dev_k60n512_uart_s3_close(desc_t desc);
static int dev_k60n512_uart_s3_seek(desc_t desc,int offset,int origin);
static int dev_k60n512_uart_s3_read(desc_t desc, char* buf,int cb);
static int dev_k60n512_uart_s3_write(desc_t desc, const char* buf,int cb);
static int dev_k60n512_uart_s3_ioctl(desc_t desc,int request,va_list ap);

dev_map_t dev_k60n512_uart_s3_map={
   dev_k60n512_uart_s3_name,
   S_IFCHR,
   dev_k60n512_uart_s3_load,
   dev_k60n512_uart_s3_open,
   dev_k60n512_uart_s3_close,
   dev_k60n512_uart_s3_isset_read,
   dev_k60n512_uart_s3_isset_write,
   dev_k60n512_uart_s3_read,
   dev_k60n512_uart_s3_write,
   dev_k60n512_uart_s3_seek,
   dev_k60n512_uart_s3_ioctl
};

//
#define	KINETIS_UART_IRQ_NO				52
#define KINETIS_UART_IRQ_PRIORITY		3


//
typedef struct board_kinetis_uart_info_st {
	unsigned int uart_base;
	kernel_pthread_mutex_t     mutex;

	desc_t _desc_rd;
	desc_t _desc_wr;

	unsigned int _input_r;
	unsigned int _input_w;
	unsigned int _output_r;
	unsigned int _output_w;

	//
	unsigned char *_input_buf;
	unsigned char *_output_buf;
	unsigned char xmit;
	
	unsigned char allowed_irqs;
	//for eCos IRQ
	cyg_handle_t  _irq_handle;
	cyg_interrupt _irq_it;

	//for lepton event
	unsigned char _flag_w_irq;
	unsigned char _flag_r_irq;

} board_kinetis_uart_info_t;

board_kinetis_uart_info_t kinetis_uart_info;
#define	INPUT_BUF_SIZE	16
unsigned char input_buf[INPUT_BUF_SIZE];

#if defined(USE_ECOS)

#define UART3_RX CYGHWR_HAL_KINETIS_PIN(C, 16, 3, 0)
#define UART3_TX CYGHWR_HAL_KINETIS_PIN(C, 17, 3, 0)

__externC void  hal_set_pin_function(cyg_uint32 pin);
__externC void 	hal_freescale_uart_setbaud(cyg_uint32 uart_p, cyg_uint32 baud);

static cyg_uint32 _kinetis_uart_isr(cyg_vector_t vector, cyg_addrword_t data);
static void _kinetis_uart_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data);
#endif

/*===========================================
Implementation
=============================================*/
/*-------------------------------------------
| Name:dev_k60n512_uart_s3_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_uart_s3_load(void){
	cyg_uint32 regval;
	pthread_mutexattr_t  mutex_attr=0;
	
	//
	kinetis_uart_info.uart_base = 0x4006d000;
	kinetis_uart_info.allowed_irqs = 0;
	kinetis_uart_info._desc_rd = -1;
	kinetis_uart_info._desc_wr = -1;
	kinetis_uart_info._input_buf = input_buf;
	kernel_pthread_mutex_init(&kinetis_uart_info.mutex,&mutex_attr);
	
	// configure PORT pins
	hal_set_pin_function(UART3_RX);
	hal_set_pin_function(UART3_TX);

	// configure baud rate
	hal_freescale_uart_setbaud(kinetis_uart_info.uart_base, 38400/*115200*/);

	//no parity + 8 bits data
	regval = 0;
	HAL_WRITE_UINT8(kinetis_uart_info.uart_base + REG_UART_C1, regval);

	return 0;
}

/*-------------------------------------------
| Name:dev_k60n512_uart_s3_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_uart_s3_open(desc_t desc, int o_flag){
	cyg_uint32 regval = 0;
	
	if(o_flag & O_RDONLY){
		if(kinetis_uart_info._desc_rd<0) {
			kinetis_uart_info._desc_rd = desc;
			//Receiver On + Receiver Full Interrupt On
			kinetis_uart_info.allowed_irqs |= REG_UART_C2_RIE | REG_UART_C2_RE;
		}
		else
			return -1; //already open
	}

	if(o_flag & O_WRONLY){
		if(kinetis_uart_info._desc_wr<0) {
			kinetis_uart_info._desc_wr = desc;
			kinetis_uart_info._output_r = -1;
			//Transmitter On
			kinetis_uart_info.allowed_irqs |= REG_UART_C2_TE;
		}
		else
			return -1; //already open
	}
	
	if(!ofile_lst[desc].p)
		ofile_lst[desc].p=&kinetis_uart_info;

		//unmask IRQ and enable ksz
	if(kinetis_uart_info._desc_rd>=0 && kinetis_uart_info._desc_wr>=0){
		cyg_interrupt_create((cyg_vector_t)KINETIS_UART_IRQ_NO,
				KINETIS_UART_IRQ_PRIORITY,
				// Data item passed to interrupt handler
				(cyg_addrword_t)&kinetis_uart_info,   
				_kinetis_uart_isr,
				_kinetis_uart_dsr,
				&kinetis_uart_info._irq_handle,
				&kinetis_uart_info._irq_it);
		
		
		cyg_interrupt_attach(kinetis_uart_info._irq_handle);
		cyg_interrupt_unmask((cyg_vector_t)KINETIS_UART_IRQ_NO);
		//
		HAL_WRITE_UINT8(kinetis_uart_info.uart_base + REG_UART_C2, kinetis_uart_info.allowed_irqs);
	}
	return 0;
}

/*-------------------------------------------
| Name:dev_k60n512_uart_s3_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_uart_s3_close(desc_t desc){
	//nothing to do
	//save code space :)
	return 0;
}

/*-------------------------------------------
| Name:dev_k60n512_uart_s3_isset_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_uart_s3_isset_read(desc_t desc){
	board_kinetis_uart_info_t * p_uart_info = (board_kinetis_uart_info_t*)ofile_lst[desc].p;
	if(!p_uart_info)
		return -1;

	if(p_uart_info->_input_r != p_uart_info->_input_w)
		return 0;

	return -1;
}

/*-------------------------------------------
| Name:dev_k60n512_uart_s3_isset_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_uart_s3_isset_write(desc_t desc){
	board_kinetis_uart_info_t * p_uart_info = (board_kinetis_uart_info_t*)ofile_lst[desc].p;
	if(!p_uart_info)
		return -1;

	if(p_uart_info->_output_r==p_uart_info->_output_w) {
		p_uart_info->_output_r = -1;
		return 0;
	}

	return -1;
}

/*-------------------------------------------
| Name:dev_k60n512_uart_s3_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_uart_s3_read(desc_t desc, char* buf,int size){
	board_kinetis_uart_info_t * p_uart_info = (board_kinetis_uart_info_t*)ofile_lst[desc].p;
	int cb = 0;
	int count = 0;
	cb = (size>=INPUT_BUF_SIZE)?INPUT_BUF_SIZE:size;
	
	cyg_interrupt_mask((cyg_vector_t)KINETIS_UART_IRQ_NO);
	//
	while(p_uart_info->_input_r != p_uart_info->_input_w && count < cb) {
		buf[count++] = p_uart_info->_input_buf[p_uart_info->_input_r];
		
		if(++p_uart_info->_input_r >= INPUT_BUF_SIZE) {
			p_uart_info->_input_r = 0;
		}
	}
	//
	cyg_interrupt_unmask((cyg_vector_t)KINETIS_UART_IRQ_NO);
	return count;
}

/*-------------------------------------------
| Name:dev_k60n512_uart_s3_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_uart_s3_write(desc_t desc, const char* buf,int size){
	board_kinetis_uart_info_t * p_uart_info = (board_kinetis_uart_info_t*)ofile_lst[desc].p;
	cyg_uint32 uart_reg;
	
	kernel_pthread_mutex_lock(&p_uart_info->mutex);
	
	p_uart_info->_output_buf = (unsigned char *)buf;
	p_uart_info->_output_r = 0;
	p_uart_info->_output_w = size;
	//
	do {
		//cyg_thread_delay(100);
		//waiting for empty fifo
		HAL_READ_UINT8(p_uart_info->uart_base + REG_UART_S1, uart_reg);
	} while(!(uart_reg & REG_UART_S1_TDRE)); //while(!(uart_reg & REG_UART_S1_TC));//
	
	//write first byte in buf
	HAL_WRITE_UINT8(p_uart_info->uart_base + REG_UART_D, p_uart_info->_output_buf[p_uart_info->_output_r++]);
	p_uart_info->xmit=1;
	//transmitter irq enable
	HAL_READ_UINT8(p_uart_info->uart_base + REG_UART_C2, uart_reg);
	uart_reg |= REG_UART_C2_TIE;
	HAL_WRITE_UINT8(p_uart_info->uart_base + REG_UART_C2, uart_reg);

    kernel_pthread_mutex_unlock(&p_uart_info->mutex);
	return size;
}

/*-------------------------------------------
| Name:dev_k60n512_uart_s3_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_uart_s3_seek(desc_t desc,int offset,int origin){
   return 0;
}

/*-------------------------------------------
| Name:dev_k60n512_uart_s3_ioctl
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_uart_s3_ioctl(desc_t desc,int request,va_list ap) {
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
cyg_uint32 _kinetis_uart_isr(cyg_vector_t vector, cyg_addrword_t data) {
	board_kinetis_uart_info_t * p_uart_info = (board_kinetis_uart_info_t*)data;
	cyg_uint8 uart_sr;
	cyg_interrupt_mask(vector);
	cyg_interrupt_acknowledge(vector);
    
	//read status
    HAL_READ_UINT8(p_uart_info->uart_base + REG_UART_S1, uart_sr);
    
    //transmit
    if(p_uart_info->xmit && uart_sr & REG_UART_S1_TDRE) {
    	//
    	if(p_uart_info->_output_r != p_uart_info->_output_w) {
    		//finish buffer
    		HAL_WRITE_UINT8(p_uart_info->uart_base + REG_UART_D, p_uart_info->_output_buf[p_uart_info->_output_r++]);
    	}
    	else {
    		//all data sent
    		cyg_uint32 uart_reg;
    		HAL_READ_UINT8(p_uart_info->uart_base + REG_UART_C2, uart_reg);
    		uart_reg &= ~REG_UART_C2_TIE;
    		HAL_WRITE_UINT8(p_uart_info->uart_base + REG_UART_C2, uart_reg);    	
    		p_uart_info->_flag_w_irq=1;
    		p_uart_info->xmit=0;
    	}
    }
    //receive
    if(uart_sr & REG_UART_S1_RDRF) {
    	HAL_READ_UINT8(p_uart_info->uart_base + REG_UART_D, p_uart_info->_input_buf[p_uart_info->_input_w]);
    	if(++p_uart_info->_input_w >= INPUT_BUF_SIZE) {
    		p_uart_info->_input_w = 0;
    	}
    	p_uart_info->_flag_r_irq=1;
    }
    //
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
void _kinetis_uart_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data) {
	board_kinetis_uart_info_t * p_uart_info = (board_kinetis_uart_info_t*)data;
	
	//write
	if(p_uart_info->_desc_wr>=0 && p_uart_info->_flag_w_irq) {
		__fire_io_int(ofile_lst[p_uart_info->_desc_wr].owner_pthread_ptr_write);
		p_uart_info->_flag_w_irq = 0;
	}
	//read
	if(p_uart_info->_desc_rd>=0 && p_uart_info->_flag_r_irq) {
		__fire_io_int(ofile_lst[p_uart_info->_desc_rd].owner_pthread_ptr_read);
		p_uart_info->_flag_r_irq = 0;
	}
	cyg_interrupt_unmask(vector);
}

/** @} */
/** @} */
/** @} */

/*============================================
| End of Source  : dev_k60n512_uart_s3.c
==============================================*/
