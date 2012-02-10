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


#ifndef __SETINTERRUPTTABLE_H
#define __SETINTERRUPTTABLE_H
/*--------------------------------------------------------------------------
|               STANDARD INCLUDES FILES
--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
|               PROJECT INCLUDES FILES
--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
|               INTERNAL FUNCTION PROTOTYPE
--------------------------------------------------------------------------*/
typedef enum {
   _breakNumVector,
   _01ReservedNumVector,
   _02ReservedNumVector,
   _03ReservedNumVector,
   _int3NumVector,
   _TimerB5NumVector,
   _TimerB4NumVector,
   _TimerB3NumVector,
   _int5NumVector,
   _int4NumVector,
   _BusCollisionNumVector,
   _DMA0NumVector,
   _DMA1NumVector,
   _KeyInputNumVector,
   _ADNumVector,
   _UART2TransmitNumVector,
   _UART2ReceiveNumVector,
   _UART0TransmitNumVector,
   _UART0ReceiveNumVector,
   _UART1TransmitNumVector,
   _UART1ReceiveNumVector,
   _TimerA0NumVector,
   _TimerA1NumVector,
   _TimerA2NumVector,
   _TimerA3NumVector,
   _TimerA4NumVector,
   _TimerB0NumVector,
   _TimerB1NumVector,
   _TimerB2NumVector,
   _INT0NumVector,
   _INT1NumVector,
   _INT2NumVector,
   //32 to 63 Software interrupt
   _lastValueNumVector
} __vectorInterruptNames;

/*--------------------------------------------------------------------------
|               EXTERNAL FUNCTION PROTOTYPE
--------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
|              GLOBAL DATA
--------------------------------------------------------------------------*/


#endif
/*========================================================================*
|*   END OF _SetInterruptTable.h
|*========================================================================*/
