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
| Compiler Directive
==============================================*/

/*============================================
| Includes
==============================================*/

//unmask USB DP irq
#define USB_START_CONTROL_ENDPOINT        0x01

//set enumeration data from class to device
#define USB_SET_ENUMERATION_DATA                                0x02

//set completion function after read or write we put fire_io_int on it
#define USB_SET_COMPLETION_FUNCTION                     0x04

//set function for class handler exemples : CDC, MSD
#define USB_SET_CLASS_HANDLER                                   0x08

//set endpoint table
#define USB_SET_ENDPOINT_TABLE                          0x10

//
#define USB_GET_CONTROL_ENDPOINT_STATE    0x11

//halt endpoint
#define USB_HALT_ENDPOINT                                               0x12

//is endpoint halted
#define USB_IS_HALTED_ENDPOINT                          0x14

//set class clear feature handler
#define USB_SET_CLEAR_FEATURE_FN                                0x18
/*============================================
| Declaration
==============================================*/
