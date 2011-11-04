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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <termios.h>

#include <sys/shm.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "virtual_rtu_0.h"
#include "virtual_hardware.h"
#include "virtual_cpu.h"
#include "virtual_cpu_gui.h"
#include "virtual_ioctl.h"

#ifndef  DEV_RTU0
#define  DEV_RTU0  "/dev/ttyS0"
#endif

char rtu0_name[] = DEV_RTU0;

int virtual_rtu0_load(void * data);
int virtual_rtu0_open(void * data);
int virtual_rtu0_close(void * data);
int virtual_rtu0_read(void * data);
int virtual_rtu0_write(void * data);
int virtual_rtu0_seek(void * data);
int virtual_rtu0_ioctl(void * data);

hdwr_info_t virtual_rtu0 = {
   rtu0_name,
   __fdev_not_fd,
   virtual_rtu0_load,
   virtual_rtu0_open,
   virtual_rtu0_close,
   virtual_rtu0_read,
   virtual_rtu0_write,
   virtual_rtu0_seek,
   virtual_rtu0_ioctl
};

//
static virtual_serial_t * rtu_0_data;
//
int virtual_rtu0_load(void * data) {
   virtual_cpu_t * vcpu = (virtual_cpu_t *)data;
   //put data pointer in write place
   rtu_0_data = (virtual_serial_t *)(vcpu->shm_base_addr + RTU0_OFFSET);
   DEBUG_TRACE("(F) %s load ok..\n", virtual_rtu0.name);
   return 0;
}

//
int virtual_rtu0_open(void * data) {
   virtual_cmd_t cmd={RTU_0, OPS_OPEN};
   struct termios options;

   //descriptor and memory are already available
   if(virtual_rtu0.fd>0)  {
      while(write(1, (void *)&cmd, sizeof(virtual_cmd_t)) !=sizeof(virtual_cmd_t));
      DEBUG_TRACE("(F) Already open %s [%d:%d]\n", virtual_rtu0.name, cmd.hdwr_id, cmd.cmd);
      return 0;
   }

   //try to open rtu descriptor
   if((virtual_rtu0.fd = open(virtual_rtu0.name, O_RDWR| O_NONBLOCK))<0) {
      while(write(1, (void *)&cmd, sizeof(virtual_cmd_t)) !=sizeof(virtual_cmd_t));
      DEBUG_TRACE("(F) Can't open %s\n", virtual_rtu0.name);
      perror("open");
      return -1;
   }
   //
   fcntl(virtual_rtu0.fd, F_SETFL, 0);

   /* get the current options */
   tcgetattr(virtual_rtu0.fd, &options);

   cfsetispeed(&options, B9600);
   cfsetospeed(&options, B9600);

   /* set raw input, 1 second timeout */
   options.c_cflag     |= (CLOCAL | CREAD);
   options.c_iflag = 0;
   options.c_oflag = 0;
   options.c_lflag = 0;

   options.c_cc[VMIN]  = 0;
   options.c_cc[VTIME] = 0;

   /* set the options */
   if (tcflush(virtual_rtu0.fd, TCIFLUSH)) {
      DEBUG_TRACE("tcflush() failed.\n");
      return -1;
   }
   if (tcsetattr(virtual_rtu0.fd, TCSANOW, &options)) {
      DEBUG_TRACE("tcsetattr() failed.\n");
      return -1;
   }


   while(write(1, (void *)&cmd, sizeof(virtual_cmd_t)) !=sizeof(virtual_cmd_t));
   DEBUG_TRACE("(F) %d open rtu0 ok..\n", virtual_rtu0.fd);

   return 0;
}

//
int virtual_rtu0_close(void * data) {
   virtual_cmd_t cmd={RTU_0, OPS_CLOSE};
   //close
   DEBUG_TRACE("(F) virtual_rtu0_close\n");
   close(virtual_rtu0.fd);
   //
   while(write(1, (void *)&cmd, sizeof(virtual_cmd_t)) !=sizeof(virtual_cmd_t));
   return 0;
}

//
int virtual_rtu0_read(void * data) {
   virtual_cmd_t cmd={SERIAL_0, OPS_READ};
   virtual_cpu_t * vcpu = (virtual_cpu_t *)data;
   //
//   if((rtu_0_data->size_in = read(virtual_rtu0.fd, (void *)rtu_0_data->data_in, SHM_SERIAL_SIZE)) < 0) {
   if((rtu_0_data->size_in = read(virtual_rtu0.fd, (void *)rtu_0_data->data_in, 1)) < 0) {
      return -1;
   }

   kill(getppid(), SIGIO);
   while(write(vcpu->app2synth, (void *)&cmd, sizeof(virtual_cmd_t)) !=sizeof(virtual_cmd_t));
   //
   DEBUG_TRACE("(F) vr0 [%d] 0x%x\n", rtu_0_data->size_in, rtu_0_data->data_in[0]);
   //
   while(read(vcpu->synth2app, (void *)&cmd, sizeof(virtual_cmd_t)) !=sizeof(virtual_cmd_t));
   //
   //DEBUG_TRACE("(F) vr0:[%d-%d]\n", cmd.hdwr_id, cmd.cmd);
   return 0;
}

//
int virtual_rtu0_write(void * data) {
   //read shared memory and write it on rtu port
   virtual_cmd_t cmd={SERIAL_0, ACK};
   virtual_cpu_t * vcpu = (virtual_cpu_t *)data;
   //
   write(virtual_rtu0.fd, (const void *)rtu_0_data->data_out, rtu_0_data->size_out);
   DEBUG_TRACE("(F)********[%d]*******\n", rtu_0_data->size_out);
   //
   while(write(1, (void *)&cmd, sizeof(virtual_cmd_t)) !=sizeof(virtual_cmd_t));
   cmd.cmd =OPS_WRITE;
   cmd.hdwr_id=SERIAL_0;
   //
   kill(getppid(), SIGIO);
   //manage IRQ
   //DEBUG_TRACE("(F) vr0 1\n");
   while(write(vcpu->app2synth, (void *)&cmd, sizeof(virtual_cmd_t)) !=sizeof(virtual_cmd_t));
   //DEBUG_TRACE("(F) vr0 2\n");
   //kill(getppid(), SIGIO);
   while(read(vcpu->synth2app, (void *)&cmd, sizeof(virtual_cmd_t)) !=sizeof(virtual_cmd_t));
   //
   DEBUG_TRACE("(F)++++++++\n[%d-%d]\n", cmd.hdwr_id, cmd.cmd);
   return 0;
}

//nothing to do
int virtual_rtu0_seek(void * data) {
   return 0;
}

//nothing to do
int virtual_rtu0_ioctl(void * data) {
   virtual_cmd_t cmd={RTU_0, ACK};
   //get data from shared memory
   int request;
   struct termios options;

   memcpy((void *)&request, (void *)rtu_0_data->data_ioctl, sizeof(int));
   DEBUG_TRACE("request : %d\n", request);
   switch(request) {
   case V_TIOCSSERIAL :
   {
      //get speed
      unsigned long speed;
      memcpy((void *)&speed, (void *)(rtu_0_data->data_ioctl+sizeof(int)), sizeof(unsigned long));
      DEBUG_TRACE("speed : %d\n", (int)speed);
      //get the current options
      tcgetattr(virtual_rtu0.fd, &options);
      //set speed
      cfsetispeed(&options, (speed_t)speed);
      cfsetospeed(&options, (speed_t)speed);
      //
      tcsetattr(virtual_rtu0.fd, TCSANOW, &options);
   }
   break;

   default:
      DEBUG_TRACE("default\n");
   break;
   }
   //
   while(write(1, (void *)&cmd, sizeof(virtual_cmd_t)) !=sizeof(virtual_cmd_t));

   return 0;
}

