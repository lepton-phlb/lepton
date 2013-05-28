/*-------------------------------------------
| Copyright(C) 2007 CHAUVIN-ARNOUX
---------------------------------------------
| Project:
| Project Manager:
| Source: dev_mkconf.c
| Path: X:\sources\kernel\config
| Authors:
| Plateform:
| Created:
| Revision/Date: $Revision: 1.14 $  $Date: 2007/12/03 11:29:39 $
| Description:
---------------------------------------------
| Historic:
---------------------------------------------
| Authors	| Date	| Comments
 -
---------------------------------------------*/


/*===========================================
Includes
=============================================*/
#include "kernel/core/kernelconf.h"
#include "kernel/fs/vfs/vfsdev.h"


/*===========================================
Global Declaration
=============================================*/


extern dev_map_t  dev_null_map;
extern dev_map_t  dev_head_map;
extern dev_map_t  dev_proc_map;
extern dev_map_t  dev_cpufs_map;
extern dev_map_t  dev_pipe_map;
extern dev_map_t  dev_posix_mqueue_map;
extern dev_map_t  dev_tty_map;
extern dev_map_t  dev_sock_map;
extern dev_map_t  dev_win32_filerom_map;
extern dev_map_t  dev_win32_fileflash_map;
extern dev_map_t  dev_win32_flash_map;
extern dev_map_t  dev_win32_com0_map;
extern dev_map_t  dev_win32_com1_map;
extern dev_map_t  dev_win32_rtc_map;
extern dev_map_t  dev_ppp_uip_map;


pdev_map_t const dev_lst[]={
&dev_null_map,
&dev_head_map,
&dev_proc_map,
&dev_cpufs_map,
&dev_pipe_map,
&dev_posix_mqueue_map,
&dev_tty_map,
&dev_sock_map,
&dev_win32_filerom_map,
&dev_win32_fileflash_map,
&dev_win32_flash_map,
&dev_win32_com0_map,
&dev_win32_com1_map,
&dev_win32_rtc_map,
&dev_ppp_uip_map
};

pdev_map_t const * pdev_lst=&dev_lst[0];
const char max_dev = sizeof(dev_lst)/sizeof(pdev_map_t);
/*===========================================
Implementation
=============================================*/


/*===========================================
End of Source dev_mkconf.c
=============================================*/
