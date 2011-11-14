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

#include "kernel/fs/vfs/vfsdev.h"


/*============================================
| Global Declaration 
==============================================*/

//
static const char dev_lcd_sanyo_lc75813_name[]="lcd0\0";

int dev_lcd_sanyo_lc75813_load(void);
int dev_lcd_sanyo_lc75813_open(desc_t desc, int o_flag);
int dev_lcd_sanyo_lc75813_close(desc_t desc);
int dev_lcd_sanyo_lc75813_isset_read(desc_t desc);
int dev_lcd_sanyo_lc75813_isset_write(desc_t desc);
int dev_lcd_sanyo_lc75813_read(desc_t desc, char* buf,int size);
int dev_lcd_sanyo_lc75813_write(desc_t desc, const char* buf,int size);
int dev_lcd_sanyo_lc75813_seek(desc_t desc,int offset,int origin);
int dev_lcd_sanyo_lc75813_ioctl(desc_t desc,int request,va_list ap);


dev_map_t dev_lcd_sanyo_lc75813_map={
   dev_lcd_sanyo_lc75813_name,
   S_IFBLK,
   dev_lcd_sanyo_lc75813_load,
   dev_lcd_sanyo_lc75813_open,
   dev_lcd_sanyo_lc75813_close,
   __fdev_not_implemented,
   __fdev_not_implemented,
   dev_lcd_sanyo_lc75813_read,
   dev_lcd_sanyo_lc75813_write,
   dev_lcd_sanyo_lc75813_seek,
   dev_lcd_sanyo_lc75813_ioctl
};

//
#define __LCD_SCL      P4.1
#define __LCD_DATA     P4.0
#define __LCD_CE       P4.2
#define __LCD_INH      P4.3

#define __SCL_1EDGE_DELAY\
   __LCD_SCL  = 1;\
   {char cpt=0;\
    for(cpt=0;cpt<50;cpt++);\
   }\
   __LCD_SCL  = 0;\
   {char cpt=0;\
    for(cpt=0;cpt<50;cpt++);\
   }


//
typedef struct {
   int pos;
   int v;
}lcdframebuffer_t;

//
#define __set_picto(__id__,__status__) {\
   unsigned char __byte__=__id__>>3;\
   unsigned char __offset__=(__id__-(__byte__<<3));\
   _sanyo_lc75813_bits_buffer[__byte__]= (__status__ ? (_sanyo_lc75813_bits_buffer[__byte__]|=((0x01)<<__offset__)) :\
   (_sanyo_lc75813_bits_buffer[__byte__]&=~((0x01)<<__offset__)));\
}

static char _sanyo_lc75813_bits_buffer[52]={0};// 4 x 104 bits

/*============================================
| Implementation 
==============================================*/
/*--------------------------------------------
| Name:        _lcd_sanyo_lc75813_write_data_block1
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int _lcd_sanyo_lc75813_write_data_block1(char* buf, int len){
   int i=0;
  
   //OS_Delay(10);
   __LCD_CE   = 0;
      //bank 4
   __LCD_SCL  = 0;
   //OS_Delay(10);


   //address
   __LCD_DATA = 0; //B0=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;
   
   __LCD_DATA = 0; //B1=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   __LCD_DATA = 0; //B2=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   __LCD_DATA = 0; //B3=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   __LCD_DATA = 0; //A0=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;
   
   __LCD_DATA = 0; //A1=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   __LCD_DATA = 1; //A2=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   __LCD_DATA = 0; //A3=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;




   //switch from MODE to DATA tranfert
   __LCD_CE   = 0;
   //OS_Delay(10);
   __LCD_CE   = 1;
   //OS_Delay(10);
   //
   __LCD_INH  = 1;
   //OS_Delay(10);
   
   //first 88 bits
   for(i=0;i<11;i++){
      unsigned char b=0;
      for(b=0;b<8;b++){
         char mask=(0x01<<b);
         char v = ( (buf[i]&mask)?0x01:0x00 );
         __LCD_DATA = v;//v;
         //clock upside front edge
         __SCL_1EDGE_DELAY;
      }
   }

   //control data
   __LCD_DATA = 0; //b0=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;
   
   __LCD_DATA = 0; //b1=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   __LCD_DATA = 0; //b2=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   __LCD_DATA = 0; //b3=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   __LCD_DATA = 0; //b4=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;


    //control data
   __LCD_DATA = 0; //P0=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;
   
   __LCD_DATA = 0; //P1=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   __LCD_DATA = 0; //P2=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   __LCD_DATA = 0; //P3=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;


   //control data
   __LCD_DATA = 0; //DR=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;
   
   __LCD_DATA = 0; //DT=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   __LCD_DATA = 0; //FC=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   __LCD_DATA = 0; //SC=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   __LCD_DATA = 0; //BU=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   //DD
   __LCD_DATA = 0; //DD0=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;
   
   __LCD_DATA = 0; //DD1=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;
   //
   //OS_Delay(1);
   __LCD_CE   = 0;

   return 0;
}

/*--------------------------------------------
| Name:        _lcd_sanyo_lc75813_write_data_block2
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int _lcd_sanyo_lc75813_write_data_block2(char* buf, int len){
   int i=0;
  
   //OS_Delay(10);
   __LCD_CE   = 0;
      //bank 4
   __LCD_SCL  = 0;
   //OS_Delay(10);


   //address
   __LCD_DATA = 0; //B0=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;
   
   __LCD_DATA = 0; //B1=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   __LCD_DATA = 0; //B2=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   __LCD_DATA = 0; //B3=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   __LCD_DATA = 0; //A0=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;
   
   __LCD_DATA = 0; //A1=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   __LCD_DATA = 1; //A2=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   __LCD_DATA = 0; //A3=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;




   //switch from MODE to DATA tranfert
   __LCD_CE   = 0;
   //OS_Delay(10);
   __LCD_CE   = 1;
   //OS_Delay(10);
   //
   __LCD_INH  = 1;
   //OS_Delay(10);
   
   //first 88 bits
   for(i=0;i<11;i++){
      unsigned char b=0;
      for(b=0;b<8;b++){
         char mask=(0x01<<b);
         char v = ( (buf[i]&mask)?0x01:0x00 );
         __LCD_DATA = v;//v;
         //clock upside front edge
         __SCL_1EDGE_DELAY;
      }
   }

   //control data
   __LCD_DATA = 0; //b0=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;
   
   __LCD_DATA = 0; //b1=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   __LCD_DATA = 0; //b2=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   __LCD_DATA = 0; //b3=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   __LCD_DATA = 0; //b4=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   __LCD_DATA = 0; //b5=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;
   
   __LCD_DATA = 0; //b6=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   __LCD_DATA = 0; //b7=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   __LCD_DATA = 0; //b8=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   __LCD_DATA = 0; //b9=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   //control data
   __LCD_DATA = 0; //b10=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;
   
   __LCD_DATA = 0; //b11=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   __LCD_DATA = 0; //b12=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   __LCD_DATA = 0; //b13=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   
   //DD
   __LCD_DATA = 0; //DD1=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;
   
   __LCD_DATA = 1; //DD0=1
   //clock upside front edge
   __SCL_1EDGE_DELAY;
      
   //OS_Delay(1);
   __LCD_CE   = 0;
   //OS_Delay(1);

   return 0;
}


/*--------------------------------------------
| Name:        _lcd_sanyo_lc75813_write_data_block3
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int _lcd_sanyo_lc75813_write_data_block3(char* buf, int len){
   int i=0;
  
   //OS_Delay(10);
   __LCD_CE   = 0;
      //bank 4
   __LCD_SCL  = 0;
   //OS_Delay(10);


   //address
   __LCD_DATA = 0; //B0=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;
   
   __LCD_DATA = 0; //B1=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   __LCD_DATA = 0; //B2=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   __LCD_DATA = 0; //B3=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   __LCD_DATA = 0; //A0=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;
   
   __LCD_DATA = 0; //A1=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   __LCD_DATA = 1; //A2=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   __LCD_DATA = 0; //A3=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;




   //switch from MODE to DATA tranfert
   __LCD_CE   = 0;
   //OS_Delay(10);
   __LCD_CE   = 1;
   //OS_Delay(10);
   //
   __LCD_INH  = 1;
   //OS_Delay(10);
   
   //first 88 bits
   for(i=0;i<11;i++){
      unsigned char b=0;
      for(b=0;b<8;b++){
         char mask=(0x01<<b);
         char v = ( (buf[i]&mask)?0x01:0x00 );
         if(i==10 && b>3)
            __LCD_DATA = 0;//v;
         else
         __LCD_DATA = v;//v;
         //clock upside front edge
         __SCL_1EDGE_DELAY;
      }
   }

   //control data
   __LCD_DATA = 0; //b0=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;
   
   __LCD_DATA = 0; //b1=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   __LCD_DATA = 0; //b2=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   __LCD_DATA = 0; //b3=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   __LCD_DATA = 0; //b4=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   __LCD_DATA = 0; //b5=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;
   
   __LCD_DATA = 0; //b6=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   __LCD_DATA = 0; //b7=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   __LCD_DATA = 0; //b8=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   __LCD_DATA = 0; //b9=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   //control data
   __LCD_DATA = 0; //b10=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;
   
   __LCD_DATA = 0; //b11=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   __LCD_DATA = 0; //b12=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   __LCD_DATA = 0; //b13=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   
   //DD
   __LCD_DATA = 1; //DD1=1
   //clock upside front edge
   __SCL_1EDGE_DELAY;
   
   __LCD_DATA = 0; //DD0=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;
      
   //OS_Delay(1);
   __LCD_CE   = 0;
   //OS_Delay(1);

   return 0;
}

/*--------------------------------------------
| Name:        _lcd_sanyo_lc75813_write_data_block4
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int _lcd_sanyo_lc75813_write_data_block4(char* buf, int len){
   int i=0;
  
   //OS_Delay(10);
   __LCD_CE   = 0;
      //bank 4
   __LCD_SCL  = 0;
   //OS_Delay(10);


   //address
   __LCD_DATA = 0; //B0=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;
   
   __LCD_DATA = 0; //B1=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   __LCD_DATA = 0; //B2=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   __LCD_DATA = 0; //B3=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   __LCD_DATA = 0; //A0=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;
   
   __LCD_DATA = 0; //A1=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   __LCD_DATA = 1; //A2=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   __LCD_DATA = 0; //A3=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;




   //switch from MODE to DATA tranfert
   __LCD_CE   = 0;
   //OS_Delay(10);
   __LCD_CE   = 1;
   //OS_Delay(10);
   //
   __LCD_INH  = 1;
   //OS_Delay(10);
   
   //first 88 bits
   for(i=0;i<11;i++){
      unsigned char b=0;
      for(b=0;b<8;b++){
         char mask=(0x01<<b);
         char v = ( (buf[i]&mask)?0x01:0x00 );
         if(i==10 && b>3)
            __LCD_DATA = 0;//v;
         else
         __LCD_DATA = v;//v;
         //clock upside front edge
         __SCL_1EDGE_DELAY;
      }
   }

   //control data
   __LCD_DATA = 0; //b0=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;
   
   __LCD_DATA = 0; //b1=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   __LCD_DATA = 0; //b2=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   __LCD_DATA = 0; //b3=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   __LCD_DATA = 0; //b4=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   __LCD_DATA = 0; //b5=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;
   
   __LCD_DATA = 0; //b6=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   __LCD_DATA = 0; //b7=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   __LCD_DATA = 0; //b8=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   __LCD_DATA = 0; //b9=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   //control data
   __LCD_DATA = 0; //b10=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;
   
   __LCD_DATA = 0; //b11=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   __LCD_DATA = 0; //b12=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   __LCD_DATA = 0; //b13=0
   //clock upside front edge
   __SCL_1EDGE_DELAY;

   
   //DD
   __LCD_DATA = 1; //DD1=1
   //clock upside front edge
   __SCL_1EDGE_DELAY;
   
   __LCD_DATA = 1; //DD0=1
   //clock upside front edge
   __SCL_1EDGE_DELAY;
      
   //OS_Delay(1);
   __LCD_CE   = 0;
   //OS_Delay(1);

   __LCD_INH  = 1;
   {
      char cpt=0;
    for(cpt=0;cpt<50;cpt++);
   }
   __LCD_INH  = 1;

   return 0;
}

/*-------------------------------------------
| Name:dev_lcd_sanyo_lc75813_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_lcd_sanyo_lc75813_load(void){
   PD4|=0x1F;//P4.0 to P4.4 set ouput mode.

   /*__LCD_SCL   = 0;
   __LCD_SCL   = 1;
   
   __LCD_CE    = 0;
   __LCD_CE    = 1;

   __LCD_INH   = 0;
   __LCD_INH   = 1;

   __LCD_DATA  = 0;
   __LCD_DATA  = 1;*/
      
   
   _lcd_sanyo_lc75813_write_data_block1(_sanyo_lc75813_bits_buffer,sizeof(_sanyo_lc75813_bits_buffer));
   _lcd_sanyo_lc75813_write_data_block2(_sanyo_lc75813_bits_buffer,sizeof(_sanyo_lc75813_bits_buffer));
   _lcd_sanyo_lc75813_write_data_block3(_sanyo_lc75813_bits_buffer,sizeof(_sanyo_lc75813_bits_buffer));
   _lcd_sanyo_lc75813_write_data_block4(_sanyo_lc75813_bits_buffer,sizeof(_sanyo_lc75813_bits_buffer));
   return 0;
}

/*-------------------------------------------
| Name:dev_lcd_sanyo_lc75813_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_lcd_sanyo_lc75813_open(desc_t desc, int o_flag){

   //
   if(o_flag & O_RDONLY){
      return -1;
   }

   if(o_flag & O_WRONLY){
   }

   ofile_lst[desc].offset=0;

   return 0;
}

/*-------------------------------------------
| Name:dev_lcd_sanyo_lc75813_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_lcd_sanyo_lc75813_close(desc_t desc){

   if(ofile_lst[desc].oflag & O_WRONLY){
      if(!ofile_lst[desc].nb_writer){
      }
   }

   return 0;
}

/*-------------------------------------------
| Name:dev_lcd_sanyo_lc75813_isset_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_lcd_sanyo_lc75813_isset_read(desc_t desc){
   return -1;
}

/*-------------------------------------------
| Name:dev_lcd_sanyo_lc75813_isset_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_lcd_sanyo_lc75813_isset_write(desc_t desc){
   return -1;
}

/*-------------------------------------------
| Name:dev_lcd_sanyo_lc75813_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_lcd_sanyo_lc75813_read(desc_t desc, char* buf,int size){
   return 0;
}

/*-------------------------------------------
| Name:dev_lcd_sanyo_lc75813_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_lcd_sanyo_lc75813_write(desc_t desc, const char* buf,int size){
   int cb=0;
   lcdframebuffer_t lcdframebuffer;

   for(cb=0;cb<size;cb++){
      lcdframebuffer.pos=ofile_lst[desc].offset+cb;
      lcdframebuffer.v=((signed char)buf[cb]);
     __set_picto(lcdframebuffer.pos,lcdframebuffer.v);
   }

   _lcd_sanyo_lc75813_write_data_block1(_sanyo_lc75813_bits_buffer,sizeof(_sanyo_lc75813_bits_buffer));
   _lcd_sanyo_lc75813_write_data_block2(_sanyo_lc75813_bits_buffer,sizeof(_sanyo_lc75813_bits_buffer));
   _lcd_sanyo_lc75813_write_data_block3(_sanyo_lc75813_bits_buffer,sizeof(_sanyo_lc75813_bits_buffer));
   _lcd_sanyo_lc75813_write_data_block4(_sanyo_lc75813_bits_buffer,sizeof(_sanyo_lc75813_bits_buffer));
  
   ofile_lst[desc].offset+=cb;


   return cb;
}

/*-------------------------------------------
| Name:dev_lcd_sanyo_lc75813_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_lcd_sanyo_lc75813_seek(desc_t desc,int offset,int origin){
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
| Name:        dev_lcd_sanyo_lc75813_ioctl
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int dev_lcd_sanyo_lc75813_ioctl(desc_t desc,int request,va_list ap){
   return -1;
}

/*============================================
| End of Source  : dev_lcd_sanyo_lc75813.c
==============================================*/
