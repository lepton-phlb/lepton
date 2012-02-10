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

#include "kernel/libstd.h"
#include "kernel/devio.h"
#include "kernel/time.h"
#include "kernel/wait.h"
#include "kernel/fcntl.h"
#include "kernel/select.h"

#include "stdio/stdio.h"


/*===========================================
Global Declaration
=============================================*/
#define  OPT_MSK_X 0x01 //-x export keyboard

#define  __BUF_MAX 10

/*===========================================
Implementation
=============================================*/
/*-------------------------------------------
| Name:tstkb_main
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int tstkb_main(int argc,char* argv[]){
   int fd;
   int oflag;
   unsigned char buf[__BUF_MAX];
   int i;
   unsigned int opt=0;
   fd_set readfs;
   struct timeval timeout;

   timeout.tv_sec = 0;
   timeout.tv_usec = 10000;     //10ms

   //get option
   for(i=1; i<argc; i++) {
      if(argv[i][0]=='-') {
         unsigned char c;
         unsigned char l=strlen(argv[i]);
         for(c=1; c<l; c++) {
            switch(argv[i][c]) {
            case 'x':
               opt |= OPT_MSK_X;
               break;
            }
         }
      }
   }

   //open dev
   fd=open("/dev/kb0",O_RDONLY|O_NONBLOCK,0);
   if(fd<0) {
      printf("cannot open /dev/kb0\r\n");
      return -1;
   }

   //set oflag|O_NONBLOCK on stdin
   oflag = fcntl(STDIN_FILENO,F_GETFL);
   fcntl(STDIN_FILENO,F_SETFL,oflag|O_NONBLOCK);


   //read current position of rotary  switch
   read(fd,buf,5);
   if(!(opt&OPT_MSK_X) && buf[0]) {
      printf("rotary position:0x%x (%d)\r\n",buf[0],buf[0]);
   }

   //async
   FD_ZERO(&readfs);

   for(;; ) {
      FD_SET(STDIN_FILENO,&readfs); //stdin
      FD_SET(fd,&readfs); //keyb

      switch( select(fd+1,&readfs,0,0,&timeout) ) {
      case 0:
         //timeout
         break;

      default:
         //
         if(FD_ISSET(STDIN_FILENO,&readfs)) {   //stdin
            if(getchar()=='\x18') {   //ctrl-x:exit
               //restore old oflag
               fcntl(STDIN_FILENO,F_SETFL,oflag);
               exit(0);
            }

         }
         //
         if(FD_ISSET(fd,&readfs)) {  //keyb
            int i;
            int cb;
            cb = read(fd,buf,5);
            for(i=0; i<cb; i++) {
               if((opt&OPT_MSK_X))
                  printf("%c",buf[i]);
               else
                  printf("kb:0x%x (%d)\r\n",buf[i],buf[i]);
            }
         }

         break;
      }

   }

   close(fd);

   return 0;
}

/*===========================================
End of Sourcetstkb.c
=============================================*/
