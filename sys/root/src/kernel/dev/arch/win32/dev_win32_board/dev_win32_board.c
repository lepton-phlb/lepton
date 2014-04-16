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
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#include "kernel/core/system.h"
#include "kernel/core/process.h"
#include "kernel/core/stat.h"
#include "kernel/core/ioctl_board.h"
#include "kernel/fs/vfs/vfsdev.h"
#include "kernel/fs/vfs/vfstypes.h"

#include <mmsystem.h>

/*===========================================
Global Declaration
=============================================*/
//win32 sound card
#define BUFFER_SIZE 4096
#define SAMPLERATE 44100
#define CANAUX 1 // mono
#define OCTETS 2 // 16 bit

static const double pi = 3.1415926535897932;

PBYTE Buffer;
static HWAVEOUT hWaveOut;
static PBYTE pBufferOut1, pBufferOut2, pBufferOut3;
static PWAVEHDR pWaveHdrOut1, pWaveHdrOut2, pWaveHdrOut3;
static WAVEFORMATEX waveform;

//
static HANDLE h_soundcard_thread;
static DWORD soundcard_thread_id;

static volatile int g_frequency=2000;
static volatile int g_duration= 200;
static volatile int g_duration_loop = 0; //ms


//
const char dev_win32_board_name[]="board\0";

int dev_win32_board_load(void);
int dev_win32_board_open(desc_t desc, int o_flag);
int dev_win32_board_close(desc_t desc);
int dev_win32_board_isset_read(desc_t desc);
int dev_win32_board_isset_write(desc_t desc);
int dev_win32_board_read(desc_t desc, char* buf,int size);
int dev_win32_board_write(desc_t desc, const char* buf,int size);
int dev_win32_board_seek(desc_t desc,int offset,int origin);
int dev_win32_board_ioctl(desc_t desc,int request,va_list ap);

dev_map_t dev_win32_board_map={
   dev_win32_board_name,
   S_IFCHR,
   dev_win32_board_load,
   dev_win32_board_open,
   dev_win32_board_close,
   dev_win32_board_isset_read,
   dev_win32_board_isset_write,
   dev_win32_board_read,
   dev_win32_board_write,
   dev_win32_board_seek,
   dev_win32_board_ioctl //ioctl
};

static int soundcard_loaded =0;

/*===========================================
Implementation
=============================================*/



/*--------------------------------------------
| Name:        soundcard_thread
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
DWORD WINAPI soundcard_thread(void* p){
   MSG Msg;

   MMRESULT mmres;

   long int i;
   unsigned long m_nSig;
   double m_fSim;
   double m_fs;

   m_nSig=0;
   m_fSim=SAMPLERATE;
   m_fs=g_frequency;

   printf("soundcard thread started!\r\n");

   //
   SetThreadPriority (GetCurrentThread (), THREAD_PRIORITY_TIME_CRITICAL);

   while (GetMessage (&Msg, NULL, 0, 0) == TRUE) {
      switch (Msg.message) {
      //
      case MM_WOM_OPEN:
         pWaveHdrOut1->lpData = (LPSTR)pBufferOut1;
         pWaveHdrOut1->dwBufferLength = BUFFER_SIZE;
         pWaveHdrOut1->dwBytesRecorded = 0;
         pWaveHdrOut1->dwUser = 0;
         pWaveHdrOut1->dwFlags = 0;
         pWaveHdrOut1->dwLoops = 0;
         pWaveHdrOut1->lpNext = NULL;
         pWaveHdrOut1->reserved = 0;
         //mmres = waveOutPrepareHeader(hWaveOut,pWaveHdrOut1,sizeof(WAVEHDR));

         pWaveHdrOut2->lpData = (LPSTR)pBufferOut2;
         pWaveHdrOut2->dwBufferLength = BUFFER_SIZE;
         pWaveHdrOut2->dwBytesRecorded = 0;
         pWaveHdrOut2->dwUser = 0;
         pWaveHdrOut2->dwFlags = 0;
         pWaveHdrOut2->dwLoops = 0;
         pWaveHdrOut2->lpNext = NULL;
         pWaveHdrOut2->reserved = 0;
         //waveOutPrepareHeader(hWaveOut,pWaveHdrOut2,sizeof(WAVEHDR));

         pWaveHdrOut3->lpData = (LPSTR)pBufferOut3;
         pWaveHdrOut3->dwBufferLength = BUFFER_SIZE;
         pWaveHdrOut3->dwBytesRecorded = 0;
         pWaveHdrOut3->dwUser = 0;
         pWaveHdrOut3->dwFlags = 0;
         pWaveHdrOut3->dwLoops = 0;
         pWaveHdrOut3->lpNext = NULL;
         pWaveHdrOut3->reserved = 0;
         //waveOutPrepareHeader(hWaveOut,pWaveHdrOut3,sizeof(WAVEHDR));

         //
         mmres= waveOutPrepareHeader(hWaveOut, pWaveHdrOut1, sizeof(WAVEHDR));
         //mmres= waveOutWrite(hWaveOut, pWaveHdrOut1, sizeof(WAVEHDR));
         mmres= waveOutPrepareHeader(hWaveOut, pWaveHdrOut2, sizeof(WAVEHDR));
         //mmres= waveOutWrite(hWaveOut, pWaveHdrOut2, sizeof(WAVEHDR));
         mmres= waveOutPrepareHeader(hWaveOut, pWaveHdrOut3, sizeof(WAVEHDR));
         //mmres= waveOutWrite(hWaveOut, pWaveHdrOut3, sizeof(WAVEHDR));
         break;


      case MM_WOM_DONE: {
         m_fs=g_frequency;

         /*
         if (OCTETS == 1){
            for(i=0; i<BUFFER_SIZE; i++){
               angle = (float)(2 * pi * j * frequence[g_frequency] / SAMPLERATE);
               if (angle > 2 * pi) angle = (float)(angle - 2 * pi);
               val = 127 + 127 * sin(angle);
               Buffer[i] = (val & 255);
               j = j + 1;
            }
         }
         */
         if (OCTETS == 2) {
            for(i=0; i<(BUFFER_SIZE>>1); i++) {
               float s;
               int sn;
               if( (++m_nSig%((long)(m_fSim/m_fs)) ) == 0)
                  m_nSig=0;

               s=(float)(32767*sin(2.0*pi*m_fs*m_nSig/m_fSim));
               sn=(int)s;

               Buffer[ 2*i] = (char)(sn & 255);
               Buffer[1+2*i] = (char)(sn >> 8);

            }
         }

         if((--g_duration_loop)) {
            waveOutPause(hWaveOut);
            waveOutReset(hWaveOut);
         }else{
            CopyMemory(((PWAVEHDR)Msg.lParam)->lpData, Buffer, BUFFER_SIZE);
            waveOutPrepareHeader(hWaveOut, ((PWAVEHDR)Msg.lParam), sizeof(WAVEHDR));
            waveOutWrite(hWaveOut, ((PWAVEHDR)Msg.lParam), sizeof(WAVEHDR));
         }
      }
      break;

      }
   }

   return 0;
}

/*--------------------------------------------
| Name:        soundcard_load
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int soundcard_load(void){
   MMRESULT mmres=0;

   unsigned long m_nSig;
   double m_fSim;
   double m_fs;
   int i=0;

   //
   m_nSig=0;
   m_fSim=SAMPLERATE;
   m_fs=2000;

   //
   g_duration_loop = (SAMPLERATE*g_duration)/(BUFFER_SIZE/OCTETS*1000);

   //
   h_soundcard_thread = CreateThread (NULL, 0, soundcard_thread, NULL, 0, &soundcard_thread_id );

   pWaveHdrOut1 = (PWAVEHDR) malloc(sizeof(WAVEHDR));
   pWaveHdrOut2 = (PWAVEHDR) malloc(sizeof(WAVEHDR));
   pWaveHdrOut3 = (PWAVEHDR) malloc(sizeof(WAVEHDR));

   Buffer = (PBYTE)malloc(BUFFER_SIZE);
   pBufferOut1 = (PBYTE)malloc(BUFFER_SIZE);
   pBufferOut2 = (PBYTE)malloc(BUFFER_SIZE);
   pBufferOut3 = (PBYTE)malloc(BUFFER_SIZE);

   if(!pBufferOut1 || !pBufferOut2 || !pBufferOut3) {
      if(pBufferOut1) free (pBufferOut1);
      if(pBufferOut2) free (pBufferOut2);
      if(pBufferOut3) free (pBufferOut3);

      return -1;
   }

   waveform.nChannels = CANAUX; // 1 pour mono 2 pour stereo
   waveform.wBitsPerSample = 8 * OCTETS; // 8 ou 16 bit
   waveform.nAvgBytesPerSec = SAMPLERATE * waveform.nChannels * waveform.wBitsPerSample/8; // nombre d'octets par seconde
   waveform.wFormatTag = 1; // 1 pour PCM
   waveform.nSamplesPerSec = SAMPLERATE; // frequence d'echantillonnage
   waveform.nBlockAlign = 1;
   waveform.cbSize = 0;

   //
   if (OCTETS == 2) {
      for(i=0; i<(BUFFER_SIZE>>1); i++) {
         float s;
         int sn;

         if( (++m_nSig%((long)(m_fSim/m_fs)) ) == 0)
            m_nSig=0;

         s=(float)(32767*sin(2.0*pi*m_fs*m_nSig/m_fSim));
         sn=(int)s;

         Buffer[ 2*i] = (char)(sn & 255);
         Buffer[1+2*i] = (char)(sn >> 8);
      }

      CopyMemory(pBufferOut1, Buffer, BUFFER_SIZE);
      CopyMemory(pBufferOut2, Buffer, BUFFER_SIZE);
      CopyMemory(pBufferOut3, Buffer, BUFFER_SIZE);
   }

   //
   if(waveOutOpen(&hWaveOut,WAVE_MAPPER,&waveform,(DWORD)soundcard_thread_id,0,CALLBACK_THREAD)) {
      free(pBufferOut1);
      free(pBufferOut2);
      free(pBufferOut3);
      return -1;
   }

   return 0;
}

/*--------------------------------------------
| Name:        soundcard_beep
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int soundcard_beep(int freq, int duration ){
   MMRESULT mmres=0;

   unsigned long m_nSig;
   double m_fSim;
   double m_fs;
   int i=0;

   waveOutReset(hWaveOut);
   //
   m_nSig=0;
   m_fSim=SAMPLERATE;
   m_fs=freq;

   //
   if(!(g_duration_loop = (SAMPLERATE*duration)/(BUFFER_SIZE/OCTETS*1000))) ;
   g_duration_loop =1;

   g_frequency = freq;

   if (OCTETS == 2) {
      for(i=0; i<(BUFFER_SIZE>>1); i++) {
         float s;
         int sn;

         if( (++m_nSig%((long)(m_fSim/m_fs)) ) == 0)
            m_nSig=0;

         s=(float)(32767*sin(2.0*pi*m_fs*m_nSig/m_fSim));
         sn=(int)s;

         Buffer[ 2*i] = (char)(sn & 255);
         Buffer[1+2*i] = (char)(sn >> 8);
      }

      CopyMemory(pBufferOut1, Buffer, BUFFER_SIZE);
      CopyMemory(pBufferOut2, Buffer, BUFFER_SIZE);
      CopyMemory(pBufferOut3, Buffer, BUFFER_SIZE);
   }

   mmres= waveOutPrepareHeader(hWaveOut, pWaveHdrOut1, sizeof(WAVEHDR));
   mmres= waveOutWrite(hWaveOut, pWaveHdrOut1, sizeof(WAVEHDR));

   mmres= waveOutPrepareHeader(hWaveOut, pWaveHdrOut2, sizeof(WAVEHDR));
   mmres= waveOutWrite(hWaveOut, pWaveHdrOut2, sizeof(WAVEHDR));

   mmres= waveOutPrepareHeader(hWaveOut, pWaveHdrOut3, sizeof(WAVEHDR));
   mmres= waveOutWrite(hWaveOut, pWaveHdrOut3, sizeof(WAVEHDR));

   //waveOutRestart(hWaveOut);

   return 0;
}

/*-------------------------------------------
| Name:dev_win32_board_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_board_load(void){
#ifdef USE_SOUNDCARD
   if(!soundcard_load())
      soundcard_loaded =1;
#endif

   if(soundcard_loaded)
      soundcard_beep(2000,500);

   Sleep(1000);
   return 0;
}

/*-------------------------------------------
| Name:dev_win32_board_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_board_open(desc_t desc, int o_flag){

   //
   if(o_flag & O_RDONLY) {
   }

   if(o_flag & O_WRONLY) {
   }

   ofile_lst[desc].offset=0;

   return 0;
}

/*-------------------------------------------
| Name:dev_win32_board_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_board_close(desc_t desc){
   return 0;
}

/*-------------------------------------------
| Name:dev_win32_board_isset_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_board_isset_read(desc_t desc){
   return -1;
}

/*-------------------------------------------
| Name:dev_win32_board_isset_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_board_isset_write(desc_t desc){
   return -1;
}
/*-------------------------------------------
| Name:dev_win32_board_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_board_read(desc_t desc, char* buf,int size){
   return -1;
}

/*-------------------------------------------
| Name:dev_win32_board_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_board_write(desc_t desc, const char* buf,int size){
   //read only mode
   return -1;
}

/*-------------------------------------------
| Name:dev_win32_board_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_board_seek(desc_t desc,int offset,int origin){

   switch(origin) {

   case SEEK_SET:
      ofile_lst[desc].offset=offset;
      break;

   case SEEK_CUR:
      ofile_lst[desc].offset+=offset;
      break;

   case SEEK_END:
      //to do: warning in SEEK_END (+ or -)????
      ofile_lst[desc].offset-=offset;
      break;
   }

   return ofile_lst[desc].offset;
}

/*-------------------------------------------
| Name:dev_win32_board_ioctl
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_board_ioctl(desc_t desc,int request,va_list ap){

   switch(request) {
   case BRDPWRDOWN:
   case BRDRESET:
      printf("board win32 power down\n");
      exit(0);
      break;
   //
   case BRDCFGPORT: {
      struct board_port_t* board_port_p = va_arg( ap, struct board_port_t*);
      if(!board_port_p)
         return -1;
   }
   break;

   case BRDBEEP: {
      struct board_beep_t* board_beep_p = va_arg( ap, struct board_beep_t*);
      if(!board_beep_p)
         return -1;
      if(soundcard_loaded)
         soundcard_beep(board_beep_p->frequency,board_beep_p->duration);
      else
         Beep(board_beep_p->frequency,board_beep_p->duration);
   }
   break;

   //
   default:
      return -1;

   }

   return 0;
}
/*===========================================
End of Source dev_win32_board.c
=============================================*/
