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

/*!
   \option
      -i interactive: ask user before crushing existing regular files.
      -f force: remove destination file if already exist.
      -v verbose.

   \brief
      copy file from directory to another directory or file.
      only regular file or directory is supported.
      if -i or -f is not set and destination file already exist, cp return an error.

   \use
      cp /dir1/f1 /dir2
      copy file 'f1' from directory 'dir1' to file 'f1' in 'dir2' directory.

      cp /dir1/f1 /dir2/f2
      copy file 'f1' from directory 'dir1' to file 'f2' in 'dir2' directory.
*/

/*===========================================
Includes
=============================================*/
#include <stdlib.h>
#include "kernel/core/libstd.h"
#include "kernel/core/stat.h"
#include "kernel/core/devio.h"
#include "kernel/core/time.h"
#include "kernel/core/fcntl.h"

#include "lib/libc/stdio/stdio.h"

/*===========================================
Global Declaration
=============================================*/

#define BUF_SZ 64

#define OPT_MSK_I 0x01   //-i interactive: ask user before crushing existing regular files.
#define OPT_MSK_F 0x02   //-f force: remove destination file if already exist.
#define OPT_MSK_V 0x04   //-v verbose

/*===========================================
Implementation
=============================================*/

/*-------------------------------------------
| Name:buildname
| Description:
| Parameters:
| Return Type:
| Comments: Build a path name from the specified directory name and file name.
|           If the directory name is NULL, then the original filename is returned.
|           The built path is in a static area, and is overwritten for each call.
| See:
---------------------------------------------*/
char *buildname(char* buf,char *dirname,char *filename){
   char  *cp;
   if ((dirname == NULL) || (*dirname == '\0'))
      return filename;
   cp = strrchr(filename, '/');
   if (cp)
      filename = cp + 1;

   strcpy(buf, dirname);
   strcat(buf, "/");
   strcat(buf, filename);

   return buf;
}

/*-------------------------------------------
| Name:cp_main
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int cp_main(int argc,char* argv[]){

   int i,cb;
   unsigned int opt=0;
   char* src=(char*)0; //source file
   char* dst=(char*)0; //destination filename

   int fd_src=-1;
   int fd_dst=-1;

   char buf[BUF_SZ];
   char dest_path[PATH_MAX];

   struct stat _stat;

   //get option
   for(i=1; i<argc; i++) {
      if(argv[i][0]=='-') {
         unsigned char c;
         unsigned char l=strlen(argv[i]);
         for(c=1; c<l; c++) {
            switch(argv[i][c]) {
            case 'i':
               opt = OPT_MSK_I;
               break;

            case 'f':
               opt = OPT_MSK_F;
               break;

            case 'v':
               opt |= OPT_MSK_V;
               break;
            }
         }
      }else{
         if(!argv[i])
            return -1;

         if(!src) {
            src= argv[i];
            continue;
         }

         if(!dst) {
            dst= argv[i];
            break;
         }

      }
   }

   //
   if(!src|| !dst) {
      fprintf(stderr,"error:invalid argument\r\n");
      return -1;
   }

   //source file
   if((fd_src=open(src,O_RDONLY,0))<0) {
      fprintf(stderr,"error:cannot open source file\r\n");
      return -1;
   }
   if(fstat(fd_src,&_stat)<0) {
      fprintf(stderr,"error:fstat source file\r\n");
      return -1;
   }
   if(!(S_ISREG(_stat.st_mode)) && !(S_ISBLK(_stat.st_mode))) {
      fprintf(stderr,"error:source file is not valid file type\r\n");
      return -1;
   }

   //destination file
   if(!stat(dst,&_stat)) {
      if((S_ISDIR(_stat.st_mode))) {
         //build dest
         buildname(dest_path,dst,src);
      }else if(!(S_ISREG(_stat.st_mode)) && !(S_ISBLK(_stat.st_mode))) {
         fprintf(stderr,"error:source file is not valid file type\r\n");
         return -1;
      }else{ //regular file
         strcpy(dest_path,dst);
      }

      //copy file
      if((fd_dst=open(dest_path,O_WRONLY,0))>=0) {
         close(fd_dst);
         //file already exist
         if(!(opt&OPT_MSK_I) && !(opt&OPT_MSK_F)) {
            fprintf(stderr,"error:destination file already exist\r\n");
            return -1;
         }

         //
         if((opt&OPT_MSK_I)) {
            //to do ask question
         }
      }

   }else{
      strcpy(dest_path,dst);
   }

   //copy file
   if((fd_dst=open(dest_path,O_CREAT|O_TRUNC|O_WRONLY,0))<0) {
      fprintf(stderr,"error:cannot create destination file\r\n",dest_path);
      return -1;
   }

   while((cb=read(fd_src,buf,sizeof(buf)))>0) {
      if(write(fd_dst,buf,cb)<cb) {
         fprintf(stderr,"error:cannot write destination file %s\r\n",dest_path);
         return -1;
      }
   }

   if(opt&OPT_MSK_V)
      printf("file %s is copyed into %s\r\n",src,dest_path);

   return 0;
}

/*===========================================
End of Source cp.c
=============================================*/
