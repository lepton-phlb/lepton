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


/*===========================================
Includes
=============================================*/
#include "kernel/core/kernelconf.h"
#include "kernel/core/types.h"
#include "kernel/core/interrupt.h"
#include "kernel/core/kernel.h"
#include "kernel/core/system.h"
#include "kernel/core/fcntl.h"
#include "kernel/core/cpu.h"
#include "kernel/core/core_rttimer.h"
#include "kernel/fs/vfs/vfsdev.h"
#include "kernel/core/ioctl_eth.h"

#include "kernel/dev/arch/cortexm/stm32f4xx/driverlib/stm32f4xx.h"
#include "kernel/dev/arch/cortexm/stm32f4xx/driverlib/stm32f4x7_eth.h"
#include "kernel/dev/arch/cortexm/stm32f4xx/types.h"
#include "kernel/dev/arch/cortexm/stm32f4xx/target.h"
#include "kernel/dev/arch/cortexm/stm32f4xx/gpio.h"
#include "kernel/dev/arch/cortexm/stm32f4xx/eth.h"


/*===========================================
Global Declaration
=============================================*/

static const char dev_stm32f4xx_eth_name[]="eth0\0";

static int dev_stm32f4xx_eth_load(void);
static int dev_stm32f4xx_eth_open(desc_t desc, int o_flag);
//
static int dev_stm32f4xx_eth_isset_read(desc_t desc);
static int dev_stm32f4xx_eth_isset_write(desc_t desc);
static int dev_stm32f4xx_eth_close(desc_t desc);
static int dev_stm32f4xx_eth_seek(desc_t desc,int offset,int origin);
static int dev_stm32f4xx_eth_read(desc_t desc, char* buf,int cb);
static int dev_stm32f4xx_eth_write(desc_t desc, const char* buf,int cb);
static int dev_stm32f4xx_eth_ioctl(desc_t desc,int request,va_list ap);
static int dev_stm32f4xx_eth_seek(desc_t desc,int offset,int origin);

dev_map_t dev_stm32f4xx_eth_map={
   dev_stm32f4xx_eth_name,
   S_IFCHR,
   dev_stm32f4xx_eth_load,
   dev_stm32f4xx_eth_open,
   dev_stm32f4xx_eth_close,
   dev_stm32f4xx_eth_isset_read,
   dev_stm32f4xx_eth_isset_write,
   dev_stm32f4xx_eth_read,
   dev_stm32f4xx_eth_write,
   dev_stm32f4xx_eth_seek,
   dev_stm32f4xx_eth_ioctl
};

//
static desc_t desc_eth_r = -1;
static desc_t desc_eth_w = -1;
static unsigned int eth_packet_recv_w;
static unsigned int eth_packet_recv_r;

static unsigned int eth_packet_send_w;
static unsigned int eth_packet_send_r;
//
//
static unsigned char dev_stm32f4xx_eth_thread_stack[1]={0};
//
static kernel_pthread_t dev_stm32f4xx_eth_thread;

extern eth_stm32f4x7_info_t eth_stm32f4x7_info;
/*===========================================
Implementation
=============================================*/

/* interrupt service routine */
void ETH_IRQHandler(void)
{
   uint32_t status;

   //
   __hw_enter_interrupt();
   //
	status = ETH->DMASR;

	/* Frame received */
	if ( ETH_GetDMAFlagStatus(ETH_DMA_FLAG_R) == SET) 
	{
		//rt_kprintf("Frame comming\n");
		/* Clear the interrupt flags. */
		/* Clear the Eth DMA Rx IT pending bits */  
		ETH_DMAClearITPendingBit(ETH_DMA_IT_R);
      //lepton: to do: optimzation empty -> not empty
      if(desc_eth_r!=-1 && eth_packet_recv_r==eth_packet_recv_w){
        eth_packet_recv_w++;
        __fire_io_int(ofile_lst[desc_eth_r].owner_pthread_ptr_read);
         
      }
      //lepton

		/* a frame has been received */
	}
   if ( ETH_GetDMAFlagStatus(ETH_DMA_FLAG_T) == SET) 
	{
      /* packet transmission */
      if (ETH_GetDMAITStatus(ETH_DMA_IT_T) == SET) 
      {
         eth_packet_send_r++;
         //lepton: optimzation nt empty -> to empty
         if(desc_eth_w!=-1 && eth_packet_send_r==eth_packet_send_w){
            __fire_io_int(ofile_lst[desc_eth_w].owner_pthread_ptr_write);
         }
         //lepton  
         ETH_DMAClearITPendingBit(ETH_DMA_IT_T);
      }
   }
	//
	ETH_DMAClearITPendingBit(ETH_DMA_IT_NIS);
   //
   __hw_leave_interrupt();
   //	

}
/*-------------------------------------------
| Name:dev_stm32f4xx_eth_thread_routine
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void * dev_stm32f4xx_eth_thread_routine(void* arg){
   for(;;){
     
   }
   return (void*)0;
}

/*-------------------------------------------
| Name:dev_stm32f4xx_eth_thread_init
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_stm32f4xx_eth_thread_init(void)
{
   //
   pthread_attr_t thread_attr={0};
   //
   thread_attr.stacksize = sizeof(dev_stm32f4xx_eth_thread_stack);
   thread_attr.stackaddr = (void*)dev_stm32f4xx_eth_thread_stack;
   thread_attr.priority  = 100;
   thread_attr.timeslice = 1;
   //
   thread_attr.name="ethpoll";
   //
   kernel_pthread_create(&dev_stm32f4xx_eth_thread,&thread_attr,(start_routine_t)dev_stm32f4xx_eth_thread_routine,(char*)0);

   return 0;
}

/*-------------------------------------------
| Name:dev_stm32f4xx_eth_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_stm32f4xx_eth_load(void){
   int error;
   //
   pthread_mutexattr_t mutex_attr=0;
   //configure pinout and the phy device
   eth_bsp_init();
   //
   return error;
}

/*-------------------------------------------
| Name:dev_stm32f4xx_eth_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_stm32f4xx_eth_open(desc_t desc, int o_flag){
   //
   if(desc_eth_r<0 && desc_eth_w<0) {
     //first startup
     // start ethernet periheral
     eth_init();
   }
   //
   if(o_flag & O_RDONLY) {
      if(desc_eth_r<0) {
         desc_eth_r = desc;
         //
         eth_packet_recv_r = 0;
         eth_packet_recv_w = 0;
         //
         if(!ofile_lst[desc].p)
            ofile_lst[desc].p=&eth_stm32f4x7_info;
      }
      else
         return -1;                //already open
   }

   if(o_flag & O_WRONLY) {
      if(desc_eth_w<0) {
         desc_eth_w = desc;
         //
         eth_packet_send_w = 0;
         eth_packet_send_r = 0;
         //
         if(!ofile_lst[desc].p)
            ofile_lst[desc].p=&eth_stm32f4x7_info;
      }
      else
         return -1;                //already open
   }

   //unmask IRQ
   if(desc_eth_r>=0 && desc_eth_w>=0) {
     
   }
   return 0;
}

/*-------------------------------------------
| Name:dev_stm32f4xx_eth_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_stm32f4xx_eth_close(desc_t desc){
  // 
  if(ofile_lst[desc].oflag & O_RDONLY) {
      if(!ofile_lst[desc].nb_reader) {
         desc_eth_r = -1;
      }
   }
   //
   if(ofile_lst[desc].oflag & O_WRONLY) {
      if(!ofile_lst[desc].nb_writer) {
         desc_eth_w = -1;
      }
   }
   //
   if(desc_eth_r<0 && desc_eth_w<0) {
     //to do:stop ethernet periheral
   }

   return 0;
}

/*-------------------------------------------
| Name:dev_stm32f4xx_eth_isset_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_stm32f4xx_eth_isset_read(desc_t desc){
   if(desc_eth_r!=-1 && eth_packet_recv_r!=eth_packet_recv_w){
      return 0;//packet available
   }  
   return -1;
}

/*-------------------------------------------
| Name:dev_stm32f4xx_eth_isset_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_stm32f4xx_eth_isset_write(desc_t desc){
   if(desc_eth_w != -1 && eth_packet_send_r==eth_packet_send_w){
      return 0;//packet was sent
   }
   return -1;
}

/*-------------------------------------------
| Name:dev_stm32f4xx_eth_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_stm32f4xx_eth_read(desc_t desc, char* buf,int size){
   int cb;
   //
   cb = eth_packet_read(buf,size);
   //
   eth_packet_recv_r++;
   //
   return cb;
}

/*-------------------------------------------
| Name:dev_stm32f4xx_eth_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_stm32f4xx_eth_write(desc_t desc, const char* buf,int size){
   int cb;
   //
   eth_packet_send_w++;
   cb = eth_packet_write(buf, size);
   return cb;
}

/*-------------------------------------------
| Name:dev_stm32f4xx_eth_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_stm32f4xx_eth_seek(desc_t desc,int offset,int origin){
   return -1;
}

/*-------------------------------------------
| Name:dev_stm32f4xx_eth_ioctl
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_stm32f4xx_eth_ioctl(desc_t desc,int request,va_list ap) {
   switch(request) {

   //reset interface
   case ETHRESET: {
      eth_stm32f4x7_info_t * p_eth_info=(eth_stm32f4x7_info_t *)ofile_lst[desc].p;
      if(!p_eth_info)
         return -1;
   }
   break;

   //status interface
   case ETHSTAT: {
      eth_stm32f4x7_info_t* p_eth_info=(eth_stm32f4x7_info_t *)ofile_lst[desc].p;
      eth_stat_t* p_eth_stat = va_arg( ap, eth_stat_t*);
      if(!p_eth_info || !p_eth_stat)
         return -1;
      //
      *p_eth_stat = p_eth_info->eth_stat;
   }
   break;

   case ETHSETHWADDRESS: {
      eth_stm32f4x7_info_t* p_eth_info=(eth_stm32f4x7_info_t *)ofile_lst[desc].p;
      unsigned char* p_eth_hwaddr = va_arg( ap, unsigned char*);
      if(!p_eth_info || !p_eth_hwaddr)
         return -1;
      //stop ehternet interface
     
      // Disable all interrupt
     
      //set mac address
      p_eth_info->mac_addr[0] = p_eth_hwaddr[0];
      p_eth_info->mac_addr[1] = p_eth_hwaddr[1];
      p_eth_info->mac_addr[2] = p_eth_hwaddr[2];
      p_eth_info->mac_addr[3] = p_eth_hwaddr[3];
      p_eth_info->mac_addr[4] = p_eth_hwaddr[4];
      p_eth_info->mac_addr[5] = p_eth_hwaddr[5];

      //reopen and restart ethernet interface
      //dmfe_open(p_eth_info);
   }
   break;

   case ETHGETHWADDRESS: {
      eth_stm32f4x7_info_t* p_eth_info=(eth_stm32f4x7_info_t *)ofile_lst[desc].p;
      unsigned char* p_eth_hwaddr = va_arg( ap, unsigned char*);
      if(!p_eth_info || !p_eth_hwaddr)
         return -1;
      p_eth_hwaddr[0] = p_eth_info->mac_addr[0];
      p_eth_hwaddr[1] = p_eth_info->mac_addr[1];
      p_eth_hwaddr[2] = p_eth_info->mac_addr[2];
      p_eth_hwaddr[3] = p_eth_info->mac_addr[3];
      p_eth_hwaddr[4] = p_eth_info->mac_addr[4];
      p_eth_hwaddr[5] = p_eth_info->mac_addr[5];
   }
   break;

   //
   default:
      return -1;
   }

   //
   return 0;
}


/*============================================
| End of Source  : dev_stm32f4xx_eth.c
==============================================*/