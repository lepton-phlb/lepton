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
#include "kernel/core/libstd.h"
#include "kernel/core/stat.h"
#include "kernel/core/statvfs.h"
#include "kernel/core/devio.h"
#include "kernel/core/time.h"
#include "kernel/core/dirent.h"

#include "lib/libc/stdio/stdio.h"

#include <string.h>
/*===========================================
Global Declaration
=============================================*/
#define MAX_COLUMN 4

const char dir_dfl[]=".";

#define OPT_MSK_I 0x01   //-i inode
#define OPT_MSK_L 0x02   //-l list
#define OPT_MSK_A 0x04   //-a all

/*===========================================
Implementation
=============================================*/
int ls_main(int argc,char* argv[]){
   DIR* dir;
   struct dirent dirent;
   struct stat _stat;
   int total=0;
   int i,l;
   unsigned int opt=0;
   char* ref = (char*)dir_dfl;
   char path[PATH_MAX];

   struct tm cur_tmb;
   struct tm tmb;

   time_t cur_time = time(&cur_time);

   gmtime_r(&cur_time,&cur_tmb);
   

   //get option
   for(i=1;i<argc;i++){
      if(argv[i][0]=='-'){
         unsigned char c;
         unsigned char l=strlen(argv[i]);
         for(c=1;c<l;c++){
            switch(argv[i][c]){
               case 'i':
                  opt |= OPT_MSK_I;
               break;

               case 'l':
                  opt |= OPT_MSK_L;
               break;

               case 'a':
                  opt |= OPT_MSK_A;
               break;
            }
         }
      }else{
         ref = argv[i];
         printf("ls %s\r\n",ref);
      }
   }

   //
   strcpy(path,ref);
   strcat(path,"/");
   l = strlen(path);
   
   if(!(dir=opendir(ref)))
      return -1;

   while(readdir2(dir,&dirent)){
      
      if(!(opt&OPT_MSK_A) && (dirent.d_name[0]=='.'))
         continue;

      if(opt&OPT_MSK_I)
         printf("%6d ",dirent.inodenb);

      if(opt&OPT_MSK_L){
         char cbuf[26];
         strcpy(&path[l],dirent.d_name);
         stat(path,&_stat);
         if(S_ISDIR(_stat.st_mode))
            printf("d");
         else if(S_ISREG(_stat.st_mode))
            printf("-");
         else if(S_ISFIFO(_stat.st_mode))
            printf("f");
         else if(S_ISBLK(_stat.st_mode))
            printf("b");
         else if(S_ISCHR(_stat.st_mode))
            printf("c");
         else
            printf("?");
         
         printf("rwxrwxrwx %6d ",_stat.st_size);

         if(!_stat.st_ctime){
            ctime_r(&cur_time,cbuf);
            printf(" %.12s ",cbuf+4);
         }else{
            gmtime_r(&_stat.st_ctime,&tmb);

            ctime_r(&_stat.st_ctime,cbuf);
            
            if(tmb.tm_year==cur_tmb.tm_year){
               printf(" %.12s ",cbuf+4);
            }else{
               printf(" %.6s %.4s ",cbuf+4,cbuf+20);
            }
         }

      }
      
      ++total;
      printf("%.32s  ",dirent.d_name);

      /*
      if( (S_ISBLK(_stat.st_mode) || S_ISCHR(_stat.st_mode)) && S_ISLNK(_stat.st_mode) ){
         printf("*");
      }*/


      if(opt&OPT_MSK_L) 
         printf("\r\n");
      else if(!(total%MAX_COLUMN) )
         printf("\r\n");
   }
   printf("\r\n");
   printf("total %d\r\n",total);


   closedir(dir);

   return 0;
}


/*===========================================
End of Sourcels.c
=============================================*/
