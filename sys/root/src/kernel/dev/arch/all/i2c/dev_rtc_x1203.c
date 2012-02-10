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
 * \addtogroup dev_rtc_c
 * @{
 *
 */


/**
 * \file
 * implementation du controleurs de rtc i2c type x1203.
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

#define DEV_RTCX1203_ADDR_LIMIT 0x003F
#define DEV_RTCX1203_SIZE 10

//unsigned char dev_rtc_x1203_buffer[DEV_RTCX1203_SIZE+2]={0};
unsigned char dev_rtc_x1203_buffer[DEV_RTCX1203_SIZE+2];

const char dev_rtc_x1203_addr   =  0x6F; //already right shift 0x6F(rtc addr)>>1.


const char dev_rtc_x1203_name[]="rtc1\0rtc_x1203\0";

int dev_rtc_x1203_load(void);
int dev_rtc_x1203_isset_read(desc_t desc);
int dev_rtc_x1203_isset_write(desc_t desc);
int dev_rtc_x1203_open(desc_t desc, int o_flag);
int dev_rtc_x1203_close(desc_t desc);
int dev_rtc_x1203_seek(desc_t desc,int offset,int origin);
int dev_rtc_x1203_read(desc_t desc, char* buf,int cb);
int dev_rtc_x1203_write(desc_t desc, const char* buf,int cb);

//specific rtc device function
int dev_rtc_x1203_settime(desc_t desc,char* buf,int size);
int dev_rtc_x1203_gettime(desc_t desc,char* buf,int size);

dev_rtc_t dev_rtc_x1203_ext={
   dev_rtc_x1203_settime,
   dev_rtc_x1203_gettime
};

//
dev_map_t dev_rtc_x1203_map={
   dev_rtc_x1203_name,
   S_IFBLK,
   dev_rtc_x1203_load,
   dev_rtc_x1203_open,
   dev_rtc_x1203_close,
   __fdev_not_implemented,
   __fdev_not_implemented,
   dev_rtc_x1203_read,
   dev_rtc_x1203_write,
   dev_rtc_x1203_seek,
   __fdev_not_implemented, //ioctl
   (pfdev_ext_t)&dev_rtc_x1203_ext
};

//
#define __check_x1203_addr(addr) \
   ( ((addr>=0x0000 && addr<=0x000F) \
      ||(addr==0x0011) \
      ||(addr>=0x0030 && addr<=0x0037) \
      ||(addr==0x003F) ) ? 1 : 0 )


//from netBSD
#define BCD_TO_BIN(val) ((val)=((val)&15) + ((val)>>4)*10)
#define BIN_TO_BCD(val) ((val)=(((val)/10)<<4) + (val)%10)


/*===========================================
Implementation
=============================================*/
/*-------------------------------------------
| Name:dev_rtc_x1203_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
/*! \fn int dev_rtc_x1203_load(void)
    \brief chargement et detection de la rtc i2c type x1203.

    \return -1 si erreur sinon 0.
*/

int dev_rtc_x1203_load(void){
   fdev_map_t* _l_kernel_if_i2c_master = __get_if_i2c_master();

   //test presence
   dev_rtc_x1203_buffer[0] = dev_rtc_x1203_addr;
   dev_rtc_x1203_buffer[1] = 2;
   dev_rtc_x1203_buffer[2] = 0x00;
   dev_rtc_x1203_buffer[3] = 0x00;

   if (_l_kernel_if_i2c_master->fdev_read(__get_if_i2c_master_desc(),dev_rtc_x1203_buffer,1+4)<0)
      return -1;

   return 0;
}

/*-------------------------------------------
| Name:dev_rtc_x1203_isset_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_rtc_x1203_isset_read(desc_t desc){
   return -1;
}

/*-------------------------------------------
| Name:dev_rtc_x1203_isset_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_rtc_x1203_isset_write(desc_t desc){
   return -1;
}

/*-------------------------------------------
| Name:dev_rtc_x1203_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_rtc_x1203_open(desc_t desc, int o_flag){
   //
   if(o_flag & O_RDONLY) {
   }

   if(o_flag & O_WRONLY) {
   }

   ofile_lst[desc].offset = 0;
   return 0;
}

/*-------------------------------------------
| Name:dev_rtc_x1203_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_rtc_x1203_close(desc_t desc){
   return 0;
}

/*-------------------------------------------
| Name:dev_rtc_x1203_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_rtc_x1203_read(desc_t desc, char* buf,int cb){

   fdev_map_t* _l_kernel_if_i2c_master = __get_if_i2c_master();
   int a;

   //profiler
   __io_profiler_start(desc);

   if(ofile_lst[desc].offset>DEV_RTCX1203_ADDR_LIMIT)
      return -1;  //address out of range.

   memset(buf,0,cb);
   for(a=0; a<cb; a++) {

      if(!__check_x1203_addr(ofile_lst[desc].offset+a)) continue;

      dev_rtc_x1203_buffer[0] = dev_rtc_x1203_addr;
      dev_rtc_x1203_buffer[1] = 2;
      dev_rtc_x1203_buffer[2] = (unsigned char) ((ofile_lst[desc].offset+a)>>8); //msb
      dev_rtc_x1203_buffer[3] = (unsigned char) ofile_lst[desc].offset+a; //lsb

      if (_l_kernel_if_i2c_master->fdev_read(__get_if_i2c_master_desc(),dev_rtc_x1203_buffer,1+4)<0)
         return -1;
      /*if(_i2c_core_read(dev_rtc_x1203_addr,dev_rtc_x1203_buffer,(unsigned char)1,2)<0)
         continue;*/

      buf[a] = dev_rtc_x1203_buffer[4];
   }

   if((ofile_lst[desc].offset+cb)<DEV_RTCX1203_ADDR_LIMIT)
      ofile_lst[desc].offset+=cb;
   else
      ofile_lst[desc].offset=DEV_RTCX1203_ADDR_LIMIT;

   //profiler
   __io_profiler_stop(desc);
   __io_profiler_add_result(desc,O_RDONLY,cb,__io_profiler_get_counter(desc));


   return cb;
}

/*-------------------------------------------
| Name:dev_rtc_x1203_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_rtc_x1203_write(desc_t desc, const char* buf,int cb){

   fdev_map_t* _l_kernel_if_i2c_master = __get_if_i2c_master();
   int a;

   //profiler
   __io_profiler_start(desc);

   if(ofile_lst[desc].offset>DEV_RTCX1203_ADDR_LIMIT)
      return -1;  //address out of range.

   for(a=0; a<cb; a++) {

      if(!__check_x1203_addr(ofile_lst[desc].offset+a)) continue;

      dev_rtc_x1203_buffer[0] = dev_rtc_x1203_addr;
      dev_rtc_x1203_buffer[1] = 2;
      dev_rtc_x1203_buffer[2] = (unsigned char) ((ofile_lst[desc].offset+a)>>8); //msb
      dev_rtc_x1203_buffer[3] = (unsigned char) ofile_lst[desc].offset+a; //lsb

      //data
      dev_rtc_x1203_buffer[4] = buf[a];

      if (_l_kernel_if_i2c_master->fdev_write(__get_if_i2c_master_desc(),dev_rtc_x1203_buffer,
                                              1+4)<0)
         return -1;
      /*
      if(_i2c_core_write(dev_rtc_x1203_addr,dev_rtc_x1203_buffer,(unsigned char)1,(unsigned char)2)<0)
         continue;
      */


   }

   if((ofile_lst[desc].offset+cb)<DEV_RTCX1203_ADDR_LIMIT)
      ofile_lst[desc].offset+=cb;
   else
      ofile_lst[desc].offset=DEV_RTCX1203_ADDR_LIMIT;

   //profiler
   __io_profiler_stop(desc);
   __io_profiler_add_result(desc,O_WRONLY,cb,__io_profiler_get_counter(desc));

   return cb;
}

/*-------------------------------------------
| Name:dev_rtc_x1203_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_rtc_x1203_seek(desc_t desc,int offset,int origin){
   switch(origin) {

   case SEEK_SET:
      if(ofile_lst[desc].offset>DEV_RTCX1203_ADDR_LIMIT)
         return -1;   //address out of range.

      ofile_lst[desc].offset=offset;
      break;

   case SEEK_CUR:
      if(ofile_lst[desc].offset>DEV_RTCX1203_ADDR_LIMIT)
         return -1;   //address out of range.
      ofile_lst[desc].offset+=offset;
      break;

   case SEEK_END:
      //to do: warning in SEEK_END (+ or -)????
      if(ofile_lst[desc].offset>DEV_RTCX1203_ADDR_LIMIT)
         return -1;   //address out of range.
      ofile_lst[desc].offset+=offset;
      break;
   }

   //_i2c_core_seek(dev_rtc_x1203_addr,ofile_lst[desc].offset,2);

   return ofile_lst[desc].offset;
}

/*-------------------------------------------
| Name:dev_rtc_x1203_settime
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_rtc_x1203_settime(desc_t desc,char* buf,int size){
//   char bufw[8]={0};
   char bufw[8];
   /*
   _tm.tm_sec  = buf[0];
   _tm.tm_min  = buf[1];
   _tm.tm_hour = buf[2];
   _tm.tm_mday = buf[3];
   _tm.tm_mon  = buf[4];
   _tm.tm_year = buf[5];
   */

   //step 1:write status register: enable latch EL
   dev_rtc_x1203_seek(desc,0x3f,SEEK_SET);
   bufw[0] = 0x02;
   dev_rtc_x1203_write(desc,bufw,1);
   //step 2:write status register: write enable latch WEL
   dev_rtc_x1203_seek(desc,0x3f,SEEK_SET);
   bufw[0] = 0x06;
   dev_rtc_x1203_write(desc,bufw,1);

   //write clock register
   dev_rtc_x1203_seek(desc,0x30,SEEK_SET);
   bufw[0]=BIN_TO_BCD(buf[0] /*tm.tm_sec*/);
   bufw[1]=BIN_TO_BCD(buf[1] /*tm.tm_min*/);
//   bufw[2]=BIN_TO_BCD(buf[2]/*tm.tm_hour*/)+0x80;
   bufw[2]=BIN_TO_BCD(buf[2] /*tm.tm_hour*/);
   bufw[3]=BIN_TO_BCD(buf[3] /*tm.tm_mday*/);

   buf[4]++; //_tm.tm_mon++;
   bufw[4]=BIN_TO_BCD(buf[4] /*tm.tm_mon*/); //m41t81 month:1..12

   buf[5]%=100; //_tm.tm_year%=100;
   bufw[5]=BIN_TO_BCD(buf[5] /*tm.tm_year*/);
//   dev_rtc_x1203_write(desc,bufw,8);
   dev_rtc_x1203_write(desc,bufw,8);

   //reset status register
   dev_rtc_x1203_seek(desc,0x3f,SEEK_SET);
   bufw[0] = 0x00;
   dev_rtc_x1203_write(desc,bufw,1);

   //
   dev_rtc_x1203_seek(desc,0x00,SEEK_SET);

   return 0;
}

/*-------------------------------------------
| Name:dev_rtc_x1203_gettime
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_rtc_x1203_gettime(desc_t desc,char* buf,int size){
//   char bufr[8]={0};
   char bufr[8];

   /*
   _tm.tm_sec  = buf[0];
   _tm.tm_min  = buf[1];
   _tm.tm_hour = buf[2];
   _tm.tm_mday = buf[3];
   _tm.tm_mon  = buf[4];
   _tm.tm_year = buf[5];
   */

   //
   dev_rtc_x1203_seek(desc,0x30,SEEK_SET);
//   dev_rtc_x1203_read(desc,bufr,8);
   dev_rtc_x1203_read(desc,bufr,8);
   bufr[0] &= 0x7F;
   buf[0]  = BCD_TO_BIN(bufr[0]);
   bufr[1] &= 0x7F;
   buf[1]  = BCD_TO_BIN(bufr[1]);
   bufr[2] &= 0x3F;
   buf[2] = BCD_TO_BIN(bufr[2]);
   bufr[3] &= 0x3F;
   buf[3] = BCD_TO_BIN(bufr[3]);
   bufr[4] &= 0x1F;
   buf[4]  = (BCD_TO_BIN(bufr[4])); //_tm.tm_mon 0..11 posix see <time.h>
   buf[4]--;
   buf[5] = BCD_TO_BIN(bufr[5]);
   buf[5] += 100;

   //
   dev_rtc_x1203_seek(desc,0x00,SEEK_SET);

   return 0;
}

/** @} */
/** @} */
/** @} */

/*===========================================
End of Source dev_rtc_x1203.c
=============================================*/
