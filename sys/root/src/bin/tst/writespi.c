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

/**********************/
/*      Includes      */
/**********************/

#include <stdlib.h>
#include <math.h>

#include "kernel/core/kernel.h"
#include "kernel/core/libstd.h"
#include "kernel/core/stat.h"
#include "kernel/core/statvfs.h"
#include "kernel/core/devio.h"
#include "kernel/core/kernel_clock.h"
#include "kernel/core/time.h"
#include "kernel/core/fcntl.h"
#include "kernel/core/ioctl_eth.h"
#include "kernel/core/ioctl_if.h"

#include "lib/libc/termios/termios.h"
#include "lib/pthread/pthread.h"

#include "lib/libc/stdio/stdio.h"
#include "lib/libc/misc/crc.h"


/**********************/
/*    Equivalences    */
/**********************/

/********************/
/*    Prototypes    */
/********************/

/**********************/
/* Variables globales */
/**********************/

/************************/
/*    Implementation    */
/************************/


#define TAILLE_BUFFER_MAX  513

char   g_buf_out[TAILLE_BUFFER_MAX];
char   g_buf_in[TAILLE_BUFFER_MAX];


#define SPI_TP_DLYBS_ARM9  100

#define __reset_CS{\
                 int t;\
                 for(t=0;t<SPI_TP_DLYBS_ARM9;t++);\
                 *AT91C_PIOA_CODR = (1<<3);\
                 for(t=0;t<SPI_TP_DLYBS_ARM9;t++);\
      }

#define __set_CS{\
                 int t;\
                 for(t=0;t<SPI_TP_DLYBS_ARM9;t++);\
                 *AT91C_PIOA_SODR = (1<<3);\
                 for(t=0;t<SPI_TP_DLYBS_ARM9;t++);\
      }


#define __SPI_DRIVER 

/*--------------------------------------------
| Name:        tst_twi.c
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
void write_spi(int nb_element, int offset, int seq_modulo) 
{
   int    fd_in, fd_out, fd;
   int    cb;
   int    i,j;
   struct   timespec tp_deb,tp_fin;
   uint32_t time_memo_deb,time_memo_fin;
   
   // Descripteur fd_out en Ecriture seule
#ifdef __SPI_DRIVER    
{        
   //if ((fd = open("/dev/hd/sdhd0",O_WRONLY,0)) == -1)   
   if ((fd = open("/dev/spi0",O_WRONLY,0)) == -1)
   {
     printf("\nSD card KO!!\n");
     exit(0);
   }
}
#endif

   // préparation buffer de 512 octets à émettre
   for (j=0; j < TAILLE_BUFFER_MAX; j++)
            g_buf_out[j]=j%seq_modulo;
   
     // lseek (fd, offset, SEEK_SET);
      
   //   *AT91C_PIOA_CODR = (1 << 3);
      
// BOUCLE PRINCIPALE ----------------------------------------------------------
for (;;)
{     
//  clock_gettime(CLOCK_MONOTONIC, &tp_deb);  // initial time for top 500ms 
//   time_memo_deb           = tp_deb.tv_sec*1000L + tp_deb.tv_nsec;  
  
   // Chip Select Bas
   __reset_CS;
     
//  clock_gettime(CLOCK_MONOTONIC, &tp_fin);  // initial time for top 500ms 
//   time_memo_fin           = tp_fin.tv_sec*1000L + tp_fin.tv_nsec; 
    
//   printf("time = %d ns",time_memo_fin-time_memo_deb);  
   ((*AT91C_PIOA_SODR) = (1<<28));           // PA28_1
           
#ifdef __SPI_DRIVER    
{     

     
      // test d'écriture sur spi
      cb = write (fd,
                 &g_buf_out[0],
                 nb_element); // data + taille entete
      
      if (cb == -1)
      {
         printf("KO sur RET write\r\n");
         exit(0);
      }
}
#else
{
    ((*AT91C_PIOA_CODR) = (1<<28));         // PA28 à 0
    
     // memcpy (s_buf_out, buf, size); // with Static
     // préparation buffer de 512 octets à émettre
  // for (j=0; j < TAILLE_BUFFER_MAX; j++)
     for (j=0; j < 50; j++)
            g_buf_out[j]=j%seq_modulo;
   
  // initialisation
  *AT91C_SPI0_PTCR = AT91C_PDC_RXTDIS | AT91C_PDC_TXTDIS; // disable DMA transfer TX
   
  *AT91C_SPI0_RPR = (unsigned int)g_buf_in;   // trash
  *AT91C_SPI0_TPR = (unsigned int)g_buf_out;

  *AT91C_SPI0_RCR = nb_element;
  *AT91C_SPI0_TCR = nb_element; // Common sizes
     
  *AT91C_SPI0_PTCR = AT91C_PDC_RXTEN | AT91C_PDC_TXTEN; // enable DMA transfer TX
   
  while ( !(*AT91C_SPI0_SR & AT91C_SPI_ENDTX) )  ;
  // at91_spi_poll_status (AT91C_SPI_ENDTX, TIMEOUT_SPI*nb_element); 
       
  *AT91C_SPI0_PTCR = AT91C_PDC_RXTDIS | AT91C_PDC_TXTDIS; // disable DMA transfer TX

}   
#endif     
   
   // Chip Select Haut
    __set_CS;
	

}
// BOUCLE PRINCIPALE ----------------------------------------------------------

   close (fd);
}

/*-------------------------------------------
| Name        : tstspi_main
| Description :
| Parameters  :
| Return Type :
| Comments    :
| See         :
---------------------------------------------*/          
int writespi_main (int argc, char* argv[])
{
   int nb_element;
   int offset;
   int seq_modulo;

   nb_element = atoi(argv[1]);
   offset     = atoi(argv[2]);
   seq_modulo = atoi(argv[3]);
   
   // Enable peripheral clock for selected PIOA
   *AT91C_PMC_PCER     = 1 << AT91C_ID_PIOA; // GPIO clock enable  
   (*AT91C_PIOA_PER)   = 1 << 28;            // PA28
   (*AT91C_PIOA_OER)   = 1 << 28;            // Ouput
 
//   ((*AT91C_PIOA_CODR) = (1<<28));           // PA28_0
//   ((*AT91C_PIOA_SODR) = (1<<28));           // PA28_1
   
   write_spi (nb_element,offset,seq_modulo);

   return 0;
}

/*===========================================
End of Source writespi_main.c
=============================================*/
