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


/*============================================
| Includes    
==============================================*/
#include <stdlib.h>

#include "kernel/signal.h"
#include "kernel/libstd.h"
#include "kernel/devio.h"
#include "kernel/wait.h"
#include "kernel/fcntl.h"
#include "stdio/stdio.h"


#include "pthread/pthread.h"

#include "dev/dev_mem/dev_mem.h"

#include "bin/arch/m16c/msrd/msrd.h"

/*============================================
| Global Declaration 
==============================================*/


/*============================================
| Implementation 
==============================================*/
int tstspy_main(int argc,char* argv[]){

   int fd=0;
   prm_msrd_t spy_prm_msrd;
   res_msrd_t spy_res_msrd;


   printf("spy /dev/mem...\r\n");

   fd = open("/dev/mem",O_RDWR,0);

   //memspy
   
   ioctl(fd,MEMREG,MEM_ADDR_PRM_MSRD);
   ioctl(fd,MEMREG,MEM_ADDR_RES_MSRD);

   for(;;){
      int cb=0;
      //wait g_prm_msr modification
      lseek(fd,MEM_ADDR_PRM_MSRD,SEEK_SET);
      cb = read(fd,&spy_prm_msrd,sizeof(prm_msrd_t));
      if(cb>0){
         printf("prm_msrd.msr_type=%d\r\n",spy_prm_msrd.msr_type);
         printf("prm_msrd.startstop=%d\r\n",spy_prm_msrd.startstop);
      }
      lseek(fd,MEM_ADDR_RES_MSRD,SEEK_SET);
      cb = read(fd,&spy_res_msrd,sizeof(res_msrd_t));
      if(cb>0){
         int d = spy_res_msrd.msr_status;
         printf("res_msrd.msr_status=%d\r\n",d);
      }
   }


   return 0;

}

/*============================================
| End of Source  : tstspy.c
==============================================*/
