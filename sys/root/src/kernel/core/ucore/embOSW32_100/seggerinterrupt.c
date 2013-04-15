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

/*===========================================
Global Declaration
=============================================*/
//volatile HANDLE hSemInterrupt=NULL;

CRITICAL_SECTION interruptCriticalSection; 

/*===========================================
Implementation
=============================================*/


/*-------------------------------------------
| Name:OS_EnterInterrupt
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void OS_EnterInterrupt(void){

   int i;
   
   ENTER_INTERRUPT();
   //
   seggerStarted=FALSE;
  
   PROTECT_INTERNAL_SEGGER_BEGIN();
   for(i=0;i < MAX_TASKLIST; i++){
      if(!osTaskList[i])continue;
      SuspendThread(osTaskList[i]->hTask);
   }
   PROTECT_INTERNAL_SEGGER_END();
   
}

/*-------------------------------------------
| Name:OS_LeaveInterrupt
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void OS_LeaveInterrupt(void){

   int i;

   //
   seggerStarted=TRUE;
   //
   PROTECT_INTERNAL_SEGGER_BEGIN();
   for(i=0;i < MAX_TASKLIST; i++){
      if(!osTaskList[i])continue;

      ResumeThread(osTaskList[i]->hTask);
   }
   PROTECT_INTERNAL_SEGGER_END();
   //
   LEAVE_INTERRUPT();  
}

/*-------------------------------------------
| Name:OS_EnterNestableInterrupt
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void OS_EnterNestableInterrupt(void){

   int i;

   ENTER_INTERRUPT();
   //
   seggerStarted=FALSE;
   //
   PROTECT_INTERNAL_SEGGER_BEGIN();
   for(i=0;i < MAX_TASKLIST; i++){
      if(!osTaskList[i])continue;
      SuspendThread(osTaskList[i]->hTask);
   }
   PROTECT_INTERNAL_SEGGER_END();
}

/*-------------------------------------------
| Name:OS_LeaveNestableInterrupt
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void OS_LeaveNestableInterrupt(void){

   int i;

   //
   seggerStarted=TRUE;
   //
   PROTECT_INTERNAL_SEGGER_BEGIN();
   for(i=0;i < MAX_TASKLIST; i++){
      if(!osTaskList[i])continue;
      ResumeThread(osTaskList[i]->hTask);
   }
   PROTECT_INTERNAL_SEGGER_END();
   //
   LEAVE_INTERRUPT();
   
}

/*-------------------------------------------
| Name:OS_EnterRegion
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void OS_EnterRegion(void){ 
   int i;

   PROTECT_INTERNAL_SEGGER_BEGIN();
   for(i=0;i < MAX_TASKLIST; i++){
      if(!osTaskList[i] 
         || osTaskList[i]->Id!=GetCurrentThreadId())
         continue;

      if(osTaskList[i]->regionCount>0){
         osTaskList[i]->regionCount++;
         PROTECT_INTERNAL_SEGGER_END();
         return;
      }

      break;
   }
   PROTECT_INTERNAL_SEGGER_END();
   // 
   if(i==MAX_TASKLIST){
      return;
   }
   //
   ENTER_INTERRUPT();
   //
   seggerStarted=FALSE;
   //
   PROTECT_INTERNAL_SEGGER_BEGIN();
   for(i=0;i < MAX_TASKLIST; i++){
      if(!osTaskList[i])continue;

      if(osTaskList[i]->Id!=GetCurrentThreadId()){
         SuspendThread(osTaskList[i]->hTask);
         Sleep(__SLEEP_SEGGER);
      }else
         osTaskList[i]->regionCount++;
   }
   PROTECT_INTERNAL_SEGGER_END();
}

/*-------------------------------------------
| Name:OS_LeaveRegion
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void OS_LeaveRegion(void){ 

   int i;

   seggerStarted=TRUE;   
   //
   PROTECT_INTERNAL_SEGGER_BEGIN();
   for(i=0;i < MAX_TASKLIST; i++){
      if(!osTaskList[i] 
         || osTaskList[i]->Id!=GetCurrentThreadId())
         continue;

      if(osTaskList[i]->regionCount>1){
         osTaskList[i]->regionCount--;
         PROTECT_INTERNAL_SEGGER_END();
         return;
      }

      break;
   }
   PROTECT_INTERNAL_SEGGER_END();
   //
   if(i==MAX_TASKLIST){
      return;
   }
   //
   PROTECT_INTERNAL_SEGGER_BEGIN();
   for(i=0;i < MAX_TASKLIST; i++){
      if(!osTaskList[i])continue;

      if(osTaskList[i]->Id!=GetCurrentThreadId()){
         ResumeThread(osTaskList[i]->hTask);
         Sleep(__SLEEP_SEGGER);
      }else
         osTaskList[i]->regionCount--;
   }
   PROTECT_INTERNAL_SEGGER_END();
   //
   LEAVE_INTERRUPT();
   //
}

/*===========================================
End of SourceSeggerInterrupt.c
=============================================*/
