/*
The contents of this file are subject to the Mozilla Public License Version 1.1 
(the "License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS" basis, 
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License for the 
specific language governing rights and limitations under the License.

The Original Code is ______________________________________.

The Initial Developer of the Original Code is ________________________.
Portions created by ______________________ are Copyright (C) ______ _______________________.
All Rights Reserved.

Contributor(s): ______________________________________.

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
#include "kernel/signal.h"
#include "kernel/libstd.h"
#include "kernel/stat.h"
#include "kernel/statvfs.h"
#include "kernel/devio.h"
#include "kernel/time.h"
#include "kernel/wait.h"
#include "kernel/fcntl.h"
#include "kernel/select.h"

#include "stdio/stdio.h"
#include "xlcd/xlcd.h"


/*===========================================
Global Declaration
=============================================*/

#define  OPT_MSK_I 0x01 //-i use export keyboard
#define  OPT_MSK_O 0x02 //-o use export lcd
#define  OPT_MSK_L 0x04 //-l specify lcd layout
#define  OPT_MSK_SERVER 0x08 //server specify server/client for distant or local mode

#define __BUF_MAX 10

#define __SERVER "server"

//to remove: only for script compatibility
const char dflt_bin_path[] = "mgr/mgr";


/*===========================================
Implementation
=============================================*/
/*-------------------------------------------
| Name:mmgrd_main
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int xlcdd_main(int argc,char* argv[]){

   int fd_kb;
   int fd_lcd;
   int fdin_server;
   int fdout_server;
   int i;
   unsigned int opt=0;

   char * arg_layout=0;
   
   char* __stdin=0;
   char* __stdout=0;

   char * bin_path = (char*)dflt_bin_path;

   //get option
   for(i=1;i<argc;i++){
      if(argv[i][0]=='-'){
         unsigned char c;
         unsigned char l=strlen(argv[i]);
         for(c=1;c<l;c++){
            switch(argv[i][c]){
               case 'i':
                  opt |= OPT_MSK_I;
                  //get stdin device
                  if((i+1) == argc)//not enough parameter
                     return -1;
                  if(argv[i][c+1])
                     break;
                  if(argv[i+1][0]=='-')
                     break;

                  i++;
                  if(argv[i])
                     __stdin = argv[i];
                  if((opt&OPT_MSK_O) && !__stdout)
                     __stdout = __stdin;

               break;

               case 'o':
                  opt |= OPT_MSK_O;
                  //get stdin device
                  if((i+1) == argc)//not enough parameter
                     return -1;
                  if(argv[i][c+1])
                     break;
                  if(argv[i+1][0]=='-')
                     break;

                  i++;
                  if(argv[i])
                     __stdout = argv[i];
                  if((opt&OPT_MSK_I) && !__stdin)
                     __stdin = __stdout;

               break;

               case 'l':
                  opt |= OPT_MSK_L;
                  //get lcd layout
                  if((i+1) == argc)//not enough parameter
                     return -1;
                  if(argv[i+1][0]=='-')
                     break;
                  i++;
                  if(argv[i])
                     arg_layout = argv[i];
               break;
            }
         }
      }else{
         if(!argv[i])
            return -1;
         if(!strcmp(argv[i],__SERVER))
            opt |= OPT_MSK_SERVER;
         else
            bin_path = argv[i];

      }
   }

   if(opt&OPT_MSK_SERVER){
      printf("server mode enable (send ctrl-x to exit)\r\n");
      
      fdin_server  = dup(STDIN_FILENO);
      fdout_server = dup(STDOUT_FILENO);
   }

   //open stdin
   if( !(opt&OPT_MSK_I)){
      //close stdin
      close(STDIN_FILENO);
      //open keyboard
      if((fd_kb=open("/dev/kb0",O_RDONLY|O_NONBLOCK,0))<0){
         printf("error: cannot open /dev/kb0\r\n");
         return -1;
      }
   }else if(__stdin){
      //close stdin
      close(STDIN_FILENO);
      //open keyboard
      if((fd_kb=open(__stdin,O_RDONLY|O_NONBLOCK,0))<0){
         printf("error: cannot open %s\r\n",__stdin);
         return -1;
      }
   }else{
      fd_kb = STDIN_FILENO;
   }

   //open stdout
   if( !(opt&OPT_MSK_O)){
      DIR* dir;
      struct dirent dirent;
      char buf[24]={0};
      
      //open lcd display
      if(!(dir=opendir("/dev")))
         return -1;

      while(readdir2(dir,&dirent)){
      
         if(dirent.d_name[0]=='l'
            && dirent.d_name[1]=='c'
            && dirent.d_name[2]=='d'){
            strcpy(buf,"/dev/");
            strcat(buf,dirent.d_name);
            break;
         }
      }
      closedir(dir);

      printf("open lcd driver:%.8s\r\n",buf);
      //close stdout
      close(STDOUT_FILENO);

      if((fd_lcd=open(buf,O_WRONLY,0))<0){
         fprintf(stderr,"error: cannot open %.8s\r\n",buf);
         return -1;
      }
   }else if(__stdout){
      //close stdout
      close(STDOUT_FILENO);
      //open 
      if((fd_lcd=open(__stdout,O_WRONLY,0))<0){
         fprintf(stderr,"error: cannot open %s\r\n",__stdout);
         return -1;
      }
   }else{
      fd_lcd = STDOUT_FILENO;
   }
     
   if(opt&OPT_MSK_SERVER){
      struct xlcd_attr_t attr={0};
      struct xlcd_context_t context;
   
      attr.callback     = (xlcd_callback_t)0;
      attr.argc         = argc;
      attr.argv         = argv;
      attr.fdin_server  = fdin_server;
      attr.fdout_server = fdout_server;

      xlcd_create(&context,&attr);

      xlcd_engine(&context);
 

   }else if(!vfork()){
      setpgid(0,0);
      if((opt&OPT_MSK_O)) {
         
         if(__stdout){
            struct stat buf;
            stat(__stdout,&buf);
            
            if(S_ISBLK(buf.st_mode))
               execl(bin_path,"-l",arg_layout,NULL);
         }
         if(arg_layout)
            execl(bin_path,"-l",arg_layout,"-x",NULL);
         else
            execl(bin_path,"-x",NULL);
      }else{
         if(arg_layout)
            execl(bin_path,"-l",arg_layout,NULL);
         else
            execl(bin_path,NULL);
      }
      //error
      exit(2);
   }

   return 0;
}



/*===========================================
End of Source xlcdd.c
=============================================*/
