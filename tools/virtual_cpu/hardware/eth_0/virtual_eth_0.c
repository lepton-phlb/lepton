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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

#include <sys/shm.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <time.h>
#include <sys/time.h>

#include <netinet/in.h>
#include <net/ethernet.h>
#include <netinet/ether.h>

//
#include <netpacket/packet.h>
#include <net/if.h>
#include <linux/if_tun.h>

//
#include "virtual_eth_0.h"
#include "virtual_hardware.h"
#include "virtual_cpu.h"
#include "virtual_cpu_gui.h"

//
#define ETH_FRAME       1600
#define ETH_WR_OFF      SHM_ETH_0_SIZE/2
#define ETH_HDR_OFF     14

char eth0_name[] = "eth0\0";

int virtual_eth0_load(void * data);
int virtual_eth0_open(void * data);
int virtual_eth0_close(void * data);
int virtual_eth0_read(void * data);
int virtual_eth0_write(void * data);
int virtual_eth0_seek(void * data);
int virtual_eth0_ioctl(void * data);

hdwr_info_t virtual_eth0 = {
   eth0_name,
   __fdev_not_fd,
   virtual_eth0_load,
   virtual_eth0_open,
   virtual_eth0_close,
   virtual_eth0_read,
   virtual_eth0_write,
   virtual_eth0_seek,
   virtual_eth0_ioctl
};

//
static virtual_eth_t * eth_0_data;
static int nb_read = 0;
//
//try allocate shared memory for data
int virtual_eth0_load(void * data) {
   virtual_cpu_t * vcpu = (virtual_cpu_t *)data;
   //put data pointer in write place
   eth_0_data = (virtual_eth_t *)(vcpu->shm_base_addr + ETH0_OFFSET);
   DEBUG_TRACE("(F) %s load ok..\n", virtual_eth0.name);
   return 0;
}

//
int virtual_eth0_open(void * data) {
   virtual_cmd_t cmd={ETH_0, OPS_OPEN};
   struct ifreq ifr;

   //
   if(virtual_eth0.fd>0) {
      DEBUG_TRACE("(F) socket already open[%d:%d]\n",cmd.hdwr_id, cmd.cmd);
      while(write(1, (void *)&cmd, sizeof(virtual_cmd_t)) !=sizeof(virtual_cmd_t)) ;
      return 0;
   }
   //
   if( (virtual_eth0.fd = open("/dev/net/tun", O_RDWR)) < 0 ) {
      perror("open");
      //write cmd to unblock client
      while(write(1, (void *)&cmd, sizeof(virtual_cmd_t)) !=sizeof(virtual_cmd_t)) ;
      virtual_eth0.fd = __fdev_not_fd;
      return -1;
   }
   //
   memset(&ifr, 0, sizeof(ifr));
   ifr.ifr_flags = IFF_TAP|IFF_NO_PI;
   strncpy(ifr.ifr_name, "tap0\0", IFNAMSIZ);

   if( ioctl(virtual_eth0.fd, TUNSETIFF, (void *) &ifr) < 0 ) {
      perror("ioctl");
      while(write(1, (void *)&cmd, sizeof(virtual_cmd_t)) !=sizeof(virtual_cmd_t)) ;
      //
      close(virtual_eth0.fd);
      virtual_eth0.fd = __fdev_not_fd;
      return -1;
   }

   if (ioctl(virtual_eth0.fd, TUNSETPERSIST, 1) < 0) {
      perror("ioctl");
      while(write(1, (void *)&cmd, sizeof(virtual_cmd_t)) !=sizeof(virtual_cmd_t)) ;
      //
      close(virtual_eth0.fd);
      virtual_eth0.fd = __fdev_not_fd;
      return -1;
   }

   //
   //
   DEBUG_TRACE("(F) %d [%s] open ok[%d:%d]..\n", virtual_eth0.fd, ifr.ifr_name,
               cmd.hdwr_id, cmd.cmd);

   while(write(1, (void *)&cmd, sizeof(virtual_cmd_t)) !=sizeof(virtual_cmd_t)) ;

   return 0;
}

//
int virtual_eth0_close(void * data) {
   virtual_cmd_t cmd={ETH_0, OPS_CLOSE};
   //close
   DEBUG_TRACE("(F) virtual_eth0_close\n");
   close(virtual_eth0.fd);
   //
   while(write(1, (void *)&cmd, sizeof(virtual_cmd_t)) !=sizeof(virtual_cmd_t)) ;
   //
   return 0;
}

//
int virtual_eth0_read(void * data) {
   virtual_cmd_t cmd={ETH_0, OPS_READ};
   virtual_cpu_t * vcpu = (virtual_cpu_t *)data;
   struct timeval t1,t2;

   //
   gettimeofday(&t1,NULL);
   //
   //read data from socket
   eth_0_data->size_in = read(virtual_eth0.fd, (void *)eth_0_data->data_in, SHM_ETH_SIZE);
   //kill(getppid(), SIGIO);
   while(write(vcpu->app2synth, (void *)&cmd, sizeof(virtual_cmd_t)) !=sizeof(virtual_cmd_t)) ;
   kill(getppid(), SIGIO);
   while(read(vcpu->synth2app, (void *)&cmd, sizeof(virtual_cmd_t)) !=sizeof(virtual_cmd_t)) ;
//
   gettimeofday(&t2,NULL);
   DEBUG_TRACE("(F)-------------[%d-%d]\t%d\telapse:%d s .. %d us\t(%d)\n",
               cmd.hdwr_id, cmd.cmd, nb_read++,
               (int)difftime(t2.tv_sec,t1.tv_sec),(int)difftime(t2.tv_usec,t1.tv_usec)
               , eth_0_data->size_in);
   //
   return 0;
}

//
int virtual_eth0_write(void * data) {
   virtual_cmd_t cmd={ETH_0, ACK};
   virtual_cpu_t * vcpu = (virtual_cpu_t *)data;

   int cb=0;
   struct timeval t1,t2;
   //
   cb = write(virtual_eth0.fd, (const void *)eth_0_data->data_out, eth_0_data->size_out);
   //
   DEBUG_TRACE("\n(F) sendto(): [cb:%d]\n",cb);

   //
   //while(write(1, (void *)&cmd, sizeof(virtual_cmd_t)) !=sizeof(virtual_cmd_t));
   cmd.cmd =OPS_WRITE;
   cmd.hdwr_id=ETH_0;

   gettimeofday(&t1,NULL);
   //kill(getppid(), SIGIO);
   //
   while(write(vcpu->app2synth, (void *)&cmd, sizeof(virtual_cmd_t)) !=sizeof(virtual_cmd_t)) ;
   kill(getppid(), SIGIO);
   while(read(vcpu->synth2app, (void *)&cmd, sizeof(virtual_cmd_t)) !=sizeof(virtual_cmd_t)) ;
   gettimeofday(&t2,NULL);

   DEBUG_TRACE("\n(F)+++[%d-%d]\telapse:%d us\n", cmd.hdwr_id, cmd.cmd,
               (int)difftime(t2.tv_usec,t1.tv_usec));
   return 0;
}

//
int virtual_eth0_seek(void * data) {
   return 0;
}

//
int virtual_eth0_ioctl(void * data) {
   return 0;
}

