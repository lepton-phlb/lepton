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
#include <stdlib.h>

#include "kernel/libstd.h"
#include "kernel/stat.h"
#include "kernel/statvfs.h"
#include "kernel/devio.h"
#include "kernel/time.h"
#include "kernel/ioctl_hd.h"
#include "kernel/fcntl.h"
#include "kernel/stropts.h"

#include "stdio/stdio.h"


/*============================================
| Global Declaration
==============================================*/
#define OPT_MSK_O 0x01   //-o offset
#define OPT_MSK_V 0x02   //-v verbose
#define OPT_MSK_E 0x04   //-e erase all
#define OPT_MSK_F 0x08   //-f use ftl


/*static const unsigned short memory_pattern[13] = {  0xF1E2, 0xD3C4, 0xB5A6, 0x9788, 0x796A, 0x5B4C,
                                       0x3D21, 0xF2E3, 0xD4C5, 0xB6A7, 0x9889, 0x7A6B,
                                       0x6C4D};
                                       */

/*static const unsigned short memory_pattern[13] = {  0x0100, 0x0302, 0x0504, 0x0706, 0x0908, 0x0100,
                                                    0x0302, 0x0504, 0x0706, 0x0908, 0x0100, 0x0302,
                                                    0x0504};*/

static const unsigned short memory_pattern[6] = {  0x3130, 0x3332, 0x3534, 0x3736, 0x3938,0x3a3a};
//static const unsigned short memory_pattern[6] = {  0x3a3a, 0x3938, 0x3736, 0x3534, 0x3332, 0x3130};



/*============================================
| Implementation
==============================================*/

/*--------------------------------------------
| Name:        tstfs_main
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int tsthd_main(int argc,char* argv[]){
   int fd;
   int fd_ftl;
   int r;
   int cb=0;
   int verbose=0;
   unsigned long dev_sz;
   unsigned long o = 0x00000000;

   unsigned short memory_pattern_read[13]={0};

   int i;
   unsigned int opt=0;
   char* dev=(char*)0;

   int oflag;

   for(i=1; i<argc; i++) {
      if(argv[i][0]=='-') {
         unsigned char c;
         unsigned char l=strlen(argv[i]);
         for(c=1; c<l; c++) {
            switch(argv[i][c]) {

            case 'e': {  //erase all
               opt |= OPT_MSK_E;
            }
            break;

            case 'f': {  //use ftl
               opt |= OPT_MSK_F;
            }
            break;

            //
            case 'o': {
               opt |= OPT_MSK_O;
               if((i+1) == argc) {  //not enough parameter
                  printf("error: //not enough parameter\r\n");
                  return 0;
               }
               i++;
               if(!argv[i]) {
                  o=0x00000000;
                  break;
               }
               sscanf( argv[i], "0x%x", &o);
            }
            break;

            //
            case 'v': {
               opt |= OPT_MSK_V;
               if((i+1) == argc) {  //not enough parameter
                  printf("error: //not enough parameter\r\n");
                  return 0;
               }
               i++;
               if(!argv[i]) {
                  verbose=1;
                  break;
               }

               verbose=atoi(argv[i]);
            }
            break;
            } //switch
         } //for
      }else{

         if(!argv[i]) {
            printf("error: device not defined\r\n");
            return 0;
         }

         dev = argv[i];
      }

   } //for

   if(!dev) {
      printf("error: device not defined\r\n");
      return 0;
   }

   fd = open(dev,O_RDWR,0);
   if(fd<0) {
      printf("error: cannot open device\r\n");
      return 0;
   }


   r=ioctl(fd,HDGETSZ,&dev_sz);
   if(r<0) {
      printf("error: cannot get size of device\r\n");
      return 0;
   }


   if( (opt&OPT_MSK_E) ) {
      printf("erase device %s %lu bytes ....\r\n", dev, dev_sz);
      r=ioctl(fd,HDCLRDSK);
      if(r<0) {
         printf("error: cannot erase device\r\n");
         return 0;
      }
      printf("ok\r\n");
   }

   /*
   if( (opt&OPT_MSK_E) )
      return 0;
   */

   //use ftl
   if( (opt&OPT_MSK_F) ) {
      if((fd_ftl = open("/dev/ftl",O_RDWR,0))<0) {
         printf("error: cannot open ftl device\r\n");
         return 0;
      }

      ioctl(fd_ftl,I_LINK,fd);

      fd = fd_ftl;
      fattach(fd,"/dev/hd/hdd0");
   }

   //set oflag|O_NONBLOCK on stdin
   oflag = fcntl(STDIN_FILENO,F_GETFL);
   fcntl(STDIN_FILENO,F_SETFL,oflag|O_NONBLOCK);
/*
   //1)
   o=0x00000000+5;//sector 2 cluster 0
   lseek(fd,o,SEEK_SET);
   memset(memory_pattern_read,0x00,sizeof(memory_pattern_read));
   if(cb=write(fd,memory_pattern_read,sizeof(memory_pattern))<=0){
      printf("error: cannot write on device at 0x%08x\r\n", o);
      fcntl(STDIN_FILENO,F_SETFL,oflag);
      return 0;
   }

   //read pattern
   memset(memory_pattern_read,0,sizeof(memory_pattern_read));
   if(verbose==2)
      printf("r");

   lseek(fd,o,SEEK_SET);
   if(cb=read(fd,memory_pattern_read,sizeof(memory_pattern_read))<=0){
      printf("error: cannot read on device at 0x%08x\r\n", o);
      fcntl(STDIN_FILENO,F_SETFL,oflag);
      return 0;
   }


   //2)
   o=0x00000000+5;//sector 2 cluster 0

   lseek(fd,o,SEEK_SET);
   if(cb=write(fd,memory_pattern,sizeof(memory_pattern)/2)<=0){
      printf("error: cannot write on device at 0x%08x\r\n", o);
      fcntl(STDIN_FILENO,F_SETFL,oflag);
      return 0;
   }


   //read pattern
   memset(memory_pattern_read,0,sizeof(memory_pattern_read));
   if(verbose==2)
      printf("r");

   lseek(fd,o,SEEK_SET);
   if(cb=read(fd,memory_pattern_read,sizeof(memory_pattern_read))<=0){
      printf("error: cannot read on device at 0x%08x\r\n", o);
      fcntl(STDIN_FILENO,F_SETFL,oflag);
      return 0;
   }
*/
/*

   for(;o<(dev_sz/2);o+=sizeof(memory_pattern)){
      char c;

      //
      while((cb=read(0,&c,1)>0) && c!=0x18);

      if(c==0x18){
         fcntl(STDIN_FILENO,F_SETFL,oflag);
         return 0;
      }

      //
      if((verbose==1 && !(o%0x100)) || verbose==2)
         printf("check at 0x%08x ... ",o);

      //write pattern
      if(verbose==2)
         printf("w");

      lseek(fd,o,SEEK_SET);
      if(cb=write(fd,memory_pattern,sizeof(memory_pattern))<=0){
         printf("error: cannot write on device at 0x%08x\r\n", o);
         fcntl(STDIN_FILENO,F_SETFL,oflag);
         return 0;
      }

      //read pattern
      memset(memory_pattern_read,0,sizeof(memory_pattern_read));
      if(verbose==2)
         printf("r");

      lseek(fd,o,SEEK_SET);
      if(cb=read(fd,memory_pattern_read,sizeof(memory_pattern_read))<=0){
         printf("error: cannot read on device at 0x%08x\r\n", o);
         fcntl(STDIN_FILENO,F_SETFL,oflag);
         return 0;
      }

      //check pattern
      if(verbose==2)
         printf("c");

      if(memcmp(memory_pattern_read,memory_pattern,cb)){
         printf("error: check failed on device at 0x%08x\r\n", o);
         fcntl(STDIN_FILENO,F_SETFL,oflag);
         return 0;
      }
      if((verbose==1 && !(o%0x100)) || verbose==2)
         printf("ok\r\n");
   }

*/
   fcntl(STDIN_FILENO,F_SETFL,oflag);

   return 0;
}
/*============================================
| End of Source  : tsthd.c
==============================================*/
