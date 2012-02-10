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
#include "kernel/core/signal.h"
#include "kernel/core/libstd.h"
#include "kernel/core/stat.h"
#include "kernel/core/statvfs.h"
#include "kernel/core/devio.h"
#include "kernel/core/time.h"
#include "kernel/core/wait.h"
#include "kernel/core/fcntl.h"

#include "lib/libc/stdio/stdio.h"

/*===========================================
Global Declaration
=============================================*/

#define __MAX_BUF 64


/*===========================================
Implementation
=============================================*/

/*-------------------------------------------
| Name:rdstream
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int rdstream(FILE* fstream,char *buf, int len)
{
   int i = 0;
   signed char ch;

   while(1) {

      ch = fgetc(fstream);
      //to do: up down key filter.

      switch(ch) {
      case -1:
         return -1;

      case  8:
         if(i>0) {
            putchar(ch);
            i--;
         }
         break;

      case '\x18': //ctrl-x:return -1
         putchar('\r');
         putchar('\n');
         return -1;
         break;

      case '\0':
         return -1;
         break;

      case '\n':
         break;

      case '\r':
         buf[i] = ch;
         putchar('\r');
         putchar('\n');
         if(++i==len)
            return -1;
         return i;

      default:
         buf[i] = ch;
         putchar(ch);
         if(++i==len)
            return -1;
      }
   }

   return 0;
}

/*-------------------------------------------
| Name:ecat_main
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int ecat_main(int argc, char* argv[]){
   int len=0;
   FILE* fstream;
   char buf [__MAX_BUF];

   if(argc!=2)
      return -1;  // not valid parameter

   printf("type [ctrl][x] to exit.\r\n");

   //open and reading file
   if(!(fstream = fopen(argv[argc-1],"r"))) {
      printf("cannot open file : %s\r\n",argv[argc-1]);
      return -1;
   }
   while(rdstream(fstream,buf,__MAX_BUF)>=0) ;
   fclose(fstream);

   //open file for writing
   if(!(fstream = fopen(argv[argc-1],"a"))) {
      printf("cannot open file : %s\r\n",argv[argc-1]);
      return -1;
   }

   //concat file
   for(;; ) {
      if((len=rdstream(stdin,buf,__MAX_BUF))<0)
         break;
      if(!len) continue;

      fwrite(buf,sizeof(char),len,fstream);
   }

   fclose(fstream);

   return 0;
}

/*===========================================
End of Source ecat.c
=============================================*/
