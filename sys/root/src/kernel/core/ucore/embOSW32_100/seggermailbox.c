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
| Name:OS_CreateMB
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void OS_CreateMB (OS_MAILBOX* pMB, OS_U8 sizeofMsg, OS_UINT maxnofMsg, void* Buffer){
   
   if(!pMB)return;


   pMB->hSemMB = CreateSemaphore (NULL,1,1,NULL);
   pMB->hEvtMB = CreateEvent(NULL,0,0,NULL);
   pMB->dwThreadId = 0;

   pMB->pData  = Buffer;
   pMB->pRead  = pMB->pData;
   pMB->pWrite = pMB->pData;

   pMB->pEnd   = pMB->pData + ((maxnofMsg-1)*sizeofMsg);

   pMB->maxnofMsg    = maxnofMsg;
   pMB->sizeofMsg    = sizeofMsg;

   pMB->r      = 0;
   pMB->w      = 0;
   pMB->size   = maxnofMsg*sizeofMsg;
   

}

/*-------------------------------------------
| Name:OS_DeleteMB
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void OS_DeleteMB     (OS_MAILBOX* pMB){

   if(!pMB)return;

   pMB->dwThreadId=0;

   CloseHandle(pMB->hSemMB);
   CloseHandle(pMB->hEvtMB);
}

/*-------------------------------------------
| Name:OS_ClearMB
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void OS_ClearMB (OS_MAILBOX* pMB){

   if(!pMB)return;

   //interrupt protection
   ENTER_INTERRUPT();

   //
   WaitForSingleObject(pMB->hSemMB,INFINITE);

   pMB->r      = 0;
   pMB->w      = 0;

   pMB->pRead  = pMB->pData;
   pMB->pWrite = pMB->pData;

   ReleaseSemaphore(pMB->hSemMB,1,NULL);

   //
   LEAVE_INTERRUPT();
   
}
/*-------------------------------------------
| Name:OS_PutMail
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void OS_PutMail (OS_MAILBOX* pMB, void* pmail){
   char * pTmpWrite;

   if(!pMB)return;

   //interrupt protection
   ENTER_INTERRUPT();

   //
   WaitForSingleObject(pMB->hSemMB,INFINITE);

   if(pMB->pWrite == pMB->pEnd){
      pTmpWrite = pMB->pData;
   }
   else{
      pTmpWrite=pMB->pWrite+pMB->sizeofMsg;
   }

   if(pTmpWrite==pMB->pRead) {
      //
      ReleaseSemaphore(pMB->hSemMB,1,NULL);
      //
      LEAVE_INTERRUPT();
      return;
   }
 
   memcpy(pMB->pWrite,pmail,pMB->sizeofMsg);

   pMB->pWrite=pTmpWrite;

   SetEvent(pMB->hEvtMB);
   /*
   if(pMB->dwThreadId){
      PostThreadMessage(pMB->dwThreadId,OS_MAIL_POSTED,0L,0L);
   }
   */

   ReleaseSemaphore(pMB->hSemMB,1,NULL);

   //
   LEAVE_INTERRUPT();
}

/*-------------------------------------------
| Name:OS_PutMailCond
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
char OS_PutMailCond (OS_MAILBOX* pMB, void* pmail){

   char * pTmpWrite;

   if(!pMB)return 1;

   //interrupt protection
   ENTER_INTERRUPT();

   WaitForSingleObject(pMB->hSemMB,INFINITE);


   if(pMB->pWrite == pMB->pEnd){
      pTmpWrite = pMB->pData;
   }
   else{
      pTmpWrite=pMB->pWrite+pMB->sizeofMsg;
   }

   if(pTmpWrite==pMB->pRead) {
      //
      ReleaseSemaphore(pMB->hSemMB,1,NULL);
      //
      LEAVE_INTERRUPT();
      return 1;
   }

   memcpy(pMB->pWrite,pmail,pMB->sizeofMsg);

   pMB->pWrite=pTmpWrite;

   SetEvent(pMB->hEvtMB);
   /*
   if(pMB->dwThreadId){
      PostThreadMessage(pMB->dwThreadId,OS_MAIL_POSTED,0L,0L);
   }*/

   ReleaseSemaphore(pMB->hSemMB,1,NULL);

   //
   LEAVE_INTERRUPT();
   
   return 0;
}

/*-------------------------------------------
| Name:OS_PutMail1
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void OS_PutMail1 (OS_MAILBOX* pMB, void* pmail){
   OS_PutMail (pMB,pmail);
}

/*-------------------------------------------
| Name:OS_PutMailCond1
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
char OS_PutMailCond1 (OS_MAILBOX* pMB, void* pmail){
   return OS_PutMailCond (pMB,pmail);
}

/*-------------------------------------------
| Name:OS_GetMail
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void OS_GetMail (OS_MAILBOX* pMB, void* Result){

   while(pMB->pRead==pMB->pWrite)
      WaitForSingleObject(pMB->hEvtMB,INFINITE);

   //
   //interrupt protection
   ENTER_INTERRUPT();

   //
   WaitForSingleObject(pMB->hSemMB,INFINITE);
   
   memcpy(Result,pMB->pRead,pMB->sizeofMsg);

   if(pMB->pRead == pMB->pEnd){
      pMB->pRead = pMB->pData;
   }
   else{
      pMB->pRead=pMB->pRead+pMB->sizeofMsg;
   }
  
   pMB->dwThreadId=0;
   ReleaseSemaphore(pMB->hSemMB,1,NULL);

   //
   LEAVE_INTERRUPT();
 
}

/*-------------------------------------------
| Name:OS_GetMailTimed
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
char OS_GetMailTimed (OS_MAILBOX* pMB, void* Result,int TimeOut){

   int end_time = 0, start_time = 0;
   
   while(pMB->pRead==pMB->pWrite)
      if(WaitForSingleObject(pMB->hEvtMB,TimeOut)==WAIT_TIMEOUT)
         return 1;

   //
   //interrupt protection
   ENTER_INTERRUPT();

   //
   if(WaitForSingleObject(pMB->hSemMB,TimeOut)==WAIT_TIMEOUT)
      return 1;
   
   memcpy(Result,pMB->pRead,pMB->sizeofMsg);

   if(pMB->pRead == pMB->pEnd){
      pMB->pRead = pMB->pData;
   }
   else{
      pMB->pRead=pMB->pRead+pMB->sizeofMsg;
   }
  
   pMB->dwThreadId=0;
   ReleaseSemaphore(pMB->hSemMB,1,NULL);

   //
   LEAVE_INTERRUPT();

   return 0;
}

/*-------------------------------------------
| Name:OS_GetMailCond
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
char OS_GetMailCond (OS_MAILBOX* pMB, void* Result){
   
   
   //interrupt protection
   ENTER_INTERRUPT();

   WaitForSingleObject(pMB->hSemMB,INFINITE);
  
   if(pMB->pRead==pMB->pWrite) {
      ReleaseSemaphore(pMB->hSemMB,1,NULL);

      //
      LEAVE_INTERRUPT();
      return 1;
   }
   
   memcpy(Result,pMB->pRead,pMB->sizeofMsg);

   if(pMB->pRead == pMB->pEnd){
      pMB->pRead = pMB->pData;
   }
   else{
      pMB->pRead=pMB->pRead+pMB->sizeofMsg;
   }

   ReleaseSemaphore(pMB->hSemMB,1,NULL);

   //
   LEAVE_INTERRUPT();

   return 0;
}

/*-------------------------------------------
| Name:OS_GetMail1
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void OS_GetMail1 (OS_MAILBOX* pMB, void* Result){
   OS_GetMail (pMB,Result);
}

/*-------------------------------------------
| Name:OS_GetMailCond1
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
char OS_GetMailCond1 (OS_MAILBOX* pMB, void* Result){
   return OS_GetMailCond (pMB,Result);
}

/*===========================================
End of SourceSeggerMailBox.c
=============================================*/
