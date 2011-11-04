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
#include "kernel/core/ioctl.h"
#include "kernel/core/malloc.h"

#include "kernel/fs/vfs/vfsdev.h"
#include "kernel/fs/vfs/vfstypes.h"

#include "kernel/dev/arch/gnu32/common/linux_hdwr_ops.h"

#include "dev_fb.h"
#include "kernel/core/ioctl_fb.h"

/*============================================
| Global Declaration
==============================================*/
static int dev_fb_load(void);
static int dev_fb_open(desc_t desc,int o_flag);
static int dev_fb_close(desc_t desc);
static int dev_fb_isset_read(desc_t desc);
static int dev_fb_isset_write(desc_t desc);
static int dev_fb_write(desc_t desc, const char* buf,int size);
static int dev_fb_read(desc_t desc, char* buf,int size);
static int dev_fb_seek(desc_t desc,int offset,int origin);
static int dev_fb_ioctl(desc_t desc,int request,va_list ap);

static const char dev_fb_name[]="fb/fb\0";

dev_map_t dev_fb_map={
   dev_fb_name,
   S_IFBLK,
   dev_fb_load,
   dev_fb_open,
   dev_fb_close,
   dev_fb_isset_read,
   dev_fb_isset_write,
   dev_fb_read,
   dev_fb_write,
   dev_fb_seek,
   dev_fb_ioctl //ioctl
};


//list of fb_info struct
static short g_fb_no=0;
static fb_info_t * g_p_fb_list = NULL;
static short g_fb_current=0;

static void fb_insert_instance(fb_info_t * fb);
static void fb_delete_instance(unsigned short no);
static int  fb_set_config(fb_info_t * fb, unsigned short no);
static void fb_get_config(fb_info_t * fb, unsigned short no);
/*============================================
| Implementation
==============================================*/
/*--------------------------------------------
| Name:        dev_fb_load
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int dev_fb_load(void) {
   return 0;
}

/*--------------------------------------------
| Name:        dev_fb_open
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int dev_fb_open(desc_t desc,int o_flag) {
   if(ofile_lst[desc].oflag & O_RDONLY){
   }

   if(ofile_lst[desc].oflag & O_WRONLY){
   }

   return 0;
}

/*--------------------------------------------
| Name:        dev_fb_close
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int dev_fb_close(desc_t desc) {
   if(ofile_lst[desc].oflag & O_RDONLY){
      if(!ofile_lst[desc].nb_reader){
         //don't remove generic fb
         if(ofile_lst[desc].p!=(void *)g_p_fb_list) {
            ofile_lst[desc].p=NULL;
         }
      }
   }

   if(ofile_lst[desc].oflag & O_WRONLY){
      if(!ofile_lst[desc].nb_writer){
         if(!ofile_lst[desc].nb_reader){
            //don't remove generic fb
            if(ofile_lst[desc].p!=(void *)g_p_fb_list) {
               ofile_lst[desc].p=NULL;
            }
         }
      }
   }

   return 0;
}

/*--------------------------------------------
| Name:        dev_fb_seek
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int dev_fb_seek(desc_t desc,int offset,int origin) {
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


/*--------------------------------------------
| Name:        dev_fb_isset_read
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int dev_fb_isset_read(desc_t desc) {
   return -1;
}


/*--------------------------------------------
| Name:        dev_fb_isset_write
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int dev_fb_isset_write(desc_t desc) {
   return -1;
}


/*--------------------------------------------
| Name:        dev_fb_seek
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int dev_fb_read(desc_t desc, char* buf,int size) {
   //fb_info_t * fb = fb_get_current();
   fb_info_t * fb = (fb_info_t *)ofile_lst[desc].p;
   int cb = size;
   char * p_vmem = NULL;
   //is fb_exist
   if(!fb) {
      return -1;
   }
   //
   p_vmem = (char *)(fb->smem_start+ofile_lst[desc].offset);

   //test where we want to read
   if(ofile_lst[desc].offset+size>=fb->smem_len) {
      cb = fb->smem_len - ofile_lst[desc].offset;
   }
   //
   memcpy((void *)buf, (void *)p_vmem, cb);
   ofile_lst[desc].offset +=cb;
   return cb;
}

/*--------------------------------------------
| Name:        dev_fb_write
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int dev_fb_write(desc_t desc, const char* buf,int size) {
   //fb_info_t * fb = fb_get_current();
   fb_info_t * fb = (fb_info_t *)ofile_lst[desc].p;
   int cb = size;
   char * p_vmem = NULL;
   //is fb_exist
   if(!fb) {
      return -1;
   }
   //
   p_vmem = (char *)(fb->smem_start+ofile_lst[desc].offset);

   //test where we want to write
   if(ofile_lst[desc].offset+size>=fb->smem_len) {
      cb = fb->smem_len - ofile_lst[desc].offset;
   }
   //
   memcpy((void *)p_vmem, (void *)buf, cb);
   ofile_lst[desc].offset +=cb;
   return cb;
}


/*--------------------------------------------
| Name:        dev_fb_ioctl
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int dev_fb_ioctl(desc_t desc,int request,va_list ap) {

   switch(request) {
   case I_LINK:
      //must be open in O_WRONLY mode
      if(!(ofile_lst[desc].oflag & O_WRONLY)) {
         return -1;
      }

      //allow fb struct for informations
      if(!(ofile_lst[desc].p)) {
         int desc_screen=va_arg(ap, int);
         fb_info_t * fb_info=va_arg(ap, fb_info_t *);
         //
         if(!(ofile_lst[desc].p = _sys_malloc(sizeof(fb_info_t)))) {
            return -1;
         }
         else {
            memcpy(ofile_lst[desc].p, fb_info, sizeof(fb_info_t));
            fb_insert_instance((fb_info_t *)ofile_lst[desc].p);
            //now ofile_lst[desc].p is the first in g_p_fb_list
            g_p_fb_list->smem_len = g_p_fb_list->x_res*g_p_fb_list->y_res*(g_p_fb_list->bpp/8);
            g_p_fb_list->desc_w = -1;
         }

         //write descriptor
         if(ofile_lst[desc].desc_nxt[1]!=INVALID_DESC && (((fb_info_t*)ofile_lst[desc].p)->desc_w<0) )
            g_p_fb_list->desc_w = ofile_lst[desc].desc_nxt[1];
      }

   break;

   case I_UNLINK: {
      int fb_num = va_arg(ap, int);
      fb_delete_instance(fb_num);
   }
   break;

   //set informations for generic framebuffer
   case FBIOPUT_INIT: {
   }
   break;

   //set informations for a new allocated framebuffer
   case FBIOPUT_MODEINFO: {
      //get x_res, y_res, bpp, linelen and fb_no from app
      fb_info_t * fb_info = va_arg(ap, fb_info_t *);
      unsigned int fb_num;

      //not allocated data or not param infos
      if(!fb_info || !g_p_fb_list) {
         return -1;
      }
      //AND set infos
      fb_num = va_arg(ap, unsigned int);
      if(fb_set_config(fb_info, fb_num)<0) {
         return -1;
      }

      //
      fb_get_config(fb_info, fb_num);
      ofile_lst[desc].p = (void *)fb_info;
   }
   break;

   case FBIOGET_DISPINFO : {
      //get x_res, y_res, bpp, linelen and fb_no
      fb_info_t * fb_info = va_arg(ap, fb_info_t *);
      unsigned int fb_num;

      //not allocated data or not param infos
      if(!fb_info || !g_p_fb_list) {
         return -1;
      }

      //get framebuffer number unless p is set
      fb_num = va_arg(ap, unsigned int);
      if(fb_num>g_fb_no) {
         return -1;
      }
      //get infos
      fb_get_config(fb_info, fb_num);
      //
      ofile_lst[desc].p = (void *)fb_info;
   }
   break;

   default:
   break;
   }
   //
   return 0;
}


/*--------------------------------------------
| Name:        fb_insert_instance
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
----------------------------------------------*/
void fb_insert_instance(fb_info_t * fb) {
   fb_info_t * tmp;

   if(!g_p_fb_list) {
      g_p_fb_list = fb;
   }
   else {
      tmp = g_p_fb_list;
      while(tmp->next) tmp = tmp->next;
      tmp->next = fb;
   }
   g_fb_no++;
}


/*--------------------------------------------
| Name:        fb_set_config
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
----------------------------------------------*/
int fb_set_config(fb_info_t * fb, unsigned short no) {
   //data is fill with base lcd0 addr
   fb_info_t * tmp_fb = g_p_fb_list;
   unsigned long tmp_addr = g_p_fb_list->smem_start;
   fb_info_t * tmp=NULL;

   if(!(tmp = _sys_malloc(sizeof(fb_info_t)))) {
      return -1;
   }

   //init add to generic framebuffer
   fb->smem_start = tmp_addr;
   //begin with first after generic framebuffer
   while(no-- !=1) {
      tmp_addr += tmp_fb->next->y_res*
         tmp_fb->next->line_len*(tmp_fb->next->bpp/8);
      tmp_fb = tmp_fb->next;
   }
   tmp_fb->next = tmp;
   //copy infos
   memcpy((void *)tmp, (void *)fb, sizeof(fb_info_t));
   tmp->desc_w = g_p_fb_list->desc_w;
   tmp->cmap = g_p_fb_list->cmap;
   tmp->cmap_len = g_p_fb_list->cmap_len;
   tmp->smem_start = tmp_addr;
   tmp->smem_len = tmp->x_res*tmp->y_res*(tmp->bpp/8);
   //
   g_fb_no++;
   //g_fb_current++;
   return 0;
}


/*--------------------------------------------
| Name:        fb_get_config
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
----------------------------------------------*/
void fb_get_config(fb_info_t * fb, unsigned short no) {
   //data is fill with base fb{no}
   fb_info_t * tmp_fb = g_p_fb_list;
   g_fb_current = 0;

   while(no--) {
      tmp_fb = tmp_fb->next;
      //set current framebuffer
      g_fb_current++;
   }
   //copy infos
   memcpy((void *)fb, (void *)tmp_fb, sizeof(fb_info_t));
}


/*--------------------------------------------
| Name:        fb_delete_instance
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
----------------------------------------------*/
void fb_delete_instance(unsigned short no) {
   fb_info_t * tmp = g_p_fb_list;
   fb_info_t * save = NULL;

   //not delete generic framebuffer
   if(no<1) {
      return;
   }
   //set by default generic fb
   g_fb_no--;
   g_fb_current=0;
   //
   if(no == 1) {
      _sys_free(tmp->next);
      tmp->next = NULL;
      return;
   }

   //more than one entry
   no--;
   while(no--)    tmp=tmp->next;
   save = tmp->next->next;
   _sys_free(tmp->next);
   tmp->next = save;
}


/*============================================
| End of Source  : dev_fb.c
|---------------------------------------------
| Historic:
|---------------------------------------------
| Authors     | Date     | Comments
| $Log: not supported by cvs2svn $
| Revision 1.2  2009/07/30 08:11:19  jjp
| bug fix on read and write
|
| Revision 1.1  2009/07/15 13:21:55  jjp
| add basic framebuffer device
|
==============================================*/
