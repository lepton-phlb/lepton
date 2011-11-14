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

/*!
   \option
      -i use input file.
      -b binary file.
      -f filter definition file

   \brief
      get or set value in binary file.
      by default use .btb filter file in /usr/share/btb.

   \use
      btb adj_loop.i_trip
      get value of adj_loop.i_trip parameter

      btb adj_loop.i_trip=12
      set value 12 to adj_loop.i_trip parameter.
*/

/*===========================================
Includes
=============================================*/
#include <stdlib.h>
#include "kernel/core/signal.h"
#include "kernel/core/libstd.h"
#include "kernel/core/devio.h"
#include "kernel/core/fcntl.h"
#include "kernel/core/wait.h"

#include "lib/libc/stdio/stdio.h"

#include "lib/libc/misc/ftoa.h"


/*===========================================
Global Declaration
=============================================*/
const char dflt_fltr_path[] = "/usr/share/btb/.btb";

#define  OPT_MSK_I 0x01 //-i use input file
#define  OPT_MSK_B 0x02 //-b binary file
#define  OPT_MSK_F 0x04 //-f filter definition file
#define  OPT_MSK_L 0x08 //-l list filter definition file
#define  OPT_MSK_A 0x10 //-a list all bin file

typedef struct{
   char type;
   char size;
   int offset;
}prm_t;

#define GET_SECTION     0
#define GET_PRM         1
#define SET_VALUE       2

/*===========================================
Implementation
=============================================*/
/*-------------------------------------------
| Name:get_section
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int get_section(int fd,char* name,int* fd_out,int* offset){
   char c;
   int st=0;
   int i=0;
   int len = strlen(name);
   char buf[PATH_MAX];
   char* filename=buf;
   char* s_offset=buf;

   *offset=0;

   lseek(fd,0,SEEK_SET);

   while(read(fd,&c,1)>0){
      if(c=='\x18')
         exit(0);

      if(c==' ')
         continue;
      if(!st && c=='#'){
         st=-1;
         continue;
      }
      if(st<0){
         if(c=='\r'|| c=='\n')
            st = 0;
         continue;
      }

      if(st==0 && c=='/'){
         i=0;
         st=1;
         continue;
      }else if(st==1 && c=='|'){
         if(i!=len)
            return -1;
         i=0;
         st=2;
         len=sizeof(buf)-1;
         continue;
      }else if(st>0 && (c==';'||c=='@')){
         if(st==2){
            int _fd;
            filename[i]='\0';
            if((_fd=open(filename,O_CREAT|O_RDWR|O_NONBLOCK,0))>=0){
               close(*fd_out);
               *fd_out=_fd;
            }
            //
            if(c==';')
               return lseek(fd,0,SEEK_CUR);

            i=0;
            st=3;
            len=sizeof(buf)-1;
            continue;
         }else if(st==3){

            s_offset[i]='\0';
            *offset = atoi(s_offset);
            return lseek(fd,0,SEEK_CUR);
         }
      }

      //
      if(i==len)
         return -1;
      //
      if(c=='\r'|| c=='\n')
         continue;
      if(st==1
         && c!=name[i++] ){
           st=-1;
           i=0;
      }else if(st==2){
         filename[i++]=c;
      }else if(st==3){
         s_offset[i++]=c;
      }

   }

   return -1;
}

/*-------------------------------------------
| Name:get_prminfo
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int get_prminfo(int fd,int pos,char* name,prm_t* prm){

   char c;
   int st=0;
   int i=0;
   int len = strlen(name);
   char buf[16];

   if(lseek(fd,(off_t)pos,SEEK_SET)<0)
      return -1;

   while(read(fd,&c,1)>0){
      if(c=='\x18')
         exit(0);

      if(c=='/')
         return -1;//next section is encounter

      if(!st && c=='#'){
         st=-1;
         continue;
      }
      if(st<0){
         if(c=='\r'|| c=='\n')
            st = 0;
         continue;
      }
      if((c=='\r'|| c=='\n'))
         continue;
      if(c==' ')
         continue;
      if(!st && c==':'){
         if(i==len)
            break;
         i=0;
         st=-1;
         continue;
      }
      if(!st && (i==len || c!=name[i++]) ){
         i=0;
         st=-1;
         continue;
      }
   }

   if(st!=0)
      return -1;//cannot match parameter
   //
   while(read(fd,&c,1)>0){
      if(c=='\x18')
         exit(0);

      switch(st){
         case 0:
            if(c=='%')
               st=1;
         break;
         case 1://get format
            if(c=='#'){
               st=2;
               i=0;
               break;
            }
            prm->type = c;
         break;
         case 2://get size
            if(c=='@'){
               buf[i]='\0';
               prm->size=atoi(buf);
               i=0;
               st=3;
               break;
            }
            buf[i++]=c;
            if(i==sizeof(buf))
               return -1;
         break;

         case 3://get offset
            if(c==';'){
               buf[i]='\0';
               prm->offset=atoi(buf);
               i=0;
               return 0;
            }
            buf[i++]=c;
            if(i==sizeof(buf))
               return -1;
         break;
      }

   }


   return -1;
}

/*-------------------------------------------
| Name:get_c
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int get_c(int fd_in,char** cmd){
   int c=-1;

   if(*cmd){
      c = **cmd;
      if(!c){
         **cmd = c ='\n';
         return c;
      }else if(c=='\n')
         return -1;
      (*cmd)++;
      return c;
   }

   if(read(fd_in,&c,1)<=0)
      return -1;

   return c;
}

/*-------------------------------------------
| Name:parser
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int parser(int fd_in,int fd,int fd_out,char * cmd){
   signed char c;
   int st=GET_SECTION;
   char buf[32];
   int i=0;
   int pos= 0;
   prm_t prm;
   char * p =(char*)0;

  int offset = 0;

   if(cmd)
      p = (char*)cmd;

   lseek(fd_in,(off_t)0,SEEK_SET);

   while((c = get_c(fd_in,&p))>0){
      if(c==' ')
         continue;

      if(c=='\x18')
         break;

      switch(st){
         case GET_SECTION:
            if(c=='.'){
               buf[i]='\0';
               printf("%s.",buf);
               if((pos = get_section(fd,buf,&fd_out,&offset))<0)
                  return -2;//cannot find section
               st=GET_PRM;
               i=0;
               break;
            }
            if(c=='\r' || c=='\n')
               break;
            buf[i++]=c;
            if(i==sizeof(buf))
               return -1;
         break;
         case GET_PRM:
            if(c=='=' || c==';' ||c=='\r' || c=='\n'){
               buf[i]='\0';
               printf("%s",buf);
               if(get_prminfo(fd,pos,buf,&prm)<0)
                  return -3;//cannot find parameter in this section
               if(fd_out<0)
                  return -1;
               //
               if(fd_out>=0)
                  if(lseek(fd_out,(off_t)(offset+prm.offset),SEEK_SET)<0)
                     return -1;
               //
               if(c=='='){
                  st=SET_VALUE;
                  printf("=");
               }else{
                  switch(prm.type){
                     case 'c':{
                        char c=0;
                        read(fd_out,&c,prm.size);
                        printf("= %c\r\n",c);
                     }
                     break;
                     case 'd':{
                        int d=0;
                        int e;
                        e= read(fd_out,(char*)&d,prm.size);
                        printf("= %d\r\n",(int)d);
                     }
                     break;
                     case 'l':{
                        long l=0;
                        read(fd_out,(char*)&l,prm.size);
                        printf("= %ld\r\n",(long)l);
                     }
                     break;
                     case 'f':{
                        float f=0;
                        read(fd_out,(char*)&f,prm.size);
                        ftoa(buf,(float)f);
                        //printf(("= %f\r\n",f);
                        printf("= %s\r\n",buf);
                     }
                     break;
//-----------------------------------------------------------------------
                     case 's':{  //modif AS du 07-07-2008
                        char c=0;
                        char count = prm.size;

                        if (count != 0) {
                              read(fd_out,&c,1);
                              printf("= %c",c);
                              count = count - 1;

                              while ((count != 0) && (c != 0) ) {
                                   read(fd_out,&c,1);
                                   printf("%c",c);
                                   count = count - 1;
                              }
                        }
                        printf("\r\n");
                     }
                     break;
//-----------------------------------------------------------------------
                     default:
                        return -1;
                     break;
                  }
                  //printf(("get value: type:'%c' size:%d pos:%d\r\n>",prm.type,prm.size,prm.offset);
                  st=GET_SECTION;
                  if(!c)
                     return 0;
               }
               i=0;
               buf[0]='\0';
               break;
            }
            if(c=='\r' || c=='\n')
               break;
            buf[i++]=c;
            if(i==sizeof(buf))
               return -1;
         break;
         case SET_VALUE:
            if(c==';' ||c=='\r' || c=='\n'){
               buf[i]='\0';
               printf("%s\r\n",buf);
               switch(prm.type){
                  case 'c':
                     write(fd_out,buf,prm.size);
                  break;
                  case 'd':{
                     int d = atoi(buf);
                     write(fd_out,(char*)&d,prm.size);
                  }
                  break;
                  case 'l':{
                     long l = atol(buf);
                     write(fd_out,(char*)&l,prm.size);
                  }
                  break;
                  case 'f':{
                     float f = (float)atof(buf);
                     write(fd_out,(char*)&f,prm.size);
                  }
                  break;
                  default:
                     return -1;
                  break;
               }
               //printf("\r\nset value: type:'%c' size:%d pos:%d value=%s\r\n",prm.type,prm.size,prm.offset,buf);
               st = GET_SECTION;
               i=0;
               buf[0]='\0';
               if(!c)
                  return 0;
               break;
            }
            if(c=='\r' || c=='\n')
               break;
            buf[i++]=c;
            if(i==sizeof(buf))
               return -1;
         break;
      }



   }

   return 0;
}

/*-------------------------------------------
| Name:main
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int btb_main (int argc,char* argv[]){

   int fd_in   = 0;
   int fd_out  = -1;
   int fd;
   int i;
   int error=0;
   unsigned int opt=0;

   char * fltr_file_path   = (char*)dflt_fltr_path;
   char * cmd_file_path    = (char*)0;
   char * bin_file_path    = (char*)0;

   char* cmd =(char*)0;

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
                     cmd_file_path = argv[i];

               break;

               case 'b':
                  opt |= OPT_MSK_B;
                  //get binary file device
                  if((i+1) == argc)//not enough parameter
                     return -1;
                  if(argv[i][c+1])
                     break;
                  if(argv[i+1][0]=='-')
                     break;

                  i++;
                  if(argv[i])
                     bin_file_path = argv[i];
               break;

               case 'f':
                  opt |= OPT_MSK_F;
                  //get lcd layout
                  if((i+1) == argc)//not enough parameter
                     return -1;
                  if(argv[i+1][0]=='-')
                     break;
                  i++;
                  if(argv[i])
                     fltr_file_path = argv[i];
               break;

               case 'l'://list only filter file
                  opt |= OPT_MSK_L;
               break;

               case 'a'://list all parameter
                  opt |= OPT_MSK_A;
               break;
            }
         }
      }else{
         if(!argv[i])
            return -1;

         cmd = argv[i];

      }
   }

   //
   if( (opt&OPT_MSK_B) && (fd_out = open(bin_file_path,O_CREAT|O_RDWR|O_NONBLOCK,0))<0){
      fprintf(stderr,"cannot open bin file\r\n");
      return -1;
   }

   //
   if((fd = open(fltr_file_path,O_RDONLY,0))<0){
      fprintf(stderr,"cannot open filter file\r\n");
      return -1;
   }

   //
   if( (opt&OPT_MSK_I) && (fd_in = open(cmd_file_path,O_RDONLY,0))<0){
      fprintf(stderr,"cannot open cmd file\r\n");
      return -1;
   }

   //
   if((error=parser(fd_in,fd,fd_out,cmd))<0){
      switch(error){
         case -2:
            fprintf(stderr,"\r\nerror: section not found\r\n");
         break;

         case -3:
            fprintf(stderr,"\r\nerror: parameter not found\r\n");
         break;

         default:
            fprintf(stderr,"\r\nerror\r\n");
         break;
      }


   }

   //
   close(fd_in);
   close(fd);
   close(fd_out);


   return 0;

}

/*===========================================
End of Source btb.c
=============================================*/
