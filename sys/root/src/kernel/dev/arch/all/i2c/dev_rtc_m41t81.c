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
 * \defgroup dev_rtc_c Les rtc i2c.
 * @{
 *
 * Pilote de rtc i2c.
 *    
 */


/**
 * \file
 * implementation du controleurs de rtc i2c type m41t81.
 * \author philippe le boulanger
 */

/*===========================================
Includes
=============================================*/
#include "kernel/core/kernel.h"
#include "kernel/core/system.h"
#include "kernel/core/stat.h"
#include "kernel/fs/vfs/vfsdev.h"


/*===========================================
Global Declaration
=============================================*/

#define DEV_RTCM41T81_SIZE 20

//unsigned char dev_rtc_m41t81_buffer[DEV_RTCM41T81_SIZE+2]={0};
unsigned char dev_rtc_m41t81_buffer[DEV_RTCM41T81_SIZE+4];

const char dev_rtc_m41t81_addr   =  0x68; //already right shift 0x68=(rtc addr)>>1.

const char dev_rtc_m41t81_name[]="rtc0\0rtc_m41t81\0";

int dev_rtc_m41t81_load(void);
int dev_rtc_m41t81_isset_read(desc_t desc);
int dev_rtc_m41t81_isset_write(desc_t desc);
int dev_rtc_m41t81_open(desc_t desc, int o_flag);
int dev_rtc_m41t81_close(desc_t desc);
int dev_rtc_m41t81_seek(desc_t desc,int offset,int origin);
int dev_rtc_m41t81_read(desc_t desc, char* buf,int cb);
int dev_rtc_m41t81_write(desc_t desc, const char* buf,int cb);

//specific rtc device function
int dev_rtc_m41t81_settime(desc_t desc,char* buf,int size);  
int dev_rtc_m41t81_gettime(desc_t desc,char* buf,int size);  

dev_rtc_t dev_rtc_m41t81_ext={
   dev_rtc_m41t81_settime,
   dev_rtc_m41t81_gettime
};

//
dev_map_t dev_rtc_m41t81_map={
   dev_rtc_m41t81_name,
   S_IFBLK,
   dev_rtc_m41t81_load,
   dev_rtc_m41t81_open,
   dev_rtc_m41t81_close,
   __fdev_not_implemented,
   __fdev_not_implemented,
   dev_rtc_m41t81_read,
   dev_rtc_m41t81_write,
   dev_rtc_m41t81_seek,
   __fdev_not_implemented, //ioctl
   (pfdev_ext_t)&dev_rtc_m41t81_ext
};

//from netBSD
#define BCD_TO_BIN(val) ((val)=((val)&15) + ((val)>>4)*10)
#define BIN_TO_BCD(val) ((val)=(((val)/10)<<4) + (val)%10)


/*===========================================
Implementation
=============================================*/
/*-------------------------------------------
| Name:dev_rtc_m41t81_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
/*! \fn int dev_rtc_m41t81_load(void)
    \brief chargement et detection de la rtc i2c type m41t81.

    \return -1 si erreur sinon 0.
*/
int dev_rtc_m41t81_load(void){

   char sec;
   unsigned char dev_addr = dev_rtc_m41t81_addr;
   fdev_map_t* _l_kernel_if_i2c_master = __get_if_i2c_master();


   dev_rtc_m41t81_buffer[0] = (unsigned char) dev_addr ;
   dev_rtc_m41t81_buffer[1] = (unsigned char) 1;
   dev_rtc_m41t81_buffer[2] = (unsigned char) 0;//offset
   dev_rtc_m41t81_buffer[3] = (unsigned char) 0;//data
   //
   if (_l_kernel_if_i2c_master->fdev_read(__get_if_i2c_master_desc(),dev_rtc_m41t81_buffer,1+3)<0)
      return -1;
   
   // Write registers
   dev_rtc_m41t81_buffer[0]  = (unsigned char) dev_addr ;
   dev_rtc_m41t81_buffer[1]  = (unsigned char) 1;
   dev_rtc_m41t81_buffer[2]  = (unsigned char) 0x08;// adr reg
   //data
   dev_rtc_m41t81_buffer[3]  = 0x80;    // set out
   dev_rtc_m41t81_buffer[4]  = 0x00;    // Watchdog multipliers bits;    
   dev_rtc_m41t81_buffer[5]  = 0x00;    // reset SQWE and month alarm
   dev_rtc_m41t81_buffer[6]  = 0x00;    // date alarm
   dev_rtc_m41t81_buffer[7]  = 0x00;    // minutes alarm
   dev_rtc_m41t81_buffer[8]  = 0x00;    // seconds alarm
   dev_rtc_m41t81_buffer[9]  = 0x00;    // flags
   dev_rtc_m41t81_buffer[10] = 0x00;    // reserved
   dev_rtc_m41t81_buffer[11] = 0x00;    // reserved
   dev_rtc_m41t81_buffer[12] = 0x00;   // reserved
   dev_rtc_m41t81_buffer[13] = 0x00;   // square wave frequency
   if (_l_kernel_if_i2c_master->fdev_write(__get_if_i2c_master_desc(),dev_rtc_m41t81_buffer,11+3)<0)
      return -1;
   
   // Read date
   dev_rtc_m41t81_buffer[0] = (unsigned char) dev_addr ;
   dev_rtc_m41t81_buffer[1] = (unsigned char) 1;
   dev_rtc_m41t81_buffer[2] = (unsigned char) 0;//offset
   if (_l_kernel_if_i2c_master->fdev_read(__get_if_i2c_master_desc(),dev_rtc_m41t81_buffer,8+3)<0)
      return -1;
   
   sec = dev_rtc_m41t81_buffer[2];
   
   //WRITE
   //start oscillator
   dev_rtc_m41t81_buffer[0] = (unsigned char) dev_addr ;
   dev_rtc_m41t81_buffer[1] = (unsigned char) 1;
   dev_rtc_m41t81_buffer[2] = (unsigned char) 0x01;// adr reg
   //data  
   dev_rtc_m41t81_buffer[3] = sec&0x7f;// value of second only
   if (_l_kernel_if_i2c_master->fdev_write(__get_if_i2c_master_desc(),dev_rtc_m41t81_buffer,1+3)<0)
      return -1;
   
   
   return 0;
}

/*-------------------------------------------
| Name:dev_rtc_m41t81_isset_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_rtc_m41t81_isset_read(desc_t desc){
  return -1;
}

/*-------------------------------------------
| Name:dev_rtc_m41t81_isset_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_rtc_m41t81_isset_write(desc_t desc){
   return -1;
}

/*-------------------------------------------
| Name:dev_rtc_m41t81_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_rtc_m41t81_open(desc_t desc, int o_flag){
   //
   if(o_flag & O_RDONLY){
   }

   if(o_flag & O_WRONLY){
   }

   ofile_lst[desc].offset = 0;
   return 0;
}
 
/*-------------------------------------------
| Name:dev_rtc_m41t81_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_rtc_m41t81_close(desc_t desc){
   return 0;
}

/*-------------------------------------------
| Name:dev_rtc_m41t81_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_rtc_m41t81_read(desc_t desc, char* buf,int cb){
   fdev_map_t* _l_kernel_if_i2c_master = __get_if_i2c_master();

   //profiler
   __io_profiler_start(desc);

   //get time from rtc
   if(cb>=DEV_RTCM41T81_SIZE)
      cb = DEV_RTCM41T81_SIZE;

   memset(dev_rtc_m41t81_buffer,0,DEV_RTCM41T81_SIZE);

   dev_rtc_m41t81_buffer[0] = (unsigned char) dev_rtc_m41t81_addr ;
   dev_rtc_m41t81_buffer[1] = (unsigned char) 1;
   dev_rtc_m41t81_buffer[2] = (unsigned char) ofile_lst[desc].offset;
   //
   if (_l_kernel_if_i2c_master->fdev_read(__get_if_i2c_master_desc(),dev_rtc_m41t81_buffer,cb+3)<0)
      return -1;
   
   
   memcpy(buf,&dev_rtc_m41t81_buffer[3],cb);

   ofile_lst[desc].offset+=cb;

   //profiler
   __io_profiler_stop(desc);
   __io_profiler_add_result(desc,O_RDONLY,cb,__io_profiler_get_counter(desc));

   return cb;
}

/*-------------------------------------------
| Name:dev_rtc_m41t81_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_rtc_m41t81_write(desc_t desc, const char* buf,int cb){
   fdev_map_t* _l_kernel_if_i2c_master = __get_if_i2c_master();   

   //profiler
   __io_profiler_start(desc);

   if(cb>=DEV_RTCM41T81_SIZE)
      cb = DEV_RTCM41T81_SIZE;
   
   //dev_rtc_m41t81_buffer[0] = 0;//msb
   dev_rtc_m41t81_buffer[0] = (unsigned char) dev_rtc_m41t81_addr ;
   dev_rtc_m41t81_buffer[1] = (unsigned char) 1;
   dev_rtc_m41t81_buffer[2] = (unsigned char) (unsigned char) ofile_lst[desc].offset;

   memcpy(&dev_rtc_m41t81_buffer[3],buf,cb);
   
   if (_l_kernel_if_i2c_master->fdev_write(__get_if_i2c_master_desc(),dev_rtc_m41t81_buffer,cb+3)<0)
      return -1;

   ofile_lst[desc].offset+=cb;

   //profiler
   __io_profiler_stop(desc);
   __io_profiler_add_result(desc,O_WRONLY,cb,__io_profiler_get_counter(desc));

   return cb;
}

/*-------------------------------------------
| Name:dev_rtc_m41t81_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_rtc_m41t81_seek(desc_t desc,int offset,int origin){
   switch(origin){

      case SEEK_SET:
         if(offset>=DEV_RTCM41T81_SIZE)
            return -1;
         ofile_lst[desc].offset=offset;
      break;

      case SEEK_CUR:
         if(ofile_lst[desc].offset+offset>=DEV_RTCM41T81_SIZE)
            return -1;
         ofile_lst[desc].offset+=offset;
      break;

      case SEEK_END:
         //to do: warning in SEEK_END (+ or -)????
         if(ofile_lst[desc].offset+offset>=DEV_RTCM41T81_SIZE)
            return -1;
         ofile_lst[desc].offset+=offset;
      break;
   }
  
   //_i2c_core_seek(dev_rtc_m41t81_addr,ofile_lst[desc].offset,2);

   return ofile_lst[desc].offset;
}

/*-------------------------------------------
| Name:dev_rtc_m41t81_settime
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_rtc_m41t81_settime(desc_t desc,char* buf,int size){
   char bufw[8]={0};
   /*
   _tm.tm_sec  = buf[0];
   _tm.tm_min  = buf[1];
   _tm.tm_hour = buf[2];
   _tm.tm_mday = buf[3];
   _tm.tm_mon  = buf[4];
   _tm.tm_year = buf[5];
   */
   //
   dev_rtc_m41t81_seek(desc,0,SEEK_SET);

   //
   bufw[0]=0; //force to 0. 1/10 sec and 1/100 sec see M41T81 documentation).
   bufw[1]=BIN_TO_BCD(buf[0]/*tm.tm_sec*/);
   bufw[2]=BIN_TO_BCD(buf[1]/*tm.tm_min*/);

   bufw[3]=BIN_TO_BCD(buf[2]/*tm.tm_hour*/);
   bufw[3]&=0x3f;
   bufw[3]|=0x80; //CEB 1000 0000: set to 1

   bufw[5]=BIN_TO_BCD(buf[3]/*tm.tm_mday*/);
   buf[4]++;/*tm.tm_mon*/
   bufw[6]=BIN_TO_BCD(buf[4]/*tm.tm_mon*/);//m41t81 month:1..12
   buf[5]%=100;/*tm.tm_year*/
   bufw[7]=BIN_TO_BCD(buf[5]/*tm.tm_year*/);

   //
   dev_rtc_m41t81_write(desc,bufw,8);

   //
   dev_rtc_m41t81_seek(desc,0,SEEK_SET);

   return 0;
}

/*-------------------------------------------
| Name:dev_rtc_m41t81_gettime
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/

int dev_rtc_m41t81_gettime(desc_t desc,char* buf,int size){
   char bufr[8]={0};   
   /*
   _tm.tm_sec  = buf[0];
   _tm.tm_min  = buf[1];
   _tm.tm_hour = buf[2];
   _tm.tm_mday = buf[3];
   _tm.tm_mon  = buf[4];
   _tm.tm_year = buf[5];
   */
   dev_rtc_m41t81_seek(desc,0,SEEK_SET);
   dev_rtc_m41t81_read(desc,bufr,8);

   bufr[1]&=0x7f;
   buf[0]  = BCD_TO_BIN(bufr[1]);
   bufr[2]&=0x7f;
   buf[1]  = BCD_TO_BIN(bufr[2]);
   bufr[3]&=0x3f;
   buf[2] = BCD_TO_BIN(bufr[3]);
   bufr[5]&=0x3f;
   buf[3] = BCD_TO_BIN(bufr[5]);
   bufr[6]&=0x1f;
   buf[4] = (BCD_TO_BIN(bufr[6]));//_tm.tm_mon 0..11 posix see <time.h>
   buf[4]--;
   buf[5] = BCD_TO_BIN(bufr[7]);
   buf[5] += 100;

   return 0;
}

/** @} */
/** @} */
/** @} */

/*===========================================
End of Sourcedev_rtc_m41t81.c
=============================================*/
