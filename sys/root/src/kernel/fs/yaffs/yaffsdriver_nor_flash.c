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