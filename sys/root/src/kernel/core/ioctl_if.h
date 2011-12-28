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
| Compiler Directive   
==============================================*/
#ifndef _IOCTL_IF_H
#define _IOCTL_IF_H


/*============================================
| Includes 
==============================================*/


/*============================================
| Declaration  
==============================================*/
//interface flag
#define IFF_BROADCAST    (0x0001)
#define IFF_POINTTOPOINT (0x0001<<1)
#define IFF_UP           (0x0001<<2)
#define IFF_DOWN         (0x0001<<3)
#define IFF_ADD          (0x0001<<4) //add  pseudo device interface
#define IFF_DEL          (0x0001<<5) //delete pseudo device interface

#define IFADD        0x0001 //add  pseudo device interface
#define IFDEL        0x0002 //delete pseudo device interface
#define IFSETCFG     0x0003
#define IFGETCFG     0x0004

typedef struct if_config_st{
   char if_ip_addr[32];
   char gw_ip_addr[32];
   char if_net_msk[32];

   int  if_mtu;//1500;
   int  if_flags;//NETIF_FLAG_BROADCAST or NETIF_FLAG_POINTTOPOINT (slip ppp)

   unsigned char hw_mac_addr[6];

   int if_no;
}if_config_t;


#endif
