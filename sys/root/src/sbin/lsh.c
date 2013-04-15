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

#include "kernel/core/errno.h"
#include "kernel/core/signal.h"
#include "kernel/core/libstd.h"
#include "kernel/core/devio.h"
#include "kernel/core/fcntl.h"
#include "kernel/core/wait.h"
#include "kernel/core/stat.h"

#include "lib/libc/stdio/stdio.h"

//for cygwin unix test
/*
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <io.h>
#include <stdio.h>
*/

/*===========================================
Global Declaration
=============================================*/
#define CMD_MAX 64
#define ARG_MAX 20

#define GET_CMD      0
#define GET_STDIN    1
#define GET_STDOUT   2
#define GET_COMMENT  3
#define GET_ARG      4
#define GET_POSPRM   5

#define  OPT_MSK_V 0x01 //-v verbose mode
#define  OPT_MSK_E 0x02 //-e
#define  OPT_MSK_X 0x04 //-x xon/xoff

#define SET_OPT_XONXOFF   "xonxoff"

const char _c_xon_   =    0x11; //ctrl-Q
const char _c_xoff_  =    0x13; //ctrl-S

const char prompt[]="lepton";
const char lsh_path[]="/usr/sbin/lsh";
const char lshrc_path[]="/usr/sbin/.lshrc";




const char* lsh_env[] = {
   "/usr/sbin",
   "/usr/sbin/sh",
   "/",
   (char*)0
};


const char lsh_banner[] = "\r\n\tlepton shell\r\n\r\n";


/*===========================================
Implementation
=============================================*/

/*-------------------------------------------
| Name:sigchild_handler
| Description:to tes SIGCHLD under win32
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static void sigchild_handler(int sig){
/*
   static char _buf[255] = "\r\nrcv SIGCHLD\r\n";
   int status;
   pid_t pid;
   pid = waitpid((pid_t)0,&status,WNOHANG);
   if(pid>0){
      _buf[strlen("\r\nrcv SIGCHLD\r\n")]=pid+'0';
      _buf[strlen("\r\nrcv SIGCHLD\r\n")+1]='\0';
      write(1,_buf,strlen(_buf));
   }else{
      write(1,"\r\nrcv SIGCHLD pid<0\r\n",strlen("\r\nrcv SIGCHLD pid<0\r\n"));
   }
*/
}

/*-------------------------------------------
| Name:check_script
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int check_script(char* ref, int* argc, char* argv[], const char* arge[]){
   char env_path[PATH_MAX]={0};
   char buf[2]={0};
   int fd;
   int i=0;
   struct stat _stat;

   //open exec file
   fd=open(ref,O_RDONLY,0);
   if(fd<0) { //error: file not exist
      while(arge[i]) {

         strcpy(env_path,arge[i]);

         if(ref[0]!='/') {
            int len = strlen(arge[i]);
            env_path[len]='/';
            env_path[len+1]='\0';
         }
         //to do: chek length of env_path+path
         strcat(env_path,ref);
         if((fd = open((char*)env_path,O_RDONLY,0))>=0)
            break;
         i++;
      }
      if(fd<0)
         return 0;  //cannot find.
      //is not an error.
   }

   if(fstat(fd,&_stat)<0) {
      close(fd);
      return -1;
   }

   //must be a regular file
   if(!S_ISREG(_stat.st_mode)) {
      close(fd);
      return -1;
   }
   //check
   read(fd,buf,1);
   if(buf[0]==0x1b) {
      close(fd);
      return 0; //is not a shell script :( it's a binary file.
      //is not an error.
   }
   close(fd);

   //it's a shell script :)
   for(i=*argc; i>0; i--) {
      argv[i] = argv[i-1];
   }

   argv[0] = (char*)lsh_path;

   (*argc)+=1;

   return 0;
}

/*-------------------------------------------
| Name:parse_cmd
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int parse_cmd(char* last_cmd, char* string,int p_argc, char* p_argv[],unsigned int* p_opt,
                     char verbose){
   int argc=0;
   char* argv[ARG_MAX]={0};

   int error = 0;
   char* __stdin=0;
   char* __stdout=0;
   int len =  strlen(string);
   int i;
   char* p=string;

   char sep_cmd=' ';

   int status;
   char flg_bckg_process = 0;
   char flg_recall_lastcmd = 0;

   pid_t pid =0;
   //
   int st=GET_CMD;
   //
   int fdin = -1;

   //
   if(!len)
      return 0;

   //save original stdin
   fdin = dup(STDIN_FILENO); //WARNING!!! dont forget to close it before return;
   if(fdin<0)
      return -1;

   if(p[0]=='#')
      st=GET_COMMENT;
   //
   for(i=0; (i<=len && p); i++) {

      if( (string[i]==sep_cmd || string[i]=='\'')
          && (st==GET_CMD || st==GET_ARG) ) {

         if(string[i]=='\'' && sep_cmd==' ') {
            sep_cmd = '\'';
            st=GET_ARG;
         }else if(string[i]=='\'' && sep_cmd=='\'') {
            sep_cmd = ' ';
            st=GET_CMD;
         }
         string[i]=0x00;
         if(strlen(p))
            argv[argc++]=p;
         p = &string[i+1];
         if(p[0]=='#')
            st=GET_COMMENT;
      }else if(string[i]==' ' && st==GET_STDIN) {
         string[i]=0x00;
         if(strlen(p)) {
            __stdin=p;
            st=GET_CMD;
         }
         p = &string[i+1];
      }else if(string[i]==' ' && st==GET_STDOUT) {
         string[i]=0x00;
         if(strlen(p)) {
            __stdout=p;
            st=GET_CMD;
         }
         p = &string[i+1];
      }else if(string[i]==' ' && st==GET_POSPRM) {
         //postionnal parameter $1 $2 ...
         string[i]=0x00;
         if(strlen(p)) {
            //0:"/usr/bin/lsh"; 1:"script name"; 2:"first parameter"; 3:"second parameter" etc...
            int pos= atoi(p)+1;
            if( (pos < p_argc) && (pos< ARG_MAX) )
               argv[argc++]=p_argv[pos];
            else
               argv[argc]=(char*)0;
         }
         st=GET_CMD;
         p = &string[i+1];
      }else if(st!=GET_COMMENT && st!=GET_ARG && string[i] == '<' ) {
         st=GET_STDIN;
         string[i]=0x00;
         if(strlen(p))
            argv[argc++]=p;
         p = &string[i+1];
      }else if(st!=GET_COMMENT && st!=GET_ARG && string[i] == '>' ) {
         st=GET_STDOUT;
         string[i]=0x00;
         if(strlen(p))
            argv[argc++]=p;
         p = &string[i+1];
      }else if(st!=GET_COMMENT && st!=GET_ARG && string[i] == '$' ) {
         st=GET_POSPRM; //postionnal parameter $1 $2 ...
         string[i]=0x00;
         if(strlen(p))
            argv[argc++]=p;
         p = &string[i+1];
      }else if(st!=GET_COMMENT && st!=GET_ARG && string[i] == '!' ) {
         if(!last_cmd)
            continue;
         //insert last cmd
         if(string[i+1]!='!')
            continue;
         if((i+strlen(&string[i+2]))>=CMD_MAX)
            return -1;  //command too long

         strcat(last_cmd,&string[i+2]);
         strcpy(&string[i],last_cmd);
         len = i+strlen(&string[i]);
         i--;
         flg_recall_lastcmd = 1;
         continue;
      }else if(string[i] == ';'
               || string[i] == '|'
               || string[i] == '\n'
               || string[i] == '\0') {
         char sep = string[i];
         int error=0;
         int fd[2];
         fd[0]=-1;
         fd[1]=-1;

         //last cmd
         if(string[i]=='\n' || string[i]=='\0') {
            if(last_cmd) {
               int j;
               memcpy(last_cmd,string,CMD_MAX);
               for(j=0; j<len; j++) {
                  if(!last_cmd[j])
                     last_cmd[j]=' ';
               }
               if(flg_recall_lastcmd)
                  printf("\r\n %s\r\n",last_cmd);
            }
         }


         //save original stdin
         /*fdin = dup(STDIN_FILENO);//WARNING!!! dont forget to close it before return;
         if(fdin<0)
            return -1;*/

         //
         string[i]=0x00;
         if(strlen(p)) {
            if(st==GET_COMMENT) {
               if(fdin>=0)
                  close(fdin);
               return error;
            }else if(st==GET_CMD)
               argv[argc++]=p;
            else if(st==GET_STDIN)
               __stdin=p;
            else if(st==GET_STDOUT)
               __stdout=p;
            else if(st==GET_POSPRM) {
               //postionnal parameter $1 $2 ...
               int pos= atoi(p);
               if( (pos < p_argc) && (pos< ARG_MAX) )
                  argv[argc++]=p_argv[pos+1];
               else
                  argv[argc]=(char*)0;
            }
         }else if(!argc) {
            if(fdin>=0)
               close(fdin);
            return error;
         }
         p = &string[i+1];

         //
         if(!argc) {
            error = -1;
            break;
         }

         //inline command
         if(!strcmp(argv[0],"cd")) {
            //inline command: cd.
            if(argc<2) { //not enough argument
               error = -1;
               break;
            }

            if(argc>2) { //too much argument
               error = -1;
               break;
            }

            if(argv[1]=='\0') {
               error = -1;
               break;
            }

            if(fdin>=0)
               close(fdin);

            return chdir(argv[1]);
         }else if(!strcmp(argv[0],"exit")) {
            int exit_code=1;
            if(fdin>=0)
               close(fdin);

            if(argv[1])
               exit_code = atoi(argv[1]);

            return exit_code;
         }else if( !strcmp(argv[0],"set") || !strcmp(argv[0],"unset")) {
            int i;
            int setunset=0; //0: unset, 1: set:
            if( !strcmp(argv[0],"set") )
               setunset = 1;  //set

            if(argc<2) { //no parameter
               if(setunset) { //set
                  //show all
                  printf("option=");
                  if((*p_opt)&OPT_MSK_X) {
                     printf(SET_OPT_XONXOFF);
                  }
                  printf(";\r\n");
                  return 0;
               }else{ //unset
                      //no action
                  return 0;
               }
            }

            for(i=1; i<argc; i++) {
               if(!argv[i])
                  return 0;
               if(!strcmp(argv[i],SET_OPT_XONXOFF)) {
                  if(setunset) //set
                     (*p_opt) = ((*p_opt) |OPT_MSK_X);
                  else //unset
                     (*p_opt) = ((*p_opt) & (~(OPT_MSK_X)) );
                  //xon/xoff flow control synchro
               }else{
                  printf(" %s is not a valid option\r\n",argv[i]);
               }

            } //end for

            return 0;
         }

         //
         if(__stdin!=0) {
            struct stat _stat;
            //printf("open __stdin %s\r\n",__stdin);
            if(stat(__stdin,&_stat)<0)
               return -1;
            if(S_ISFIFO(_stat.st_mode)) {
               int oflag;
               if((fd[0]=open(__stdin,O_RDONLY|O_NONBLOCK,0))<0)
                  return -1;
               oflag = fcntl(STDIN_FILENO,F_GETFL);
               oflag&=(~O_NONBLOCK);
               fcntl(STDIN_FILENO,F_SETFL,oflag);
            }else{
               if((fd[0]=open(__stdin,O_RDONLY,0))<0)
                  return -1;
            }
         }

         if(__stdout!=0) {
            //printf("open __stdout %s\r\n",__stdout);
            fd[1]=open(__stdout,O_WRONLY|O_CREAT|O_TRUNC,0);
         }

         //to do: check script
         error=check_script(argv[0],&argc,argv,lsh_env);

         //
         if(!error && sep == '|') {
            //to do: if flg_bckg_process == 1 grammatical error
            int pp[2];
            pp[0]=-1;
            pp[1]=-1;
            pipe(pp);

            if(fd[0]<0)
               fd[0]=pp[0];
            if(fd[1]<0)
               fd[1]=pp[1];


            if(!(pid = vfork())) {
               //printf("exec 1 %s\r\n",argv[0]);
               if(fd[1]>=0) {
                  close(STDOUT_FILENO);
                  dup(fd[1]);
                  close(fd[1]);
               }
               if(fd[0]>=0)
                  close(fd[0]);
               //
               if(flg_bckg_process)
                  setpgid(0,0);
               //
               if(fdin>=0)
                  close(fdin);
               //
               execve(argv[0],(const char **)argv,0);
               exit(2);
            }

            //
            if(fd[0]>=0) {
               close(STDIN_FILENO);
               dup(fd[0]);
               close(fd[0]);
               fd[0]=-1;
            }

            //
            if(fd[1]>=0) {
               close(fd[1]);
               fd[1]=-1;
            }
         }else if(!error) {

            //
            if(!(pid = vfork())) {
               //printf("exec 2 %s\r\n",argv[0]);
               if(fd[0]>=0) {
                  close(STDIN_FILENO);
                  dup(fd[0]);
                  close(fd[0]);
               }
               if(fd[1]>=0) {
                  close(STDOUT_FILENO);
                  dup(fd[1]);
                  close(fd[1]);
               }
               //
               if(flg_bckg_process)
                  setpgid(0,0);
               //
               if(fdin>=0)
                  close(fdin);
               //
               execve(argv[0],(const char **)argv,0);
               exit(2);
            }
            //restore original stdin
            close(STDIN_FILENO);
            dup(fdin);
            close(fdin);

            //
            if(fd[0]>=0)
               close(fd[0]);
            if(fd[1]>=0)
               close(fd[1]);

            if(!error && !flg_bckg_process && pid>0 &&  sep != '|') {
               while((pid=waitpid((pid_t)0,&status,0))>0) {
                  if(!status && verbose)
                     printf("[%d] done(%d)\r\n",pid,status);
                  else if(status) {
                     fprintf(stderr,"error: [%d] done(%d)\r\n",pid,status);
                     //to do: print on console explicit message
                  }
               };
               //printf("\r\n");
            }

         }

         memset(argv,0,ARG_MAX*sizeof(char*));
         argc=0;
         __stdin=0;
         __stdout=0;
         st=GET_CMD;
      }else if(st!=GET_COMMENT && string[i] == '&' ) {
         //set flag bakground process flg_bckg_process;
         flg_bckg_process = 1;
         string[i]=0x00;
      }
   }


   return error;
}

/*-------------------------------------------
| Name:rdstream
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int rdstream(FILE* fin,FILE* fout,char *buf, int len)
{
   int i = 0;
   signed char ch;

   while(1) {

      ch = fgetc(fin);
      //to do: up down key filter.

      switch(ch) {
      case -1:
         return -1;


      case 127: //DEL
      case   8:          //^H
         if(i>0) {
            putc(8,fout);                    //^H
            putc(' ',fout); //^H
            putc(8,fout); //^H
            i--;
         }
         break;

      //
      case '\x18': //ctrl-x:exit
         putc('\r',fout);
         putc('\n',fout);
         exit(0);
         break;

      //
      case '\0':
//#if defined(CPU_GNU32)
//        if(!i)	continue;
//#endif
         buf[i] = 0;
         return i;

      //
      case '\n':
#if defined(CPU_GNU32)
         buf[i] = 0;
         putc(ch,fout);
         putc('\r',fout);
         return i;
#endif
         break;

      case '\r':
         buf[i] = 0;
         putc(ch,fout);
         putc('\n',fout);
         return i;

      //
      default:
         if(ch<32 || ch>126)
            break;

         buf[i] = ch;
//#if !defined(CPU_GNU32)
         putc(ch,fout);
//#endif
         if(++i==len) {
            fprintf(stderr,"\r\nerror: command line is too long (60 char max).\r\n");
            return ENOMEM;
         }
      }
   }

   return 0;
}

/*-------------------------------------------
| Name:rdstream_noecho
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int rdstream_noecho(FILE* fstream,char *buf, int len)
{
   int i = 0;
   signed char ch;

   while(1) {

      ch = fgetc(fstream);
      //to do: up down key filter.

      switch(ch) {
      case -1:
         return -1;

      //
      case '\x18': //ctrl-x:exit
         exit(0);
         break;

      //
      case '\0':
         buf[i] = 0;
         return i;

      //
      case '\n':
         buf[i] = 0;
         return i;
      //break;

      case '\r':
         buf[i] = 0;
         return i;

      //
      default:
         if(ch<32 || ch>126)
            break;

         buf[i] = ch;
         if(++i==len) {
            fprintf(stderr,"\r\nerror: command line is too long (60 char max).\r\n");
            return ENOMEM;
         }
      }
   }

   return 0;
}

/*-------------------------------------------
| Name:lsh_main
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
//int main(int argc, char* argv[])
int lsh_main(int argc, char* argv[])
{
   char buf[CMD_MAX+1];
   //char last_cmd[CMD_MAX+1];

   //
   pid_t pid=getpid();
   int exit_code=0;

   int i;
   unsigned int opt=0;
   char verbose=0;
   FILE* fout = stdout;

   //only for SIGCHLD bug test under win32
   //struct sigaction sa;
   //SIGCHLD interception
   //sa.sa_handler=sigchild_handler;
   //sigaction(SIGCHLD,&sa,NULL);


   //silent mode for script shell
   //get option
   for(i=1; i<argc; i++) {
      if(argv[i][0]=='-') {
         unsigned char c;
         unsigned char l=strlen(argv[i]);
         for(c=1; c<l; c++) {
            switch(argv[i][c]) {
            case 'v':
               opt |= OPT_MSK_V;
               //verbose
               verbose = 1;
               break;

            case 'e':
               opt |= OPT_MSK_E;
               //echo
               fout = stderr;
               break;

            case 'x':
               opt |= OPT_MSK_X;
               //xon/xof flow control
               break;
            }
         }
      }else{
         int e= 0;
         FILE* fstream=0;

         if(!argv[i])
            break;

         //execute script
         fstream = fopen(argv[i],"r");
         //
         if(!fstream) { //error: file not exist
            while(lsh_env[e]) {

               strcpy(buf,lsh_env[e]);

               if(argv[i][0]!='/') {
                  int len = strlen(lsh_env[e]);
                  buf[len]='/';
                  buf[len+1]='\0';
               }
               //to do: chek length of env_path+path
               strcat(buf,argv[i]);
               if((fstream = fopen(buf,"r")))
                  break;
               e++;
            }
            if(!fstream) {
               fprintf(stderr,"cannot open script file : %s\r\n",argv[i]);
               return -1;
            }
         }

         //
         //setpgid(0,0);
         //
         if(!verbose) {
            while(rdstream_noecho(fstream,buf,CMD_MAX)>=0) {
               if((exit_code = parse_cmd((char*)0 /*last_cmd*/,buf,argc,argv,&opt,verbose))>0) {
                  fclose(fstream);
                  exit(exit_code);
               }
            }
         }else{
            while(rdstream(fstream,stdout,buf,CMD_MAX)>=0) {
               if((exit_code = parse_cmd((char*)0 /*last_cmd*/,buf,argc,argv,&opt,verbose))>0) {
                  fclose(fstream);
                  exit(exit_code);
               }
            }
         }

         //
         fclose(fstream);

      }
   }

   fprintf(fout,lsh_banner);
   //
   //setpgid(0,0);
   //

   //
   for(;; ) {
      int status;
      pid_t _pid;
      int error;

      //prompt
      //if(!(opt&OPT_MSK_E)) //echo is enabled
      fprintf(fout,"%s#%d$ ",prompt,pid);
      fflush( fout );
      //read command line

      //send xon: ok previous command is terminated, ready for next command
      if(opt&OPT_MSK_X)
         write(1,&_c_xon_,1);

      if(!(opt&OPT_MSK_E) && ((error=rdstream(stdin,fout,buf,CMD_MAX))<0)) {
         if(error==ENOMEM)
            continue;  //line too long
         //else
         return 0; //end of file or remote connection down.
      }else if((opt&OPT_MSK_E) && ((error=rdstream_noecho(stdin,buf,CMD_MAX))<0)) {
         if(error==ENOMEM)
            continue;  //line too long
         //else
         return 0; //end of file or remote connection down.
      }

      //send xoff: wait end of command
      if(opt&OPT_MSK_X)
         write(1,&_c_xoff_,1);

      //

      //interpret command line
      if((exit_code=parse_cmd((char*)0 /*last_cmd*/,buf,argc,argv,&opt,verbose))>0)
         exit(exit_code);

      //wait backgroud process termination (&)
      while((_pid=waitpid((pid_t)-1,&status,WNOHANG))>0) {
         if(!status && verbose)
            printf("+[%d] done(%d)\r\n",_pid,status);
         else if(status)
            fprintf(stderr,"error: +[%d] done(%d)\r\n",_pid,status);
      };
   }

   return 0;
}


/*===========================================
End of Source shell.c
=============================================*/
