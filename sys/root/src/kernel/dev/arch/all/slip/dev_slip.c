/*
The contents of this file are subject to the Mozilla Public License Version 1.1
(the "License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License for the
specific language governing rights and limitations under the License.

The Original Code is Lepton.

The Initial Developer of the Original Code is Chauvin-Arnoux.
Portions created by Chauvin-Arnoux are Copyright (C) 2011. All Rights Reserved.

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
#include "kernel/core/interrupt.h"
#include "kernel/core/kernelconf.h"
#include "kernel/core/kernel.h"
#include "kernel/core/system.h"
#include "kernel/core/process.h"
#include "kernel/core/stat.h"
#include "kernel/core/fcntl.h"
#include "kernel/core/ioctl.h"
#include "kernel/core/cpu.h"
#include "kernel/core/malloc.h"

#include "kernel/fs/vfs/vfsdev.h"

#include "lib/libc/termios/termios.h"




/*============================================
| Global Declaration
==============================================*/
const char dev_slip_name[]="net/slip\0";

int dev_slip_load(void);
int dev_slip_open(desc_t desc, int o_flag);
int dev_slip_close(desc_t desc);
int dev_slip_isset_read(desc_t desc);
int dev_slip_isset_write(desc_t desc);
int dev_slip_read(desc_t desc, char* buf,int size);
int dev_slip_write(desc_t desc, const char* buf,int size);
int dev_slip_seek(desc_t desc,int offset,int origin);
int dev_slip_ioctl(desc_t desc,int request,va_list ap);

dev_map_t dev_slip_map={
   dev_slip_name,
   S_IFCHR,
   dev_slip_load,
   dev_slip_open,
   dev_slip_close,
   dev_slip_isset_read,
   dev_slip_isset_write,
   dev_slip_read,
   dev_slip_write,
   dev_slip_seek,
   dev_slip_ioctl //ioctl
};


#define END            (unsigned char)192 /*0300*/    /* indicates end of packet */
#define ESC            (unsigned char)219 /*0333*/    /* indicates byte stuffing */
#define ESC_END        (unsigned char)220 /*0334*/    /* ESC ESC_END means END data byte */
#define ESC_ESC        (unsigned char)221 /*0335*/    /* ESC ESC_ESC means ESC data byte */

#if !defined(CPU_M16C62)
   #define USE_SLIP_IO_BUFFER //not supported for very small arch
#endif

#ifdef USE_SLIP_IO_BUFFER
//#define USE_SLIP_READ_BUFFER
   #define USE_SLIP_WRITE_BUFFER

   #define SLIP_WRITE_BUFFER_SZ 2048
   #define SLIP_READ_BUFFER_SZ 2048

typedef struct {
   unsigned char buf[SLIP_WRITE_BUFFER_SZ];
   int cb;
}slip_write_t;

typedef struct {
   unsigned char buf[SLIP_READ_BUFFER_SZ];
   int cb;
   int i;
}slip_read_t;

typedef struct {
   slip_read_t read;
   slip_write_t write;
}slip_io_t;


   #define __dev_slip_write_c(__desc__,__slip_desc__,__p_slip_write__,__c__,__last__){ \
      int __cb__=-1; \
      if(!(ofile_lst[__desc__].oflag&O_NONBLOCK)) { \
         __p_slip_write__->buf[__p_slip_write__->cb++]=__c__; \
         if(__last__) { \
            __cb__ = __p_slip_write__->cb; \
            __p_slip_write__->cb = 0; \
            __cb__ = ofile_lst[__desc__].pfsop->fdev.fdev_write(__desc__,__p_slip_write__->buf, \
                                                                __cb__); \
         } \
      } \
}

#else

   #define __dev_slip_write_c(__desc_link__,__slip_desc__,__p_slip_write__,__c__, \
                              __last__) _dev_slip_write_c(__desc_link__,__c__,__last__);

#endif

/*============================================
| Implementation
==============================================*/

/*-------------------------------------------
| Name:_slip_send_packet
| Description:
| Parameters:
| Return Type:
| Comments:sends a packet of length "len", starting at
| location "p".
| See:
---------------------------------------------*/
int _dev_slip_write_c(desc_t desc, const unsigned char c, int last){
   int cb=-1;

#ifdef USE_SLIP_WRITE_BUFFER
   if(!(ofile_lst[desc].oflag&O_NONBLOCK)) {

      desc_t slip_desc=ofile_lst[desc].desc_prv;
      slip_write_t* p_slip_write = &(((slip_io_t*)ofile_lst[slip_desc].p)->write);
      if(!p_slip_write)
         return -1;

      p_slip_write->buf[p_slip_write->cb++]=c;

      if(last) {
         cb = p_slip_write->cb;
         p_slip_write->cb = 0;
         if((cb = ofile_lst[desc].pfsop->fdev.fdev_write(desc,p_slip_write->buf,cb))<0) {
            return -1;
         }
      }
   }
   return 1;

#else
   kernel_pthread_t* pthread_ptr;

   if(!(pthread_ptr = kernel_pthread_self()))
      return -1;
   if(!(ofile_lst[desc].oflag&O_NONBLOCK)) {
      if((cb = ofile_lst[desc].pfsop->fdev.fdev_write(desc,(void*)&c,1))<0) {
         return -1;
      }

      //see write() in devio.c. write wait an event from last character
      //fix a deadlock pb in this case.
      if(last)
         return cb;

      do {
         __wait_io_int(pthread_ptr);
      } while(ofile_lst[desc].pfsop->fdev.fdev_isset_write
              && ofile_lst[desc].pfsop->fdev.fdev_isset_write(desc));
   }
   return cb;
#endif
}


/*-------------------------------------------
| Name:_slip_send_packet
| Description:
| Parameters:
| Return Type:
| Comments:sends a packet of length "len", starting at
| location "p".
| See:
---------------------------------------------*/
int _dev_slip_read_c(desc_t desc, unsigned char* c,int first){
   int cb=-1;
   kernel_pthread_t* pthread_ptr;

   if(!(pthread_ptr = kernel_pthread_self()))
      return -1;

#ifdef USE_SLIP_READ_BUFFER
   if(!(ofile_lst[desc].oflag&O_NONBLOCK)) {

      desc_t slip_desc=ofile_lst[desc].desc_prv;
      slip_read_t* p_slip_read = &(((slip_io_t*)ofile_lst[slip_desc].p)->read);
      if(!p_slip_read)
         return -1;

      if(p_slip_read->cb>0 && p_slip_read->i<p_slip_read->cb) {
         *c=p_slip_read->buf[p_slip_read->i++];
         return 1;
      }

      p_slip_read->i=0;
      while(ofile_lst[desc].pfsop->fdev.fdev_isset_read
            && ofile_lst[desc].pfsop->fdev.fdev_isset_read(desc))
         __wait_io_int(pthread_ptr);   //wait incomming data
      if((p_slip_read->cb=
             ofile_lst[desc].pfsop->fdev.fdev_read(desc,p_slip_read->buf,SLIP_READ_BUFFER_SZ))<=0)
         return p_slip_read->cb;
      *c=p_slip_read->buf[p_slip_read->i++];
   }
   return 1;
#else
   if(!(ofile_lst[desc].oflag&O_NONBLOCK)) {
      if(!first) {
         while(ofile_lst[desc].pfsop->fdev.fdev_isset_read
               && ofile_lst[desc].pfsop->fdev.fdev_isset_read(desc))
            __wait_io_int(pthread_ptr);   //wait incomming data
      }
      cb=ofile_lst[desc].pfsop->fdev.fdev_read(desc,c,1);
   }

   return cb;
#endif
}


/*-------------------------------------------
| Name:dev_slip_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_slip_load(void){

   return 0;
}

/*-------------------------------------------
| Name:dev_slip_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_slip_open(desc_t desc, int o_flag){

   if(!ofile_lst[desc].nb_reader && !ofile_lst[desc].nb_writer) {
#ifdef USE_SLIP_IO_BUFFER
      slip_io_t* p_slip_io;
      if(ofile_lst[desc].p)
         return 0;
      p_slip_io= malloc(sizeof(slip_io_t));
      if(!p_slip_io)
         return -1;
      p_slip_io->write.cb = 0;
      p_slip_io->read.cb = 0;
      p_slip_io->read.i=0;
      ofile_lst[desc].p=p_slip_io;;
#endif
   }


   return 0;
}

/*-------------------------------------------
| Name:dev_slip_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_slip_close(desc_t desc){
   if(ofile_lst[desc].oflag & O_RDONLY) {
      if(!ofile_lst[desc].nb_reader) {
      }
   }

   if(ofile_lst[desc].oflag & O_WRONLY) {
      if(!ofile_lst[desc].nb_writer) {
      }
   }

   if(!ofile_lst[desc].nb_writer
      && !ofile_lst[desc].nb_reader) {
#ifdef USE_SLIP_IO_BUFFER
      free(ofile_lst[desc].p);
#endif
   }
   return 0;
}

/*-------------------------------------------
| Name:dev_slip_isset_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_slip_isset_read(desc_t desc){
   desc_t desc_link = ofile_lst[desc].desc_nxt[0];
   if(desc_link<0)
      return -1;
   if(!ofile_lst[desc_link].pfsop->fdev.fdev_isset_read)
      return -1;

   return ofile_lst[desc_link].pfsop->fdev.fdev_isset_read(desc_link);
}

/*-------------------------------------------
| Name:dev_slip_isset_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_slip_isset_write(desc_t desc){

   desc_t desc_link = ofile_lst[desc].desc_nxt[1];
   if(desc_link<0)
      return -1;
   if(!ofile_lst[desc_link].pfsop->fdev.fdev_isset_write)
      return -1;

   return ofile_lst[desc_link].pfsop->fdev.fdev_isset_write(desc_link);
   return 0;

}
/*-------------------------------------------
| Name:dev_slip_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_slip_read(desc_t desc, char* buf,int size){

   unsigned char c;
   int cb = 0;
   int r=0;
   int first=0;
   desc_t desc_link = ofile_lst[desc].desc_nxt[0];
   //
   if(desc_link<0)
      return -1;
   //
   __lock_io(ofile_lst[desc_link].owner_pthread_ptr_read,desc_link,O_RDONLY);

   /* sit in a loop reading bytes until we put together
   * a whole packet.
   * Make sure not to copy them into the packet if we
   * run out of room.
   */
   while(1) {
      /* get a character to process
      */
      if((r=_dev_slip_read_c(desc_link,&c, ((!first) ? 1 : 0) ))<=0) {
         __unlock_io(ofile_lst[desc_link].owner_pthread_ptr_read,desc_link,O_RDONLY);
         return r;
      }

      first++;
      //printf("%02x ",c);

      /* handle bytestuffing if necessary
      */
      switch(c) {

      /* if it's an END character then we're done with
      * the packet
      */
      case END:
         /* a minor optimization: if there is no
         * data in the packet, ignore it. This is
         * meant to avoid bothering IP with all
         * the empty packets generated by the
         * duplicate END characters which are in
         * turn sent to try to detect line noise.
         */
         if(cb) {
            //
            __unlock_io(ofile_lst[desc_link].owner_pthread_ptr_read,desc_link,O_RDONLY);
            return cb;
         }else{
            cb=0;
         }

         break;

      /* if it's the same code as an ESC character, wait
      * and get another character and then figure out
      * what to store in the packet based on that.
      */
      case ESC:
         if((r=_dev_slip_read_c(desc_link,&c,0))<=0) {
            __unlock_io(ofile_lst[desc_link].owner_pthread_ptr_read,desc_link,O_RDONLY);
            return r;
         }

         /* if "c" is not one of these two, then we
         * have a protocol violation.  The best bet
         * seems to be to leave the byte alone and
         * just stuff it into the packet
         */
         switch(c) {
         case ESC_END:
            c = END;
            break;

         case ESC_ESC:
            c = ESC;
            break;
         }

      /* here we fall into the default handler and let
      * it store the character for us
      */
      default:
         if(cb < size)
            buf[cb++] = c;
         else{
            __unlock_io(ofile_lst[desc_link].owner_pthread_ptr_read,desc_link,O_RDONLY);
            return cb;
         }
      }
   }

   //
   __unlock_io(ofile_lst[desc_link].owner_pthread_ptr_read,desc_link,O_RDONLY);
   //
   return cb;
}

/*-------------------------------------------
| Name:dev_slip_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_slip_write(desc_t desc, const char* buf,int size){
   int len = size;
   desc_t desc_link = ofile_lst[desc].desc_nxt[1];
   //
#ifdef USE_SLIP_IO_BUFFER
   slip_write_t* p_slip_write = (slip_write_t*)0;
#endif
   //
   if(desc_link<0)
      return -1;
   //
#ifdef USE_SLIP_IO_BUFFER
   p_slip_write = &(((slip_io_t*)ofile_lst[desc].p)->write);
#endif
   //
   __lock_io(ofile_lst[desc_link].owner_pthread_ptr_write,desc_link,O_WRONLY);

   /* send an initial END character to flush out any data that may
   * have accumulated in the receiver due to line noise
   */
   //_dev_slip_write_c(desc_link,END,0);
   __dev_slip_write_c(desc_link,desc,p_slip_write,END,0);

   /* for each byte in the packet, send the appropriate character
   * sequence
   */
   for(; len--; ) {
      switch((unsigned char)(*buf)) {
      /* if it's the same code as an END character, we send a
      * special two character code so as not to make the
      * receiver think we sent an END
      */
      case END:
         //_dev_slip_write_c(desc_link,ESC,0);
         //_dev_slip_write_c(desc_link,ESC_END,0);
         __dev_slip_write_c(desc_link,desc,p_slip_write,ESC,0);
         __dev_slip_write_c(desc_link,desc,p_slip_write,ESC_END,0);
         break;

      /* if it's the same code as an ESC character,
      * we send a special two character code so as not
      * to make the receiver think we sent an ESC
      */
      case ESC:
         //_dev_slip_write_c(desc_link,ESC,0);
         //_dev_slip_write_c(desc_link,ESC_ESC,0);
         __dev_slip_write_c(desc_link,desc,p_slip_write,ESC,0);
         __dev_slip_write_c(desc_link,desc,p_slip_write,ESC_ESC,0);
         break;

      /* otherwise, we just send the character
      */
      default:
         //_dev_slip_write_c(desc_link,*buf,0);
         __dev_slip_write_c(desc_link,desc,p_slip_write,*buf,0);
         break;
      }
      buf++;
   }

   /* tell the receiver that we're done sending the packet
    */
   //_dev_slip_write_c(desc_link,END,1);
   __dev_slip_write_c(desc_link,desc,p_slip_write,END,1);

   //
   __unlock_io(ofile_lst[desc_link].owner_pthread_ptr_write,desc_link,O_WRONLY);
   return (size);
}

/*-------------------------------------------
| Name:dev_slip_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_slip_seek(desc_t desc,int offset,int origin){
   return -1;
}

/*-------------------------------------------
| Name:dev_slip_ioctl
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_slip_ioctl(desc_t desc,int request,va_list ap){

   switch(request) {

   case I_LINK: {
      desc_t desc_link = ofile_lst[desc].desc_nxt[0];
      char c=0;
      if(desc_link<0)
         return 0;    //not flush buffer

      //flush input buffer
      //while(ofile_lst[desc_link].pfsop->fdev.fdev_read(desc,&c,1)>0);
      return 0;
   }
   break;

   case I_UNLINK: {
   }
   break;

   //
   default:
      return -1;

   }
   return -1;
}


/*============================================
| End of Source  : dev_slip.c
==============================================*/
