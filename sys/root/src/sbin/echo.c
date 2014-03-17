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
#include <stdlib.h>
#include <string.h>

#include "kernel/core/libstd.h"
#include "kernel/core/time.h"
#include "kernel/core/devio.h"

/*============================================
| Global Declaration
==============================================*/


/*============================================
| Implementation
==============================================*/
/*--------------------------------------------
| Name:        echo_main
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int echo_main(int argc,char* argv[]){
   char buf[32];
   int cb;
   int i;

   if(argc<1) {
      while((cb=read(0,buf,sizeof(buf)))) {
         int i;
         for(i=0; i<cb; i++)
            if(buf[i]==0x18) //ctrl-x
               return 0;
         write(1,buf,cb);
      }
   }

   for(i=1; i<argc; i++) {
      int l=strlen(argv[i]);
      cb=0;
      while(l-cb) {
         int w=0;
         if((w=write(1,argv[i]+cb,l-cb))<=0)
            break;
         cb+=w;
      }
   }

   write(1,"\n\r\n",3);

   return 0;
}
/*============================================
| End of Source  : echo.c
==============================================*/
