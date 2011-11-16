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
#include "kernel/system.h"
#include "kernel/libstd.h"
#include "kernel/devio.h"
#include "stdio/stdio.h"

#include "kernel/ioctl_board.h"

/*============================================
| Global Declaration 
==============================================*/


/*============================================
| Implementation 
==============================================*/
int tstsound_main(int argc,char* argv[]){
   int fd;
   int index;
   int f = 800;
   int d = 500;
   int l = 3;
   int n = 1;

   struct board_beep_t board_beep;

   if((fd=open("/dev/board",O_WRONLY,0))<0){
      printf("error: cannot open board dev\r\n");
      return -1;
   }
 
   if(argv[1]!=(char*)0 && argv[1][1]=='h') {
      printf("tstsound: help\r\n");
      printf("tstsound: -f [frequency Hz] -d [duration ms] -l [level 0..4] -n [repetition]\r\n");
   }else{
      //get options
      for(index=1;index<argc;index++){
         if (argv[index][0] == '-') {
            switch (argv[index][1]) {
               case 'f': //frequency
                  if(argc >= index+1) {
                     f = atoi(argv[index+1]);
                  }else{
                     f = 800;
                  }
                  break;
               case 'd'://duration
                  if(argc >= index+1) {
                     d = atoi(argv[index+1]);
                  }else{
                     d = 500;
                  }
                  break;
               case 'l'://level
                  if(argc >= index+1) {
                     l = atoi(argv[index+1]);
                     if(l>BRDBEEP_LVL_MAX)
                        l=BRDBEEP_LVL_MAX;
                  }
                  break;
               case 'n'://repetition
                  if(argc >= index+1) {
                     n = atoi(argv[index+1]);
                  }
                  break;

            }  //fin du switch
         }  //fin du if(argv...
      }  //fin du for
   }

   //
   for(;n>0;n--){
      board_beep.frequency = f;
      board_beep.duration  = d;
      board_beep.level     = l;
      ioctl(fd,BRDBEEP,&board_beep);
   }

   return 0;

}

/*============================================
| End of Source  : tstsound.c
==============================================*/
