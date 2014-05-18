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


/*===========================================
Includes
=============================================*/
#include "kernel/core/kernelconf.h"
#include "kernel/fs/vfs/vfsdev.h"


/*===========================================
Global Declaration
=============================================*/


extern dev_map_t dev_null_map;
extern dev_map_t dev_proc_map;
extern dev_map_t dev_cpufs_map;
extern dev_map_t dev_pipe_map;
extern dev_map_t dev_tty_map;
extern dev_map_t dev_at91sam9261_cpu_map;
extern dev_map_t dev_at91sam9261_uart_dbg_map;
extern dev_map_t dev_at91sam9261_lcd_map;
extern dev_map_t dev_at91sam9261_rtt_map;


pdev_map_t const dev_lst[]={
   &dev_null_map,
   &dev_proc_map,
   &dev_cpufs_map,
   &dev_pipe_map,
   &dev_tty_map,
   &dev_at91sam9261_cpu_map,
   &dev_at91sam9261_uart_dbg_map,
   &dev_at91sam9261_lcd_map,
   &dev_at91sam9261_rtt_map
};

pdev_map_t const * pdev_lst=&dev_lst[0];
const char max_dev = sizeof(dev_lst)/sizeof(pdev_map_t);
/*===========================================
Implementation
=============================================*/


/*===========================================
| End of Source : dev_mkconf.c
|--------------------------------------------
| Historic:
|--------------------------------------------
| Authors	| Date	| Comments
|--------------------------------------------
=============================================*/
