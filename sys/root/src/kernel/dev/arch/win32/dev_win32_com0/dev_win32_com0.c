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
#include "kernel/core/types.h"
#include "kernel/core/interrupt.h"
#include "kernel/core/system.h"
#include "kernel/core/stat.h"
#include "kernel/core/fcntl.h"

#include "kernel/fs/vfs/vfsdev.h"
#include "kernel/fs/vfs/vfstypes.h"

#include "kernel/dev/arch/win32/dev_win32_com0/dev_socketsrv_0.h"

/*===========================================
Global Declaration
=============================================*/
int dev_win32_com0_load(void);
int dev_win32_com0_open(desc_t desc, int o_flag);
int dev_win32_com0_close(desc_t desc);
int dev_win32_com0_isset_read(desc_t desc);
int dev_win32_com0_isset_write(desc_t desc);
int dev_win32_com0_read(desc_t desc, char* buf,int size);
int dev_win32_com0_write(desc_t desc, const char* buf,int size);
int dev_win32_com0_seek(desc_t desc,int offset,int origin);

const char dev_win32_com0_name[]="ttyp0\0";

dev_map_t dev_win32_com0_map={
   dev_win32_com0_name,
   S_IFCHR,
   dev_win32_com0_load,
   dev_win32_com0_open,
   dev_win32_com0_close,
   dev_win32_com0_isset_read,
   dev_win32_com0_isset_write,
   dev_win32_com0_read,
   dev_win32_com0_write,
   dev_win32_com0_seek,
   __fdev_not_implemented //ioctl
};

#define TTYP0_INPUT_BUFFER_SIZE   32 //120
#define TTYP0_OUTPUT_BUFFER_SIZE  32 //120

//input buffer
char _ttyp0_input_buffer[TTYP0_INPUT_BUFFER_SIZE];
//output buffer
char _ttyp0_output_buffer[TTYP0_OUTPUT_BUFFER_SIZE];

volatile static char _input_r;
volatile static char _input_w;

volatile static char _output_r;
volatile static char _output_w;

//desc_t ttyp0_desc[3]={-1};//O_RDONLY,O_WRONLY,O_RDWR (O_RDONLY|O_WRONLY),
volatile static desc_t desc_rd = -1;   //O_RDONLY
volatile static desc_t desc_wr = -1;   //O_WRONLY


HANDLE dev_win32_com0_thr_h;
DWORD dev_win32_com0_thr_id;

/*===========================================
Implementation
=============================================*/

/*-------------------------------------------
| Name:dev_win32_com0_rcv_interrupt
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
__hw_interrupt(125,dev_win32_com0_rcv_interrupt){

   __hw_enter_interrupt();

   _ttyp0_input_buffer[_input_w]=read_socket_0();


   if((desc_rd>=0) && (_input_r==_input_w)) //empty to not empty
      __fire_io_int(ofile_lst[desc_rd].owner_pthread_ptr_read);

   if(++_input_w==TTYP0_INPUT_BUFFER_SIZE)
      _input_w=0;

   __hw_leave_interrupt();
}

/*-------------------------------------------
| Name:dev_win32_com0_snd_interrupt
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
__hw_interrupt(126,dev_win32_com0_snd_interrupt){

   __hw_enter_interrupt();

   if(_output_r<0)
      goto end;

   _output_r++;
   if(_output_r!=_output_w)
      write_socket_0(_ttyp0_output_buffer[_output_r]);
   else if(desc_wr>=0)
      __fire_io_int(ofile_lst[desc_wr].owner_pthread_ptr_write);
end:
   __hw_leave_interrupt();
}


/*-------------------------------------------
| Name:dev_win32_com0_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_com0_load(void){

   desc_rd = -1;
   desc_wr = -1;

   set_socket_0_rcv_intr(0);
   set_socket_0_snd_intr(0);

   start_socketsrv_0(0,0);

   return 0;
}

/*-------------------------------------------
| Name:dev_win32_com0_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_com0_open(desc_t desc, int o_flag){
   //
   if(o_flag & O_RDONLY) {
      if(desc_rd>=0) //already open: exclusive resource.
         return -1;

      _input_r = 0;
      _input_w = 0;
      desc_rd = desc;
      set_socket_0_rcv_intr(1);
   }

   if(o_flag & O_WRONLY) {
      if(desc_wr>=0) //already open: exclusive resource.
         return -1;

      _output_r = -1;
      _output_w = 0;
      desc_wr = desc;
      set_socket_0_snd_intr(1);
   }

   return 0;
}

/*-------------------------------------------
| Name:dev_win32_com0_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_com0_close(desc_t desc){

   if(ofile_lst[desc].oflag & O_RDONLY) {
      if(!ofile_lst[desc].nb_reader) {
         set_socket_0_rcv_intr(0);
         close_socketsrv_0();
         desc_rd = -1;
      }
   }

   if(ofile_lst[desc].oflag & O_WRONLY) {
      if(!ofile_lst[desc].nb_writer) {
         set_socket_0_snd_intr(0);
         close_socketsrv_0();
         desc_wr = -1;
      }
   }

   return 0;
}

/*-------------------------------------------
| Name:dev_win32_com0_isset_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_com0_isset_read(desc_t desc){
   if(_input_w!=_input_r)
      return 0;
   else
      return -1;
}

/*-------------------------------------------
| Name:dev_win32_com0_isset_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_com0_isset_write(desc_t desc){
   if(_output_w==_output_r) {
      _output_r=-1;
      return 0;
   }
   else
      return -1;
}

/*-------------------------------------------
| Name:dev_win32_com0_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_com0_read(desc_t desc, char* buf,int size){

   char w   = _input_w;
   char cb=0;

   for(cb=0; ((_input_r!=w)&&cb<size); cb++) {
      buf[cb]=_ttyp0_input_buffer[_input_r];

      if(++_input_r==TTYP0_INPUT_BUFFER_SIZE)
         _input_r=0;
   }

   return cb;
}

/*-------------------------------------------
| Name:dev_win32_com0_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_com0_write(desc_t desc, const char* buf,int size){

   if(size<TTYP0_OUTPUT_BUFFER_SIZE)
      _output_w = size;
   else
      _output_w = TTYP0_OUTPUT_BUFFER_SIZE;

   _output_r = 0;

   memcpy(_ttyp0_output_buffer,buf,_output_w);

   write_socket_0(_ttyp0_output_buffer[_output_r]);


   return _output_w;
}

/*-------------------------------------------
| Name:dev_win32_com0_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_com0_seek(desc_t desc,int offset,int origin){
   return 0;
}



/*===========================================
End of Sourcedev_win32_com0.c
=============================================*/
