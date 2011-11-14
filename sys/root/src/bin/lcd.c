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
#include "kernel/devio.h"
#include "kernel/libstd.h"

#include "stdio/stdio.h"
#include "xlcd/xlcd.h"


/*===========================================
Global Declaration
=============================================*/
#define  OPT_MSK_T 0x01 //-t tiny display
#define  OPT_MSK_M 0x02 //-m main display


/*===========================================
Implementation
=============================================*/

int lcd_main(int argc,char* argv[]){

   struct xlcd_attr_t attr;
   struct xlcd_context_t context;
   int i;
   unsigned int opt=0;
   DIR* dir;
   struct dirent dirent;
   char buf[24]={0};
   char* str=(char*)0;

   attr.callback     = (xlcd_callback_t)0;
   attr.argc         = argc;
   attr.argv         = argv;
   attr.lcd_layout   = 0;
   attr.fdin_server  = -1;
   attr.fdout_server  = -1;

   //search lcd display
   if(!(dir=opendir("/dev")))
      return -1;

   while(readdir(dir,&dirent)){
   
      if(dirent.d_name[0]=='l'
         && dirent.d_name[1]=='c'
         && dirent.d_name[2]=='d'){
         strcpy(buf,"/dev/");
         strcat(buf,dirent.d_name);
         break;
      }
   }
   closedir(dir);

   //open lcd display
   close(1);

   if(open(buf,O_WRONLY,0)<0)
      return -1;

   xlcd_create(&context,&attr);


   //get option
   for(i=1;i<argc;i++){
      if(argv[i][0]=='-'){
         unsigned char c;
         unsigned char l=strlen(argv[i]);
         for(c=1;c<l;c++){
            switch(argv[i][c]){
               case 't'://tiny
                  opt |= OPT_MSK_T;
               break;

               case 'm'://main
                  opt |= OPT_MSK_M;
               break;

            }
         }
      }else{
         int len;
         int pos;
         if(!(str = argv[i]))
            return -1;

         len = strlen(str);

         if(len>4){

            for(pos=0; pos<len;pos++){

               memset(buf,'*',8);
               memcpy(buf,&str[pos],((len-pos)>4?4:(len-pos)));

               if(opt&OPT_MSK_T)
                  xlcd_tiny_display_str(&context,buf,0);
               if(opt&OPT_MSK_M)
                  xlcd_main_display_str(&context,buf,0);
               usleep(500000);//0.5 s
            }
            
         }else{
            if(opt&OPT_MSK_T)
               xlcd_tiny_display_str(&context,str,0);
            if(opt&OPT_MSK_M)
               xlcd_main_display_str(&context,str,0);
            usleep(500000);//0.5 s
         }

         

      }
   }

   return 0;
}

/*===========================================
End of Source lcd.c
=============================================*/
