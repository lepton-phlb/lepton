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


/*===========================================
Implementation
=============================================*/


/*-------------------------------------------
| Name:OS_CreateRSema
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void OS_CreateRSema (OS_RSEMA* pRS) {
#if defined(USE_SEMAPHORE)
   pRS->ownerThreadId = GetCurrentThreadId();
   pRS->count = 0;
   pRS->hSem = CreateSemaphore (NULL,1,1,NULL);
#elif defined(USE_CSECTION)
   pRS->ownerThreadId=0;
   InitializeCriticalSection(&pRS->sem);
#endif
}

/*-------------------------------------------
| Name:OS_DeleteRSema
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void OS_DeleteRSema (OS_RSEMA* pRS) {
#if defined(USE_SEMAPHORE)
   CloseHandle(pRS->hSem);
#elif defined(USE_CSECTION)
   DeleteCriticalSection(&pRS->sem);
   pRS->ownerThreadId = 0;
#endif
}

/*-------------------------------------------
| Name:OS_Use
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void OS_Use (OS_RSEMA* pRS) {
#if defined(USE_SEMAPHORE)
   //WaitForSingleObject(pRS->hSem,INFINITE);
   if(pRS->ownerThreadId != GetCurrentThreadId()){
      while(WaitForSingleObject(pRS->hSem,100)==WAIT_TIMEOUT);
   }
   pRS->count++;
   pRS->ownerThreadId = GetCurrentThreadId();
   //printf("s[%x]=%d\n",pRS->hSem,pRS->count);
#elif defined(USE_CSECTION)
   EnterCriticalSection(&pRS->sem);
   pRS->ownerThreadId = GetCurrentThreadId();
#endif
}

/*-------------------------------------------
| Name:OS_Unuse
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void OS_Unuse (OS_RSEMA* pRS) {
#if defined(USE_SEMAPHORE)
   if((--pRS->count)==0){
      pRS->ownerThreadId = 0;
      ReleaseSemaphore(pRS->hSem,1,NULL);
   }
   //printf("s[%x]=%d\n",pRS->hSem,pRS->count);
#elif defined(USE_CSECTION)
   pRS->ownerThreadId = 0;
   LeaveCriticalSection(&pRS->sem);
#endif
}

/*-------------------------------------------
| Name:OS_Request
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
char OS_Request (OS_RSEMA* pRS) {
#if defined(USE_SEMAPHORE)
   if(pRS->ownerThreadId != GetCurrentThreadId()){
      if(WaitForSingleObject(pRS->hSem,0)==WAIT_TIMEOUT) return 0;
      pRS->count++;
      pRS->ownerThreadId = GetCurrentThreadId();
   }else{
      pRS->count++;
   }
#elif defined(USE_CSECTION)
   if(!TryEnterCriticalSection(&pRS->sem)) 
      return 0;
#endif
   return 1;
}

/*-------------------------------------------
| Name:OS_GetSemaValue
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int OS_GetSemaValue (OS_RSEMA* pRS) {
  return pRS->count;
}

/*-------------------------------------------
| Name:OS_GetResourceOwner
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
OS_TASK* OS_GetResourceOwner(OS_RSEMA* pRS) {
 int i;

 if(!pRS->ownerThreadId) return NULL;

 for(i=0;i < MAX_TASKLIST; i++){
      if(!osTaskList[i]) continue;
      if(osTaskList[i]->Id == pRS->ownerThreadId)
         return osTaskList[i];
 }

 return NULL;
}



/*===========================================
End of SourceSeggerSemaphore.c
=============================================*/

