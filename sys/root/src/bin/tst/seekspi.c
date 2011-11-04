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

#include "ctype/ctype.h"
#include "kernel/types.h"
#include "kernel/signal.h"
#include "kernel/libstd.h"
#include "kernel/stat.h"
#include "kernel/time.h"
#include "kernel/devio.h"
#include "kernel/fcntl.h"

#include "kernel/errno.h"

#include "termios/termios.h"
#include "stdio/stdio.h"

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
/*--------------------------------------------
| Name:        tst_twi.c
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
void seek_spi(int pos)              // Nous sommes dans le processus P1
{
  int    fd_in, fd_out, fd;
  int    cb;
  int    i;

  
  // Descripteur fd_out en lecture seule
  if ((fd = open("/dev/spi0",O_RDONLY|O_WRONLY,0)) == -1)
  {
     printf("\nOpen KO\n");
     exit(0);
  }
   
   // test d'écriture sur i2c
   cb = lseek (fd,
              pos,
              SEEK_SET);
}




/*-------------------------------------------
| Name        : tstspi_main
| Description :
| Parameters  :
| Return Type :
| Comments    :
| See         :
---------------------------------------------*/
int seekspi_main (int argc, char* argv[])
{
  
  int offset;
  
  offset = atoi(argv[1]);
    
  seek_spi (offset);

  return 0;


}

/*===========================================
End of Source tstspi_main.c
=============================================*/
