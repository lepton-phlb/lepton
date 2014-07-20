/*
The contents of this file are subject to the Mozilla Public License Version 1.1
(the "License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License for the
specific language governing rights and limitations under the License.

The Original Code is Lepton.

The Initial Developer of the Original Code is Chauvin-Arnoux.
Portions created by Chauvin-Arnoux are Copyright (C) 2011. All Rights Reserved.

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
#include <windows.h>
#include <commdlg.h>
#include <string.h>
#include <stdio.h>
#include <conio.h>
#include <io.h>
#include <memory.h>

#include "kernel/dev/arch/win32/dev_win32_com2/dev_win32uart.h"
#include "kernel/core/ucore/embOSW32_100/segger_intr.h"

/*===========================================
Global Declaration
=============================================*/
#define MAX_BUFFER   255
#define USE_COM      "COM2"
#define DFLT_SPEED      9600
#define DFLT_PARITY     'N'
#define DFLT_DATA       8
#define DFLT_STOPBIT    1

static uart_config current_config;

//
volatile static HANDLE hCom;
static HANDLE hComThread;
static DWORD dwThreadID;

static OVERLAPPED osWrite, osRead;

#define COM_STOP  TRUE
#define COM_START FALSE

volatile static BOOLEAN bComStopped=FALSE;

volatile static BOOLEAN rcv_interrupt_enable=FALSE;
volatile static BOOLEAN snd_interrupt_enable=FALSE;

static int rcvLength;
static int sndLength;

static char rcvBuffer[MAX_BUFFER];
static char sndBuffer[MAX_BUFFER];

volatile static char rcvData;

static unsigned char slipBuffer[255];

static int nRS232PhysicalSimTimeOut=50; //ms//INFINITE;
static HANDLE hRS232PhysicalSimEvent;


/*===========================================
Implementation
=============================================*/


/*-------------------------------------------
| Name:_readCom
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _readCom(HANDLE hCom,char*  buffer, UINT bufferlen )
{
   BOOL fReadStat;
   COMSTAT ComStat;
   DWORD dwErrorFlags;
   DWORD dwLength;
   DWORD dwError;

   // only try to read number of bytes in queue
   ClearCommError( hCom, &dwErrorFlags, &ComStat );
   dwLength = min( (DWORD) bufferlen, ComStat.cbInQue );

   if (dwLength > 0)
   {
      fReadStat = ReadFile( hCom, buffer,dwLength, &dwLength, &osRead );
      if (!fReadStat)
      {
         if (GetLastError() == ERROR_IO_PENDING)
         {
            printf("\n\rIO Pending");

            while(!GetOverlappedResult( hCom,&osRead, &dwLength, TRUE ))
            {
               dwError = GetLastError();
               if(dwError == ERROR_IO_INCOMPLETE)
                  continue;
               else
               {
                  // an error occurred, try to recover
                  ClearCommError( hCom, &dwErrorFlags, &ComStat );
                  break;
               }
            }
         }
         else
         {
            // some other error occurred
            dwLength = 0;
            ClearCommError( hCom, &dwErrorFlags, &ComStat );
         }
      }
   }

   return dwLength;
}

/*-------------------------------------------
| Name:_writeCom
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _writeCom(HANDLE hCom,char*  buffer, UINT bufferlen )
{
   BOOL fWriteStat;
   DWORD dwBytesWritten;
   DWORD dwErrorFlags;
   DWORD dwError;
   DWORD dwBytesSent=0;
   COMSTAT ComStat;

   fWriteStat = WriteFile( hCom, buffer, bufferlen,
                           &dwBytesWritten, &osWrite );

   if (!fWriteStat)
   {
      if((dwError=GetLastError()) == ERROR_IO_PENDING)
      {
         while(!GetOverlappedResult( hCom,
                                     &osWrite, &dwBytesWritten, /*TRUE*/ FALSE ))
         {
            Sleep(/*0*/ 1);
            dwError = GetLastError();
            if(dwError == ERROR_IO_INCOMPLETE)
            {
               // normal result if not finished
               dwBytesSent += dwBytesWritten;
               continue;
            }
            else
            {
               ClearCommError( hCom, &dwErrorFlags, &ComStat );
               break;
            }
         }

         dwBytesSent += dwBytesWritten;

      }
      else
      {
         // some other error occurred
         ClearCommError( hCom, &dwErrorFlags, &ComStat );
         return -1;
      }
   }

   return dwBytesSent;
}

/*-------------------------------------------
| Name:readWin32UartReceiveRegister
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
unsigned char readWin32UartReceiveRegister(void) {

   unsigned char c = rcvData;

   //Synchro
   SetEvent(hRS232PhysicalSimEvent);
   return c;
}

/*-------------------------------------------
| Name:WriteWin32UartTransmitRegister
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void writeWin32UartTransmitRegister(char reg)
{
   _writeCom(hCom,&reg,1);

   if(snd_interrupt_enable)
      emuFireInterrupt(68);
}

/*-------------------------------------------
| Name:comThread
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
DWORD comThread (LPVOID lpParameter)
{
   int i;

   printf("uart wait on %s...\n",USE_COM);
   while ( !bComStopped )
   {
      DWORD dwEvtMask = 0;

      WaitCommEvent( hCom, &dwEvtMask, NULL );
      if ((dwEvtMask & EV_RXCHAR) == EV_RXCHAR)
      {
         //printf("event\n");
         do
         {
            if (rcvLength  = _readCom( hCom, rcvBuffer, MAX_BUFFER  ))
            {

               for(i=0; i<rcvLength; i++)
               {
                  //printf("%c\n",rcvBuffer[i]);
                  Sleep(/*5*/ 0);
                  //WriteRS232ReceiveRegister(rcvBuffer[i]);
                  rcvData = rcvBuffer[i];
                  if(rcv_interrupt_enable) {
                     emuFireInterrupt(72);
                     //Synchro
                     WaitForSingleObject(hRS232PhysicalSimEvent,nRS232PhysicalSimTimeOut);
                  }

               }
            }
         } while ( rcvLength > 0 );
      }
      if ( (dwEvtMask & EV_TXEMPTY) == EV_TXEMPTY )
      {

      }
   }

   // get rid of event handle
   dwThreadID = 0;

   return 0;
}

/*-------------------------------------------
| Name:setRcvInterrupt
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void setRcvInterrupt(int enable){
   rcv_interrupt_enable = enable;
}

/*-------------------------------------------
| Name:setRecvInterrupt
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void setSndInterrupt(int enable){
   snd_interrupt_enable = enable;
}

/*-------------------------------------------
| Name:getRs2322
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int getRs232(uart_config* config){
   memcpy(config,&current_config,sizeof(uart_config));
   return 0;
}

/*-------------------------------------------
| Name:setRs2322
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int setRs232(uart_config* config){
   COMMCONFIG commconfig;
   DWORD dwSize=sizeof(COMMCONFIG);
   char buffer[256]={0};
   sprintf(buffer,"%s: baud=%d parity=%c data=%d stop=%d",USE_COM,
           config->speed,
           config->parity,
           config->data,
           config->stop);

   if(!GetCommConfig(hCom,&commconfig,&dwSize))
      return -1;

   BuildCommDCB(buffer,&commconfig.dcb);

   //CommConfigDialog(USE_COM,NULL,&commconfig);

   if(!SetCommConfig(hCom,&commconfig,dwSize))
      return -1;

   printf("com cfg: %s\n",buffer);

   memcpy(&current_config,config,sizeof(uart_config));


   return 0;
}

/*-------------------------------------------
| Name:startAsyncRs232
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int startAsyncRs232(void)
{
   uart_config config={DFLT_SPEED,DFLT_PARITY,DFLT_DATA,DFLT_STOPBIT};

   hRS232PhysicalSimEvent=CreateEvent(NULL,FALSE,FALSE,NULL);

   hCom = CreateFile(USE_COM, GENERIC_READ | GENERIC_WRITE,
                     0,
                     NULL,
                     OPEN_EXISTING,
                     FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
                     NULL );
   if(hCom==INVALID_HANDLE_VALUE)
      return -1;

   //set comm
   setRs232(&config);

   // purge any information in the buffer
   PurgeComm( hCom, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR );

   //
   ClearCommBreak(hCom);

   //
   //EscapeCommFunction( hCom, SETDTR ) ;

   // get any early notifications
   SetCommMask(hCom, EV_RXFLAG|EV_RXCHAR|EV_TXEMPTY);

   // setup device buffers
   SetupComm( hCom, 4096, 4096 );


   //
   bComStopped=COM_START;
   hComThread = CreateThread( (LPSECURITY_ATTRIBUTES) NULL,
                              0,
                              (LPTHREAD_START_ROUTINE) comThread,
                              NULL,
                              0, &dwThreadID );
   if(hComThread== INVALID_HANDLE_VALUE)
      return -1;



   printf("uart started\n");

   return 0;
}

/*-------------------------------------------
| Name:stopAsyncRs232
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int stopAsyncRs232(void)
{
   // set connected flag to FALSE
   bComStopped=COM_STOP;

   // disable event notification and wait for thread
   // to halt
   SetCommMask( hCom, 0 );

   // block until thread has been halted
   while(dwThreadID)
      Sleep(10);

   // drop DTR
   EscapeCommFunction( hCom, CLRDTR );

   // purge any outstanding reads/writes and close device handle

   PurgeComm( hCom, PURGE_TXABORT | PURGE_RXABORT |
              PURGE_TXCLEAR | PURGE_RXCLEAR );

   CloseHandle(hCom);

   return 0;
}



/*===========================================
End of Sourcedev_win32uart.c
=============================================*/
