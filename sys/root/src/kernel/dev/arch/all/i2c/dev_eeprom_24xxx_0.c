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

/**
 * \addtogroup lepton_dev
 * @{
 *
 */

/**
 * \addtogroup hard_dev_m16c
 * @{
 *
 */

/**
 * \addtogroup dev_eeprom_c
 * @{
 *
 */


/**
 * \file
 * implementation du controleur de la première eeprom i2c
 * \author philippe le boulanger
 */

/*===========================================
Includes
=============================================*/
#include "kernel/core/types.h"
#include "kernel/core/interrupt.h"
#include "kernel/core/kernel.h"
#include "kernel/core/system.h"
#include "kernel/core/stat.h"
#include "kernel/core/fcntl.h"


#include "kernel/fs/vfs/vfsdev.h"

//I2C
#include "dev_eeprom_24xxx.h"


/*===========================================
Global Declaration
=============================================*/

int dev_eeprom_24xxx_0_load(void);


static char dev_eeprom_24xxx_0_name[]="hd\0eeprom_24xxx_0\0";


dev_map_t dev_eeprom_24xxx_0_map={
   dev_eeprom_24xxx_0_name,
   S_IFBLK,
   dev_eeprom_24xxx_0_load,
   dev_eeprom_24xxx_open,
   dev_eeprom_24xxx_close,
   __fdev_not_implemented,
   __fdev_not_implemented,
   dev_eeprom_24xxx_read,
   dev_eeprom_24xxx_write,
   dev_eeprom_24xxx_seek,
   __fdev_not_implemented //ioctl
};


/*===========================================
Implementation
=============================================*/
/*-------------------------------------------
| Name:dev_eeprom_24xxx_0_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
/*! \fn int dev_eeprom_24xxx_0_load(void)
    \brief chargement et detection du pilotes de périphérique de la première eeprom i2c.

    \return -1 si erreur sinon 0.
*/
//extern int _i2c_io_write(unsigned char dev_addr,unsigned char* data,unsigned int data_len,unsigned char addr_size);
int dev_eeprom_24xxx_0_load(void){
   unsigned char buf[16]={0};
   unsigned char dev_current_nb = (DEV_EEPROM_24XXX_0>>1);
   fdev_map_t* _l_kernel_if_i2c_master = __get_if_i2c_master();

   dev_eeprom_24xxx_0_name[3] = (char)DEV_EEPROM_24XXX_0;

   //read
   buf[0] = (unsigned char) dev_current_nb;
   buf[1] = (unsigned char) 2;
   buf[2] = (unsigned char) (0 >> 8);
   buf[3] = (unsigned char)  0;
   //
   if (_l_kernel_if_i2c_master->fdev_read(__get_if_i2c_master_desc(),buf,7+4)<0)
      return -1;

   return 0;
}

/** @} */
/** @} */
/** @} */

/*===========================================
End of Source dev_eeprom_24xxx_0.c
=============================================*/
