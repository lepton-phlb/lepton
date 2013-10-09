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
#include <string.h>

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
typedef struct {
   int sig_num;
   const char* signame;
}kill_cmd;

kill_cmd kill_cmd_lst[NSIG]={
   {0,""},
   { SIGABRT,"ABRT"},   //1   //ii  Process abort signal.
   { SIGALRM,"ALRM"},   //2   //i  Alarm clock.
   { SIGFPE,"FPE"},     //3   //ii  Erroneous arithmetic operation.  NOT IMPLEMENTED IN THIS VERSION
   { SIGHUP,"HUP"},     //4   //i  Hangup.
   { SIGILL,"ILL"},     //5   //ii  Illegal instruction.             NOT IMPLEMENTED IN THIS VERSION
   { SIGINT,"INT"},     //6   //i  Terminal interrupt signal.
   { SIGQUIT,"QUIT"},   //7   //ii  Terminal quit signal.
   { SIGPIPE,"PIPE"},   //8   //i  Write on a pipe with no one to read it.
   { SIGKILL,"KILL"},   //9   //i  Kill (cannot be caught or ignored).
   { SIGSEGV,"SEGV"},   //10  //ii  Invalid memory reference.
   { SIGTERM,"TERM"},   //11  //i  Termination signal.
   { SIGUSR1,"USR1"},   //12  //i  User-defined signal 1.
   { SIGUSR2,"USR2"},   //13  //i  User-defined signal 2.
   { SIGCHLD,"CHLD"},   //14  //iii  Child process terminated or stopped.
};

static const int kill_nsig = sizeof(kill_cmd_lst)/sizeof(kill_cmd);

/*===========================================
Implementation
=============================================*/

int kill_main(int argc,char* argv[]){
   int i = 0;
   int sig=0;

   int status=0;

   if(argc<2) //not enough parameter
      return -1;

   for(i=1; i<argc; i++) {
      if(argv[i][0]=='-') {
         unsigned char c;
         unsigned char l=strlen(argv[i]);
         for(c=1; c<l; c++) {
            switch(argv[i][c]) {
            //
            case 'l': {
               int nsig= 0;
               for(nsig=1; nsig<kill_nsig; nsig++) {
                  printf("%2.2d: %.4s  ",nsig, kill_cmd_lst[nsig].signame);
               }
            }
            break;

            //
            case 's': {
               int nsig;
               if((i+1) == argc)   //not enough parameter
                  return -1;
               i++;
               if(!argv[i])
                  return -1;

               for(nsig=1; nsig<kill_nsig; nsig++) {
                  if(strcmp(argv[i],kill_cmd_lst[nsig].signame)) continue;
                  sig = nsig;
                  break;
               }

            }
            break;

            default:
               if(!sig) {
                  sig = atoi(&argv[i][c]);
                  if(!sig)
                     return -1;
               }
               break;

            }
         }
      }else{
         pid_t pid;
         if(!sig) continue;

         if(!argv[i])
            return -1;

         pid = atoi(argv[i]);
         if(!pid) continue;
         //printf("kill sig:%d to pid:%d\r\n",sig,pid);
         kill(pid,sig);
      }

   }

   printf("\r\n");
   return 0;
}


/*===========================================
End of Source kill.c
=============================================*/
