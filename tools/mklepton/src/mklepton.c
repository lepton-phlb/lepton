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


/*============================================
| Includes
==============================================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/statvfs.h>
#include <strings.h>
#include <dirent.h>
#include <libgen.h>

#include <expat.h>
#include "mklepton.h"
#include "kernel_stub.h"


/*============================================
| Global Declaration
==============================================*/
char * xml_current_target=(char*)0;
char * mk_current_target=(char*)0;
char* xml_mk_file=(char*)0;

FILE* f_kernelconf   = NULL;
struct kernel_conf_t g_kernel_conf={0};

const char dflt_kernel_conf_filepath[] = "kernel_mkconf.h";
#define MK_KERNELCONF_FILEPATH   kernel_conf_filepath  
char * kernel_conf_filepath = (char*)dflt_kernel_conf_filepath; //see xml_elmt_mklepton()
const char kernelconf_top_header[]=
"/*===========================================\n\
Compiler Directive\n\
=============================================*/\n\
#ifndef _KERNEL_MKCONF_H\n\
#define _KERNEL_MKCONF_H\n\n\
/*===========================================\n\
Includes\n\
=============================================*/\n\n\
/*===========================================\n\
Declaration\n\
=============================================*/\n";

const char kernelconf_decl_header[]=
"\n/*===========================================\n\
Declaration\n\
=============================================*/\n\n";

const char kernelconf_bottom_header[]="\n#endif\n\n";

const char kernelconf_cpu_freq_define[]="#define __KERNEL_CPU_FREQ";
const char kernelconf_heap_define[]="#define __KERNEL_HEAP_SIZE";
const char kernelconf_thread_define[]="#define __KERNEL_PTHREAD_MAX";
const char kernelconf_process_define[]="#define __KERNEL_PROCESS_MAX";
const char kernelconf_openfiles_define[]="#define MAX_OPEN_FILE";
const char kernelconf_descriptors_define[]="#define OPEN_MAX";
const char kernelconf_env_path_define[]="#define __KERNEL_ENV_PATH";
const char kernelconf_network_define[]="#define __KERNEL_NET_IPSTACK";

//boot configuration
FILE* f_bootconf   = NULL;
const char dflt_boot_filepath[] = ".boot";
#define MK_BOOT_FILEPATH   boot_filepath  
char * boot_filepath = (char*)dflt_boot_filepath; //see xml_elmt_mklepton()

const char dflt_boot_dev[] = "/dev/kb0";
#define MK_BOOT_DEV   boot_dev  
char * boot_dev = (char*)dflt_boot_dev;

const char dflt_boot_delay[] = "0";
char * boot_delay = (char*)dflt_boot_delay;

struct boot_t* pboot_lst_end=0;
struct boot_t* pboot_lst_start=0;

//mount configuration
FILE* f_mountconf   = NULL;
const char dflt_mount_filepath[] = ".mount";
#define MK_MOUNT_FILEPATH   mount_filepath  
char * mount_filepath = (char*)dflt_mount_filepath; //see xml_elmt_mklepton()

struct mount_t* pmount_lst_end=0;
struct mount_t* pmount_lst_start=0;

//devices configuration
FILE* f_devconf   = NULL;
const char dflt_dev_conf_filepath[] = "dev_mkconf.c";
char * dev_conf_filepath = (char*)dflt_dev_conf_filepath; //see xml_elmt_mklepton()
#define MK_DEVCONF_FILEPATH   dev_conf_filepath   
const char devconf_top_header[]=
"/*-------------------------------------------\n\
| Copyright(C) 2008 CHAUVIN-ARNOUX\n\
---------------------------------------------\n\
| Project:\n\
| Project Manager:\n\
| Source: dev_mkconf.c\n\
| Path: config\n\
| Authors:\n\
| Plateform:GNU/Linux\n\
| Created:\n\
| Revision/Date:	$Revision: 1.3 $ $Date: 2009-08-28 09:56:38 $\n\
| Description:\n\
---------------------------------------------*/\n\n\n\
/*===========================================\n\
Includes\n\
=============================================*/\n\
#include \"kernel/core/kernelconf.h\"\n\
#include \"kernel/fs/vfs/vfsdev.h\"\n\n\n\
/*===========================================\n\
Global Declaration\n\
=============================================*/\n\n\n";

const char dev_map_t_extern_declaration[]="extern dev_map_t ";

const char dev_lst_open_declaration[]="\n\npdev_map_t const dev_lst[]={\n";
const char dev_lst_close_declaration[]="\n};\n\n";

//
const char devconf_bottom_header[]="pdev_map_t const * pdev_lst=&dev_lst[0];\n\
const char max_dev = sizeof(dev_lst)/sizeof(pdev_map_t);\n\
/*===========================================\n\
Implementation\n\
=============================================*/\n\n\n\
/*===========================================\n\
| End of Source : dev_mkconf.c\n\
|--------------------------------------------\n\
| Historic:\n\
|--------------------------------------------\n\
| Authors	| Date	| Comments\n\
|--------------------------------------------\n\
=============================================*/\n";

struct mkdev_t* pdev_lst_end=0;
struct mkdev_t* pdev_lst_start=0;


//binaries configuration
FILE* f_binconf  = NULL;
const char dflt_bin_conf_filepath[] = "bin_mkconf.c";
char * bin_conf_filepath = (char*)dflt_bin_conf_filepath; //see xml_elmt_mklepton()
#define MK_BINCONF_FILEPATH   bin_conf_filepath   

const char binconf_top_header[]=
"/*-------------------------------------------\n\
| Copyright(C) 2008 CHAUVIN-ARNOUX\n\
---------------------------------------------\n\
| Project:\n\
| Project Manager:\n\
| Source: mklepton.c\n\
| Path:mkleptux\n\
| Authors:\n\
| Plateform:GNU/Linux\n\
| Created:\n\
| Revision/Date:	$Revision: 1.3 $ $Date: 2009-08-28 09:56:38 $\n\
| Description:\n\
---------------------------------------------*/\n\n\n";

const char binconf_include_header[]=
"/*===========================================\n\
Includes\n\
=============================================*/\n";

const char binconf_globaldecl_header[]=
"\n\n/*===========================================\n\
Global Declaration\n\
=============================================*/\n";

const char binconf_implementation_header[]=
"\n\n/*===========================================\n\
Implementation\n\
=============================================*/\n";

const char binconf_bottom_header[]=
"\n/*===========================================\n\
| End of Source : bin_mkconf.c\n\
|--------------------------------------------\n\
| Historic:\n\
|--------------------------------------------\n\
| Authors	| Date	| Comments\n\
|--------------------------------------------\n\
=============================================*/\n";

const char binconf_decl_bin_lst[]=
"const int bin_lst_size   = sizeof(_bin_lst)/sizeof(bin_t);\n\
const bin_t* bin_lst = &_bin_lst[0];\n";


static char* bin_src_path  =0;
static char* bin_dest_path =0;
static int   bin_index     =0;


struct mkbin_t* pbin_lst_end=0;
struct mkbin_t* pbin_lst_start=0;

//disk image configuration
const char pattern_start_c[]="const unsigned char filecpu_memory[]={";
const char pattern_end_c[]="0x00};\r\n\r\n//\r\nconst unsigned long filecpu_memory_size = sizeof(filecpu_memory);\n\n";

const char fname_src[]=".fsflash.o";
//const char fname_dest[]="dev_dskimg.c";
const char dflt_dskimg_conf_filepath[] = "dev_dskimg.c";
char * dskimg_conf_filepath = (char*)dflt_dskimg_conf_filepath; //see xml_elmt_mklepton()
#define MK_DSKIMG_FILEPATH   dskimg_conf_filepath   

const char dflt_dskimg_conf_header_filepath[] = "dev_dskimg.h";
char * dskimg_conf_header_filepath = (char*)dflt_dskimg_conf_header_filepath; //see xml_elmt_mklepton()
#define MK_DSKIMG_HEADER_FILEPATH   dskimg_conf_header_filepath 

const char dskimg_top_header[]=
"/*===========================================\n\
Compiler Directive\n\
=============================================*/\n\
#ifndef _DEV_DSKIMG_H\n\
#define _DEV_DSKIMG_H\n\
\n\
\n";

const char dskimg_include_header[]=
"\n/*===========================================\n\
Includes\n\
=============================================*/\n";

const char dskimg_decl_header[]=
"\n\n/*===========================================\n\
Declaration\n\
=============================================*/\n";

const char dskimg_bottom_header[]=
"\n\n\
#endif\n\
\n\
/*===========================================\n\
End of Source dev_dskimg.h\n\
=============================================*/\n\n\n";

//
static int fsrc=-1;
static int fdest=-1;


//file
struct mkfile_t* pfile_lst_end=0;
struct mkfile_t* pfile_lst_start=0;



//expat var
unsigned int opt=0;

#define _BUF_SIZE 1024
unsigned char buf_src[_BUF_SIZE];
char buf_dest[20];

int size=0;

//
#define BUFFSIZE        8192
char Buff[BUFFSIZE];
int Depth;
//
#define __kernel_dev_gettime(__desc,__buf,__size)	__wrpr_kernel_dev_gettime(__desc,__buf,__size) 

//directories
struct mkdir_t * pdir_lst_end=NULL;
struct mkdir_t * pdir_lst_start=NULL;

/*============================================
| Implementation
==============================================*/

/*-------------------------------------------
| Name:xml_elmt_start_kernel
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int xml_elmt_start_kernel(const char **attr){

   if(!(f_kernelconf = fopen(MK_KERNELCONF_FILEPATH,"w+"))){
      printf("error! cannot open files %s\n",MK_KERNELCONF_FILEPATH);
      exit(0);
   }


   //
   fprintf(f_kernelconf,"//generated from %s by mklepton\n",xml_mk_file);
   //top header
   fwrite(kernelconf_top_header,sizeof(char),strlen(kernelconf_top_header),f_kernelconf);
   printf("make kernel:%s ...\n",MK_KERNELCONF_FILEPATH);

   //default value
   g_kernel_conf.cpufs_size = 16000;
   g_kernel_conf.cpufs_blocksz = 32;

   return 0;
}

/*-------------------------------------------
| Name:xml_elmt_end_kernel
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int xml_elmt_end_kernel(void){
   //char buf[255]={0};
   
   //include
   //#include dev_diskimg.h
   //fprintf(f_kernelconf,"#include \"%s\" \n",MK_DSKIMG_HEADER_FILEPATH);
	fprintf(f_kernelconf,"#include \"dev_dskimg.h\" \n");

   //declaration header
   fwrite(kernelconf_decl_header,sizeof(char),strlen(kernelconf_decl_header),f_kernelconf);

   //
   if(g_kernel_conf.str_cpu_type){
      fprintf(f_kernelconf,"//#define %s \n\n",cpu_type_list[g_kernel_conf.cpu_type]);
      //fprintf(f_kernelconf,"#define %s \n\n",cpu_type_list[g_kernel_conf.cpu_type]);
   }else{
      fprintf(f_kernelconf,"//#define %s \n\n",cpu_type_list[CPU_TYPE_GNU32]);
      //fprintf(f_kernelconf,"#define %s \n\n",cpu_type_list[CPU_TYPE_GNU32]);
   }

   if(g_kernel_conf.cpu_freq
      && g_kernel_conf.str_cpu_freq){
      fprintf(f_kernelconf,"%s %ldL\n\n",kernelconf_cpu_freq_define,g_kernel_conf.cpu_freq);
   }
   
   //
   if(g_kernel_conf.heap_size 
      && g_kernel_conf.str_heap_size){
      fprintf(f_kernelconf,"%s %d\n\n",kernelconf_heap_define,g_kernel_conf.heap_size);
   }
   //
   if(   (g_kernel_conf.thread_max && g_kernel_conf.str_thread_max)
      || (g_kernel_conf.process_max && g_kernel_conf.str_process_max)){
      //check values
      if(!g_kernel_conf.thread_max && g_kernel_conf.process_max){
         if(g_kernel_conf.network_used)
            g_kernel_conf.thread_max = g_kernel_conf.process_max+2; //(kernel thread + ip stack thread)
         else
            g_kernel_conf.thread_max = g_kernel_conf.process_max+1; //(kernel thread)
      }else if(g_kernel_conf.thread_max && !g_kernel_conf.process_max){
         if(g_kernel_conf.network_used)
            g_kernel_conf.process_max = g_kernel_conf.thread_max-2; //(kernel thread + ip stack thread)
         else
            g_kernel_conf.process_max = g_kernel_conf.thread_max-1; //(kernel thread)
      }else{
         if(g_kernel_conf.network_used){
            if(g_kernel_conf.process_max>(g_kernel_conf.thread_max-2))
               perror("\nerror: too many process\n");
         }else{
            if(g_kernel_conf.process_max>(g_kernel_conf.thread_max-1))
               perror("\nerror: too many process\n");
         }
      }
      //
      fprintf(f_kernelconf,"%s %d\n\n",kernelconf_thread_define,g_kernel_conf.thread_max);
      //
      fprintf(f_kernelconf,"%s %d\n\n",kernelconf_process_define,g_kernel_conf.process_max);
   }else{
      perror("\nerror: thread max or process max must be set\n");
   }
   //
   if(g_kernel_conf.openfiles_max && g_kernel_conf.str_openfiles_max){
      fprintf(f_kernelconf,"%s %d\n\n",kernelconf_openfiles_define,g_kernel_conf.openfiles_max);
   }
   //
   if(g_kernel_conf.descriptors_max && g_kernel_conf.str_descriptors_max){
      fprintf(f_kernelconf,"%s %d\n\n",kernelconf_descriptors_define,g_kernel_conf.descriptors_max);
   }
   //
   if(g_kernel_conf.str_env_path){
      char *token;
      fprintf(f_kernelconf,"%s {",kernelconf_env_path_define);
      token = strtok(g_kernel_conf.str_env_path,";");
      while( token != NULL )
      {
         /* While there are tokens in "string" */
         fprintf(f_kernelconf,"\"%s\"",token);
         /* Get next token: */
         if((token = strtok( NULL,";")))
            fprintf(f_kernelconf,",");
      }
      fprintf(f_kernelconf,"}\n\n");
   }
   //
   if(g_kernel_conf.network_used && g_kernel_conf.str_network_used){
      fprintf(f_kernelconf,"%s\n\n",kernelconf_network_define);
   }


   //bottom header
   fwrite(kernelconf_bottom_header,sizeof(char),strlen(kernelconf_bottom_header),f_kernelconf);

   fclose(f_kernelconf);

   printf("make kernel:%s ok!\n",MK_KERNELCONF_FILEPATH);
   
   return 0;
}

/*-------------------------------------------
| Name:xml_elmt_kernel_cpu
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int xml_elmt_kernel_cpu(const char **attr){

   int i;
   char* attr_name;
   char* attr_val;

   for (i = 0; attr[i]; i += 2) {
      attr_name = (char*)attr[i];
      attr_val  = (char*)attr[i + 1];
      if(!attr_name || !attr_val) continue;
      if(!strcasecmp(attr_name,XML_TAG_ATTR_FREQ)){
         g_kernel_conf.str_cpu_freq =strdup(attr_val);
         //modif since support sma7se. 29/07/2007
         g_kernel_conf.cpu_freq = atol(attr_val);
         if( !(g_kernel_conf.cpu_freq/1000000L) )
            g_kernel_conf.cpu_freq = atol(attr_val)*1000000L;//define in MHz in xml else define in Hz.
        
         if(g_kernel_conf.cpu_freq<=0)
            return -1;
      }else if(!strcasecmp(attr_name,XML_TAG_ATTR_TYPE)){
         cpu_type_enum_t cpu_type_enum=CPU_TYPE_GNU32;
         
         g_kernel_conf.str_cpu_type =strdup(attr_val);
         while(cpu_type_list[cpu_type_enum]){
            if(!strcasecmp(g_kernel_conf.str_cpu_type,cpu_type_list[cpu_type_enum]))
               break;
            cpu_type_enum++;
         }
         g_kernel_conf.cpu_type = cpu_type_enum;
      }else 
         return -1;
   }
   
   return 0;
}
/*-------------------------------------------
| Name:xml_elmt_kernel_heap
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int xml_elmt_kernel_heap(const char **attr){

   int i;
   char* attr_name;
   char* attr_val;

   for (i = 0; attr[i]; i += 2) {
      attr_name = (char*)attr[i];
      attr_val  = (char*)attr[i + 1];
      if(!attr_name || !attr_val) continue;
      if(!strcasecmp(attr_name,XML_TAG_ATTR_SIZE)){
         g_kernel_conf.str_heap_size =strdup(attr_val);
         g_kernel_conf.heap_size = atoi(attr_val);
      }else 
         return -1;
   }
   
   return 0;
}

/*-------------------------------------------
| Name:xml_elmt_kernel_thread
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int xml_elmt_kernel_thread(const char **attr){

   int i;
   char* attr_name;
   char* attr_val;
   
   for (i = 0; attr[i]; i += 2) {
      attr_name = (char*)attr[i];
      attr_val  = (char*)attr[i + 1];
      if(!attr_name || !attr_val) continue;
      if(!strcasecmp(attr_name,XML_TAG_ATTR_MAX)){
         g_kernel_conf.str_thread_max =strdup(attr_val);
         g_kernel_conf.thread_max = atoi(attr_val);
      }else 
         return -1;
   }
   
   return 0;
}

/*-------------------------------------------
| Name:xml_elmt_kernel_process
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int xml_elmt_kernel_process(const char **attr){

   int i;
   char* attr_name;
   char* attr_val;
   
   for (i = 0; attr[i]; i += 2) {
      attr_name = (char*)attr[i];
      attr_val  = (char*)attr[i + 1];
      if(!attr_name || !attr_val) continue;
      if(!strcasecmp(attr_name,XML_TAG_ATTR_MAX)){
         g_kernel_conf.str_process_max =strdup(attr_val);
         g_kernel_conf.process_max = atoi(attr_val);
      }else 
         return -1;
   }
   
   return 0;
}

/*-------------------------------------------
| Name:xml_elmt_kernel_openfiles
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int xml_elmt_kernel_openfiles(const char **attr){

   int i;
   char* attr_name;
   char* attr_val;
   
   for (i = 0; attr[i]; i += 2) {
      attr_name = (char*)attr[i];
      attr_val  = (char*)attr[i + 1];
      if(!attr_name || !attr_val) continue;
      if(!strcasecmp(attr_name,XML_TAG_ATTR_MAX)){
         g_kernel_conf.str_openfiles_max =strdup(attr_val);
         g_kernel_conf.openfiles_max = atoi(attr_val);
      }else 
         return -1;
   }
   
   return 0;
}

/*-------------------------------------------
| Name:xml_elmt_kernel_descriptors
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int xml_elmt_kernel_descriptors(const char **attr){

   int i;
   char* attr_name;
   char* attr_val;
   
   for (i = 0; attr[i]; i += 2) {
      attr_name = (char*)attr[i];
      attr_val  = (char*)attr[i + 1];
      if(!attr_name || !attr_val) continue;
      if(!strcasecmp(attr_name,XML_TAG_ATTR_MAX)){
         g_kernel_conf.str_descriptors_max=strdup(attr_val);
         g_kernel_conf.descriptors_max = atoi(attr_val);
      }else 
         return -1;
   }
   
   return 0;
}

/*-------------------------------------------
| Name:xml_elmt_kernel_cpufs
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int xml_elmt_kernel_cpufs(const char **attr){

   int i;
   char* attr_name;
   char* attr_val;
   
   for (i = 0; attr[i]; i += 2) {
      attr_name = (char*)attr[i];
      attr_val  = (char*)attr[i + 1];
      if(!attr_name || !attr_val) continue;
      if(!strcasecmp(attr_name,XML_TAG_ATTR_SIZE)){
         g_kernel_conf.cpufs_size=atol(attr_val);
      }else if(!strcasecmp(attr_name,XML_TAG_ATTR_NODE)){
         g_kernel_conf.cpufs_node=atoi(attr_val);
      }else if(!strcasecmp(attr_name,XML_TAG_ATTR_BLOCK)){
         g_kernel_conf.cpufs_block=atoi(attr_val);
      }else if(!strcasecmp(attr_name,XML_TAG_ATTR_BLOCKSZ)){
         g_kernel_conf.cpufs_blocksz=atoi(attr_val);
      }else if(!strcasecmp(attr_name,XML_TAG_ATTR_OPTION)){
         g_kernel_conf.str_cpufs_option=strdup(attr_val);
      }else
         return -1;
   }
   
   return 0;
}

/*-------------------------------------------
| Name:xml_elmt_kernel_env
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int xml_elmt_kernel_env(const char **attr){

   int i;
   char* attr_name;
   char* attr_val;
   
   for (i = 0; attr[i]; i += 2) {
      attr_name = (char*)attr[i];
      attr_val  = (char*)attr[i + 1];
      if(!attr_name || !attr_val) continue;
      if(!strcasecmp(attr_name,XML_TAG_ATTR_PATH)){
         g_kernel_conf.str_env_path=strdup(attr_val);
      }else 
         return -1;
   }
   
   return 0;
}

/*-------------------------------------------
| Name:xml_elmt_kernel_network
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int xml_elmt_kernel_network(const char **attr){

   int i;
   char* attr_name;
   char* attr_val;
   
   for (i = 0; attr[i]; i += 2) {
      attr_name = (char*)attr[i];
      attr_val  = (char*)attr[i + 1];
      if(!attr_name || !attr_val) continue;
      if(!strcasecmp(attr_name,XML_TAG_ATTR_USE)){
         g_kernel_conf.str_network_used =strdup(attr_val);
         if(!strcasecmp(attr_val,"on"))
            g_kernel_conf.network_used = 1;
         else
            g_kernel_conf.network_used = 0;
      }else 
         return -1;
   }
   
   return 0;
}

/*-------------------------------------------
| Name:xml_elmt_start_boot
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int xml_elmt_start_boot(const char **attr){

   int i;
   char* attr_name;
   char* attr_val;
   
   printf("read xml boot...\n");

  //malloc(30);
  for (i = 0; attr[i]; i += 2) {
      attr_name = (char*)attr[i];
      attr_val  = (char*)attr[i + 1];
      if(!attr_name || !attr_val) continue;
      

      if(!strcasecmp(attr_name,XML_TAG_ATTR_DESTPATH)){
         //boot
         char* path=malloc(strlen(attr_val)+strlen(boot_filepath)+3);
      	sprintf(path,"%s/%s",attr_val,boot_filepath);
      	
      	path[strlen(path)+1] = '\0';
         boot_filepath = path;
      }else if(!strcasecmp(attr_name,XML_TAG_ATTR_DEV)){
         //multi boot devices
         boot_dev = strdup(attr_val);
      }else if(!strcasecmp(attr_name,XML_TAG_ATTR_DELAY)){
         //boot delay in ms
         boot_delay = strdup(attr_val);
      }
   }
   return 0;
}

/*-------------------------------------------
| Name:xml_elmt_end_boot
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int xml_elmt_end_boot(void){
   printf("read xml boot:ok!\n");
   return 0;
}

/*-------------------------------------------
| Name:xml_elmt_kernel_heap
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int xml_elmt_boot_command(const char **attr){

   int i;
   char* attr_name;
   char* attr_val;

   char* arg   = 0;
   char* kb_val   = 0;
   //char* src_file    = 0;
   //char* dest_path   = 0;

   struct boot_t* pboot=0;
     
   for (i = 0; attr[i]; i += 2) {
      attr_name = (char*)attr[i];
      attr_val  = (char*)attr[i + 1];
      if(!attr_name || !attr_val) continue;
      if(!strcasecmp(attr_name,XML_TAG_ATTR_ARG)){
         arg=strdup(attr_val);
      }else if(!strcasecmp(attr_name,XML_TAG_ATTR_VALUE)){
         kb_val=strdup(attr_val);
      }
   }

   //
   if(!arg)
      return -1;

   pboot = malloc(sizeof(struct boot_t));
   //
   pboot->arg     = arg;
   if(kb_val)
      pboot->kb_val  = kb_val;
   else
      pboot->kb_val  = strdup("0x00");

   //
   if(pboot_lst_end)
      pboot_lst_end->pprev   = pboot;
   if(!pboot_lst_end)
      pboot_lst_start = pboot;

   pboot->pnext       = pboot_lst_end;
   pboot->pprev       = 0;

   pboot_lst_end = pboot;
   return 0;
}

/*-------------------------------------------
| Name:xml_elmt_start_mount
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int xml_elmt_start_mount(const char **attr){

   int i;
   char* attr_name;
   char* attr_val;
   
   printf("read xml mount...\n");

   //malloc(30);
   for (i = 0; attr[i]; i += 2) {
      attr_name = (char*)attr[i];
      attr_val  = (char*)attr[i + 1];
      if(!attr_name || !attr_val) continue;

      if(!strcasecmp(attr_name,XML_TAG_ATTR_DESTPATH)){
         //boot
         char* path =malloc(strlen(attr_val)+strlen(mount_filepath)+3);
         sprintf(path,"%s/%s",attr_val,mount_filepath);
         
         path[strlen(path)+1] = '\0';
         mount_filepath = path;
      }
   }
   return 0;
}

/*-------------------------------------------
| Name:xml_elmt_end_mount
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int xml_elmt_end_mount(void){
   printf("read xml mount:ok!\n");
   return 0;
}

/*-------------------------------------------
| Name:xml_elmt_mount_command
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int xml_elmt_mount_command(const char **attr){

   int i;
   char* attr_name;
   char* attr_val;

   char* fstype      = 0;
   char* dev         = 0;
   char* mount_point = 0;
   
   struct mount_t* pmount=0;
     
   for (i = 0; attr[i]; i += 2) {
      attr_name = (char*)attr[i];
      attr_val  = (char*)attr[i + 1];
      if(!attr_name || !attr_val) continue;
      if(!strcasecmp(attr_name,XML_TAG_ATTR_TYPE)){
         fstype=strdup(attr_val);
      }else if(!strcasecmp(attr_name,XML_TAG_ATTR_DEV)){
         dev=strdup(attr_val);
      }else if(!strcasecmp(attr_name,XML_TAG_ATTR_POINT)){
         mount_point=strdup(attr_val);
      }
   }

   //
   if(!fstype 
      ||!dev
      ||!mount_point)
      return -1;

   pmount = malloc(sizeof(struct mount_t));
   //
   pmount->fstype       = fstype;
   pmount->dev          = dev;
   pmount->mount_point  = mount_point;
   
   //
   if(pmount_lst_end)
      pmount_lst_end->pprev   = pmount;
   if(!pmount_lst_end)
      pmount_lst_start = pmount;

   pmount->pnext       = pmount_lst_end;
   pmount->pprev       = 0;

   pmount_lst_end = pmount;
   return 0;
}


/*-------------------------------------------
| Name:xml_elmt_start_devices
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int xml_elmt_start_devices(const char **attr){

   if(!(f_devconf = fopen(MK_DEVCONF_FILEPATH,"w+")))
      return -1;

   //top header
   fwrite(devconf_top_header,sizeof(char),strlen(devconf_top_header),f_devconf);
   printf("make devices:%s ...\n",MK_DEVCONF_FILEPATH);

   return 0;
}

/*-------------------------------------------
| Name:xml_elmt_start_devices
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int xml_elmt_end_devices(void){

   if(!f_devconf)
      return -1;

   //bottom header
   fwrite(devconf_bottom_header,sizeof(char),strlen(devconf_bottom_header),f_devconf);

   fclose(f_devconf);

   printf("make devices:%s ok!\n",MK_DEVCONF_FILEPATH);
   
   return 0;
}

/*-------------------------------------------
| Name:xml_elmt_dev
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int xml_elmt_dev(const char **attr){
   int i;
   char* attr_name;
   char* attr_val;
     
   char* dev_name = NULL;

   struct mkdev_t* pdev;
   
   if(!f_devconf)
      return -1;
     
   for (i = 0; attr[i]; i += 2) {
      attr_name = (char*)attr[i];
      attr_val  = (char*)attr[i + 1];
      if(!attr_name || !attr_val) continue;

      if(!strcasecmp(attr_name,XML_TAG_ATTR_NAME))
         dev_name = attr_val;
      else if(!strcasecmp(attr_name,XML_TAG_ATTR_USE)){
         if(!strcasecmp(attr_val,XML_DEV_VALUE_ON)){
            //insert dev;
            pdev = malloc(sizeof(struct mkdev_t));
            pdev->dev_map_name = strdup(dev_name);
            //
            if(pdev_lst_end)
               pdev_lst_end->pprev = pdev;
            if(!pdev_lst_end)
               pdev_lst_start = pdev;

            pdev->pnext       = pdev_lst_end;
            pdev->pprev       = 0;

            pdev_lst_end = pdev;
            printf("use devices %s\n",dev_name);
         }
      }
   }

   
   return 0;
}

/*-------------------------------------------
| Name:xml_elmt_start_binaries
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int xml_elmt_start_binaries(const char **attr){
   int i;
   char* attr_name;
   char* attr_val;
   
   printf("read xml binaries:%s ...\n",MK_BINCONF_FILEPATH);

   for (i = 0; attr[i]; i += 2) {
      attr_name = (char*)attr[i];
      attr_val  = (char*)attr[i + 1];
      if(!attr_name || !attr_val) continue;
      if(!strcasecmp(attr_name,XML_TAG_ATTR_SRCPATH))
         bin_src_path = strdup(attr_val);
      else if(!strcasecmp(attr_name,XML_TAG_ATTR_DESTPATH))
         bin_dest_path = strdup(attr_val);
   }

   return 0;
}

/*-------------------------------------------
| Name:xml_elmt_end_binaries
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int xml_elmt_end_binaries(void){
   
   printf("read xml binaries:%s ok!\n",MK_BINCONF_FILEPATH);
   
   return 0;
}

/*-------------------------------------------
| Name:xml_elmt_bin
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int xml_elmt_bin(const char **attr){

   int i;
   char* attr_name;
   char* attr_val;

   char* bin_name    = 0;
   char* stack       = 0;
   char* priority    = 0; 
   char* timeslice   = 0; 

   struct mkbin_t* pbin;
     
   for (i = 0; attr[i]; i += 2) {
      attr_name = (char*)attr[i];
      attr_val  = (char*)attr[i + 1];
      if(!attr_name || !attr_val) continue;

      if(!strcasecmp(attr_name,XML_TAG_ATTR_NAME))
         bin_name = attr_val;
      else if(!strcasecmp(attr_name,XML_TAG_ATTR_STACK))
         stack = attr_val;
      else if(!strcasecmp(attr_name,XML_TAG_ATTR_PRIORITY))
         priority = attr_val;
      else if(!strcasecmp(attr_name,XML_TAG_ATTR_TIMESLICE))
         timeslice = attr_val;
   }

   if(!bin_name
      || !stack
      || !priority
      || !timeslice)
      return -1;

   //insert bin;
   pbin = malloc(sizeof(struct mkbin_t));

   pbin->bin_index   = bin_index;
   pbin->src_path    = strdup(bin_src_path);
   pbin->dest_path   = strdup(bin_dest_path);
   pbin->bin_name    = strdup(bin_name);
   pbin->stack       = strdup(stack);
   pbin->priority    = strdup(priority);
   pbin->timeslice   = strdup(timeslice);

   if(pbin_lst_end)
      pbin_lst_end->pprev = pbin;
   if(!pbin_lst_end)
      pbin_lst_start = pbin;

   pbin->pnext       = pbin_lst_end;
   pbin->pprev       = 0;

   pbin_lst_end = pbin;

   bin_index++;
   

   return 0;
}

/*-------------------------------------------
| Name:xml_elmt_files
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int xml_elmt_files(const char **attr){
   return 0;
}

/*-------------------------------------------
| Name:xml_elmt_file
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int xml_elmt_file(const char **attr){
   int i;
   char* attr_name;
   char* attr_val;

   char* file_name   = 0;
   char* src_file    = 0;
   char* dest_path   = 0;

   struct mkfile_t* pfile=0;
     
   for (i = 0; attr[i]; i += 2) {
      attr_name = (char*)attr[i];
      attr_val  = (char*)attr[i + 1];
      if(!attr_name || !attr_val) continue;
      if(!strcasecmp(attr_name,XML_TAG_ATTR_NAME))
         file_name = attr_val;
      else if(!strcasecmp(attr_name,XML_TAG_ATTR_SRCFILE))
         src_file = attr_val;
      else if(!strcasecmp(attr_name,XML_TAG_ATTR_DESTPATH))
         dest_path = attr_val;
      
   }

   if(!file_name || !src_file || !dest_path)
      return -1;

   pfile = malloc(sizeof(struct mkfile_t));

   pfile->file_name  = strdup(file_name);
   pfile->src_file   = strdup(src_file);
   pfile->dest_path  = strdup(dest_path);

   if(pfile_lst_end)
      pfile_lst_end->pprev   = pfile;
   if(!pfile_lst_end)
      pfile_lst_start = pfile;

   pfile->pnext       = pfile_lst_end;
   pfile->pprev       = 0;

   pfile_lst_end = pfile;

   return 0;
}

/*-------------------------------------------
| Name:xml_elmt_mklepton
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int xml_elmt_mklepton(const char **attr){
   int i;
   char* attr_name;
   char* attr_val;
   
   for (i = 0; attr[i]; i += 2) {
      attr_name = (char*)attr[i];
      attr_val  = (char*)attr[i + 1];
      if(!attr_name || !attr_val) continue;
      if(!strcasecmp(attr_name,XML_TAG_ATTR_DESTPATH)){
         char* path;
         //kernel
         path =malloc(strlen(attr_val)+strlen(kernel_conf_filepath)+1);
         sprintf(path,"%s/%s",attr_val,kernel_conf_filepath);
         kernel_conf_filepath = path;
         //dev
         path =malloc(strlen(attr_val)+strlen(dev_conf_filepath)+1);
         sprintf(path,"%s/%s",attr_val,dev_conf_filepath);
         dev_conf_filepath = path;
         //bin
         path =malloc(strlen(attr_val)+strlen(bin_conf_filepath)+1);
         sprintf(path,"%s/%s",attr_val,bin_conf_filepath);
         bin_conf_filepath = path;
         //disk image
         //source file .c
         path =malloc(strlen(attr_val)+strlen(dskimg_conf_filepath)+1);
         sprintf(path,"%s/%s",attr_val,dskimg_conf_filepath);
         dskimg_conf_filepath = strdup(path);
         //header file .h
         free(path);
         path =malloc(strlen(attr_val)+strlen(dskimg_conf_header_filepath)+1);
         sprintf(path,"%s/%s",attr_val,dskimg_conf_header_filepath);
         dskimg_conf_header_filepath = strdup(path);
      }
   }

   return 0;
}

/*-------------------------------------------
| Name:xml_elmt_mklepton
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int xml_elmt_arch(const char **attr){
   int i;
   char* attr_name;
   char* attr_val;
   
   for (i = 0; attr[i]; i += 2) {
      attr_name = (char*)attr[i];
      attr_val  = (char*)attr[i + 1];
      if(!attr_name || !attr_val) continue;
      if(!strcasecmp(attr_name,XML_TAG_ATTR_DESTPATH)){
         char* path;
         //kernel
         path =malloc(strlen(attr_val)+strlen(kernel_conf_filepath)+3);
         sprintf(path,"%s/%s",attr_val,kernel_conf_filepath);
         path[strlen(path)+1] = '\0';
         kernel_conf_filepath = path;
         //dev
         path =malloc(strlen(attr_val)+strlen(dev_conf_filepath)+3);
         sprintf(path,"%s/%s",attr_val,dev_conf_filepath);
         path[strlen(path)+1] = '\0';
         dev_conf_filepath = path;
         //bin
         path =malloc(strlen(attr_val)+strlen(bin_conf_filepath)+3);
         sprintf(path,"%s/%s",attr_val,bin_conf_filepath);
         path[strlen(path)+1] = '\0';
         bin_conf_filepath = path;
         //disk image
         //source file .c
         path =malloc(strlen(attr_val)+strlen(dskimg_conf_filepath)+3);
         sprintf(path,"%s/%s",attr_val,dskimg_conf_filepath);
         path[strlen(path)+1] = '\0';
         dskimg_conf_filepath = strdup(path);
         //header file .h
         path =malloc(strlen(attr_val)+strlen(dskimg_conf_header_filepath)+3);
         sprintf(path,"%s/%s",attr_val,dskimg_conf_header_filepath);
         path[strlen(path)+1] = '\0';
         dskimg_conf_header_filepath = strdup(path);
      }
   }

   return 0;
}

/*-------------------------------------------
| Name:xml_elmt_start_target
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int xml_elmt_start_target(const char **attr){
   int i;
   char* attr_name;
   char* attr_val;

   for (i = 0; attr[i]; i += 2) {
      attr_name = (char*)attr[i];
      attr_val  = (char*)attr[i + 1];
      if(!attr_name || !attr_val) continue;

      if(!strcasecmp(attr_name,XML_TAG_ATTR_NAME)){
         xml_current_target=strdup(attr_val);
         printf("xml target=%s\n",xml_current_target);
      }
   }
   return 0;
}

/*-------------------------------------------
| Name:xml_elmt_end_target
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int xml_elmt_end_target(void){
   
   if(xml_current_target)
      free(xml_current_target);
   xml_current_target=(char*)0;
   printf("xml end target: ok!\n");
   return 0;
}

/*-------------------------------------------
| Name:xml_elmt_dirs
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int xml_elmt_dirs(const char **attr) {
   return 0;
}

/*-------------------------------------------
| Name:xml_elmt_dir
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int xml_elmt_dir(const char **attr) {
   int i;
   char* attr_name;
   char* attr_val;

   char* src_path    = 0;
   char* dest_path   = 0;

   struct mkdir_t* pdir=0;

   for (i = 0; attr[i]; i += 2) {
      attr_name = (char*)attr[i];
      attr_val  = (char*)attr[i + 1];
      if(!attr_name || !attr_val) continue;
      if(!strcasecmp(attr_name,XML_TAG_ATTR_SRCPATH))
         src_path = attr_val;
      else if(!strcasecmp(attr_name,XML_TAG_ATTR_DESTPATH))
         dest_path = attr_val;
   }

   if(!src_path || !dest_path)
      return -1;

   pdir = malloc(sizeof(struct mkdir_t));

   pdir->src_path   = strdup(src_path);
   pdir->dest_path  = strdup(dest_path);

   if(pdir_lst_end)
      pdir_lst_end->pprev   = pdir;
   if(!pdir_lst_end)
      pdir_lst_start = pdir;

   pdir->pnext       = pdir_lst_end;
   pdir->pprev       = 0;

   pdir_lst_end = pdir;

   return 0;
}

/*-------------------------------------------
| Name:start
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static void start(void *data, const char *el, const char **attr){
   int error=0;
   
   if(!strcasecmp(el,XML_TAG_ELMT_TARGET)){
     error=xml_elmt_start_target(attr);
   }else {
      //target filter
      if(   mk_current_target
         && xml_current_target
         && strcasecmp(xml_current_target,mk_current_target))
         return;

      if(!mk_current_target && xml_current_target)
         return;//filter not set
      //
      if(!strcasecmp(el,XML_TAG_ELMT_MKLEPTON)){
         error=xml_elmt_mklepton(attr);
      }else if(!strcasecmp(el,XML_TAG_ELMT_ARCH)){
         error=xml_elmt_arch(attr);
      }else if(!strcasecmp(el,XML_TAG_ELMT_KERNEL) && (opt&OPT_MSK_K) ){
         error=xml_elmt_start_kernel(attr);
      }else if(!strcasecmp(el,XML_TAG_ELMT_CPU) && (opt&OPT_MSK_K)){
         error=xml_elmt_kernel_cpu(attr);
      }else if(!strcasecmp(el,XML_TAG_ELMT_HEAP) && (opt&OPT_MSK_K)){
         error=xml_elmt_kernel_heap(attr);
      }else if(!strcasecmp(el,XML_TAG_ELMT_THREAD) && (opt&OPT_MSK_K)){
        error=xml_elmt_kernel_thread(attr);
      }else if(!strcasecmp(el,XML_TAG_ELMT_PROCESS) && (opt&OPT_MSK_K)){
        error=xml_elmt_kernel_process(attr);
      }else if(!strcasecmp(el,XML_TAG_ELMT_OPENFILES) && (opt&OPT_MSK_K)){
        error=xml_elmt_kernel_openfiles(attr);
      }else if(!strcasecmp(el,XML_TAG_ELMT_DESCRIPTORS) && (opt&OPT_MSK_K)){
        error=xml_elmt_kernel_descriptors(attr);
      }else if(!strcasecmp(el,XML_TAG_ELMT_CPUFS) && (opt&OPT_MSK_K)){
        error=xml_elmt_kernel_cpufs(attr);
      }else if(!strcasecmp(el,XML_TAG_ELMT_ENV) && (opt&OPT_MSK_K)){
        error=xml_elmt_kernel_env(attr);
      }else if(!strcasecmp(el,XML_TAG_ELMT_NETWORK) && (opt&OPT_MSK_K)){
        error=xml_elmt_kernel_network(attr);
      }else if(!strcasecmp(el,XML_TAG_ELMT_BOOT) && (opt&OPT_MSK_K) ){
        error=xml_elmt_start_boot(attr);
      }else if(!strcasecmp(el,XML_TAG_ELMT_MOUNT) && (opt&OPT_MSK_K) ){
        error=xml_elmt_start_mount(attr);
      }else if(!strcasecmp(el,XML_TAG_ELMT_COMMAND) && (opt&OPT_MSK_K) ){
        error=xml_elmt_boot_command(attr);
      }else if(!strcasecmp(el,XML_TAG_ELMT_DISK) && (opt&OPT_MSK_K) ){
        error=xml_elmt_mount_command(attr);
      }else if(!strcasecmp(el,XML_TAG_ELMT_OPENFILES) && (opt&OPT_MSK_K)){
        error=xml_elmt_kernel_openfiles(attr);
      }else if(!strcasecmp(el,XML_TAG_ELMT_DEVICES) && (opt&OPT_MSK_D) ){
        error=xml_elmt_start_devices(attr);
      }else if(!strcasecmp(el,XML_TAG_ELMT_DEV) && (opt&OPT_MSK_D)){
        error=xml_elmt_dev(attr);
      }else if(!strcasecmp(el,XML_TAG_ELMT_BINARIES) && (opt&OPT_MSK_B)){
        error=xml_elmt_start_binaries(attr);
      }else if(!strcasecmp(el,XML_TAG_ELMT_BIN) && (opt&OPT_MSK_B) ){
        error=xml_elmt_bin(attr);
      }else if(!strcasecmp(el,XML_TAG_ELMT_FILES) && (opt&OPT_MSK_F)){
        error=xml_elmt_files(attr);
      }else if(!strcasecmp(el,XML_TAG_ELMT_FILE) && (opt&OPT_MSK_F)){
        error=xml_elmt_file(attr);
      }
      //
      else if(!strcasecmp(el,XML_TAG_ELMT_DIRS) && (opt&OPT_MSK_F)){
         error=xml_elmt_dirs(attr);
      }
      else if(!strcasecmp(el,XML_TAG_ELMT_DIR) && (opt&OPT_MSK_F)){
         error=xml_elmt_dir(attr);
      }
   }

   if(error<0)
      fprintf(stderr,"\nerror: in %s\n",el);

   Depth++;
}

/*-------------------------------------------
| Name:end
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static void end(void *data, const char *el)
{
   Depth--;
   if(!strcasecmp(el,XML_TAG_ELMT_TARGET)){
     xml_elmt_end_target();
   }else {
      if(   mk_current_target
         && xml_current_target
         && strcasecmp(xml_current_target,mk_current_target))
         return;

      if(!mk_current_target && xml_current_target)
         return;//filter not set

      if(!strcasecmp(el,XML_TAG_ELMT_KERNEL) && (opt&OPT_MSK_K)){
         xml_elmt_end_kernel();
      }else if(!strcasecmp(el,XML_TAG_ELMT_DEVICES) && (opt&OPT_MSK_D)){
         xml_elmt_end_devices();
      }else if(!strcasecmp(el,XML_TAG_ELMT_BOOT) && (opt&OPT_MSK_K)){
         xml_elmt_end_boot();
      }else if(!strcasecmp(el,XML_TAG_ELMT_MOUNT) && (opt&OPT_MSK_K)){
         xml_elmt_end_mount();
      }else if(!strcasecmp(el,XML_TAG_ELMT_DEV) && (opt&OPT_MSK_D)){
      }else if(!strcasecmp(el,XML_TAG_ELMT_BINARIES) && (opt&OPT_MSK_B)){
         xml_elmt_end_binaries();
      }else if(!strcasecmp(el,XML_TAG_ELMT_BIN)){
      }else if(!strcasecmp(el,XML_TAG_ELMT_FILES)){
      }else if(!strcasecmp(el,XML_TAG_ELMT_FILE)){
      }
   }
}

/*-------------------------------------------
| Name:_mk_conf
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _mk_conf(char* file_conf){

   FILE *stream;

   XML_Parser p = XML_ParserCreate(NULL);

   if (! p) {
      fprintf(stderr, "Couldn't allocate memory for parser\n");
      exit(-1);
   }

   //
   if( (stream  = fopen( file_conf, "r" )) == 0 ){
      printf("error: cannot open xml file:%s\n",file_conf);
      return -1;
   }

   printf("open xml file:%s\n",file_conf);

   XML_SetElementHandler(p, start, end);

   for (;;) {
      int done;
      int len;

      len = fread(Buff, 1, BUFFSIZE, stream);
      if (ferror(stdin)) {
         fprintf(stderr, "Read error\n");
         exit(-1);
      }

      done = feof(stream);

      if (XML_Parse(p, Buff, len, done) == XML_STATUS_ERROR) {
         fprintf(stderr, "Parse error at line %d:\n%s\n",
               (int)XML_GetCurrentLineNumber(p),
               XML_ErrorString(XML_GetErrorCode(p)));
         exit(-1);
      }

      if (done)
         break;
   }

   fclose(stream);
   return 0;
}

/*-------------------------------------------
| Name:_mk_binaries
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _mk_binaries(void){
   
   struct mkbin_t* pbin = pbin_lst_start;
   struct _vfs_formatopt_t vfs_formatopt={0};
   struct _vfs_statvfs_st  statvfs={0};
   char current_path[MAX_PATH] = {0};
   char path[MAX_PATH] = {0};
   char ref[MAX_PATH] = {0};
   _vfs_desc_t desc;
   
   
   printf("make binaries:%s ...\n",MK_BINCONF_FILEPATH);

   //list all binaries;
   while(pbin){
      printf("(%d):%s/%s\n",pbin->bin_index,
                                 pbin->dest_path,
                                 pbin->bin_name);
      pbin = pbin->pprev;
   }

   //create fileflash dev
   desc=_vfs_open("/dev/hd/hdc",_O_RDWR,0);
   if(desc<0){
      printf("error! cannot open /dev/hd/hdc\n");
      return -1;
   }

   if(_vfs_ioctl(desc,HDSETSZ,g_kernel_conf.cpufs_size)<0){
      printf("error! cannot set size (%d bytes) on /dev/hd/hdc\n",(int)g_kernel_conf.cpufs_size);
      return -1;
   }

   //get size of device
   vfs_formatopt.dev_sz=g_kernel_conf.cpufs_size;
   if(_vfs_ioctl(desc,HDGETSZ,&vfs_formatopt.dev_sz)<0)
      vfs_formatopt.dev_sz=g_kernel_conf.cpufs_size;

   _vfs_close(desc);
   
   
   vfs_formatopt.max_blk   = g_kernel_conf.cpufs_size/g_kernel_conf.cpufs_blocksz; //500//16Ko;
   vfs_formatopt.max_node  = (g_kernel_conf.cpufs_node>0?g_kernel_conf.cpufs_node:120);//120
   vfs_formatopt.blk_sz    = g_kernel_conf.cpufs_blocksz;
   printf("makefs hdc\n");
   _vfs_makefs(fs_ufs,"/dev/hd/hdc",&vfs_formatopt);
   
   //mount /dev/hd/hdc on /usr.
   printf("mount /dev/hd/hdc on /usr\n");
   _vfs_mount(fs_ufs,"/dev/hd/hdc","/usr");

   if(!pbin_lst_start)
      return 0;

   //create files
   pbin = pbin_lst_start;
   while(pbin){
      exec_file_t exec_file;
      int cb = 0;

      //create new directory
      if(strcmp(current_path,pbin->dest_path)){
         strcpy(current_path,pbin->dest_path);
         strcpy(path,"/usr/");
         strcat(path,pbin->dest_path);
         printf("mkdir %s\n",path);
         if(_vfs_mkdir(path,0)<0)
            printf("%s already exist\n",path);
      }
  
      //create binary
      exec_file.signature  = EXEC_SIGNT;
      exec_file.index      = pbin->bin_index; 
      exec_file.priority   = atoi(pbin->priority);
      exec_file.stacksize  = atoi(pbin->stack);
      exec_file.timeslice  = atoi(pbin->timeslice);

      strcpy(ref,path);
      strcat(ref,"/");
      strcat(ref,pbin->bin_name);
      printf("creat %s\n",ref);
      desc= _vfs_open(ref,_O_CREAT|_O_WRONLY,0);
      cb=sizeof(exec_file);
      cb=_vfs_write(desc,(char*)&exec_file,cb);
      _vfs_close(desc);

      //
      pbin = pbin->pprev;
   }

   //check files
   current_path[0]='\0';
   pbin = pbin_lst_start;
   while(pbin){

      if(strcmp(current_path,pbin->dest_path)){
         strcpy(current_path,pbin->dest_path);
         strcpy(path,"/usr/");
         strcat(path,pbin->dest_path);
         printf("mkdir %s\n",path);
         _vfs_ls(path);
      }
  
      pbin = pbin->pprev;
   }

   //create c file for binary
   if(!(f_binconf = fopen(MK_BINCONF_FILEPATH,"w+")))
         return -1;

   //top header
   fwrite(binconf_top_header,sizeof(char),strlen(binconf_top_header),f_binconf);

   //include header
   fwrite(binconf_include_header,sizeof(char),strlen(binconf_include_header),f_binconf);
   pbin = pbin_lst_start;
   
   fprintf(f_binconf, "#include \"kernel/core/bin.h\"\n");
   
   while(pbin){
      //fprintf(f_binconf, "#include \"%s/%s.h\"\n",pbin->src_path,pbin->bin_name);
      fprintf(f_binconf, "//see \"%s/%s.c\"\n",pbin->src_path,pbin->bin_name);
      fprintf(f_binconf, "int %s_main(int argc, char* argv[]);\n\n",pbin->bin_name);
      pbin = pbin->pprev;
   }

   //global declaration header
   fwrite(binconf_globaldecl_header,sizeof(char),strlen(binconf_globaldecl_header),f_binconf);

   pbin = pbin_lst_start;
   fprintf(f_binconf, "static const bin_t _bin_lst[]={\n");
   while(pbin){
      char str_bin_buf[20];
      sprintf(str_bin_buf,"\"%s\"",pbin->bin_name);
                       //{"init",    init_main,     100,    512,        1},         
      fprintf(f_binconf,"{%16s,\t\t\t%22s_main,\t\t\t%.8s,\t\t\t%.8s,\t\t\t%.8s}",
                        str_bin_buf,pbin->bin_name,
                        pbin->priority,
                        pbin->stack,
                        pbin->timeslice);
      pbin = pbin->pprev;
      if(pbin)
         fprintf(f_binconf,",\n");
   }
   fprintf(f_binconf, "\n};\n\n");

   //bin list declaration header
   fwrite(binconf_decl_bin_lst,sizeof(char),strlen(binconf_decl_bin_lst),f_binconf);

   //implementation header
   fwrite(binconf_implementation_header,sizeof(char),strlen(binconf_implementation_header),f_binconf);

   //bottom header
   fwrite(binconf_bottom_header,sizeof(char),strlen(binconf_bottom_header),f_binconf);

   fclose(f_binconf);

   _vfs_statvfs("/usr",&statvfs );
   printf("/usr free block:%d free size=%d bytes\r\n",
                              statvfs.f_bfree,statvfs.f_bfree*statvfs.f_bsize);

   //unmount /dev/hd/hdc on /usr.
   printf("umount /usr\n");
   _vfs_umount("/usr");

   printf("make binaries:%s ok\n",MK_BINCONF_FILEPATH);

   return 0;
}

/*-------------------------------------------
| Name:_mk_file
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _mk_file(void){
   struct mkfile_t* pfile = pfile_lst_start;
   struct _vfs_statvfs_st  statvfs;
   char dest_path_buf[MAX_PATH];
   char* dest_path=(char*)&dest_path_buf;
   char current_path[MAX_PATH] = {0};
   char path[MAX_PATH] = {0};
   char ref[MAX_PATH] = {0};
   _vfs_desc_t desc;
   int fd;
   char fbuf[9];
   
   int err=0;

   if(!pfile_lst_start)
      return 0;

   printf("make files: ...\n");
   
   //mount /dev/hd/hdc on /usr.
   printf("mount /dev/hd/hdc on /usr\n");
   _vfs_mount(fs_ufs,"/dev/hd/hdc","/usr");

   //create files
   pfile = pfile_lst_start;
   while(pfile){
      int i =0;
      int cb = 0;

      //init
      strcpy(path,"/usr/");
      dest_path=(char*)&dest_path_buf;
      strcpy(dest_path,pfile->dest_path);

      //create new directory
      for(i=0;i<=strlen(dest_path);i++){
         int brk = 0;
         char token[MAX_PATH]={0};
         if(dest_path[i] != '/'
            && dest_path[i] != '\0') continue;

         if(!dest_path[i])
            brk=1;

         dest_path[i] = '\0';
         strcpy(token,dest_path);
         

         if(strcmp(current_path,token)){
            strcpy(current_path,token);
            strcat(path,token);
            printf("mkdir %s\n",path);
            if(_vfs_mkdir(path,0)<0){
               printf("warning! mkdir %s\n",path);
            }
         }

         if(brk)
            break;

         dest_path[i] = '/';
         dest_path = (char*)&dest_path[i];
         i=0;

      }
      
      //create file
      if( (fd = open(pfile->src_file,O_RDONLY,S_IREAD)) <0 ){
         printf("error! cannot open file:%s\n",pfile->src_file);
         pfile = pfile->pprev;
         err=-1;
         continue;
      }

      strcpy(ref,"/usr/");
      strcat(ref,pfile->dest_path);
      strcat(ref,"/");
      strcat(ref,pfile->file_name);
      printf("creat %s\n",ref);
      
      if((desc= _vfs_open(ref,_O_CREAT|_O_WRONLY,0))<0){
         printf("error! cannot open file:%s\n",ref);
         pfile = pfile->pprev;
         err=-1;
         continue;
      }

      while((cb=read(fd,fbuf,sizeof(fbuf)-1))>0){
         cb=_vfs_write(desc,fbuf,cb);
         if(cb<0){
            printf("error! cannot write file:%s\n",ref);
            pfile = pfile->pprev;
            err=-1;
            continue;
         }

      }

      close(fd);
      _vfs_close(desc);

      //
      pfile = pfile->pprev;
   }

   //check files
   current_path[0]='\0';
   pfile = pfile_lst_start;
   while(pfile){

      if(strcmp(current_path,pfile->dest_path)){
         strcpy(current_path,pfile->dest_path);
         strcpy(path,"/usr/");
         strcat(path,pfile->dest_path);
         printf("mkdir %s\n",path);
         _vfs_ls(path);
      }
  
      pfile = pfile->pprev;
   }

   //
   _vfs_statvfs("/usr",&statvfs );
   printf("/usr free block:%d free size=%d bytes\r\n",
                              statvfs.f_bfree,statvfs.f_bfree*statvfs.f_bsize);

   _vfs_ls("/usr/bin");
   //unmount /dev/hd/hdc on /usr.
   printf("umount /usr\n");
   _vfs_umount("/usr");

   //
   if(err<0)
      return -1;

   printf("make files ok\n");

   return 0;
}


/*-------------------------------------------
| Name:_mk_dir
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _mk_dir(void) {
   struct mkdir_t* pdir = pdir_lst_start;
   struct mkdir_t * pdir_tmp = NULL;
   DIR * dir = NULL;
   struct dirent * dirent = NULL;
   struct stat d_stat ={0};
   int host_fd=-1;
   char host_path[MAX_PATH] = {0};
   //
   char target_path[MAX_PATH];
   char* target_ppath=(char*)&target_path;
   _vfs_desc_t target_fd;
   char fbuf[256];
   //
   int err=0;
   int cb=0;

   if(!pdir_lst_start)
      return 0;

   printf("make dir: ...\n");

   //mount /dev/hd/hdc on /usr.
   printf("mount /dev/hd/hdc on /usr\n");
   _vfs_mount(fs_ufs,"/dev/hd/hdc","/usr");

   //create dir
   while(pdir){
      //open host dir
      if(!(dir = opendir(pdir->src_path))) {
         printf("can't open %s\n", pdir->src_path);
         pdir = pdir->pprev;
         continue;
      }

      //create target dir
      strcpy(target_ppath,"/usr/");
      strncat(target_ppath, pdir->dest_path, 32);
      printf("mkdir %s\n",target_ppath);
      if(_vfs_mkdir(target_ppath,0)<0){
         printf("warning! mkdir %s\n",target_ppath);
      }

      //read local dir
      while((dirent = readdir(dir))) {
         //skip . and ..
         if(!(strcasecmp(dirent->d_name,".")) || !(strcasecmp(dirent->d_name,".."))){
            continue;
         }

         //copy complete host path
         strncpy(host_path, pdir->src_path,MAX_PATH);
         strcat(host_path, "/");
         strncat(host_path, dirent->d_name, 32);

         //
         if(stat(host_path, &d_stat)<0) {
            break;
         }
         //simple file(s) case
         if(S_ISREG(d_stat.st_mode)) {
            //open it and copy local file(s) to remote file(s)
            host_fd = open(host_path, O_RDONLY, S_IREAD);
            //
            strcat(target_ppath, "/");
            strncat(target_ppath, dirent->d_name, 32);
            if((target_fd=_vfs_open(target_ppath,_O_CREAT|_O_WRONLY,0))<0){
               printf("can't open %s\n", target_ppath);
               err=-1;
               continue;
            }

            //copy file content
            while((cb=read(host_fd,fbuf,sizeof(fbuf)-1))>0){
               cb=_vfs_write(target_fd,fbuf,cb);
               if(cb<0){
                  printf("error! cannot write file:%s\n",target_ppath);
                  err=-1;
                  continue;
               }
            }

            close(host_fd);
            _vfs_close(target_fd);
            //
            target_ppath=dirname(target_path);
         }
         //directories case
         else if(S_ISDIR(d_stat.st_mode)) {
            //create a new mdir_t entry with previous value
            pdir_tmp = malloc(sizeof(struct mkdir_t));

            pdir_tmp->src_path   = strdup(host_path);

            strcat(target_ppath, "/");
            strncat(target_ppath, dirent->d_name, 32);
            pdir_tmp->dest_path  = strdup(target_ppath+strlen("/usr/"));

            if(pdir_lst_end)
               pdir_lst_end->pprev   = pdir_tmp;
            if(!pdir_lst_end)
               pdir_lst_start = pdir_tmp;

            pdir_tmp->pnext       = pdir_lst_end;
            pdir_tmp->pprev       = 0;

            pdir_lst_end = pdir_tmp;
            //
            target_ppath=dirname(target_path);

         }
      }

      closedir(dir);
      pdir = pdir->pprev;
   }

   //unmount /dev/hd/hdc on /usr.
   printf("umount /usr\n");
   _vfs_umount("/usr");

   //
   if(err<0)
      return -1;

   printf("make dir ok\n");

   return 0;
}


/*-------------------------------------------
| Name:_mk_boot
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _mk_boot(void){

   //int fd=-1;
   struct boot_t* pboot;

   if(!pboot_lst_start)
      return 0;
   //
   printf("make boot: ...\n");
   //create file
   if(!(f_bootconf = fopen(MK_BOOT_FILEPATH,"w+")) ){
      fprintf(stderr,"error! cannot create file :%s\n",MK_BOOT_FILEPATH);
      return -1;
   }

   fprintf(f_bootconf,"%s;\n",boot_dev);
   fprintf(f_bootconf,"%s;\n",boot_delay);
   //create .boot files
   pboot = pboot_lst_start;
   while(pboot){
      //int cb = 0;
      fprintf(f_bootconf,"%s:%s;\n",pboot->kb_val,pboot->arg);
      //
      pboot = pboot->pprev;
   }

   fclose(f_bootconf);
   printf("make boot ok\n");

   return 0;
}

/*-------------------------------------------
| Name:_mk_mount
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _mk_mount(void){

   //int fd=-1;
   struct mount_t* pmount;

   if(!pmount_lst_start)
      return 0;

   //
   printf("make mount: ...\n");
   //create file
   if(!(f_mountconf = fopen(MK_MOUNT_FILEPATH,"w+")) ){
      fprintf(stderr,"error! cannot create file :%s\n",MK_MOUNT_FILEPATH);
      return -1;
   }

   //create .boot files
   pmount = pmount_lst_start;
   while(pmount){
      //int cb = 0;
      //ex:ufs /dev/hd/hdb /etc
      fprintf(f_mountconf,"%s %s %s\n",pmount->fstype,pmount->dev,pmount->mount_point);
      //
      pmount = pmount->pprev;
   }

   fclose(f_mountconf);
   printf("make mount ok\n");

   return 0;
}


/*-------------------------------------------
| Name:_mk_dev
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _mk_dev(void){
   struct mkdev_t* pdev;

   if(!pdev_lst_start)
      return 0;

   printf("make dev: ...\n");

      //create file
   if(!(f_devconf = fopen(MK_DEVCONF_FILEPATH,"w+")) ){
      fprintf(stderr,"error! cannot create file :%s\n",MK_DEVCONF_FILEPATH);
      return -1;
   }

   //top file header
   fwrite(devconf_top_header,sizeof(char),strlen(devconf_top_header),f_devconf);

   //create dev list
   //extern declaration
   pdev = pdev_lst_start;
   while(pdev){
      //int cb = 0;
      //cb=_write(fd,pboot->arg,strlen(pboot->arg));
      fprintf(f_devconf,"%s %s;\n",dev_map_t_extern_declaration,pdev->dev_map_name);
      //
      pdev = pdev->pprev;
   }

   //dev_lst
   fwrite(dev_lst_open_declaration,sizeof(char),strlen(dev_lst_open_declaration),f_devconf);
   pdev = pdev_lst_start;
   fprintf(f_devconf,"&%s",pdev->dev_map_name);
   pdev = pdev->pprev;
   while(pdev){
      //int cb = 0;
      //cb=_write(fd,pboot->arg,strlen(pboot->arg));
      fprintf(f_devconf,",\n&%s",pdev->dev_map_name);
      //
      pdev = pdev->pprev;
   }
   fwrite(dev_lst_close_declaration,sizeof(char),strlen(dev_lst_close_declaration),f_devconf);

   //top file header
   fwrite(devconf_bottom_header,sizeof(char),strlen(devconf_bottom_header),f_devconf);

   fclose(f_devconf);

   printf("make dev ok\n");

   return 0;
}

/*-------------------------------------------
| Name:_mk_dskimg
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
#define CPUFS_SPLIT_OPTION "-split"
//cpu variant
#define CPUFS_SPLIT_OPTION_M16C62_FLAG (0x01)
const char pattern_split_option_m16c62_iar_begin_c[]=
"\n\
/*\n\
be aware ( jcvd ;) ) -split option in .xml !!! :\n\
must be used with the following directive in iar linker command file (.xcl):\n\
-Z(FARCONST)FILECPU_MEMORY=B0000\n\
*/\n\n\
#pragma memory=constseg(FILECPU_MEMORY):far\n\n";
const char pattern_split_option_m16c62_iar_end_c[]="\n#pragma memory = default\n";

const char pattern_split_option_m16c62_iar_header_c[]=
"\n\
/*\n\
be aware ( jcvd ;) ) -split option in .xml !!! :\n\
must be used with the following directive in iar linker command file (.xcl):\n\
-Z(FARCONST)FILECPU_MEMORY=B0000\n\
*/\n\n\
#pragma memory=constseg(FILECPU_MEMORY):far\n\
extern  const unsigned char filecpu_memory[];\n\
extern  const unsigned long filecpu_memory_size;\n\
#pragma memory = default\n";

const char pattern_no_split_option_default_header_c[]=
"extern  const unsigned char filecpu_memory[];\n\
extern  const unsigned long filecpu_memory_size;\n";


int _mk_dskimg(int argc, char* argv[])
{
   int r=0;
   int cb=0;
   char* cpufs_option_token;
   unsigned long cpufs_option_mask=0;

   //source file
   if( (fsrc = open (fname_src,O_RDONLY|O_EXCL,S_IREAD)) == -1 ){
         perror("open");
         //printf("cannot open file %s\n",fname_src);
         return -1;
   }

   //parse cpufs option
   if(g_kernel_conf.str_cpufs_option){
      cpufs_option_token = strtok(g_kernel_conf.str_cpufs_option," ");
      while(cpufs_option_token){
         if(!strcmp(cpufs_option_token,CPUFS_SPLIT_OPTION)){
            switch(g_kernel_conf.cpu_type){
               //
               case CPU_TYPE_M16C62:
                  cpufs_option_mask|=CPUFS_SPLIT_OPTION_M16C62_FLAG;
               break;
               //
               default:
               break;
            }
         }
         cpufs_option_token = strtok(NULL," ");
      }
   }
   //

   //dev_dskimg.c
   //destination file
   if( (fdest = open( MK_DSKIMG_FILEPATH,O_RDWR|O_CREAT|O_TRUNC|O_EXCL,S_IREAD|S_IWRITE)) == -1 ){
   	 perror("open");

      if( (fdest = open( MK_DSKIMG_FILEPATH,O_RDWR |O_TRUNC,S_IREAD|S_IWRITE )) == -1 ){
         printf("cannot open file %s\n",MK_DSKIMG_FILEPATH);
         return -1;
      }
   }

   printf("%s to %s...",fname_src,MK_DSKIMG_FILEPATH);
   //reset position at begining of file.
   lseek( fsrc, 0, SEEK_SET );
   lseek( fdest, 0, SEEK_SET );
   
   //write split option pattern for m16c62 and iar 
   if(cpufs_option_mask&CPUFS_SPLIT_OPTION_M16C62_FLAG){
     write(fdest,pattern_split_option_m16c62_iar_begin_c,strlen(pattern_split_option_m16c62_iar_begin_c));
   }

   //write start
   write(fdest,pattern_start_c,strlen(pattern_start_c));

   //read src file
   while((r=read(fsrc,buf_src,sizeof(buf_src)))>0){
      int i;
      cb+=r;

      //format
      for(i=0;i<r;i++){
         size++;
         sprintf(buf_dest,"0x%.2x",buf_src[i]);
         buf_dest[4]=',';
         buf_dest[5]=' ';
         write(fdest,buf_dest,6);
         if(!(size%(1*25)))
            write(fdest,"\r\n",2);
         
      }

   }

   //write end
   write(fdest,pattern_end_c,strlen(pattern_end_c));

   //write split option pattern for m16c62 and iar 
   if(cpufs_option_mask&CPUFS_SPLIT_OPTION_M16C62_FLAG){
      write(fdest,pattern_split_option_m16c62_iar_end_c,strlen(pattern_split_option_m16c62_iar_end_c));
   }

   close(fsrc);
   close(fdest);

   //dev_dskimg.h
   //destination file
   if( (fdest = open( MK_DSKIMG_HEADER_FILEPATH,O_RDWR|O_CREAT|O_TRUNC|O_EXCL,S_IREAD|S_IWRITE)) == -1 ){
   	 perror("open");

      if( (fdest = open( MK_DSKIMG_HEADER_FILEPATH,O_RDWR |O_TRUNC ,S_IREAD|S_IWRITE )) == -1 ){
         printf("cannot open file %s\n",MK_DSKIMG_HEADER_FILEPATH);
         return -1;
      }
   }

   //top header
   write(fdest,dskimg_top_header,strlen(dskimg_top_header));

   //include header
   write(fdest,dskimg_include_header,strlen(dskimg_include_header));

   //declaration header
   write(fdest,dskimg_decl_header,strlen(dskimg_decl_header));


   //write split option pattern for m16c62 and iar 
   if(cpufs_option_mask&CPUFS_SPLIT_OPTION_M16C62_FLAG){
      write(fdest,pattern_split_option_m16c62_iar_header_c,strlen(pattern_split_option_m16c62_iar_header_c));
   }else{
      write(fdest,pattern_no_split_option_default_header_c,strlen(pattern_no_split_option_default_header_c));
   }

   //bottom header
   write(fdest,dskimg_bottom_header,strlen(dskimg_bottom_header));


   close(fdest);


   printf("(%d bytes) ok.\n",cb);

	return 0;
}

/*-------------------------------------------
| Name:_mk_rtc
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _mk_rtc(void){
   _vfs_desc_t desc;
   char buf[8]={0};
   struct k_tm _tm={ 0, 0, 12, 28, 0, 103 };//init for test
   k_time_t time=0;

   //set kernel time from rtc
   if((desc = _vfs_open("/dev/rtc0",_O_RDONLY,0))<0) //ST m41t81
      desc = _vfs_open("/dev/rtc1",_O_RDONLY,0);


   if(desc<0)
      return -1;
     
   //
   memset(&_tm,0,sizeof(struct k_tm));
   //

   __kernel_dev_gettime(desc,buf,6);
   
   _tm.tm_sec  = buf[0];
   _tm.tm_min  = buf[1];
   _tm.tm_hour = buf[2];
   _tm.tm_mday = buf[3];
   _tm.tm_mon  = buf[4];
   _tm.tm_year = buf[5];

   //
   time = __mktime(&_tm);
   xtime.tv_usec=0;
   xtime.tv_sec = time;
   _vfs_close(desc);
   
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
int main( int argc, char *argv[])
{
   int i;
   char* ref = (char*)0;

   printf("\nmkelpton xml version: %s-%s\n\n",__DATE__,__TIME__);

   for(i=1;i<argc;i++){
      if(argv[i][0]=='-'){
         unsigned char c;
         unsigned char l=strlen(argv[i]);
         for(c=1;c<l;c++){
            switch(argv[i][c]){
               case 'a'://all
                  opt = OPT_MSK_ALL;
               break;

               case 'b'://binaries
                  opt |= OPT_MSK_B;
               break;

               case 'd'://devices
                  opt |= OPT_MSK_D;
               break;

               case 'f'://files
                  opt |= OPT_MSK_F;
               break;

               case 'k'://kernel
                  opt |= OPT_MSK_K;
               break;

               case 't'://target
               {
                  opt |= OPT_MSK_T;
                  if((i+1) == argc)//not enough parameter
                     return -1;
                  if(argv[i][c+1])
                     break;
                  if(argv[i+1][0]=='-')
                     break;

                  i++;
                  if(argv[i])
                     mk_current_target=argv[i];
               }
               break;
            }
         }
      }else{
         ref = argv[i];
         xml_mk_file = ref;
      }
   }

   if(!opt || (opt == OPT_MSK_T))
      opt = OPT_MSK_ALL;
      
   //
   _vfs();

   //make devices on rootfs
   _kernel_warmup_rootfs();
   //
   _kernel_warmup_dev();
   //
   _kernel_warmup_rtc();
   //
   _kernel_warmup_mount();
   

   //rtc init
   if(_mk_rtc()<0){
      perror("\nerror: rtc cannot be used!\n");
      return -1;
   }
   

   //read xml config file:mkconf.xml
   if(ref && _mk_conf(ref)<0){
      perror("\nerror: _mk_conf from");
      printf(" %s!",ref);
      return -1;
   }else if(!ref && _mk_conf("mkconf.xml")<0){
      perror("\nerror: _mk_conf from default ref mkconf.xml!\n");
      return -1;
   }
   printf("\n_mk_conf ok!\n\n");

   //make devices list
   if( (opt&OPT_MSK_D) && _mk_dev()<0){
      perror("\nerror: _mk_dev failed!\n");
      return -1;
   }
   printf("\n_mk_dev ok!\n\n");

   //make mount files (.boot) 
   if( (opt&OPT_MSK_K) && _mk_mount()<0){
      perror("\nerror: _mk_mount failed!\n");
      return -1;
   }
   printf("\n_mk_mount ok!\n\n");
   
   //make boot files (.boot) 
   if( (opt&OPT_MSK_K) && _mk_boot()<0){
      perror("\nerror: _mk_boot failed!\n");
      return -1;
   }
   printf("\n_mk_boot ok!\n\n");

   //make binaries 
   if( (opt&OPT_MSK_B) && _mk_binaries()<0){
      perror("\nerror: _mk_binaries failed!\n");
      return -1;
   }
   printf("\n_mk_binaries ok!\n\n");
      
   //make files 
   if((opt&OPT_MSK_F) && _mk_file()<0){
      perror("\nerror: _mk_file failed!\n");
      return -1;
   }
   printf("\n_mk_file ok!\n\n");

   //make dir
   if((opt&OPT_MSK_R) && _mk_dir()<0){
      perror("\nerror: _mk_dir failed!\n");
      return -1;
   }
   printf("\n_mk_dir ok!\n\n");

   //make disk image
   if( (opt& (OPT_MSK_B|OPT_MSK_F)) && _mk_dskimg(0,0)<0){
      perror("\nerror: _mk_dskimg failed!\n");
      return -1;
   }
   printf("\n_mk_dskimg ok!\n\n");

   if(mk_current_target)
      printf("\nmklepton for %s success :)!\n",mk_current_target);
   else
      printf("\nmklepton success :)! no target specified???\n");

   return 0;
}


/*============================================
| End of Source  : mkleptux.c
|---------------------------------------------
| Historic:
|---------------------------------------------
| Authors     | Date     | Comments
| $Log: not supported by cvs2svn $
| Revision 1.2  2009/08/25 13:55:49  jjp
| manage <dir />
|
| Revision 1.1  2009/03/30 15:49:10  jjp
| first import of tauon
|
| Revision 1.1  2009/03/30 11:06:13  jjp
| First import of tauon
|
|---------------------------------------------
==============================================*/
