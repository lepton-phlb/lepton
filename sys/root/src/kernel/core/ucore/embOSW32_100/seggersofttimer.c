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
#define TIMER_MAXRESPONSE  2000 // 2 sec

static volatile DWORD dwTimerIndex=-1;

/*===========================================
Implementation
=============================================*/



/*-------------------------------------------
| Name:timerTask
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
DWORD  timerTask(LPVOID lpParameter) {

   MSG Msg;

   OS_TIMER* pTM = (OS_TIMER*)lpParameter;

   unsigned long* pData; 
   // Initialize the TLS index for this thread. 
   pData = (unsigned long*) LocalAlloc(LPTR, sizeof(unsigned long)); 
   if (! TlsSetValue(dwTimerIndex, pData)) 
      return -1;
   (*pData)=(unsigned long)pTM;

   //
   //pTM->timerId=SetTimer(NULL,0,pTM->timeout,NULL);
   //pTM->dwStart = GetTickCount();

   //to force the system to create the message queue.(see msdn)
   PeekMessage(&Msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);

   //
   while (GetMessage(&Msg, NULL, 0, 0)) {
      
      //
      switch(Msg.message){

         case OS_START_TIMER:
            KillTimer(0,pTM->timerId);
            pTM->timerId=SetTimer(NULL,0,pTM->timeout,NULL);
            pTM->tickCountStart = GetTickCount(); 
            pTM->dwStart=0;
            pTM->dwRunning   = 1;
         break;

         case OS_STOP_TIMER:
            //TO DO:(See Segger Documentation)The  actual  value  of  the  timer  
            //(the  time  until  expiration)  is  kept  until 
            //OS_StartTimer() lets the timer continue. 
            KillTimer(0,pTM->timerId);
            pTM->dwStart=0;
            pTM->dwRunning   = 0;

            //Remove All WM_TIMER MESSAGE
            PeekMessage(&Msg,NULL,WM_TIMER,WM_TIMER,PM_REMOVE);
         break;

         case WM_TIMER:
            //
            KillTimer(0,pTM->timerId);
            //Remove All WM_TIMER MESSAGE
            PeekMessage(&Msg,NULL,WM_TIMER,WM_TIMER,PM_REMOVE);
            if(!seggerStarted){
               //OS not started retrigged timer
               pTM->timerId=SetTimer(NULL,0,pTM->timeout,NULL);
               pTM->tickCountStart = GetTickCount(); 
               continue;
            }

            //Simulate Segger Timer callback
            OS_EnterInterrupt();
            if(pTM->pfnCallback)pTM->pfnCallback();
            //Simulate Segger Timer callback
            OS_LeaveInterrupt();
           
            pTM->dwStart=0;
            pTM->dwRunning   = 0;
            
         break;

         case OS_RETRIGGERED_TIMER:
             KillTimer(0,pTM->timerId);
             //Remove All WM_TIMER MESSAGE
             PeekMessage(&Msg,NULL,WM_TIMER,WM_TIMER,PM_REMOVE);

             pTM->timerId=SetTimer(NULL,0,pTM->timeout,NULL);
             pTM->tickCountStart = GetTickCount(); 
             pTM->dwStart=0;
             pTM->dwRunning   = 1;
         break;

      }

	   DispatchMessage(&Msg);
	};
   

   KillTimer(0,pTM->timerId);
   pTM->dwRunning   = 0;

   return 0;
}

/*--------------------------------------------
| Name:        OS_GetpCurrentTimer
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
OS_TIMER* OS_GetpCurrentTimer(void){
   unsigned long* pData=TlsGetValue(dwTimerIndex); 
   OS_TIMER* pTM;

   pTM=(OS_TIMER*)(*pData);

   return pTM;
}

/*-------------------------------------------
| Name:OS_CreateTimer
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void OS_CreateTimer (OS_TIMER* pTM, OS_TIMERROUTINE callback, uint timeout){
   // tls creation
   if(dwTimerIndex==-1)
      if((dwTimerIndex = TlsAlloc()) == -1) 
         return;
       
    //TO DO: Create Semaphore for Multiple Access??????
    if(pTM==NULL) return;

    pTM->timeout     = timeout;
    pTM->pfnCallback = callback;
    pTM->dwRunning   = 0;

    if(pTM->hThread!=NULL) return;//ERROR TIMER Already exist

    pTM->hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)timerTask,pTM,0, &pTM->dwThreadId);
    Sleep(100);

    //start timer(see segger documentation).
    //while(!(PostThreadMessage(pTM->dwThreadId,OS_START_TIMER,0,0)))Sleep(100);
}

/*-------------------------------------------
| Name:OS_StartTimer
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void OS_StartTimer (OS_TIMER* pTM){

   //Start Timer
   if(pTM==NULL || pTM->hThread==NULL) return;
   //PostThreadMessage(pTM->dwThreadId,OS_START_TIMER,0,0);
   while(!(PostThreadMessage(pTM->dwThreadId,OS_START_TIMER,0,0)))Sleep(100);
}

/*-------------------------------------------
| Name:OS_StopTimer
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void OS_StopTimer (OS_TIMER* pTM){

   //Stop Timer
   if(pTM==NULL || pTM->hThread==NULL) return;
   PostThreadMessage(pTM->dwThreadId,OS_STOP_TIMER,0,0);
   
}

/*-------------------------------------------
| Name:OS_RetriggerTimer
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void OS_RetriggerTimer (OS_TIMER* pTM){

  //Restart Timer
  if(pTM==NULL || pTM->hThread==NULL) return;
  PostThreadMessage(pTM->dwThreadId,OS_RETRIGGERED_TIMER,0,0);
}

/*-------------------------------------------
| Name:OS_SetTimerPeriod
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void OS_SetTimerPeriod (OS_TIMER* pTM, uint period) {
   pTM->timeout  = period;
}

/*-------------------------------------------
| Name:OS_GetTimerValue
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
uint OS_GetTimerValue(OS_TIMER* pTM) {

   if(pTM==NULL)
      return 0;
   if(!pTM->dwRunning)
      return pTM->timeout;

   return (uint)(pTM->timeout-(GetTickCount() - pTM->tickCountStart));
}


/*-------------------------------------------
| Name:OS_GetTimerStatus
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
uint OS_GetTimerStatus (OS_TIMER* pTM) {

   if(pTM==NULL) return 0;

   return (uint)(pTM->dwRunning);
}

/*-------------------------------------------
| Name:OS_DeleteTimer
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void OS_DeleteTimer   (OS_TIMER* pTM){

   if(pTM==NULL || pTM->hThread==NULL) return;

   //TO DO:Remove this code after the next release
   //ResumeThread(pTM->hThread);

   PostThreadMessage(pTM->dwThreadId,WM_QUIT,0,0);

   if(WaitForSingleObject(pTM->hThread,TIMER_MAXRESPONSE)==WAIT_TIMEOUT){
      //OS_DeleteTimer Timeout;
      Sleep(10);
   }

   TerminateThread(pTM->hThread,0);
   CloseHandle(pTM->hThread);

   pTM->hThread=NULL;
   
}

/*===========================================
End of SourceSeggerSoftTimer.c
=============================================*/






