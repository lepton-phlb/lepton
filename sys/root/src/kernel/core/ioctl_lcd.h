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
#ifndef _IOCTL_LCD_H
#define _IOCTL_LCD_H


/*============================================
| Includes
==============================================*/
#include "kernel/core/kernelconf.h"

/*============================================
| Declaration
==============================================*/


#define LCDFLSBUF          0x0001
#define LCDGETVADDR        0x0002

#define LCDGETCONTRAST     0x0003
#define LCDGETLUMINOSITY   0x0004
#define LCDSETCONTRAST     0x0005
#define LCDSETLUMINOSITY   0x0006
#define LCDSETBACKLIGHTON  0x0007
#define LCDSETBACKLIGHTOFF 0x0008

//for driving lowlevel lcd
#define LCDSETSHWBUFF      0x0009
#define LCDGETACTADDR      0x000a
#define LCDSWITCHBUFF      0x000b

//inspired by sun solaris BSD http://docs.sun.com/app/docs/doc/816-5177/fbio-7i?a=view
#define FBIOPUTCMAP        0x0010
#define FBIOGETCMAP        0x0011

#if (__tauon_compiler__!=__compiler_gnuc__)
   #pragma pack(push, 1)
#endif

typedef struct fbcmap {
   unsigned char red;              /* red color map elements */
   unsigned char green;         /* green color map elements */
   unsigned char blue;             /* blue color map elements */
   unsigned char pe_flags;
}fbcmap_t;

#if (__tauon_compiler__!=__compiler_gnuc__)
   #pragma pack(pop)
#endif
//end 1 byte struct alignment

#endif
