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
int mv_main(int argc,char* argv[]){

   int i,cb=0;
   unsigned int opt=0;
   char* src=(char*)0; //source file
   char* dst=(char*)0; //destination filename

   int fd_src=-1;
   int fd_dst=-1;


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

   rename(src,dst);

   return 0;
}

/*===========================================
End of Sourcemv.c
=============================================*/
