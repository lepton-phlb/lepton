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
#ifndef _IOCTL_ETH_H
#define _IOCTL_ETH_H


/*============================================
| Includes 
==============================================*/


/*============================================
| Declaration  
==============================================*/


#define ETHRESET           0x0001
#define ETHSTAT            0x0002
#define ETHSETHWADDRESS    0x0003
#define ETHGETHWADDRESS    0x0004

//ETHSTAT link state speed
#define ETH_STAT_LINK_MASK (0x0000000F)
#define ETH_STAT_LINK_DOWN (0x00000000)
#define ETH_STAT_LINK_10   (0x00000001)
#define ETH_STAT_LINK_50   (0x00000002)
#define ETH_STAT_LINK_100  (0x00000003)
#define ETH_STAT_LINK_1000 (0x00000004)

//
#define ETH_STAT_ERROR     (0x10000000)
typedef unsigned long eth_stat_t;


#endif
