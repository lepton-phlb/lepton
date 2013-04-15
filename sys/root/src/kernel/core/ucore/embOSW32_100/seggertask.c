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

volatile BOOLEAN seggerStarted=FALSE;
OS_TASK* osTaskList[MAX_TASKLIST]={NULL};

/*===========================================
Implementation
=============================================*/



/*-------------------------------------------
| Name:OS_Start
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void OS_Start(void){

   int i;

   seggerStarted=TRUE;

   PROTECT_INTERNAL_SEGGER_BEGIN();
   for(i=0;i < MAX_TASKLIST; i++){
      if(!osTaskList[i])continue;

      ResumeThread(osTaskList[i]->hTask);
   }
   PROTECT_INTERNAL_SEGGER_END();

   while(TRUE)Sleep(200);
}

/*-------------------------------------------
| Name:OS_CreateTask
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void OS_CreateTask(
        OS_TASK* pt,
        char* pName,
        uchar Priority,
        LPTHREAD_START_ROUTINE pRoutine,
        void* pStack,
        uint StackSize,
        uint TimeSlice
        ){
   
   int i=0;

   pt->hEvent=CreateEvent(NULL,FALSE,FALSE,NULL);

   pt->hTask = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)pRoutine, NULL, CREATE_SUSPENDED, &pt->Id ); 

   pt->Name=strdup(pName);
   pt->Priority=Priority;
   pt->StackSize=StackSize;
   pt->TimeSlice=TimeSlice;
   pt->regionCount=0;
   pt->hCurrentWaitObject=NULL;
   pt->Events=0;
   
   PROTECT_INTERNAL_SEGGER_BEGIN();
   for(i=0;i < MAX_TASKLIST; i++){
      if(!osTaskList[i]){
         osTaskList[i]=pt;
         pt->index=i;
         break;
      }
   }
   PROTECT_INTERNAL_SEGGER_END()
      
   if(seggerStarted)
      ResumeThread(pt->hTask);
   
}

/*-------------------------------------------
| Name:OS_Delay
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void OS_Delay(int ms){
   MSG msg;
   int timerId;

   //flush all timer message
   while(PeekMessage(&msg,NULL,WM_TIMER,WM_TIMER,PM_REMOVE));

   timerId=SetTimer(NULL,0,ms,(TIMERPROC) NULL);     // no timer callback 
   GetMessage(&msg,NULL,WM_TIMER,WM_TIMER);
   KillTimer(0,timerId);

   //flush all timer message
   while(PeekMessage(&msg,NULL,WM_TIMER,WM_TIMER,PM_REMOVE));
}

/*-------------------------------------------
| Name:OS_DelayUntil
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
/*
void OS_DelayUntil(int t0) {
}*/

/*-------------------------------------------
| Name:OS_Terminate
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void OS_Terminate (OS_TASK* pt) {
   if(!pt)return ;

   PROTECT_INTERNAL_SEGGER_BEGIN();
   osTaskList[pt->index]=NULL;
   PROTECT_INTERNAL_SEGGER_END();

   TerminateThread(pt->hTask,0);
   CloseHandle(pt->hTask);
  
}

/*-------------------------------------------
| Name:OS_WakeTask
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void OS_WakeTask (OS_TASK* pt) {
   PostThreadMessage(pt->Id,WM_TIMER,0,0);
}

/*-------------------------------------------
| Name:OS_Resume
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void OS_Resume (OS_TASK* pt) {
   return;
}

/*-------------------------------------------
| Name:OS_GetState
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
/*void OS_GetState (OS_TASK* pt) {
   //Unused in Segger RT Kernel
}*/

/*-------------------------------------------
| Name:OS_SetPriority
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
/*void OS_SetPriority (OS_TASK* pt, U8 Priority) {
   //No effect under WIN32
}*/

/*===========================================
End of SourceSeggerTask.c
=============================================*/

