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


#include "dev_linux_serial_pt.h"
#include "kernel/dev/arch/gnu32/common/linux_hdwr_ops.h"

/*============================================
| Global Declaration
==============================================*/
#define CYGNUM_HAL_H_PRIOR 3

int dev_linux_serial_pt_load(void);
int dev_linux_serial_pt_open(desc_t desc, int o_flag);
int dev_linux_serial_pt_close(desc_t desc);
int dev_linux_serial_pt_isset_read(desc_t desc);
int dev_linux_serial_pt_isset_write(desc_t desc);
int dev_linux_serial_pt_read(desc_t desc, char* buf,int size);
int dev_linux_serial_pt_write(desc_t desc, const char* buf,int size);
int dev_linux_serial_pt_seek(desc_t desc,int offset,int origin);
int dev_linux_serial_pt_ioctl(desc_t desc,int request,va_list ap);
void dev_linux_serial_pt_snd(void);
void dev_linux_serial_pt_rcv(void);

//ISR and DSR
cyg_uint32 dev_linux_serial_pt_isr(cyg_vector_t vector, cyg_addrword_t data);
void dev_linux_serial_pt_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data);

const char dev_linux_serial_pt_name[]="ttypt\0";

//dev_map_t dev_linux_serial_pt_map=KEEP OLD NAME for moment
dev_map_t dev_linux_compt_map={
   dev_linux_serial_pt_name,
   S_IFCHR,
   dev_linux_serial_pt_load,
   dev_linux_serial_pt_open,
   dev_linux_serial_pt_close,
   dev_linux_serial_pt_isset_read,
   dev_linux_serial_pt_isset_write,
   dev_linux_serial_pt_read,
   dev_linux_serial_pt_write,
   dev_linux_serial_pt_seek,
   dev_linux_serial_pt_ioctl
};

#define SERIAL_PT_INPUT_BUFFER_SIZE     (SHM_SERIAL_SIZE*4)
#define SERIAL_PT_OUTPUT_BUFFER_SIZE    (SHM_SERIAL_SIZE*4)

//buffer et variables de lecture
unsigned char serial_pt_rcv_buffer[SERIAL_PT_INPUT_BUFFER_SIZE] = {0};
static volatile signed int _linux_serial_pt_input_r; //associée au ptr de lecture du user ds buffer réception
static volatile signed int _linux_serial_pt_input_w; //associée au ptr d'écriture de kernel ds buffer réception
static volatile int _linux_serial_pt_desc_rd = -1; //flag O_RDONLY

//buffer et variables d'ecriture
unsigned char serial_pt_snd_buffer[SERIAL_PT_OUTPUT_BUFFER_SIZE] = {0};
static volatile signed int _linux_serial_pt_output_r;
static volatile signed int _linux_serial_pt_output_w;
static volatile signed int _linux_serial_pt_desc_wr = -1; //flag O_WRONLY
int _linux_serial_pt_loaded = 0;

//variables pour la gestion de l'IT
static cyg_interrupt _linux_serial_pt_it;
static cyg_handle_t _linux_serial_pt_handle;

//
//termios
static struct termios ttys_termios;
static int termios2ttys(struct termios* termios_p);
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


static int rcv_flag = 0;
//
static virtual_cmd_t serial_pt_cmd;
static virtual_serial_t * serial_pt_data;
extern void * shared_dev_addr;

/*============================================
| Implementation
==============================================*/
/*-------------------------------------------
| Name:dev_linux_serial_pt_isr
| Description:
| Parameters:
| Return Type:
| Comments: data is read from synth_io_sighandler on synth_intr
| See:
---------------------------------------------*/
/*Handler d'IT*/
cyg_uint32 dev_linux_serial_pt_isr(cyg_vector_t vector, cyg_addrword_t data)
{
   /*Blocage d'une IT similaire tant que la DSR ne sait pas exécutée*/
   cyg_interrupt_mask(vector);
   if(_linux_serial_pt_loaded) {
      //
      if(data==OPS_READ) {
         dev_linux_serial_pt_rcv();
      }
      if(data==OPS_WRITE) {
         dev_linux_serial_pt_snd();
      }
   }
   /*ACK de l'IT au CPU*/
   cyg_interrupt_acknowledge(vector);
   /*Informe kernel d'exécuter DSR*/
   return(CYG_ISR_HANDLED | CYG_ISR_CALL_DSR);
}

/*-------------------------------------------
| Name:dev_linux_serial_pt_dsr
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
/*DSR*/
void dev_linux_serial_pt_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
   /*Autorise à nouveau les IT de ce type*/
   if(rcv_flag) {
      rcv_flag=0;
      __fire_io_int(ofile_lst[_linux_serial_pt_desc_rd].owner_pthread_ptr_read);
   }
   if(_linux_serial_pt_desc_wr>=0 && _linux_serial_pt_output_r==_linux_serial_pt_output_w) {
      __fire_io_int(ofile_lst[_linux_serial_pt_desc_wr].owner_pthread_ptr_write);
   }
   cyg_interrupt_unmask(vector);
}

/*-------------------------------------------
| Name:dev_linux_serial_pt_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_serial_pt_load(void)
{
   cyg_vector_t serial_vector = CYGNUM_HAL_INT_SERIAL_PT;
   cyg_priority_t serial_prior = CYGNUM_HAL_H_PRIOR;
   int key=-1;

   if(_linux_serial_pt_loaded)
      return 0;   //yes

   _linux_serial_pt_desc_rd = -1;
   _linux_serial_pt_desc_wr = -1;

   _linux_serial_pt_loaded=1;

   //get shared memory
   serial_pt_data = (virtual_serial_t *)(shared_dev_addr + TTYPT_OFFSET);

   //Primitive de creation de l'IT
   cyg_interrupt_create(serial_vector, serial_prior, 0,
                        &dev_linux_serial_pt_isr, &dev_linux_serial_pt_dsr,
                        &_linux_serial_pt_handle, &_linux_serial_pt_it);
   //Liaison entre l'IT crée et le vecteur d'IT
   cyg_interrupt_attach(_linux_serial_pt_handle);

   return 0;
}

/*-------------------------------------------
| Name:dev_linux_serial_pt_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_serial_pt_open(desc_t desc, int o_flag)
{
   cyg_vector_t serial_vector = CYGNUM_HAL_INT_SERIAL_PT;
   //
   if(o_flag & O_WRONLY) {
      if(_linux_serial_pt_desc_wr>=0) //already open: exclusive resource.
         return -1;
      _linux_serial_pt_output_r = -1;
      _linux_serial_pt_output_w = 0;
      _linux_serial_pt_desc_wr = desc;
   }
   if(o_flag & O_RDONLY) {
      if(_linux_serial_pt_desc_rd>=0) //already open: exclusive resource.
         return -1;
      _linux_serial_pt_input_r = 0;
      _linux_serial_pt_input_w = 0;
      _linux_serial_pt_desc_rd= desc;
   }

   //open serial port in virtual cpu
   serial_pt_cmd.hdwr_id = SERIAL_PT;
   serial_pt_cmd.cmd = OPS_OPEN;
   //
   //disable IT
   //__clr_irq();
   while(cyg_hal_sys_write(1, (void *)&serial_pt_cmd,
                           sizeof(virtual_cmd_t)) != sizeof(virtual_cmd_t)) ;
   while(cyg_hal_sys_read(0, (void *)&serial_pt_cmd, sizeof(virtual_cmd_t)) != sizeof(virtual_cmd_t)) ;
   //enable IT
   //__set_irq();

   //
   cyg_interrupt_unmask(serial_vector);
   return 0;
}

/*-------------------------------------------
| Name:dev_linux_serial_pt_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_serial_pt_close(desc_t desc)
{
   if(ofile_lst[desc].oflag & O_RDONLY) {
      if(!ofile_lst[desc].nb_reader) {
         // Disable Interrupt on RXRDY
         //
         _linux_serial_pt_desc_rd = -1;
      }
   }
   //
   if(ofile_lst[desc].oflag & O_WRONLY) {
      if(!ofile_lst[desc].nb_writer) {
         // Disable Interrupt on TXRDY
         //
         _linux_serial_pt_desc_wr = -1;
      }
   }
   //close all
   if(_linux_serial_pt_desc_wr<0 && _linux_serial_pt_desc_rd<0) {
      //close serial port in virtual cpu
      serial_pt_cmd.hdwr_id = SERIAL_PT;
      serial_pt_cmd.cmd = OPS_CLOSE;
      //
      while(cyg_hal_sys_write(1, (void *)&serial_pt_cmd,
                              sizeof(virtual_cmd_t)) != sizeof(virtual_cmd_t)) ;
      while(cyg_hal_sys_read(0, (void *)&serial_pt_cmd,
                             sizeof(virtual_cmd_t)) != sizeof(virtual_cmd_t)) ;
   }

   return 0;
}


/*-------------------------------------------
| Name:dev_linux_serial_pt_isset_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_serial_pt_isset_read(desc_t desc)
{
   if(_linux_serial_pt_input_w!=_linux_serial_pt_input_r) {
      return 0;
   }
   else {
      return -1;
   }
}

/*-------------------------------------------
| Name:dev_linux_serial_pt_isset_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_serial_pt_isset_write(desc_t desc)
{
   if(_linux_serial_pt_output_w==_linux_serial_pt_output_r)
      return 0;
   else
      return -1;
}

/*-------------------------------------------
| Name:dev_linux_serial_pt_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_serial_pt_read(desc_t desc, char* buf,int size)
{
   int cb = 0;
   while(_linux_serial_pt_input_r != _linux_serial_pt_input_w) {
      buf[cb++] = serial_pt_rcv_buffer[_linux_serial_pt_input_r];
      _linux_serial_pt_input_r = (_linux_serial_pt_input_r+1)&(~SERIAL_PT_INPUT_BUFFER_SIZE);
      if(cb == size) break;
   }
   return cb;
}

/*-------------------------------------------
| Name:dev_linux_serial_pt_rcv
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void dev_linux_serial_pt_rcv(void)
{
   volatile int cb = 0;
   if(_linux_serial_pt_input_w == _linux_serial_pt_input_r) {
      rcv_flag=1;
   }
   //get data from shared memory
   while(serial_pt_data->size_in--) {
      serial_pt_rcv_buffer[_linux_serial_pt_input_w] = serial_pt_data->data_in[cb++];
      _linux_serial_pt_input_w = (_linux_serial_pt_input_w+1)&(~SERIAL_PT_INPUT_BUFFER_SIZE);
   }
}

/*-------------------------------------------
| Name:dev_linux_serial_pt_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_serial_pt_write(desc_t desc, const char* buf,int size){
   //variables pour les signaux

   if(size>=SERIAL_PT_OUTPUT_BUFFER_SIZE)
      size = SERIAL_PT_OUTPUT_BUFFER_SIZE-1;

   //copy data directly on shared memory
   serial_pt_data->size_out = size;
   memcpy(serial_pt_data->data_out, buf, serial_pt_data->size_out);

   _linux_serial_pt_output_r = 0;
   _linux_serial_pt_output_w = size;

   //send order to write data on hardware serial port
   serial_pt_cmd.hdwr_id = SERIAL_PT;
   serial_pt_cmd.cmd = OPS_WRITE;

   //disable IT
   //__clr_irq();
   //
   while(cyg_hal_sys_write(1, (void *)&serial_pt_cmd, sizeof(virtual_cmd_t)) !=sizeof(virtual_cmd_t)) ;
   while(cyg_hal_sys_read(0, (void *)&serial_pt_cmd, sizeof(virtual_cmd_t)) !=sizeof(virtual_cmd_t)) ;

   //Enable all IT
   //__set_irq();
   return size;
}

/*-------------------------------------------
| Name:dev_linux_serial_pt_snd
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void dev_linux_serial_pt_snd(void)
{
   _linux_serial_pt_output_r = _linux_serial_pt_output_w;
}

/*-------------------------------------------
| Name:dev_linux_serial_pt_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_serial_pt_seek(desc_t desc,int offset,int origin){
   return 0;
}

/*-------------------------------------------
| Name:dev_linux_serial_pt_ioctl
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_serial_pt_ioctl(desc_t desc,int request,va_list ap)
{
   struct termios* termios_p = (struct termios*)0;

   switch(request) {
   //
   case TIOCSSERIAL: {
      unsigned long speed  = va_arg( ap, unsigned long);
      if(speed<0)
         return -1;

      //copy the opt number and the speed
      memcpy((void *)serial_pt_data->data_ioctl,(void *)&request, sizeof(int));
      memcpy((void *)(serial_pt_data->data_ioctl+sizeof(int)),(void *)&speed, sizeof(unsigned long));
      //prepare cmd
      serial_pt_cmd.hdwr_id = SERIAL_PT;
      serial_pt_cmd.cmd = OPS_IOCTL;
      //
      //disable IT
      //__clr_irq();
      while(cyg_hal_sys_write(1, (void *)&serial_pt_cmd,
                              sizeof(virtual_cmd_t)) != sizeof(virtual_cmd_t)) ;
      while(cyg_hal_sys_read(0, (void *)&serial_pt_cmd,
                             sizeof(virtual_cmd_t)) != sizeof(virtual_cmd_t)) ;
      //enable IT
      //__set_irq();

   } break;

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

   //
   default:
      return -1;

   }

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
static int termios2ttys(struct termios* termios_p)
{
   const struct s2s *sp;
   long n_speed;
   speed_t speed;

   //xon/xoff
   if((termios_p->c_iflag&IXON) != (ttys_termios.c_iflag&IXON)) {
      if(termios_p->c_iflag&IXON) {
         termios_p->c_iflag |= (IXOFF|IXON);
         _xonoff_option = 1;
      }else{
         termios_p->c_iflag &= ~(IXOFF|IXON);
         _xonoff_option = 0;

      }
   }else if((termios_p->c_iflag&IXOFF) != (ttys_termios.c_iflag&IXOFF)) {
      if(termios_p->c_iflag&IXOFF) {
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
      if (sp->ts == speed) {
         unsigned int div;
         unsigned int prescaler=0;
         n_speed = sp->ns;
         //tset speed
         //send_order_w('W', US_BRGR, __KERNEL_CPU_FREQ / n_speed / 16); // Set baud rate.
         break;
      }
   }
   //
   memcpy(&ttys_termios,termios_p,sizeof(struct termios));
   return 0;
}

/*============================================
| End of Source  : dev_linux_serial_pt.c
|---------------------------------------------
| Historic:
|---------------------------------------------
| Authors     | Date     | Comments
| $Log: not supported by cvs2svn $
| Revision 1.1  2010/01/07 14:01:27  jjp
| add virtual pseudo-terminal device for synthetic target
|
|---------------------------------------------
==============================================*/
