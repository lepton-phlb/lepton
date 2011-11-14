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



/*===========================================
Includes
=============================================*/
#include "kernel/core/kernel.h"
#include "kernel/core/system.h"
#include "kernel/fs/vfs/vfsdev.h"

//I2C
#include "at91sam9261_twi_io.h"


/*===========================================
Global Declaration
=============================================*/
const char dev_at91sam9261_twi_io_name[]="i2c0\0";

int dev_at91sam9261_twi_io_load(void);
int dev_at91sam9261_twi_io_isset_read(desc_t desc);
int dev_at91sam9261_twi_io_isset_write(desc_t desc);
int dev_at91sam9261_twi_io_open(desc_t desc, int o_flag);
int dev_at91sam9261_twi_io_close(desc_t desc);
int dev_at91sam9261_twi_io_seek(desc_t desc,int offset,int origin);
int dev_at91sam9261_twi_io_read(desc_t desc, char* buf,int cb);
int dev_at91sam9261_twi_io_write(desc_t desc, const char* buf,int cb);

dev_map_t dev_at91sam9261_twi_io_map={
   dev_at91sam9261_twi_io_name,
   S_IFBLK,
   dev_at91sam9261_twi_io_load,
   dev_at91sam9261_twi_io_open,
   dev_at91sam9261_twi_io_close,
   dev_at91sam9261_twi_io_isset_read,
   dev_at91sam9261_twi_io_isset_write,
   dev_at91sam9261_twi_io_read,
   dev_at91sam9261_twi_io_write,
   dev_at91sam9261_twi_io_seek,
   __fdev_not_implemented //ioctl
};

static unsigned char s_twi_io_init=0;


// static kernel_pthread_mutex_t _i2c_core_mutex;

//#define __twi_lock()      kernel_pthread_mutex_lock  (&s_twi_mutex)
//#define __twi_unlock()    kernel_pthread_mutex_unlock(&s_twi_mutex)


/*===========================================
Implementation
=============================================*/

/*-------------------------------------------
| Name:dev_at91sam9261_twi_io_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91sam9261_twi_io_load(void)
{
  pthread_mutexattr_t  mutex_attr=0;

  if(s_twi_io_init)
     return 0;

  s_twi_io_init++;

  kernel_pthread_mutex_init(&_i2c_core_mutex, &mutex_attr);
  
  _twi_io_init();   // SCL en sortie

 return 0;
}

/*-------------------------------------------
| Name:dev_at91sam9261_twi_io_isset_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91sam9261_twi_io_isset_read(desc_t desc)
{
 return -1;
}

/*-------------------------------------------
| Name:dev_at91sam9261_twi_io_isset_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91sam9261_twi_io_isset_write(desc_t desc)
{
 return -1;
}

/*-------------------------------------------
| Name:dev_at91sam9261_twi_io_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91sam9261_twi_io_open(desc_t desc, int o_flag)
{
 if(o_flag & O_RDONLY)
    {
    }

 if(o_flag & O_WRONLY)
    {
    }
   
 ofile_lst[desc].offset = 0;
   
 return 0;
}
 
/*-------------------------------------------
| Name:dev_at91sam9261_twi_io_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91sam9261_twi_io_close(desc_t desc)
{
 return 0;
}

/*-------------------------------------------
| Name:dev_at91sam9261_twi_io_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91sam9261_twi_io_seek(desc_t desc,int offset,int origin)
{
 return -1;
}

/*-------------------------------------------
| Name:dev_at91sam9261_twi_io_read
| Description: lecture d'un périphérique I2C
| Parameters: à l'appel
|                buf[0]:i2c io addr , buf[1]:offset size , buf[2]:offset MSB , buf[3]:offset LSB
| Return Type: - cb = 0 lecture correcte  /  cb = -1 lecture incorrecte
|              - Datas lues en buf[3] si offset size=1  et  en buf[4] si offset size=2
| Comments: 
| See:
---------------------------------------------*/
int dev_at91sam9261_twi_io_read(desc_t desc, char* buf,int cb)
{
  unsigned int data_len=cb-2-buf[1];
  
  cb = _twi_io_read(buf[0], &buf[2], data_len, buf[1]);
  
  return (cb);
}

/*-------------------------------------------
| Name:dev_at91sam9261_twi_io_write
| Description: écriture sur un périphérique I2C
| Parameters: à l'appel
|                buf[0]:i2c io addr , buf[1]:offset size , buf[2]:offset MSB , buf[3]:offset LSB
| Return Type: - cb = 0 écriture correcte  /  cb = -1 écriture incorrecte
|              - Datas écrites en buf[3] si offset size=1  et  en buf[4] si offset size=2
| Comments: 
| See:
---------------------------------------------*/
int dev_at91sam9261_twi_io_write(desc_t desc, const char* buf,int cb)
{
  unsigned int data_len=cb-2-buf[1];
  
  // _i2c_lock();
  
  cb = _twi_io_write(buf[0], &buf[2], data_len, buf[1]);
      
  //_i2c_unlock();
  
  cyg_thread_delay(10);  
  return (cb);
}

/*============================================
| End of Source  : dev_at91sam9261_twi_io.c
==============================================*/
