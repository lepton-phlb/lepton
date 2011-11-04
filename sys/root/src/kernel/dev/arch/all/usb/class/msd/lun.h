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

//------------------------------------------------------------------------------
/// \unit
/// !Purpose
/// 
/// Logical Unit Number (LUN) used by the Mass Storage driver and the SCSI
/// protocol. Represents a logical hard-drive.
/// 
/// !Usage
/// -# Initialize Memory related pins (see pio & board.h).
/// -# Initialize a Media instance for the LUN (see memories).
/// -# Initlalize the LUN with LUN_Init, and link to the initialized Media.
/// -# To read data from the LUN linked media, uses LUN_Read.
/// -# To write data to the LUN linked media, uses LUN_Write.
//------------------------------------------------------------------------------

#ifndef MSDLUN_H
#define MSDLUN_H

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#include "sbc.h"

//------------------------------------------------------------------------------
//      Definitions
//------------------------------------------------------------------------------

#define LUN_STATUS_SUCCESS          0x00
#define LUN_STATUS_ERROR            0x02

//------------------------------------------------------------------------------
//      Structures
//------------------------------------------------------------------------------

/// LUN structure
typedef struct {

    /// Pointer to a SBCInquiryData instance.
    SBCInquiryData        	*inquiryData;
    /// Buffer for USB transfer, must be assigned.
    unsigned char         	*readWriteBuffer;
    /// Data for the RequestSense command.
    SBCRequestSenseData   	requestSenseData;
    /// Data for the ReadCapacity command.
    SBCReadCapacity10Data 	readCapacityData;
    /// Pointer to Media instance for the LUN.
    //Media                 *media;
    /// The start position of the media allocated to the LUN.
    unsigned int          	baseAddress;
    /// The size of the media allocated to the LUN.
    unsigned int          	size;
    /// Sector size of the media
    unsigned int          	blockSize;
    
    //tauon add
    char							*name; //name of lun (ex : /dev/ramdsk)
    int							desc;	 //

} MSDLun;

#endif //#ifndef MSDLUN_H
