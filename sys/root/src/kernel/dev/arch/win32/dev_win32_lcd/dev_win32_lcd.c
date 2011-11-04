/*
The contents of this file are subject to the Mozilla Public License Version 1.1 
(the "License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS" basis, 
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License for the 
specific language governing rights and limitations under the License.

The Original Code is ______________________________________.

The Initial Developer of the Original Code is ________________________.
Portions created by ______________________ are Copyright (C) ______ _______________________.
All Rights Reserved.

Contributor(s): ______________________________________.

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

#include "kernel/fs/vfs/vfsdev.h"
#include "kernel/fs/vfs/vfstypes.h"

/*===========================================
Global Declaration
=============================================*/
const char dev_win32_lcd_name[]="lcd0";

int dev_win32_lcd_load(void);
int dev_win32_lcd_open(desc_t desc, int o_flag);
int dev_win32_lcd_close(desc_t desc);
int dev_win32_lcd_isset_read(desc_t desc);
int dev_win32_lcd_isset_write(desc_t desc);
int dev_win32_lcd_read(desc_t desc, char* buf,int size);
int dev_win32_lcd_write(desc_t desc, const char* buf,int size);
int dev_win32_lcd_seek(desc_t desc,int offset,int origin);


dev_map_t dev_win32_lcd_map={
   dev_win32_lcd_name,
   S_IFBLK,
   dev_win32_lcd_load,
   dev_win32_lcd_open,
   dev_win32_lcd_close,
   __fdev_not_implemented,
   __fdev_not_implemented,
   dev_win32_lcd_read,
   dev_win32_lcd_write,
   dev_win32_lcd_seek,
   __fdev_not_implemented //ioctl
};

static HANDLE hPipe; 
static LPTSTR lpszPipename = "\\\\.\\pipe\\lepton_lcdpipe"; 

//
typedef struct {
   int pos;
   int v;
}lcdframebuffer_t;


/*===========================================
Implementation
=============================================*/



/*-------------------------------------------
| Name:dev_win32_lcd_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_lcd_load(void){
   return 0;
}

/*-------------------------------------------
| Name:dev_win32_lcd_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_lcd_open(desc_t desc, int o_flag){

   //
   if(o_flag & O_RDONLY){
      
   }

   if(o_flag & O_WRONLY){
      DWORD dwMode; 
      BOOL fSuccess; 
     
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
 
         if (GetLastError() != ERROR_PIPE_BUSY){ 
            perror("Could not open pipe\n"); 
            return 0;
         }
 
         // All pipe instances are busy, so wait for 20 seconds. 
 
         if (! WaitNamedPipe(lpszPipename, 20000) ) {
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

      printf("win32 named pipe connected\n");
   
   }
   

   ofile_lst[desc].offset=0;

   return 0;
}

/*-------------------------------------------
| Name:dev_win32_lcd_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_lcd_close(desc_t desc){

   if(ofile_lst[desc].oflag & O_WRONLY){
      if(!ofile_lst[desc].nb_writer){
         CloseHandle(hPipe); 
      }
   }
   
   return 0;
}

/*-------------------------------------------
| Name:dev_win32_lcd_isset_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_lcd_isset_read(desc_t desc){
   return -1;
}

/*-------------------------------------------
| Name:dev_win32_lcd_isset_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_lcd_isset_write(desc_t desc){
   return -1;
}

/*-------------------------------------------
| Name:dev_win32_lcd_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_lcd_read(desc_t desc, char* buf,int size){
   return 0;
}

/*-------------------------------------------
| Name:dev_win32_lcd_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_lcd_write(desc_t desc, const char* buf,int size){
   int cb=0;
   BOOL fSuccess; 
   DWORD cbWritten; 

   lcdframebuffer_t lcdframebuffer;

   if (hPipe == INVALID_HANDLE_VALUE) 
      return -1;

   for(cb=0;cb<size;cb++){

      //used win32 named pipe
      lcdframebuffer.pos=ofile_lst[desc].offset+cb;
      lcdframebuffer.v=buf[cb];

      fSuccess = WriteFile( 
      hPipe,                  // pipe handle 
      &lcdframebuffer,             // message 
      sizeof(lcdframebuffer), // message length 
      &cbWritten,             // bytes written 
      NULL);                  // not overlapped 

      if (! fSuccess) 
         perror("WriteFile\n");

   }

   ofile_lst[desc].offset+=cb;

   return cb;
}

/*-------------------------------------------
| Name:dev_win32_lcd_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_lcd_seek(desc_t desc,int offset,int origin){
   switch(origin){

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



/*===========================================
End of Source dev_win32_lcd.c
=============================================*/
