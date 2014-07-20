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

/**
 * \addtogroup lepton_dev
 * @{
 *
 */

/**
 * \addtogroup hard_dev_arm9261
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
#include "kernel/core/interrupt.h"
#include "kernel/core/kernelconf.h"
#include "kernel/core/kernel.h"
#include "kernel/core/system.h"
#include "kernel/core/fcntl.h"
#include "kernel/core/cpu.h"
#include "lib/libc/termios/termios.h"
#include "kernel/fs/vfs/vfsdev.h"
#include "kernel/core/core_rttimer.h"
#include "kernel/dev/arch/arm9/at91sam9261/common/dev_at91sam9261_common.h"
#include "kernel/dev/arch/all/sdcard/drv_sdcard_arch.h"

#if defined(__IAR_SYSTEMS_ICC) || defined(__IAR_SYSTEMS_ICC__)
   #include <atmel/ioat91sam9261.h>
#else
   #include "cyg/hal/at91sam9261.h"
#endif

#if defined(__KERNEL_UCORE_ECOS)
   #include "pkgconf/hal_arm_at91sam9261.h"
#endif

// Profiling SPI (see test associated)
// #define      PROFILING_SPI


// ATTENTION: Les Buffers de lecture et d'écriture DMA ne transitent plus par la Pile
volatile unsigned char g_BufferRead[512]   __attribute__ ((section (".no_cache")));
volatile unsigned char g_Bufferwrite[512]  __attribute__ ((section (".no_cache")));
//

/*===========================================
Global Declaration
=============================================*/
const char dev_at91sam9261_spi_name[]="spi0\0";

int dev_at91sam9261_spi_load        (void);
int dev_at91sam9261_spi_open        (desc_t, int);
int dev_at91sam9261_spi_close       (desc_t);
int dev_at91sam9261_spi_read        (desc_t, char *, int);
int dev_at91sam9261_spi_write       (desc_t, const char *, int);
int dev_at91sam9261_spi_seek        (desc_t, int, int);

dev_map_t dev_at91sam9261_spi_map={
   dev_at91sam9261_spi_name,
   S_IFBLK,
   dev_at91sam9261_spi_load,
   dev_at91sam9261_spi_open,
   dev_at91sam9261_spi_close,
   __fdev_not_implemented,
   __fdev_not_implemented,
   dev_at91sam9261_spi_read,
   dev_at91sam9261_spi_write,
   dev_at91sam9261_spi_seek,
   __fdev_not_implemented //ioctl
};

// delai entre deux transferts consecutifs
#define  DLYBCT_SPI_CSR0    0

// ---------------------------------------------------------------------------------------------
// Choix du mode de fonctionnement du driver
//#define  USE_BYTE_PER_BYTE_TRANSMISSION //transfert octet par octet
#define  USE_DMA_TRANSMISSION    //transfert avec DMA, attente active avec timer
//#define  USE_DMA_INTERUPT_TRANSMISSION  //transfert avec DMA, attente passive avec interruption
// ---------------------------------------------------------------------------------------------

//valeurs du flag de passage dans la routine de timeout du timer
#define  SPI_TIMEOUT_OK             1
#define  SPI_TIMEOUT_KO             0
//duree du timeout programmee pour l'envoi d'un caractere
#define  TIMEOUT_SPI                3 //ms

//#define CYGNUM_HAL_H_PRIOR 3 //priorite de l'interruption
#define CYGNUM_HAL_H_PRIOR 3 //priorite de l'interruption
static cyg_handle_t _at91sam9261_spi_handle;
static cyg_interrupt _at91sam9261_spi_it;

//flag indiquant que le driver spi a deja ete initialise
static int dev_at91sam9261_spi_loaded = 0;

//flag indiquant le passage dans la routine d'IT du timer
static volatile short spi_timeout;
// routine d'interruption du timer
static void           dev_at91sam9261_spi_timer_callback(void);
//identifiant du driver
tmr_t dev_at91sam9261_spi_timer;

//gestion du mutex pour eviter une reentrance du driver
static kernel_pthread_mutex_t s_spi_mutex;
#define __dev_spi_lock()      kernel_pthread_mutex_lock(&s_spi_mutex)
#define __dev_spi_unlock()    kernel_pthread_mutex_unlock(&s_spi_mutex)

//flag pour ignorer l'appel de l'IT a la creation du timer et lors d'un reload
volatile unsigned short ItDueToReload = true;

#ifdef USE_DMA_INTERUPT_TRANSMISSION
   #include "kernel/core/kernel_sem.h"
kernel_sem_t spi_sem_tx_end;
#endif

#define  SPI_SDCARD_BLOCK_SIZE          512
#define  LIMIT_SIZE_BYTE_PER_BYTE_FCT   16
//
volatile char dummy_block[SPI_SDCARD_BLOCK_SIZE];
volatile char s_buf_in[SPI_SDCARD_BLOCK_SIZE];

/*============================================
| Implementation
==============================================*/
/*-------------------------------------------
| Name: dev_at91sam9261_spi_dsr
| Description: traitement post isr
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
#ifdef USE_DMA_INTERUPT_TRANSMISSION
void dev_at91sam9261_spi_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
   kernel_sem_post(&spi_sem_tx_end);

   //autorise à nouveau les IT
   cyg_interrupt_unmask(vector);
}
#endif


/*--------------------------------------------
| Name       : dev_spi_handscopearm9_isr
| Description: Generic Interrupt function for
|              reading/writing bytes
| Parameters : descriptor  (desc_t)
| Return Type: none
| Comments   : -
| See        : -
----------------------------------------------*/
#ifdef USE_DMA_INTERUPT_TRANSMISSION
cyg_uint32 dev_at91sam9261_isr(cyg_vector_t vector, cyg_addrword_t data)
{
   volatile unsigned int spi_sr;

   cyg_interrupt_mask(vector);

   // le driver est il deja charge?
   if (dev_at91sam9261_spi_loaded)
   {
      //lecture du registre d'etat pour connaitre l'origine de l'IT
      spi_sr = *AT91C_SPI0_SR;

      //est ce une it de fin d'emission?
      if (spi_sr & AT91C_SPI_ENDTX)
      {
         // desactivation de l'emetteur et du récepteur
         *AT91C_SPI0_PTCR = AT91C_PDC_RXTDIS| AT91C_PDC_TXTDIS;
         //desactivation de l'it ENDTX
         *AT91C_SPI0_IDR = AT91C_SPI_ENDTX;
      }
   }

   //ACK de l'IT au CPU
   cyg_interrupt_acknowledge(vector);
   //appel du DSR
   return(CYG_ISR_HANDLED | CYG_ISR_CALL_DSR);
}
#endif




/*-------------------------------------------
| Name:dev_at91sam9261_spi_timer_callback
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static void  dev_at91sam9261_spi_timer_callback(void)
{
   //le create et le reload entrainent un appel systematique a l'IT:
   // => on ignore cet appel
   if (!ItDueToReload)
      spi_timeout = SPI_TIMEOUT_KO;
   ItDueToReload = false;
}


/*-------------------------------------------
| Name       :dev_at91sam9261_spi_load
| Description:
| Parameters :
| Return Type:
| Comments   :
| See        :
---------------------------------------------*/
int dev_at91sam9261_spi_load (void)
{
   int cpt;
   //attributs de programmation du mutex
   pthread_mutexattr_t mutex_attr = 0;
   //le driver est il deja charge?
   if (dev_at91sam9261_spi_loaded)
      //oui: on quitte
      return 0;
   //positionnement de l'indicateur de chargement du driver
   dev_at91sam9261_spi_loaded = 1;
   //creation du mutex de reentrance du driver
   kernel_pthread_mutex_init(&s_spi_mutex, &mutex_attr);

   // initialization
   for (cpt=0; cpt < SPI_SDCARD_BLOCK_SIZE; cpt++)
   {
#ifndef PROFILING_SPI
      dummy_block[cpt]=0xff;
#else
      dummy_block[cpt]=cpt%0xff;     // cycle
#endif
   }

   return 0;
}

/*-------------------------------------------
| Name       :dev_at91sam9261_spi_open
| Description:
| Parameters :
| Return Type:
| Comments   :
| See        :
---------------------------------------------*/
int dev_at91sam9261_spi_open(desc_t desc, int o_flag)
{
#ifdef USE_DMA_INTERUPT_TRANSMISSION
   //variables pour la gestion de l'IT de fin d'emission
   cyg_vector_t spi_vector = CYGNUM_HAL_INTERRUPT_SPI0;
   cyg_priority_t spi_prior = CYGNUM_HAL_H_PRIOR;
#endif
   //attributs de programmation du timer
   rttmr_attr_t spi_timer_attr;

   //creation du timer de timeout pour la fin d'attente d'emission
   spi_timer_attr.tm_msec = TIMEOUT_SPI;
   spi_timer_attr.func     = (tmr_func_t)dev_at91sam9261_spi_timer_callback;
   spi_timer_attr.data     = 0;
   rttmr_create(&dev_at91sam9261_spi_timer, &spi_timer_attr);

   // Enable peripheral clock for selected SPI
   *AT91C_PMC_PCER   = 1 << AT91C_ID_SPI0;

   // Enable peripheral clock for selected PIOA
   *AT91C_PMC_PCER   = 1 << AT91C_ID_PIOA;     //GPIO clock enable

   // Enable Write Enable and SD card presence
   *AT91C_PIOA_PER   = AT91C_PIO_PA4;      // (1<<4);   // Write Enable on PA04
   *AT91C_PIOA_MDDR  = AT91C_PIO_PA4;      // (1<<4);   // Write Enable/disable open drain
   *AT91C_PIOA_PER   = AT91C_PIO_PA5;      // (1<<5);   // SD Card presence on PB05
   *AT91C_PIOA_MDDR  = AT91C_PIO_PA5;      // (1<<5);   // SD Card disable open drain
   *AT91C_PIOA_ODR   = AT91C_PIO_PA4;      // (1<<4);  //  Input only
   *AT91C_PIOA_ODR   = AT91C_PIO_PA5;      // (1<<5);  //  Input only

   // Enable Chip Select line /GPIO on NPCS0 PA03
   *AT91C_PIOA_PER   = AT91C_PIO_PA3;      // (1<<3);  // Write Enable on PA111
   *AT91C_PIOA_MDDR  = AT91C_PIO_PA3;      // (1<<3);  // Write Enable disable open drain
   *AT91C_PIOA_OER   = AT91C_PIO_PA3;      //(1<<3);   // Enable Output on the I/O line
   *AT91C_PIOA_OWER  = AT91C_PIO_PA3;      //(1<<3);	  // Port A all ODSR enabled
   *AT91C_PIOA_SODR  = AT91C_PIO_PA3;      //(1<<3);   //=1 => set CS

   // Setup PIO pins for SPI interface
   // Disable PIO control of PA11/NPCS0, PA12/MISO, PA13/MOSI, PA14/SPCK
   *AT91C_PIOA_PDR = ( AT91C_PIO_PA0 |      // (1<<0)
                       AT91C_PIO_PA1 |                                  // (1<<1)
                       AT91C_PIO_PA2);                                  // (1<<2)

   // Enable I/O peripheral mode A
   *AT91C_PIOA_ASR = ( AT91C_PIO_PA0 |      // (1<<0)
                       AT91C_PIO_PA1 |                                  // (1<<1)
                       AT91C_PIO_PA2);                                   // (1<<2)

   *AT91C_SPI0_PTCR = AT91C_PDC_RXTDIS|AT91C_PDC_TXTDIS;      // disable DMA transfer TX

   *AT91C_SPI0_CR  = AT91C_SPI_SWRST;      // Software reset
   *AT91C_SPI0_CR  = AT91C_SPI_SPIEN;      // enable the SPI to transfer and receive data
   *AT91C_SPI0_IDR = 0xffffffff;           // Disable all SPI interrupts.

   // Set master mode with:
   *AT91C_SPI0_MR  =  AT91C_SPI_MSTR     |       // SPI MASTER
                     AT91C_SPI_PS_FIXED |                                      // Fixed peripheral Select
                     AT91C_SPI_MODFDIS;                                        // No mode fault

   // Setup data transfer format and rate for device 0 => 8 bits, CPOL=0, NCPHA=1
   *AT91C_SPI0_CSR  = AT91C_SPI_NCPHA         |
                      AT91C_SPI_BITS_8        |
                      (DLYBCT_SPI_CSR0 << 24) |                                // Delay between 2 charact
                      (SPI_SCKDIV_ARM9 << 8);                                  // SCBR

   *AT91C_AIC_ICCR = (1 << AT91C_ID_SPI0);      // Clears spi interrupt.

#ifdef USE_DMA_INTERUPT_TRANSMISSION
   //initialisation du semaphore d'attente
   kernel_sem_init(&spi_sem_tx_end, 0, 0);

   //Primitive de creation de l'IT au chargement du driver
   cyg_interrupt_create(spi_vector,
                        spi_prior,
                        0,
                        &dev_at91sam9261_isr,
                        &dev_at91sam9261_spi_dsr,
                        &_at91sam9261_spi_handle,
                        &_at91sam9261_spi_it);

   //Liaison entre l'IT crée et le vecteur d'IT
   cyg_interrupt_attach(_at91sam9261_spi_handle);
   //connecter l'it
   cyg_interrupt_unmask(spi_vector);
#endif

   return 0;
}

/*-------------------------------------------
| Name:dev_at91sam9261_spi_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91sam9261_spi_close(desc_t desc){
#ifdef USE_DMA_INTERUPT_TRANSMISSION
   //masquer l'it SPI
   cyg_interrupt_mask(CYGNUM_HAL_INTERRUPT_SPI0);
   //supprimer l'IT
   cyg_interrupt_delete(_at91sam9261_spi_handle);
   //detruire le semaphore d'attente
   kernel_sem_destroy(&spi_sem_tx_end);
#endif
   //destruction du timer
   rttmr_delete(&dev_at91sam9261_spi_timer);
}

/*-------------------------------------------
| Name:dev_at91sam9261_spi_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91sam9261_spi_seek(desc_t desc,int offset,int origin){
   return 0;
}


/*-------------------------------------------
| Name:       at91_spi_poll_status
| Description:Test status bit with Bit with timeout
| Parameters: Bit , delay in ms
| Return Type: OK or KO
| Comments:
| See:
---------------------------------------------*/
static short at91_spi_poll_status(unsigned long bit, unsigned int delay)
{
   static unsigned int lastDelay = TIMEOUT_SPI;  //valeur a initialiser avec la valeur fournie lors du rttmr_create()

   //le delay a t'il change par rapport au dernier passage ?
   if (delay != lastDelay)
   {
      //oui: on programme la nouvelle valeur
      //attention le reload provoque la generation immediate d'une IT
      ItDueToReload = true;
      rttmr_reload(&dev_at91sam9261_spi_timer, delay);
      lastDelay = delay;
   }
   //on ignore l'IT eventuelle due au reload
   spi_timeout = SPI_TIMEOUT_OK;
   rttmr_start(&dev_at91sam9261_spi_timer);

   while (((*AT91C_SPI0_SR & bit) == 0)   &&
          (spi_timeout != SPI_TIMEOUT_KO))
      ;

   rttmr_stop(&dev_at91sam9261_spi_timer);

   return (spi_timeout);
}



/*-------------------------------------------
| Name       :dev_at91sam9261_spi_write
| Description:
| Parameters : desc : descriptor
|              buf  : buffer (data + header)
|              size : dada size + header size
|
| Return Type:
| Comments   : -
| See        : -
---------------------------------------------*/
int dev_at91sam9261_spi_write (desc_t desc, const char* buf, int size)
{
   int cpt;
   unsigned char c;

   __dev_spi_lock();

   memcpy (g_Bufferwrite,buf,size);

   // force BYTE_PER_BYTE_TRANSMISSION
   if (size <= LIMIT_SIZE_BYTE_PER_BYTE_FCT)
   {
      // Write data to be transmit
      for (cpt=0; cpt < size; cpt++)
      {
         // wait for transmit completion/ready
         at91_spi_poll_status (AT91C_SPI_TDRE, TIMEOUT_SPI);
         *AT91C_SPI0_TDR = g_Bufferwrite[cpt];

         // wait for completion
         at91_spi_poll_status (AT91C_SPI_RDRF, TIMEOUT_SPI);
         c=*AT91C_SPI0_RDR;            //  read RDR here
      }
   }
   else
   {
#ifdef USE_BYTE_PER_BYTE_TRANSMISSION
      // Write data to be transmit
      for (cpt=0; cpt <size; cpt++)
      {
         //attente active de la fin d'emission
         at91_spi_poll_status (AT91C_SPI_TDRE, TIMEOUT_SPI);
         *AT91C_SPI0_TDR = g_Bufferwrite[cpt];

         //attente active de la fin de réception
         at91_spi_poll_status (AT91C_SPI_RDRF, TIMEOUT_SPI);
         c=*AT91C_SPI0_RDR;            //  read RDR here
      }
#endif

#ifdef USE_DMA_TRANSMISSION
      // disable DMA transfer RX & TX
      *AT91C_SPI0_PTCR = AT91C_PDC_RXTDIS | AT91C_PDC_TXTDIS;

      //indication au DMA de l'adresse et de la taille du buffer a emettre
      *AT91C_SPI0_RPR = (unsigned int)s_buf_in;             // trash
      *AT91C_SPI0_TPR = (unsigned int)g_Bufferwrite;
      *AT91C_SPI0_RCR = size;            // A remettre
      *AT91C_SPI0_TCR = size;            // A remettre

      // enable DMA transfer RX & TX
      *AT91C_SPI0_PTCR = AT91C_PDC_RXTEN | AT91C_PDC_TXTEN;
      //attente active de la fin d'emission
      at91_spi_poll_status (AT91C_SPI_ENDTX, TIMEOUT_SPI*size);

      // disable DMA transfer RX & TX
      *AT91C_SPI0_PTCR = AT91C_PDC_RXTDIS | AT91C_PDC_TXTDIS;
#endif


#ifdef USE_DMA_INTERUPT_TRANSMISSION
      __clr_irq();
      // disable DMA transfer RX & TX
      *AT91C_SPI0_PTCR = AT91C_PDC_RXTDIS | AT91C_PDC_TXTDIS;

      *AT91C_SPI0_RPR = (unsigned int)s_buf_in;           // trash
      //indication au DMA de l'adresse et de la taille du buffer a emettre
      *AT91C_SPI0_TPR = (unsigned int)g_Bufferwrite;
      *AT91C_SPI0_RCR = size;
      *AT91C_SPI0_TCR = size;

      // enable DMA transfer RX & TX
      *AT91C_SPI0_PTCR = AT91C_PDC_RXTEN | AT91C_PDC_TXTEN;

      //autoriser l'interruption ENDTX
      *AT91C_SPI0_IER = AT91C_SPI_ENDTX;
      __set_irq();

      //attente passive de la fin d'emission
      kernel_sem_wait(&spi_sem_tx_end);

      __clr_irq();
      *AT91C_SPI0_PTCR = AT91C_PDC_RXTDIS | AT91C_PDC_TXTDIS;           // disable DMA transfer RX & TX
      __set_irq();
#endif
   }
   __dev_spi_unlock();

   return (size);
}

/*-------------------------------------------
| Name:dev_at91sam9261_spi_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91sam9261_spi_read(desc_t desc, char* buf, int size)
{
   int cpt,i=0;
   unsigned char c;

   __dev_spi_lock();

   // force BYTE_PER_BYTE_TRANSMISSION
   if (size <= LIMIT_SIZE_BYTE_PER_BYTE_FCT)
   {
      // wait for receive ready
      do
      {
         c = *AT91C_SPI0_RDR;
      }
      while ((*AT91C_SPI0_SR & AT91C_SPI_RDRF));

      // Receive the data block into buffer (byte per byte)
      for (cpt=0; cpt < size; cpt++)
      {
         at91_spi_poll_status (AT91C_SPI_TDRE, TIMEOUT_SPI);
#ifndef PROFILING_SPI
         *AT91C_SPI0_TDR = 0xff;
#else
         *AT91C_SPI0_TDR = cpt%0xff;         // cycle
#endif
         at91_spi_poll_status (AT91C_SPI_RDRF, TIMEOUT_SPI);
         g_BufferRead[cpt] = *AT91C_SPI0_RDR;
      }
   }
   else
   {
#ifdef USE_BYTE_PER_BYTE_TRANSMISSION
      // wait for receive ready
      do
      {
         c = *AT91C_SPI0_RDR;
      } while ((*AT91C_SPI0_SR & AT91C_SPI_RDRF));

      // Receive the data block into buffer (byte per byte)
      for (cpt=0; cpt < size; cpt++)
      {
         at91_spi_poll_status (AT91C_SPI_TDRE, TIMEOUT_SPI);
   #ifndef PROFILING_SPI
         *AT91C_SPI0_TDR = 0xff;
   #else
         *AT91C_SPI0_TDR = cpt%0xff;        // cycle
   #endif
         at91_spi_poll_status (AT91C_SPI_RDRF, TIMEOUT_SPI);
         buf[cpt] = *AT91C_SPI0_RDR;
      }
#endif

#ifdef USE_DMA_TRANSMISSION
      // initialisation
      *AT91C_SPI0_PTCR = AT91C_PDC_RXTDIS | AT91C_PDC_TXTDIS;           // disable DMA transfer TX

      *AT91C_SPI0_RPR  = (unsigned int)g_BufferRead;
      *AT91C_SPI0_TPR  = (unsigned int)dummy_block;           // 0xFF array
      *AT91C_SPI0_RCR  = size;
      *AT91C_SPI0_TCR  = size;

      *AT91C_SPI0_PTCR = AT91C_PDC_RXTEN | AT91C_PDC_TXTEN;           // enable DMA transfer RX & TX
      at91_spi_poll_status (AT91C_SPI_ENDTX, TIMEOUT_SPI*size);

      *AT91C_SPI0_PTCR = AT91C_PDC_RXTDIS | AT91C_PDC_TXTDIS;   // disable DMA transfer Rx & TX
#endif


#ifdef USE_DMA_INTERUPT_TRANSMISSION
      __clr_irq();
      // initialisation
      *AT91C_SPI0_PTCR = AT91C_PDC_RXTDIS | AT91C_PDC_TXTDIS;           // disable DMA transfer TX

      *AT91C_SPI0_RPR  = (unsigned int)g_BufferRead;
      *AT91C_SPI0_TPR  = (unsigned int)dummy_block;            // 0xFF array
      *AT91C_SPI0_RCR  = size;
      *AT91C_SPI0_TCR  = size;

      *AT91C_SPI0_PTCR = AT91C_PDC_RXTEN | AT91C_PDC_TXTEN;           // enable DMA transfer RX & TX
      *AT91C_SPI0_IER  = AT91C_SPI_ENDTX;
      __set_irq();
      // attente passive de la fin d'emission
      kernel_sem_wait(&spi_sem_tx_end);
      __clr_irq();
      *AT91C_SPI0_PTCR = AT91C_PDC_RXTDIS | AT91C_PDC_TXTDIS;           // disable DMA transfer RX & TX
      __set_irq();
#endif
   }
   memcpy (buf,g_BufferRead,size);

   __dev_spi_unlock();

   return (size);
}

/** @} */
/** @} */
/** @} */
/*===========================================
End of Source dev_at91sam9261_spi.c
=============================================*/
