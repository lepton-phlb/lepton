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
#include "kernel/core/types.h"
#include "kernel/core/interrupt.h"
#include "kernel/core/kernelconf.h"
#include "kernel/core/kernel.h"
#include "kernel/core/system.h"
#include "kernel/core/fcntl.h"
#include "kernel/core/cpu.h"
#include "kernel/core/ioctl.h"
#include "kernel/core/ioctl_usb.h"

#include "kernel/fs/vfs/vfsdev.h"

#include "usbmsd.h"
#include "msd.h"
#include "sbc.h"
#include "lun.h"

#include "kernel/dev/arch/all/usb/usbcore.h"
#include "kernel/dev/arch/all/usb/usbslave.h"

#include <string.h>
#include <stdlib.h>

#define  USBS_MSD_ENDPOINT_OFFSET   (USB_INTERFACE_DESCRIPTOR_LENGTH)

/*============================================
| Global Declaration
==============================================*/
static const char dev_usbmsd_name[]="usbmsd\0";

static int dev_usbmsd_load(void);
static int dev_usbmsd_open(desc_t desc, int o_flag);
static int dev_usbmsd_close(desc_t desc);
static int dev_usbmsd_ioctl(desc_t desc,int request,va_list ap);

//
dev_map_t dev_usbmsd_map={
   dev_usbmsd_name,
   S_IFBLK,
   dev_usbmsd_load,
   dev_usbmsd_open,
   dev_usbmsd_close,
   __fdev_not_implemented,
   __fdev_not_implemented,
   __fdev_not_implemented,
   __fdev_not_implemented,
   __fdev_not_implemented,
   dev_usbmsd_ioctl
};

//
static const usb_configuration_descriptor usb_configuration_mass = {
    length:             USB_CONFIGURATION_DESCRIPTOR_LENGTH,
    type:               USB_CONFIGURATION_DESCRIPTOR_TYPE,
    total_length_lo :   USB_MSD_CONFIGURATION_DESCRIPTOR_SIZE,
    total_length_hi :   0,
    number_interfaces:  USB_MSD_INTERFACE_NUMBER,
    configuration_id:   1,
    configuration_str:  0,
    attributes:         (USB_CONFIGURATION_DESCRIPTOR_ATTR_REQUIRED |
                         USB_CONFIGURATION_DESCRIPTOR_ATTR_SELF_POWERED),
    max_power:          0//50
};

static const usb_interface_descriptor usb_interface_Mass_Storage_Interface[] = {
      USB_INTERFACE_DESCRIPTOR(
            USB_INTERFACE_DESCRIPTOR_LENGTH,
            USB_INTERFACE_DESCRIPTOR_TYPE,
            0,
            0,
            USB_MSD_ENDPOINT_NUMBER,
            USB_INTERFACE_DESCRIPTOR_MSD_CLASS,
            USB_INTERFACE_DESCRIPTOR_MSD_SUBCLASS_SCSI,
            USB_INTERFACE_DESCRIPTOR_MSD_PROTOCOL_BULKO,
            0x00
      ),
      // Tx (Bulk IN) Endpoint Descriptor (data class)
      USB_ENDPOINT_DESCRIPTOR(
            USB_ENDPOINT_DESCRIPTOR_LENGTH,
            USB_ENDPOINT_DESCRIPTOR_TYPE,
            USB_ENDPOINT_DESCRIPTOR_ENDPOINT_IN | USB_ENDPOINT_DESCRIPTOR_DC_TX_NO,
            USB_ENDPOINT_DESCRIPTOR_ATTR_BULK,
            USB_ENDPOINT_DESCRIPTOR_DC_FIFO_LENGHT_LO,
            USB_ENDPOINT_DESCRIPTOR_DC_FIFO_LENGHT_HI,
            0
      ),
      // Rx (Bulk OUT) Endpoint Descriptor (data class)
      USB_ENDPOINT_DESCRIPTOR(
            USB_ENDPOINT_DESCRIPTOR_LENGTH,
            USB_ENDPOINT_DESCRIPTOR_TYPE,
            USB_ENDPOINT_DESCRIPTOR_ENDPOINT_OUT | USB_ENDPOINT_DESCRIPTOR_DC_RX_NO,
            USB_ENDPOINT_DESCRIPTOR_ATTR_BULK,
            USB_ENDPOINT_DESCRIPTOR_DC_FIFO_LENGHT_LO,
            USB_ENDPOINT_DESCRIPTOR_DC_FIFO_LENGHT_HI,
            0
      )
};

static const usb_interface_descriptor * usb_interface_mass [] ={
      usb_interface_Mass_Storage_Interface
};

//from usbstring.c
extern const unsigned char g_usb_language_id_str[];
extern const unsigned char g_usb_manufacturer_id_str[];
extern const unsigned char g_usb_product_id_str[];
extern const unsigned char g_usb_serial_number_str[];

//
static const unsigned char* usb_string_mass[] = {
      g_usb_language_id_str,
      g_usb_manufacturer_id_str,
      g_usb_product_id_str,
      g_usb_serial_number_str
};

//
static usbs_enumeration_data usbs_enumeration_mass = {
    {
        length:                 USB_DEVICE_DESCRIPTOR_LENGTH,
        type:                   USB_DEVICE_DESCRIPTOR_TYPE,
        usb_spec_lo:            USB_DEVICE_DESCRIPTOR_USB20_LO,
        usb_spec_hi:            USB_DEVICE_DESCRIPTOR_USB20_HI,
        device_class:           USB_DEVICE_DESCRIPTOR_MSD_CLASS_VENDOR,
        device_subclass:        USB_DEVICE_DESCRIPTOR_MSD_SUBCLASS_VENDOR,
        device_protocol:        USB_DEVICE_DESCRIPTOR_MSD_PROTOCOL_VENDOR,
        max_packet_size:        8,
        vendor_lo:              USB_DEVICE_DESCRIPTOR_VENDOR_LO,
        vendor_hi:              USB_DEVICE_DESCRIPTOR_VENDOR_HI,
        product_lo:             USB_DEVICE_DESCRIPTOR_PRODUCT_LO,
        product_hi:             USB_DEVICE_DESCRIPTOR_PRODUCT_HI,
        device_lo:              USB_DEVICE_DESCRIPTOR_DEVICE_LO,
        device_hi:              USB_DEVICE_DESCRIPTOR_DEVICE_HI,
        manufacturer_str:       1,
        product_str:            2,
        serial_number_str:      3,//0,
        number_configurations:  1
    },
	{
		length : 					USB_QUALIFIER_DESCRIPTOR_LENGTH,
		type : 						USB_QUALIFIER_DESCRIPTOR_TYPE,
		bcd_low : 					USB_DEVICE_DESCRIPTOR_USB20_LO,
		bcd_high : 					USB_DEVICE_DESCRIPTOR_USB20_HI,
		device_class:           USB_DEVICE_DESCRIPTOR_MSD_CLASS_VENDOR,
      device_subclass:        USB_DEVICE_DESCRIPTOR_MSD_SUBCLASS_VENDOR,
      device_protocol:        USB_DEVICE_DESCRIPTOR_MSD_PROTOCOL_VENDOR,
      max_packet_size:        8,
      number_configurations:	1,
      reserved:					0
		
	},
    total_number_interfaces:    USB_MSD_INTERFACE_NUMBER,
    total_number_endpoints:     USB_MSD_ENDPOINT_NUMBER,
    total_number_strings:       4,
    configurations:             &usb_configuration_mass,
    interfaces:                 usb_interface_mass,
    strings:                    usb_string_mass
};

//
typedef usbs_enumeration_data * pusb_mass_t;
pusb_mass_t pusb_mass = &usbs_enumeration_mass;

//
static usbs_control_return  usbs_mass_acm_class_handler(usbs_control_endpoint* ep0, void* data);
static usbs_control_return  usbs_mass_clear_feature(usbs_control_endpoint* ep0, void* data);
static void usbmsd_complete_fn_write(void *data, int err);
static void usbmsd_complete_fn_read(void *data, int err);

//
static unsigned char * usb_mass_virtual_ep_tbl[3] = {0};
static void fill_virtual_endpoint_table(void);

//
static unsigned char rsp_buf[32]={0};

//
#define	USBS_MSD_THREAD_STACK_SIZE			(8*1024)//4096
static int usb_mass_run_th_stack(void);
static void usb_mass_machine_state(void);
static kernel_pthread_t usb_mass_thread={0};

static inf_usb_mass_t g_inf_usb_mass = {
	desc_rd: -1,
	desc_wr: -1,
	rx_endpoint:USB_ENDPOINT_DESCRIPTOR_DC_RX_NO,
	tx_endpoint:USB_ENDPOINT_DESCRIPTOR_DC_TX_NO 
};

static usb_mass_state_t g_usb_mass_machine_state;

//
#define  MAX_LUN_NAME      32
static char g_usb_mass_lun_name[MAX_LUN_NAME];

static MSDLun g_usb_mass_lun[] = {
	{
     	inquiryData : NULL,
    	readWriteBuffer : NULL,
    	//requestSenseData
    	{
			bResponseCode:SBC_SENSE_DATA_FIXED_CURRENT,//Sense data format
   		isValid      :1,//Information field is standard
   		bObsolete1:0,//Obsolete byte
   		bSenseKey :SBC_SENSE_KEY_NO_SENSE,//Generic error information
   		bReserved1:0,// Reserved bit
			isILI     :0,//SSC
   		isEOM     :0,//SSC
			isFilemark:0,//SSC
			pInformation:{0,0,0,0},//Command-specific
			bAdditionalSenseLength:sizeof(SBCRequestSenseData) - 8,//sizeof(SBCRequestSense_data)-8
   		pCommandSpecificInformation:{0,0,0,0},//ommand-specific
			bAdditionalSenseCode:0,//Additional error information
   		bAdditionalSenseCodeQualifier:0,//Further error information
			bFieldReplaceableUnitCode:0,//Specific component code
			bSenseKeySpecific:0,//Additional exception info
   		isSKSV           :0,//Is sense key specific valid?
			pSenseKeySpecific:{0,0} //Additional exception info
		},
		//readCapacityData
		{
			pLogicalBlockAddress : {0,0,0,0},
			pLogicalBlockLength : {0,0,0,0}
		},
    	baseAddress : 0,
    	size : 0,
    	blockSize : 0,
    	//tauon add
    	name : g_usb_mass_lun_name,
    	desc : -1
	}
};

static void usb_mass_reset_machine_state();
static void usb_mass_init_machine_state(void);
static void usb_mass_get_cmd_info(MSCbw *cbw, unsigned int  *length, unsigned char *type);
static unsigned char usb_mass_preprocess_cmd(void);
static void usb_mass_postprocess_cmd(void);
//
/*============================================
| Implementation
==============================================*/
/*-------------------------------------------
| Name:usbs_mass_acm_class_handler
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
usbs_control_return  usbs_mass_acm_class_handler(usbs_control_endpoint* ep0, void* data) {
   usbs_control_return result = USBS_CONTROL_RETURN_UNKNOWN;
   usb_devreq      *req = (usb_devreq *) ep0->control_buffer;
   
   switch (req->request) {

   case USBS_MSD_GET_MAX_LUN :
   	if ((req->value_lo == 0)
  		&& (req->index_lo == 0)
  		&& (req->length_lo == 1)) {
   		rsp_buf[0] = USBS_MSD_MAX_LUN;
   		ep0->buffer = rsp_buf;
      	ep0->buffer_size = USBS_MSD_MAX_LUN_LENGTH;
      	result = USBS_CONTROL_RETURN_HANDLED;
		}
		else {
			result = USBS_CONTROL_RETURN_STALL;
		}
   break;
   
   case USBS_MSD_BULK_ONLY_RESET :
   	if ((req->value_lo == 0)
  		&& (req->index_lo == 0)
  		&& (req->length_lo == 0)) {
			//
			usb_mass_reset_machine_state();
			//
   		ep0->buffer = rsp_buf;
      	ep0->buffer_size = 0;
      	result = USBS_CONTROL_RETURN_HANDLED;
		}
		else {
			result = USBS_CONTROL_RETURN_STALL;
		}
   break;

   default :
   	result = USBS_CONTROL_RETURN_STALL;
   break;
   }

  return result;
}

/*-------------------------------------------
| Name:usbs_mass_acm_class_handler
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
usbs_control_return  usbs_mass_clear_feature(usbs_control_endpoint* ep0, void* data) {
	return g_usb_mass_machine_state.wait_reset_recovery;
}

/*-------------------------------------------
| Name:usbmsd_complete_fn_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
//#define DUMMY_COMPLETE_RD	128
//static int dummy_complete_rd[DUMMY_COMPLETE_RD]={0};
//static int dummy_complete_rd_cb=0;

void usbmsd_complete_fn_read(void *data, int err) {
	//post sem
	if(err != -1) {
		kernel_sem_post(&g_usb_mass_machine_state.sem_read);
	}
	else {
	   //a++;
//	   dummy_complete_rd[dummy_complete_rd_cb] = err;
//	   dummy_complete_rd_cb = (dummy_complete_rd_cb+1)&(~DUMMY_COMPLETE_RD);
	}
}

/*-------------------------------------------
| Name:usbmsd_complete_fn_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
//#define DUMMY_COMPLETE_WR	128
//static int dummy_complete_wr[DUMMY_COMPLETE_WR]={0};
//static int dummy_complete_wr_cb=0;

void usbmsd_complete_fn_write(void *data, int err) {
	//maybe other things
	//post sem
	if(err != -1) {
		kernel_sem_post(&g_usb_mass_machine_state.sem_write);
	}
	else {
//	   dummy_complete_wr[dummy_complete_wr_cb] = err;
//	   dummy_complete_wr_cb = (dummy_complete_wr_cb+1)&(~DUMMY_COMPLETE_WR);
	}
}

/*-------------------------------------------
| Name:dev_usbmsd_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_usbmsd_load(void){
   return 0;
}

/*-------------------------------------------
| Name:dev_usbmsd_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_usbmsd_open(desc_t desc, int o_flag){
	if(g_inf_usb_mass.desc_rd < 0 && g_inf_usb_mass.desc_wr < 0) {
		//launch thread
		usb_mass_run_th_stack();
	}
	
   if(o_flag & O_RDONLY){
   }

   if(o_flag & O_WRONLY){
   }

   return 0;
}

/*-------------------------------------------
| Name:dev_usbmsd_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_usbmsd_close(desc_t desc){
   if(ofile_lst[desc].oflag & O_RDONLY){
      if(!ofile_lst[desc].nb_reader){
      }
   }

   if(ofile_lst[desc].oflag & O_WRONLY){
      if(!ofile_lst[desc].nb_writer){
      }
   }

   return 0;
}

/*-------------------------------------------
| Name:dev_usbmsd_ioctl
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_usbmsd_ioctl(desc_t desc,int request,va_list ap) {
   switch(request){
      //
      case I_LINK:{
			usbs_completefn_endpoint tmp;
			int desc_dev = va_arg(ap, int);
			char * msd_dev = va_arg(ap, char *);

			if(!msd_dev)
			   return -1;

         if(ofile_lst[desc].p)//already set.
            return 0;

         //set usb_enumeration pointer
         ofile_lst[desc].p = (void *)pusb_mass;
         memcpy((void *)g_usb_mass_lun_name, msd_dev, MAX_LUN_NAME);
         
         //fill endpoint table for driver
         fill_virtual_endpoint_table();
         ofile_lst[ofile_lst[desc].desc_nxt[0]].pfsop->fdev.fdev_ioctl(desc,USB_SET_ENDPOINT_TABLE,&usb_mass_virtual_ep_tbl);
         
         //set usb enumeration to driver
         ofile_lst[ofile_lst[desc].desc_nxt[0]].pfsop->fdev.fdev_ioctl(desc,USB_SET_ENUMERATION_DATA,&pusb_mass);
         
         //set usb special class function
         ofile_lst[ofile_lst[desc].desc_nxt[0]].pfsop->fdev.fdev_ioctl(desc,USB_SET_CLASS_HANDLER,&usbs_mass_acm_class_handler);
         ofile_lst[ofile_lst[desc].desc_nxt[0]].pfsop->fdev.fdev_ioctl(desc,USB_SET_CLEAR_FEATURE_FN,&usbs_mass_clear_feature);
         
         //set complete function for all desire endpoints read and write
         tmp.complete_fn = usbmsd_complete_fn_write;
         tmp.epn = USB_ENDPOINT_DESCRIPTOR_DC_TX_NO;
         ofile_lst[ofile_lst[desc].desc_nxt[0]].pfsop->fdev.fdev_ioctl(desc,USB_SET_COMPLETION_FUNCTION,&tmp);
         //
         tmp.complete_fn = usbmsd_complete_fn_read;
         tmp.epn = USB_ENDPOINT_DESCRIPTOR_DC_RX_NO;
         ofile_lst[ofile_lst[desc].desc_nxt[0]].pfsop->fdev.fdev_ioctl(desc,USB_SET_COMPLETION_FUNCTION,&tmp);
   
         //start control endpoint and enable IRQ on EP3, EP4 and EP5
         ofile_lst[ofile_lst[desc].desc_nxt[0]].pfsop->fdev.fdev_ioctl(desc,USB_START_CONTROL_ENDPOINT,0);
         
         //
         g_inf_usb_mass.desc_rd = ofile_lst[desc].desc_nxt[0];
         g_inf_usb_mass.desc_wr = ofile_lst[desc].desc_nxt[0];
         
         ///dummy BUG no reset of ofile_lst[ofile_lst[desc].desc_nxt[0]].desc_nxt[0|1]
         ofile_lst[ofile_lst[desc].desc_nxt[0]].desc_nxt[0]=-1;
         ofile_lst[ofile_lst[desc].desc_nxt[0]].desc_nxt[1]=-1;
         ///
         return 0;
      }
      break;

      //
      case I_UNLINK:
      break;

      case USB_GET_CONTROL_ENDPOINT_STATE :{
         int *usb_serial_state = va_arg(ap, int *);
         ofile_lst[ofile_lst[desc].desc_nxt[0]].pfsop->fdev.fdev_ioctl(desc,USB_GET_CONTROL_ENDPOINT_STATE,usb_serial_state);
         return 0;
      }
      break;

      //
      default:
         return -1;

   }
   return -1;
}

/*-------------------------------------------
| Name:fill_virtual_endpoint_table
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void fill_virtual_endpoint_table(void) {
   int i=0;
   //We know all offset to get endpoint
   usb_mass_virtual_ep_tbl[i++]=(unsigned char *)(usb_interface_Mass_Storage_Interface+USB_INTERFACE_DESCRIPTOR_LENGTH);
   usb_mass_virtual_ep_tbl[i] =(unsigned char *)(usb_interface_Mass_Storage_Interface+USB_INTERFACE_DESCRIPTOR_LENGTH+USB_ENDPOINT_DESCRIPTOR_LENGTH);
}

/*-------------------------------------------
| Name:usb_mass_run_th_stack
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int usb_mass_run_th_stack(void) {
	pthread_attr_t thread_attr;
	unsigned char * pstack = NULL;

	pstack = (unsigned char*)malloc(USBS_MSD_THREAD_STACK_SIZE);
   if(!pstack)
      return -1;

   thread_attr.stacksize = USBS_MSD_THREAD_STACK_SIZE;
   thread_attr.stackaddr = (void*)pstack;
   thread_attr.priority  = 10;//prio;
   thread_attr.timeslice = 5;
   thread_attr.name= "usbmsd_thread";
   
   kernel_pthread_create(&usb_mass_thread,&thread_attr,(start_routine_t)usb_mass_machine_state,(char*)0);
	return 0;
}

/*-------------------------------------------
| Name:usb_mass_reset_machine_state
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void usb_mass_reset_machine_state(void) {
	g_usb_mass_machine_state.state = MSDD_STATE_READ_CBW;
	g_usb_mass_machine_state.wait_reset_recovery = 0;
	g_usb_mass_machine_state.cmd_state = 0;
}

/*-------------------------------------------
| Name:usb_mass_init_machine_state
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
extern void SBC_LUN_Init(MSDLun *lun);
void usb_mass_init_machine_state(void) {
	//init state
	usb_mass_reset_machine_state();

	//init semaphore
	kernel_sem_init(&g_usb_mass_machine_state.sem_read,0,0);
	kernel_sem_init(&g_usb_mass_machine_state.sem_write,0,0);

	g_usb_mass_machine_state.cmd_length = 0;
	g_usb_mass_machine_state.post_process = 0;
	//
	memset(&g_usb_mass_machine_state.cbw,0,sizeof(MSCbw));
	memset(&g_usb_mass_machine_state.csw,0,sizeof(MSCsw));
	//
	g_usb_mass_machine_state.luns = g_usb_mass_lun;
	SBC_LUN_Init(g_usb_mass_machine_state.luns);
}

/*-------------------------------------------
| Name:usb_mass_read_payload
| Description:
| Parameters:
| Return Type:
| Comments:wait sem and do read from driver-layer
| See:
---------------------------------------------*/
int usb_mass_read_payload(int desc, char* buf,int cb) {
	usbs_data_endpoint_halted_req ep_state_req={desc,-1};
	
	//is endpoint halted now?
	do {
		ofile_lst[g_inf_usb_mass.desc_rd].pfsop->fdev.fdev_ioctl(desc,USB_IS_HALTED_ENDPOINT,&ep_state_req);
	}while(ep_state_req.halted);
	
	kernel_sem_wait(&g_usb_mass_machine_state.sem_read);
	return ofile_lst[g_inf_usb_mass.desc_rd].pfsop->fdev.fdev_read(desc,buf,cb);
}

/*-------------------------------------------
| Name:usb_mass_write_payload
| Description:
| Parameters:
| Return Type:
| Comments:write to driver-layer and wait sem
| See:
---------------------------------------------*/
int usb_mass_write_payload(int desc, char* buf,int cb) {
	int w = 0;
	usbs_data_endpoint_halted_req ep_state_req={desc,-1};
	
	//is endpoint halted now?
	do {
		ofile_lst[g_inf_usb_mass.desc_wr].pfsop->fdev.fdev_ioctl(desc,USB_IS_HALTED_ENDPOINT,&ep_state_req);
	}while(ep_state_req.halted);
	
	w=ofile_lst[g_inf_usb_mass.desc_wr].pfsop->fdev.fdev_write(desc,buf,cb);
	kernel_sem_wait(&g_usb_mass_machine_state.sem_write);

	//
	return w;	
}

/*-------------------------------------------
| Name:usb_mass_get_cmd_info
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void usb_mass_get_cmd_info(MSCbw *cbw, unsigned int  *length, unsigned char *type) {
	// Expected host transfer direction and length
    (*length) = cbw->dCBWDataTransferLength;

    if (*length == 0) {
		 (*type) = MSDD_NO_TRANSFER;
    }
    else if ((cbw->bmCBWFlags & MSD_CBW_DEVICE_TO_HOST) != 0) {
		 (*type) = MSDD_DEVICE_TO_HOST;
    }
    else {
		 (*type) = MSDD_HOST_TO_DEVICE;
    }
}

/*-------------------------------------------
| Name:usb_mass_postprocess_cmd
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void usb_mass_postprocess_cmd(void) {
	MSCsw *pcsw = &(g_usb_mass_machine_state.csw);

	// STALL Bulk IN endpoint ?
	if ((g_usb_mass_machine_state.post_process & MSDD_CASE_STALL_IN) != 0) {
		//MSDD_Halt(MSDD_CASE_STALL_IN);
		ofile_lst[g_inf_usb_mass.desc_wr].pfsop->fdev.fdev_ioctl(g_inf_usb_mass.desc_wr,USB_HALT_ENDPOINT,&g_inf_usb_mass.tx_endpoint);
	}

	// STALL Bulk OUT endpoint ?
	if ((g_usb_mass_machine_state.post_process & MSDD_CASE_STALL_OUT) != 0) {
		//MSDD_Halt(MSDD_CASE_STALL_OUT);
		ofile_lst[g_inf_usb_mass.desc_rd].pfsop->fdev.fdev_ioctl(g_inf_usb_mass.desc_rd,USB_HALT_ENDPOINT,&g_inf_usb_mass.rx_endpoint);
	}

	// Set CSW status code to phase error ?
	if ((g_usb_mass_machine_state.post_process & MSDD_CASE_PHASE_ERROR) != 0) {
		pcsw->bCSWStatus = MSD_CSW_PHASE_ERROR;
	}
}

/*-------------------------------------------
| Name:usb_mass_process_cmd
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
extern unsigned char SBC_ProcessCommand(MSDLun *lun, usb_mass_state_t * pusb_mass_state);	
static unsigned char usb_mass_process_cmd(void) {
	unsigned char   status;
	MSCbw           *pcbw = &(g_usb_mass_machine_state.cbw);
	MSCsw           *pcsw = &(g_usb_mass_machine_state.csw);
	MSDLun          *plun = &(g_usb_mass_machine_state.luns[(unsigned char) pcbw->bCBWLUN]);
	unsigned char   isCommandComplete = 0;

	// Check if LUN is valid
	if (pcbw->bCBWLUN > USBS_MSD_MAX_LUN) {
		status = MSDD_STATUS_ERROR;
	}
	
	else {
		// Process command
		status = SBC_ProcessCommand(plun, &g_usb_mass_machine_state);
	}

	// Check command result code
	if (status == MSDD_STATUS_PARAMETER) {
		// Update sense data
		SBC_UpdateSenseData(&(plun->requestSenseData),
								 SBC_SENSE_KEY_ILLEGAL_REQUEST,
								 SBC_ASC_INVALID_FIELD_IN_CDB,
								 0);

		// Result codes
		pcsw->bCSWStatus = MSD_CSW_COMMAND_FAILED;
		isCommandComplete = 1;

		// stall the request, IN or OUT
		if (((pcbw->bmCBWFlags & MSD_CBW_DEVICE_TO_HOST) == 0)
			&& (pcbw->dCBWDataTransferLength > 0)) {
			// Stall the OUT endpoint : host to device
			ofile_lst[g_inf_usb_mass.desc_rd].pfsop->fdev.fdev_ioctl(g_inf_usb_mass.desc_rd,USB_HALT_ENDPOINT,&g_inf_usb_mass.rx_endpoint);
		}
		else {
			// Stall the IN endpoint : device to host
			ofile_lst[g_inf_usb_mass.desc_wr].pfsop->fdev.fdev_ioctl(g_inf_usb_mass.desc_wr,USB_HALT_ENDPOINT,&g_inf_usb_mass.tx_endpoint);
		}
	}
	
	else if (status == MSDD_STATUS_ERROR) {
		// Update sense data
		// TODO (jjoannic#1#): Change code
		SBC_UpdateSenseData(&(plun->requestSenseData),
								 SBC_SENSE_KEY_MEDIUM_ERROR,
								 SBC_ASC_INVALID_FIELD_IN_CDB,
								 0);

		// Result codes
		pcsw->bCSWStatus = MSD_CSW_COMMAND_FAILED;
		isCommandComplete = 1;
	}
	else {
		// Update sense data
		SBC_UpdateSenseData(&(plun->requestSenseData),
								 SBC_SENSE_KEY_NO_SENSE,
								 0,
								 0);

		// Is command complete ?
		if (status == MSDD_STATUS_SUCCESS) {
			isCommandComplete = 1;
		}
	}

	// Check if command has been completed
	if (isCommandComplete) {
		// Adjust data residue
		if (g_usb_mass_machine_state.cmd_length != 0) {
			pcsw->dCSWDataResidue += g_usb_mass_machine_state.cmd_length;

			// STALL the endpoint waiting for data
			if ((pcbw->bmCBWFlags & MSD_CBW_DEVICE_TO_HOST) == 0) {
				// Stall the OUT endpoint : host to device
				ofile_lst[g_inf_usb_mass.desc_rd].pfsop->fdev.fdev_ioctl(g_inf_usb_mass.desc_rd,USB_HALT_ENDPOINT,&g_inf_usb_mass.rx_endpoint);
			}
			else {
				// Stall the IN endpoint : device to host
				ofile_lst[g_inf_usb_mass.desc_wr].pfsop->fdev.fdev_ioctl(g_inf_usb_mass.desc_wr,USB_HALT_ENDPOINT,&g_inf_usb_mass.tx_endpoint);
			}
		}

		// Reset command state
		g_usb_mass_machine_state.cmd_state = 0;
	}

	return isCommandComplete;
}

/*-------------------------------------------
| Name:usb_mass_preprocess_cmd
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
extern unsigned char SBC_GetCommandInformation(void *command, unsigned int *length, unsigned char *type, MSDLun *lun);
unsigned char usb_mass_preprocess_cmd(void) {
	unsigned int        hostLength;
	unsigned int        deviceLength;
	unsigned char       hostType;
	unsigned char       deviceType;
	unsigned char       isCommandSupported;

	MSCsw           *pcsw = &g_usb_mass_machine_state.csw;
	MSCbw           *pcbw = &g_usb_mass_machine_state.cbw;
	MSDLun          *plun = &(g_usb_mass_machine_state.luns[(unsigned char) pcbw->bCBWLUN]);

	// Get information about the command
	// Host-side
	usb_mass_get_cmd_info(pcbw, &hostLength, &hostType);

	// Device-side
	isCommandSupported = SBC_GetCommandInformation(pcbw->pCommand,&deviceLength,&deviceType,plun);

	// Initialize data residue and result status
   pcsw->dCSWDataResidue = 0;
   pcsw->bCSWStatus = MSD_CSW_COMMAND_PASSED;

    // Check if the command is supported
	if (isCommandSupported) {
		// Identify the command case
		if(hostType == MSDD_NO_TRANSFER) {
			
			// Case 1  (Hn = Dn)
			if(deviceType == MSDD_NO_TRANSFER) {
				g_usb_mass_machine_state.post_process = 0;
				g_usb_mass_machine_state.cmd_length = 0;
			}
			
			else if(deviceType == MSDD_DEVICE_TO_HOST) {
				// Case 2  (Hn < Di)
				g_usb_mass_machine_state.post_process = MSDD_CASE_PHASE_ERROR;
				g_usb_mass_machine_state.cmd_length = 0;
			}
			
			else { //if(deviceType == MSDD_HOST_TO_DEVICE) {
				// Case 3  (Hn < Do)
				g_usb_mass_machine_state.post_process = MSDD_CASE_PHASE_ERROR;
				g_usb_mass_machine_state.cmd_length = 0;
			}
		}

		// Case 4  (Hi > Dn)
		else if(hostType == MSDD_DEVICE_TO_HOST) {
			if(deviceType == MSDD_NO_TRANSFER) {
				g_usb_mass_machine_state.post_process = MSDD_CASE_STALL_IN;
				g_usb_mass_machine_state.cmd_length = 0;
				pcsw->dCSWDataResidue = hostLength;
			}
			else if(deviceType == MSDD_DEVICE_TO_HOST) {

				if(hostLength > deviceLength) {
					// Case 5  (Hi > Di)
					g_usb_mass_machine_state.post_process = MSDD_CASE_STALL_IN;
					g_usb_mass_machine_state.cmd_length = deviceLength;
					pcsw->dCSWDataResidue = hostLength - deviceLength;
				}
				
				else if(hostLength == deviceLength) {
					// Case 6  (Hi = Di)
					g_usb_mass_machine_state.post_process = 0;
					g_usb_mass_machine_state.cmd_length = deviceLength;
				}
	
				else { //if(hostLength < deviceLength) {
					// Case 7  (Hi < Di)
					g_usb_mass_machine_state.post_process = MSDD_CASE_PHASE_ERROR;
					g_usb_mass_machine_state.cmd_length = hostLength;
				}
			}

			else { //if(deviceType == MSDD_HOST_TO_DEVICE) {
				// Case 8  (Hi <> Do)
				g_usb_mass_machine_state.post_process = MSDD_CASE_STALL_IN | MSDD_CASE_PHASE_ERROR;
				g_usb_mass_machine_state.cmd_length = 0;
			}
		}

		else if(hostType == MSDD_HOST_TO_DEVICE) {
			if(deviceType == MSDD_NO_TRANSFER) {
				// Case 9  (Ho > Dn)
				g_usb_mass_machine_state.post_process = MSDD_CASE_STALL_OUT;
				g_usb_mass_machine_state.cmd_length = 0;
				pcsw->dCSWDataResidue = hostLength;
			}

			else if(deviceType == MSDD_DEVICE_TO_HOST) {
				// Case 10 (Ho <> Di)
				g_usb_mass_machine_state.post_process =
				  MSDD_CASE_STALL_OUT | MSDD_CASE_PHASE_ERROR;
				g_usb_mass_machine_state.cmd_length = 0;
			}
			else { //if(deviceType == MSDD_HOST_TO_DEVICE) {

				if(hostLength > deviceLength) {
					// Case 11 (Ho > Do)
					g_usb_mass_machine_state.post_process = MSDD_CASE_STALL_OUT;
					g_usb_mass_machine_state.cmd_length = deviceLength;
					pcsw->dCSWDataResidue = hostLength - deviceLength;
				}

				else if(hostLength == deviceLength) {
					// Case 12 (Ho = Do)
					g_usb_mass_machine_state.post_process = 0;
					g_usb_mass_machine_state.cmd_length = deviceLength;
				}
				
				else { //if(hostLength < deviceLength) {
					// Case 13 (Ho < Do)
					g_usb_mass_machine_state.post_process = MSDD_CASE_PHASE_ERROR;
					g_usb_mass_machine_state.cmd_length = hostLength;
				}
			}
		}
	}

	return isCommandSupported;
}

/*-------------------------------------------
| Name:usb_mass_machine_state
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void usb_mass_machine_state(void) {
	volatile MSCbw * pcbw = NULL;
	volatile MSCsw * pcsw = NULL;
	usb_mass_init_machine_state();
	
	for(;;) {
		pcbw = &g_usb_mass_machine_state.cbw;
		pcsw = &g_usb_mass_machine_state.csw;
		
		//machine state for MSC
		switch(g_usb_mass_machine_state.state) {
			case MSDD_STATE_READ_CBW: {
				int cb=0;
				int r=0;
				char * buf = (char *)pcbw;
				
				//read data
         	while((cb+=r)<MSD_CBW_SIZE){
					r=usb_mass_read_payload(USB_ENDPOINT_DESCRIPTOR_DC_RX_NO,buf+cb,MSD_CBW_SIZE-cb);
					//
					if(r<0) {
						break;
					}
				}
            
            //
            if(cb == MSD_CBW_SIZE) {
					g_usb_mass_machine_state.state = MSDD_STATE_PROCESS_CBW;
				}
				//Invalid CBW (Bad Length)
				else {
					// Wait for a reset recovery
               g_usb_mass_machine_state.wait_reset_recovery = 1;
               
               // Halt the Bulk-IN and Bulk-OUT pipes
               ofile_lst[g_inf_usb_mass.desc_rd].pfsop->fdev.fdev_ioctl(g_inf_usb_mass.desc_rd,USB_HALT_ENDPOINT,&g_inf_usb_mass.rx_endpoint);
               ofile_lst[g_inf_usb_mass.desc_wr].pfsop->fdev.fdev_ioctl(g_inf_usb_mass.desc_wr,USB_HALT_ENDPOINT,&g_inf_usb_mass.tx_endpoint);
					pcsw->bCSWStatus = MSD_CSW_COMMAND_FAILED;
				}
			}
			break;
			
			case MSDD_STATE_PROCESS_CBW: {
				// Check if this is a new command
        		if(g_usb_mass_machine_state.cmd_state == 0) {
					// Copy the CBW tag
            	pcsw->dCSWTag = pcbw->dCBWTag;

            	// Check the CBW Signature
            	if (pcbw->dCBWSignature != MSD_CBW_SIGNATURE) {

               	// Wait for a reset recovery
               	g_usb_mass_machine_state.wait_reset_recovery = 1;

               	// Halt the Bulk-IN and Bulk-OUT pipes
               	ofile_lst[g_inf_usb_mass.desc_rd].pfsop->fdev.fdev_ioctl(g_inf_usb_mass.desc_rd,USB_HALT_ENDPOINT,&g_inf_usb_mass.rx_endpoint);
						ofile_lst[g_inf_usb_mass.desc_wr].pfsop->fdev.fdev_ioctl(g_inf_usb_mass.desc_wr,USB_HALT_ENDPOINT,&g_inf_usb_mass.tx_endpoint);

               	pcsw->bCSWStatus = MSD_CSW_COMMAND_FAILED;
               	g_usb_mass_machine_state.state = MSDD_STATE_READ_CBW;
            	}
            	else {
						// Pre-process command
						usb_mass_preprocess_cmd();
            	}
        		}

        		// Process command
        		if (pcsw->bCSWStatus == MSDD_STATUS_SUCCESS) {
					if (usb_mass_process_cmd()) {
						// Post-process command if it is finished
                	usb_mass_postprocess_cmd();
                	g_usb_mass_machine_state.state = MSDD_STATE_SEND_CSW;
            	}
        		}
			}
			break;
			
			case MSDD_STATE_SEND_CSW: {
				int cb=0;
				int w=0;
				char *buf = (char *)pcsw;
				
				// Set signature
				pcsw->dCSWSignature = MSD_CSW_SIGNATURE;
				
				//write data
				while((cb+=w)<MSD_CSW_SIZE){
            	if((w=usb_mass_write_payload(USB_ENDPOINT_DESCRIPTOR_DC_TX_NO,buf+cb,MSD_CSW_SIZE-cb))<0) {
               	break;
            	}
         	}
         	
         	//
         	if(cb == MSD_CSW_SIZE) {
					g_usb_mass_machine_state.state = MSDD_STATE_READ_CBW;
				}
			}
			break;

			default:
			break;
		}
	}
}
/*============================================
| End of Source  : usbmsd.c
|---------------------------------------------
| Historic:
|---------------------------------------------
| Authors     | Date     | Comments
| $Log:$
==============================================*/
