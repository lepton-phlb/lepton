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
| Compiler Directive
==============================================*/
#ifndef IOCTL_FB_H_
#define IOCTL_FB_H_

/*============================================
| Includes
==============================================*/
#include "kernel/core/ioctl_lcd.h"

/*============================================
| Declaration
==============================================*/

#define FBIOGET_CURRENT          0x0001
#define FBIOSET_CURRENT          0x0002
//from kernel_src/include/linux/fb.h
#define FBIOPUT_MODEINFO         0x4617
#define FBIOGET_DISPINFO         0x4618
#define FBIOPUT_INIT             0x4619

//minimal structure for framebuffer
typedef struct fb_info_st {
   unsigned int   x_res; //x res
   unsigned int   y_res; //y res

   unsigned short bpp; //bits per pixel
   unsigned int   line_len; //line lenght

   unsigned long  smem_start; //addr of framebuffer
   unsigned int   smem_len; //size of framebuffer

   fbcmap_t * cmap; //color map from screen
   unsigned int cmap_len;//color map length

   int desc_w;
   struct fb_info_st * next; //next framebuffer data
}fb_info_t;

#endif /* IOCTL_FB_H_ */
