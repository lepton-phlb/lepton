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
#include <stdlib.h>
#include <stdio.h>

#include "kernel/core/types.h"
#include "kernel/core/interrupt.h"
#include "kernel/core/system.h"
#include "kernel/core/stat.h"
#include "kernel/core/fcntl.h"
#include "kernel/core/ioctl_lcd.h"

#include "kernel/fs/vfs/vfsdev.h"
#include "kernel/fs/vfs/vfstypes.h"

/*===========================================
Global Declaration
=============================================*/
const char dev_win32_lcd_vga_name[]="lcd0";

int dev_win32_lcd_vga_load(void);
int dev_win32_lcd_vga_open(desc_t desc, int o_flag);
int dev_win32_lcd_vga_close(desc_t desc);
int dev_win32_lcd_vga_isset_read(desc_t desc);
int dev_win32_lcd_vga_isset_write(desc_t desc);
int dev_win32_lcd_vga_read(desc_t desc, char* buf,int size);
int dev_win32_lcd_vga_write(desc_t desc, const char* buf,int size);
int dev_win32_lcd_vga_seek(desc_t desc,int offset,int origin);
int dev_win32_lcd_vga_ioctl(desc_t desc,int request,va_list ap);


dev_map_t dev_win32_lcd_vga_map={
   dev_win32_lcd_vga_name,
   S_IFBLK,
   dev_win32_lcd_vga_load,
   dev_win32_lcd_vga_open,
   dev_win32_lcd_vga_close,
   __fdev_not_implemented,
   __fdev_not_implemented,
   dev_win32_lcd_vga_read,
   dev_win32_lcd_vga_write,
   dev_win32_lcd_vga_seek,
   dev_win32_lcd_vga_ioctl //ioctl
};

//
typedef struct {
   int pos;
   int v;
}lcdframebuffer_t;


#define GUI_XSIZE  320
#define GUI_YSIZE  320

static HANDLE hFile=NULL;
static HANDLE hFileMapping;
static char * mappedScreen = NULL;
static char * mappedData = NULL;
static int * counter = NULL;
#define WHOLE_SIZE ((GUI_XSIZE*GUI_YSIZE/8) + sizeof(int))


static HANDLE hPipe;
static LPTSTR lpszPipename = "\\\\.\\pipe\\lepton_lcdpipe";

static int refreshStatus=FALSE;



/*===========================================
Implementation
=============================================*/



/*-------------------------------------------
| Name:dev_win32_lcd_vga_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_lcd_vga_load(void){
   return 0;
}

/*-------------------------------------------
| Name:dev_win32_lcd_vga_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_lcd_vga_open(desc_t desc, int o_flag){

   //
   if(o_flag & O_RDONLY) {

   }

   if(o_flag & O_WRONLY) {
      DWORD dwMode;
      BOOL fSuccess;
      DWORD cbWritten;
      lcdframebuffer_t lcdframebuffer;

      int map_file_sz;

      if(hPipe!=NULL) //already open
         return 0;

      if(GUI_XSIZE>=GUI_YSIZE)
         map_file_sz = GUI_XSIZE*GUI_XSIZE/8 + sizeof(int);
      else
         map_file_sz = GUI_YSIZE*GUI_YSIZE/8 + sizeof(int);


      hFile = CreateFile(
         "c:\\map",          //  LPCTSTR lpFileName,
         GENERIC_WRITE | GENERIC_READ,           //DWORD dwDesiredAccess,
         FILE_SHARE_READ,           //DWORD dwShareMode,
         NULL,           //LPSECURITY_ATTRIBUTES lpSecurityAttributes,
         OPEN_ALWAYS,          //DWORD dwCreationDisposition,
         0,          //DWORD dwFlagsAndAttributes,
         NULL          //HANDLE hTemplateFile
         );

      if(hFile == INVALID_HANDLE_VALUE) return -1;

      hFileMapping = CreateFileMapping(
         hFile,          //HANDLE hFile,
         NULL,          //LPSECURITY_ATTRIBUTES lpAttributes,
         PAGE_READWRITE,                //DWORD flProtect,
         0,           //DWORD dwMaximumSizeHigh,
         map_file_sz,          //DWORD dwMaximumSizeLow,
         NULL          //LPCTSTR lpName
         );

      if(hFileMapping == NULL) {
         int err = GetLastError();
         CloseHandle(hFile);
         return -1;
      }

      mappedData = (char *)MapViewOfFile(
         hFileMapping,          //HANDLE hFileMappingObject,
         FILE_MAP_ALL_ACCESS,          //DWORD dwDesiredAccess,
         0,          //DWORD dwFileOffsetHigh,
         0,          //DWORD dwFileOffsetLow,
         map_file_sz          //SIZE_T dwNumberOfBytesToMap
         );
      if(mappedData == NULL) {
         int err = GetLastError();
         CloseHandle(hFileMapping);
         CloseHandle(hFile);
         return -1;
      }
      counter = (int*)mappedData;
      mappedScreen = mappedData+sizeof(int);
      *counter = 0;


      // Try to open a named pipe; wait for it, if necessary.
      while (1)
      {
         hPipe = CreateFile(
            lpszPipename,   // pipe name
            GENERIC_READ |  // read and write access
            GENERIC_WRITE,
            0,              // no sharing
            NULL,           // no security attributes
            OPEN_EXISTING,  // opens existing pipe
            0,              // default attributes
            NULL);          // no template file

         // Break if the pipe handle is valid.

         if (hPipe != INVALID_HANDLE_VALUE)
            break;

         // Exit if an error other than ERROR_PIPE_BUSY occurs.

         if (GetLastError() != ERROR_PIPE_BUSY) {
            perror("Could not open pipe\n");
            return -1;
         }

         // All pipe instances are busy, so wait for 20 seconds.

         if (!WaitNamedPipe(lpszPipename, 20000) ) {
            perror("Could not open pipe\n");
            return -1;
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
         return -1;
      }

      printf("win32 named pipe connected\n");

      //clr screen
      if(mappedScreen) {
         int i;
         for(i=0; i<WHOLE_SIZE; i++) {
            mappedScreen[ofile_lst[desc].offset+i] = 0x00;
            (*counter)++;
         }
      }

      //refresh
      lcdframebuffer.pos=512;
      lcdframebuffer.v=1;

      if (hPipe == INVALID_HANDLE_VALUE)
         return -1;

      fSuccess = WriteFile(
         hPipe,                  // pipe handle
         &lcdframebuffer,             // message
         sizeof(lcdframebuffer), // message length
         &cbWritten,             // bytes written
         NULL);                  // not overlapped

      if (!fSuccess)
         perror("WriteFile\n");

      Sleep(100);

   }


   ofile_lst[desc].offset=0;

   return 0;
}

/*-------------------------------------------
| Name:dev_win32_lcd_vga_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_lcd_vga_close(desc_t desc){

   if(ofile_lst[desc].oflag & O_WRONLY) {
      if(!ofile_lst[desc].nb_writer) {
         CloseHandle(hPipe);
      }
   }

   return 0;
}

/*-------------------------------------------
| Name:dev_win32_lcd_vga_isset_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_lcd_vga_isset_read(desc_t desc){
   return -1;
}

/*-------------------------------------------
| Name:dev_win32_lcd_vga_isset_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_lcd_vga_isset_write(desc_t desc){
   return -1;
}

/*-------------------------------------------
| Name:dev_win32_lcd_vga_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_lcd_vga_read(desc_t desc, char* buf,int size){
   return 0;
}

/*-------------------------------------------
| Name:dev_win32_lcd_vga_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_lcd_vga_write(desc_t desc, const char* buf,int size){
   int cb=size;

   BOOL fSuccess;
   DWORD cbWritten;
   lcdframebuffer_t lcdframebuffer;

   if(ofile_lst[desc].offset+size>=WHOLE_SIZE)
      cb=(WHOLE_SIZE-ofile_lst[desc].offset);

   if(mappedScreen) {
      int i;
      for(i=0; i<cb; i++) {
         mappedScreen[ofile_lst[desc].offset+i] = buf[i];
         (*counter)++;
      }
   }

   //refresh
   lcdframebuffer.pos=512;
   lcdframebuffer.v=1;

   if (hPipe == INVALID_HANDLE_VALUE)
      return -1;

   fSuccess = WriteFile(
      hPipe,                  // pipe handle
      &lcdframebuffer,             // message
      sizeof(lcdframebuffer), // message length
      &cbWritten,             // bytes written
      NULL);                  // not overlapped

   if (!fSuccess)
      perror("WriteFile\n");

   ofile_lst[desc].offset+=cb;

   return cb;
}

/*-------------------------------------------
| Name:dev_win32_lcd_vga_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_lcd_vga_seek(desc_t desc,int offset,int origin){
   switch(origin) {

   case SEEK_SET:
      ofile_lst[desc].offset=offset;
      break;

   case SEEK_CUR:
      ofile_lst[desc].offset+=offset;
      break;

   case SEEK_END:
      //to do: warning in SEEK_END (+ or -)????
      ofile_lst[desc].offset+=offset;
      break;
   }

   return ofile_lst[desc].offset;
}

/*--------------------------------------------
| Name:        dev_win32_lcd_vga_ioctl
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int dev_win32_lcd_vga_ioctl(desc_t desc,int request,va_list ap){

   switch(request) {
   //flush internal buffer of lcd device driver
   case LCDFLSBUF:
   {   /*
         int cb=WHOLE_SIZE;
         BOOL fSuccess;
         DWORD cbWritten;
         lcdframebuffer_t lcdframebuffer;


         //refresh
         lcdframebuffer.pos=512;
         lcdframebuffer.v=1;

         if (hPipe == INVALID_HANDLE_VALUE)
            return-1;

         fSuccess = WriteFile(
            hPipe,                  // pipe handle
            &lcdframebuffer,             // message
            sizeof(lcdframebuffer), // message length
            &cbWritten,             // bytes written
            NULL);                  // not overlapped

         if (! fSuccess)
            perror("WriteFile\n");
         */
   }
   break;

   case LCDGETVADDR: {
      unsigned long* vaddr= va_arg( ap, long*);
      *vaddr=(unsigned long)mappedScreen;
   }
   break;

   //
   default:
      return -1;

   }

   return 0;
}


/*============================================
| End of Source  : dev_win32_lcd_vga.c
==============================================*/
