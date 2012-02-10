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

/*
   -i [numitem]  -v [value] -b [0|1]
   -l [backlight 0|1]
   -a [set/clr all 0|1]
   -r [reset blink 0|1]

   ctrl x (exit)

*/
/*===========================================
Includes
=============================================*/

#include <stdlib.h>
#include "kernel/signal.h"
#include "kernel/libstd.h"
#include "kernel/devio.h"
#include "kernel/fcntl.h"
#include "kernel/wait.h"
#include "kernel/time.h"
#include "kernel/wait.h"
#include "kernel/fcntl.h"
#include "kernel/select.h"

#include "stdio/stdio.h"
#include "misc/prsopt.h"

/*===========================================
Global Declaration
=============================================*/
#define CMD_MAX 64

static const char prompt[]="$>";

/*===========================================
Implementation
=============================================*/

#define  OPT_MSK_I 0x01 //-i input driver command (ex: /dev/ttys1)
#define  OPT_MSK_O 0x02 //-o lcd driver path (ex: /dev/lcdw32)

/*-------------------------------------------
| Name:x_cmd_line
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int x_cmd_line(char *buf, int len){
   int i = 0;
   char ch;
   while(1) {

      ch = getchar();
      //to do: up down key filter.

      switch(ch) {
      case  8:
         if(i>0) {
            i--;
         }
         break;

      case '\x18': //ctrl-x:exit
         exit(0);
         break;

      case '\r':
      case '\n':
         buf[i] = 0;
         return i;

      default:
         buf[i] = ch;
         i++;
      }
   }
}

/*-------------------------------------------
| Name:cmd_line
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int cmd_line(char *buf, int len)
{
   int i = 0;
   signed char ch;

   while(1) {

      if(read(0,&ch,1)<=0)
         ch='\0';

      //ch = getchar();

      //to do: up down key filter.
      if(i==CMD_MAX) {
         buf[i-1]=0;
         return -1;
      }

      switch(ch) {
      //
      case -1:
         buf[i] = 0;
         return i;

      case '\0':
         buf[i] = 0;
         return i;

      case  8:
         if(i>0) {
            write(1,&ch,1);
            i--;
         }
         break;

      case '\x18': //ctrl-x:exit
         write(1,"\r\n",2);
         exit(0);
         break;

      case '\n':
         break;

      case '\r':

         buf[i] = 0;
         write(1,"\r\n",2);
         return i;

      default:
         buf[i] = ch;
         putchar(ch);
         i++;
      }
   }

   return i;
}

/*-------------------------------------------
| Name:tstlcd_main
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int tstlcd_main(int argc,char* argv[]){
   int fd;
   char buf[CMD_MAX+1];
   struct prsopt_t _prs_opt;
   struct opt_t* opt;

   char* __stdlcd=(char*)0;
   char* __stdin=(char*)0;

   int i;
   unsigned int _opt=0;

   int item = 0;
   int item_nb = 0;
   char value[6] = {0};
   char blnk_value[6] = {0};
   int blnk= -1;
   char restart=0;

   //get option
   for(i=1; i<argc; i++) {
      if(argv[i][0]=='-') {
         unsigned char c;
         unsigned char l=strlen(argv[i]);
         for(c=1; c<l; c++) {
            switch(argv[i][c]) {
            case 'i':
               _opt |= OPT_MSK_I;
               if(argv[i][c+1])
                  break;
               if(argv[i+1][0]=='-')
                  break;

               i++;
               if(!argv[i])
                  break;

               __stdin = argv[i];

               break;

            case 'o':
               _opt |= OPT_MSK_O;
               if((i+1) == argc)   //not enough parameter
                  return -1;
               if(argv[i][c+1])
                  break;
               i++;
               if(argv[i])
                  __stdlcd = argv[i];
               break;
            }
         }
      }
   }

   if(!__stdlcd) {
      fprintf(stderr,"error: lcd dev not defined\r\n");
      return -1;
   }

   //
   if(__stdin) {
      close(STDIN_FILENO);
      if((fd=open(__stdin,O_RDONLY,0))<0) {
         fprintf(stderr,"error: cannot open %s\r\n",argv[i]);
         return -1;
      }
   }

   //
   fd=open(__stdlcd,O_WRONLY,0);
   if(fd<0) {
      printf("error: cannot open %s\r\n",__stdlcd);
      return -1;
   }

   //
   for(;; ) {
      long t_timeout=0;

      if(!(_opt&OPT_MSK_I)) {
         //printf(prompt);
         write(1,prompt,sizeof(prompt)-1);
         if(cmd_line(buf,CMD_MAX)<=0)
            return 0;
      }else{
         x_cmd_line(buf,CMD_MAX);
      }

      blnk =0;

      opt= prsopt(&_prs_opt,buf,"aivblrmt!");
      while(opt) {
         //printf("o=%c v=%s\r\n",opt->opt,opt->v);
         switch(opt->opt) {
         case 'i': {  //item
            item = atoi(opt->v);
         }
         break;
         case 'v':   //value
         {
            int i=0;
            item_nb = 0;

            memset(value,-1,sizeof(value));

            while( (opt->v[i]!='\0')
                   && (item_nb<sizeof(value)) ) {

               if(opt->v[i+1]=='.') {
                  value[item_nb]=opt->v[i]-48+10;   //add point segment
                  i++;
               }else if( (opt->v[i]>=48) && (opt->v[i]<=57)) {
                  value[item_nb] = opt->v[i]-48;
               }else if( (opt->v[i]>=65) && (opt->v[i]<=90)) {
                  value[item_nb] = opt->v[i]-46;
               }else if( (opt->v[i]>=97) && (opt->v[i]<=122)) {
                  value[item_nb] = opt->v[i]-77;
               }else if(opt->v[i]=='^') {
                  value[item_nb] = opt->v[i]-48;   //46
               }else if(opt->v[i]=='-') {
                  value[item_nb] = opt->v[i]+2;   //47
               }else if(opt->v[i]=='_') {
                  value[item_nb] = opt->v[i]-47;   //48
               }else if(opt->v[i]=='*') {
                  value[item_nb] = -1;
               }else{
                  value[item_nb] = 0;
               }

               item_nb++;
               i++;
            }
         }
         break;
         case 'b':   //blink
            blnk = atoi(opt->v);
            break;

         case 'a': {  //set/clr all item
            char setclr = 1;

            //1) reset blink
            lseek(fd,(off_t)514,SEEK_SET);
            write(fd,(char*)&setclr,1);

            //2)set or clr all item
            setclr = atoi(opt->v);
            lseek(fd,(off_t)512,SEEK_SET);
            write(fd,(char*)&setclr,1);
         }
         break;
         case 'l': {  //backlight on/off
            char bckl = atoi(opt->v);
            lseek(fd,(off_t)513,SEEK_SET);
            write(fd,(char*)&bckl,1);
         }
         break;

         case 'r': {  //reset blink
            char r_blnk = atoi(opt->v);
            if(!r_blnk)
               break;

            lseek(fd,(off_t)514,SEEK_SET);
            write(fd,(char*)&r_blnk,1);
         }

         case 't': {  //timeout delay
            t_timeout = atol(opt->v)*1000L;   //ms
         }
         break;

         case '!': {  //restart script
            restart=1;
         }
         break;

         }

         //next option
         opt= prsopt(&_prs_opt,0,"ivbt!");
      }

      //
      lseek(fd,(off_t)item,SEEK_SET); //110 win32, 200 m16c
      write(fd,value,item_nb);

      if(blnk>0) {
         memset(blnk_value,1,item_nb);
         lseek(fd,(off_t)(item+256),SEEK_SET); //110 win32, 200 m16c
         write(fd,blnk_value,item_nb);
      }else if(!blnk) {
         memset(blnk_value,0,item_nb);
         lseek(fd,(off_t)(item+256),SEEK_SET); //110 win32, 200 m16c
         write(fd,blnk_value,item_nb);
      }

      if(t_timeout)
         usleep(t_timeout);

      if(restart)
         lseek(0,0,SEEK_SET);

      blnk = -1;
      restart=0;
   }

   close(fd);

   return 0;
}

/*===========================================
End of Sourcetstlcd.c
=============================================*/
