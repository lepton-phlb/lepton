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
#ifndef _DEV_EEPROM_24XXX_H
#define _DEV_EEPROM_24XXX_H


/*===========================================
Includes
=============================================*/
#include "kernel/core/kernelconf.h"
#include "kernel/fs/vfs/vfsdev.h"


/*===========================================
Declaration
=============================================*/

extern dev_map_t dev_eeprom_24xxx_map;

int dev_eeprom_24xxx_isset_read(desc_t desc);
int dev_eeprom_24xxx_isset_write(desc_t desc);
int dev_eeprom_24xxx_open(desc_t desc, int o_flag);
int dev_eeprom_24xxx_close(desc_t desc);
int dev_eeprom_24xxx_seek(desc_t desc,int offset,int origin);
int dev_eeprom_24xxx_read(desc_t desc, char* buf,int cb);
int dev_eeprom_24xxx_write(desc_t desc, const char* buf,int cb);

//chip address
#define DEV_EEPROM_24XXX_0    0xa0//0xa0 //dev/hd/hd'(x)'
#define DEV_EEPROM_24XXX_1    0xa2 //0xa1 //dev/hd/hd'(x+1)'
#define DEV_EEPROM_24XXX_2    0xa4//0xa6 //dev/hd/hd'(x+2)'


#endif
