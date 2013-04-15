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
#define OS_EVENT_MASK     0x000000FF

/*===========================================
Implementation
=============================================*/

/*-------------------------------------------
| Name:OS_WaitEvent
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
char OS_WaitEvent (char EventMask){

   int i;
   DWORD res;
   char events;

   for(i=0;i < MAX_TASKLIST; i++){
      if(!osTaskList[i])continue;

      if(osTaskList[i]->Id==GetCurrentThreadId()) 
         break;
   }
   //printf("->waitevent %s\r\n",osTaskList[i]->Name);
   if(!(osTaskList[i]->Events & EventMask))
      while( (res=WaitForSingleObject(osTaskList[i]->hEvent,100))!=WAIT_OBJECT_0
         || !(osTaskList[i]->Events & EventMask)){
         if(res==WAIT_OBJECT_0)
            //printf("!!rcv evt %s [0x%04x] from 0x%08x \r\n",osTaskList[i]->Name,osTaskList[i]->Events,osTaskList[i]->hEvent);
            Sleep(__SLEEP_SEGGER);
      }
   //printf("<<rcv evt %s [0x%04x] from 0x%08x \r\n",osTaskList[i]->Name,osTaskList[i]->Events,osTaskList[i]->hEvent);
   events = osTaskList[i]->Events;
   osTaskList[i]->Events=0;

   //printf("<-waitevent %s\r\n",osTaskList[i]->Name);

   Sleep(__SLEEP_SEGGER);

   

   return events;

}

/*-------------------------------------------
| Name:OS_WaitEventTimed
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
char OS_WaitEventTimed (char EventMask, int TimeOut){

   int i;
   DWORD res;
   char events;

   for(i=0;i < MAX_TASKLIST; i++){
      if(!osTaskList[i])continue;

      if(osTaskList[i]->Id==GetCurrentThreadId()) 
         break;
   }

   //printf("->waitevent timed\r\n",osTaskList[i]->Name);

   while(!(osTaskList[i]->Events & EventMask)){
      if((res=WaitForSingleObject(osTaskList[i]->hEvent,TimeOut))==WAIT_TIMEOUT){
         Sleep(__SLEEP_SEGGER);
         return 0;
      }
   }

   events = osTaskList[i]->Events;
   osTaskList[i]->Events=0;

   Sleep(__SLEEP_SEGGER);

   return events;

}

/*-------------------------------------------
| Name:OS_SignalEvent
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void OS_SignalEvent (char Event, OS_TASK* pt){

   pt->Events|=(char)(Event&OS_EVENT_MASK);
   //printf(">>snd evt %s [0x%04x] from 0x%08x \r\n",pt->Name,pt->Events,pt->hEvent);
   SetEvent(pt->hEvent);
   Sleep(__SLEEP_SEGGER);
}

/*-------------------------------------------
| Name:OS_GetEventsOccured
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
char OS_GetEventsOccured (OS_TASK* pt){
   return pt->Events;
}

/*-------------------------------------------
| Name:OS_ClearEvents
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void OS_ClearEvents (OS_TASK* pt){
   pt->Events=0;
}

/*===========================================
End of SourceSeggerEvent.c
=============================================*/



