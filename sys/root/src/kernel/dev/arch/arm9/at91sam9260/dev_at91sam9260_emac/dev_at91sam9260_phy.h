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
#ifndef _DEV_AT91SAM9260_PHY_H
#define _DEV_AT91SAM9260_PHY_H

/*============================================
| Includes
==============================================*/


/*============================================
| Declaration
==============================================*/

/// The reset length setting for external reset configuration
#define DM9161_RESET_LENGTH         0xD


/// The DM9161 instance
typedef struct
{
   /// The retry & timeout settings
   unsigned int retryMax;
   /// PHY address ( pre-defined by pins on reset )
   unsigned char phyAddress;
} phy_stuff_t;

/*============================================
| Prototypes
==============================================*/
extern void dm9161_init(phy_stuff_t *pDm, unsigned char phyAddress);
extern unsigned char dm9161_init_phy(phy_stuff_t *pDm);
extern unsigned char dm9161_auto_negotiate(phy_stuff_t *pDm);
extern unsigned char dm9161_get_link_speed(phy_stuff_t *pDm, unsigned char applySetting);


#endif // _DEV_AT91SAM9260_PHY_H

/*============================================
| End of Source  : dev_at91sam9260_phy.h
==============================================*/

