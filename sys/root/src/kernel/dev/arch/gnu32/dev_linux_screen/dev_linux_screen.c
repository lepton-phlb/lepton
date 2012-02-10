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
#include <stdlib.h>
#include <string.h>

#include "kernel/core/types.h"
#include "kernel/core/interrupt.h"
#include "kernel/core/system.h"
#include "kernel/core/fcntl.h"
#include "kernel/core/ioctl_lcd.h"
#include "kernel/core/ioctl_fb.h"


#include "kernel/fs/vfs/vfsdev.h"
#include "kernel/fs/vfs/vfstypes.h"

#include "dev_linux_screen.h"
#include "kernel/dev/arch/gnu32/common/linux_hdwr_ops.h"
#include "_screen_linux.h"

/*============================================
| Global Declaration
==============================================*/
int dev_linux_screen_load(void);
int dev_linux_screen_open(desc_t desc, int o_flag);
int dev_linux_screen_close(desc_t desc);
int dev_linux_screen_isset_read(desc_t desc);
int dev_linux_screen_isset_write(desc_t desc);
int dev_linux_screen_read(desc_t desc, char* buf,int size);
int dev_linux_screen_write(desc_t desc, const char* buf,int size);
int dev_linux_screen_seek(desc_t desc,int offset,int origin);
int dev_linux_screen_ioctl(desc_t desc,int request,va_list ap);

const char dev_linux_screen_name[]="lcd0\0";

dev_map_t dev_linux_screen_map={
   dev_linux_screen_name,
   S_IFCHR,
   dev_linux_screen_load,
   dev_linux_screen_open,
   dev_linux_screen_close,
   __fdev_not_implemented,
   __fdev_not_implemented,
   dev_linux_screen_read,
   dev_linux_screen_write,
   dev_linux_screen_seek,
   dev_linux_screen_ioctl
};

//
static int _screen_open_count=0;
static virtual_cmd_t screen_cmd;

//
extern void * shared_dev_addr;
static virtual_screen_t * screen_0_data;

//short size
#define  VIRTUAL_SCREEN_IOCTL_LOAD_XRES   0
#define  VIRTUAL_SCREEN_IOCTL_LOAD_YRES   4
#define  VIRTUAL_SCREEN_IOCTL_LOAD_BPP    8

static unsigned int screen_0_x_res=0;
static unsigned int screen_0_y_res=0;
static unsigned int screen_0_bpp=0;
static unsigned int screen_0_size=0;
/*============================================
| Implementation
==============================================*/

/*-------------------------------------------
| Name:dev_linux_screen_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_screen_load(void) {
   //
   screen_0_data = (virtual_screen_t *)(shared_dev_addr + SCRN_OFFSET);

   //get screen resolution
   screen_0_x_res = atoi(&screen_0_data->data_ioctl[VIRTUAL_SCREEN_IOCTL_LOAD_XRES]);
   screen_0_y_res = atoi(&screen_0_data->data_ioctl[VIRTUAL_SCREEN_IOCTL_LOAD_YRES]);
   screen_0_bpp = atoi(&screen_0_data->data_ioctl[VIRTUAL_SCREEN_IOCTL_LOAD_BPP]);

   if(screen_0_x_res<0 || screen_0_y_res<0 || screen_0_bpp<0) {
      return -1;
   }

   //
   screen_0_size = screen_0_x_res*screen_0_y_res*screen_0_bpp;
   screen_0_data->data_out = (unsigned char *)screen_0_data + SHM_SCRN_IOCTL;
   //
   return 0;
}

/*-------------------------------------------
| Name:dev_linux_screen_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_screen_open(desc_t desc, int o_flag) {
   if(o_flag & O_RDONLY) {
      return -1;
   }
   //
   if(o_flag & O_WRONLY) {
      if(_screen_open_count++>0) {
         return 0;
      }
   }
   //
   ofile_lst[desc].offset=0;

   return 0;
}

/*-------------------------------------------
| Name:dev_linux_screen_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_screen_close(desc_t desc) {
   if(ofile_lst[desc].oflag & O_WRONLY) {
      _screen_open_count=(_screen_open_count>0) ? _screen_open_count-1 : 0;
   }
   return 0;
}


/*-------------------------------------------
| Name:dev_linux_screen_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_screen_read(desc_t desc, char* buf,int size){
   int cb = size;
   char * p_vmem=(char *)(screen_0_data->data_out+ofile_lst[desc].offset);
   //
   if(ofile_lst[desc].offset+size>=screen_0_size) {
      cb=screen_0_size - ofile_lst[desc].offset;
   }
   //
   memcpy((void *)buf, (void *)p_vmem, cb);
   ofile_lst[desc].offset += cb;
   return cb;
}

/*-------------------------------------------
| Name:dev_linux_screen_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_screen_write(desc_t desc, const char* buf,int size){
   int cb = size;
   char * p_vmem = (char *)(screen_0_data->data_out+ofile_lst[desc].offset);
   //
   if(ofile_lst[desc].offset+size>=screen_0_size) {
      cb=screen_0_size - ofile_lst[desc].offset;
   }
   //
   memcpy((void *)p_vmem, (void *)buf, cb);
   ofile_lst[desc].offset += cb;
   return cb;
}

/*-------------------------------------------
| Name:dev_linux_screen_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_screen_seek(desc_t desc,int offset,int origin){
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
   //
   return ofile_lst[desc].offset;
}

/*-------------------------------------------
| Name:dev_linux_screen_ioctl
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_screen_ioctl(desc_t desc,int request,va_list ap) {

   switch(request) {
   case LCDGETVADDR: {
      long * vaddr = va_arg(ap, long *);
      *vaddr=(long)screen_0_data->data_out;
   }
   break;

   //color map (palette)
   case FBIOGETCMAP: {
      unsigned int * fbcmap_len = va_arg(ap, unsigned int *);
      fbcmap_t** fbcmap= va_arg( ap, fbcmap_t**);

      //same fields
      //we need palette only for 8bpp
      if(screen_0_bpp == 8 ) {
         *fbcmap_len = RGB_PALETTE_SIZE;
         *fbcmap = (fbcmap_t*)_screen_get_palette();
      }
      else {
         *fbcmap_len = -1;
         *fbcmap = (fbcmap_t*)NULL;
      }
   }
   break;

   //
   case FBIOGET_DISPINFO: {
      fb_info_t * fb_info = va_arg(ap, fb_info_t*);
      if(!fb_info)
         return -1;
      //fb_info_t fb_info = {FB_USER_X,FB_USER_Y,FB_USER_BPP,FB_USER_X,0,0,NULL,0,-1,NULL};

      fb_info->x_res=screen_0_x_res;    //x res
      fb_info->y_res=screen_0_y_res;    //y res

      fb_info->bpp=screen_0_bpp;    //bits per pixel

      fb_info->line_len=fb_info->x_res;    //line lenght

      fb_info->smem_start=(long)screen_0_data->data_out;    //addr of framebuffer
      fb_info->smem_len=0;    //size of framebuffer

      //we need palette only for 8bpp
      if(screen_0_bpp == 8 ) {
         fb_info->cmap=(fbcmap_t*)_screen_get_palette();    //color map from screen
         fb_info->cmap_len=RGB_PALETTE_SIZE;   //color map length
      }
      else {
         fb_info->cmap=(fbcmap_t*)NULL;    //color map from screen
         fb_info->cmap_len=-1;   //color map length
      }

      fb_info->desc_w=-1;
      fb_info->next=(void*)0;    //next framebuffer data
   }
   break;

   default:
      break;
   }
   return 0;
}

/*============================================
| End of Source  : dev_linux_screen.c
|---------------------------------------------
| Historic:
|---------------------------------------------
| Authors     | Date     | Comments
| $Log: not supported by cvs2svn $
| Revision 1.4  2009/07/09 15:27:42  jjp
| add commentary template
|
==============================================*/

