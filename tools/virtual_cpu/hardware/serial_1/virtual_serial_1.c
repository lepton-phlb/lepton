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

#include "virtual_serial_1.h"
#include "virtual_hardware.h"
#include "virtual_cpu.h"
#include "virtual_cpu_gui.h"
#include "virtual_ioctl.h"

#ifndef  DEV_TTYS1
#define  DEV_TTYS1  "/dev/ttyS1"
#endif

char serial1_name[] = DEV_TTYS1;

int virtual_serial1_load(void * data);
int virtual_serial1_open(void * data);
int virtual_serial1_close(void * data);
int virtual_serial1_read(void * data);
int virtual_serial1_write(void * data);
int virtual_serial1_seek(void * data);
int virtual_serial1_ioctl(void * data);

hdwr_info_t virtual_serial1 = {
   serial1_name,
   __fdev_not_fd,
   virtual_serial1_load,
   virtual_serial1_open,
   virtual_serial1_close,
   virtual_serial1_read,
   virtual_serial1_write,
   virtual_serial1_seek,
   virtual_serial1_ioctl
};

//
static virtual_serial_t * serial_1_data;
//
int virtual_serial1_load(void * data) {
   virtual_cpu_t * vcpu = (virtual_cpu_t *)data;
   //put data pointer in write place
   serial_1_data = (virtual_serial_t *)(vcpu->shm_base_addr + TTYS1_OFFSET);
   DEBUG_TRACE("(F) %s load ok..\n", virtual_serial1.name);
   return 0;
}

//
int virtual_serial1_open(void * data) {
   virtual_cmd_t cmd={SERIAL_1, OPS_OPEN};
   struct termios options;

   //descriptor and memory are already available
   if(virtual_serial1.fd>0)  {
      while(write(1, (void *)&cmd, sizeof(virtual_cmd_t)) !=sizeof(virtual_cmd_t));
      DEBUG_TRACE("(F) Already open %s [%d:%d]\n", virtual_serial1.name, cmd.hdwr_id, cmd.cmd);
      return 0;
   }

   //try to open serial descriptor
   if((virtual_serial1.fd = open(virtual_serial1.name, O_RDWR| O_NONBLOCK))<0) {
      while(write(1, (void *)&cmd, sizeof(virtual_cmd_t)) !=sizeof(virtual_cmd_t));
      DEBUG_TRACE("(F) Can't open %s\n", virtual_serial1.name);
      perror("open");
      return -1;
   }
   //
   fcntl(virtual_serial1.fd, F_SETFL, 0);

   /* get the current options */
   tcgetattr(virtual_serial1.fd, &options);

//   cfsetispeed(&options, B115200);
//   cfsetospeed(&options, B115200);
   cfsetispeed(&options, B57600);
   cfsetospeed(&options, B57600);
//   cfsetispeed(&options, B19200);
//   cfsetospeed(&options, B19200);

   /* set raw input, 1 second timeout */
   options.c_cflag     |= (CLOCAL | CREAD);
   options.c_iflag = 0;
   options.c_oflag = 0;
   options.c_lflag = 0;

   //débloque sur 255 caractères ou au bout de 100ms
   options.c_cc[VMIN]  = 255;//0;
   options.c_cc[VTIME] = 1;//10;

   /* set the options */
   if (tcflush(virtual_serial1.fd, TCIFLUSH)) {
      DEBUG_TRACE("tcflush() failed.\n");
      return -1;
   }
   if (tcsetattr(virtual_serial1.fd, TCSANOW, &options)) {
      DEBUG_TRACE("tcsetattr() failed.\n");
      return -1;
   }


   while(write(1, (void *)&cmd, sizeof(virtual_cmd_t)) !=sizeof(virtual_cmd_t));
   DEBUG_TRACE("(F) %d open serial1 ok..\n", virtual_serial1.fd);

   return 0;
}

//
int virtual_serial1_close(void * data) {
   virtual_cmd_t cmd={SERIAL_1, OPS_CLOSE};
   //close
   DEBUG_TRACE("(F) virtual_serial1_close\n");
   close(virtual_serial1.fd);
   //
   while(write(1, (void *)&cmd, sizeof(virtual_cmd_t)) !=sizeof(virtual_cmd_t));
   return 0;
}

//
int virtual_serial1_read(void * data) {
   virtual_cmd_t cmd={SERIAL_1, OPS_READ};
   virtual_cpu_t * vcpu = (virtual_cpu_t *)data;
   //
   if((serial_1_data->size_in = read(virtual_serial1.fd, (void *)serial_1_data->data_in, SHM_SERIAL_SIZE)) < 0) {
      DEBUG_TRACE("(F) vs1 error\n");
      return -1;
   }
   //
   DEBUG_TRACE("(F) vs1 (r) : %d\n", serial_1_data->size_in);
   //
   kill(getppid(), SIGIO);
   DEBUG_TRACE("(F) vs1 1\n");
   while(write(vcpu->app2synth, (void *)&cmd, sizeof(virtual_cmd_t)) !=sizeof(virtual_cmd_t));
   DEBUG_TRACE("(F) vs1 2\n");
   while(read(vcpu->synth2app, (void *)&cmd, sizeof(virtual_cmd_t)) !=sizeof(virtual_cmd_t));
   //
   DEBUG_TRACE("(F) vs1 (r):[%d-%d]\n", cmd.hdwr_id, cmd.cmd);
   return 0;
}

//
int virtual_serial1_write(void * data) {
   //read shared memory and write it on serial port
   virtual_cmd_t cmd={SERIAL_1, ACK};
   virtual_cpu_t * vcpu = (virtual_cpu_t *)data;
   //
   write(virtual_serial1.fd, (const void *)serial_1_data->data_out, serial_1_data->size_out);
   DEBUG_TRACE("(F)********[%d]*******\n", serial_1_data->size_out);
   //
   while(write(1, (void *)&cmd, sizeof(virtual_cmd_t)) !=sizeof(virtual_cmd_t));
   cmd.cmd =OPS_WRITE;
   cmd.hdwr_id=SERIAL_1;
   //
   kill(getppid(), SIGIO);
   //manage IRQ
   while(write(vcpu->app2synth, (void *)&cmd, sizeof(virtual_cmd_t)) !=sizeof(virtual_cmd_t));
   //kill(getppid(), SIGIO);
   while(read(vcpu->synth2app, (void *)&cmd, sizeof(virtual_cmd_t)) !=sizeof(virtual_cmd_t));
   //
   DEBUG_TRACE("(F)++++++++\n[%d-%d]\n", cmd.hdwr_id, cmd.cmd);
   return 0;
}

//nothing to do
int virtual_serial1_seek(void * data) {
   return 0;
}

//nothing to do
int virtual_serial1_ioctl(void * data) {
   virtual_cmd_t cmd={SERIAL_1, ACK};
   //get data from shared memory
   int request;
   struct termios options;

   memcpy((void *)&request, (void *)serial_1_data->data_ioctl, sizeof(int));
   DEBUG_TRACE("request : %d\n", request);
   switch(request) {
   case V_TIOCSSERIAL :
   {
      //get speed
      unsigned long speed;
      memcpy((void *)&speed, (void *)(serial_1_data->data_ioctl+sizeof(int)), sizeof(unsigned long));
      DEBUG_TRACE("speed : %d\n", (int)speed);
      //get the current options
      tcgetattr(virtual_serial1.fd, &options);
      //set speed
      cfsetispeed(&options, (speed_t)speed);
      cfsetospeed(&options, (speed_t)speed);
      //
      tcsetattr(virtual_serial1.fd, TCSANOW, &options);
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
