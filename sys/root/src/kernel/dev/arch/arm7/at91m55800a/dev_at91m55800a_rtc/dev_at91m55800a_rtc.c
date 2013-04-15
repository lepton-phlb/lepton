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
 * \addtogroup hard_dev_at91m55800a
 * @{
 *
 */

/**
 * \defgroup dev_at91m55800a_rtc
 * @{
 *
 * Pilote de rtc pour l'arm7 at91m55800a.
 *
 */


/**
 * \file
 * implementation du controleurs de rtc pour l'arm7 at91m55800a.
 * \author philippe le boulanger
 */

/*===========================================
Includes
=============================================*/
#include "kernel/core/types.h"
#include "kernel/core/interrupt.h"
#include "kernel/core/kernelconf.h"
#include "kernel/core/kernel.h"
#include "kernel/core/system.h"
#include "kernel/core/fcntl.h"

#include "lib/libc/termios/termios.h"

#include "kernel/fs/vfs/vfsdev.h"

/*===========================================
Global Declaration
=============================================*/
#if defined(GNU_GCC)
   #define __RTC_MR        *((unsigned int *)      0xFFFB8000)
   #define __RTC_HMR       *((unsigned int *)      0xFFFB8004)
   #define __RTC_TIMR      *((unsigned int *)      0xFFFB8008)
   #define __RTC_CALR      *((unsigned int *)      0xFFFB800C)
   #define __RTC_TAR       *((unsigned int *)      0xFFFB8010)
   #define __RTC_CAR       *((unsigned int *)      0xFFFB8014)
   #define __RTC_SR                *((unsigned int *)      0xFFFB8018)
   #define __RTC_SCR       *((unsigned int *)      0xFFFB801C)
   #define __RTC_IER       *((unsigned int *)      0xFFFB8020)
   #define __RTC_IDR       *((unsigned int *)      0xFFFB8024)
   #define __RTC_IMR       *((unsigned int *)      0xFFFB8028)
   #define __RTC_VER       *((unsigned int *)      0xFFFB802C)
#endif

const char dev_at91m55800a_rtc_name[]="rtc0\0\0";

int dev_at91m55800a_rtc_load(void);
int dev_at91m55800a_rtc_isset_read(desc_t desc);
int dev_at91m55800a_rtc_isset_write(desc_t desc);
int dev_at91m55800a_rtc_open(desc_t desc, int o_flag);
int dev_at91m55800a_rtc_close(desc_t desc);
int dev_at91m55800a_rtc_seek(desc_t desc,int offset,int origin);
int dev_at91m55800a_rtc_read(desc_t desc, char* buf,int cb);
int dev_at91m55800a_rtc_write(desc_t desc, const char* buf,int cb);

//specific rtc device function
int dev_at91m55800a_rtc_settime(desc_t desc,char* buf,int size);
int dev_at91m55800a_rtc_gettime(desc_t desc,char* buf,int size);

dev_rtc_t dev_at91m55800a_rtc_ext={
   dev_at91m55800a_rtc_settime,
   dev_at91m55800a_rtc_gettime
};

//
dev_map_t dev_at91m55800a_rtc_map={
   dev_at91m55800a_rtc_name,
   S_IFBLK,
   dev_at91m55800a_rtc_load,
   dev_at91m55800a_rtc_open,
   dev_at91m55800a_rtc_close,
   __fdev_not_implemented,
   __fdev_not_implemented,
   dev_at91m55800a_rtc_read,
   dev_at91m55800a_rtc_write,
   dev_at91m55800a_rtc_seek,
   __fdev_not_implemented, //ioctl
   (pfdev_ext_t)&dev_at91m55800a_rtc_ext
};

//from netBSD
#define BCD_TO_BIN(val) ((val)=((val)&15) + ((val)>>4)*10)
#define BIN_TO_BCD(val) ((val)=(((val)/10)<<4) + (val)%10)

/*===========================================
Implementation
=============================================*/
/*-------------------------------------------
| Name:dev_at91m55800a_rtc_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
/*! \fn int dev_at91m55800a_rtc_load(void)
    \brief chargement et detection de la rtc i2c type m41t81.

    \return -1 si erreur sinon 0.
*/
int dev_at91m55800a_rtc_load(void){
#if defined(GNU_GCC)
   __RTC_MR  = 0x0L;      //instead __RTC_MR
   __RTC_IER = 0x0L;
   __RTC_IER = 0xffffffff;
   __RTC_HMR &= ~(0x1L);     //mode 12/24 (24) instead __RTC_HMR.
#else
   __RTC_CR  = 0x0L; //instead __RTC_MR
   __RTC_IER = 0x0L;
   //__RTC_IER = 0xffffffff;
   __RTC_MR &= ~(0x1L); //mode 12/24 (24) instead __RTC_HMR.
#endif
   return 0;
}

/*-------------------------------------------
| Name:dev_at91m55800a_rtc_isset_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91m55800a_rtc_isset_read(desc_t desc){
   return -1;
}

/*-------------------------------------------
| Name:dev_at91m55800a_rtc_isset_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91m55800a_rtc_isset_write(desc_t desc){
   return -1;
}

/*-------------------------------------------
| Name:dev_at91m55800a_rtc_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91m55800a_rtc_open(desc_t desc, int o_flag){
   //
   if(o_flag & O_RDONLY) {
   }

   if(o_flag & O_WRONLY) {
   }

   ofile_lst[desc].offset = 0;
   return 0;
}

/*-------------------------------------------
| Name:dev_at91m55800a_rtc_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91m55800a_rtc_close(desc_t desc){
   return 0;
}

/*-------------------------------------------
| Name:dev_at91m55800a_rtc_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91m55800a_rtc_read(desc_t desc, char* buf,int cb){
#if defined(GNU_GCC)
   unsigned char * ptr_timr = (unsigned char *)0xFFFB8008;     //__RTC_TIMR;
   unsigned char * ptr_calr = (unsigned char *)0xFFFB800C;     //__RTC_CALR;
   buf[0] = 0;

   buf[1] = (char)*ptr_timr; ptr_timr++;    //__RTC_TIMR_bit.sec
   buf[2] = (char)*ptr_timr; ptr_timr++;    //__RTC_TIMR_bit.min
   buf[3] = (char)*ptr_timr; ptr_timr++;    //__RTC_TIMR_bit.hour

   ptr_calr++;
   buf[7] = (char)*ptr_calr; ptr_calr++;    //__RTC_CALR_bit.year;
   buf[6] = (char)*ptr_calr; ptr_calr++;    //__RTC_CALR_bit.month;
   buf[5] = (char)*ptr_calr; ptr_calr++;    //__RTC_CALR_bit.date;
#else
   buf[0] = 0;
   buf[1] = __RTC_TIMR_bit.sec;
   buf[2] = __RTC_TIMR_bit.min;
   buf[3] = __RTC_TIMR_bit.hour;

   buf[7] = __RTC_CALR_bit.year;
   buf[6] = __RTC_CALR_bit.month;
   buf[5] = __RTC_CALR_bit.date;

   /*
   //to do:check bit century??? (phlb 9/11/2007)
   if(__RTC_CALR_bit.year<0x69)
      __RTC_CALR_bit.cent  = 0x20;
   else
      __RTC_CALR_bit.cent  = 0x19;
   */
#endif
   return 0;
}

/*-------------------------------------------
| Name:dev_at91m55800a_rtc_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91m55800a_rtc_write(desc_t desc, const char* buf,int cb){
#if defined(GNU_GCC)
   unsigned char * ptr_timr = (unsigned char *)0xFFFB8008;     //__RTC_TIMR;
   unsigned char * ptr_calr = (unsigned char *)0xFFFB800C;     //__RTC_CALR;
   //__RTC_CR = 3L;//stop RTC
   __RTC_MR = 3L;     //stop RTC
   while(!(__RTC_SR&1L)) ;    //wait ack update bit
   __RTC_SCR|=1L;     //clear ack update bit

   *ptr_timr = buf[1]; ptr_timr++;    //__RTC_TIMR_bit.sec   = buf[1];
   *ptr_timr = buf[2]; ptr_timr++;    //__RTC_TIMR_bit.min   = buf[2];
   *ptr_timr = buf[3]; ptr_timr++;    //__RTC_TIMR_bit.hour  = buf[3];
   //DON'T FORGET MASK

   //year century/ bug fix 9/11/2007 from juan fernandez (a0350)
   if (buf[7] <0x69)
      *ptr_calr = 0x20;          //__RTC_CALR_bit.cent  = 0x20;
   else
      *ptr_calr = 0x19;          //__RTC_CALR_bit.cent  = 0x19;
   ptr_calr++;
   //
   *ptr_calr = buf[7]; ptr_calr++;    //__RTC_CALR_bit.year  = buf[7];
   *ptr_calr = buf[6]; ptr_calr++;    //__RTC_CALR_bit.month = buf[6];
   *ptr_calr = buf[5]; ptr_calr++;    //__RTC_CALR_bit.date  = buf[5];
   //DON'T FORGET MASK

   __RTC_MR = 0L;     //__RTC_CR = 0L;//restart RTC

#else
   __RTC_CR = 3L; //stop RTC

   while(!(__RTC_SR&1L)) ;  //wait ack update bit
   __RTC_SCCR|=1L; //clear ack update bit

   __RTC_TIMR_bit.sec   = buf[1];
   __RTC_TIMR_bit.min   = buf[2];
   __RTC_TIMR_bit.hour  = buf[3];

   //year century/ bug fix 9/11/2007 from juan fernandez (a0350)
   if (buf[7] <0x69)
      __RTC_CALR_bit.cent  = 0x20;
   else
      __RTC_CALR_bit.cent  = 0x19;
   //
   __RTC_CALR_bit.year  = buf[7];
   //
   __RTC_CALR_bit.month = buf[6];
   __RTC_CALR_bit.date  = buf[5];

   __RTC_CR = 0L; //restart RTC
#endif
   return 0;
}

/*-------------------------------------------
| Name:dev_at91m55800a_rtc_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91m55800a_rtc_seek(desc_t desc,int offset,int origin){
   return -1;
}

/*-------------------------------------------
| Name:dev_at91m55800a_rtc_settime
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91m55800a_rtc_settime(desc_t desc,char* buf,int size){
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
   dev_at91m55800a_rtc_seek(desc,0,SEEK_SET);

   //
   bufw[0]=0; //force to 0. 1/10 sec and 1/100 sec see M41T81 documentation).
   bufw[1]=BIN_TO_BCD(buf[0] /*tm.tm_sec*/);
   bufw[2]=BIN_TO_BCD(buf[1] /*tm.tm_min*/);

   bufw[3]=BIN_TO_BCD(buf[2] /*tm.tm_hour*/);

   /*
   bufw[3]&=0x3f;
   bufw[3]|=0x80; //CEB 1000 0000: set to 1
   */

   bufw[5]=BIN_TO_BCD(buf[3] /*tm.tm_mday*/);
   buf[4]++; /*tm.tm_mon*/
   bufw[6]=BIN_TO_BCD(buf[4] /*tm.tm_mon*/); //m41t81 month:1..12
   buf[5]%=100; /*tm.tm_year*/
   bufw[7]=BIN_TO_BCD(buf[5] /*tm.tm_year*/);

   //
   dev_at91m55800a_rtc_write(desc,bufw,8);

   //
   dev_at91m55800a_rtc_seek(desc,0,SEEK_SET);

   return 0;
}

/*-------------------------------------------
| Name:dev_at91m55800a_rtc_gettime
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/

int dev_at91m55800a_rtc_gettime(desc_t desc,char* buf,int size){
   char bufr[8]={0};
#if defined(GNU_GCC)
   unsigned char * ptr_calr = (unsigned char *)0xFFFB800C; //__RTC_CALR;
#endif
   /*
   _tm.tm_sec  = buf[0];
   _tm.tm_min  = buf[1];
   _tm.tm_hour = buf[2];
   _tm.tm_mday = buf[3];
   _tm.tm_mon  = buf[4];
   _tm.tm_year = buf[5];
   */
   dev_at91m55800a_rtc_seek(desc,0,SEEK_SET);
   dev_at91m55800a_rtc_read(desc,bufr,8);

   bufr[1]&=0x7f;
   buf[0]  = BCD_TO_BIN(bufr[1]); //
   bufr[2]&=0x7f;
   buf[1]  = BCD_TO_BIN(bufr[2]);
   bufr[3]&=0x3f;
   buf[2] = BCD_TO_BIN(bufr[3]);
   bufr[5]&=0x3f;
   buf[3] = BCD_TO_BIN(bufr[5]);
   bufr[6]&=0x1f;
   buf[4] = (BCD_TO_BIN(bufr[6])); //_tm.tm_mon 0..11 posix see <time.h>
   buf[4]--;
   buf[5] = BCD_TO_BIN(bufr[7]);

#if defined(GNU_GCC)
   if(*ptr_calr==0x20)
#else
   if(__RTC_CALR_bit.cent==0x20)
#endif
      buf[5] += 100;

   return 0;
}

/** @} */
/** @} */
/** @} */


/*============================================
| End of Source  : dev_at91m55800a_rtc.c
==============================================*/
