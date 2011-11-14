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
 * \addtogroup hard_dev_arm7se 
 * @{
 *
 */

/**
 * \defgroup dev_spi_c pilote spi.
 * @{
 *
 * pilotes du périphérique spi.
 *    
 */


/**
 * \file
 * implementation du pilote spi.
 * \author bruno mollo
 */

/*===========================================
Includes
=============================================*/
#include "kernel/interrupt.h"
#include "kernel/kernelconf.h"
#include "kernel/kernel.h"
#include "kernel/system.h"
#include "kernel/fcntl.h"
#include "kernel/cpu.h"

#include "termios/termios.h"
#include "vfs/vfsdev.h"

#include <ioat91sam7se512.h>
#include <intrinsic.h>

#include "ctype/ctype.h"
#include "kernel/types.h"
#include "kernel/signal.h"
#include "kernel/libstd.h"
#include "kernel/stat.h"
#include "kernel/time.h"
#include "kernel/devio.h"
#include "kernel/errno.h"

#include "dev/arch/all/sdcard/drv_sdcard_arch.h"

/*===========================================
Global Declaration
=============================================*/
const char dev_at91sam7s_spi_name[]="spi0\0";

int dev_at91sam7s_spi_load        (void);
int dev_at91sam7s_spi_open        (desc_t, int);
int dev_at91sam7s_spi_close       (desc_t);
int dev_at91sam7s_spi_read        (desc_t, char *, int);
int dev_at91sam7s_spi_write       (desc_t, const char *, int);
int dev_at91sam7s_spi_seek        (desc_t, int, int);

dev_map_t dev_at91sam7s_spi_map={
   dev_at91sam7s_spi_name,
   S_IFBLK,
   dev_at91sam7s_spi_load,
   dev_at91sam7s_spi_open,
   dev_at91sam7s_spi_close,
   __fdev_not_implemented,
   __fdev_not_implemented,
   dev_at91sam7s_spi_read,
   dev_at91sam7s_spi_write,
   dev_at91sam7s_spi_seek,
   __fdev_not_implemented //ioctl
};

#define  SDCARD_BLOCK_SIZE      64

#define  READ_OPERATION         0
#define  WRITE_OPERATION        1
#define  FALSE                  0
#define  TRUE                   1
#define  TIMEOUT_OK             1
#define  TIMEOUT_KO             0
#define  WRITE_DMA              0
#define  READ_DMA               1

#define  PCS_SPI_MR             0x00
// Time between caract in DMA
// #define  DLYBCT_SPI_CSR0        12     

//#define DMA_SPI_INT_RAM _Pragma ("location=\"INTRAMSTART_REMAP\"")

#define  USE_BYTE_PER_BYTE_TRANSMISSION
//#define  USE_DMA_TRANSMISSION          
//#define  USE_DMA_INTERUPT_TRANSMISSION

//#if defined (USE_SEGGER)
OS_TIMER dev_at91sam7s_spi_timer;
//#endif

//termios
static struct termios  ttys_termios;
static int             dev_at91sam7s_spi_loaded = 0;

static volatile desc_t spi_desc_rd = -1;   //O_RDONLY
static volatile desc_t spi_desc_wr = -1;   //O_WRONLY

static volatile short      spi_timeout;
static void                dev_at91sam7s_spi_timer_callback(void);

static char dummy_ff_block[SDCARD_BLOCK_SIZE];
//#pragma location="MY_SEG_INTRAM"
//__no_init static char dummy_ff_block[SDCARD_BLOCK_SIZE];

static char buf_recv[SDCARD_BLOCK_SIZE];
//#pragma location="MY_SEG_INTRAM"
//__no_init static char buf_recv[SDCARD_BLOCK_SIZE];

static int dma_size_buff;
//#pragma location="MY_SEG_INTRAM"
//__no_init static int dma_size_buff;


static unsigned char s_spi_io_init=0;

static kernel_pthread_mutex_t s_spi_mutex;
//pthread_mutex_t s_spi_mutex;

#define __dev_spi_lock()      kernel_pthread_mutex_lock  (&s_spi_mutex)
#define __dev_spi_unlock()    kernel_pthread_mutex_unlock(&s_spi_mutex)

//#define __dev_spi_lock()      pthread_mutex_lock  (&s_i2c_mutex)
//#define __dev_spi_unlock()    pthread_mutex_unlock(&s_i2c_mutex)

/*============================================
| Implementation 
==============================================*/

/*-------------------------------------------
| Name       : dev_at91sam7s_uart_sx_snd
| Description: Send Data
| Parameters : - 
| Return Type: None
| Comments   : -
| See        : -
---------------------------------------------*/
void dev_at91sam7s_spi_snd ()
{

}
                                   
/*-------------------------------------------
| Name       : dev_at91sam7s_uart_sx_rcv
| Description: Receive Data
| Parameters : - 
| Return Type: None
| Comments   : -
| See        : -
---------------------------------------------*/
void dev_at91sam7s_spi_rcv ()
{       

}                                   
                                   
                                   
/*--------------------------------------------
| Name       : dev_at91sam7s_spi_interrupt
| Description: Generic Interrupt function for
|              reading/writing bytes
| Parameters : descriptor  (desc_t)
| Return Type: none
| Comments   : -   
| See        : -        
----------------------------------------------*/
void dev_at91sam7s_spi_interrupt(void)
{
   *AT91C_AIC_EOICR = 0; // Signal end of interrupt to AIC.
}


/*-------------------------------------------
| Name:dev_win32_com1_timer_callback
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static void  dev_at91sam7s_spi_timer_callback(void)
{
  spi_timeout = TIMEOUT_KO;
}

/*-------------------------------------------
| Name       :dev_at91sam7s_spi_load
| Description:
| Parameters :
| Return Type:
| Comments   :
| See        :
---------------------------------------------*/
int dev_at91sam7s_spi_load (void)
{
    int cpt;
    
    pthread_mutexattr_t  mutex_attr=0;

    if(s_spi_io_init)
     return 0;

    s_spi_io_init++;

    kernel_pthread_mutex_init(&s_spi_mutex, &mutex_attr);
//pthread_mutex_init(&s_spi_mutex, &mutex_attr);
    
   //already loaded?
   if (dev_at91sam7s_spi_loaded)
      return 0;//yes
   
   // initialization
   for (cpt=0; cpt < SDCARD_BLOCK_SIZE; cpt++)
   {
     dummy_ff_block[cpt]=0xff;
   }
   
   // to remove...
   cfmakeraw   (&ttys_termios);
   cfsetispeed (&ttys_termios, B9600);
   cfsetospeed (&ttys_termios, B9600);
   
   dev_at91sam7s_spi_loaded = 1; // loaded
  
   OS_CreateTimer (&dev_at91sam7s_spi_timer,
                   dev_at91sam7s_spi_timer_callback,
                   10);  // delay * ms   
   return 0;
}

/*-------------------------------------------
| Name       :dev_at91sam7s_spi_open
| Description:
| Parameters :
| Return Type:
| Comments   :
| See        :
---------------------------------------------*/
int dev_at91sam7s_spi_open(desc_t desc, int o_flag)
{
   // Enable peripheral clock for selected USART 
   *AT91C_PMC_PCER   = 1 << AT91C_ID_SPI;

   // Enable peripheral clock for selected PIOA/B
   *AT91C_PMC_PCER   = 1 << AT91C_ID_PIOA;//GPIO clock enable
   *AT91C_PMC_PCER   = 1 << AT91C_ID_PIOB;//GPIO clock enable
   
   // Enable Write Enable and SD card presence
   *AT91C_PIOB_PER   = (1<<21);   // Write Enable on PB21
   *AT91C_PIOB_MDDR  = (1<<21);   // Write Enable disable open drain
   *AT91C_PIOB_PER   = (1<<22);   // SD Card presence on PB22
   *AT91C_PIOB_MDDR  = (1<<22);   // SD Card disable open drain      
   *AT91C_PIOB_ODR   = (1<<21);  //  Input only
   *AT91C_PIOB_ODR   = (1<<22);  //  Input only
      
   // Enable Chip Select line /GPIO on NPCS0 PA11
   *AT91C_PIOA_PER   = (1<<11);   // Write Enable on PA111
   *AT91C_PIOA_MDDR  = (1<<11);   // Write Enable disable open drain
   *AT91C_PIOA_OER   = (1<<11);   // Enable Output on the I/O line
   *AT91C_PIOA_OWER  = (1<<11);	  // Port A all ODSR enabled    
   *AT91C_PIOA_SODR  = (1<<11);   //=1 => set CS
   
   // Setup PIO pins for SPI interface
   // Disable PIO control of PA11/NPCS0, PA12/MISO, PA13/MOSI, PA14/SPCK
   *AT91C_PIOA_PDR = ( (1<<12)|
                       (1<<13)|
                       (1<<14) );
 
   // Enable I/O peripheral mode A
   *AT91C_PIOA_ASR = ( (1<<12)|
                       (1<<13)|
                       (1<<14) );
   
   *AT91C_SPI_CR  = AT91C_SPI_SWRST; // Software reset          
   *AT91C_SPI_CR  = AT91C_SPI_SPIEN; // enable the SPI to transfer and receive data
   *AT91C_SPI_IDR = 0xffffffff;      // Disable all SPI interrupts.
   
   // Set master mode with:
   *AT91C_SPI_MR  =   AT91C_SPI_MSTR     |  // SPI MASTER
                      AT91C_SPI_PS_FIXED |  // Fixed peripheral Select
                      AT91C_SPI_MODFDIS  |  // No mode fault
                      (PCS_SPI_MR << 16);                       
      
   // Setup data transfer format and rate for device 0 => 8 bits, CPOL=0, NCPHA=1
   *AT91C_SPI_CSR  =  //AT91C_SPI_CPOL |
                      AT91C_SPI_NCPHA    |  
                      AT91C_SPI_BITS_8   |
                      AT91C_SPI_DLYBS  | // Maximum 0xff
#ifdef USE_DMA_TRANSMISSION
                     (DLYBCT_SPI_CSR0 << 24) |
#endif                        
                      (SPI_SCKDIV_ARM7 << 8);
                           
   *AT91C_AIC_ICCR = (1 << AT91C_ID_SPI); // Clears spi interrupt.
   // *AT91C_AIC_IECR = (1 << AT91C_ID_TWI); // Enable twi interrupt.
       
   // twi interrupt vector.
   // AT91C_AIC_SVR[AT91C_ID_SPI] = (unsigned long)&dev_at91sam7s_spi_interrupt; 

   // SRCTYPE=3, PRIOR=3. TWI 0 interrupt positive edge-triggered at prio 3.
   // AT91C_AIC_SMR[AT91C_ID_SPI] = 0x63; 
#ifdef USE_DMA_TRANSMISSION
   // disable DMA
  *AT91C_SPI_PTCR = AT91C_PDC_TXTDIS; 
  *AT91C_SPI_PTCR = AT91C_PDC_RXTDIS;
    // enable DMA transfer
  *AT91C_SPI_PTCR = AT91C_PDC_RXTEN;
  *AT91C_SPI_PTCR = AT91C_PDC_TXTEN;   // enable DMA transfer TX
#endif                      
   //
   if(o_flag & O_RDONLY)
   {
      // initializations 
      spi_desc_rd = desc;   //O_RDONLY         
   }

   //
   if(o_flag & O_WRONLY)
   {
      spi_desc_wr = desc;   //O_WRONLY
   }
      
   return 0;
}
 
/*-------------------------------------------
| Name:dev_at91sam7s_spi_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91sam7s_spi_close(desc_t desc){
   return 0;
}

/*-------------------------------------------
| Name:dev_at91sam7s_spi_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91sam7s_spi_seek(desc_t desc,int offset,int origin){
   return -1;
}


/*-------------------------------------------
| Name:at91_poll_status
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static short at91_spi_poll_status (unsigned long bit, unsigned int delay) 
{
  spi_timeout = TIMEOUT_OK;

  OS_SetTimerPeriod (&dev_at91sam7s_spi_timer, delay);  
  OS_StartTimer     (&dev_at91sam7s_spi_timer);
  
  while (!(*AT91C_SPI_SR & bit) && (spi_timeout != TIMEOUT_KO ));
    
  OS_StopTimer(&dev_at91sam7s_spi_timer);
  
  return (spi_timeout);
}


static void dev_at91sam7s_spi_dma_read_write (int         oper_type, 
                                              const char *buf, 
                                              int         size)
{  
  unsigned char c;
  
 do 
 {
    c = *AT91C_SPI_RDR;
 } while ((*AT91C_SPI_SR & AT91C_SPI_RDRF));
    
  // READ Or WRITE Operation
  switch (oper_type)
  {
    case WRITE_DMA:
        *AT91C_SPI_TPR  = (unsigned long)buf;
        *AT91C_SPI_RPR  = (unsigned long)buf_recv; // trash beware
        break;
    case READ_DMA:
      *AT91C_SPI_RPR = (unsigned long)buf;
      *AT91C_SPI_TPR = (unsigned long)dummy_ff_block; // 0xFF array
        break;
  }    
  
  *AT91C_SPI_TCR = size; // Common sizes
  *AT91C_SPI_RCR = size;
 
  // enable DMA transfer
  *AT91C_SPI_PTCR = AT91C_PDC_TXTEN;   // enable DMA transfer TX
  *AT91C_SPI_PTCR = AT91C_PDC_RXTEN;
   
   while ( !(*AT91C_SPI_SR & AT91C_SPI_RXBUFF) ); 
   while ( !(*AT91C_SPI_SR & AT91C_SPI_TXBUFE));
 
//while (!(*AT91C_SPI_SR & AT91C_SPI_ENDRX) );
//while (!(*AT91C_SPI_SR & AT91C_SPI_ENDTX));

   //c = *AT91C_SPI_RDR;
   
  // disable DMA
  *AT91C_SPI_PTCR = AT91C_PDC_RXTDIS;
  *AT91C_SPI_PTCR = AT91C_PDC_TXTDIS;    
}

/*-------------------------------------------
| Name:dev_at91sam7s_spi_read
| Description:
| Parameters:
| Return Type:
| Comments: [0] i2c io addr, [1] offset size
|           [2] offset MSB, [3] offset LSB
| See:
---------------------------------------------*/
int dev_at91sam7s_spi_read(desc_t desc, char* buf, int size)
{
  int           cpt,i=0;
  unsigned char c;
  // test if size > max (ACU) and zero
  
  // if (size ==0) return (-1);
  
  __dev_spi_lock();
 

#ifdef USE_DMA_TRANSMISSION  
  // enable DMA transfer
   *AT91C_SPI_PTCR = AT91C_PDC_RXTEN;
   *AT91C_SPI_PTCR = AT91C_PDC_TXTEN;   // enable DMA transfer TX
 
   // initialisation
   // clear register courant
  *AT91C_SPI_RPR = (unsigned int)0;
  *AT91C_SPI_TPR = (unsigned int)0; // 0xFF array 
  *AT91C_SPI_TCR = 0; // Common sizes
  *AT91C_SPI_RCR = 0;
   
  // clear register next
   *AT91C_SPI_RNPR = (unsigned int)0;
   *AT91C_SPI_TNPR = (unsigned int)0; // 0xFF array 
   *AT91C_SPI_TNCR = 0; // Common sizes
   *AT91C_SPI_RNCR = 0;           
   // for (i=0;i < size;i++) buf_recv[i]= 0x00; // initialization
 
   // wait for receive ready  
   do 
   {
     c = *AT91C_SPI_RDR;
   } while ((*AT91C_SPI_SR & AT91C_SPI_RDRF));
 //  
 while (!(*AT91C_SPI_SR & AT91C_SPI_TDRE)); // + timeout ACU  
 
  // courant
  *AT91C_SPI_RPR = (unsigned int)buf;
  *AT91C_SPI_TPR = (unsigned int)dummy_ff_block; // 0xFF array
  *AT91C_SPI_RCR = size;
  *AT91C_SPI_TCR = size; // Common sizes
  
  // Next pointer
//  *AT91C_SPI_RNPR = (unsigned int)buf_recv;
//  *AT91C_SPI_TNPR = (unsigned int)dummy_ff_block; // 0xFF array  
//  *AT91C_SPI_TNCR = dma_size_buff; // Common sizes
//  *AT91C_SPI_RNCR = dma_size_buff;
  
//while (!(*AT91C_SPI_SR & AT91C_SPI_ENDTX));
//while ( !(*AT91C_SPI_SR & AT91C_SPI_TXBUFE)); 
//while ((*AT91C_SPI_TNCR) || (*AT91C_SPI_TCR));
   while (*AT91C_SPI_TNCR);
       
  //while (*AT91C_SPI_TCR);
  // *AT91C_SPI_PTCR = AT91C_PDC_TXTDIS; 
   
//  if ((*AT91C_SPI_SR & AT91C_SPI_OVRES))
//  {
//    return (-1);
//  }
    
  //while (!(*AT91C_SPI_SR & AT91C_SPI_ENDRX) );
  //while ( !(*AT91C_SPI_SR & AT91C_SPI_RXBUFF) );  
  //while (*AT91C_SPI_RCR);
 // while ((*AT91C_SPI_RNCR) || (*AT91C_SPI_TCR));
 
  while (*AT91C_SPI_RNCR);
   
//  *AT91C_SPI_RPR = (unsigned int)0;
//  *AT91C_SPI_TPR = (unsigned int)0; // 0xFF array 
//  *AT91C_SPI_TCR = 0; // Common sizes
//  *AT91C_SPI_RCR = 0;
     
//  memcpy (buf,(const *)&buf_recv[0],dma_size_buff);
     
  *AT91C_SPI_PTCR = AT91C_PDC_TXTDIS;
  *AT91C_SPI_PTCR = AT91C_PDC_RXTDIS;
  
   //dev_at91sam7s_spi_dma_read_write(READ_DMA, buf, size);                        
#endif

  
#ifdef USE_BYTE_PER_BYTE_TRANSMISSION   
    // wait for receive ready      
    do 
    {
       c = *AT91C_SPI_RDR;
    } while ((*AT91C_SPI_SR & AT91C_SPI_RDRF));
             
   // Receive the data block into buffer (byte per byte)
   for (cpt=0; cpt < size; cpt++)
   {	      
      while (!(*AT91C_SPI_SR & AT91C_SPI_TDRE)); // + timeout ACU
      *AT91C_SPI_TDR = 0xff;

      // wait for completion
      while (!(*AT91C_SPI_SR & AT91C_SPI_RDRF)); // + timeout ACU     
      buf[cpt] = *AT91C_SPI_RDR;
   }   
#endif
  
   __dev_spi_unlock();
   
    return size;
}


/*-------------------------------------------
| Name       :dev_at91sam7s_spi_write
| Description:
| Parameters : desc : descriptor
|              buf  : buffer (data + header)
|              size : dada size + header size              
|
| Return Type:
| Comments   : -
| See        : -
---------------------------------------------*/
int dev_at91sam7s_spi_write (desc_t desc, const char* buf, int size)
{   
  int cpt;
  unsigned char c;
  // verify maximum size of data
  
  __dev_spi_lock();
  
#ifdef USE_DMA_TRANSMISSION   
     while (!(*AT91C_SPI_SR & AT91C_SPI_TDRE)); // + timeout ACU    
    dev_at91sam7s_spi_dma_read_write(WRITE_DMA, buf, size);
   
#endif
   
#ifdef USE_BYTE_PER_BYTE_TRANSMISSION 
   // Write data to be transmit
   for (cpt=0; cpt <size; cpt++)
   {     
      // wait for transmit completion/ready
     while (!(*AT91C_SPI_SR & AT91C_SPI_TDRE)); // + timeout ACU     
     *AT91C_SPI_TDR = buf[cpt];

     // wait for completion
     while (!(*AT91C_SPI_SR & AT91C_SPI_RDRF)); // + timeout ACU      
     c=*AT91C_SPI_RDR;     //  read RDR here
   }  
#endif
   
  __dev_spi_unlock();
   
  return (size);
}

/** @} */
/** @} */
/** @} */
/*===========================================
End of Source dev_at91sam7s_spi.c
=============================================*/
