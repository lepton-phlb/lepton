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

#include "kernel/signal.h"
#include "kernel/libstd.h"
#include "kernel/devio.h"
#include "kernel/wait.h"
#include "kernel/fcntl.h"
#include "stdio/stdio.h"


#include "pthread/pthread.h"

#include "dev/dev_mem/dev_mem.h"
/*============================================
| Global Declaration 
==============================================*/
#define OPT_MSK_S 0x01   //-s start to send
#define OPT_MSK_V 0x02   //-v verbose


/*============================================
| Implementation 
==============================================*/

/*--------------------------------------------
| Name:        thread1_run
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
static void* thread1_run(void* p){
   int fd=-1;
   int cb;
   //
   struct1_t t_struct1={0};
   struct2_t t_struct2={0};
   struct2_t t_struct3={0};

   fd = open("/dev/mem",O_RDWR,0);

   ioctl(fd,MEMADD,0x0001,sizeof(struct1_t),&t_struct1);
   ioctl(fd,MEMADD,0x0200,sizeof(struct2_t),&t_struct2);
   ioctl(fd,MEMADD,0x0400,sizeof(struct2_t),&t_struct3);


   for(;;){
      usleep(10000000);
      //
      t_struct1.prm0++;
      t_struct1.prm1++;
      t_struct1.prm2++;
  
      printf_r("thread 1 write struct1\r\n");
      lseek(fd,0x0001,SEEK_SET);
      cb = write(fd,&t_struct1,sizeof(t_struct1));


      //
      sprintf(t_struct2.str0,"str%d",t_struct1.prm0);
      sprintf(t_struct2.str1,"str%d",t_struct1.prm1);
      t_struct2.prm2++;

   
      printf_r("thread 2 write struct3\r\n");
      lseek(fd,0x0400,SEEK_SET);
      cb = write(fd,&t_struct2,sizeof(t_struct2));
   

      /*
      printf_r("thread 1 write struct2\r\n");
      lseek(fd,0x0200,SEEK_SET);
      cb = write(fd,&t_struct2,sizeof(t_struct2));
      */
      //
   }
      
   
   return (void*)0;
}

/*--------------------------------------------
| Name:        thread2_run
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
static void* thread2_run(void* p){
   int fd=-1;
   int cb;
   //
   struct1_t t_struct1={0};
   struct2_t t_struct2={0};
   struct2_t t_struct3={0};

   fd = open("/dev/mem",O_RDWR,0);
   
   ioctl(fd,MEMREG,0x0001);
   ioctl(fd,MEMREG,0x0200);
   ioctl(fd,MEMREG,0x0400);

   
   for(;;){

      lseek(fd,0x0001,SEEK_SET);
      printf_r("thread 2 read struct1...\r\n");
      cb = read(fd,&t_struct1,sizeof(t_struct1));
      if(cb>0){
         printf_r("(2)t_struct1.prm0=%d\r\n", t_struct1.prm0);
         printf_r("(2)t_struct1.prm1=%d\r\n", t_struct1.prm1);
         printf_r("(2)t_struct1.prm2=%f\r\n", t_struct1.prm2);
      }


      printf_r("thread 2 read struct2...\r\n");
      lseek(fd,0x0200,SEEK_SET);
      cb = read(fd,&t_struct2,sizeof(t_struct2));
      if(cb>0){
         printf_r("(2)t_struct2.str0=%s\r\n", t_struct2.str0);
         printf_r("(2)t_struct2.str1=%s\r\n", t_struct2.str1);
         printf_r("(2)t_struct2.prm2=%f\r\n", t_struct2.prm2);
      }

      printf_r("thread 2 read struct3...\r\n");
      lseek(fd,0x0400,SEEK_SET);
      cb = read(fd,&t_struct3,sizeof(t_struct3));
      if(cb>0){
         printf_r("(3)t_struct3.str0=%s\r\n", t_struct3.str0);
         printf_r("(3)t_struct3.str1=%s\r\n", t_struct3.str1);
         printf_r("(3)t_struct3.prm2=%f\r\n", t_struct3.prm2);
      }
      printf_r("------------------\r\n");

      /*
      printf_r("thread 2 write struct3\r\n");
      lseek(fd,0x0400,SEEK_SET);
      cb = write(fd,&t_struct2,sizeof(t_struct2));
      */
   }

   return (void*)0;
}


/*--------------------------------------------
| Name:        thread3_run
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
static void* thread3_run(void* p){
   int fd;
   int cb;
   //
   struct1_t t_struct1={0};
   struct2_t t_struct3={0};

   fd = open("/dev/mem",O_RDWR,0);
   
   ioctl(fd,MEMREG,0x0001);
   ioctl(fd,MEMREG,0x0400);

   for(;;){
      lseek(fd,0x0001,SEEK_SET);
      cb = read(fd,&t_struct1,sizeof(t_struct1));
      printf_r("thread 3 read struct1\r\n");
      if(cb>0){
         printf_r("(3)t_struct1.prm0=%d\r\n", t_struct1.prm0);
         printf_r("(3)t_struct1.prm1=%d\r\n", t_struct1.prm1);
         printf_r("(3)t_struct1.prm2=%f\r\n", t_struct1.prm2);
      }

      lseek(fd,0x0400,SEEK_SET);
      cb = read(fd,&t_struct3,sizeof(t_struct3));
      printf_r("thread 3 read struct3\r\n");
      if(cb>0){
         printf_r("(3)t_struct3.str0=%s\r\n", t_struct3.str0);
         printf_r("(3)t_struct3.str1=%s\r\n", t_struct3.str1);
         printf_r("(3)t_struct3.prm2=%f\r\n", t_struct3.prm2);
      }

      printf("------------------\r\n");
   }
   return (void*)0;
}


/*--------------------------------------------
| Name:        tstsynd_main
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int tstsynd_main(int argc,char* argv[]){
   int i;
   int verbose=0;
   unsigned int opt=0;
   char* dev=(char*)0;
   int cb=0;
   
   pthread_t my_pthread1;
   pthread_t my_pthread2;
   //pthread_t my_pthread3;
   pthread_attr_t thread_attr;


   for(i=1;i<argc;i++){
      if(argv[i][0]=='-'){
         unsigned char c;
         unsigned char l=strlen(argv[i]);
         for(c=1;c<l;c++){
            switch(argv[i][c]){

               //
               case 's':{
                  opt |= OPT_MSK_S;
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
               //
               default:
               break;
            }//switch
         }//for
      }else{
      }

   }//for


   
   //
   thread_attr.stacksize = 512;
   thread_attr.stackaddr = NULL;
   thread_attr.priority  = 100;
   thread_attr.timeslice = 1;

   //
   if(opt&OPT_MSK_S){
      pthread_create(&my_pthread1, &thread_attr,thread1_run,NULL);
      printf("thread 1 %ul\r\n",my_pthread1);
   }

   pthread_create(&my_pthread2, &thread_attr,thread2_run,NULL);
   printf("thread 2 %ul\r\n",my_pthread2);

   /*pthread_create(&my_pthread3, &thread_attr,thread3_run,NULL);
   printf("thread 3 %ul\r\n",my_pthread3);*/
   
   
   for(;;){
      //printf("main %u\r\n",pthread_self());
      usleep(1000000);
   }
   return 0;
}

/*============================================
| End of Source  : tstsynd.c
==============================================*/
