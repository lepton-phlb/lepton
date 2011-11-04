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
#include <string.h>
#include "cyg/hal/hal_io.h"

#include "kernel/core/types.h"
#include "kernel/core/interrupt.h"
#include "kernel/core/system.h"
#include "kernel/core/fcntl.h"

#include "kernel/fs/vfs/vfsdev.h"
#include "kernel/fs/vfs/vfstypes.h"

#include "lib/libc/termios/termios.h"


#include "dev_linux_kb.h"
#include "kernel/dev/arch/gnu32/common/linux_hdwr_ops.h"

/*============================================
| Global Declaration
==============================================*/

#define CYGNUM_HAL_H_PRIOR 3

int dev_linux_kb_load(void);
int dev_linux_kb_open(desc_t desc, int o_flag);
int dev_linux_kb_close(desc_t desc);
int dev_linux_kb_isset_read(desc_t desc);
int dev_linux_kb_isset_write(desc_t desc);
int dev_linux_kb_read(desc_t desc, char* buf,int size);
int dev_linux_kb_write(desc_t desc, const char* buf,int size);
int dev_linux_kb_seek(desc_t desc,int offset,int origin);
int dev_linux_kb_ioctl(desc_t desc,int request,va_list ap);

void dev_linux_kb_rcv(void);

//ISR and DSR
cyg_uint32 dev_linux_kb_isr(cyg_vector_t vector, cyg_addrword_t data);
void dev_linux_kb_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data);

//
const char dev_linux_kb_name[]="kb0\0";

dev_map_t dev_linux_kb_map={
   dev_linux_kb_name,
   S_IFCHR,
   dev_linux_kb_load,
   dev_linux_kb_open,
   dev_linux_kb_close,
   dev_linux_kb_isset_read,
   dev_linux_kb_isset_write,
   dev_linux_kb_read,
   dev_linux_kb_write,
   dev_linux_kb_seek,
   dev_linux_kb_ioctl
};

//
#define KB_INPUT_BUFFER_SIZE    10

//buffer et variables de lecture
unsigned char kb_rcv_buffer[KB_INPUT_BUFFER_SIZE] = {0};
static volatile signed int _linux_kb_input_r;
static volatile signed int _linux_kb_input_w;
static volatile int _linux_kb_desc_rd = -1; //flag O_RDONLY


//variables pour la gestion de l'IT
static cyg_interrupt _linux_kb_it;
static cyg_handle_t _linux_kb_handle;

static int rcv_flag = 0;
//
static virtual_cmd_t kb_cmd;
static virtual_kb_t * kb_0_data;
extern void * shared_dev_addr;

/*============================================
| Implementation
==============================================*/

/*-------------------------------------------
| Name:dev_linux_com0_isr
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
/*Handler d'IT*/
cyg_uint32 dev_linux_kb_isr(cyg_vector_t vector, cyg_addrword_t data)
{
   /*Blocage d'une IT similaire tant que la DSR ne sait pas exécutée*/
   cyg_interrupt_mask(vector);
   //
   if(data==OPS_READ) {
      dev_linux_kb_rcv();
   }

   /*ACK de l'IT au CPU*/
   cyg_interrupt_acknowledge(vector);
   /*Informe kernel d'exécuter DSR*/
   return(CYG_ISR_HANDLED | CYG_ISR_CALL_DSR);
}

/*-------------------------------------------
| Name:dev_linux_kb_dsr
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
/*DSR*/
void dev_linux_kb_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data) {
   if(rcv_flag) {
      rcv_flag=0;
      __fire_io_int(ofile_lst[_linux_kb_desc_rd].owner_pthread_ptr_read);
   }
   cyg_interrupt_unmask(vector);
}

/*-------------------------------------------
| Name:dev_linux_kb_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_kb_load(void) {
   cyg_vector_t kb_vector = CYGNUM_HAL_INT_KB;
   cyg_priority_t kb_prior = CYGNUM_HAL_H_PRIOR;
   //
   kb_0_data = (virtual_kb_t *)(shared_dev_addr + KB0_OFFSET);

   //Primitive de creation de l'IT
   cyg_interrupt_create(kb_vector, kb_prior, 0,
         &dev_linux_kb_isr, &dev_linux_kb_dsr,
         &_linux_kb_handle, &_linux_kb_it);
   //Liaison entre l'IT crée et le vecteur d'IT
   cyg_interrupt_attach(_linux_kb_handle);

   return 0;
}

/*-------------------------------------------
| Name:dev_linux_kb_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_kb_open(desc_t desc, int o_flag) {
   cyg_vector_t kb_vector = CYGNUM_HAL_INT_KB;

   if(o_flag & O_WRONLY) {

   }
   //
   if(o_flag & O_RDONLY) {
      if(_linux_kb_desc_rd>=0) //already open
         return -1;
      _linux_kb_input_r = 0;
      _linux_kb_input_w = 0;
      _linux_kb_desc_rd = desc;
   }
   //
   kb_cmd.hdwr_id = KB;
   kb_cmd.cmd = OPS_OPEN;
   //
   //disable IT
   __clr_irq();
   while(cyg_hal_sys_write(1, (void *)&kb_cmd, sizeof(virtual_cmd_t)) !=sizeof(virtual_cmd_t));
   while(cyg_hal_sys_read(0, (void *)&kb_cmd, sizeof(virtual_cmd_t)) !=sizeof(virtual_cmd_t));
   //enable IT
   __set_irq();

   cyg_interrupt_unmask(kb_vector);
   return 0;
}

/*-------------------------------------------
| Name:dev_linux_kb_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_kb_close(desc_t desc) {
   if(ofile_lst[desc].oflag & O_RDONLY){
      if(!ofile_lst[desc].nb_reader){
         _linux_kb_desc_rd = -1;
         kb_cmd.hdwr_id = KB;
         kb_cmd.cmd = OPS_CLOSE;
         //
         //disable IT
         __clr_irq();
         while(cyg_hal_sys_write(1, (void *)&kb_cmd, sizeof(virtual_cmd_t)) !=sizeof(virtual_cmd_t));
         while(cyg_hal_sys_read(0, (void *)&kb_cmd, sizeof(virtual_cmd_t)) !=sizeof(virtual_cmd_t));
         //enable IT
         __set_irq();
      }
   }
   //
   if(ofile_lst[desc].oflag & O_WRONLY){
      if(!ofile_lst[desc].nb_writer){
      }
   }

   return 0;
}


/*-------------------------------------------
| Name:dev_linux_kb_isset_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_kb_isset_read(desc_t desc) {
   if(_linux_kb_input_w!=_linux_kb_input_r)
      return 0;
   else
      return -1;
}

/*-------------------------------------------
| Name:dev_linux_kb_isset_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_kb_isset_write(desc_t desc) {
   return 0;
}

/*-------------------------------------------
| Name:dev_linux_kb_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_kb_read(desc_t desc, char* buf,int size) {
   int cb = 0;
   //
   while(_linux_kb_input_r != _linux_kb_input_w){
      buf[cb++] = kb_rcv_buffer[_linux_kb_input_r];
      _linux_kb_input_r = (_linux_kb_input_r+1)&(~KB_INPUT_BUFFER_SIZE);
      if(cb == size)   break;
   }
   return cb;
}

/*-------------------------------------------
| Name:dev_linux_kb_rcv
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void dev_linux_kb_rcv(void) {
   //get data from shared memory
   if(_linux_kb_input_w == _linux_kb_input_r) {
      rcv_flag=1;
   }
   //
   kb_rcv_buffer[_linux_kb_input_w] = kb_0_data->data_in;
   _linux_kb_input_w = (_linux_kb_input_w+1)&(~KB_INPUT_BUFFER_SIZE);
}

/*-------------------------------------------
| Name:dev_linux_kb_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
//extern volatile cyg_bool_t hal_interrupts_enabled;
int dev_linux_kb_write(desc_t desc, const char* buf,int size){
   return 0;
}

/*-------------------------------------------
| Name:dev_linux_kb_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_kb_seek(desc_t desc,int offset,int origin){
   return 0;
}

/*-------------------------------------------
| Name:dev_linux_kb_ioctl
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_kb_ioctl(desc_t desc,int request,va_list ap) {
   return 0;
}

/*============================================
| End of Source  : dev_linux_kb.c
|---------------------------------------------
| Historic:
|---------------------------------------------
| Authors     | Date     | Comments
| $Log: not supported by cvs2svn $
| Revision 1.2  2009/07/09 15:21:54  jjp
| add commentary template
|
==============================================*/


