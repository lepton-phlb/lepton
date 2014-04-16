/*
The contents of this file are subject to the Mozilla Public License Version 1.1
(the "License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License for the
specific language governing rights and limitations under the License.

The Original Code is Lepton.

The Initial Developer of the Original Code is Chauvin-Arnoux.
Portions created by Chauvin-Arnoux are Copyright (C) 2011. All Rights Reserved.

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
#include "lwip/debug.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/sys.h"
#include "lwip/stats.h"
#include "lwip/tcpip.h"
#include "netif/loopif.h"
#include "kernel/net/lwip/netif/ppp/ppp.h"
#include "arch/perf.h"

#include "kernel/core/types.h"
#include "kernel/core/kernel.h"
#include "kernel/core/system.h"
#include "kernel/fs/vfs/vfskernel.h"
#include "kernel/fs/vfs/vfs.h"
#include "kernel/core/ioctl.h"
#include "kernel/core/ioctl_eth.h"
#include "kernel/core/net/lwip_core/lwip_core.h"
#include "kernel/core/malloc.h"



#include "lib/libc/stdio/stdio.h"

/*============================================
| Global Declaration
==============================================*/

//ARM7
#define LWIP_CORE_STACK_SIZE 4*1024 //9*1024
//m16c
//#define LWIP_CORE_STACK_SIZE 1000

//
#if !defined(GNU_GCC)
   #define LWIP_CORE_PRIORITY   100

#else

   #include <string.h>
   #define LWIP_CORE_PRIORITY   10

#endif


_macro_stack_addr char lwip_core_stack[LWIP_CORE_STACK_SIZE];

kernel_pthread_t lwip_core_thread;

err_t ethif_core_init(struct netif *netif);

extern int ethif_core_periodic_input(struct lwip_if_st *lwip_if_head);


static int dev_lwip_stack_load(void);
static int dev_lwip_stack_open(desc_t desc,int o_flag);
static int dev_lwip_stack_close(desc_t desc);
static int dev_lwip_stack_ioctl(desc_t desc,int request,va_list ap);

static const char dev_lwip_stack_name[]="net/ip";

dev_map_t dev_lwip_stack_map={
   dev_lwip_stack_name,
   S_IFBLK,
   dev_lwip_stack_load,
   dev_lwip_stack_open,
   dev_lwip_stack_close,
   __fdev_not_implemented,
   __fdev_not_implemented,
   __fdev_not_implemented,
   __fdev_not_implemented,
   __fdev_not_implemented,
   dev_lwip_stack_ioctl //ioctl
};

static int g_if_no=0;
static lwip_if_t* g_p_lwip_if_list=(lwip_if_t*)0;


/*============================================
| Implementation
==============================================*/

/*--------------------------------------------
| Name:        tcpip_init_done
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
void tcpip_init_done(void * arg)
{
   sys_sem_t *sem = arg;
   sys_sem_signal(*sem);
}

/*--------------------------------------------
| Name:        lwip_core_routine
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/

typedef void (*__lwip_sys_thread)(void *arg);
typedef __lwip_sys_thread lwip_sys_thread_t;

void* lwip_core_routine(void* arg){

   //struct netif netif;
   struct netif loop_netif={0};
   //struct ip_addr ipaddr, netmask, gw;
   static int inited = 0;
   sys_sem_t sem;
   //struct ip_addr loop_ipaddr, loop_netmask, loop_gw;

   if (inited)
      return (void*)0;
   inited++;


   /*sys_init();	   // eCos specific initialization
   mem_init();	   // heap based memory allocator
   memp_init();   // pool based memory allocator
   pbuf_init();	// packet buffer allocator
   netif_init();	// netif layer
//
//pppInit();
*/

   /* Start the stack.It will spawn a new dedicated thread */
   sem = sys_sem_new(0);
   tcpip_init(tcpip_init_done,&sem);
   sys_sem_wait(sem);
   sys_sem_free(sem);



/*
  IP4_ADDR(&loop_gw, 127,0,0,1);
  IP4_ADDR(&loop_ipaddr, 127,0,0,1);
  IP4_ADDR(&loop_netmask, 255,0,0,0);
  printf("Starting lwIP, loopback interface IP is %s\n", inet_ntoa(*(struct in_addr*)&loop_ipaddr));
  netif_add(&loop_netif, &loop_ipaddr, &loop_netmask, &loop_gw, NULL, loopif_init, tcpip_input);
  netif_set_up(&loop_netif);
*/
/*
//ethernet configuration
//IP4_ADDR(&gw, 14,2,10,1);
//IP4_ADDR(&ipaddr, 14,2,212,126);

//slip configuration
//IP4_ADDR(&gw, 128,1,1,2);
//IP4_ADDR(&ipaddr, 128,1,1,3);

     IP4_ADDR(&netmask, 255,255,0,0);
netif_add(&netif, &ipaddr, &netmask, &gw, NULL, ethif_core_init, tcpip_input);

netif_set_default(&netif);

netif_set_up(&netif);
*/
//
   for(;; ) {
      ethif_core_periodic_input(g_p_lwip_if_list);
   }
   //update_adapter();

   return (void*)0;
}

/*--------------------------------------------
| Name:        lwip_core_run
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int lwip_core_run(void){

   pthread_attr_t thread_attr;

   thread_attr.stacksize = LWIP_CORE_STACK_SIZE;
   thread_attr.stackaddr = (void*)&lwip_core_stack;
   thread_attr.priority  = LWIP_CORE_PRIORITY;
   thread_attr.timeslice = 1;

   thread_attr.name = "lwip_core";

   kernel_pthread_create(&lwip_core_thread,&thread_attr,lwip_core_routine,(char*)0);

   return 0;
}

/*--------------------------------------------
| Name:        add_lwip_if
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int add_lwip_if(lwip_if_t* p_lwip_if){

   lwip_if_t* p_lwip_if_head;
   if(!p_lwip_if)
      return -1;

   p_lwip_if_head = g_p_lwip_if_list;
   g_p_lwip_if_list = p_lwip_if;
   g_p_lwip_if_list->lwip_if_next = p_lwip_if_head;


   return 0;
}

/*--------------------------------------------
| Name:        config_lwip_if
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int config_lwip_if(lwip_if_t* p_lwip_if){

   struct ip_addr if_ipaddr, if_netmask, if_gw;
   int ip_addr_1;
   int ip_addr_2;
   int ip_addr_3;
   int ip_addr_4;

   //interface gateway ip address
   sscanf(p_lwip_if->if_config.gw_ip_addr,"%d.%d.%d.%d",&ip_addr_1,&ip_addr_2,&ip_addr_3,&ip_addr_4);
   IP4_ADDR(&if_gw, ip_addr_1,ip_addr_2,ip_addr_3,ip_addr_4);

   //interface ip address
   sscanf(p_lwip_if->if_config.if_ip_addr,"%d.%d.%d.%d",&ip_addr_1,&ip_addr_2,&ip_addr_3,&ip_addr_4);
   IP4_ADDR(&if_ipaddr, ip_addr_1,ip_addr_2,ip_addr_3,ip_addr_4);

   //interface subnetwork mask
   sscanf(p_lwip_if->if_config.if_net_msk,"%d.%d.%d.%d",&ip_addr_1,&ip_addr_2,&ip_addr_3,&ip_addr_4);
   IP4_ADDR(&if_netmask, ip_addr_1,ip_addr_2,ip_addr_3,ip_addr_4);

   //
   g_if_no++;
   //selfpointer
   p_lwip_if->netif.state = p_lwip_if;
   //
   p_lwip_if->if_config.if_no = g_if_no;

   //add interface
   netif_add(&p_lwip_if->netif, &if_ipaddr, &if_netmask, &if_gw, p_lwip_if, ethif_core_init, tcpip_input);

   if(g_if_no==1)
      netif_set_default(&p_lwip_if->netif);

   netif_set_up(&p_lwip_if->netif);

   //
   add_lwip_if(p_lwip_if);

   return 0;
}

/*--------------------------------------------
| Name:        add_lwip_if
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int remove_lwip_if(lwip_if_t* p_lwip_if){
   return 0;
}

/*--------------------------------------------
| Name:        dev_lwip_stack_load
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int dev_lwip_stack_load(void){
   lwip_core_run();
   return 0;
}

/*--------------------------------------------
| Name:        dev_lwip_stack_open
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int dev_lwip_stack_open(desc_t desc, int o_flag){

   if(ofile_lst[desc].oflag & O_RDONLY) {
   }

   if(ofile_lst[desc].oflag & O_WRONLY) {
   }

   if(!ofile_lst[desc].nb_writer
      &&!ofile_lst[desc].nb_reader) {

      //alloc struct for netif configuration
   }

   return 0;
}

/*--------------------------------------------
| Name:        dev_lwip_stack_close
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int dev_lwip_stack_close(desc_t desc){
   if(ofile_lst[desc].oflag & O_RDONLY) {
      if(!ofile_lst[desc].nb_reader) {

      }
   }

   if(ofile_lst[desc].oflag & O_WRONLY) {
      if(!ofile_lst[desc].nb_writer) {
      }
   }

   if(!ofile_lst[desc].nb_writer
      &&!ofile_lst[desc].nb_reader) {
   }
   return 0;
}

/*--------------------------------------------
| Name:        dev_lwip_stack_ioctl
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int dev_lwip_stack_ioctl(desc_t desc,int request,va_list ap){
   switch(request) {

   case I_LINK: {
      //must be open in O_RDWR mode
      if((ofile_lst[desc].oflag&O_RDWR)!=O_RDWR)
         return -1;

      //alloc for lwip interface information.
      if(!(ofile_lst[desc].p))
         if(!(ofile_lst[desc].p=_sys_malloc(sizeof(lwip_if_t)) ) ) {
            return -1;
         }else{
            ((lwip_if_t*)ofile_lst[desc].p)->desc_r=-1;
            ((lwip_if_t*)ofile_lst[desc].p)->desc_w=-1;
            ((lwip_if_t*)ofile_lst[desc].p)->lwip_if_next=(lwip_if_t*)0;
         }

      //read descriptor
      if(ofile_lst[desc].desc_nxt[0]!=INVALID_DESC && (((lwip_if_t*)ofile_lst[desc].p)->desc_r<0) )
         ((lwip_if_t*)ofile_lst[desc].p)->desc_r = ofile_lst[desc].desc_nxt[0];

      //write descriptor
      if(ofile_lst[desc].desc_nxt[1]!=INVALID_DESC && (((lwip_if_t*)ofile_lst[desc].p)->desc_w<0) )
         ((lwip_if_t*)ofile_lst[desc].p)->desc_w = ofile_lst[desc].desc_nxt[1];

   }
   break;

   case I_UNLINK: {
   }
   break;


   case IFGETCFG: {
      if_config_t* p_if_config= va_arg( ap, if_config_t*);
      if(!p_if_config)
         return -1;
      if(!(ofile_lst[desc].p))
         return -1;
      memcpy(p_if_config,&((lwip_if_t*)ofile_lst[desc].p)->if_config,sizeof(if_config_t));
   }
   break;

   case IFSETCFG: {
      if_config_t* p_if_config= va_arg( ap, if_config_t*);
      if(!p_if_config)
         return -1;
      if(!(ofile_lst[desc].p))
         return -1;
      memcpy(&((lwip_if_t*)ofile_lst[desc].p)->if_config,p_if_config,sizeof(if_config_t));

      config_lwip_if(((lwip_if_t*)ofile_lst[desc].p));
   }
   break;


   //
   default:
      return -1;
   }

   return 0;
}


/*============================================
| End of Source  : lwip_core.c
==============================================*/
