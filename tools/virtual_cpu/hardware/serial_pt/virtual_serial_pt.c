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


#include "virtual_serial_pt.h"
#include "virtual_hardware.h"
#include "virtual_cpu.h"
#include "virtual_cpu_gui.h"
#include "virtual_ioctl.h"

#define SLAVE_PATH   32

char serialpt_name[] = "/tmp/tauonpt\0";
char serialpt_real_name[] = "/dev/ptmx\0";
//char serialpt_name[] = "/dev/ptmx\0";

int virtual_serialpt_load(void * data);
int virtual_serialpt_open(void * data);
int virtual_serialpt_close(void * data);
int virtual_serialpt_read(void * data);
int virtual_serialpt_write(void * data);
int virtual_serialpt_seek(void * data);
int virtual_serialpt_ioctl(void * data);

hdwr_info_t virtual_serialpt = {
   serialpt_name,
   __fdev_not_fd,
   virtual_serialpt_load,
   virtual_serialpt_open,
   virtual_serialpt_close,
   virtual_serialpt_read,
   virtual_serialpt_write,
   virtual_serialpt_seek,
   virtual_serialpt_ioctl
};

//headers and define give warnings
extern int getpt(void);
extern int grantpt(int fd);
extern int unlockpt(int fd);
extern char *ptsname_r(int fd, char *buf, size_t buflen);

//
static virtual_serial_t * serial_pt_data;
//
int virtual_serialpt_load(void * data) {
   virtual_cpu_t * vcpu = (virtual_cpu_t *)data;
   //put data pointer in write place
   serial_pt_data = (virtual_serial_t *)(vcpu->shm_base_addr + TTYPT_OFFSET);
   fprintf(stderr, "(F) %s load ok..\n", virtual_serialpt.name);
   return 0;
}

//
int virtual_serialpt_open(void * data) {
   virtual_cmd_t cmd={SERIAL_PT, OPS_OPEN};
   struct termios options;
   char slave_path[SLAVE_PATH]={0};
   int ret=-1;

   //descriptor and memory are already available
   if(virtual_serialpt.fd>0)  {
      while(write(1, (void *)&cmd, sizeof(virtual_cmd_t)) !=sizeof(virtual_cmd_t));
      DEBUG_TRACE("(F) Already open %s [%d:%d]\n", virtual_serialpt.name, cmd.hdwr_id, cmd.cmd);
      return 0;
   }

   //remove symlink from fs
   ret = remove(virtual_serialpt.name);
   perror("remove");
   DEBUG_TRACE("(F) remove %s RET : %d\n", virtual_serialpt.name, ret);
   //try to open pseudo terminal
   if((virtual_serialpt.fd = open(serialpt_real_name/*virtual_serialpt.name*/, O_RDWR|O_NOCTTY|O_NONBLOCK))<0) {
      while(write(1, (void *)&cmd, sizeof(virtual_cmd_t)) !=sizeof(virtual_cmd_t));
      DEBUG_TRACE("(F) Can't open %s\n", virtual_serialpt.name);
      perror("open");
      return -1;
   }
   //
   grantpt(virtual_serialpt.fd);
   unlockpt(virtual_serialpt.fd);
   //get slave pts name
   if(ptsname_r(virtual_serialpt.fd, slave_path, SLAVE_PATH-1)) {
      while(write(1, (void *)&cmd, sizeof(virtual_cmd_t)) !=sizeof(virtual_cmd_t));
      //
      perror("ptsname_r");
      return -1;
   }

   //make link between dynamic slave name and generic symlink
   ret = symlink(slave_path, virtual_serialpt.name);
   //
   DEBUG_TRACE("(F) ECHO your command to %s\n", slave_path);
   //
   fcntl(virtual_serialpt.fd, F_SETFL, 0);

   //get the current options
   tcgetattr(virtual_serialpt.fd, &options);

/*   cfsetispeed(&options, B115200);
   cfsetospeed(&options, B115200);

   //set raw input, 1 second timeout
   options.c_cflag     |= (CLOCAL | CREAD);
   options.c_lflag     &= ~(ICANON | ECHO | ECHOE | ISIG);
   options.c_oflag     &= ~OPOST;
   options.c_cc[VMIN]  = 0;
   options.c_cc[VTIME] = 0;
   */
   cfmakeraw(&options);
   //options.c_cc[VMIN]  = 0;
   //débloque au bout de 256 caractères ou au bout de 100ms
   options.c_cc[VMIN]  = 255;//0;
   options.c_cc[VTIME] = 1;//10;

   //set the options
   tcsetattr(virtual_serialpt.fd, TCSANOW, &options);

   while(write(1, (void *)&cmd, sizeof(virtual_cmd_t)) !=sizeof(virtual_cmd_t));
   DEBUG_TRACE("(F) %d open ok..\n", virtual_serialpt.fd);

   return 0;
}

//
int virtual_serialpt_close(void * data) {
   virtual_cmd_t cmd={SERIAL_PT, OPS_CLOSE};
   //close
   DEBUG_TRACE("(F) virtual_serialpt_close\n");
   close(virtual_serialpt.fd);
   //
   while(write(1, (void *)&cmd, sizeof(virtual_cmd_t)) !=sizeof(virtual_cmd_t));
   return 0;
}

//
int virtual_serialpt_read(void * data) {
   virtual_cmd_t cmd={SERIAL_PT, OPS_READ};
   virtual_cpu_t * vcpu = (virtual_cpu_t *)data;
   //
   if((serial_pt_data->size_in = read(virtual_serialpt.fd, (void *)serial_pt_data->data_in, SHM_SERIAL_SIZE)) < 0) {
      return -1;
   }

   DEBUG_TRACE("(F) virtual_serialpt_read : %d\n", serial_pt_data->size_in);
   //
   kill(getppid(), SIGIO);
   while(write(vcpu->app2synth, (void *)&cmd, sizeof(virtual_cmd_t)) !=sizeof(virtual_cmd_t));
   while(read(vcpu->synth2app, (void *)&cmd, sizeof(virtual_cmd_t)) !=sizeof(virtual_cmd_t));
   //
   return 0;
}

//
int virtual_serialpt_write(void * data) {
   //read shared memory and write it on serial port
   virtual_cmd_t cmd={SERIAL_PT, ACK};
   virtual_cpu_t * vcpu = (virtual_cpu_t *)data;
   //
   write(virtual_serialpt.fd, (const void *)serial_pt_data->data_out, serial_pt_data->size_out);
   //
   while(write(1, (void *)&cmd, sizeof(virtual_cmd_t)) !=sizeof(virtual_cmd_t));

   //DEBUG_TRACE("(F) virtual_serialpt_write %s [%d]\n", serial_pt_data->data_out, serial_pt_data->size_out);
   cmd.cmd =OPS_WRITE;
   cmd.hdwr_id=SERIAL_PT;
   //manage IRQ
   kill(getppid(), SIGIO);
   while(write(vcpu->app2synth, (void *)&cmd, sizeof(virtual_cmd_t)) !=sizeof(virtual_cmd_t));
   while(read(vcpu->synth2app, (void *)&cmd, sizeof(virtual_cmd_t)) !=sizeof(virtual_cmd_t));

   return 0;
}

//nothing to do
int virtual_serialpt_seek(void * data) {
   return 0;
}

//nothing to do
int virtual_serialpt_ioctl(void * data) {
   virtual_cmd_t cmd={SERIAL_PT, ACK};
   //get data from shared memory
   int request;
   struct termios options;

   memcpy((void *)&request, (void *)serial_pt_data->data_ioctl, sizeof(int));
   DEBUG_TRACE("request : %d\n", request);

   switch(request) {
   case V_TIOCSSERIAL :
   {
      //get speed
      unsigned long speed;
      memcpy((void *)&speed, (void *)(serial_pt_data->data_ioctl+sizeof(int)), sizeof(unsigned long));
      DEBUG_TRACE("speed : %d\n", (int)speed);
      //get the current options
      tcgetattr(virtual_serialpt.fd, &options);
      //set speed
      cfsetispeed(&options, (speed_t)speed);
      cfsetospeed(&options, (speed_t)speed);
      //
      tcsetattr(virtual_serialpt.fd, TCSANOW, &options);
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

