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




/**
 * \addtogroup lepton_dev
 * @{
 *
 */

/**
 * \addtogroup hard_dev_at91m55800a 
 * @{
 *
 */

/**
 * \defgroup dev_uart_arm7_c Les uart de l'arm7 at91m55800a.
 * @{
 *
 * Les pilotes de périphériques pour les uart de l'arm7 at91m55800a..
 * Ces pilotes de périphériques gère les vitesses de 50 à 38400 bauds, le xon/xoff 
 * et supporte le timeout (VTIME) pour le mode bloquant.
 *    
 */


/**
 * \file
 * implementation du pilote de périphérique pour l'uart 2 de l'arm7 at91m55800a.
 * \author philippe le boulanger
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

#include "lib/libc/termios/termios.h"

#include "kernel/fs/vfs/vfsdev.h"

#include <ioat91m55800.h>
#include <intrinsic.h>

/*===========================================
Global Declaration
=============================================*/
const char dev_at91m55800a_uart_s2_name[]="ttys2\0";

int dev_at91m55800a_uart_s2_load(void);
int dev_at91m55800a_uart_s2_open(desc_t desc, int o_flag);
int dev_at91m55800a_uart_s2_close(desc_t desc);
int dev_at91m55800a_uart_s2_isset_read(desc_t desc);
int dev_at91m55800a_uart_s2_isset_write(desc_t desc);
int dev_at91m55800a_uart_s2_read(desc_t desc, char* buf,int size);
int dev_at91m55800a_uart_s2_write(desc_t desc, const char* buf,int size);
int dev_at91m55800a_uart_s2_seek(desc_t desc,int offset,int origin);
int dev_at91m55800a_uart_s2_ioctl(desc_t desc,int request,va_list ap);

static int termios2ttys(struct termios* termios_p);


dev_map_t dev_at91m55800a_uart_s2_map={
   dev_at91m55800a_uart_s2_name,
   S_IFCHR,
   dev_at91m55800a_uart_s2_load,
   dev_at91m55800a_uart_s2_open,
   dev_at91m55800a_uart_s2_close,
   dev_at91m55800a_uart_s2_isset_read,
   dev_at91m55800a_uart_s2_isset_write,
   dev_at91m55800a_uart_s2_read,
   dev_at91m55800a_uart_s2_write,
   dev_at91m55800a_uart_s2_seek,
   dev_at91m55800a_uart_s2_ioctl
};

//input fifo multibuffering 
#define MAX_POOL 16//4
#define MAX_POOL_BUF_SZ 512
#define UART_FIFO_INPUT_BUFFER_SZ (MAX_POOL*MAX_POOL_BUF_SZ)

typedef struct {
   uint16_t cb;
   uchar8_t* p;
}fifo_buf_pool_t;

static fifo_buf_pool_t _fifo_buf_pool[MAX_POOL];
static char _at91m55800a_uart_fifo_input_buffer[UART_FIFO_INPUT_BUFFER_SZ+1];

static volatile int buf_in_dma_no;
static volatile int buf_in_rcv_no;

//output buffer
#define UART_S2_OUTPUT_BUFFER_SIZE 400
static char _at91m55800a_uart_s2_output_buffer[UART_S2_OUTPUT_BUFFER_SIZE+1];

//
static volatile signed int _at91m55800a_uart_s2_input_r;
static volatile signed int _at91m55800a_uart_s2_input_w;

static volatile signed int _at91m55800a_uart_s2_output_r;
static volatile signed int _at91m55800a_uart_s2_output_w;

static volatile desc_t _at91m55800a_uart_s2_desc_rd = -1;   //O_RDONLY
static volatile desc_t _at91m55800a_uart_s2_desc_wr = -1;   //O_WRONLY

//#define OS_FSYS 10000000L //to do: warning!!! OSFSYS redeclaration cpu clock = 10Mhz
#define OS_RS232_BAUDRATE 9600L //19200L //9600L
#define OS_RS232_BAUDDIVIDE ((OS_FSYS+OS_RS232_BAUDRATE*8L)/(OS_RS232_BAUDRATE*16L)-1)

//for SLIP mode.
#define  N_SLIP  0x0001

//to remove : must be set to 0;
#ifdef __KERNEL_NET_IPSTACK
   #define SLIP_ACTIVE 1
#else
   #define SLIP_ACTIVE 0
#endif

volatile unsigned int _at91m55800a_uart_s2_slip_option = SLIP_ACTIVE;//0 

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
 
static unsigned char XMIT=0;


   
#define __pause_snd()
#define __resume_snd()

//termios
static struct termios ttys_termios;

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

//inter character timer 
#if defined (USE_SEGGER)
   //VTIME timer in units of 0.1 seconds (posix specification).
   static OS_TIMER dev_at91m55800a_uart_s2_timer;
   static volatile char inter_char_timer=0;
#endif

#define IER_MASK_RCV     0x00000128
#define IER_MASK_SND     0x00000022


#define US_STTTO     0x0800      /* Start Time-out */

static int dev_at91m55800a_uart_s2_loaded=0;

/*===========================================
Implementation
=============================================*/

/*--------------------------------------------
| Name:        _dev_at91m55800a_uart_s2_fifo_pool_init
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
static int dev_at91m55800a_uart_s2_fifo_pool_init(void){
   int i;
   uchar8_t* p = _at91m55800a_uart_fifo_input_buffer;
   uchar8_t c;

   //
   for(i=0;i<MAX_POOL;i++){
      _fifo_buf_pool[i].cb=0;
      _fifo_buf_pool[i].p=p;
      p+=(MAX_POOL_BUF_SZ);
   }
   //
   buf_in_dma_no=0;
   buf_in_rcv_no=0;
   //flush input register;
   c= __US_RHR2;
   //
   __US_RTOR2 = (1 * 10 * 4) ;
   __US_CR2 = US_STTTO ;
   __US_RCR2 = MAX_POOL_BUF_SZ ;
   __US_RPR2 = (unsigned long)_fifo_buf_pool[buf_in_dma_no].p ;
   return 0;
}

/*--------------------------------------------
| Name:        dev_at91m55800a_uart_s2_fifo_pool_rcv
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
void dev_at91m55800a_uart_s2_fifo_pool_rcv(void){
   int sz=0;
   int prev_buf_in_dma_no=buf_in_dma_no;

   //stop dma
   __US_CR2 = US_STTTO ;
   __US_RCR2=0;
   //get size
   sz= (__US_RPR2)-((unsigned long)(_fifo_buf_pool[buf_in_dma_no].p));
   //next dma buffer
   buf_in_dma_no=((buf_in_dma_no+1)&(~MAX_POOL));
   //
   if(buf_in_dma_no==buf_in_rcv_no){//error overrun
      _fifo_buf_pool[buf_in_dma_no].p=&_at91m55800a_uart_fifo_input_buffer[buf_in_dma_no*MAX_POOL_BUF_SZ];
      _fifo_buf_pool[buf_in_dma_no].cb=0;
   }
    //prepare dma operation
   __US_RPR2 = (unsigned long)(_fifo_buf_pool[buf_in_dma_no].p);
   //restart dma
   __US_RCR2 = (MAX_POOL_BUF_SZ);
   inter_char_timer=ttys_termios.c_cc[VTIME];
   //
   if(!sz)
      return;
   //
   _fifo_buf_pool[prev_buf_in_dma_no].cb=sz;
   //
   if((_at91m55800a_uart_s2_desc_rd>=0) && (_at91m55800a_uart_s2_input_r==_at91m55800a_uart_s2_input_w)){//empty to not empty
      //profiler
      __io_profiler_start(_at91m55800a_uart_s2_desc_rd);
      //
      __fire_io_int(ofile_lst[_at91m55800a_uart_s2_desc_rd].owner_pthread_ptr_read);
   }
   //
   _at91m55800a_uart_s2_input_w=((_at91m55800a_uart_s2_input_w+sz)&(~UART_FIFO_INPUT_BUFFER_SZ));

   return;
}

/*-------------------------------------------
| Name:dev_at91m55800a_uart_s2_snd
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void dev_at91m55800a_uart_s2_snd(void){

   unsigned char snd_data;
   
   if(_at91m55800a_uart_s2_output_r<0)
      return;

   if(_xonoff_option && _rcv_xonoff_status == STATUS_RCV_XOFF)
      return;

   _at91m55800a_uart_s2_output_r++;
   if(_at91m55800a_uart_s2_output_r<_at91m55800a_uart_s2_output_w){
      snd_data = _at91m55800a_uart_s2_output_buffer[_at91m55800a_uart_s2_output_r];
      __US_THR2 = snd_data;
   }else if(_at91m55800a_uart_s2_desc_wr>=0 && _at91m55800a_uart_s2_output_r==_at91m55800a_uart_s2_output_w){
      while (!(__US_CSR2 & (0x00000200)));    // Wait until TX shift register empty
      __fire_io_int(ofile_lst[_at91m55800a_uart_s2_desc_wr].owner_pthread_ptr_write);
      __US_IDR2=2;
      XMIT=0;
   }

}

/*-------------------------------------------
| Name:dev_win32_com1_timer_callback
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static void  dev_at91m55800a_uart_s2_timer_callback(void){
   if( (_at91m55800a_uart_s2_desc_rd>=0) 
      && (ttys_termios.c_cc[VTIME]) 
      && inter_char_timer
      && !(--inter_char_timer)){

      if(_at91m55800a_uart_s2_input_w==_at91m55800a_uart_s2_input_r){
         __fire_io_int(ofile_lst[_at91m55800a_uart_s2_desc_rd].owner_pthread_ptr_read);
      }

   }

   OS_RetriggerTimer(&dev_at91m55800a_uart_s2_timer);
}

/*--------------------------------------------
| Name:        dev_at91m55800a_uart_s2_interrupt
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
void dev_at91m55800a_uart_s2_interrupt(void)
{
  __AIC_IVR = 0; // Debug variant of vector read, protected mode is used.
   if(dev_at91m55800a_uart_s2_loaded){

      unsigned int _usart_csr;
      do {
         _usart_csr = __US_CSR2;   

         if ((_usart_csr&0x00000020)==0x00000020){
            __US_CR2 = 0x00000100; // Reset status bits, reset rx/tx.
         }

         if ((_usart_csr&0x00000108)){//receive fifo
            dev_at91m55800a_uart_s2_fifo_pool_rcv();
         }
         
         if (((_usart_csr&2)==0x02)&&(XMIT)){ //Transmission
            dev_at91m55800a_uart_s2_snd();
         }

      } while ( (__US_CSR2 & __US_IMR2 & (0x00000108)) || ( (XMIT) && (__US_CSR2 & __US_IMR2 & (0x00000002)) ) ); 

   }

  __AIC_EOICR = 0; // Signal end of interrupt to AIC.
}


/*-------------------------------------------
| Name:dev_at91m55800a_uart_s2_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91m55800a_uart_s2_load(void){

   //already loaded?
   if(dev_at91m55800a_uart_s2_loaded)
      return 0;//yes
   
   _at91m55800a_uart_s2_desc_rd = -1;
   _at91m55800a_uart_s2_desc_wr = -1;

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
   inter_char_timer = 0;

   #if defined (USE_SEGGER)
   //VTIME timer in units of 0.1 seconds (posix specification).
   OS_CreateTimer(&dev_at91m55800a_uart_s2_timer,dev_at91m55800a_uart_s2_timer_callback,100);  // 100ms
   #endif
   
   dev_at91m55800a_uart_s2_loaded=1;

   return 0;
}

/*-------------------------------------------
| Name:dev_at91m55800a_uart_s2_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91m55800a_uart_s2_open(desc_t desc, int o_flag){

   //
   if(_at91m55800a_uart_s2_desc_rd<0 && _at91m55800a_uart_s2_desc_wr<0){
      __US_IDR2 = 0xffffffff; // Disable all USART interrupts.
      // Enable peripheral clock for selected USART 
      __APMC_PCER = 1 << US2IRQ;
      // Don't use the serial port when run in Angel mode.
      __PIO_PDR   = 1<<21; // Disable PIO control of PA21/TXD1 and PA22/RXD1.
      __PIO_PDR   = 1<<22;

      __US_MR2 = 0x000008c0; // Normal mode, 1 stop bit, no parity, async mode, 8 bits, MCK.
      __US_IDR2 = 0xffffffff; // Disable all USART interrupts.
      __US_TTGR2 = 5; // Transmit time guard in number of bit periods.
      __US_BRGR2 = __KERNEL_CPU_FREQ / 9600 / 16; // Set baud rate.

      __AIC_ICCR_bit.us2irq = 1; // Clears timer/counter 1 interrupt.
      __AIC_IECR_bit.us2irq = 1; // Enable timer/counter 1 interrupt.

      __US_CR2 = 0x000000a0; // Disable receiver, disable transmitter.
      __US_CR2 = 0x0000010c; // Reset status bits, reset rx/tx.
      __US_CR2 = 0x00000050; // Enable receiver, enable transmitter.

      __AIC_ICCR = 1 << US2IRQ; // Clears usart 1 interrupt.
   
      // Usart 1 interrupt vector.
      __AIC_SVR4 = (unsigned long)&dev_at91m55800a_uart_s2_interrupt; // Usart 1 interrupt vector.
      // SRCTYPE=3, PRIOR=3. USART 1 interrupt positive edge-triggered at prio 3.
      __AIC_SMR4 = 0x63; 
   }
  
   //
   if(o_flag & O_RDONLY){
      char _rcv_buf;
      if(_at91m55800a_uart_s2_desc_rd>=0) //already open: exclusive resource.
         return -1;
      _at91m55800a_uart_s2_input_r = 0;
      _at91m55800a_uart_s2_input_w = 0;
      _at91m55800a_uart_s2_desc_rd = desc;
      //pool multi-buffering
      dev_at91m55800a_uart_s2_fifo_pool_init();
      //
      ttys_termios.c_cc[VTIME]=0;// no timeout, blocking call
      inter_char_timer = 0;
      // Interrupt on RXRDY
      __US_IER2 = IER_MASK_RCV; 
   }

   //
   if(o_flag & O_WRONLY){
      if(_at91m55800a_uart_s2_desc_wr>=0) //already open: exclusive resource.
         return -1;
      _at91m55800a_uart_s2_output_r = -1;
      _at91m55800a_uart_s2_output_w = 0;
      _at91m55800a_uart_s2_desc_wr = desc;
      // Interrupt on TXRDY
      __US_IER2 = IER_MASK_SND; 
   }

   return 0;
}

/*-------------------------------------------
| Name:dev_at91m55800a_uart_s2_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91m55800a_uart_s2_close(desc_t desc){

   if(ofile_lst[desc].oflag & O_RDONLY){
      //
      if(!ofile_lst[desc].nb_reader){
         // Disable Interrupt on RXRDY
         __US_IDR2 = IER_MASK_RCV; 
         //stop inter char timer
         OS_StopTimer(&dev_at91m55800a_uart_s2_timer);
         //
         ttys_termios.c_cc[VTIME]=0;// no timeout, blocking call
         inter_char_timer = 0;
         //
         _at91m55800a_uart_s2_desc_rd = -1;
      }
   }
   //
   if(ofile_lst[desc].oflag & O_WRONLY){
      if(!ofile_lst[desc].nb_writer){
         // Disable Interrupt on TXRDY
         __US_IDR2 = IER_MASK_SND; 
         //
         _at91m55800a_uart_s2_desc_wr = -1;
      }
   }

   //close all
   if(_at91m55800a_uart_s2_desc_wr<0 && _at91m55800a_uart_s2_desc_rd<0){
      __US_IDR2 = 0xffffffff; // Disable all USART interrupts.
      //
      __AIC_ICCR_bit.us2irq = 1; // Clears timer/counter 1 interrupt.
      __AIC_IECR_bit.us2irq = 1; // Enable timer/counter 1 interrupt.

      __US_CR2 = 0x000000a0; // Disable receiver, disable transmitter.
      __US_CR2 = 0x0000010c; // Reset status bits, reset rx/tx.
      // disable peripheral clock for selected USART 
      __APMC_PCDR = 1 << US2IRQ;
   }
   
   return 0;
}

/*-------------------------------------------
| Name:dev_at91m55800a_uart_s2_isset_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91m55800a_uart_s2_isset_read(desc_t desc){
   if((_at91m55800a_uart_s2_input_w!=_at91m55800a_uart_s2_input_r) 
      || (ttys_termios.c_cc[VTIME] && !inter_char_timer) ){
      inter_char_timer=ttys_termios.c_cc[VTIME];
      return 0;
   }else{
      return -1;
   }
}

/*-------------------------------------------
| Name:dev_at91m55800a_uart_s2_isset_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91m55800a_uart_s2_isset_write(desc_t desc){
   if(_at91m55800a_uart_s2_output_w==_at91m55800a_uart_s2_output_r){
      _at91m55800a_uart_s2_output_r=-1;
      return 0;
   }
   else 
      return -1;
}

/*-------------------------------------------
| Name:dev_at91m55800a_uart_s2_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91m55800a_uart_s2_read(desc_t desc, char* buf,int size){
   int r   = _at91m55800a_uart_s2_input_r;
   int _buf_in_rcv_no=buf_in_rcv_no;
   int _buf_in_dma_no=-1;
   
   int cb=0;

   //
   OS_DI();
   _buf_in_dma_no=buf_in_dma_no;
   OS_EI();

   //
   do{
      int w=0;
      if(!_fifo_buf_pool[_buf_in_rcv_no].cb || cb==size)
         break;
      w=(size<(cb+_fifo_buf_pool[_buf_in_rcv_no].cb)?(size-cb):_fifo_buf_pool[_buf_in_rcv_no].cb);

      memcpy(buf+cb,_fifo_buf_pool[_buf_in_rcv_no].p,w);
      cb+=w;
      _fifo_buf_pool[_buf_in_rcv_no].cb-=w;
      _fifo_buf_pool[_buf_in_rcv_no].p+=w;

      if(_fifo_buf_pool[_buf_in_rcv_no].cb<=0){
         _fifo_buf_pool[_buf_in_rcv_no].p=&_at91m55800a_uart_fifo_input_buffer[_buf_in_rcv_no*MAX_POOL_BUF_SZ];
         _buf_in_rcv_no=((_buf_in_rcv_no+1)&(~MAX_POOL));
      }

   }while( _buf_in_rcv_no!=_buf_in_dma_no);

   //
   OS_DI();
   _at91m55800a_uart_s2_input_r=((_at91m55800a_uart_s2_input_r+cb)&(~UART_FIFO_INPUT_BUFFER_SZ));
   buf_in_rcv_no=_buf_in_rcv_no;
   OS_EI();
   return cb;

}

/*-------------------------------------------
| Name:dev_at91m55800a_uart_s2_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91m55800a_uart_s2_write(desc_t desc, const char* buf,int size){

   unsigned char snd_data;
  
   if(size>=UART_S2_OUTPUT_BUFFER_SIZE)
      size = UART_S2_OUTPUT_BUFFER_SIZE-1;
      

   memcpy(_at91m55800a_uart_s2_output_buffer,buf,size);
   //buffer ready
   snd_data = _at91m55800a_uart_s2_output_buffer[0];

   while (!(__US_CSR2 & (0x00000002)));    // Wait until TB empty
   //protection xon/xoff
   //OS_DI()
   _at91m55800a_uart_s2_output_r = 0;
   _at91m55800a_uart_s2_output_w = size;
   //U0TBL    = snd_data;
   OS_DI();
   XMIT=1;
   __US_THR2 = snd_data;
   __US_IER2 = 2;
   OS_EI();
   
   //end protection xon/xoff
   //OS_EI()

   return size;
}

/*-------------------------------------------
| Name:dev_at91m55800a_uart_s2_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91m55800a_uart_s2_seek(desc_t desc,int offset,int origin){
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
static int termios2ttys(struct termios* termios_p){
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
         __US_BRGR2 = (__KERNEL_CPU_FREQ / n_speed / 16); // Set baud rate.
         break;
      }
   }

   //timeout read in blocking call
   if(termios_p->c_cc[VTIME]){
      OS_StopTimer(&dev_at91m55800a_uart_s2_timer);
      inter_char_timer = termios_p->c_cc[VTIME];
      OS_RetriggerTimer(&dev_at91m55800a_uart_s2_timer);
   }else{
      OS_StopTimer(&dev_at91m55800a_uart_s2_timer);
   }
   
   //
   memcpy(&ttys_termios,termios_p,sizeof(struct termios));
   return 0;
}

/*-------------------------------------------
| Name:dev_at91m55800a_uart_s2_ioctl
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91m55800a_uart_s2_ioctl(desc_t desc,int request,va_list ap){

   struct termios* termios_p = (struct termios*)0;
  
   switch(request){
      //
      case TIOCSSERIAL:{
         unsigned long speed  = va_arg( ap, unsigned long);
         if(speed<0)
            return -1;
         __US_BRGR2 = (__KERNEL_CPU_FREQ / speed / 16); // Set baud rate.
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


      case TCFLSH:{
         int flush_io= va_arg( ap, int);
         if(flush_io>2)
            return -1;
         //TCIFLUSH 0 -> 1:01
         //TCIFLUSH 1 -> 2:10
         //TCOFLUSH 2 -> 3:11
         flush_io+=1; 
         //
         if( (flush_io&(TCIFLUSH+1)) ){
            if( (ofile_lst[desc].oflag & O_RDONLY) )
               return -1;//not compatible open mode
            _at91m55800a_uart_s2_input_r = 0;
            _at91m55800a_uart_s2_input_w = 0;
         }
         if( (flush_io&(TCOFLUSH+1)) ){
            if( (ofile_lst[desc].oflag & O_WRONLY) )
               return -1;//not compatible open mode
            _at91m55800a_uart_s2_output_r = 0;
            _at91m55800a_uart_s2_output_w = 0;
         }
      }
      break;

      //
      default:
         return -1;

   }

   return 0;
}

/** @} */
/** @} */
/** @} */

/*============================================
| End of Source  : dev_at91m55800a_uart_s2.c
==============================================*/
