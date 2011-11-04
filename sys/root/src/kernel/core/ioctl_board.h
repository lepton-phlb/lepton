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
#ifndef _BOARD_H
#define _BOARD_H


/*===========================================
Includes
=============================================*/
#include "kernel/core/ioctl.h"

/*===========================================
Declaration
=============================================*/

//ioctl command
#define BRDPWRDOWN      0x0001
#define BRDPWRUP        0x0002
#define BRDRESET        0x0004
#define BRDBEEP         0x0008
#define BRDCFGPORT      0x0010
#define BRDSETPORT      0x0020
#define BRDGETPORT      0x0040
#define BRDWATCHDOG     0x0080
#define BRDVERSION      0x0100
#define BRDPWRSAVEON    0x0200
#define BRDPWRSAVEOFF   0x0400
#define BRDPWRSAVESET   0x0800
#define BRDPWRSAVETRIGG 0x1000
#define BRDSTANDBY      0x2000


typedef enum {
   BRDBEEP_LVL_NUL,
   BRDBEEP_LVL_LOW,
   BRDBEEP_LVL_MID,
   BRDBEEP_LVL_HIG,
   BRDBEEP_LVL_MAX
}BRDBEEP_LVL;

struct board_port_t{
   unsigned char port_no;
   unsigned char type;
   unsigned char dir;
   unsigned char bit_no;
   unsigned char v;
};

struct board_beep_t{
   int frequency;
   int duration;//ms
   int level;
};


typedef int board_version_t;




#endif
