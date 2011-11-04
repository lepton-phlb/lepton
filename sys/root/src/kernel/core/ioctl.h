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


/*===========================================
Includes
=============================================*/


/*===========================================
Global Declaration
=============================================*/
#define IOCTL_RESERVED_CMD_BASE  0x8000
#define IOCTL_MULTIBOOT_GETVAL   (IOCTL_RESERVED_CMD_BASE|0x0001)
#define I_LINK                   (IOCTL_RESERVED_CMD_BASE|0x0002)
#define I_UNLINK                 (IOCTL_RESERVED_CMD_BASE|0x0003)

#define FIONBIO                  (IOCTL_RESERVED_CMD_BASE|0x0004) //If arg is non-zero, set O_NONBLOCK, otherwise clear O_NONBLOCK. 
#define FIONREAD                 (IOCTL_RESERVED_CMD_BASE|0x0005) //not supported by all device driver. at this moment only supported by lwip_socket api.

/*
#define FIOASYNC //, default processing will be done:
#define FIOCLEX (0x5451) //Sets the close-on-exec bit. 
#define FIONCLEX (0x5450) //Clears the close-on-exec bit. 
#define FIONBIO (0x5421) //
#define FIOASYNC (0x5452)
*/

/*===========================================
Implementation
=============================================*/


/*===========================================
End of Source ioctl.h
=============================================*/
