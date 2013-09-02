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
#include <string.h>
#include "kernel/core/system.h"
#include "kernel/core/kernel.h"
#include "kernel/core/process.h"
#include "kernel/core/fcntl.h"
#include "kernel/core/ioctl.h"
#include "kernel/core/ioctl_lcd.h"

#include "kernel/fs/vfs/vfsdev.h"

//fb test
#if defined(__GNUC__)
   #define USE_FRAMEBUFFER
   #include "kernel/core/ioctl_fb.h"
#endif

/*============================================
| Global Declaration
==============================================*/
const char dev_tty_name[]="tty0\0";

int dev_tty_load(void);
int dev_tty_open(desc_t desc, int o_flag);
int dev_tty_close(desc_t desc);
int dev_tty_isset_read(desc_t desc);
int dev_tty_isset_write(desc_t desc);
int dev_tty_read(desc_t desc, char* buf,int size);
int dev_tty_write(desc_t desc, const char* buf,int size);
int dev_tty_seek(desc_t desc,int offset,int origin);
int dev_tty_ioctl(desc_t desc,int request,va_list ap);

dev_map_t dev_tty_map={
   dev_tty_name,
   S_IFBLK,
   dev_tty_load,
   dev_tty_open,
   dev_tty_close,
   __fdev_not_implemented,
   __fdev_not_implemented,
   dev_tty_read,
   dev_tty_write,
   dev_tty_seek,
   dev_tty_ioctl //ioctl
};

#if defined(__GNUC__)
   #define SZ_FONT_X       8
   #define SZ_FONT_Y       16
#endif

static uint32_t g_loc_X;
static uint32_t g_loc_Y;
static uint32_t g_max_loc_X;
static uint32_t g_max_loc_Y;

#define CHAR_256 0

#if !defined(__BIG_ENDIAN) && !defined(__LITTLE_ENDIAN)
   #define __LITTLE_ENDIAN
#endif


#if CHAR_256==1
   #define cmapsz  (16*256)
#else
   #define cmapsz        (16*96)
#endif

extern unsigned char vga_font[cmapsz];


typedef struct boot_infos {
   /* NEW (vers. 2) this holds the current _logical_ base addr of
   the frame buffer (for use by early boot message) */
   uchar8_t*       logicalDisplayBase;
   /* Some infos about the current MacOS display */
   uint32_t dispDeviceRect[4];
   /* left,top,right,bottom */
   uint32_t dispDeviceDepth;
   /* (8, 16 or 32) */
   uchar8_t*       dispDeviceBase;
   /* base address (physical) */
   uint32_t dispDeviceRowBytes;
   /* rowbytes (in bytes) */
   uint32_t dispDeviceColorsOffset;
   /* Colormap (8 bits only) or 0 (*) */
   /* The framebuffer size (optional, currently 0) */
   uint32_t frameBufferSize;
   /* Represents a max size, can be 0. */
} boot_infos_t;

boot_infos_t disp_bi;

uint32_t boot_text_mapped;

/*============================================
| Implementation
==============================================*/


static void btext_setup_display(uint32_t width, uint32_t height, uint32_t depth, uint32_t pitch,
                                unsigned long address)
{
   boot_infos_t* bi = &disp_bi;

   g_loc_X = 0;
   g_loc_Y = 0;
   g_max_loc_X = width / 8;
   g_max_loc_Y = (height / 16)-1;
   bi->logicalDisplayBase = (unsigned char *)address;
   bi->dispDeviceBase = (unsigned char *)address;
   bi->dispDeviceRowBytes = pitch;
   bi->dispDeviceDepth = depth;
   bi->dispDeviceRect[0] = bi->dispDeviceRect[1] = 0;
   bi->dispDeviceRect[2] = width;
   bi->dispDeviceRect[3] = height;
   boot_text_mapped = 0;
}

static unsigned char * calc_base(boot_infos_t *bi, uint32_t x, uint32_t y)
{
   unsigned char *base;
#if 0
   base = bi->logicalDisplayBase;
   if (base == 0)
#endif
   base = bi->dispDeviceBase;
   if(bi->dispDeviceDepth<8 && bi->dispDeviceDepth==1) {
      base += ((x>>3) + bi->dispDeviceRect[0]);
      base += ((y>>3) + bi->dispDeviceRect[1]) * bi->dispDeviceRowBytes;
   }else{
      base += (x + bi->dispDeviceRect[0]) * (bi->dispDeviceDepth >> 3);
      base += (y + bi->dispDeviceRect[1]) * bi->dispDeviceRowBytes;
   }
   return base;
}

static uint32_t expand_bits_8[16]  = {
#if defined(__BIG_ENDIAN)
   0x00000000,0x000000ff,0x0000ff00,0x0000ffff,
   0x00ff0000,0x00ff00ff,0x00ffff00,0x00ffffff,
   0xff000000,0xff0000ff,0xff00ff00,0xff00ffff,
   0xffff0000,0xffff00ff,0xffffff00,0xffffffff
#elif defined(__LITTLE_ENDIAN)
   0x00000000,0xff000000,0x00ff0000,0xffff0000,
   0x0000ff00,0xff00ff00,0x00ffff00,0xffffff00,
   0x000000ff,0xff0000ff,0x00ff00ff,0xffff00ff,
   0x0000ffff,0xff00ffff,0x00ffffff,0xffffffff
#else
   #error FIXME: No endianness??
#endif
};

#if 0
static const uint32_t expand_bits_16[4]  = {
   #if defined(__BIG_ENDIAN)
   0x00000000, 0x0000ffff, 0xffff0000, 0xffffffff
   #elif defined(__LITTLE_ENDIAN)
   0x00000000, 0xffff0000, 0x0000ffff, 0xffffffff
   #else
      #error FIXME: No endianness??
   #endif
};
#endif


#if 0
static void draw_byte_32(unsigned char *font, uint32_t *base, uint32_t rb){
   uint32_t l, bits;
   uint32_t fg = 0xFFFFFFFF;
   uint32_t bg = 0x00000000;

   for (l = 0; l < 16; ++l)
   {
      bits = *font++;
      base[0] = (-(bits >> 7) & fg) ^ bg;
      base[1] = (-((bits >> 6) & 1) & fg) ^ bg;
      base[2] = (-((bits >> 5) & 1) & fg) ^ bg;
      base[3] = (-((bits >> 4) & 1) & fg) ^ bg;
      base[4] = (-((bits >> 3) & 1) & fg) ^ bg;
      base[5] = (-((bits >> 2) & 1) & fg) ^ bg;
      base[6] = (-((bits >> 1) & 1) & fg) ^ bg;
      base[7] = (-(bits & 1) & fg) ^ bg;
      base = (uint32_t *) ((char *)base + rb);
   }
}

static void draw_byte_16(unsigned char *font, uint32_t *base, uint32_t rb){
   uint32_t l, bits;
   uint32_t fg = 0xFFFFFFFF;
   uint32_t bg = 0x00000000;
   uint32_t *eb = expand_bits_16;

   for (l = 0; l < 16; ++l)
   {
      bits = *font++;
      base[0] = (eb[bits >> 6] & fg) ^ bg;
      base[1] = (eb[(bits >> 4) & 3] & fg) ^ bg;
      base[2] = (eb[(bits >> 2) & 3] & fg) ^ bg;
      base[3] = (eb[bits & 3] & fg) ^ bg;
      base = (uint32_t *) ((char *)base + rb);
   }
}
#endif
static void draw_byte_8(unsigned char *font, uint32_t *base, uint32_t rb){
   uint32_t l, bits;
   //uint32_t fg = 0x0F0F0F0F;
   uint32_t fg = 0xFFFFFFFF;
   uint32_t bg = 0x00000000;
   uint32_t *eb = expand_bits_8;

   for (l = 0; l < 16; ++l)
   {
      bits = *font++;
      base[0] = (eb[bits >> 4] & fg) ^ bg;
      base[1] = (eb[bits & 0xf] & fg) ^ bg;
      base = (uint32_t *) ((char *)base + rb);
   }
}

static void draw_byte_1(unsigned char *font, char *base, uint32_t rb){
   uint32_t l;
   uchar8_t bits;
   for (l = 0; l < 16; ++l)
   {
      bits = *font++;
      base[0] = (bits);
      //base[1] = (eb[bits & 0xf] & fg) ^ bg;
      base =  ((base) + (rb>>3));
   }
}

static void draw_byte(unsigned char c, uint32_t locX, uint32_t locY){
   boot_infos_t* bi        = &disp_bi;
   unsigned char *base     = calc_base(bi, locX << 3, locY << 4);
#if CHAR_256==1
   unsigned char *font     = &vga_font[((uint32_t)c) * 16];
#else
   unsigned char *font     = &vga_font[((uint32_t)c-0x20) * 16];      // skip the first 0x20
#endif
   uint32_t rb                     = bi->dispDeviceRowBytes;

   switch(bi->dispDeviceDepth) {
#if 0
   case 24:
   case 32:
      draw_byte_32(font, (uint32_t *)base, rb);
      break;
   case 15:
   case 16:
      draw_byte_16(font, (uint32_t *)base, rb);
      break;
#endif
   case 8:
      draw_byte_8(font, (uint32_t *)base, rb);
      break;
   case 1:
      draw_byte_1(font, base, rb);
      break;
   }

}


#ifndef NO_SCROLL
static void scrollscreen(void)
{
   boot_infos_t* bi             = &disp_bi;
   uint32_t *src           = (uint32_t *)calc_base(bi,0,16);
   uint32_t *dst           = (uint32_t *)calc_base(bi,0,0);
   uint32_t width               = 0;
   uint32_t vram_sz  = 0;

   #if defined(__GNUC__)
   //nb lignes affichables * nb caracteres par lignes * taille un caractere (font_x*font_y) * profondeur (en octects)
   vram_sz = ((bi->dispDeviceRect[3]/SZ_FONT_Y)-1)*(bi->dispDeviceRect[2]/SZ_FONT_X)*
             SZ_FONT_X*SZ_FONT_Y*bi->dispDeviceDepth/8; //19*30*16*8;
   #else
   vram_sz =
      ((bi->dispDeviceRect[2] /*width*/ *
        (bi->dispDeviceRect[3] /*height*/-16)) >> ((bi->dispDeviceDepth-1)))>>3;
   #endif
   memcpy(dst,src,vram_sz);
}
#endif /* ndef NO_SCROLL */


void tty_drawchar(char c)
{
   uint32_t cline = 0;

   switch (c) {
   case '\b':
      if (g_loc_X > 0)
         --g_loc_X;
      break;
   case '\t':
      g_loc_X = (g_loc_X & -8) + 8;
      break;
   case '\r':
      g_loc_X = 0;
      break;
   case '\n':
      g_loc_X = 0;
      g_loc_Y++;
      cline = 1;
      break;
   default:
      draw_byte(c, g_loc_X++, g_loc_Y);
   }
   if (g_loc_X >= g_max_loc_X) {
      g_loc_X = 0;
      g_loc_Y++;
      cline = 1;
   }
#ifndef NO_SCROLL
   while (g_loc_Y >= g_max_loc_Y) {
      scrollscreen();
      g_loc_Y--;
   }
#else
   /* wrap around from bottom to top of screen so we don't
      waste time scrolling each line.  -- paulus. */
   if (g_loc_Y >= g_max_loc_Y)
      g_loc_Y = 0;
   if (cline) {
      for (x = 0; x < g_max_loc_X; ++x)
         draw_byte(' ', x, g_loc_Y);
   }
#endif

}



/*--------------------------------------------
| Name:        tty_ioctl_link
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
static int tty_ioctl_link (desc_t desc,int request,...){
   va_list ap;
   int r=-1;

   va_start(ap, request);
   r= ofile_lst[desc].pfsop->fdev.fdev_ioctl(desc,request,ap);
   va_end(ap);

   return r;
}

/*-------------------------------------------
| Name:dev_tty_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_tty_load(void){
   return 0;
}

/*-------------------------------------------
| Name:dev_tty_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_tty_open(desc_t desc, int o_flag){

   //
   if(o_flag & O_RDONLY) {
   }

   if(o_flag & O_WRONLY) {
   }

   ofile_lst[desc].offset=0;


   return 0;
}

/*-------------------------------------------
| Name:dev_tty_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_tty_close(desc_t desc){
   return 0;
}

/*-------------------------------------------
| Name:dev_tty_isset_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_tty_isset_read(desc_t desc){
   return -1;
}

/*-------------------------------------------
| Name:dev_tty_isset_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_tty_isset_write(desc_t desc){
   return -1;
}
/*-------------------------------------------
| Name:dev_tty_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_tty_read(desc_t desc, char* buf,int size){
   return -1;
}

/*-------------------------------------------
| Name:dev_tty_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_tty_write(desc_t desc, const char* buf,int size){
   int i;

   for(i=0; i<size; i++) {
      tty_drawchar(buf[i]);
   }

   //not link the no output device
   if(ofile_lst[desc].desc_nxt[1]<0)
      return -1;

   //tty_ioctl_link(ofile_lst[desc].desc_nxt[1],LCDFLSBUF,0);

   return size;
}

/*-------------------------------------------
| Name:dev_tty_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_tty_seek(desc_t desc,int offset,int origin){
   return -1;
}

/*-------------------------------------------
| Name:dev_ttys1_ioctl
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
#ifdef USE_FRAMEBUFFER
int dev_tty_ioctl(desc_t desc,int request,va_list ap){
   fb_info_t fb;

   switch(request) {

   case I_LINK: {
      desc_t desc_link;

      //must be open in write mode
      if(!(ofile_lst[desc].oflag&O_WRONLY))
         return -1;

      desc_link = ofile_lst[desc].desc_nxt[1];
      if(desc_link<0)
         return -1;

      //get video buffer address
      if(tty_ioctl_link(desc_link,FBIOGET_DISPINFO,&fb, 1)<0)
         return -1;

      btext_setup_display(fb.x_res, fb.y_res, fb.bpp, fb.line_len,fb.smem_start);
      return 0;
   }
   break;

   case I_UNLINK: {
   }
   break;

   //
   default:
      return -1;

   }
   return -1;
}

#else
int dev_tty_ioctl(desc_t desc,int request,va_list ap){

   switch(request) {

   case I_LINK: {
      unsigned long vaddr=0x00000000;
      desc_t desc_link;

      //must be open in write mode
      if(!(ofile_lst[desc].oflag&O_WRONLY))
         return -1;

      desc_link = ofile_lst[desc].desc_nxt[1];
      if(desc_link<0)
         return -1;

      //get video buffer address
      if(tty_ioctl_link(desc_link,LCDGETVADDR,&vaddr)<0)
         return -1;

      //btext_setup_display(320, 240, 1, 320,vaddr);
   #if defined(EVAL_BOARD)
      btext_setup_display(240, 320, 8, 240,vaddr);
   #else
      //btext_setup_display(240, 320, 8, 240,vaddr);//btext_setup_display(640, 480, 8, 640,vaddr);
      btext_setup_display(480, 640, 8, 480,vaddr);
   #endif
      return 0;
   }
   break;

   case I_UNLINK: {
   }
   break;

   //
   default:
      return -1;

   }
   return -1;
}
#endif

/*============================================
| End of Source  : dev_tty.c
==============================================*/
