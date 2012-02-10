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
#ifndef _DEV_M16C_NJU6433_H
#define _DEV_M16C_NJU6433_H


/*============================================
| Includes
==============================================*/


/*============================================
| Declaration
==============================================*/
/*
old lcd display
enum _nju6433_segment{
//
_S31,
_S29,
_S28,
_S17,
//
_5A,
_5B,
_5C,
_DP5,
//
_5F,
_5G,
_5E,
_5D,
//
_6A,
_6B,
_6C,
_DP6,
//
_6F,
_6G,
_6E,
_6D,
//
_7A,
_7B,
_7C,
_DP7,
//
_7F,
_7G,
_7E,
_7D,
//
_8A,
_8B,
_8C,
_NC0,
//
_8F,
_8G,
_8E,
_8D,
//
_S36,
_S32,
_NC1,
_S10,
//
_S37,
_S34,
_S33,
_NC2,
//
_S38,
_NC3,
_S11,
_S35,
//
_S14,
_S12,
_S13,
_S15,
//
_S21,
_S23,
_S25,
_S9,
//
_S20,
_S22,
_S24,
_S8,
//
_S39,
_S40,
_S19,
_S7,
//
_4A,
_4B,
_4C,
_NC4,
//
_4F,
_4G,
_4E,
_4D,
//
_3A,
_3B,
_3C,
_DP3,
//
_3F,
_3G,
_3E,
_3D,
//
_2A,
_2B,
_2C,
_DP2,
//
_2F,
_2G,
_2E,
_2D,
//
_1A,
_1B,
_1C,
_DP1,
//
_1F,
_1G,
_1E,
_1D,
//
_NC5,
_S18,
_S2,
_S26,
//
_S3,
_S4,
_S5,
_S6,
//
_S1,
_S16,
_S27,
_S30
};
*/


enum _nju6433_segment {
//S1
   _NC0,
   _NC1,
   _S27,
   _S30,
//S2
   _NC2,
   _S28,
   _S29,
   _S31,
//S3
   _5F,
   _5G,
   _5E,
   _5D,
//S4
   _5A,
   _5B,
   _5C,
   _NC3,
//S5
   _6A,
   _6B,
   _6C,
   _6D,
//S6
   _6F,
   _6G,
   _6E,
   _DP5,
//S7
   _7F,
   _7G,
   _7E,
   _7D,
//S8
   _7A,
   _7B,
   _7C,
   _DP6,
//S9
   _8F,
   _8G,
   _8E,
   _8D,
//S10
   _8A,
   _8B,
   _8C,
   _DP7,
//S11
   _NC4,
   _S32,
   _S37,
   _S36,
//S12
   _S33,
   _S34,
   _S38,
   _NC5,
//S13
   _NC8,
   _S20,
   _S22,
   _S25,
//S14
   _4A,
   _4B,
   _4C,
   _S24,
//S15
   _4F,
   _4G,
   _4E,
   _4D,
//S16
   _3A,
   _3B,
   _3C,
   _DP3,
//S17
   _3F,
   _3G,
   _3E,
   _3D,
//S18
   _S19,
   _S8,
   _NC9,
   _NC10,
//S19
   _2A,
   _2B,
   _2C,
   _DP2,
//S20
   _2F,
   _2G,
   _2E,
   _2D,
//S21
   _1A,
   _1B,
   _1C,
   _DP1,
//S22
   _1F,
   _1G,
   _1E,
   _1D,
//S23
   _S18,
   _S7,
   _NC11,
   _NC12,
//S24
   _S2,
   _S39,
   _S40,
   _S17,
//S25
   _S6,
   _S5,
   _S4,
   _S3,
//S26
   _NC13,
   _NC14,
   _S1,
   _S16,
//S27
   _NC15,
   _NC16,
   _NC17,
   _S26,
//S28
   _S12,
   _S13,
   _S14,
   _S15,
//S29
   _NC6,
   _S9,
   _S10,
   _S35,
//S30
   _NC7,
   _S21,
   _S23,
   _S11
};


#endif
