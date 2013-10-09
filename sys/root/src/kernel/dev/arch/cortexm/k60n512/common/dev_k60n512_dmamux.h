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
#ifndef __DEV_CORTEXM_K60N512_DMAMUX_H__
#define __DEV_CORTEXM_K60N512_DMAMUX_H__


/*============================================
| Includes
==============================================*/

/*============================================
| Declaration
==============================================*/
#define REG_DMAMUX_CHCFG0        (0x0) //Channel Configuration Register
#define REG_DMAMUX_CHCFG1        (0x1) //Channel Configuration Register
#define REG_DMAMUX_CHCFG2        (0x2) //Channel Configuration Register
#define REG_DMAMUX_CHCFG3        (0x3) //Channel Configuration Register
#define REG_DMAMUX_CHCFG4        (0x4) //Channel Configuration Register
#define REG_DMAMUX_CHCFG5        (0x5) //Channel Configuration Register
#define REG_DMAMUX_CHCFG6        (0x6) //Channel Configuration Register
#define REG_DMAMUX_CHCFG7        (0x7) //Channel Configuration Register
#define REG_DMAMUX_CHCFG8        (0x8) //Channel Configuration Register
#define REG_DMAMUX_CHCFG9        (0x9) //Channel Configuration Register
#define REG_DMAMUX_CHCFG10       (0xa) //Channel Configuration Register
#define REG_DMAMUX_CHCFG11       (0xb) //Channel Configuration Register
#define REG_DMAMUX_CHCFG12       (0xc) //Channel Configuration Register
#define REG_DMAMUX_CHCFG13       (0xd) //Channel Configuration Register
#define REG_DMAMUX_CHCFG14       (0xe) //Channel Configuration Register
#define REG_DMAMUX_CHCFG15       (0xf) //Channel Configuration Register

//
#define REG_DMAMUX_CHCFG_SOURCE_MASK                 0x3Fu
#define REG_DMAMUX_CHCFG_SOURCE_SHIFT                0
#define REG_DMAMUX_CHCFG_SOURCE(x)                   (((uint8_t)(((uint8_t)(x))<< \
                                                                 REG_DMAMUX_CHCFG_SOURCE_SHIFT))& \
                                                      REG_DMAMUX_CHCFG_SOURCE_MASK)
#define REG_DMAMUX_CHCFG_TRIG_MASK                   0x40u
#define REG_DMAMUX_CHCFG_TRIG_SHIFT                  6
#define REG_DMAMUX_CHCFG_ENBL_MASK                   0x80u
#define REG_DMAMUX_CHCFG_ENBL_SHIFT                  7

#endif //__DEV_CORTEXM_K60N512_DMAMUX_H__
