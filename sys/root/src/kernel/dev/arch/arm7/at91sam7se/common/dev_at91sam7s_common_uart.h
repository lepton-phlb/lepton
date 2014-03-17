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
#ifndef _DEV_AT91SAM7S_COMMON_UART_H
#define _DEV_AT91SAM7S_COMMON_UART_H


/*============================================
| Includes
==============================================*/

/*============================================
| Declaration
==============================================*/

enum UART_offset_register
{
   US_CR = 0x0000,
   US_MR         = US_CR + 0x0001,    //0x0004
   US_IER        = US_CR + 0x0002,    //0x0008
   US_IDR        = US_CR + 0x0003,    //0x000C
   US_IMR        = US_CR + 0x0004,    //0x0010
   US_CSR        = US_CR + 0x0005,    //0x0014
   US_RHR        = US_CR + 0x0006,    //0x0018
   US_THR        = US_CR + 0x0007,    //0x001C
   US_BRGR       = US_CR + 0x0008,    //0x0020
   US_RTOR       = US_CR + 0x0009,    //0x0024
   US_TTGR       = US_CR + 0x000A,    //0x0028
   US_RESERVED_1 = US_CR + 0x000B,    //0x002C
   US_FIDI       = US_CR + 0x0010,    //0x0040
   US_NER        = US_CR + 0x0011,    //0x0044
   US_RESERVED_2 = US_CR + 0x0017,    //0x005C
   US_PDC_RPR    = US_CR + 0x0040,    //0x0100
   US_PDC_RCR    = US_CR + 0x0041,    //0x0104
   US_PDC_TPR    = US_CR + 0x0042,    //0x0108
   US_PDC_TCR    = US_CR + 0x0043,    //0x010C
   US_PDC_RNPR   = US_CR + 0x0044,    //0x0110
   US_PDC_RNCR   = US_CR + 0x0045,    //0x0114
   US_PDC_TNPR   = US_CR + 0x0046,    //0x0118
   US_PDC_TNCR   = US_CR + 0x0047,    //0x011C
   US_PDC_PTCR   = US_CR + 0x0048,    //0x0120
   US_PDC_PTSR   = US_CR + 0x0049,    //0x0124
};

#endif
