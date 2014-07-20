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
#include "kernel/core/kernelconf.h"
#include "kernel/core/kernel.h"
#include "kernel/core/system.h"
#include "kernel/core/stat.h"
#include "kernel/core/fcntl.h"

#include "lib/libc/termios/termios.h"

#include "kernel/fs/vfs/vfsdev.h"
#include "kernel/dev/arch/win32/dev_win32_com1/dev_win32uart2.h"

/*===========================================
Global Declaration
=============================================*/
int dev_win32_com1_load(void);
int dev_win32_com1_open(desc_t desc, int o_flag);
int dev_win32_com1_close(desc_t desc);
int dev_win32_com1_isset_read(desc_t desc);
int dev_win32_com1_isset_write(desc_t desc);
int dev_win32_com1_read(desc_t desc, char* buf,int size);
int dev_win32_com1_write(desc_t desc, const char* buf,int size);
int dev_win32_com1_seek(desc_t desc,int offset,int origin);
int dev_win32_com1_ioctl(desc_t desc,int request,va_list ap);

const char dev_win32_com1_name[]="ttys1\0";

dev_map_t dev_win32_com1_map={
   dev_win32_com1_name,
   S_IFCHR,
   dev_win32_com1_load,
   dev_win32_com1_open,
   dev_win32_com1_close,
   dev_win32_com1_isset_read,
   dev_win32_com1_isset_write,
   dev_win32_com1_read,
   dev_win32_com1_write,
   dev_win32_com1_seek,
   dev_win32_com1_ioctl
};

#define TTYS1_INPUT_BUFFER_SIZE  (16*1024) //64//120
#define TTYS1_OUTPUT_BUFFER_SIZE (2*1024) //64//120

//input buffer
unsigned char _ttys1_input_buffer[TTYS1_INPUT_BUFFER_SIZE];
//output buffer
unsigned char _ttys1_output_buffer[TTYS1_OUTPUT_BUFFER_SIZE];

volatile static int _input_r;
volatile static int _input_w;

volatile static int _output_r;
volatile static int _output_w;

//desc_t ttys1_desc[3]={-1};//O_RDONLY,O_WRONLY,O_RDWR (O_RDONLY|O_WRONLY),
volatile static desc_t desc_rd = -1;   //O_RDONLY
volatile static desc_t desc_wr = -1;   //O_WRONLY


HANDLE dev_win32_com1_thr_h;
DWORD dev_win32_com1_thr_id;

//for SLIP mode.
#define  N_SLIP  0x0001

//to remove : must be set to 0;
#ifdef __KERNEL_NET_IPSTACK
   #define SLIP_ACTIVE 1
#else
   #define SLIP_ACTIVE 0
#endif

volatile unsigned int _ttys1_slip_option = SLIP_ACTIVE; //0

//xon/xoff flow control
#define FLOWCTRL_XON    0x11 //ctrl-Q
#define FLOWCTRL_XOFF   0x13 //ctrl-S

#define XONOFF_DSBL     0
#define XONOFF_ENBL     1
static volatile unsigned char _xonoff_option = XONOFF_DSBL;

#define STATUS_IDLE      0
#define STATUS_RCV_XOFF  1
#define STATUS_SND_XOFF  1

static volatile unsigned char _snd_xonoff_status = STATUS_IDLE;
static volatile unsigned char _rcv_xonoff_status = STATUS_IDLE;


#define __pause_snd() \
   /*disable tx interrupt*/ \
   setSndInterrupt2(0);

#define __resume_snd() \
   /*enable tx interrupt*/ \
   setSndInterrupt2(1); \
   /*send buffer byte*/ \
   if(_output_r>0 && _output_r<_output_w) { \
      _output_r++; \
      if(_output_r!=_output_w) \
         writeWin32UartTransmitRegister2(_ttys1_output_buffer[_output_r]); \
      else if(desc_wr>=0 && _output_r==_output_w) \
         __fire_io_int(ofile_lst[desc_wr].owner_pthread_ptr_write); \
   }

//termios
static struct termios ttys_termios;

typedef struct s2s {
   speed_t ts;
   long ns;
}s2s_t;

static s2s_t const s2s[] = {
   { B0,                0 },
   { B50,              50 },
   { B75,              75 },
   { B110,            110 },
   { B134,            134 },
   { B150,            150 },
   { B200,            200 },
   { B300,            300 },
   { B600,            600 },
   { B1200,          1200 },
   { B1800,          1800 },
   { B2400,          2400 },
   { B4800,          4800 },
   { B9600,          9600 },
   { B19200,        19200 },
   { B38400,        38400 }
};

//inter character timer
#if defined (__KERNEL_UCORE_EMBOS)
//VTIME timer in units of 0.1 seconds (posix specification).
OS_TIMER dev_win32_com1_timer;
static volatile char inter_char_timer=0;
#endif
/*===========================================
Implementation
=============================================*/

/*-------------------------------------------
| Name:dev_win32_com1_rcv_interrupt
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
__hw_interrupt(80,dev_win32_com1_rcv_interrupt){
   unsigned char c;
   int sz;
   __hw_enter_interrupt();

   c = readWin32UartReceiveRegister2();

   //printf("%02x ",(unsigned char)c);

   if(!_xonoff_option)
      goto rcv;

   //xonxoff
   if(_rcv_xonoff_status == STATUS_IDLE
      && c==FLOWCTRL_XOFF) { //rcv XOFF
      _rcv_xonoff_status = STATUS_RCV_XOFF;
      __pause_snd();
      goto end;
   }else if(_rcv_xonoff_status == STATUS_RCV_XOFF
            && c==FLOWCTRL_XON) { //rcv XON
      _rcv_xonoff_status = STATUS_IDLE;
      __resume_snd();
      goto end;
   }

   //check buffer overflow
   if((sz=_input_w-_input_r)<0)
      sz=TTYS1_INPUT_BUFFER_SIZE-_input_r+_input_w;

   if( (_snd_xonoff_status == STATUS_IDLE)
       &&(sz>=(TTYS1_INPUT_BUFFER_SIZE/4))) { //send XOFF
      __pause_snd();
      _snd_xonoff_status = STATUS_SND_XOFF;
      writeWin32UartTransmitRegister2(FLOWCTRL_XOFF);
      __resume_snd();
   }

   //
rcv:
   //
   _ttys1_input_buffer[_input_w]= c;

   if((desc_rd>=0) && (_input_r==_input_w)) { //empty to not empty
      __fire_io_int(ofile_lst[desc_rd].owner_pthread_ptr_read);
   }

   if(++_input_w==TTYS1_INPUT_BUFFER_SIZE)
      _input_w=0;

end:
   inter_char_timer=ttys_termios.c_cc[VTIME];
   __hw_leave_interrupt();
}

/*-------------------------------------------
| Name:dev_win32_com1_snd_interrupt
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
__hw_interrupt(76,dev_win32_com1_snd_interrupt){

   __hw_enter_interrupt();

   if(_output_r<0)
      goto end;

   if(_xonoff_option && _rcv_xonoff_status == STATUS_RCV_XOFF)
      goto end;

   //printf("%02x ",_ttys1_output_buffer[_output_r]);
   _output_r++;
   if(_output_r<_output_w)
      writeWin32UartTransmitRegister2(_ttys1_output_buffer[_output_r]);
   else if(desc_wr>=0 && _output_r==_output_w) {
      __fire_io_int(ofile_lst[desc_wr].owner_pthread_ptr_write);
   }

end:
   __hw_leave_interrupt();
}

/*-------------------------------------------
| Name:dev_win32_com1_timer_callback
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static void  dev_win32_com1_timer_callback(void){
   if( (desc_rd>=0)
       && (ttys_termios.c_cc[VTIME])
       && inter_char_timer
       && !(--inter_char_timer)) {

      if(_input_w==_input_r) {
         __fire_io_int(ofile_lst[desc_rd].owner_pthread_ptr_read);
      }

   }

   OS_RetriggerTimer(&dev_win32_com1_timer);

}

/*-------------------------------------------
| Name:dev_win32_com1_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_com1_load(void){

   setRcvInterrupt2(0);
   setSndInterrupt2(0);

   startAsyncRs2322();

   desc_rd = -1;
   desc_wr = -1;

   _xonoff_option = XONOFF_DSBL;
   _rcv_xonoff_status = STATUS_IDLE;
   _snd_xonoff_status = STATUS_IDLE;

   //
   cfmakeraw(&ttys_termios);
   cfsetispeed (&ttys_termios,B9600);
   cfsetospeed (&ttys_termios,B9600);
   ttys_termios.c_iflag &= ~(IXOFF|IXON); //xon/xoff disable

   ttys_termios.c_cc[VTIME]=0; // no timeout, blocking call
   inter_char_timer = 0;

#if defined (__KERNEL_UCORE_EMBOS)
   //VTIME timer in units of 0.1 seconds (posix specification).
   OS_CreateTimer(&dev_win32_com1_timer,dev_win32_com1_timer_callback,100);  // 100ms
#endif

   return 0;
}

/*-------------------------------------------
| Name:dev_win32_com1_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_com1_open(desc_t desc, int o_flag){
   //
   if(o_flag & O_RDONLY) {
      if(desc_rd>=0) //already open: exclusive resource.
         return -1;
      if(!ofile_lst[desc].nb_reader) {
         _input_r = 0;
         _input_w = 0;
      }
      desc_rd = desc;
      setRcvInterrupt2(1);
   }

   if(o_flag & O_WRONLY) {
      if(desc_wr>=0) //already open: exclusive resource.
         return -1;
      _output_r = -1;
      _output_w = 0;
      desc_wr = desc;
      setSndInterrupt2(1);
   }

   return 0;
}

/*-------------------------------------------
| Name:dev_win32_com1_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_com1_close(desc_t desc){

   if(ofile_lst[desc].oflag & O_RDONLY) {
      if(!ofile_lst[desc].nb_reader) {
         setRcvInterrupt2(0);
         desc_rd = -1;

         if(_xonoff_option
            && _snd_xonoff_status == STATUS_SND_XOFF) {
            __pause_snd();
            _snd_xonoff_status = STATUS_IDLE;
            writeWin32UartTransmitRegister2(FLOWCTRL_XON);
         }
      }
   }

   if(ofile_lst[desc].oflag & O_WRONLY) {
      if(!ofile_lst[desc].nb_writer) {
         setSndInterrupt2(0);
         desc_wr = -1;
      }
   }

   return 0;
}

/*-------------------------------------------
| Name:dev_win32_com1_isset_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_com1_isset_read(desc_t desc){
   if((_input_w!=_input_r)
      || (ttys_termios.c_cc[VTIME] && !inter_char_timer) ) {
      inter_char_timer=ttys_termios.c_cc[VTIME];
      return 0;
   }else{
      return -1;
   }
}

/*-------------------------------------------
| Name:dev_win32_com1_isset_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_com1_isset_write(desc_t desc){
   if(_output_w==0){
      //ready to sent data
      return 0;
   } 
   if(_output_w==_output_r) {
      _output_r=-1;
      return 0;
   }
   else
      return -1;
}

/*-------------------------------------------
| Name:dev_win32_com1_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_com1_read(desc_t desc, char* buf,int size){

   int w   = _input_w;
   int r   = _input_r;
   int cb=0;

   for(cb=0; ((r!=w)&&cb<size); cb++) {
      buf[cb]=_ttys1_input_buffer[r];

      if(++r==TTYS1_INPUT_BUFFER_SIZE)
         r=0;
   }

   _input_r = r;

   if(_xonoff_option
      && _snd_xonoff_status == STATUS_SND_XOFF) {

      if(r!=w && cb!=size)
         return cb;

      __pause_snd();
      _snd_xonoff_status = STATUS_IDLE;
      writeWin32UartTransmitRegister2(FLOWCTRL_XON);
      __resume_snd();
   }

   return cb;
}

/*-------------------------------------------
| Name:dev_win32_com1_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_com1_write(desc_t desc, const char* buf,int size){

   if(size<TTYS1_OUTPUT_BUFFER_SIZE)
      _output_w = size;
   else
      _output_w = TTYS1_OUTPUT_BUFFER_SIZE;

   _output_r = 0;

   memcpy(_ttys1_output_buffer,buf,_output_w);

   writeWin32UartTransmitRegister2(_ttys1_output_buffer[_output_r]);


   return _output_w;
}

/*-------------------------------------------
| Name:dev_win32_com1_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_com1_seek(desc_t desc,int offset,int origin){
   return 0;
}

/*-------------------------------------------
| Name:termios2ttys
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int termios2ttys(struct termios* termios_p){
   const struct s2s *sp;
   long n_speed;
   speed_t speed;
   uart2_config config;

   //xon/xoff
   if((termios_p->c_iflag&IXON) != (ttys_termios.c_iflag&IXON)) {
      if(termios_p->c_iflag&IXON) {
         termios_p->c_iflag |= (IXOFF|IXON);
         _xonoff_option = 1;
      }else{
         termios_p->c_iflag &= ~(IXOFF|IXON);
         _xonoff_option = 0;

      }
   }else if((termios_p->c_iflag&IXOFF) != (ttys_termios.c_iflag&IXOFF)) {
      if(termios_p->c_iflag&IXOFF) {
         termios_p->c_iflag |= (IXOFF|IXON);
         _xonoff_option = 1;
      }else{
         termios_p->c_iflag &= ~(IXOFF|IXON);
         _xonoff_option = 0;
      }
   }


   //speed
   speed = cfgetospeed(termios_p);
   for (sp = s2s; sp < s2s + (sizeof(s2s) / sizeof(s2s[0])); sp++) {
      if (sp->ts == speed) {
         n_speed = sp->ns;
         break;
      }
   }
   getRs2322(&config);
   //to do: set speed
   config.speed = n_speed;
   setRs2322(&config);

   //timeout read in blocking call
   if(termios_p->c_cc[VTIME]) {
      OS_StopTimer(&dev_win32_com1_timer);
      inter_char_timer = termios_p->c_cc[VTIME];
      OS_RetriggerTimer(&dev_win32_com1_timer);
   }else{
      OS_StopTimer(&dev_win32_com1_timer);
   }

   //

   memcpy(&ttys_termios,termios_p,sizeof(struct termios));
   return 0;
}

/*-------------------------------------------
| Name:dev_win32_com1_ioctl
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_win32_com1_ioctl(desc_t desc,int request,va_list ap){

   struct termios* termios_p = (struct termios*)0;

   switch(request) {

   /* If optional_actions is TCSANOW, the change will occur immediately.*/
   /* If optional_actions is TCSADRAIN, the change will occur after all output written to fildes is transmitted.
      This function should be used when changing parameters that affect output.*/
   /* If optional_actions is TCSAFLUSH, the change will occur after all output written to fildes is transmitted,
      and all input so far received but not read will be discarded before the change is made.
      return 0;*/
   case TCSETS:
   case TCSAFLUSH:
   case TCSADRAIN:
   case TCSANOW:
      termios_p = va_arg( ap, struct termios*);
      if(!termios_p)
         return -1;
      termios2ttys(termios_p);

      break;

   //
   case TCGETS:
      termios_p = va_arg( ap, struct termios*);
      if(!termios_p)
         return -1;

      memcpy(termios_p,&ttys_termios,sizeof(struct termios));
      break;

   //
   default:
      return -1;

   }

   return 0;
}


/*===========================================
End of Sourcedev_win32_com1.c
=============================================*/
