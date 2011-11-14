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

//#include "dev/arch/win32/dev_win32_com0/dev_socketsrv_0.h"

#include "kernel/core/types.h"
#include "kernel/core/interrupt.h"
#include "kernel/core/system.h"
#include "kernel/core/fcntl.h"

#include "kernel/fs/vfs/vfsdev.h"
#include "kernel/fs/vfs/vfstypes.h"

#include "lib/libc/termios/termios.h"

#include "dev_linux_com0.h"
#include "manage_order.h"

/*===========================================
Global Declaration
=============================================*/
/*valeurs pour driver serie*/
#define OS_RS232_BAUDRATE 9600L //19200L //9600L
#define IER_MASK_RCV     0x00000001 // //IT sur RXRDY 0x00000128
#define IER_MASK_SND     0x00000002 //0x00000022
#define CYGNUM_HAL_H_PRIOR 3

int dev_linux_com0_load(void);
int dev_linux_com0_open(desc_t desc, int o_flag);
int dev_linux_com0_close(desc_t desc);
int dev_linux_com0_isset_read(desc_t desc);
int dev_linux_com0_isset_write(desc_t desc);
int dev_linux_com0_read(desc_t desc, char* buf,int size);
int dev_linux_com0_write(desc_t desc, const char* buf,int size);
int dev_linux_com0_seek(desc_t desc,int offset,int origin);
int dev_linux_com0_ioctl(desc_t desc,int request,va_list ap);
void dev_linux_com0_snd(void);
void dev_linux_com0_rcv(void);

//ISR and DSR
cyg_uint32 dev_linux_com0_isr(cyg_vector_t vector, cyg_addrword_t data);
void dev_linux_com0_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data);

const char dev_linux_com0_name[]="ttys0\0";

dev_map_t dev_linux_com0_map={
   dev_linux_com0_name,
   S_IFCHR,
   dev_linux_com0_load,
   dev_linux_com0_open,
   dev_linux_com0_close,
   dev_linux_com0_isset_read,
   dev_linux_com0_isset_write,
   dev_linux_com0_read,
   dev_linux_com0_write,
   dev_linux_com0_seek,
   dev_linux_com0_ioctl
};

#define UART_FIFO_INPUT_BUFFER_SZ 128
#define UART_S0_OUTPUT_BUFFER_SIZE 256

/*buffer et variables de lecture*/
unsigned char uart_0_rcv_buffer[UART_FIFO_INPUT_BUFFER_SZ] = {0};
static volatile signed int _linux_uart_s0_input_r; //associée au ptr de lecture du user ds buffer réception 
static volatile signed int _linux_uart_s0_input_w; //associée au ptr d'écriture de kernel ds buffer réception
static volatile int _linux_uart_s0_desc_rd = -1; //flag O_RDONLY

/*buffer et variables d'ecriture*/
unsigned char uart_0_snd_buffer[UART_S0_OUTPUT_BUFFER_SIZE] = {0};
static volatile signed int _linux_uart_s0_output_r;
static volatile signed int _linux_uart_s0_output_w;
static volatile signed int _linux_uart_s0_desc_wr = -1; //flag O_WRONLY
int  _linux_uart_s0_loaded = 0;

/*variables pour la gestion de l'IT*/
cyg_interrupt _linux_uart_s0_it;
cyg_handle_t _linux_uart_s0_handle;
//desc_t ttyp0_desc[3]={-1};//O_RDONLY,O_WRONLY,O_RDWR (O_RDONLY|O_WRONLY),

//termios
static struct termios ttys_termios;
static int termios2ttys(struct termios* termios_p);
//xon/xoff flow control
#define FLOWCTRL_XON    0x11 //ctrl-Q 
#define FLOWCTRL_XOFF   0x13 //ctrl-S

#define XONOFF_DSBL     0
#define XONOFF_ENBL     1
static volatile unsigned char _xonoff_option = XONOFF_DSBL;

#define STATUS_IDLE      0
#define STATUS_RCV_XOFF  1
#define STATUS_SND_XOFF  1

static volatile unsigned char _snd_xonoff_status = STATUS_IDLE;
static volatile unsigned char _rcv_xonoff_status = STATUS_IDLE;

typedef struct s2s {
        speed_t ts;
        long ns;
}s2s_t;

static s2s_t const s2s[] = {
        { B0,                0 },
        { B50,              50 },
        { B75,              75 },
        { B110,            110 },
        { B134,            134 },
        { B150,            150 },
        { B200,            200 },
        { B300,            300 },
        { B600,            600 },
        { B1200,          1200 },
        { B1800,          1800 },
        { B2400,          2400 },
        { B4800,          4800 },
        { B9600,          9600 },
        { B19200,        19200 },
        { B38400,        38400 },
        { B57600,        57600 },
        { B115200,      115200 },
        { B230400,      230400 },
        { B460800,      460800 }
};

static int rcv_flag = 0;
/*===========================================
Implementation
=============================================*/

/*-------------------------------------------
| Name:dev_linux_com0_isr
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
/*Handler d'IT*/
cyg_uint32 dev_linux_com0_isr(cyg_vector_t vector, cyg_addrword_t data)
{
	/*Blocage d'une IT similaire tant que la DSR ne sait pas exécutée*/
	cyg_interrupt_mask(vector);
	u_int32 _usart_csr;
	u_int32 _usart_imr;
	if(_linux_uart_s0_loaded) {
		do {
			send_order_r('R', US_CSR, &_usart_csr);
			send_order_r('R', US_IMR, &_usart_imr);
			if((_usart_csr&IER_MASK_RCV)==IER_MASK_RCV) {
				//dev_linux_com0_read(_linux_uart_s0_desc_rd, uart_0_rcv_buffer, 1);
				dev_linux_com0_rcv();
			}			
			if((_usart_csr&IER_MASK_SND)==IER_MASK_SND) {
				dev_linux_com0_snd();
			}
		}while((_usart_csr & _usart_imr & (IER_MASK_RCV)) || (_usart_csr & _usart_imr & (IER_MASK_SND)) );
	}
	/*ACK de l'IT au CPU*/
	cyg_interrupt_acknowledge(vector);
	/*Informe kernel d'exécuter DSR*/
	return(CYG_ISR_HANDLED | CYG_ISR_CALL_DSR);
}

/*-------------------------------------------
| Name:dev_linux_com0_dsr
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
/*DSR*/
void dev_linux_com0_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
	/*Autorise à nouveau les IT de ce type*/
	if(rcv_flag) {
		rcv_flag=0;
		__fire_io_int(ofile_lst[_linux_uart_s0_desc_rd].owner_pthread_ptr_read);
	}
	if(_linux_uart_s0_desc_wr>=0 && _linux_uart_s0_output_r==_linux_uart_s0_output_w){
		__fire_io_int(ofile_lst[_linux_uart_s0_desc_wr].owner_pthread_ptr_write);
	}
	cyg_interrupt_unmask(vector);
}

/*-------------------------------------------
| Name:dev_linux_com0_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_com0_load(void)
{
	cyg_vector_t serial_vector = CYGNUM_HAL_INT_SERIAL;
	cyg_priority_t serial_prior = CYGNUM_HAL_H_PRIOR;
		
	if(_linux_uart_s0_loaded)
	      return 0;//yes
	
	_linux_uart_s0_desc_rd = -1;
	_linux_uart_s0_desc_wr = -1;
	
	_xonoff_option = XONOFF_DSBL;
	_rcv_xonoff_status = STATUS_IDLE;
	_snd_xonoff_status = STATUS_IDLE;
		   //
	cfmakeraw(&ttys_termios);
	cfsetispeed (&ttys_termios,B9600);
	cfsetospeed (&ttys_termios,B9600);
		   
	ttys_termios.c_iflag &= ~(IXOFF|IXON); //xon/xoff disable
	//
	ttys_termios.c_cc[VTIME]=0;// no timeout, blocking call
	//inter_char_timer = 0;
		
	_linux_uart_s0_loaded=1;
	 //Primitive de creation de l'IT
	cyg_interrupt_create(serial_vector, serial_prior, 0,
	  			&dev_linux_com0_isr, &dev_linux_com0_dsr,
	   			&_linux_uart_s0_handle, &_linux_uart_s0_it);
	//Liaison entre l'IT crée et le vecteur d'IT
	cyg_interrupt_attach(_linux_uart_s0_handle);
	cyg_interrupt_unmask(serial_vector);
	
	return 0;
}

/*-------------------------------------------
| Name:dev_linux_com0_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_com0_open(desc_t desc, int o_flag)
{
	if(_linux_uart_s0_desc_rd<0 && _linux_uart_s0_desc_wr<0){
		send_order_w('W', US_IDR, 0xffffffff); // Disable all USART interrupts.
	    send_order_w('W', US_MR, 0x000008c0); // Normal mode, 1 stop bit, no parity, async mode, 8 bits, MCK.
	    send_order_w('W', US_IDR, 0xffffffff); // Disable all USART interrupts.
	    send_order_w('W', US_TTGR, 5); // Transmit time guard in number of bit periods.
	    send_order_w('W', US_BRGR, __KERNEL_CPU_FREQ / OS_RS232_BAUDRATE / 16); // Set baud rate.
	 }
	/*Test read modifier à refaire comme avant if(o_flag & O_WRONLY) */
	if(o_flag & O_WRONLY){
   		if(_linux_uart_s0_desc_wr>=0) //already open: exclusive resource.
  			  return -1;
		send_order_w('W', US_CR, 0x00000040);//Enable transmitter
   		_linux_uart_s0_output_r = -1;
   		_linux_uart_s0_output_w = 0;
   		_linux_uart_s0_desc_wr = desc;
   		 // Interrupt on TXRDY
   		send_order_w('W', US_IER, IER_MASK_SND);
	}
	if(o_flag & O_RDONLY){
		if(_linux_uart_s0_desc_rd>=0) //already open: exclusive resource.
			return -1;
		send_order_w('W', US_CR, 0x00000010);//Enable receiver
	    _linux_uart_s0_input_r = 0;
	    _linux_uart_s0_input_w = 0;
	    _linux_uart_s0_desc_rd= desc;
			
	    // Interrupt on RXRDY
	    send_order_w('W', US_IER, IER_MASK_RCV); 
	}
	//_linux_uart_s0_loaded = 1;
	return 0;	
}

/*-------------------------------------------
| Name:dev_linux_com0_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_com0_close(desc_t desc)
{
	if(ofile_lst[desc].oflag & O_RDONLY){
		if(!ofile_lst[desc].nb_reader){
			// Disable Interrupt on RXRDY
			send_order_w('W', US_IDR, IER_MASK_RCV); 
			//send_order_w('W', US_CR, 0x00000020); //Disable receiver
			_linux_uart_s0_desc_rd = -1;
		}
	}
	//
	if(ofile_lst[desc].oflag & O_WRONLY){
		if(!ofile_lst[desc].nb_writer){
			// Disable Interrupt on TXRDY
			send_order_w('W', US_IDR, IER_MASK_SND);
			//send_order_w('W', US_CR, 0x00000080); 
			_linux_uart_s0_desc_wr = -1;
		}
	}
	//close all
	if(_linux_uart_s0_desc_wr<0 && _linux_uart_s0_desc_rd<0){
			send_order_w('W', US_IDR, 0xffffffff);  // Disable all USART interrupts.
			send_order_w('W', US_CR, 0x000000a0); // Disable receiver, disable transmitter.
	}
		
	return 0;	
}
//ne pas oublier de tester le nombre d'écrivains et de lecteurs

/*-------------------------------------------
| Name:dev_linux_com0_isset_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_com0_isset_read(desc_t desc)
{
	if(_linux_uart_s0_input_w!=_linux_uart_s0_input_r) {
			return 0;
	}
	else {
		    return -1;
	}
}

/*-------------------------------------------
| Name:dev_linux_com0_isset_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_com0_isset_write(desc_t desc)
{
	if(_linux_uart_s0_output_w==_linux_uart_s0_output_r){
		_linux_uart_s0_output_r=-1;
	    return 0;
	}
	else 
	      return -1;
}

/*-------------------------------------------
| Name:dev_linux_com0_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_com0_read(desc_t desc, char* buf,int size)
{

	int cb = 0;
	while(_linux_uart_s0_input_r != _linux_uart_s0_input_w){
		    buf[cb++] = uart_0_rcv_buffer[_linux_uart_s0_input_r];
		    _linux_uart_s0_input_r = (_linux_uart_s0_input_r+1)&(~UART_FIFO_INPUT_BUFFER_SZ);
		    if(cb == size)	break;
	}
	return cb;
}

/*-------------------------------------------
| Name:dev_linux_com0_rcv
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void dev_linux_com0_rcv(void)
{
	/*Récupération du caractère lu*/
	u_int32 rcv_data = 0;
	unsigned char c;
	u_int32 _csr;
	
	send_order_r('R', US_RHR, &rcv_data);
	c = (unsigned char) rcv_data;
	//un caractère au moins risque d'arrivé
	if(_linux_uart_s0_input_w == _linux_uart_s0_input_r) {
		rcv_flag=1;
	}
	/*copie dans le buffer*/
	uart_0_rcv_buffer[_linux_uart_s0_input_w] = c;
	_linux_uart_s0_input_w = (_linux_uart_s0_input_w+1)&(~UART_FIFO_INPUT_BUFFER_SZ);
	//_linux_uart_s0_input_w++;
}
/*-------------------------------------------
| Name:dev_linux_com0_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_com0_write(desc_t desc, const char* buf,int size){
	unsigned char snd_data;
	u_int32 csr_value ;//= 0;
		
	//variables pour les signaux
	cyg_hal_sys_sigset_t ens, old;
	CYG_HAL_SYS_SIGEMPTYSET(&ens);
	CYG_HAL_SYS_SIGADDSET(&ens, CYG_HAL_SYS_SIGIO);
	//sigaddset(&ens, SIGALRM);
	cyg_hal_sys_sigprocmask(CYG_HAL_SYS_SIG_BLOCK, &ens, &old); //masquage de SIGIO	

	if(size>=UART_S0_OUTPUT_BUFFER_SIZE)
		size = UART_S0_OUTPUT_BUFFER_SIZE-1;
	      
	memcpy(uart_0_snd_buffer,buf,size);
	//buffer ready
	snd_data = uart_0_snd_buffer[0];
	do {
		send_order_r('R', US_CSR, &csr_value);
	}while (!(csr_value & (0x00000002)));    // Wait until TB empty

	//Disable all IT
	__clr_irq();
	_linux_uart_s0_output_r = 0;
	_linux_uart_s0_output_w = size;
	      	
	send_order_w('W', US_IER, 2);
	send_order_w('W', US_THR, snd_data);	
	  	
	//Enable all IT	
	__set_irq();
	//deblocage SIGIO
	cyg_hal_sys_sigprocmask(CYG_HAL_SYS_SIG_UNBLOCK, &ens, &old);
	return size;
}

/*-------------------------------------------
| Name:dev_linux_com0_snd
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void dev_linux_com0_snd(void)
{
	unsigned char snd_data;
	u_int32 data;
	  	
   	if(_linux_uart_s0_output_r<0)
      		return;
   	_linux_uart_s0_output_r++;
  	//_linux_uart_s0_output_r = (_linux_uart_s0_output_r+1)&(~UART_S0_OUTPUT_BUFFER_SIZE);
	if(_linux_uart_s0_output_r < _linux_uart_s0_output_w){
      	snd_data = uart_0_snd_buffer[_linux_uart_s0_output_r];
		data = (u_int32)snd_data;		
		send_order_w('W', US_THR, data);
   	}
	else if(_linux_uart_s0_desc_wr>=0 && _linux_uart_s0_output_r==_linux_uart_s0_output_w){
		//__fire_io_int(ofile_lst[_linux_uart_s0_desc_wr].owner_pthread_ptr_write);
		send_order_w('W', US_IDR, 2);
		send_order_w('W', US_IMR, 0xfffffffd);	
	}
}

/*-------------------------------------------
| Name:dev_linux_com0_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_com0_seek(desc_t desc,int offset,int origin){
   return 0;
}

/*-------------------------------------------
| Name:dev_linux_com0_ioctl
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_com0_ioctl(desc_t desc,int request,va_list ap)
{
	struct termios* termios_p = (struct termios*)0;
  
	switch(request){
      //
		case TIOCSSERIAL:{
			unsigned long speed  = va_arg( ap, unsigned long);
			if(speed<0)
				return -1;
			send_order_w('W', US_BRGR, __KERNEL_CPU_FREQ / speed / 16);// Set baud rate.
		}break;

		/* If optional_actions is TCSANOW, the change will occur immediately.*/
		/* If optional_actions is TCSADRAIN, the change will occur after all output written to fildes is transmitted. 
         This function should be used when changing parameters that affect output.*/
		/* If optional_actions is TCSAFLUSH, the change will occur after all output written to fildes is transmitted, 
         and all input so far received but not read will be discarded before the change is made. 
         return 0;*/
      case TCSETS:
      case TCSAFLUSH:
      case TCSADRAIN:
      case TCSANOW:
         termios_p = va_arg( ap, struct termios*);
         if(!termios_p)
            return -1;
         termios2ttys(termios_p);

      break;

      //
      case TCGETS:
         termios_p = va_arg( ap, struct termios*);
         if(!termios_p)
            return -1;

         memcpy(termios_p,&ttys_termios,sizeof(struct termios));
      break;

      //
      default:
         return -1;

   }

   return 0;
}

/*-------------------------------------------
| Name:termios2ttys
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int termios2ttys(struct termios* termios_p)
{
   const struct s2s *sp;
   long n_speed;
   speed_t speed;

   //xon/xoff
   if((termios_p->c_iflag&IXON) != (ttys_termios.c_iflag&IXON)){
      if(termios_p->c_iflag&IXON){
         termios_p->c_iflag |= (IXOFF|IXON);
         _xonoff_option = 1;
      }else{
         termios_p->c_iflag &= ~(IXOFF|IXON);
         _xonoff_option = 0;

      }
   }else if((termios_p->c_iflag&IXOFF) != (ttys_termios.c_iflag&IXOFF)){
     if(termios_p->c_iflag&IXOFF){
         termios_p->c_iflag |= (IXOFF|IXON);
         _xonoff_option = 1;
      }else{
         termios_p->c_iflag &= ~(IXOFF|IXON);
         _xonoff_option = 0;
      }
   }
  
   //speed
   speed = cfgetospeed(termios_p);
   for (sp = s2s; sp < s2s + (sizeof(s2s) / sizeof(s2s[0])); sp++) {
      if (sp->ts == speed){
         unsigned int div;
         unsigned int prescaler=0;
         n_speed = sp->ns;
         //tset speed
         send_order_w('W', US_BRGR, __KERNEL_CPU_FREQ / n_speed / 16); // Set baud rate.
         break;
      }
   }

   //timeout read in blocking call
//   if(termios_p->c_cc[VTIME]){
//      OS_StopTimer(&dev_at91m55800a_uart_s0_timer);
//      inter_char_timer = termios_p->c_cc[VTIME];
//      OS_RetriggerTimer(&dev_at91m55800a_uart_s0_timer);
//   }else{
//      OS_StopTimer(&dev_at91m55800a_uart_s0_timer);
//   }
//   
   //
   memcpy(&ttys_termios,termios_p,sizeof(struct termios));
   return 0;
}

/*============================================
| End of Source  : dev_linux_com0.c
|---------------------------------------------
| Historic:
|---------------------------------------------
| Authors     | Date     | Comments
| $Log: not supported by cvs2svn $
| Revision 1.1  2009/03/30 15:49:19  jjp
| first import of tauon
|
| Revision 1.1  2009/03/30 11:16:09  jjp
| First import of tauon
|
|---------------------------------------------
==============================================*/
