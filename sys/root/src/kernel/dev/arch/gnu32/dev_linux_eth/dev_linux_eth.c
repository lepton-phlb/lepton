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

#include "dev_linux_eth.h"
#include "kernel/dev/arch/gnu32/common/linux_hdwr_ops.h"

/*============================================
| Global Declaration
==============================================*/

#define CYGNUM_HAL_H_PRIOR       3
#define ETH_FRAME_LEN            SHM_ETH_SIZE
#define MAX_POOL                 32//64//16//4

#define ETH_0_INPUT_BUFFER_SZ    (MAX_POOL*ETH_FRAME_LEN)//SHM_ETH_0_SIZE

#define ETH_0_OUTPUT_BUFFER_SZ   ETH_FRAME_LEN
#define ETH_0_OUTPUT_BUFFER_OFF  SHM_ETH_0_SIZE/2

//
int dev_linux_eth_load(void);
int dev_linux_eth_open(desc_t desc, int o_flag);
int dev_linux_eth_close(desc_t desc);
int dev_linux_eth_isset_read(desc_t desc);
int dev_linux_eth_isset_write(desc_t desc);
int dev_linux_eth_read(desc_t desc, char* buf,int size);
int dev_linux_eth_write(desc_t desc, const char* buf,int size);
int dev_linux_eth_seek(desc_t desc,int offset,int origin);
int dev_linux_eth_ioctl(desc_t desc,int request,va_list ap);
void dev_linux_eth_snd(void);
void dev_linux_eth_rcv(void);

//ISR and DSR
cyg_uint32 dev_linux_eth_isr(cyg_vector_t vector, cyg_addrword_t data);
void dev_linux_eth_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data);

const char dev_linux_eth_name[]="eth0\0";

dev_map_t dev_linux_eth_map={
   dev_linux_eth_name,
   S_IFCHR,
   dev_linux_eth_load,
   dev_linux_eth_open,
   dev_linux_eth_close,
   dev_linux_eth_isset_read,
   dev_linux_eth_isset_write,
   dev_linux_eth_read,
   dev_linux_eth_write,
   dev_linux_eth_seek,
   dev_linux_eth_ioctl
};

//variables pour la gestion de l'IT
static cyg_interrupt _linux_eth_it;
static cyg_handle_t _linux_eth_handle;

//
static int _linux_eth_input_r = -1;
static int _linux_eth_input_w = -1;
//
static int _linux_eth_output_r = -1;
static int _linux_eth_output_w = -1;
//
static int _linux_eth_desc_wr = -1;
static int _linux_eth_desc_rd = -1;

//
static int rcv_flag = 0;

//shared memories for read and write

static virtual_cmd_t eth_cmd;
static virtual_eth_t * eth_0_data;
extern void * shared_dev_addr;
//
static int rcv_buf_input_no=0;
static int rcv_buf_read_no=0;
//

typedef struct {
   int size;
   char data[ETH_FRAME_LEN+1];
}net_packet_t;

static net_packet_t eth_rcv_buffer[MAX_POOL] = {0};

unsigned char lwip_ethaddr[6] = {0x00,0xff,0x8d,0x1a,0x2d,0xcc};
//00:ff:40:3e:ba:9b
//00:ff:8d:1a:2d:cc


/*============================================
| Implementation
==============================================*/

//
cyg_uint32 dev_linux_eth_isr(cyg_vector_t vector, cyg_addrword_t data) {
   //Blocage d'une IT similaire tant que la DSR ne sait pas exécutée
   cyg_interrupt_mask(vector);
   //
   if(data==OPS_READ) {
      dev_linux_eth_rcv();
   }
   if(data==OPS_WRITE) {
      dev_linux_eth_snd();
   }

   //ACK de l'IT au CPU
   cyg_interrupt_acknowledge(vector);
   //Informe kernel d'exécuter DSR
   return(CYG_ISR_HANDLED | CYG_ISR_CALL_DSR);
}


//DSR
void dev_linux_eth_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data) {
   if(rcv_flag) {
      rcv_flag=0;//if(rcv_buf_input_no == rcv_buf_read_no)   rcv_flag=0;
      __fire_io_int(ofile_lst[_linux_eth_desc_rd].owner_pthread_ptr_read);
   }
   if(_linux_eth_desc_wr>=0 && _linux_eth_output_r==_linux_eth_output_w){
      _linux_eth_output_r = -1;
      __fire_io_int(ofile_lst[_linux_eth_desc_wr].owner_pthread_ptr_write);
   }
   cyg_interrupt_unmask(vector);
}

//just allocate the shared memory segment and it entry
int dev_linux_eth_load(void) {
   cyg_vector_t eth_vector = CYGNUM_HAL_INT_ETH;
   cyg_priority_t eth_prior = CYGNUM_HAL_H_PRIOR;
   //
   eth_0_data = (virtual_eth_t *)(shared_dev_addr + ETH0_OFFSET);

   //Primitive de creation de l'IT
   cyg_interrupt_create(eth_vector, eth_prior, 0,
         &dev_linux_eth_isr, &dev_linux_eth_dsr,
         &_linux_eth_handle, &_linux_eth_it);
   //Liaison entre l'IT crée et le vecteur d'IT
   cyg_interrupt_attach(_linux_eth_handle);
   //cyg_interrupt_unmask(serial_vector);

   return 0;
}

//initialize data and enable IT
int dev_linux_eth_open(desc_t desc, int o_flag) {
   cyg_vector_t eth_vector = CYGNUM_HAL_INT_ETH;
   int rc;
   //
   if(o_flag & O_WRONLY) {
      if(_linux_eth_desc_wr>=0)
         return -1;
      _linux_eth_desc_wr=desc;
   }
   if(o_flag & O_RDONLY) {
      if(_linux_eth_desc_rd>=0)
         return -1;
      _linux_eth_desc_rd = desc;
      _linux_eth_input_r = 0;
      _linux_eth_input_w = 0;
   }

      //
   if(_linux_eth_desc_wr>=0 && _linux_eth_desc_rd>=0) {
      //open eth port in virtual cpu
      eth_cmd.hdwr_id = ETH_0;
      eth_cmd.cmd = OPS_OPEN;
      //

      //disable IT
      //__clr_irq();
      while(cyg_hal_sys_write(1, (void *)&eth_cmd, sizeof(virtual_cmd_t)) != sizeof(virtual_cmd_t));
      while(cyg_hal_sys_read(0, (void *)&eth_cmd, sizeof(virtual_cmd_t)) != sizeof(virtual_cmd_t));
      //enable IT
      //__set_irq();
      cyg_interrupt_unmask(eth_vector);
   }
   return 0;
}

//close virtual_cpu socket if nothing else to do
int dev_linux_eth_close(desc_t desc) {
   if(ofile_lst[desc].oflag & O_RDONLY) {
      if(!ofile_lst[desc].nb_reader) {
         _linux_eth_desc_rd = -1;
      }
   }
   //
   if(ofile_lst[desc].oflag & O_WRONLY) {
      if(!ofile_lst[desc].nb_writer) {
         _linux_eth_desc_wr = -1;
      }
   }
   //close all
   if(_linux_eth_desc_wr<0 && _linux_eth_desc_rd<0) {
      eth_cmd.hdwr_id = ETH_0;
      eth_cmd.cmd = OPS_CLOSE;
      //
      while(cyg_hal_sys_write(1, (void *)&eth_cmd, sizeof(virtual_cmd_t)) != sizeof(virtual_cmd_t));
      while(cyg_hal_sys_read(0, (void *)&eth_cmd, sizeof(virtual_cmd_t)) != sizeof(virtual_cmd_t));
   }
   return 0;
}

//
int dev_linux_eth_isset_read(desc_t desc) {
   if(rcv_buf_read_no != rcv_buf_input_no)
      return 0;
   else
      return -1;
}


//
int dev_linux_eth_isset_write(desc_t desc) {
   if(_linux_eth_output_r < 0)
      return 0;
   else
      return -1;
}


//read data
int dev_linux_eth_read(desc_t desc, char* buf,int size) {
   int cb = 0;
   //
   if(rcv_buf_read_no != rcv_buf_input_no) {
      cb = eth_rcv_buffer[rcv_buf_read_no].size;
      memcpy((void *)buf, (void *)eth_rcv_buffer[rcv_buf_read_no].data, cb);
      //
      rcv_buf_read_no = ((rcv_buf_read_no+1)&(~MAX_POOL));
   }
   return cb;
}


//send data to eth interface
int dev_linux_eth_write(desc_t desc, const char* buf,int size) {
   int rc;

   if(size>=ETH_0_OUTPUT_BUFFER_SZ)
      size = ETH_0_OUTPUT_BUFFER_SZ-1;

   eth_0_data->size_out = size;
   memcpy(eth_0_data->data_out, buf, eth_0_data->size_out);
   //
   _linux_eth_output_r = 0;
   _linux_eth_output_w = size;

   //send order to write data on hardware serial port
   eth_cmd.hdwr_id = ETH_0;
   eth_cmd.cmd = OPS_WRITE;

   //disable IT
   //__clr_irq();
   //write to wake virtual_cpu
   while(cyg_hal_sys_write(1, (void *)&eth_cmd, sizeof(virtual_cmd_t)) != sizeof(virtual_cmd_t));
   //while(cyg_hal_sys_read(0, (void *)&eth_cmd, sizeof(virtual_cmd_t)) != sizeof(virtual_cmd_t));

   //Enable all IT
   //__set_irq();
   return size;

}

//
int dev_linux_eth_seek(desc_t desc,int offset,int origin) {
   return 0;
}

//
int dev_linux_eth_ioctl(desc_t desc,int request,va_list ap) {
   return 0;
}

//clean buffer to send data
void dev_linux_eth_snd(void) {
   _linux_eth_output_r = _linux_eth_output_w;
}

//read data and store it on local buffer
void dev_linux_eth_rcv(void) {
   eth_rcv_buffer[rcv_buf_input_no].size = eth_0_data->size_in;
   memcpy((void *)(eth_rcv_buffer[rcv_buf_input_no].data), (void *)eth_0_data->data_in, ETH_FRAME_LEN);

   rcv_buf_input_no = ((rcv_buf_input_no+1)&(~MAX_POOL));
   rcv_flag=1;
}

/*============================================
| End of Source  : dev_linux_eth.c
|---------------------------------------------
| Historic:
|---------------------------------------------
| Authors     | Date     | Comments
| $Log: not supported by cvs2svn $
| Revision 1.7  2009/11/13 12:38:13  jjp
| bug fix on isset_write
|
| Revision 1.6  2009/07/09 15:19:42  jjp
| add commentary template
|
==============================================*/
