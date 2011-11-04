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
#include <stdlib.h>
#include <string.h>

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

#include "virtual_cpu.h"
#include "virtual_hardware_conf.h"

#define     TST_NB_TICKS      20

//

//
extern phdwr_info_t hdwr_lst[];
extern int hdwr_max_dev;
//

#define RFDS_SIZE    hdwr_max_dev
int set_watch_fd(fd_set *rfd);
///

int decode_cmd(virtual_cmd_t *c);
//
static int nb_sig=0;
////////////////////////////////////////////////////////

//call all load hardware functions
int init_hardware(void * arg) {
   int i=0;
   //

   for(;i<hdwr_max_dev;i++) {
      hdwr_lst[i]->load(arg);
   }

   return 0;
}

//
int set_watch_fd(fd_set *rfd) {
   int i = 1;//because hdwr id 0 is clock
   int max_fd = MAX(0, 0);
   int tmp = 0;

   //set pipe read descriptor
   FD_SET(0, rfd);

   //set hdwr descriptor
   for(;i<RFDS_SIZE;i++) {
      if(hdwr_lst[i]->fd<0) continue;
      //
      FD_SET(hdwr_lst[i]->fd, rfd);
      tmp = max_fd;
      max_fd = MAX(tmp, hdwr_lst[i]->fd);
   }
   return max_fd;
}

//
int decode_cmd(virtual_cmd_t *c) {
   DEBUG_TRACE("\ndecode_cmd => id :%d, func :%d\n", c->hdwr_id, c->cmd);
   if(c->hdwr_id<0) {
      DEBUG_TRACE("\n***** ILLEGAL *********\n");
      return 0;
   }

   switch(c->cmd) {
   case OPS_OPEN:
      hdwr_lst[c->hdwr_id]->open(NULL);
   break;

   case OPS_CLOSE:
      hdwr_lst[c->hdwr_id]->close(NULL);
   break;

   case OPS_READ:
      hdwr_lst[c->hdwr_id]->read(NULL);
      nb_sig++;
   break;

   case OPS_WRITE:
      hdwr_lst[c->hdwr_id]->write(NULL);
      nb_sig++;
   break;

   case OPS_SEEK:
      hdwr_lst[c->hdwr_id]->seek(NULL);
   break;

   case OPS_IOCTL:
      hdwr_lst[c->hdwr_id]->ioctl(NULL);
   break;

   default:
      DEBUG_TRACE("default[%d:%d]\n",c->hdwr_id, c->cmd);
   break;
   }
   //
   DEBUG_TRACE("\n\tNB_SIG(R/W):%d\n", nb_sig);
   //
   return 0;
}

//
int main(int argc, char *argv[]){
   void * data;
   int retval = -1;
   int max_fd = -1;
   int i=1;
   virtual_cmd_t cmd={0,0};
   int rc=1;
   //get a tab of fd_set same sizo of hardware size
   fd_set rfds_in;
   //
   //call all hardware load function
   init_hardware(data);
   //

   //wake dad all stuff are good
   while(write(1, &cmd, sizeof(virtual_cmd_t)) != sizeof(virtual_cmd_t));
   while(read(0, &cmd, sizeof(virtual_cmd_t)) != sizeof(virtual_cmd_t));
   //
   while(1) {
      FD_ZERO(&rfds_in);
      max_fd = set_watch_fd(&rfds_in) + 1;
      //
      retval = select(max_fd, &rfds_in, NULL, NULL, NULL);

      //because of interrupt system call
      if(retval<=0) {
         DEBUG_TRACE("\n--\n");
         continue;
      }
      //test descriptors which have move
      else if(retval>0) {
         //pipe descriptor
         if(FD_ISSET(0, &rfds_in)) {
            //read command and exectute it
            while(read(0, (void *)&cmd, sizeof(virtual_cmd_t)) != sizeof(virtual_cmd_t));
            //decode cmd
            decode_cmd(&cmd);
         }
         //hardware descriptor
         for(i=1;i<RFDS_SIZE;i++) {
            if(hdwr_lst[i]->fd<0)   continue;
            //fprintf(stderr, "fd:%d-hdwr_id:%d\n", hdwr_lst[i]->fd, i);
            //
            if(FD_ISSET(hdwr_lst[i]->fd, &rfds_in)) {
               //fprintf(stderr, "fd:%d-hdwr_id:%d\tNB_SIG(R/W):%d\n", hdwr_lst[i]->fd, i, ++nb_sig);
               hdwr_lst[i]->read(NULL);
            }
         }
      }
   }
   //
   return 0;
}

