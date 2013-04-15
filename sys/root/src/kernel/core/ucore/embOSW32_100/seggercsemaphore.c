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
| Name:OS_CreateCSema
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int OS_CreateCSema(OS_CSEMA* pCSema,unsigned char InitValue){

   pCSema->hCSem = CreateSemaphore(NULL,(LONG)InitValue,1000,NULL);
   //pCSema->hCSem=CreateEvent(NULL,FALSE,FALSE,NULL);
   pCSema->count=InitValue;

   return 0;
}

/*-------------------------------------------
| Name:OS_SignalCSema
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void OS_DeleteCSema(OS_CSEMA* pCSema){
   if(pCSema->hCSem)
      CloseHandle(pCSema->hCSem);
   pCSema->count=0;
}

/*-------------------------------------------
| Name:OS_SignalCSema
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void OS_SignalCSema(OS_CSEMA* pCSema){
   ReleaseSemaphore(pCSema->hCSem,1,NULL);
   InterlockedIncrement(&pCSema->count);
   //printf("0x%x->(%d)\r\n",(unsigned long)pCSema,pCSema->count);
}

/*-------------------------------------------
| Name:OS_WaitCSema
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void OS_WaitCSema(OS_CSEMA* pCSema){
   OS_TASK* pOsTask;
   
   GET_CURRENT_OS_TASK(pOsTask);
   
   pOsTask->hCurrentWaitObject = pCSema->hCSem;
   WaitForSingleObject(pCSema->hCSem,INFINITE);
   pOsTask->hCurrentWaitObject = NULL;
   //
   InterlockedDecrement(&pCSema->count);
   //printf("0x%x<-(%d)\r\n",(unsigned long)pCSema,pCSema->count);
}

/*-------------------------------------------
| Name:OS_WaitCSemaTimed
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int OS_WaitCSemaTimed(OS_CSEMA* pCSema,int TimeOut){
   int res;
   if(TimeOut>0){
      if((res=WaitForSingleObject(pCSema->hCSem,TimeOut))==WAIT_TIMEOUT)
         return 0;
   }else if(TimeOut==0){
      if((res=WaitForSingleObject(pCSema->hCSem,0))!=WAIT_OBJECT_0)
         return 0;
   }else
      return 0;
   
   //
   InterlockedDecrement(&pCSema->count);
   return 1;
}

/*-------------------------------------------
| Name:OS_GetCSemaValue
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int OS_GetCSemaValue(OS_CSEMA* pCSema){
   return pCSema->count;
}









/*===========================================
End of Source SeggercSemaphore.c
=============================================*/
