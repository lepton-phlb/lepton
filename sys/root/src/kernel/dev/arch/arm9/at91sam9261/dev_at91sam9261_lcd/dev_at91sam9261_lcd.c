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


/*============================================
| Includes
==============================================*/
#include "kernel/core/types.h"
#include "kernel/core/interrupt.h"
#include "kernel/core/kernel.h"
#include "kernel/core/system.h"
#include "kernel/core/fcntl.h"
#include "kernel/core/ioctl_lcd.h"
#include "kernel/core/ioctl_fb.h"


#include "kernel/fs/vfs/vfsdev.h"

#include "at91sam9261_lcd_lut.h"
#include "at91sam9261_lcd.h"

#if defined(USE_SEGGER)
   #include <ioat91sam9261.h>
   #include "kernel/core/ucore/embOSARM7_332/arch/cpu_at91sam9261/at91sam9261_init.h"
#endif

#include "kernel/dev/arch/arm9/at91sam9261/dev_at91sam9261_cpu/dev_at91sam9261_cpu.h"


/*============================================
| Global Declaration
==============================================*/


#define DFLT_CONTRAST      70 //70%
//#define DFLT_LUMINOSITY    30 //30%
#define DFLT_LUMINOSITY    69 //69%


static int at91sam9261_lcd_contrast    = DFLT_CONTRAST;
static int at91sam9261_lcd_luminosity  = DFLT_LUMINOSITY;

static int at91sam9261_lcd_frame_buffer_sz=0;

static char f_at91sam9261_lcd_open_count=0;

const char dev_at91sam9261_lcd_name[]="lcd0\0";

/*============================================
| Implementation
==============================================*/

/*-------------------------------------------
| Name:dev_at91sam9261_lcd_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91sam9261_lcd_load(dev_panel_info_t* p_dev_panel_info){

   char* p_at91sam9261_lcd_frame_buffer=(char*)at91sam9261_lcd_get_current_frame_buffer();
   int err=0;

   if(!p_dev_panel_info)
      return -1;

   if(!p_at91sam9261_lcd_frame_buffer)
      return -1;

   if((err=at91sam9261_lcd_ctrl_init(p_dev_panel_info))<0)
      return err;

   at91sam9261_lcd_frame_buffer_sz = at91sam9261_lcd_calc_frame_buffer_size();
   memset(p_at91sam9261_lcd_frame_buffer,0x00,at91sam9261_lcd_frame_buffer_sz);
   return 0;
}

/*-------------------------------------------
| Name:dev_at91sam9261_lcd_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91sam9261_lcd_open(desc_t desc, int o_flag){

   //
   if(o_flag & O_RDONLY) {
      return -1;
   }

   if(o_flag & O_WRONLY) {

      if((f_at91sam9261_lcd_open_count++)>0) //already initialized
         return 0;

   }

   ofile_lst[desc].offset=0;

   return 0;
}

/*-------------------------------------------
| Name:dev_at91sam9261_lcd_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91sam9261_lcd_close(desc_t desc){

   if(ofile_lst[desc].oflag & O_WRONLY) {
      f_at91sam9261_lcd_open_count =
         (f_at91sam9261_lcd_open_count>0 ? f_at91sam9261_lcd_open_count-1 : 0);
      if(!ofile_lst[desc].nb_writer) {
      }
   }

   return 0;
}

/*-------------------------------------------
| Name:dev_at91sam9261_lcd_isset_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91sam9261_lcd_isset_read(desc_t desc){
   return -1;
}

/*-------------------------------------------
| Name:dev_at91sam9261_lcd_isset_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91sam9261_lcd_isset_write(desc_t desc){
   return -1;
}

/*-------------------------------------------
| Name:dev_at91sam9261_lcd_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91sam9261_lcd_read(desc_t desc, char* buf,int size){
   int cb=size;
   uchar8_t* p_vmem=(char*)at91sam9261_lcd_get_current_frame_buffer();
   p_vmem+=ofile_lst[desc].offset;

   if(ofile_lst[desc].offset+size>=at91sam9261_lcd_frame_buffer_sz)
      cb=(at91sam9261_lcd_frame_buffer_sz-ofile_lst[desc].offset);

   memcpy(buf,p_vmem,cb);

   ofile_lst[desc].offset+=cb;
   return cb;
}

/*-------------------------------------------
| Name:dev_at91sam9261_lcd_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91sam9261_lcd_write(desc_t desc, const char* buf,int size){
   int cb=size;
   int vmem_size=at91sam9261_lcd_frame_buffer_sz;

   uchar8_t* p_vmem=(char*)at91sam9261_lcd_get_shadow_frame_buffer();

   p_vmem+=ofile_lst[desc].offset;

   if(ofile_lst[desc].offset+size>=vmem_size)
      cb=(vmem_size-ofile_lst[desc].offset);

   memcpy(p_vmem,buf,cb);

   ofile_lst[desc].offset+=cb;
   return cb;
}

/*-------------------------------------------
| Name:dev_at91sam9261_lcd_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91sam9261_lcd_seek(desc_t desc,int offset,int origin){
   switch(origin) {

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
| Name:        dev_at91sam9261_lcd_ioctl
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int dev_at91sam9261_lcd_ioctl(desc_t desc,int request,va_list ap){

   switch(request) {
   //flush internal buffer of lcd device driver
   case LCDFLSBUF: {
      at91sam9261_lcd_switch_active_frame_buffer();
   }
   break;

   //get physical video buffer address
   case LCDGETVADDR: {
      unsigned long* vaddr= va_arg( ap, unsigned long*);
      *vaddr=at91sam9261_lcd_get_current_frame_buffer();
   }
   break;


   case LCDGETCONTRAST: {
      unsigned int* p_v= va_arg( ap, int*);
      *p_v = at91sam9261_lcd_contrast;
   }
   break;

   case LCDGETLUMINOSITY: {
      unsigned int* p_v= va_arg( ap, int*);
      *p_v = at91sam9261_lcd_luminosity;
   }
   break;

   case LCDSETCONTRAST: {
      unsigned int p_v= va_arg( ap, int);
      at91sam9261_lcd_contrast= p_v;
   }
   break;

   case LCDSETLUMINOSITY: {
      unsigned int p_v= va_arg( ap, int);
      at91sam9261_lcd_luminosity= p_v;
   }
   break;

   case LCDSETBACKLIGHTON: {
   }
   break;

   case LCDSETBACKLIGHTOFF: {
   }
   break;

   //color map (palette)
   case FBIOGETCMAP: {
      unsigned int * fbcmap_len = va_arg(ap, unsigned int *);
      fbcmap_t** fbcmap= va_arg( ap, fbcmap_t**);
      //same fields
      *fbcmap_len = 256;
      *fbcmap = (fbcmap_t*)at91sam9261_lcd_get_palette();
   }
   break;

   case FBIOPUTCMAP: {
      fbcmap_t* fbcmap= va_arg( ap, fbcmap_t*);
      at91sam9261_lcd_set_lut((const PALETTEENTRY *)fbcmap);
   }
   break;

   case FBIOGET_DISPINFO: {
      fb_info_t * fb_info = va_arg(ap, fb_info_t*);
      if(!fb_info)
         return -1;
      dev_panel_info_t* p_dev_panel_info= at91sam9261_lcd_get_panel_info();
      //fb_info_t fb_info = {FB_USER_X,FB_USER_Y,FB_USER_BPP,FB_USER_X,0,0,NULL,0,-1,NULL};

      fb_info->x_res=p_dev_panel_info->vl_col;    //x res
      fb_info->y_res=p_dev_panel_info->vl_row;    //y res

      switch(p_dev_panel_info->vl_bpix) {  //Bits per pixel, 0 = 1, 1 = 2, 2 = 4, 3 = 8, 4 = 16
      case 0:
         fb_info->bpp=1;       //bits per pixel
         break;
      case 1:
         fb_info->bpp=2;       //bits per pixel
         break;
      case 2:
         fb_info->bpp=4;       //bits per pixel
         break;
      case 3:
         fb_info->bpp=8;       //bits per pixel
         break;
      case 4:
         fb_info->bpp=16;       //bits per pixel
         break;
      }

      fb_info->line_len=p_dev_panel_info->vl_col;    //line lenght

      fb_info->smem_start=at91sam9261_lcd_get_current_frame_buffer();    //addr of framebuffer
      fb_info->smem_len=0;    //size of framebuffer

      fb_info->cmap=(fbcmap_t*)at91sam9261_lcd_get_palette();    //color map from screen
      fb_info->cmap_len=256;   //color map length

      fb_info->desc_w=-1;
      fb_info->next=(void*)0;    //next framebuffer data
   }
   break;
   //
   default:
      return -1;

   }

   return 0;
}

/*============================================
| End of Source  : dev_at91sam9261_lcd.c
==============================================*/
