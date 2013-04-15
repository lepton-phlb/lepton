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



/*============================================
| Includes
==============================================*/
#include "kernel/core/system.h"
#include "kernel/core/kernel.h"
#include "kernel/core/process.h"
#include "kernel/fs/vfs/vfsdev.h"
#include "kernel/core/ioctl_cpu.h"

#include "lib/libc/stdio/stdio.h"

//#include <ioat91m55800.h> //GD
//#include <intrinsic.h> //GD

#include <string.h>     // memcpy



/*============================================
| Global Declaration
==============================================*/
const char dev_at91_name[]="cpu0\0";

int dev_at91_load(void);
int dev_at91_open(desc_t desc, int o_flag);
int dev_at91_close(desc_t desc);
int dev_at91_isset_read(desc_t desc);
int dev_at91_isset_write(desc_t desc);
int dev_at91_read(desc_t desc, char* buf,int size);
int dev_at91_write(desc_t desc, const char* buf,int size);
int dev_at91_seek(desc_t desc,int offset,int origin);
int dev_at91_ioctl(desc_t desc,int request,va_list ap);

dev_map_t dev_at91_map={
   dev_at91_name,
   S_IFBLK,
   dev_at91_load,
   dev_at91_open,
   dev_at91_close,
   __fdev_not_implemented,
   __fdev_not_implemented,
   dev_at91_read,
   dev_at91_write,
   dev_at91_seek,
   dev_at91_ioctl //ioctl
};


static unsigned char _requested_interrupt_vector[8]={0};

static char dump_buffer[64];


#ifndef NUM_INT_SOURCES
   #define NUM_INT_SOURCES  32
#endif

extern OS_ISR_HANDLER* OS_ARM_InstallISRHandler (int ISRIndex, OS_ISR_HANDLER* pISRHandler);

/*============================================
| Implementation
==============================================*/

/*--------------------------------------------
| Name:        _kernel_print
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
void _kernel_print( char * p){

}

/*--------------------------------------------
| Name:        _kernel_dump_kernel
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
void _kernel_dump_kernel(void){
   unsigned int sz;
   unsigned char* p;
#if OS_CHECKSTACK
   _kernel_print("\r\n--------------------\r\n");
   _kernel_print("lepton kernel");
   sprintf(dump_buffer,"\r\nkernel stack begin addr= 0x%x\r\n",kernel_thread.tcb->pStack);
   _kernel_print(dump_buffer);
   //
   sprintf(dump_buffer,"kernel stack end addr= 0x%x\r\n",kernel_thread.tcb->pStackBot);
   _kernel_print(dump_buffer);

   for(p=(unsigned char*)kernel_thread.tcb->pStack;
       p!=kernel_thread.tcb->pStackBot; p--) {
      if(!((sz++)%16)) {
         sprintf(dump_buffer,"\r\n0x%x   ",p);
         _kernel_print(dump_buffer);
      }
      sprintf(dump_buffer,"0x%2x ",*p);
      _kernel_print(dump_buffer);
   }
#endif
}

/*--------------------------------------------
| Name:        _kernel_dump_process
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
void _kernel_dump_process(pid_t _pid){
   unsigned int sz;
   char* p;
#if OS_CHECKSTACK
   _kernel_print("\r\n--------------------\r\n");
   _kernel_print(process_lst[_pid]->arg);

   sprintf(dump_buffer,"\r\nstack begin addr= 0x%x\r\n",process_lst[_pid]->pthread_ptr->tcb->pStack);
   _kernel_print(dump_buffer);
   //
   sprintf(dump_buffer,"stack end addr= 0x%x\r\n",process_lst[_pid]->pthread_ptr->tcb->pStackBot);
   _kernel_print(dump_buffer);

   for(p=(unsigned char*)process_lst[_pid]->pthread_ptr->tcb->pStack;
       p!=process_lst[_pid]->pthread_ptr->tcb->pStackBot; p--) {
      if(!((sz++)%16)) {
         sprintf(dump_buffer,"\r\n0x%x   ",p);
         _kernel_print(dump_buffer);
      }
      sprintf(dump_buffer,"0x%2x ",*p);
      _kernel_print(dump_buffer);
   }
#endif
}


/*--------------------------------------------
| Name:        _kernel_dump
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
void _kernel_at91_dump(void){
   pid_t _pid=1;

   sprintf(dump_buffer,"_kernel_dump\r\n");
   _kernel_print(dump_buffer);

   //
   _kernel_dump_kernel();

   _kernel_print("\r\nall running process dump\r\n");

   //current task
   for(_pid=1; _pid<=PROCESS_MAX; _pid++) {

      if(!process_lst[_pid])
         continue;
      /*
      if( !(process_lst[_pid]->pstat&(PSTAT_FORK|PSTAT_ZOMBI))
         && __is_thread_self(process_lst[_pid]->pthread_ptr->tcb)){
         _kernel_print(process_lst[_pid]->arg);
         sprintf(dump_buffer,"\r\ncurrent task pid= %d\r\n",_pid);
         _kernel_print(dump_buffer);
      }

      //to do: replace __pthread_self_id() with __is_thread_self(tcb)
      if( !(process_lst[_pid]->pstat&(PSTAT_FORK|PSTAT_ZOMBI))
         && (&process_lst[_pid]->pthread_ptr->tcb==OS_pActiveTask)){
         sprintf(dump_buffer,"active task pid= %d\r\n",_pid);
         _kernel_print(dump_buffer);
      }
      */
   }


   //current task
   for(_pid=1; _pid<=PROCESS_MAX; _pid++) {
      if(!process_lst[_pid])
         continue;
      _kernel_dump_process(_pid);
   }

}

/*--------------------------------------------
| Name:        _default_interrupt
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/

void  _default_at91_interrupt(void){
   //OS_Error(0);
}

/*--------------------------------------------
| Name:        _set_vector_handler
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
static void _set_vector_handler (char irq_no, void (* fn)(void)){
   //OS_ARM_InstallISRHandler (irq_no, (OS_ISR_HANDLER*)fn);
}

/*--------------------------------------------
| Name:        set_default_interrrupt_vector
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
static void _set_default_interrupt_vector(void){
   int i;
   for(i=0; i<NUM_INT_SOURCES; i++)
      _set_vector_handler(i,_default_at91_interrupt);
}

/*-------------------------------------------
| Name:dev_at91_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91_load(void){
   unsigned char irq_no;

   _set_default_interrupt_vector();
   return 0;
}

/*-------------------------------------------
| Name:dev_at91_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91_open(desc_t desc, int o_flag){

   //
   if(o_flag & O_RDONLY) {
   }

   if(o_flag & O_WRONLY) {
   }

   ofile_lst[desc].offset=0;


   return 0;
}

/*-------------------------------------------
| Name:dev_at91_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91_close(desc_t desc){
   return 0;
}

/*-------------------------------------------
| Name:dev_at91_isset_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91_isset_read(desc_t desc){
   return -1;
}

/*-------------------------------------------
| Name:dev_at91_isset_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91_isset_write(desc_t desc){
   return -1;
}
/*-------------------------------------------
| Name:dev_at91_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91_read(desc_t desc, char* buf,int size){
   return -1;
}

/*-------------------------------------------
| Name:dev_at91_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91_write(desc_t desc, const char* buf,int size){
   return -1;
}

/*-------------------------------------------
| Name:dev_at91_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91_seek(desc_t desc,int offset,int origin){

   switch(origin) {

   case SEEK_SET:
      ofile_lst[desc].offset=offset;
      break;

   case SEEK_CUR:
      ofile_lst[desc].offset+=offset;
      break;

   case SEEK_END:
      //to do: warning in SEEK_END (+ or -)????
      ofile_lst[desc].offset+=offset;
      break;
   }

   return ofile_lst[desc].offset;
}

/*--------------------------------------------
| Name:        _set_request_interrupt
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
static int _request_interrupt(int irq_no){
   unsigned char byte_no = irq_no>>3;
   unsigned char bit_no  = irq_no - byte_no<<3;
   unsigned char msk = (0x01<<(bit_no));

   if(_requested_interrupt_vector[byte_no]&msk)
      return -1;  //already requested

   _requested_interrupt_vector[byte_no]|=msk;

   return 0;
}

/*--------------------------------------------
| Name:        _free_interrupt
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
static int _free_interrupt(int irq_no){
   unsigned char byte_no = irq_no>>3;
   unsigned char bit_no  = irq_no - byte_no<<3;
   unsigned char msk = (0x01<<(bit_no));

   if(_requested_interrupt_vector[byte_no]&msk)
      return -1;  //already requested

   _requested_interrupt_vector[byte_no]|=msk;

   return 0;
}
/*-------------------------------------------
| Name:dev_ttys1_ioctl
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91_ioctl(desc_t desc,int request,va_list ap){

   switch(request) {
   case CPURQSTIRQ: {
      int irq_no;
      fdev_interrupt_t fn_interrupt;

      irq_no = va_arg( ap, int);

      fn_interrupt = va_arg( ap, fdev_interrupt_t);
      if(!fn_interrupt)
         return -1;

      if(_request_interrupt(irq_no)<0)
         return -1;   //allready allocated


      _set_vector_handler(irq_no,fn_interrupt);

   }
   break;

   case CPUFREEIRQ: {
      int irq_no;
      irq_no = va_arg( ap, int);

      _free_interrupt(irq_no);
      _set_vector_handler(irq_no,_default_at91_interrupt);
   }
   break;


   //
   default:
      return -1;

   }

   return 0;
}

/*============================================
| End of Source  : dev_at91.c
==============================================*/
