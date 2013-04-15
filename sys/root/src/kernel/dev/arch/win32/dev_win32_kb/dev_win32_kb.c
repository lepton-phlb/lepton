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
#include <stdlib.h>
#include <stdio.h>

#include "kernel/core/types.h"
#include "kernel/core/interrupt.h"
#include "kernel/core/system.h"
#include "kernel/core/stat.h"
#include "kernel/core/fcntl.h"
#include "kernel/core/ioctl_keyb.h"

#include "kernel/fs/vfs/vfsdev.h"
#include "kernel/fs/vfs/vfstypes.h"


/*===========================================
Global Declaration
=============================================*/
const char dev_win32_kb_name[]="kb0\0";

int dev_win32_kb_load(void);
int dev_win32_kb_open(desc_t desc, int o_flag);
int dev_win32_kb_close(desc_t desc);
int dev_win32_kb_isset_read(desc_t desc);
int dev_win32_kb_isset_write(desc_t desc);
int dev_win32_kb_read(desc_t desc, char* buf,int size);
int dev_win32_kb_write(desc_t desc, const char* buf,int size);
int dev_win32_kb_seek(desc_t desc,int offset,int origin);
int dev_win32_kb_ioctl(desc_t desc,int request,va_list ap);


dev_map_t dev_win32_kb_map={
   dev_win32_kb_name,
   S_IFCHR,
   dev_win32_kb_load,
   dev_win32_kb_open,
   dev_win32_kb_close,
   dev_win32_kb_isset_read,
   dev_win32_kb_isset_write,
   dev_win32_kb_read,
   dev_win32_kb_write,
   dev_win32_kb_seek,
   dev_win32_kb_ioctl
};


static HANDLE hPipe;
static LPTSTR lpszPipename = "\\\\.\\pipe\\lepton_keybpipe";
static LPTSTR lpszRotaryPipename = "\\\\.\\pipe\\lepton_rotarypipe";

static HANDLE hThread;

static int enbl_keyb_intr = 0;
static unsigned char keyb_win32_register = 0;
#define __KEYB_BUF_MAX  5
static unsigned char keyb_register[__KEYB_BUF_MAX] = {0};

static int keyb_r=0;
static int keyb_w=0;

desc_t desc_keyb_rd = -1;

/*===========================================
Implementation
=============================================*/

/*-------------------------------------------
| Name:_dev_read_rotary
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
char _dev_read_rotary(void){

   DWORD cbBytesRead;
   DWORD dwMode;
   BOOL fSuccess;
   HANDLE hPipe;
   int cb;
   unsigned char keybframebuffer[10]={0};
   char c=0;

   hPipe = CreateFile(
      lpszRotaryPipename,            // pipe name
      GENERIC_READ |           // read and write access
      GENERIC_WRITE,
      0,                       // no sharing
      NULL,                    // no security attributes
      OPEN_EXISTING,           // opens existing pipe
      0,                       // default attributes
      NULL);                   // no template file

   // Break if the pipe handle is valid.
   if (hPipe == INVALID_HANDLE_VALUE) {
      return 0;

      // Exit if an error other than ERROR_PIPE_BUSY occurs.
      if (GetLastError() != ERROR_PIPE_BUSY) {
         perror("Could not open pipe\n");
         return 0;
      }

      // All pipe instances are busy, so wait for 20 seconds.

      if (!WaitNamedPipe(lpszRotaryPipename, 20000) ) {
         perror("Could not open pipe\n");
         return 0;
      }
   }


   // The pipe connected; change to message-read mode.
   dwMode = PIPE_READMODE_MESSAGE;
   fSuccess = SetNamedPipeHandleState(
      hPipe,    // pipe handle
      &dwMode,  // new pipe mode
      NULL,     // don't set maximum bytes
      NULL);    // don't set maximum time
   if (!fSuccess) {
      perror("SetNamedPipeHandleState\n");
      return 0;
   }


   fSuccess = ReadFile(
      hPipe,        // handle to pipe
      &keybframebuffer,    // buffer to receive data
      sizeof(keybframebuffer),      // size of buffer
      &cbBytesRead, // number of bytes read
      NULL);        // not overlapped I/O

   if (!fSuccess)
      return 0;

   for(cb=0; cb<(int)cbBytesRead; cb++) {
      //printf("c=%d\n",keybframebuffer[cb]);
      //fire interrupt
      c = keybframebuffer[cb];
   }

   //
   CloseHandle(hPipe);

   return c;
}

/*-------------------------------------------
| Name:kbThread
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void kbThread(LPVOID lpvParam)
{
   DWORD cbBytesRead;
   BOOL fSuccess;
   HANDLE hPipe;
   int cb;

// The thread's parameter is a handle to a pipe instance.

   unsigned char keybframebuffer[10];

   hPipe = (HANDLE) lpvParam;


   while (1)
   {
      // Read client requests from the pipe.
      fSuccess = ReadFile(
         hPipe,        // handle to pipe
         keybframebuffer,    // buffer to receive data
         sizeof(keybframebuffer),      // size of buffer
         &cbBytesRead, // number of bytes read
         NULL);        // not overlapped I/O

      if (!fSuccess)
         break;

      for(cb=0; cb<(int)cbBytesRead; cb++) {
         //printf("c=%d\n",keybframebuffer[cb]);
         //fire interrupt
         keyb_win32_register = keybframebuffer[cb];

         if(enbl_keyb_intr)
            emuFireInterrupt(124);
      }

   }

// Flush the pipe to allow the client to read the pipe's contents
// before disconnecting. Then disconnect the pipe, and close the
// handle to this pipe instance.

   printf("\nkeyb disconnect pipe\n");

   if(!hPipe)
      return;

   FlushFileBuffers(hPipe);
   DisconnectNamedPipe(hPipe);
   CloseHandle(hPipe);
}

/*-------------------------------------------
| Name:dev_win32_kb_interrupt
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
__hw_interrupt(124,dev_win32_kb_interrupt){
   __hw_enter_interrupt();

   keyb_register[keyb_w] = keyb_win32_register;

   if(keyb_w == keyb_r)
      __fire_io_int(ofile_lst[desc_keyb_rd].owner_pthread_ptr_read);

   if(++keyb_w==__KEYB_BUF_MAX)
      keyb_w = 0;


   __hw_leave_interrupt();
}

/*-------------------------------------------
| Name:dev_win32_kb_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_kb_load(void){
   return 0;
}

/*-------------------------------------------
| Name:dev_win32_kb_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_kb_open(desc_t desc, int o_flag){

   //
   if(o_flag & O_RDONLY) {
      DWORD dwMode;
      BOOL fSuccess;
      DWORD dwThreadId;

      // Try to open a named pipe; wait for it, if necessary.
      while (1)
      {
         hPipe = CreateFile(
            lpszPipename,      // pipe name
            GENERIC_READ |     // read and write access
            GENERIC_WRITE,
            0,                 // no sharing
            NULL,              // no security attributes
            OPEN_EXISTING,     // opens existing pipe
            0,                 // default attributes
            NULL);             // no template file

         // Break if the pipe handle is valid.

         if (hPipe != INVALID_HANDLE_VALUE)
            break;

         // Exit if an error other than ERROR_PIPE_BUSY occurs.

         if (GetLastError() != ERROR_PIPE_BUSY) {
            perror("Could not open pipe\n");
            return 0;
         }

         // All pipe instances are busy, so wait for 20 seconds.

         if (!WaitNamedPipe(lpszPipename, 20000) ) {
            perror("Could not open pipe\n");
            return 0;
         }
      }

      // The pipe connected; change to message-read mode.
      dwMode = PIPE_READMODE_MESSAGE;
      fSuccess = SetNamedPipeHandleState(
         hPipe,    // pipe handle
         &dwMode,  // new pipe mode
         NULL,     // don't set maximum bytes
         NULL);    // don't set maximum time
      if (!fSuccess) {
         perror("SetNamedPipeHandleState\n");
         return 0;
      }

      printf("kbwin32 named pipe connected\n");

      hThread = CreateThread(
         NULL,                 // no security attribute
         0,                    // default stack size
         (LPTHREAD_START_ROUTINE) kbThread,
         (LPVOID) hPipe,       // thread parameter
         0,                    // not suspended
         &dwThreadId);         // returns thread ID

      if (hThread == NULL) {
         perror("CreateThread");
         return 0;
      }

      //
      desc_keyb_rd = desc;

      keyb_r = 0;
      keyb_w = 0;
      enbl_keyb_intr = 1;

   }

   if(o_flag & O_WRONLY) {
   }

   ofile_lst[desc].offset=0;

   return 0;
}

/*-------------------------------------------
| Name:dev_win32_kb_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_kb_close(desc_t desc){

   if(ofile_lst[desc].oflag & O_RDONLY) {
      if(!ofile_lst[desc].nb_reader) {
         enbl_keyb_intr = 0;
         //FlushFileBuffers(hPipe);
         //DisconnectNamedPipe(hPipe);
      }
   }

   if(ofile_lst[desc].oflag & O_WRONLY) {
      if(!ofile_lst[desc].nb_writer) {
      }
   }

   return 0;
}

/*-------------------------------------------
| Name:dev_win32_kb_isset_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_kb_isset_read(desc_t desc){
   if(keyb_w!=keyb_r)
      return 0;
   else
      return -1;
}

/*-------------------------------------------
| Name:dev_win32_kb_isset_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_kb_isset_write(desc_t desc){
   return -1;
}

/*-------------------------------------------
| Name:dev_win32_kb_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_kb_read(desc_t desc, char* buf,int size){
   int cb;

   buf[0]=0;

   for(cb=0; cb<size; cb++) {

      if(keyb_r == keyb_w)
         break;

      buf[cb] = keyb_register[keyb_r];

      if(++keyb_r==__KEYB_BUF_MAX)
         keyb_r = 0;
   }

   if(!cb && (ofile_lst[desc].oflag&O_NONBLOCK))
      buf[0]= _dev_read_rotary();

   return cb;
}

/*-------------------------------------------
| Name:dev_win32_kb_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_kb_write(desc_t desc, const char* buf,int size){
   return 0;
}

/*-------------------------------------------
| Name:dev_win32_kb_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_kb_seek(desc_t desc,int offset,int origin){
   return -1;
}

/*-------------------------------------------
| Name:dev_win32_kb_ioctl
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_kb_ioctl(desc_t desc,int request,va_list ap){
   switch(request) {
   case IOCTL_MULTIBOOT_GETVAL:
   case KBGETVAL: {  //get current keyboard value
      int* kbval_p = va_arg( ap, int*);
      if(!kbval_p)
         return -1;
      *kbval_p= _dev_read_rotary();
   }
   break;
   //
   default:
      return -1;

   }

   return 0;
}

/*===========================================
End of Sourcedev_win32_kb.c
=============================================*/
