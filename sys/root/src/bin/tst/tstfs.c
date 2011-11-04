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

/*!
   \option
      -d number of directories
      -f number of files
      -s file size
      -r test repetition 
      
       last parameter path to mount point of file system
      
   \brief
      stress test for file system.
      
   \use
      tstfs -d 5 -f 2 -s 200 -r 8 /etc
      in /etc path create 5 directories with 2 files of 200 bytes in each one of them.
      this test is executed 8 times
*/


/*===========================================
Includes
=============================================*/
#include <stdlib.h>

#include "kernel/libstd.h"
#include "kernel/stat.h"
#include "kernel/statvfs.h"
#include "kernel/devio.h"
#include "kernel/time.h"

#include "stdio/stdio.h"

/*===========================================
Global Declaration
=============================================*/
#define OPT_MSK_D 0x01
#define OPT_MSK_F 0x02
#define OPT_MSK_S 0x04
#define OPT_MSK_R 0x08

#define __FREE_BLOCK "free block"
#define __FREE_SIZE  "free size(bytes)"


/*===========================================
Implementation
=============================================*/

/*-------------------------------------------
| Name:test
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int test(char* path, int dir_max,int file_max,int file_sz){

   int i;
   int j;
   struct statvfs  stat;
   char buf[32+1]={0};
      
   if(dir_max>255)
      return 0;
   if(file_max>255)
      return 0;

   //
   if(chdir(path)<0)
      return 0;

   //
   if(statvfs(".",&stat)<0)
         return 0;

   printf("%10s\t\t%16s\r\n",__FREE_BLOCK,__FREE_SIZE);
   printf("%10d\t\t%16d\r\n",stat.f_bfree,stat.f_bfree*stat.f_bsize);

   //create 
   printf("*** create ***\r\n");
   for(i=0;i<dir_max;i++){
      int sz=0;
      char n=i;
      char dname[5]={0};
      dname[0] = 'd';
      dname[1] = n/100+'0';
      n=n%100;
      dname[2] = n/10+'0';
      dname[3] = n%10+'0';
      dname[4] ='\0';
      printf("mkdir %s ...",dname);
      if(mkdir(dname,0)<0)
         return 0;
      printf("ok\r\n");
      //
      printf("chdir %s ...",dname);
      if(chdir(dname)<0)
         return 0;
      printf("ok\r\n");
      //
      for(j=0;j<file_max;j++){
         int fd;
         char fname[5]={0};
         n=j;
         fname[0] = 'f';
         fname[1] = n/100+'0';
         n=n%100;
         fname[2] = n/10+'0';
         fname[3] = n%10+'0';
         fname[4] ='\0';
         //
         printf("create %s ...",fname);
         if( (fd= open(fname,O_CREAT|O_WRONLY,0))<0)
            return 0;
         printf("ok\r\n");

         //
         printf("write in %s ...",fname);
         sz=0;
         while(sz<file_sz){
            int cb = sz%19;
            int _cb;
            buf[cb]='0'+ (sz%10);
            if(!(++sz%19)|| sz==file_sz)
               if( (_cb=write(fd,buf,cb+1))!=(cb+1) )
                  break;
         }
       
         printf("ok\r\n");
         //
         printf("close %s ...",fname);
         close(fd);
         printf("ok\r\n");
      }

      //
      if(chdir("..")<0)
         return 0;

      //
      if(statvfs(".",&stat)<0)
         return 0;

      printf("%10s\t\t%16s\r\n",__FREE_BLOCK,__FREE_SIZE);
      printf("%10d\t\t%16d\r\n",stat.f_bfree,stat.f_bfree*stat.f_bsize);
   }

   //check
   printf("*** check ***\r\n");
   for(i=0;i<dir_max;i++){
      int sz=0;
      char n=i;
      char dname[5]={0};
      dname[0] = 'd';
      dname[1] = n/100+'0';
      n=n%100;
      dname[2] = n/10+'0';
      dname[3] = n%10+'0';
      dname[4] ='\0';
      //
      printf("chdir %s ...",dname);
      if(chdir(dname)<0)
         return 0;
      printf("ok\r\n");
      //
      for(j=0;j<file_max;j++){
         int fd;
         int cb;
         char fname[5]={0};
         n=j;
         fname[0] = 'f';
         fname[1] = n/100+'0';
         n=n%100;
         fname[2] = n/10+'0';
         fname[3] = n%10+'0';
         fname[4] ='\0';
         //
         printf("open %s ...",fname);
         if( (fd= open(fname,O_RDONLY,0))<0)
            return 0;
         printf("ok\r\n");

         //
         printf("check ...");
         sz=0;
         while( (cb= read(fd,buf,19))>0){
            int k;
            for(k=0;k<cb;k++){
               char b = ('0'+ ((sz+k)%10)) ;
               if( buf[k]!=b)
                  return 0;
            }
            sz+=cb;
         }
         if(sz!=file_sz)
            return 0;
         printf("ok\r\n");
         //
         printf("close %s ...",fname);
         close(fd);
         printf("ok\r\n");

         //
         
         sz=file_sz/2;
         printf("truncate %s ...",fname);
         truncate(fname,(off_t)sz);
         printf("ok\r\n");

         //
         //
         printf("open %s ...",fname);
         if( (fd= open(fname,O_RDONLY,0))<0)
            return 0;
         printf("ok\r\n");

         sz = 0;
         //
         
         printf("check truncate...");
         while( (cb= read(fd,buf,19))>0){
            int k;
            for(k=0;k<cb;k++){
               char b = ('0'+ ((sz+k)%10)) ;
               printf("buf[]=%c == %c?\r\n",buf[k],b);
               if( buf[k]!=b)
                  return 0;
            }
            sz+=cb;
         }
         if(sz!=(file_sz/2))
            return 0;
         printf("ok\r\n");
         //
         printf("close %s ...",fname);
         close(fd);
         printf("ok\r\n");
      }

      //
      if(chdir("..")<0)
         return 0;
   }

   //remove
   printf("*** remove ***\r\n");
   for(i=0;i<dir_max;i++){
      int sz=0;
      char n=i;
      char dname[5]={0};
      dname[0] = 'd';
      dname[1] = n/100+'0';
      n=n%100;
      dname[2] = n/10+'0';
      dname[3] = n%10+'0';
      dname[4] ='\0';
      //
      printf("chdir %s ...",dname);
      if(chdir(dname)<0)
         return 0;
      printf("ok\r\n");
      //
      for(j=0;j<file_max;j++){
         char fname[5]={0};
         n=j;
         fname[0] = 'f';
         fname[1] = n/100+'0';
         n=n%100;
         fname[2] = n/10+'0';
         fname[3] = n%10+'0';
         fname[4] ='\0';
         //
         printf("rm %s ...",fname);
         if( remove(fname)<0)
            return 0;
         printf("ok\r\n");

         if(statvfs(".",&stat)<0)
            return 0;

         printf("%10s\t\t%16s\r\n",__FREE_BLOCK,__FREE_SIZE);
         printf("%10d\t\t%16d\r\n",stat.f_bfree,stat.f_bfree*stat.f_bsize);
      }
      //
      if(statvfs(".",&stat)<0)
         return 0;

      printf("%10s\t\t%16s\r\n",__FREE_BLOCK,__FREE_SIZE);
      printf("%10d\t\t%16d\r\n",stat.f_bfree,stat.f_bfree*stat.f_bsize);

      //
      if(chdir("..")<0)
         return 0;

      //rmdir
      printf("rmdir %s ...",dname);
      if(rmdir(dname)<0)
         return 0;
      printf("ok\r\n");

      //
      if(statvfs(".",&stat)<0)
         return 0;

      printf("%10s\t\t%16s\r\n",__FREE_BLOCK,__FREE_SIZE);
      printf("%10d\t\t%16d\r\n",stat.f_bfree,stat.f_bfree*stat.f_bsize);
   }

   return 0;
}

/*-------------------------------------------
| Name:tstlcd_main
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int tstfs_main(int argc,char* argv[]){
   int i;
   unsigned int opt=0;
   int dir_max  = 0;
   int file_max = 0;
   int file_sz  = 0;
   int r = 1;
   char* path=(char*)0;
   
   //get option
   for(i=1;i<argc;i++){
      if(argv[i][0]=='-'){
         unsigned char c;
         unsigned char l=strlen(argv[i]);
         for(c=1;c<l;c++){
            switch(argv[i][c]){
               case 'd'://-d number of directory
                  opt |= OPT_MSK_D;
                  if(argv[i][c+1])
                     break;
                  if(argv[i+1][0]=='-')
                     break;

                  i++;
                  if(!argv[i])
                     break;
                  dir_max = atoi(argv[i]);
               break;

               case 'f'://-f number of file
                  opt |= OPT_MSK_F;
                  if(argv[i][c+1])
                     break;
                  if(argv[i+1][0]=='-')
                     break;

                  i++;
                  if(!argv[i])
                     break;
                  file_max = atoi(argv[i]);
               break;

               case 's'://-s file size
                  opt |= OPT_MSK_S;
                  if(argv[i][c+1])
                     break;
                  if(argv[i+1][0]=='-')
                     break;

                  i++;
                  if(!argv[i])
                     break;
                  file_sz = atoi(argv[i]);
               break;

               case 'r'://-r test repetition
                  opt |= OPT_MSK_R;
                  if(argv[i][c+1])
                     break;
                  if(argv[i+1][0]=='-')
                     break;

                  i++;
                  if(!argv[i])
                     break;
                  r = atoi(argv[i]);
               break;

            }
         }
      }else{
         if(!argv[i])
            return 0;
         path = argv[i];
      }
   }

   if(!path) 
      return 0;

   while(r--){
      printf("**** test %d ****\r\n",r+1);
      if(test(path,dir_max,file_max,file_sz)<0){
         fprintf(stderr,"error: tstfs failed\r\n");
         return 0;
      }

   }

   return 0;
}

/*===========================================
End of Source tstfs.c
=============================================*/
