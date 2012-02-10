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
#include "sbc.h"
#include "usbmsd.h"

#include "kernel/fs/vfs/vfsdev.h"
#include "kernel/fs/vfs/vfstypes.h"
#include "kernel/fs/vfs/vfs.h"

#include "kernel/core/system.h"
#include "kernel/core/ioctl_hd.h"
#include "kernel/core/malloc.h"
/*============================================
| Global Declaration
==============================================*/
/// Inquiry data to return to the host for the Lun.
static SBCInquiryData inquiryData = {

   SBC_DIRECT_ACCESS_BLOCK_DEVICE,   // Direct-access block device
   SBC_PERIPHERAL_DEVICE_CONNECTED,  // Peripheral device is connected
   0x00,                             // Reserved bits
   0x01,                             // Media is removable
   SBC_SPC_VERSION_4,                // SPC-4 supported
   0x2,                              // Response data format, must be 0x2
   0,                            // Hierarchical addressing not supported
   0,                            // ACA not supported
   0x0,                              // Obsolete bits
   sizeof(SBCInquiryData) - 5,   // Additional length
   0,                            // No embedded SCC
   0,                            // No access control coordinator
   SBC_TPGS_NONE,                    // No target port support group
   0,                            // Third-party copy not supported
   0x0,                              // Reserved bits
   0,                            // Protection information not supported
   0x0,                              // Obsolete bit
   0,                            // No embedded enclosure service component
   0x0,                              // ???
   0,                            // Device is not multi-port
   0x0,                              // Obsolete bits
   0x0,                              // Unused feature
   0x0,                              // Unused features
   0,                            // Task management model not supported
   0x0,                              // ???
   {'C','h','a','u','v','i','n', 0},
   {'M','a','s','s',' ','S','t','o','r','a','g','e',' ','M','S','D'},
   {'0','.','0','1'},
   {'M','a','s','s',' ','S','t','o','r','a','g','e',' ','E','x','a','m','p','l','e'},
   0x00,                             // Unused features
   0x00,                             // Reserved bits
   {SBC_VERSION_DESCRIPTOR_SBC_3},     // SBC-3 compliant device
   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}  // Reserved
};

static const SBCModeParameterHeader6 modeParameterHeader6 = {
   sizeof(SBCModeParameterHeader6) - 1,             //Length is 0x03
   SBC_MEDIUM_TYPE_DIRECT_ACCESS_BLOCK_DEVICE,       //Direct-access block device
   0,                                                //Reserved bits
   0,                                                //DPO/FUA not supported
   0,                                                //Reserved bits
   0,                                                //not write-protected
   0                                                 //No block descriptor
};
/*============================================
| Implementation
==============================================*/

/*OLD INIT
 * void LUN_Init(MSDLun         *lun,
              Media       *media,
              unsigned char *buffer,
              unsigned int  baseAddress,
              unsigned int  size,
              unsigned int  blockSize)
*/

void SBC_LUN_Init(MSDLun *lun){
   unsigned int logicalBlockAddress = 0;
   unsigned int blockSize=0;

   //DO Various ioctl to get it
   //try to open device
   if((lun->desc = _vfs_open(lun->name, O_RDWR, 0))<0) {
      return;
   }
   _vfs_ioctl(lun->desc,HDGETSZ,&lun->size);
   _vfs_ioctl(lun->desc,HDGETSCTRSZ,&lun->blockSize);     //lun->blockSize=4096;//

   //if(!lun->size || !lun->blockSize) {
   if(!lun->size) {
      return;
   }

   lun->blockSize = 512;
   //allocate read write buffer
   if(!(lun->readWriteBuffer = (unsigned char*)_sys_malloc(lun->blockSize))) {
      return;
   }

   // Initialize inquiry data
   lun->inquiryData = &inquiryData;

   // Initialize read capacity data
   logicalBlockAddress = (lun->size / lun->blockSize) - 1;
   STORE_DWORDB(logicalBlockAddress, lun->readCapacityData.pLogicalBlockAddress);
   STORE_DWORDB(lun->blockSize, lun->readCapacityData.pLogicalBlockLength);
}

//------------------------------------------------------------------------------
//! \brief  Return information about the transfer length and direction expected
//!         by the device for a particular command.
//! \param  command Pointer to a buffer holding the command to evaluate
//! \param  length  Expected length of the data transfer
//! \param  type    Expected direction of data transfer
//! \param  lun     Pointer to the LUN affected by the command
//------------------------------------------------------------------------------
unsigned char SBC_GetCommandInformation(void *command, unsigned int *length, unsigned char *type,
                                        MSDLun *lun) {
   SBCCommand *sbcCommand = (SBCCommand *) command;
   unsigned char isCommandSupported = 1;

   // Identify command
   switch (sbcCommand->bOperationCode) {
   //---------------
   case SBC_INQUIRY:
      //---------------
      (*type) = MSDD_DEVICE_TO_HOST;

      // Allocation length is stored in big-endian format
      (*length) = WORDB(sbcCommand->inquiry.pAllocationLength);
      break;

   //--------------------
   case SBC_MODE_SENSE_6:
      //--------------------
      (*type) = MSDD_DEVICE_TO_HOST;
      if (sbcCommand->modeSense6.bAllocationLength > sizeof(SBCModeParameterHeader6)) {
         *length = sizeof(SBCModeParameterHeader6);
      }
      else {
         *length = sbcCommand->modeSense6.bAllocationLength;
      }

      // Only "return all pages" command is supported
      if (sbcCommand->modeSense6.bPageCode != SBC_PAGE_RETURN_ALL) {
         // Unsupported page
         isCommandSupported = 0;
         (*length) = 0;
      }
      break;

   //------------------------------------
   case SBC_PREVENT_ALLOW_MEDIUM_REMOVAL:
      //------------------------------------
      (*type) = MSDD_NO_TRANSFER;
      break;

   //---------------------
   case SBC_REQUEST_SENSE:
      //---------------------
      (*type) = MSDD_DEVICE_TO_HOST;
      (*length) = sbcCommand->requestSense.bAllocationLength;
      break;

   //-----------------------
   case SBC_TEST_UNIT_READY:
      //-----------------------
      (*type) = MSDD_NO_TRANSFER;
      break;

   //---------------------
   case SBC_READ_CAPACITY_10:
      //---------------------
      (*type) = MSDD_DEVICE_TO_HOST;
      (*length) = sizeof(SBCReadCapacity10Data);
      break;

   //---------------
   case SBC_READ_10:
      //---------------
      (*type) = MSDD_DEVICE_TO_HOST;
      (*length) = WORDB(sbcCommand->read10.pTransferLength)
                  * lun->blockSize;
      break;

   //----------------
   case SBC_WRITE_10:
      //----------------
      (*type) = MSDD_HOST_TO_DEVICE;
      (*length) = WORDB(sbcCommand->write10.pTransferLength)
                  * lun->blockSize;
      break;

   //-----------------
   case SBC_VERIFY_10:
      //-----------------
      (*type) = MSDD_NO_TRANSFER;
      break;

   //------
   default:
      //------
      isCommandSupported = 0;
   }

   // If length is 0, no transfer is expected
   if ((*length) == 0) {
      (*type) = MSDD_NO_TRANSFER;
   }

   return isCommandSupported;
}

//------------------------------------------------------------------------------
//! \brief  Performs a TEST UNIT READY COMMAND command.
//------------------------------------------------------------------------------
static unsigned char SBC_TestUnitReady(MSDLun *lun) {
   return MSDD_STATUS_SUCCESS;
}

//------------------------------------------------------------------------------
//! \brief  Handles an INQUIRY command.
//!
//!         This function operates asynchronously and must be called multiple
//!         times to complete. A result code of MSDDriver_STATUS_INCOMPLETE
//!         indicates that at least another call of the method is necessary.
//------------------------------------------------------------------------------
static unsigned char SBC_Inquiry(MSDLun *lun, usb_mass_state_t * pusb_mass_state) {
   unsigned char result = MSDD_STATUS_INCOMPLETE;
   unsigned int cb=0;
   int w=0;
   char *buf=NULL;

   // Check if required length is 0
   if (pusb_mass_state->cmd_length == 0) {
      // Nothing to do
      return MSDD_STATUS_SUCCESS;
   }

   // Initialize command state if needed
   else if (pusb_mass_state->cmd_state == 0) {
      pusb_mass_state->cmd_state = SBC_STATE_WRITE;
      // Change additional length field of inquiry data
      lun->inquiryData->bAdditionalLength
         = (unsigned char) (pusb_mass_state->cmd_length - 5);
   }

   buf = (char *)lun->inquiryData;
   while((cb+=w)<pusb_mass_state->cmd_length) {
      if((w=
             usb_mass_write_payload(USB_ENDPOINT_DESCRIPTOR_DC_TX_NO,buf+cb,
                                    (int)(pusb_mass_state->cmd_length-cb)))<=0) {
         break;
      }
   }

   if(cb == pusb_mass_state->cmd_length) {
      result = MSDD_STATUS_SUCCESS;
   }
   else {
      result = MSDD_STATUS_ERROR;
   }
   pusb_mass_state->cmd_length-=cb;

   return result;
}

//------------------------------------------------------------------------------
//! \brief  Performs a READ CAPACITY (10) command.
//!
//!         This function operates asynchronously and must be called multiple
//!         times to complete. A result code of MSDDriver_STATUS_INCOMPLETE
//!         indicates that at least another call of the method is necessary.
//------------------------------------------------------------------------------
static unsigned char SBC_ReadCapacity10(MSDLun *lun, usb_mass_state_t * pusb_mass_state){
   unsigned char result = MSDD_STATUS_INCOMPLETE;
   unsigned char status;
   int cb=0;
   int w=0;
   char *buf=NULL;

   // Initialize command state if needed
   if (pusb_mass_state->cmd_state == 0) {
      pusb_mass_state->cmd_state = SBC_STATE_WRITE;
   }

   buf = (char *)&(lun->readCapacityData);
   while((cb+=w)<pusb_mass_state->cmd_length) {
      if((w=
             usb_mass_write_payload(USB_ENDPOINT_DESCRIPTOR_DC_TX_NO,buf+cb,
                                    pusb_mass_state->cmd_length-
                                    cb))<=0) {
         break;
      }
   }

   if(cb == pusb_mass_state->cmd_length) {
      result = MSDD_STATUS_SUCCESS;
   }
   else {
      result = MSDD_STATUS_ERROR;
   }
   pusb_mass_state->cmd_length-=cb;

   return result;
}

//------------------------------------------------------------------------------
//! \brief  Performs a REQUEST SENSE command.
//!
//!         This function operates asynchronously and must be called multiple
//!         times to complete. A result code of MSDDriver_STATUS_INCOMPLETE
//!         indicates that at least another call of the method is necessary.
//! \param  lun          Pointer to the LUN affected by the command
//! \param  commandState Current state of the command
//! \return Operation result code (SUCCESS, ERROR, INCOMPLETE or PARAMETER)
//! \see    MSDLun
//! \see    MSDCommandState
//------------------------------------------------------------------------------
static unsigned char SBC_RequestSense(MSDLun *lun, usb_mass_state_t * pusb_mass_state){
   unsigned char result = MSDD_STATUS_INCOMPLETE;
   unsigned char status;
   int cb=0;
   int w=0;
   char *buf=NULL;

   // Check if requested length is zero
   if (pusb_mass_state->cmd_length == 0) {
      return MSDD_STATUS_SUCCESS;
   }
   // Initialize command state if needed
   else if (pusb_mass_state->cmd_state == 0) {
      pusb_mass_state->cmd_state = SBC_STATE_WRITE;
   }

   buf = (char *)&(lun->requestSenseData);
   while((cb+=w)<pusb_mass_state->cmd_length) {
      if((w=
             usb_mass_write_payload(USB_ENDPOINT_DESCRIPTOR_DC_TX_NO,buf+cb,
                                    pusb_mass_state->cmd_length-
                                    cb))<=0) {
         break;
      }
   }

   if(cb == pusb_mass_state->cmd_length) {
      result = MSDD_STATUS_SUCCESS;
   }
   else {
      result = MSDD_STATUS_ERROR;
   }
   pusb_mass_state->cmd_length-=cb;

   return result;
}

//------------------------------------------------------------------------------
//! \brief  Performs a MODE SENSE (6) command.
//!
//!         This function operates asynchronously and must be called multiple
//!         times to complete. A result code of MSDDriver_STATUS_INCOMPLETE
//!         indicates that at least another call of the method is necessary.
//! \param  lun          Pointer to the LUN affected by the command
//! \param  commandState Current state of the command
//! \return Operation result code (SUCCESS, ERROR, INCOMPLETE or PARAMETER)
//! \see    MSDLun
//! \see    MSDCommandState
//------------------------------------------------------------------------------
static unsigned char SBC_ModeSense6(usb_mass_state_t * pusb_mass_state) {
   unsigned char result = MSDD_STATUS_INCOMPLETE;
   unsigned char status;
   unsigned int cb=0;
   int w=0;
   char *buf=NULL;

   // Check if mode page is supported
   if (((SBCCommand *) pusb_mass_state->cbw.pCommand)->modeSense6.bPageCode
       != SBC_PAGE_RETURN_ALL) {
      return MSDD_STATUS_PARAMETER;
   }

   // Initialize command state if needed
   if (pusb_mass_state->cmd_state == 0) {
      pusb_mass_state->cmd_state = SBC_STATE_WRITE;
   }

   //
   buf = (char *)&modeParameterHeader6;
   while((cb+=w)<pusb_mass_state->cmd_length) {
      if((w=
             usb_mass_write_payload(USB_ENDPOINT_DESCRIPTOR_DC_TX_NO,buf+cb,
                                    (int)(pusb_mass_state->cmd_length-cb)))<=0) {
         break;
      }
   }

   if(cb == pusb_mass_state->cmd_length) {
      result = MSDD_STATUS_SUCCESS;
   }
   else {
      result = MSDD_STATUS_ERROR;
   }
   pusb_mass_state->cmd_length-=cb;

   return result;
}

//------------------------------------------------------------------------------
//! \brief  Performs a READ (10) command on specified LUN.
//!
//!         The data is first read from the media and then sent to the USB host.
//!         This function operates asynchronously and must be called multiple
//!         times to complete. A result code of MSDDriver_STATUS_INCOMPLETE
//!         indicates that at least another call of the method is necessary.
//! \param  lun          Pointer to the LUN affected by the command
//! \param  commandState Current state of the command
//! \return Operation result code (SUCCESS, ERROR, INCOMPLETE or PARAMETER)
//! \see    MSDLun
//! \see    MSDCommandState
//------------------------------------------------------------------------------
static unsigned char SBC_Read10(MSDLun  *lun, usb_mass_state_t * pusb_mass_state) {
   unsigned char status;
   unsigned char result = MSDD_STATUS_INCOMPLETE;
   SBCRead10 *command = (SBCRead10 *) pusb_mass_state->cbw.pCommand;

   // Init command state
   if (pusb_mass_state->cmd_state == 0) {
      pusb_mass_state->cmd_state = SBC_STATE_READ;
   }

   // Convert length from bytes to blocks
   pusb_mass_state->cmd_length /= lun->blockSize;

   // Check length
   if (pusb_mass_state->cmd_length == 0) {
      result = MSDD_STATUS_SUCCESS;
   }
   else {
      switch(pusb_mass_state->cmd_state) {
      case SBC_STATE_READ: {
         int cb=0;
         int r=0;

         //move on device and read one block desc,off,origin
         _vfs_lseek(lun->desc,(DWORDB(command->pLogicalBlockAddress))*lun->blockSize,SEEK_SET);
         //
         while((cb+=r)<lun->blockSize) {
            if((r=_vfs_read(lun->desc,lun->readWriteBuffer+cb,lun->blockSize-cb))<lun->blockSize) {
               break;
            }
         }

         //
         if(cb == lun->blockSize) {
            pusb_mass_state->cmd_state = SBC_STATE_WRITE;
         }
         else {
            //RBC_Read10: Failed to start reading media
            //RBC_Read10: Failed to read media
            SBC_UpdateSenseData(&(lun->requestSenseData),
                                SBC_SENSE_KEY_NOT_READY,
                                SBC_ASC_LOGICAL_UNIT_NOT_READY,
                                0);
            result = MSDD_STATUS_ERROR;
         }
      }
      break;

      //useless
      //case SBC_STATE_WAIT_READ
      //break;

      case SBC_STATE_WRITE: {
         int cb=0;
         int w=0;

         //move on device and write to usb endpoint
         while((cb+=w)<lun->blockSize) {
            if((w=
                   usb_mass_write_payload(USB_ENDPOINT_DESCRIPTOR_DC_TX_NO,lun->readWriteBuffer+cb,
                                          lun->blockSize-cb))<=0) {
               break;
            }
         }

         if(cb == lun->blockSize) {
            pusb_mass_state->cmd_state = SBC_STATE_NEXT_BLOCK;
         }
         else {
            //RBC_Read10: Failed to start to send data
            SBC_UpdateSenseData(&(lun->requestSenseData),
                                SBC_SENSE_KEY_HARDWARE_ERROR,
                                0,
                                0);
            result = MSDD_STATUS_ERROR;
         }
      }
      break;

      //useless
      //case SBC_STATE_WAIT_WRITE
      //break;

      case SBC_STATE_NEXT_BLOCK: {
         // Update transfer length and block address
         STORE_DWORDB(DWORDB(command->pLogicalBlockAddress) + 1,
                      command->pLogicalBlockAddress);

         pusb_mass_state->cmd_length--;

         // Check if transfer is finished
         if (pusb_mass_state->cmd_length == 0) {
            result = MSDD_STATUS_SUCCESS;
         }
         else {
            pusb_mass_state->cmd_state = SBC_STATE_READ;
         }
      }
      break;

      default:
         break;
      }
   }
   pusb_mass_state->cmd_length *= lun->blockSize;

   return result;
}

//------------------------------------------------------------------------------
//! \brief  Performs a WRITE (10) command on the specified LUN.
//!
//!         The data to write is first received from the USB host and then
//!         actually written on the media.
//!         This function operates asynchronously and must be called multiple
//!         times to complete. A result code of MSDDriver_STATUS_INCOMPLETE
//!         indicates that at least another call of the method is necessary.
//! \param  lun          Pointer to the LUN affected by the command
//! \param  commandState Current state of the command
//! \return Operation result code (SUCCESS, ERROR, INCOMPLETE or PARAMETER)
//! \see    MSDLun
//! \see    MSDCommandState
//------------------------------------------------------------------------------

static unsigned char SBC_Write10(MSDLun *lun, usb_mass_state_t * pusb_mass_state) {
   unsigned char status;
   unsigned char result = MSDD_STATUS_INCOMPLETE;
   SBCWrite10 *command = (SBCWrite10 *) pusb_mass_state->cbw.pCommand;

   // Init command state
   if (pusb_mass_state->cmd_state == 0) {
      pusb_mass_state->cmd_state = SBC_STATE_READ;
   }

   // Convert length from bytes to blocks
   pusb_mass_state->cmd_length /= lun->blockSize;

   // Check if length equals 0
   if (pusb_mass_state->cmd_length == 0) {
      result = MSDD_STATUS_SUCCESS;
   }
   else {
      // Current command status
      switch (pusb_mass_state->cmd_state) {
      case SBC_STATE_READ: {
         int cb=0;
         int r=0;

         while((cb+=r)<lun->blockSize) {
            if((r=
                   usb_mass_read_payload(USB_ENDPOINT_DESCRIPTOR_DC_RX_NO,lun->readWriteBuffer+cb,
                                         lun->blockSize-cb))<0) {
               break;
            }
         }

         if(cb == lun->blockSize) {
            pusb_mass_state->cmd_state = SBC_STATE_WRITE;
         }
         else {
            //RBC_Write10: Failed to start receiving data
            //RBC_Write10: Failed to received data
            SBC_UpdateSenseData(&(lun->requestSenseData),
                                SBC_SENSE_KEY_HARDWARE_ERROR,
                                0,
                                0);
            result = MSDD_STATUS_ERROR;
         }
      }
      break;

      //useless
      //case SBC_STATE_WAIT_READ:
      //break;

      case SBC_STATE_WRITE: {
         int w=0;
         int cb=0;

         //desc,off,origin
         _vfs_lseek(lun->desc,(DWORDB(command->pLogicalBlockAddress))*lun->blockSize,SEEK_SET);

         while((cb+=w)<lun->blockSize) {
            if((w=_vfs_write(lun->desc,lun->readWriteBuffer+cb,lun->blockSize-cb))<0) {
               break;
            }
         }

         if(cb == lun->blockSize) {
            pusb_mass_state->cmd_state = SBC_STATE_NEXT_BLOCK;
         }
         else {
            //RBC_Write10: Failed to start media write
            //RBC_Write10: Failed to write media
            SBC_UpdateSenseData(&(lun->requestSenseData),
                                SBC_SENSE_KEY_NOT_READY,
                                0,
                                0);
            result = MSDD_STATUS_ERROR;
         }
      }
      break;

      //useless
      //case SBC_STATE_WAIT_WRITE:
      //break;

      case SBC_STATE_NEXT_BLOCK: {
         pusb_mass_state->cmd_length--;

         STORE_DWORDB(DWORDB(command->pLogicalBlockAddress) + 1,
                      command->pLogicalBlockAddress);

         // Check if transfer is finished
         if (pusb_mass_state->cmd_length == 0) {
            result = MSDD_STATUS_SUCCESS;
         }
         else {
            pusb_mass_state->cmd_state = SBC_STATE_READ;
         }
      }
      break;

      default:
         break;
      }
   }

   // Convert length from blocks to bytes
   pusb_mass_state->cmd_length *= lun->blockSize;

   return result;
}


//------------------------------------------------------------------------------
//! \brief  Updates the sense data of a LUN with the given key and codes
//! \param  requestSenseData             Pointer to the sense data to update
//! \param  senseKey                     Sense key
//! \param  additionalSenseCode          Additional sense code
//! \param  additionalSenseCodeQualifier Additional sense code qualifier
//------------------------------------------------------------------------------
void SBC_UpdateSenseData(SBCRequestSenseData *requestSenseData,
                         unsigned char senseKey,
                         unsigned char additionalSenseCode,
                         unsigned char additionalSenseCodeQualifier) {
   requestSenseData->bSenseKey = senseKey;
   requestSenseData->bAdditionalSenseCode = additionalSenseCode;
   requestSenseData->bAdditionalSenseCodeQualifier
      = additionalSenseCodeQualifier;
}

//------------------------------------------------------------------------------
//! \brief  Processes a SBC command by dispatching it to a subfunction.
//! \param  lun          Pointer to the affected LUN
//! \param  commandState Pointer to the current command state
//! \return Operation result code
//------------------------------------------------------------------------------
#define DUMMY_SBC_BUFFER        64
static int dummy_sbc_op[DUMMY_SBC_BUFFER]={0};
static int dummy_sbc_cb=0;

unsigned char SBC_ProcessCommand(MSDLun *lun, usb_mass_state_t * pusb_mass_state) {
   unsigned char result = MSDD_STATUS_INCOMPLETE;
   SBCCommand *command = (SBCCommand *) pusb_mass_state->cbw.pCommand;

   dummy_sbc_op[dummy_sbc_cb]=command->bOperationCode;
   dummy_sbc_cb = (dummy_sbc_cb+1)&(~DUMMY_SBC_BUFFER);
   // Identify command
   switch (command->bOperationCode) {
   //---------------
   case SBC_READ_10:
      //---------------
      // Perform the Read10 command
      result = SBC_Read10(lun, pusb_mass_state);
      break;

   //----------------
   case SBC_WRITE_10:
      //----------------
      // Perform the Write10 command
      result = SBC_Write10(lun, pusb_mass_state);
      break;

   //---------------------
   case SBC_READ_CAPACITY_10:
      //---------------------
      // Perform the ReadCapacity command
      result = SBC_ReadCapacity10(lun, pusb_mass_state);
      break;

   //---------------------
   case SBC_VERIFY_10:
      //---------------------
      // Flush media
      //MED_Flush(lun->media);
      result = MSDD_STATUS_SUCCESS;
      break;

   //---------------
   case SBC_INQUIRY:
      //---------------
      // Process Inquiry command
      result = SBC_Inquiry(lun, pusb_mass_state);
      break;

   //--------------------
   case SBC_MODE_SENSE_6:
      //--------------------
      // Process ModeSense6 command
      result = SBC_ModeSense6(pusb_mass_state);
      break;

   //-----------------------
   case SBC_TEST_UNIT_READY:
      //-----------------------
      // Process TestUnitReady command
      result = SBC_TestUnitReady(lun);
      break;

   //---------------------
   case SBC_REQUEST_SENSE:
      //---------------------
      // Perform the RequestSense command
      result = SBC_RequestSense(lun, pusb_mass_state);
      break;

   //------------------------------------
   case SBC_PREVENT_ALLOW_MEDIUM_REMOVAL:
      //------------------------------------
      // Nothing to do
      result = MSDD_STATUS_SUCCESS;
      break;

   //------
   default:
      //------
      result = MSDD_STATUS_PARAMETER;
      break;
   }

   return result;
}

/*============================================
| End of Source  : sbc.c
|---------------------------------------------
| Historic:
|---------------------------------------------
| Authors     | Date     | Comments
| $Log: $
==============================================*/
