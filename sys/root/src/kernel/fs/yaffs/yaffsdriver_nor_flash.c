/*--------------------------------------------
| Copyright(C) 2005 CHAUVIN-ARNOUX
|---------------------------------------------
| Project:         
| Project Manager: 
| Source:          yaffsdriver_nor_flash.c
| Path:        C:\tauon\sys\root\src\kernel\fs\yaffs
| Authors:     
| Plateform:   
| Created:     
| Revision/Date: $Revision: 1.1 $  $Date: 2010-05-31 13:44:38 $ 
| Description: 
|---------------------------------------------
| Historic:    
|---------------------------------------------
| Authors     | Date     | Comments  
| $Log: not supported by cvs2svn $
|---------------------------------------------*/


/*============================================
| Includes    
==============================================*/


#include "kernel/core/types.h"

#include "kernel/core/errno.h"
#include "kernel/core/kernel.h"
#include "kernel/core/system.h"
#include "kernel/core/systime.h"
#include "kernel/core/malloc.h"
#include "kernel/core/stat.h"

#include "kernel/core/ioctl.h"
#include "kernel/core/ioctl_hd.h"
#include "kernel/core/devio.h"


#include "kernel/fs/yaffs/core/yaffs_guts.h"
#include "kernel/fs/yaffs/core/yportenv.h"
#include "kernel/fs/yaffs/core/yaffs_trace.h"

#include "kernel/fs/yaffs/core/direct/yaffs_norif1.h"

#include "kernel/fs/yaffs/yaffscore.h"
#include "kernel/fs/yaffs/yaffs.h"


/*============================================
| Global Declaration 
==============================================*/


/*============================================
| Implementation 
==============================================*/

/*--------------------------------------------
| Name:        _yaffsdriver_nor_ioctl
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
static int _yaffsdriver_nor_flash_ioctl(desc_t desc,int request,...){
   va_list ap;
   int r=-1;

   va_start(ap, request);
   r= ofile_lst[desc].pfsop->fdev.fdev_ioctl(desc,request,ap);
   va_end(ap);

   return r;
}

/*--------------------------------------------
| Name:        _yaffsdriver_nor_flash_read32
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
void _yaffsdriver_nor_flash_read32(yaffs_Device* dev, uint32_t *addr, uint32_t *data, int nwords){
   mntdev_t* pmntdev;
   desc_t dev_desc;
   int offset = (int)addr;

   pmntdev=(mntdev_t*)dev->context;
   dev_desc=pmntdev->dev_desc;

   ofile_lst[dev_desc].pfsop->fdev.fdev_seek(dev_desc,offset,SEEK_SET);
   ofile_lst[dev_desc].pfsop->fdev.fdev_read(dev_desc,(char*)data,nwords*sizeof(uint32_t));
}

/*--------------------------------------------
| Name:        _yaffsdriver_nor_flash_write32
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
void _yaffsdriver_nor_flash_write32(yaffs_Device* dev, uint32_t *addr, uint32_t *data, int nwords){
   mntdev_t* pmntdev;
   desc_t dev_desc;
   int offset = (int)addr;

   pmntdev=(mntdev_t*)dev->context;
   dev_desc=pmntdev->dev_desc;

   ofile_lst[dev_desc].pfsop->fdev.fdev_seek(dev_desc,offset,SEEK_SET);
   ofile_lst[dev_desc].pfsop->fdev.fdev_write(dev_desc,(char*)data,nwords*sizeof(uint32_t));
}

/*--------------------------------------------
| Name:        _yaffsdriver_nor_flash_eraseblock
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
void _yaffsdriver_nor_flash_eraseblock(yaffs_Device* dev, uint32_t *addr){
   mntdev_t* pmntdev;
   desc_t dev_desc;
   int offset = (int)addr;

   pmntdev=(mntdev_t*)dev->context;
   dev_desc=pmntdev->dev_desc;

   _yaffsdriver_nor_flash_ioctl(dev_desc,HDCLRSCTR,offset);
}

/*--------------------------------------------
| Name:        _yaffsdriver_nor_flash_shutdown
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
void _yaffsdriver_nor_flash_shutdown(yaffs_Device* dev){
}

/*--------------------------------------------
| Name:        _yaffsdriver_nor_flash_initialise
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
void _yaffsdriver_nor_flash_initialise(yaffs_Device* dev){
   mntdev_t* pmntdev;
   desc_t dev_desc;

   pmntdev=(mntdev_t*)dev->context;
   dev_desc=pmntdev->dev_desc;

   //_yaffsdriver_nor_flash_ioctl(dev_desc,HDCLRDSK,0);
}

/*--------------------------------------------
| Name:        _yaffsdriver_nor_flash_getbase
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
uint32_t * _yaffsdriver_nor_flash_getbase(yaffs_Device* dev){
   return (uint32_t)0;
}

/*============================================
| End of Source  : yaffsdriver_nor_flash.c
==============================================*/