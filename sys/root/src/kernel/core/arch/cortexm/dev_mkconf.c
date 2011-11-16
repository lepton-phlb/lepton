/*-------------------------------------------
| Copyright(C) 2008 CHAUVIN-ARNOUX
---------------------------------------------
| Project:
| Project Manager:
| Source: dev_mkconf.c
| Path: config
| Authors:
| Plateform:GNU/Linux
| Created:
| Revision/Date:	$Revision: 1.3 $ $Date: 2009-08-28 09:56:38 $
| Description:
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
extern dev_map_t  dev_proc_map;
extern dev_map_t  dev_cpufs_map;
extern dev_map_t  dev_lwip_sock_map;
extern dev_map_t  dev_lwip_stack_map;
extern dev_map_t  dev_k60n512_uart_s3_map;
extern dev_map_t  dev_k60n512_enet_map;


pdev_map_t const dev_lst[]={
&dev_null_map,
&dev_proc_map,
&dev_cpufs_map,
&dev_lwip_sock_map,
&dev_lwip_stack_map,
&dev_k60n512_uart_s3_map,
&dev_k60n512_enet_map
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
