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
#include "seggerwin32.h"
#include <stdio.h>

/*===========================================
Global Declaration
=============================================*/

HANDLE hLifeThread;
DWORD  dwLifeThreadId;

volatile OS_I32 OS_Time=0;

CRITICAL_SECTION seggerCriticalSection; 
/*===========================================
Implementation
=============================================*/
/*-------------------------------------------
| Name:lifeThread
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
DWORD lifeThread(LPVOID lpvParameter){
   MSG msg;

   OS_Time=0;

   SetTimer(NULL,0,1,NULL);//1 tik by ms

   while (GetMessage(&msg, NULL, 0, 0)) {
      if(msg.message==WM_TIMER){
         OS_Time+=10;
         if(!(OS_Time%5000));
            //printf("*\n");
      }
      
   }

   return 0;
}


/*-------------------------------------------
| Name:OS_InitKern
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void OS_InitKern(void){

   //Get Process Handle
   hSeggerProcess = OpenProcess(PROCESS_ALL_ACCESS,0,GetCurrentProcessId());

   hLifeThread=CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)lifeThread, NULL, 0, &dwLifeThreadId ); 

   initSeggerWin32();
}

/*-------------------------------------------
| Name:initSeggerWin32
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void initSeggerWin32(void){

   //hSemInterrupt= CreateSemaphore (NULL,1,1,"interruptSem");
   // Initialize the critical section.
   InitializeCriticalSection(&interruptCriticalSection); 

   InitializeCriticalSection(&seggerCriticalSection); 

   initEmuInterrupt();

}
/*===========================================
End of SourceseggerWin32.c
=============================================*/
