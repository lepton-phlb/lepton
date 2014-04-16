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


/*===========================================
Includes
=============================================*/
#include "kernel/kernelconf.h"
#include "vfs/vfsdev.h"


/*===========================================
Global Declaration
=============================================*/


extern dev_map_t dev_null_map;
extern dev_map_t dev_proc_map;
extern dev_map_t dev_cpufs_map;
extern dev_map_t dev_slip_map;
extern dev_map_t dev_mem_map;
extern dev_map_t dev_m16c_62p_map;
extern dev_map_t dev_a0350_board_map;
extern dev_map_t dev_m16c_i2c_map;
extern dev_map_t dev_eeprom_24xxx_0_map;
extern dev_map_t dev_eeprom_24xxx_1_map;
extern dev_map_t dev_m16c_uart_s1_map;
extern dev_map_t dev_a0350_arm7_s1_map;


pdev_map_t const dev_lst[]={
   &dev_null_map,
   &dev_proc_map,
   &dev_cpufs_map,
   &dev_slip_map,
   &dev_mem_map,
   &dev_m16c_62p_map,
   &dev_a0350_board_map,
   &dev_m16c_i2c_map,
   &dev_eeprom_24xxx_0_map,
   &dev_eeprom_24xxx_1_map,
   &dev_m16c_uart_s1_map,
   &dev_a0350_arm7_s1_map
};

pdev_map_t const * pdev_lst=&dev_lst[0];
const char max_dev = sizeof(dev_lst)/sizeof(pdev_map_t);
/*===========================================
Implementation
=============================================*/


/*===========================================
End of Source dev_mkconf.c
=============================================*/
