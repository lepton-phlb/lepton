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


/*============================================
| Compiler Directive
==============================================*/
#ifndef _USB_MSD_H_
#define _USB_MSD_H_
/*============================================
| Includes
==============================================*/

/*============================================
| Declaration
==============================================*/
/// Command Block Wrapper Size
#define MSD_CBW_SIZE                            31
/// 'USBC' 0x43425355
#define MSD_CBW_SIGNATURE                       0x43425355

/// CBW bmCBWFlags field
#define MSD_CBW_DEVICE_TO_HOST                  (1 << 7)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "MSD CSW Definitions"
/// This page lists the Command Status Wrapper (CSW) definitions.
///
/// !Constants
/// - MSD_CSW_SIZE
/// - MSD_CSW_SIGNATURE
///
/// !Command Block Status Values
/// (Table 5.3 , USB Mass Storage Class Bulk-Only Transport)
/// - MSD_CSW_COMMAND_PASSED
/// - MSD_CSW_COMMAND_FAILED
/// - MSD_CSW_PHASE_ERROR

/// Command Status Wrapper Size
#define MSD_CSW_SIZE                            13
/// 'USBS' 0x53425355
#define MSD_CSW_SIGNATURE                       0x53425355

/// Command Passed (good status)
#define MSD_CSW_COMMAND_PASSED                  0
/// Command Failed
#define MSD_CSW_COMMAND_FAILED                  1
/// Phase Error
#define MSD_CSW_PHASE_ERROR                     2
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//      Structures
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Command Block Wrapper (CBW), 
/// See Table 5.1, USB Mass Storage Class Bulk-Only Transport.
///
/// The CBW shall start on a packet boundary and shall end as a
/// short packet with exactly 31 (1Fh) bytes transferred.
//------------------------------------------------------------------------------
typedef struct {

    /// 'USBC' 0x43425355 (little endian)
    unsigned int  dCBWSignature;
    /// Must be the same as dCSWTag
    unsigned int  dCBWTag;
    /// Number of bytes transfer
    unsigned int  dCBWDataTransferLength;
    /// Indicates the directin of the transfer:
    /// 0x80=IN=device-to-host,
    /// 0x00=OUT=host-to-device
    unsigned char bmCBWFlags;
    /// bits 0->3: bCBWLUN
    unsigned char bCBWLUN   :4,
                  bReserved1:4;           /// reserved
    /// bits 0->4: bCBWCBLength
    unsigned char bCBWCBLength:5,
                  bReserved2  :3;         /// reserved
    /// Command block
    unsigned char pCommand[16];

} MSCbw;

//------------------------------------------------------------------------------
/// Command Status Wrapper (CSW),
/// See Table 5.2, USB Mass Storage Class Bulk-Only Transport.
//------------------------------------------------------------------------------
typedef struct
{
    /// 'USBS' 0x53425355 (little endian)
    unsigned int  dCSWSignature;
    /// Must be the same as dCBWTag
    unsigned int  dCSWTag;
    /// For Data-Out the device shall report in the dCSWDataResidue the
    /// difference between the amount of data expected as stated in the
    /// dCBWDataTransferLength, and the actual amount of data processed by
    /// the device. For Data-In the device shall report in the dCSWDataResidue
    /// the difference between the amount of data expected as stated in the
    /// dCBWDataTransferLength and the actual amount of relevant data sent by
    /// the device. The dCSWDataResidue shall not exceed the value sent in the
    /// dCBWDataTransferLength.
    unsigned int  dCSWDataResidue;
    /// Indicates the success or failure of the command.
    unsigned char bCSWStatus;

} MSCsw;

/// \page "MSD Driver Possible states"
/// ...
///
/// !States
/// - MSDD_STATE_READ_CBW
/// - MSDD_STATE_WAIT_CBW
/// - MSDD_STATE_PROCESS_CBW
/// - MSDD_STATE_SEND_CSW
/// - MSDD_STATE_WAIT_CSW

//! \brief  Driver is expecting a command block wrapper
#define MSDD_STATE_READ_CBW              (1 << 0)

//! \brief  Driver is waiting for the transfer to finish
#define MSDD_STATE_WAIT_CBW              (1 << 1)

//! \brief  Driver is processing the received command
#define MSDD_STATE_PROCESS_CBW           (1 << 2)

//! \brief  Driver is starting the transmission of a command status wrapper
#define MSDD_STATE_SEND_CSW              (1 << 3)

//! \brief  Driver is waiting for the CSW transmission to finish
#define MSDD_STATE_WAIT_CSW              (1 << 4)

/// \page "MSD Driver Xfr Directions"
/// This page lists possible direction values for a data transfer
///
/// !Directions
/// - MSDD_DEVICE_TO_HOST
/// - MSDD_HOST_TO_DEVICE
/// - MSDD_NO_TRANSFER

#define MSDD_DEVICE_TO_HOST              0
#define MSDD_HOST_TO_DEVICE              1
#define MSDD_NO_TRANSFER                 2

/// \page "MSD Driver Result Codes"
/// This page lists result codes for MSD functions.
///
/// !Codes
/// - MSDD_STATUS_SUCCESS
/// - MSDD_STATUS_ERROR
/// - MSDD_STATUS_INCOMPLETE
/// - MSDD_STATUS_PARAMETER

//! \brief  Method was successful
#define MSDD_STATUS_SUCCESS              0x00

//! \brief  There was an error when trying to perform a method
#define MSDD_STATUS_ERROR                0x01

//! \brief  No error was encountered but the application should call the
//!         method again to continue the operation
#define MSDD_STATUS_INCOMPLETE           0x02

//! \brief  A wrong parameter has been passed to the method
#define MSDD_STATUS_PARAMETER            0x03

/// \page "MSD Driver Action Cases"
/// This page lists actions to perform during the post-processing phase of a
/// command.
///
/// !Actions 
/// - MSDD_CASE_PHASE_ERROR
/// - MSDD_CASE_STALL_IN
/// - MSDD_CASE_STALL_OUT

//! \brief  Indicates that the CSW should report a phase error
#define MSDD_CASE_PHASE_ERROR            (1 << 0)

//! \brief  The driver should halt the Bulk IN pipe after the transfer
#define MSDD_CASE_STALL_IN               (1 << 1)

//! \brief  The driver should halt the Bulk OUT pipe after the transfer
#define MSDD_CASE_STALL_OUT              (1 << 2)

#endif /*_USB_MSD_H_*/
