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


/*============================================
| Includes    
==============================================*/
#include "kernel/core/types.h"
#include "kernel/core/interrupt.h"
#include "kernel/core/kernel.h"
#include "kernel/core/system.h"
#include "kernel/core/fcntl.h"
#include "kernel/core/ioctl_lcd.h"

#include "kernel/fs/vfs/vfsdev.h"

#include "dev_m16c_nju6433.h"

/*============================================
| Global Declaration 
==============================================*/
//
static const char dev_m16c_nju6433_name[]="lcd0\0";

int dev_m16c_nju6433_load(void);
int dev_m16c_nju6433_open(desc_t desc, int o_flag);
int dev_m16c_nju6433_close(desc_t desc);
int dev_m16c_nju6433_isset_read(desc_t desc);
int dev_m16c_nju6433_isset_write(desc_t desc);
int dev_m16c_nju6433_read(desc_t desc, char* buf,int size);
int dev_m16c_nju6433_write(desc_t desc, const char* buf,int size);
int dev_m16c_nju6433_seek(desc_t desc,int offset,int origin);
int dev_m16c_nju6433_ioctl(desc_t desc,int request,va_list ap);


dev_map_t dev_m16c_nju6433_map={
   dev_m16c_nju6433_name,
   S_IFBLK,
   dev_m16c_nju6433_load,
   dev_m16c_nju6433_open,
   dev_m16c_nju6433_close,
   __fdev_not_implemented,
   __fdev_not_implemented,
   dev_m16c_nju6433_read,
   dev_m16c_nju6433_write,
   dev_m16c_nju6433_seek,
   dev_m16c_nju6433_ioctl
};

//
typedef struct {
   int pos;
   int v;
}lcdframebuffer_t;

#define BLINK_OFFSET    256
#define LCD_SETCLR_ALL  512
#define LCD_BCKL_ONOFF  513
#define LCD_RESET_BLINK 514

#define ID_NJU6433_DIGITDISPLAY_BASE     245
#define ID_NJU6433_MAIN_DIGITDISPLAY1    ID_NJU6433_DIGITDISPLAY_BASE
#define ID_NJU6433_MAIN_DIGITDISPLAY2    ID_NJU6433_DIGITDISPLAY_BASE+1
#define ID_NJU6433_MAIN_DIGITDISPLAY3    ID_NJU6433_DIGITDISPLAY_BASE+2
#define ID_NJU6433_MAIN_DIGITDISPLAY4    ID_NJU6433_DIGITDISPLAY_BASE+3

#define ID_NJU6433_TINY_DIGITDISPLAY1    ID_NJU6433_DIGITDISPLAY_BASE+4
#define ID_NJU6433_TINY_DIGITDISPLAY2    ID_NJU6433_DIGITDISPLAY_BASE+5
#define ID_NJU6433_TINY_DIGITDISPLAY3    ID_NJU6433_DIGITDISPLAY_BASE+6
#define ID_NJU6433_TINY_DIGITDISPLAY4    ID_NJU6433_DIGITDISPLAY_BASE+7

#define ID_NJU6433_MINI_DIGITDISPLAY1    ID_NJU6433_DIGITDISPLAY_BASE+8
#define ID_NJU6433_MINI_DIGITDISPLAY2    ID_NJU6433_DIGITDISPLAY_BASE+9
#define ID_NJU6433_MINI_DIGITDISPLAY3    ID_NJU6433_DIGITDISPLAY_BASE+10

//
#define __NJU6433_SCL      P3.0
#define __NJU6433_DATA     P3.1
#define __NJU6433_CE       P3.2
#define __NJU6433_MODE     P3.3
#define __NJU6433_INH      P3.4

#define __SCL_1EDGE_DELAY\
   __NJU6433_SCL  = 1;\
   __NJU6433_SCL  = 0;


static char _digit_value[51]={
   0xD7,//0
   0x06,//1
   0xE3,//2
   0xA7,//3
   0x36,//4
   0xB5,//5
   0xF5,//6
   0x07,//7
   0xF7,//8
   0xB7,//9
   0xD7|0x08,//0.
   0x06|0x08,//1.
   0xE3|0x08,//2.
   0xA7|0x08,//3.
   0x36|0x08,//4.
   0xB5|0x08,//5.
   0xF5|0x08,//6.
   0x07|0x08,//7.
   0xF7|0x08,//8.
   0xB7|0x08, //9.
   0x77,// code for A  //20
   0xF4,// code for b  //21
   0xD1,// code for C     //22
   0xE6,// code for d     //23
   0xF1,// code for E     //24
   0x71,// code for F     //25
   0xB7,// code for g     //26
   0x76,// code for H     //27
   0x50,// code for I     //28
   0x86,// code for J     //29
   0x00,// code for k/K (not defined)     //30
   0xD0,// code for L     //31
   0x57,// code for m/M   //32
   0x64,// code for n     //33
   0xE4,// code for O     //34
   0x73,// code for P     //35
   0x37,// code for q     //36
   0x60,// code for r     //37
   0xB5,// code for S     //38
   0xF0,// code for t     //39
   0xD6,// code for U     //40
   0x00,// code for v/V (not defined)     //41
   0x00,// code for w/W (not defined)     //42
   0x00,// code for x/X (not defined)     //43
   0x36,// code for y     //44
   0x00,// code for z/Z (not defined)     //45
   0x01,// code for ^     //46
   0x20,// code for -     //47
   0x80,// code for _     //48
   0x33,// code for °     //49
   0x00 //OFF
};

#define DIGIT_OFF 50


//old lcd display

#define __MAIN_DIGITDISPLAY1 8
#define __MAIN_DIGITDISPLAY2 16
#define __MAIN_DIGITDISPLAY3 24
#define __MAIN_DIGITDISPLAY4 32

#define __TINY_DIGITDISPLAY1 84
#define __TINY_DIGITDISPLAY2 72
#define __TINY_DIGITDISPLAY3 56
#define __TINY_DIGITDISPLAY4 48




static volatile char _flush_buffer_requested;
static char _nju6433_bits_buffer[32]={0}; //32 Bytes -> 256 segment max

#define __nju6433_set_picto(__id__,__status__) {\
   unsigned char __byte__=__id__>>3;\
   unsigned char __offset__=(__id__-(__byte__<<3));\
   _nju6433_bits_buffer[__byte__]= (__status__ ? (_nju6433_bits_buffer[__byte__]|=((0x01)<<__offset__)) :\
   (_nju6433_bits_buffer[__byte__]&=~((0x01)<<__offset__)));\
}


//old lcd display
/*#define __nju6433_set_maindigit(__id__,__value__) {\
   unsigned char __byte__=__id__>>3;\
   _nju6433_bits_buffer[__byte__]=_digit_value[__value__];\
}*/


#define __nju6433_set_tinydigit12(__id__,__value__) {\
    unsigned char __byte__=__id__>>3;\
   _nju6433_bits_buffer[__byte__]=_digit_value[__value__];\
}


#define __nju6433_set_tinydigit34(__id__,__value__) {\
   unsigned char __byte__=__id__>>3;\
   unsigned char __msq__=((_digit_value[__value__])>>4);\
   unsigned char __lsq__=((_digit_value[__value__])<<4);\
   _nju6433_bits_buffer[__byte__+1]&=0xF0;\
   _nju6433_bits_buffer[__byte__+1]|=__msq__;\
   _nju6433_bits_buffer[__byte__]&=0x0F;\
   _nju6433_bits_buffer[__byte__]|=__lsq__;\
}


/*============================================
| Implementation 
==============================================*/

void _nju6433_set_maindigit(int id,int value) {
   unsigned char byte=id>>3;
   unsigned char msq;
   unsigned char lsq;

   if(value<19 && value>9){
      value-=10;
      _nju6433_bits_buffer[byte+1]|=0x80;
   }else{
      _nju6433_bits_buffer[byte+1]&=~(0x80);
   }

   msq=((_digit_value[value])>>4);
   lsq=((_digit_value[value])<<4);
   
   _nju6433_bits_buffer[byte]&=0x8F;
   _nju6433_bits_buffer[byte]|=lsq;
   _nju6433_bits_buffer[byte]&=0xF0;
   _nju6433_bits_buffer[byte]|=msq;
}
   
/*--------------------------------------------
| Name:        _nju6433_write_data
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int _nju6433_write_data(char* buf, int len){
   int i=0;
  
   //bank 4
   __NJU6433_SCL  = 0;
   //OS_Delay(10);
   //transfert mode data: 5 all register 1 to 4;
   __NJU6433_MODE = 1;
   //OS_Delay(10);
   __NJU6433_CE   = 1;
   //OS_Delay(10);
   // mode data 0101= 0x05
   __NJU6433_DATA = 1; //D0=1
   //clock upside front edge
   __SCL_1EDGE_DELAY;
   
   __NJU6433_DATA = 0; //D1=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   __NJU6433_DATA = 1; //D2=1
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   __NJU6433_DATA = 0; //D3=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   //switch from MODE to DATA tranfert
   __NJU6433_CE   = 0;
   //OS_Delay(10);
   //tranfert DATA 
   __NJU6433_MODE = 0;
   //OS_Delay(10);
   __NJU6433_CE   = 1;
   //OS_Delay(10);
   //
   __NJU6433_INH  = 0;
   //OS_Delay(10);
   
   for(i=0;i<25;i++){
      unsigned char b=0;
      for(b=0;b<8;b++){
         char mask=(0x01<<b);
         char v = ( (buf[i]&mask)?0x01:0x00 );
         __NJU6433_DATA = v;
         //clock upside front edge
         __SCL_1EDGE_DELAY;
      }
   }
   //
   //OS_Delay(1);
   __NJU6433_INH  = 1;
   //OS_Delay(1);
   __NJU6433_CE   = 0;
   //OS_Delay(1);

   _flush_buffer_requested = 0;
   return 0;
}

/*-------------------------------------------
| Name:_nju6433_set_digit
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static void _nju6433_set_digit(int id, int value) {
  
   if(value<0)
      value=DIGIT_OFF;

   switch(id){
      case ID_NJU6433_MAIN_DIGITDISPLAY4:
         _nju6433_set_maindigit(__MAIN_DIGITDISPLAY4,value);
      break;
      case ID_NJU6433_MAIN_DIGITDISPLAY3:
         _nju6433_set_maindigit(__MAIN_DIGITDISPLAY3,value);
      break;
      case ID_NJU6433_MAIN_DIGITDISPLAY2:
         _nju6433_set_maindigit(__MAIN_DIGITDISPLAY2,value);
      break;
      case ID_NJU6433_MAIN_DIGITDISPLAY1:
         _nju6433_set_maindigit(__MAIN_DIGITDISPLAY1,value);
      break;

      case ID_NJU6433_TINY_DIGITDISPLAY4:
         __nju6433_set_tinydigit34(__TINY_DIGITDISPLAY4,value);
      break;
      case ID_NJU6433_TINY_DIGITDISPLAY3:
         __nju6433_set_tinydigit34(__TINY_DIGITDISPLAY3,value);
      break;
      case ID_NJU6433_TINY_DIGITDISPLAY2:
         __nju6433_set_tinydigit12(__TINY_DIGITDISPLAY2,value);
      break;
      case ID_NJU6433_TINY_DIGITDISPLAY1:
         __nju6433_set_tinydigit12(__TINY_DIGITDISPLAY1,value);
      break;
   }

}

/*-------------------------------------------
| Name:_sed1540_setItem
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int _nju6433_set_item(int id,int status) {
   if(id<ID_NJU6433_DIGITDISPLAY_BASE){
      if(status<0)status=0;
      __nju6433_set_picto(id,status);
   }else{
      _nju6433_set_digit(id,status);
   }
   return 0;
}

/*-------------------------------------------
| Name:dev_m16c_nju6433_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_m16c_nju6433_load(void){
   PD3|=0x1F;//P3.0 to P3.4 set ouput mode.
   
   _nju6433_write_data(_nju6433_bits_buffer,sizeof(_nju6433_bits_buffer));
   _flush_buffer_requested =0;
   return 0;
}

/*-------------------------------------------
| Name:dev_m16c_nju6433_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_m16c_nju6433_open(desc_t desc, int o_flag){

   //
   if(o_flag & O_RDONLY){
      return -1;
   }

   if(o_flag & O_WRONLY){
      _flush_buffer_requested =0;
   }

   ofile_lst[desc].offset=0;

   return 0;
}

/*-------------------------------------------
| Name:dev_m16c_nju6433_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_m16c_nju6433_close(desc_t desc){

   if(ofile_lst[desc].oflag & O_WRONLY){
      if(!ofile_lst[desc].nb_writer){
      }
   }

   return 0;
}

/*-------------------------------------------
| Name:dev_m16c_nju6433_isset_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_m16c_nju6433_isset_read(desc_t desc){
   return -1;
}

/*-------------------------------------------
| Name:dev_m16c_nju6433_isset_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_m16c_nju6433_isset_write(desc_t desc){
   return -1;
}

/*-------------------------------------------
| Name:dev_m16c_nju6433_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_m16c_nju6433_read(desc_t desc, char* buf,int size){
   return 0;
}

/*-------------------------------------------
| Name:dev_m16c_nju6433_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_m16c_nju6433_write(desc_t desc, const char* buf,int size){
   int cb=0;

   lcdframebuffer_t lcdframebuffer;

   for(cb=0;cb<size;cb++){

      lcdframebuffer.pos=ofile_lst[desc].offset+cb;
      lcdframebuffer.v=((signed char)buf[cb]);

      if(lcdframebuffer.pos<BLINK_OFFSET){
         _nju6433_set_item(lcdframebuffer.pos,lcdframebuffer.v);
      }else if(lcdframebuffer.pos==LCD_SETCLR_ALL){
         if(lcdframebuffer.v){
            memset(_nju6433_bits_buffer,0xFF,sizeof(_nju6433_bits_buffer));
            _nju6433_write_data(_nju6433_bits_buffer,sizeof(_nju6433_bits_buffer));
         }else {
            memset(_nju6433_bits_buffer,0x00,sizeof(_nju6433_bits_buffer));
            _nju6433_write_data(_nju6433_bits_buffer,sizeof(_nju6433_bits_buffer));
         }
      }else if(lcdframebuffer.pos==LCD_BCKL_ONOFF){
         P5.6 = lcdframebuffer.v;
      }
       
   }

   _flush_buffer_requested = 1;
  
   ofile_lst[desc].offset+=cb;

   return cb;
}

/*-------------------------------------------
| Name:dev_m16c_nju6433_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_m16c_nju6433_seek(desc_t desc,int offset,int origin){
   switch(origin){

      case SEEK_SET:
         ofile_lst[desc].offset=offset;
      break;

      case SEEK_CUR:
         ofile_lst[desc].offset+=offset;
      break;

      case SEEK_END:
         ofile_lst[desc].offset+=offset;
      break;
   }

   return ofile_lst[desc].offset;
}

/*--------------------------------------------
| Name:        dev_m16c_nju6433_ioctl
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int dev_m16c_nju6433_ioctl(desc_t desc,int request,va_list ap){

   switch(request){
      //flush internal buffer of lcd device driver 
      case LCDFLSBUF:
         if(_flush_buffer_requested)
            _nju6433_write_data(_nju6433_bits_buffer,sizeof(_nju6433_bits_buffer));
      break;


      //
      default:
         return -1;

   }
  
   return 0;
}

/*============================================
| End of Source  : dev_m16c_nju6433.c
==============================================*/
