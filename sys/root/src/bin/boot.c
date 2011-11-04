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
#include "kernel/ioctl_cpu.h"
#include "kernel/fcntl.h"
#include "kernel/stropts.h"

#include "stdio/stdio.h"

#include "libc/misc/crc.h"

/*============================================
| Global Declaration 
==============================================*/
#define OPT_MSK_A 0x01   //-a at address 0xyyyyyyyy
#define OPT_MSK_V 0x02   //-v verbose
#define OPT_MSK_E 0x04   //-e erase firmware
#define OPT_MSK_R 0x08   //-r read firmware
#define OPT_MSK_W 0x10   //-w write firmware
#define OPT_MSK_F 0x20   //-f firmware data



typedef struct firmware_tag_st{
   uint32_t offset;
   uint32_t sz;
   crc16_t  crc;
   uint16_t sig;
}firmware_tag_t;

#define FIRMWARE_SIG 0xABCD
#define DATA_SIG     0xDCBA

/*============================================
| Implementation 
==============================================*/
/*--------------------------------------------
| Name:        boot_main
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int boot_main(int argc,char* argv[]){

   int fd=-1;
   int l=0;
   int r=0;
   int w=0;
   int cb=0;
   int verbose=0;
   unsigned long dev_sz=0;
   unsigned long o = 0x00000000;
   unsigned char buf[1024]={0};

   int i;
   unsigned int opt=0;
   char* dev=(char*)0;

   for(i=1;i<argc;i++){
      if(argv[i][0]=='-'){
         unsigned char c;
         unsigned char l=strlen(argv[i]);
         for(c=1;c<l;c++){
            switch(argv[i][c]){
              
               //
               case 'e':{//erase all
                  opt |= OPT_MSK_E;
               }
               break;
               //
               case 'a':{
                  opt |= OPT_MSK_A;
                  if((i+1) == argc){//not enough parameter
                     printf("error: //not enough parameter\r\n");
                     return 0;
                  }
                  i++;
                  if(!argv[i]){
                     o=0x00000000;
                     break;
                  }
                  sscanf( argv[i], "0x%x", &o);
               }
               break;

               //
               case 'r':{
                  //read firmware
                  opt |= OPT_MSK_R;
               }
               break;

               //
               case 'w':{
                  //write firmware
                  opt |= OPT_MSK_W;
               }
               break;

               //
               case 'f':{
                  //firmware data
                  opt |= OPT_MSK_F;
               }
               break;

               //
               case 'v':{
                  opt |= OPT_MSK_V;
                  if((i+1) == argc){//not enough parameter
                     printf("error: //not enough parameter\r\n");
                     return 0;
                  }
                  i++;
                  if(!argv[i]){
                     verbose=1;
                     break;
                  }

                  verbose=atoi(argv[i]);
               }
               break;
            }//switch
         }//for
      }else{
         
         if(!argv[i]){
            printf("error: device not defined\r\n");
            return 0;
         }

         dev = argv[i];
      }

   }//for
   
   //
   if(dev){
      fd = open(dev,O_RDWR,0);
      if(fd<0){
         printf("error: cannot open device:%s\r\n",dev);
         return 0;
      }


      r=ioctl(fd,HDGETSZ,&dev_sz);
      if(r<0){
         printf("error: cannot get size of device\r\n");
         return 0;
      }


      if( (opt&OPT_MSK_E) ){
         printf("erase device %s %lu bytes ....\r\n", dev, dev_sz);
         r=ioctl(fd,HDCLRDSK);
         if(r<0){
            printf("error: cannot erase device\r\n");
            return 0;
         }
         printf("ok\r\n");

         return 0;
      }

      
      if(opt&OPT_MSK_R){
         //for stream mode on stdin for read option
         close(0);
         dup(fd);
         close(fd);
      }else if(opt&OPT_MSK_W){
         //for stream mode on stdout for read option
         close(1);
         dup(fd);
         close(fd);
      }
      
   }


   //stream mode
   //write firmware
   if(opt&OPT_MSK_W){
      crc16_t crc;
      firmware_tag_t firmware_tag;
      int bufsz=sizeof(buf);

      //
      r=0;
      cb=0;
      //
      __init_crc16(crc);
      
      //firmware tag
      if(opt&OPT_MSK_F)
         firmware_tag.sig=FIRMWARE_SIG;
      else
         firmware_tag.sig=DATA_SIG;

      firmware_tag.offset=o;
      firmware_tag.sz=0;
      firmware_tag.crc=crc;

      //
      lseek(STDOUT_FILENO,o,SEEK_SET);
      do{
         r=0;
         cb=0;
         
         //read stream data
         while((cb+=r)<sizeof(buf)){
            r=read(STDIN_FILENO,buf+cb,sizeof(buf)-cb);
            //
            if(r<0)
               return 0;//error
            //
            if(!r)
               break;
         }
         //crc calculation
         for(r=0;r<cb;r++)
            crc=crc16(crc,buf[r]);

         //write data in stream
         l=cb;
         w=0;
         cb=0;

         while((cb+=w)<l){
            if((w=write(STDOUT_FILENO,buf+cb,l-cb))<=0)
               return -1;
         }
         //
         firmware_tag.sz+=cb;
      }while(cb>0);

      //set crc tag
      firmware_tag.crc=crc;

      //usleep to wait the termination of xmodem bin and prompt return
      //just for cosmetic printf on console
      usleep(1000000);// sleep 1 s

      //check crc after writing
      __init_crc16(crc);
      //
      fprintf(stderr,"\r\n check crc ...");
      //check firmware crc
      o=firmware_tag.offset;
      o=lseek(STDOUT_FILENO,o,SEEK_SET);

      //crc calculation
      r=0;
      cb=0;
      while((unsigned int)(cb+=r)<firmware_tag.sz){
         if((int)(firmware_tag.sz-cb)<bufsz)
            bufsz=(firmware_tag.sz-cb)%bufsz;

         r=read(STDOUT_FILENO,buf,bufsz);
         if(r<bufsz)
            fprintf(stderr,"error: read device\r\n");
         //crc calculation
         for(i=0;i<r;i++)
            crc=crc16(crc,buf[i]);
      }

      //check crc
      if(crc!=firmware_tag.crc){
         fprintf(stderr,"error: corrupted\r\n");
         return -1;
      }

      //
      fprintf(stderr," ok\r\n");

      //tag firmware
      o=lseek(STDOUT_FILENO,0,SEEK_END);
      o-=sizeof(firmware_tag_t);
      lseek(STDOUT_FILENO,o,SEEK_SET);
      write(STDOUT_FILENO,&firmware_tag,sizeof(firmware_tag));     

      return 0;
   }

   //stream mode
   //read firmware
   if(opt&OPT_MSK_R){
      crc16_t crc;
      firmware_tag_t firmware_tag;
      hdio_t hdio;
      int bufsz=sizeof(buf);

      //
      r=0;
      cb =0;
      //
      __init_crc16(crc);

      //
      if(verbose)
         printf("read data...");

      //
      if(ioctl(STDIN_FILENO,HDIO,&hdio)<0){
         if(verbose)
            printf("error: invalid boot device\r\n");
      }

      //
      o=lseek(STDIN_FILENO,0,SEEK_END);
      o-=sizeof(firmware_tag_t);
      lseek(STDIN_FILENO,o,SEEK_SET);
      if((r=read(STDIN_FILENO,&firmware_tag,sizeof(firmware_tag)))<sizeof(firmware_tag)){
         if(verbose)
            printf("error: read error on stdin\r\n");
         return -1;
      }

      //firmware signature
      if(opt&OPT_MSK_F){
         if(firmware_tag.sig!=FIRMWARE_SIG){
            if(verbose)
               printf(" error: sorry it's not a firmware bye!\r\n");
            return -1;
         }else{
            if(verbose)
               printf(" it's a firmware...\r\n");
         }
      }else{
         if(firmware_tag.sig!=FIRMWARE_SIG && firmware_tag.sig!=DATA_SIG){
            if(verbose)
               printf(" error: not a valid signature\r\n");
            return -1;
         }else if(firmware_tag.sig==FIRMWARE_SIG){
            if(verbose)
               printf(" it's a firmware...\r\n");
         }else if(firmware_tag.sig==DATA_SIG){
            if(verbose)
               printf(" it's a data...\r\n");
         }
      }

      //firmware signature
      if(!(opt&OPT_MSK_F)){
         if(verbose)
            printf("check crc ...");
         //check firmware crc
         o=firmware_tag.offset;
         o=lseek(STDIN_FILENO,o,SEEK_SET);

         //crc calculation
         r=0;
         cb=0;
         while((unsigned int)(cb+=r)<firmware_tag.sz){
            if((int)(firmware_tag.sz-cb)<bufsz)
               bufsz=(firmware_tag.sz-cb)%bufsz;

            r=read(STDIN_FILENO,buf,bufsz);
            if(r<bufsz)
               if(verbose)
                  printf("error: read device\r\n");
            //crc calculation
            for(i=0;i<r;i++)
               crc=crc16(crc,buf[i]);
         }

         //check crc
         if(crc!=firmware_tag.crc){
            if(verbose)
               printf("error: corrupted\r\n");
            return -1;
         }

         if(verbose)
            printf(" ok\r\n");
      }

      if(opt&OPT_MSK_F){
         if((fd=open("/dev/cpu0",O_WRONLY,0))<0){
            if(verbose)
               printf("error: cpu device not loaded\r\n");
         }

         if(ioctl(fd,CPUBOOT,(unsigned long)hdio.addr)<0){
            if(verbose)
               printf("error: cpu cannot boot on device\r\n");
         }
      }

      return 0;
   }

   return 0;

}

/*============================================
| End of Source  : boot.c
==============================================*/
