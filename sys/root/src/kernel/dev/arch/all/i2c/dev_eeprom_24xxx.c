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
 * \defgroup dev_eeprom_c Les eeprom i2c.
 * @{
 *
 * fonction de base pour les pilotes de périphériques de type eeprom i2c.
 *    
 */


/**
 * \file
 * implementation des controleurs eeprom i2c
 * \author philippe le boulanger
 */

/*===========================================
Includes
=============================================*/
#include "kernel/core/kernel.h"
#include "kernel/core//system.h"
#include "kernel/core/stat.h"
#include "kernel/fs/vfs/vfsdev.h"

//I2C
#include "dev_eeprom_24xxx.h"

/**
 *
 * taille des eeprom.
 * 
 */
const long   I2CEEPROM_24XXX_SIZE =  32L*1024L;//(32Ko)

/**
 *
 * nombre de eeprom.
 * 
 */
#define     EEPROM_24XXX_NB      3

/**
 *
 * adresse de la première eeprom.
 * 
 */
#define     I2CEEPROM_24XXX_1    (0x0a0>>1)

/**
 *
 * adresse de la seconde eeprom.
 * 
 */
#define     I2CEEPROM_24XXX_2    (0x0a1>>1)

/**
 *
 * adresse de la troisième eeprom.
 * 
 */
#define     I2CEEPROM_24XXX_3    (0x0a2>>1)

/*===========================================
Global Declaration
=============================================*/

//volatile unsigned char     dev_current_nb;
//volatile unsigned short    dev_current_addr;

//must be modified for 24c64: must be set to 32
#define DEV_EEPROM24XXX_BUFFERSIZE  64

//IO_EEPROM24XXX_BUFFERSIZE 64 byte + 2 addr byte
static unsigned char dev_eeprom_buffer[DEV_EEPROM24XXX_BUFFERSIZE+4];

const char dev_eeprom_24xxx_name[]="hd\0";

int dev_eeprom_24xxx_load(void);
int dev_eeprom_24xxx_isset_read(desc_t desc);
int dev_eeprom_24xxx_isset_write(desc_t desc);
int dev_eeprom_24xxx_open(desc_t desc, int o_flag);
int dev_eeprom_24xxx_close(desc_t desc);
int dev_eeprom_24xxx_seek(desc_t desc,int offset,int origin);
int dev_eeprom_24xxx_read(desc_t desc, char* buf,int cb);
int dev_eeprom_24xxx_write(desc_t desc, const char* buf,int cb);

dev_map_t dev_eeprom_24xxx_map={
   dev_eeprom_24xxx_name,
   S_IFBLK,
   dev_eeprom_24xxx_load,
   dev_eeprom_24xxx_open,
   dev_eeprom_24xxx_close,
   __fdev_not_implemented,
   __fdev_not_implemented,
   dev_eeprom_24xxx_read,
   dev_eeprom_24xxx_write,
   dev_eeprom_24xxx_seek,
   __fdev_not_implemented //ioctl
};

#define __desc2nb(desc)\
   (pdev_lst[ofile_lst[desc].ext.dev]->dev_name[3]>>1)


static fdev_read_t   _if_i2c_master_read  = (fdev_read_t)0;
static fdev_write_t  _if_i2c_master_write = (fdev_write_t)0;
static fdev_seek_t   _if_i2c_master_seek  = (fdev_seek_t)0;


/*===========================================
Implementation
=============================================*/

/*-------------------------------------------
| Name:dev_eeprom_24xxx_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_eeprom_24xxx_load(void){
   //to do: init i2core.
   //_i2c_core_init();
   return 0;
}

/*-------------------------------------------
| Name:dev_eeprom_24xxx_isset_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_eeprom_24xxx_isset_read(desc_t desc){
  return -1;
}

/*-------------------------------------------
| Name:dev_eeprom_24xxx_isset_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_eeprom_24xxx_isset_write(desc_t desc){
   return -1;
}

/*-------------------------------------------
| Name:dev_eeprom_24xxx_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_eeprom_24xxx_open(desc_t desc, int o_flag){
   unsigned char dev_current_nb;
   fdev_map_t* _l_kernel_if_i2c_master = __get_if_i2c_master();
   if(!_l_kernel_if_i2c_master)
      return -1;

   //
   if(o_flag & O_RDONLY){
      _if_i2c_master_read = _l_kernel_if_i2c_master->fdev_read;
   }

   if(o_flag & O_WRONLY){
      _if_i2c_master_write = _l_kernel_if_i2c_master->fdev_write;
   }

   dev_current_nb    = __desc2nb(desc);//0x0a0>>1;
   ofile_lst[desc].offset = 0;//dev_current_addr  = 0;

   return 0;
}
 
/*-------------------------------------------
| Name:dev_eeprom_24xxx_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_eeprom_24xxx_close(desc_t desc){
   return 0;
}

/*-------------------------------------------
| Name:dev_eeprom_24xxx_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_eeprom_24xxx_seek(desc_t desc,int offset,int origin){
   unsigned char dev_current_nb = __desc2nb(desc);//dev_current_nb    = 0x0a0>>1;
   unsigned short dev_current_addr = (unsigned short)ofile_lst[desc].offset;

   switch(origin){

      case SEEK_SET:
         dev_current_addr  = (unsigned short)(offset%(I2CEEPROM_24XXX_SIZE));
      break;

      case SEEK_CUR:
         dev_current_addr+=(unsigned short)(offset%(I2CEEPROM_24XXX_SIZE));
         offset = dev_current_addr;
      break;

      case SEEK_END:
         dev_current_addr=(unsigned short)I2CEEPROM_24XXX_SIZE;
         offset = dev_current_addr;
      break;

   }

   ofile_lst[desc].offset = dev_current_addr;

   /*
   for(i=1;i<=EEPROM_24XXX_NB;i++){

      if(! (offset/(i*I2CEEPROM_24XXX_SIZE))  ){
         _i2c_core_seek(dev_current_nb,dev_current_addr,2);
         return offset;
      }
      else{
         dev_current_addr=(unsigned short)(offset%(i*I2CEEPROM_24XXX_SIZE));
         dev_current_nb=(0x0a0+(i-1))>>1;
      }
   }
   */

   return ofile_lst[desc].offset;
}

/*-------------------------------------------
| Name:dev_eeprom_24xxx_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_eeprom_24xxx_read(desc_t desc, char* buf,int cb){
   unsigned char  dev_current_nb = __desc2nb(desc);
   unsigned short dev_current_addr = (unsigned short)ofile_lst[desc].offset;

   if( (ofile_lst[desc].offset+(long)cb)>I2CEEPROM_24XXX_SIZE){
      if(!(cb = I2CEEPROM_24XXX_SIZE-ofile_lst[desc].offset))
         return 0;//end of device.
   }

   if(cb>=DEV_EEPROM24XXX_BUFFERSIZE)
      cb = DEV_EEPROM24XXX_BUFFERSIZE;

   //profiler
   __io_profiler_start(desc);

   dev_eeprom_buffer[0] = (unsigned char) dev_current_nb ;
   dev_eeprom_buffer[1] = (unsigned char) 2;
   dev_eeprom_buffer[2] = (unsigned char) (dev_current_addr >> 8) ;
   dev_eeprom_buffer[3] = (unsigned char) dev_current_addr ;

   
   //addr size = 0, pos is set with driverEEPROM24XXXSeek
   //use current position
   //_i2c_core_read(dev_current_nb,dev_eeprom_buffer,(unsigned char)cb,2);
   if(__get_if_i2c_master_desc()<0)
      return -1;
   _if_i2c_master_read(__get_if_i2c_master_desc(),dev_eeprom_buffer,cb+4);

   memcpy(buf,&dev_eeprom_buffer[4],cb);

   dev_current_addr+=cb;

   ofile_lst[desc].offset = dev_current_addr;

   //profiler
   __io_profiler_stop(desc);
   __io_profiler_add_result(desc,O_RDONLY,cb,__io_profiler_get_counter(desc));

   return cb;
}

/*-------------------------------------------
| Name:dev_eeprom_24xxx_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
// note:
// must be modified for 24c64:
// all >>6 or <<6 must replaced by >>5 and <<5
// this modification has been check and is ok.
int dev_eeprom_24xxx_write(desc_t desc, const char* buf,int cb){

   int segment;
   unsigned char  dev_current_nb = __desc2nb(desc);
   unsigned short dev_current_addr = (unsigned short)ofile_lst[desc].offset;

   if( (ofile_lst[desc].offset+(long)cb)>I2CEEPROM_24XXX_SIZE){
      if(!(cb = I2CEEPROM_24XXX_SIZE-ofile_lst[desc].offset))
         return 0;//end of device.
   }

   //profiler
   __io_profiler_start(desc);

   if(cb>=DEV_EEPROM24XXX_BUFFERSIZE)
      cb = DEV_EEPROM24XXX_BUFFERSIZE;
   
   segment = dev_current_addr>>6;//(driverEEPROM24XXXCurrentAddr/64)

   if(segment == ((dev_current_addr + (cb-1))>>6) ){
      dev_eeprom_buffer[0] = (unsigned char) dev_current_nb ;
      dev_eeprom_buffer[1] = (unsigned char) 2;
      dev_eeprom_buffer[2] = (unsigned char) (dev_current_addr >> 8) ;
      dev_eeprom_buffer[3] = (unsigned char) dev_current_addr ;

      memcpy(&dev_eeprom_buffer[4],buf,cb);
      //_i2c_core_write(dev_current_nb,dev_eeprom_buffer,(unsigned char)cb,2);
      if(__get_if_i2c_master_desc()<0)
         return -1;
      _if_i2c_master_write(__get_if_i2c_master_desc(),dev_eeprom_buffer,cb+4);

      dev_current_addr+=cb;
   }else{
      unsigned char size = ((segment+1)<<6 ) - dev_current_addr;

      dev_eeprom_buffer[0] = (unsigned char) dev_current_nb ;
      dev_eeprom_buffer[1] = (unsigned char) 2;
      dev_eeprom_buffer[2] = (unsigned char) (dev_current_addr >> 8) ;
      dev_eeprom_buffer[3] = (unsigned char) dev_current_addr ;

      memcpy(&dev_eeprom_buffer[4],buf,size);
      //_i2c_core_write(dev_current_nb,dev_eeprom_buffer,size,2);
      if(__get_if_i2c_master_desc()<0)
         return -1;
      _if_i2c_master_write(__get_if_i2c_master_desc(),dev_eeprom_buffer,size+4);
      dev_current_addr+=size;

      //
      dev_eeprom_buffer[0] = (unsigned char) dev_current_nb ;
      dev_eeprom_buffer[1] = (unsigned char) 2;
      dev_eeprom_buffer[2] = (unsigned char) (dev_current_addr >> 8) ;
      dev_eeprom_buffer[3] = (unsigned char) dev_current_addr ;
      
      memcpy(&dev_eeprom_buffer[4],&buf[size],cb-size);
      //_i2c_core_write(dev_current_nb,dev_eeprom_buffer,(unsigned char)(cb-size),2);
      _if_i2c_master_write(__get_if_i2c_master_desc(),dev_eeprom_buffer,(unsigned char)(cb-size)+4);

      dev_current_addr=dev_current_addr+cb-size;
   }

   ofile_lst[desc].offset = dev_current_addr;

   //profiler
   __io_profiler_stop(desc);
   __io_profiler_add_result(desc,O_WRONLY,cb,__io_profiler_get_counter(desc));

   return cb;
}

/** @} */
/** @} */
/** @} */
/*===========================================
End of Source dev_eeprom_24xxx.c
=============================================*/
