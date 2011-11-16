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
#include "usbcore.h"

/*============================================
| Global Declaration
==============================================*/

const unsigned char g_usb_language_id_str [] = {
      USB_STRING_DESC_LENGTH(1),
      USB_DEVREQ_DESCRIPTOR_TYPE_STRING,
      USB_STRING_DESC_ENGLISH_US
};

const unsigned char g_usb_manufacturer_id_str [] = {
      USB_STRING_DESC_LENGTH(13),
      USB_DEVREQ_DESCRIPTOR_TYPE_STRING,
      USB_STRING_DESC_UNICODE('C'),USB_STRING_DESC_UNICODE('h'),USB_STRING_DESC_UNICODE('a'),
      USB_STRING_DESC_UNICODE('u'),USB_STRING_DESC_UNICODE('v'),USB_STRING_DESC_UNICODE('i'),
      USB_STRING_DESC_UNICODE('n'),USB_STRING_DESC_UNICODE('A'),USB_STRING_DESC_UNICODE('r'),
      USB_STRING_DESC_UNICODE('n'),USB_STRING_DESC_UNICODE('o'),USB_STRING_DESC_UNICODE('u'),
      USB_STRING_DESC_UNICODE('x')
};

const unsigned char g_usb_product_id_str [] = {
      USB_STRING_DESC_LENGTH(13),
      USB_DEVREQ_DESCRIPTOR_TYPE_STRING,
      USB_STRING_DESC_UNICODE('A'),USB_STRING_DESC_UNICODE('T'),USB_STRING_DESC_UNICODE('9'),
      USB_STRING_DESC_UNICODE('1'),USB_STRING_DESC_UNICODE('S'),USB_STRING_DESC_UNICODE('A'),
      USB_STRING_DESC_UNICODE('M'),USB_STRING_DESC_UNICODE('9'),USB_STRING_DESC_UNICODE('2'),
      USB_STRING_DESC_UNICODE('6'),USB_STRING_DESC_UNICODE('1'),USB_STRING_DESC_UNICODE('e'),
      USB_STRING_DESC_UNICODE('k')
};

const unsigned char g_usb_serial_number_str [] = {
      USB_STRING_DESC_LENGTH(12),
      USB_DEVREQ_DESCRIPTOR_TYPE_STRING,
      USB_STRING_DESC_UNICODE('0'),USB_STRING_DESC_UNICODE('1'),USB_STRING_DESC_UNICODE('0'),
      USB_STRING_DESC_UNICODE('1'),USB_STRING_DESC_UNICODE('0'),USB_STRING_DESC_UNICODE('1'),
      USB_STRING_DESC_UNICODE('0'),USB_STRING_DESC_UNICODE('1'),USB_STRING_DESC_UNICODE('0'),
      USB_STRING_DESC_UNICODE('1'),USB_STRING_DESC_UNICODE('0'),USB_STRING_DESC_UNICODE('1')
};

/*============================================
| Implementation
==============================================*/


/*============================================
| End of Source  : usbstring.c
|---------------------------------------------
| Historic:
|---------------------------------------------
| Authors     | Date     | Comments
| $Log:$
==============================================*/
