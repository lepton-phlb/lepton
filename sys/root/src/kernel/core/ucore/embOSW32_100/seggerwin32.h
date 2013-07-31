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
Compiler Directive
=============================================*/
#ifndef _SEGGERWIN32_H
#define _SEGGERWIN32_H


/*===========================================
Includes
=============================================*/
#define _WIN32_WINNT 0x0400
//#include "kernel/core/windows.h"
#include "kernel/core/ucore/embOSW32_100/win32/windows.h"
#include "kernel/core/ucore/embOSW32_100/segger_intr.h"

/*===========================================
Declaration
=============================================*/
#define __SLEEP_SEGGER 0 /*1 or 0*/

#ifndef U8
  #define U8 unsigned char
#endif

#ifndef OS_U8
  #define OS_U8 U8
#endif

#ifndef U32
  #define U32 unsigned long
#endif

#ifndef uchar
  #define uchar unsigned char
#endif

#ifndef schar
  #define schar signed char
#endif

#ifndef uint
  #define uint unsigned int
#endif

#ifndef ulong
  #define ulong unsigned long
#endif

#ifndef OS_I8
  #define OS_I8 signed char
#endif  

#ifndef OS_U8
  #define OS_U8 unsigned char
#endif

#ifndef OS_I16
  #define OS_I16 signed short
#endif

#ifndef OS_U16
  #define OS_U16 unsigned short
#endif

#ifndef OS_I32
  #define OS_I32 long
#endif

#ifndef OS_U32
  #define OS_U32 unsigned OS_I32
#endif


/*
 defines the "true" integers that operations can be handled on in a single
 operation.
 For 16-bit processors, a "true int" is 16 bit, defined by int.
 For some  8-bit processors, a "true int" is  8 bit, defined by short.
*/
#ifndef OS_SHORT
  #define OS_SHORT        short
#endif

#define OS_USHORT unsigned OS_SHORT

#ifndef OS_INT
  #define OS_INT        int
#endif

#ifndef OS_UINT
  #define OS_UINT       unsigned OS_INT
#endif

#define OS_STACKPTR  //near for IAR

//typedef void (voidRoutine)(void);

#define OS_CPU "WIN32/MS VC++ 6.0"
#define OS_CPU_VERSION 306

HANDLE hSeggerProcess;

extern  volatile OS_I32 OS_Time;
#define OS_GetTime()   ((int)OS_Time)

#define OS_DI()
#define OS_EI()

//TASK

typedef struct {

   HANDLE hTask;
   DWORD  Id; //Preserved compatiblity with Segger 

   HANDLE hSemTask;

   const char*  Name;
   uchar Priority;

   void* pStack;
   uint StackSize;
   uint TimeSlice;

   HANDLE hEvent;
   U8  Events;

   CONTEXT context;

   CONTEXT bckup_context;
   char*   bckup_stack;
   
   //specific for win32 task management
   int index;
   int regionCount;

   HANDLE hCurrentWaitObject;


}OS_TASK;


typedef struct {

   HANDLE hTask;
   DWORD  Id; //Preserved compatiblity with Segger 

   HANDLE hSemTask;

   char*  pName;
   uchar Priority;

   void* pStack;
   uint StackSize;
   uint TimeSlice;

   HANDLE hEvent;
   U8  Events;
  
   
   //specific for win32 task management
   int index;

}OS_TASK2;


extern volatile BOOLEAN seggerStarted;

#define  MAX_TASKLIST      30
extern   OS_TASK*          osTaskList[MAX_TASKLIST];

#define GET_CURRENT_OS_TASK(POS_TASK){\
   int i;\
   for(i=0;i < MAX_TASKLIST; i++){\
      if(osTaskList[i]){\
         if(osTaskList[i]->Id!=GetCurrentThreadId())continue;\
         POS_TASK=osTaskList[i];\
         break;\
      }\
   }\
}
         

#define OS_STACKPTR

void OS_Start(void);

void OS_CreateTask(
        OS_TASK* pt,
        char* Name,
        uchar Priority,
        LPTHREAD_START_ROUTINE pRoutine,
        void OS_STACKPTR* pStack,
        uint StackSize,
        uint TimeSlice
        );


void OS_Delay(int ms);
void OS_DelayUntil(int t0);
void OS_Terminate (OS_TASK*);
void OS_WakeTask (OS_TASK*);
void OS_Resume (OS_TASK* pt);
void OS_GetState (OS_TASK*);
void OS_SetPriority (OS_TASK*, U8);
OS_TASK* OS_GetpCurrentTask (void);


//Counting Semaphores
typedef struct {
   HANDLE hCSem;
   int count;
}OS_CSEMA;

int   OS_CreateCSema    (OS_CSEMA* pCSema,unsigned char InitValue);
void  OS_DeleteCSema    (OS_CSEMA* pCSema);
void  OS_SignalCSema    (OS_CSEMA* pCSema);
void  OS_WaitCSema      (OS_CSEMA* pCSema);
int   OS_WaitCSemaTimed (OS_CSEMA* pCSema,int TimeOut);
int   OS_GetCSemaValue  (OS_CSEMA* pCSema);


//Ressources Semaphores
#define USE_SEMAPHORE
typedef struct {
#if defined(USE_SEMAPHORE)
   HANDLE   hSem;
#elif defined(USE_CSECTION)
   CRITICAL_SECTION sem;
#endif
   DWORD  ownerThreadId; //owner
   int count;   
}OS_RSEMA;

void     OS_CreateRSema       (OS_RSEMA* );
void     OS_DeleteRSema       (OS_RSEMA* );
void     OS_Use               (OS_RSEMA* );
void     OS_Unuse             (OS_RSEMA* );
char     OS_Request           (OS_RSEMA* );
int      OS_GetUseCnt         (OS_RSEMA* );
int      OS_GetSemaValue      (OS_RSEMA* );
OS_TASK* OS_GetResourceOwner  (OS_RSEMA* );


//EVENT
#define OS_EVENT_POSTED    WM_APP+0x01

void OS_ClearEvents      (OS_TASK*);
char OS_GetEventsOccured (OS_TASK*);
void OS_SignalEvent      (char Event, OS_TASK*);
char OS_WaitEvent        (char EventMask);
char OS_WaitEventTimed   (char EventMask, int TimeOut);


//MAIL BOX
#define OS_MAIL_POSTED     WM_APP+0x02

   typedef struct {

      HANDLE hSemMB;
      HANDLE hEvtMB;
      DWORD  dwThreadId;

      int size;
      int r;
      int w;

      char * pData;
      char * pEnd;

      char * pWrite;
      char * pRead;

      OS_U8    sizeofMsg;
      OS_UINT  maxnofMsg;

   } OS_MAILBOX;


   void OS_CreateMB     (OS_MAILBOX*, OS_U8 sizeofMsg, OS_UINT maxnofMsg, void* Buffer);
   void OS_ClearMB      (OS_MAILBOX*);
   void OS_PutMail      (OS_MAILBOX*, void* pmail);
   char OS_PutMailCond  (OS_MAILBOX*, void* pmail);
   void OS_GetMail      (OS_MAILBOX*, void* Result);
   char OS_GetMailCond  (OS_MAILBOX*, void* Result);
   void OS_PutMail1     (OS_MAILBOX*, void* pmail);
   char OS_PutMailCond1 (OS_MAILBOX*, void* pmail);
   char OS_GetMailTimed (OS_MAILBOX* pMB, void* Result,int TimeOut);
   void OS_GetMail1     (OS_MAILBOX*, void* Result);
   char OS_GetMailCond1 (OS_MAILBOX*, void* Result);
   void OS_DeleteMB     (OS_MAILBOX*);


//TIMER

#define OS_START_TIMER        WM_APP+0x03
#define OS_STOP_TIMER         WM_APP+0x04
#define OS_RETRIGGERED_TIMER  WM_APP+0x05

typedef  void (*OS_TIMERROUTINE)(void) ;

typedef struct {

   volatile uint           timerId;
   volatile uint           timeout;
   volatile long           lPeriod;
   volatile long           tickCountStart;

   OS_TIMERROUTINE pfnCallback;

   volatile HANDLE   hThread;
   DWORD    dwThreadId;

   volatile DWORD dwStart;
   volatile DWORD dwRunning;
   
}OS_TIMER;

void OS_CreateTimer     (OS_TIMER*, OS_TIMERROUTINE, uint);
void OS_DeleteTimer     (OS_TIMER*);
void OS_StopTimer       (OS_TIMER*);
void OS_StartTimer      (OS_TIMER*);
void OS_RetriggerTimer  (OS_TIMER*);
uint OS_GetTimerValue   (OS_TIMER*);
uint OS_GetTimerStatus  (OS_TIMER*);
void OS_SetTimerPeriod  (OS_TIMER*, uint);
OS_TIMER* OS_GetpCurrentTimer(void);

//protect internal operation (see createtask and task list)
extern CRITICAL_SECTION seggerCriticalSection; 

#define PROTECT_INTERNAL_SEGGER_BEGIN()\
   EnterCriticalSection(&seggerCriticalSection); 

#define PROTECT_INTERNAL_SEGGER_END()\
   LeaveCriticalSection(&seggerCriticalSection); 

//INTERRUPT
//extern volatile HANDLE hSemInterrupt;
extern CRITICAL_SECTION interruptCriticalSection; 

#define ENTER_INTERRUPT()\
   EnterCriticalSection(&interruptCriticalSection); 

#define LEAVE_INTERRUPT()\
   LeaveCriticalSection(&interruptCriticalSection); 


void OS_EnterInterrupt(void);
void OS_LeaveInterrupt(void);

void OS_EnterNestableInterrupt(void);
void OS_LeaveNestableInterrupt(void);

//ENABLE DISABLE TASK SWITCHING
void OS_EnterRegion(void);
void OS_LeaveRegion(void);

//
void OS_InitKern(void);
void OS_InitHW(void);

//Specific WIN32
void initSeggerWin32(void);

#endif