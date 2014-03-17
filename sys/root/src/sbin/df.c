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


/*===========================================
Includes
=============================================*/
#include "kernel/core/libstd.h"
#include "kernel/core/devio.h"
#include "kernel/core/fcntl.h"
#include "kernel/core/wait.h"
#include "kernel/core/stat.h"
#include "kernel/core/statvfs.h"

#include "lib/libc/stdio/stdio.h"

#include <string.h>
/*===========================================
Global Declaration
=============================================*/
#define  OPT_MSK_I 0x01 //-i inode
#define  OPT_MSK_T 0x02 //-t [file system type]

#define __FREE_BLOCK "free block"
#define __FREE_SIZE  "free size(bytes)"

/*===========================================
Implementation
=============================================*/
int df_main(int argc, char* argv[]){

   int i;
   unsigned int opt=0;
   char* fst=(char*)0;
   char* path=(char*)0;

   //
   for(i=1; i<argc; i++) {
      if(argv[i][0]=='-') {
         unsigned char c;
         unsigned char l=strlen(argv[i]);
         for(c=1; c<l; c++) {
            switch(argv[i][c]) {
            case 'i':
               opt |= OPT_MSK_I;
               break;

            case 't':
               opt |= OPT_MSK_T;
               //get stdin device
               if((i+1) == argc)   //not enough parameter
                  return -1;
               if(argv[i][c+1])
                  break;
               if(argv[i+1][0]=='-')
                  break;
               //
               i++;
               if(!argv[i])
                  return -1;
               fst = argv[i];

               break;

            }
         }
      }else{
         if(!argv[i])
            return -1;

         path = argv[i];
      }
   }

   if(path) {
      struct statvfs stat;

      if(statvfs(path,&stat)<0)
         return -1;

      printf("%10s\t\t%16s\r\n",__FREE_BLOCK,__FREE_SIZE);
      printf("%10d\t\t%16d\r\n",stat.f_bfree,stat.f_bfree*stat.f_bsize);
   } /*else{
      //to do: list all file system mounted.
   }*/

   //

   return 0;
}

/*===========================================
End of Source df.c
=============================================*/
