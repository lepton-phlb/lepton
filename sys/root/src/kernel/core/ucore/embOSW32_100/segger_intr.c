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

/*===========================================
Includes
=============================================*/
//#include "kernel/core/windows.h"
#include "kernel/core/ucore/embOSW32_100/win32/windows.h"
#include "windows.h"
#include <stdio.h>

#include "kernel/core/ucore/embOSW32_100/segger_intr.h"


/*===========================================
Global Declaration
=============================================*/

LONG     lEmuInterruptThreadId;
HANDLE   hEmuInterruptThread=NULL;
 

 //

 PFN_INTERRUPT interrupt0;
 PFN_INTERRUPT interrupt1;
 PFN_INTERRUPT interrupt2;
 PFN_INTERRUPT interrupt3;
 PFN_INTERRUPT interrupt4;
 PFN_INTERRUPT interrupt5;
 PFN_INTERRUPT interrupt6;
 PFN_INTERRUPT interrupt7;
 PFN_INTERRUPT interrupt8;
 PFN_INTERRUPT interrupt9;
 PFN_INTERRUPT interrupt10;
 PFN_INTERRUPT interrupt11;
 PFN_INTERRUPT interrupt12;
 PFN_INTERRUPT interrupt13;
 PFN_INTERRUPT interrupt14;
 PFN_INTERRUPT interrupt15;
 PFN_INTERRUPT interrupt16;
 PFN_INTERRUPT interrupt17;
 PFN_INTERRUPT interrupt18;
 PFN_INTERRUPT interrupt19;
 PFN_INTERRUPT interrupt20;
 PFN_INTERRUPT interrupt21;
 PFN_INTERRUPT interrupt22;
 PFN_INTERRUPT interrupt23;
 PFN_INTERRUPT interrupt24;
 PFN_INTERRUPT interrupt25;
 PFN_INTERRUPT interrupt26;
 PFN_INTERRUPT interrupt27;
 PFN_INTERRUPT interrupt28;
 PFN_INTERRUPT interrupt29;
 PFN_INTERRUPT interrupt30;
 PFN_INTERRUPT interrupt31;
 PFN_INTERRUPT interrupt32;
 PFN_INTERRUPT interrupt33;
 PFN_INTERRUPT interrupt34;
 PFN_INTERRUPT interrupt35;
 PFN_INTERRUPT interrupt36;
 PFN_INTERRUPT interrupt37;
 PFN_INTERRUPT interrupt38;
 PFN_INTERRUPT interrupt39;
 PFN_INTERRUPT interrupt40;
 PFN_INTERRUPT interrupt41;
 PFN_INTERRUPT interrupt42;
 PFN_INTERRUPT interrupt43;
 PFN_INTERRUPT interrupt44;
 PFN_INTERRUPT interrupt45;
 PFN_INTERRUPT interrupt46;
 PFN_INTERRUPT interrupt47;
 PFN_INTERRUPT interrupt48;
 PFN_INTERRUPT interrupt49;
 PFN_INTERRUPT interrupt50;
 PFN_INTERRUPT interrupt51;
 PFN_INTERRUPT interrupt52;
 PFN_INTERRUPT interrupt53;
 PFN_INTERRUPT interrupt54;
 PFN_INTERRUPT interrupt55;
 PFN_INTERRUPT interrupt56;
 PFN_INTERRUPT interrupt57;
 PFN_INTERRUPT interrupt58;
 PFN_INTERRUPT interrupt59;
 PFN_INTERRUPT interrupt60;
 PFN_INTERRUPT interrupt61;
 PFN_INTERRUPT interrupt62;
 PFN_INTERRUPT interrupt63;
 PFN_INTERRUPT interrupt64;
 PFN_INTERRUPT interrupt65;
 PFN_INTERRUPT interrupt66;
 PFN_INTERRUPT interrupt67;
 PFN_INTERRUPT interrupt68;
 PFN_INTERRUPT interrupt69;
 PFN_INTERRUPT interrupt70;
 PFN_INTERRUPT interrupt71;
 PFN_INTERRUPT interrupt72;
 PFN_INTERRUPT interrupt73;
 PFN_INTERRUPT interrupt74;
 PFN_INTERRUPT interrupt75;
 PFN_INTERRUPT interrupt76;
 PFN_INTERRUPT interrupt77;
 PFN_INTERRUPT interrupt78;
 PFN_INTERRUPT interrupt79;
 PFN_INTERRUPT interrupt80;
 PFN_INTERRUPT interrupt81;
 PFN_INTERRUPT interrupt82;
 PFN_INTERRUPT interrupt83;
 PFN_INTERRUPT interrupt84;
 PFN_INTERRUPT interrupt85;
 PFN_INTERRUPT interrupt86;
 PFN_INTERRUPT interrupt87;
 PFN_INTERRUPT interrupt88;
 PFN_INTERRUPT interrupt89;
 PFN_INTERRUPT interrupt90;
 PFN_INTERRUPT interrupt91;
 PFN_INTERRUPT interrupt92;
 PFN_INTERRUPT interrupt93;
 PFN_INTERRUPT interrupt94;
 PFN_INTERRUPT interrupt95;
 PFN_INTERRUPT interrupt96;
 PFN_INTERRUPT interrupt97;
 PFN_INTERRUPT interrupt98;
 PFN_INTERRUPT interrupt99;
 PFN_INTERRUPT interrupt100;
 PFN_INTERRUPT interrupt101;
 PFN_INTERRUPT interrupt102;
 PFN_INTERRUPT interrupt103;
 PFN_INTERRUPT interrupt104;
 PFN_INTERRUPT interrupt105;
 PFN_INTERRUPT interrupt106;
 PFN_INTERRUPT interrupt107;
 PFN_INTERRUPT interrupt108;
 PFN_INTERRUPT interrupt109;
 PFN_INTERRUPT interrupt110;
 PFN_INTERRUPT interrupt111;
 PFN_INTERRUPT interrupt112;
 PFN_INTERRUPT interrupt113;
 PFN_INTERRUPT interrupt114;
 PFN_INTERRUPT interrupt115;
 PFN_INTERRUPT interrupt116;
 PFN_INTERRUPT interrupt117;
 PFN_INTERRUPT interrupt118;
 PFN_INTERRUPT interrupt119;
 PFN_INTERRUPT interrupt120;
 PFN_INTERRUPT interrupt121;
 PFN_INTERRUPT interrupt122;
 PFN_INTERRUPT interrupt123;
 PFN_INTERRUPT interrupt124;
 PFN_INTERRUPT interrupt125;
 PFN_INTERRUPT interrupt126;
 PFN_INTERRUPT interrupt127;
 PFN_INTERRUPT interrupt128;


 PFN_INTERRUPT* interruptList[]={
                               &interrupt0,
                               &interrupt1,
                               &interrupt2,
                               &interrupt3,
                               &interrupt4,
                               &interrupt5,
                               &interrupt6,
                               &interrupt7,
                               &interrupt8,
                               &interrupt9,
                               &interrupt10,
                               &interrupt11,
                               &interrupt12,
                               &interrupt13,
                               &interrupt14,
                               &interrupt15,
                               &interrupt16,
                               &interrupt17,
                               &interrupt18,
                               &interrupt19,
                               &interrupt20,
                               &interrupt21,
                               &interrupt22,
                               &interrupt23,
                               &interrupt24,
                               &interrupt25,
                               &interrupt26,
                               &interrupt27,
                               &interrupt28,
                               &interrupt29,
                               &interrupt30,
                               &interrupt31,
                               &interrupt32,
                               &interrupt33,
                               &interrupt34,
                               &interrupt35,
                               &interrupt36,
                               &interrupt37,
                               &interrupt38,
                               &interrupt39,
                               &interrupt40,
                               &interrupt41,
                               &interrupt42,
                               &interrupt43,
                               &interrupt44,
                               &interrupt45,
                               &interrupt46,
                               &interrupt47,
                               &interrupt48,
                               &interrupt49,
                               &interrupt50,
                               &interrupt51,
                               &interrupt52,
                               &interrupt53,
                               &interrupt54,
                               &interrupt55,
                               &interrupt56,
                               &interrupt57,
                               &interrupt58,
                               &interrupt59,
                               &interrupt60,
                               &interrupt61,
                               &interrupt62,
                               &interrupt63,
                               &interrupt64,
                               &interrupt65,
                               &interrupt66,
                               &interrupt67,
                               &interrupt68,
                               &interrupt69,
                               &interrupt70,
                               &interrupt71,
                               &interrupt72,
                               &interrupt73,
                               &interrupt74,
                               &interrupt75,
                               &interrupt76,
                               &interrupt77,
                               &interrupt78,
                               &interrupt79,
                               &interrupt80,
                               &interrupt81,
                               &interrupt82,
                               &interrupt83,
                               &interrupt84,
                               &interrupt85,
                               &interrupt86,
                               &interrupt87,
                               &interrupt88,
                               &interrupt89,
                               &interrupt90,
                               &interrupt91,
                               &interrupt92,
                               &interrupt93,
                               &interrupt94,
                               &interrupt95,
                               &interrupt96,
                               &interrupt97,
                               &interrupt98,
                               &interrupt99,
                               &interrupt100,
                               &interrupt101,
                               &interrupt102,
                               &interrupt103,
                               &interrupt104,
                               &interrupt105,
                               &interrupt106,
                               &interrupt107,
                               &interrupt108,
                               &interrupt109,
                               &interrupt110,
                               &interrupt111,
                               &interrupt112,
                               &interrupt113,
                               &interrupt114,
                               &interrupt115,
                               &interrupt116,
                               &interrupt117,
                               &interrupt118,
                               &interrupt119,
                               &interrupt120,
                               &interrupt121,
                               &interrupt122,
                               &interrupt123,
                               &interrupt124,
                               &interrupt125,
                               &interrupt126,
                               &interrupt127,
                               &interrupt128
 };



/*===========================================
Implementation
=============================================*/



/*-------------------------------------------
| Name:emuInterruptThread
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
DWORD  emuInterruptThread(LPVOID lpParameter) {

   MSG msg;
   
   while(GetMessage(&msg,NULL,WM_EMUINTERRUPT,WM_EMUINTERRUPT)){

      LONG lInterruptNumber;
      PFN_INTERRUPT pfnInterrupt;

      //printf("rcv io it[%d]\r\n",lInterruptNumber);
     
      lInterruptNumber=msg.lParam;
      pfnInterrupt=*interruptList[lInterruptNumber];
      if(!pfnInterrupt)continue;
      pfnInterrupt();
   }

   return 0;
}


/*-------------------------------------------
| Name:emuFireInterrupt
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void emuFireInterrupt(LONG lInterruptNumber){
   PostThreadMessage(lEmuInterruptThreadId,WM_EMUINTERRUPT,0,lInterruptNumber);
}

/*-------------------------------------------
| Name:emuSetInterruptThreadId
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void emuSetInterruptThreadId(LONG threadId){
   lEmuInterruptThreadId= threadId;
}

/*-------------------------------------------
| Name:emuGetInterruptThreadId
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
LONG emuGetInterruptThreadId(void){
   return lEmuInterruptThreadId;
}

/*-------------------------------------------
| Name:initEmuInterrupt
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void initEmuInterrupt(void) {
   hEmuInterruptThread = CreateThread(NULL,
                                      0,
                                      (LPTHREAD_START_ROUTINE)emuInterruptThread,
                                      NULL,
                                      0,
                                      &lEmuInterruptThreadId ); 
}

/*===========================================
End of SourceOS_EmuInterrupt.c
=============================================*/
 