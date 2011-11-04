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

#include <unistd.h>
#include <signal.h>
#include <sys/time.h>

#include "virtual_hardware.h"
#include "virtual_clock.h"

char clock_name[] = "clock0\0";

int virtual_clock_load(void * data);
int virtual_clock_open(void * data);
int virtual_clock_close(void * data);
int virtual_clock_read(void * data);
int virtual_clock_write(void * data);
int virtual_clock_seek(void * data);
int virtual_clock_ioctl(void * data);

hdwr_info_t virtual_clock = {
   clock_name,
   __fdev_not_fd,
   virtual_clock_load,
   virtual_clock_open,
   virtual_clock_close,
   virtual_clock_read,
   virtual_clock_write,
   virtual_clock_seek,
   virtual_clock_ioctl
};

//
void virtual_clock_it(int sig);

//configure SIGALRM and lauch it
int virtual_clock_load(void * data) {
   struct sigaction a;
   //struct itimerval val;

   //configure signal SIGALRM handler
   memset((void *)&a, 0, sizeof(struct sigaction));
   a.sa_handler = virtual_clock_it;
   a.sa_flags = SA_RESTART;
   a.sa_restorer = (void (*)(void)) 0;
   if(sigaction(SIGALRM, &a, NULL) < 0) {
      return -1;
   }

   //set soft timer
   /*val.it_interval.tv_sec = 0;
   val.it_interval.tv_usec = VIRTUAL_TICKS;
   val.it_value.tv_sec = 0;
   val.it_value.tv_usec = VIRTUAL_TICKS;
   setitimer(ITIMER_REAL, &val, NULL);*/

   //
   fprintf(stderr, "(F) %s load ok..\n", virtual_clock.name);
   return 0;
}

//send signal to dad
void virtual_clock_it(int sig) {
   //fprintf(stderr, "(F) SIGALRM\n");
   kill(getppid(), SIGALRM);
}

//nothing to do
int virtual_clock_open(void * data) {
   return 0;
}

//nothing to do
int virtual_clock_close(void * data) {
   return 0;
}

//nothing to do
int virtual_clock_read(void * data) {
   return 0;
}

//nothing to do
int virtual_clock_write(void * data) {
   return 0;
}

//nothing to do
int virtual_clock_seek(void * data) {
   return 0;
}

//nothing to do
int virtual_clock_ioctl(void * data) {
   return 0;
}
