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
#define TAILLE_BUFFER_MAX  2048
char   g_buf_in[TAILLE_BUFFER_MAX];

/************************/
/*    Implementation    */
/************************/
/*--------------------------------------------
| Name:        tst_twi.c
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
void read_spi(int nb_element, int offset,  int seq_modulo)              // Nous sommes dans le processus P1
{
   int    fd_in, fd_out, fd;
   int    cb;
   char  buf;
   int    i,j;

   // Descripteur fd_out en lecture seule
   if ((fd = open("/dev/hd/sdhd0",O_RDONLY,0)) == -1)
//   if ((fd = open("/dev/spi0",O_RDONLY,0)) == -1)
   {
     printf("\nSD card KO!!\n");
     exit(0);
   }
     
   for (j=0; j < TAILLE_BUFFER_MAX; j++)
            g_buf_in[j]=j%seq_modulo;        // reference
   
   lseek(fd, offset, SEEK_SET);
   
      for (j=0;j< nb_element;j++)
      {  
         cb = read(fd, &buf, 1);
         offset++;
          lseek(fd, offset, SEEK_SET);
         if (cb <= 0)      
              printf("\nPB!!\n");
         if (buf != g_buf_in[j])
         {  
           printf("\nPB Non concordance!!\n"); 
           close (fd);
           exit(0);
         }
      }   
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
int readspi_main (int argc, char* argv[])
{
  
  int nb_element;
  int offset;
  int seq_modulo;
  
  nb_element = atoi(argv[1]);
  offset     = atoi(argv[2]);
  seq_modulo = atoi(argv[3]);
  
  read_spi (nb_element, offset,seq_modulo);

  return 0;
}

/*===========================================
End of Source readspi_main.c
=============================================*/
