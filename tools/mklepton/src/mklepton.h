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
| Compiler Directive
==============================================*/
#ifndef _MKLEPON_H_
#define _MKLEPTON_H_

/*============================================
| Includes
==============================================*/


/*============================================
| Declaration
==============================================*/

#define XML_TAG_ELMT_MKLEPTON    "mklepton"
#define XML_TAG_ELMT_TARGET      "target"
#define XML_TAG_ELMT_ARCH        "arch"
#define XML_TAG_ELMT_KERNEL      "kernel"
#define XML_TAG_ELMT_DEVICES     "devices"
#define XML_TAG_ELMT_DEV         "dev"
#define XML_TAG_ELMT_BINARIES    "binaries"
#define XML_TAG_ELMT_CPU         "cpu"
#define XML_TAG_ELMT_HEAP        "heap"
#define XML_TAG_ELMT_THREAD      "thread"
#define XML_TAG_ELMT_PROCESS     "process"
#define XML_TAG_ELMT_OPENFILES   "openfiles"
#define XML_TAG_ELMT_DESCRIPTORS "descriptors"
#define XML_TAG_ELMT_ENV         "env"
#define XML_TAG_ELMT_NETWORK     "network"
#define XML_TAG_ELMT_CPUFS       "cpufs"
#define XML_TAG_ELMT_MOUNT       "mount"
#define XML_TAG_ELMT_BOOT        "boot"
#define XML_TAG_ELMT_BIN         "bin"
#define XML_TAG_ELMT_FILES       "files"
#define XML_TAG_ELMT_FILE        "file"
#define XML_TAG_ELMT_COMMAND     "command"
#define XML_TAG_ELMT_DISK        "disk"
//
#define XML_TAG_ELMT_DIRS        "directories"
#define XML_TAG_ELMT_DIR         "directory"

#define XML_TAG_ATTR_SRCPATH     "src_path"
#define XML_TAG_ATTR_SRCFILE     "src_file"
#define XML_TAG_ATTR_DESTPATH    "dest_path"
#define XML_TAG_ATTR_DEV         "dev"
#define XML_TAG_ATTR_DELAY       "delay"
#define XML_TAG_ATTR_NAME        "name"
#define XML_TAG_ATTR_USE         "use"
#define XML_TAG_ATTR_STACK       "stack"
#define XML_TAG_ATTR_PRIORITY    "priority"
#define XML_TAG_ATTR_TIMESLICE   "timeslice"
#define XML_TAG_ATTR_SIZE        "size"
#define XML_TAG_ATTR_MAX         "max"
#define XML_TAG_ATTR_FREQ        "freq"
#define XML_TAG_ATTR_ARG         "arg"
#define XML_TAG_ATTR_VALUE       "value"
#define XML_TAG_ATTR_TYPE        "type"
#define XML_TAG_ATTR_POINT       "point"
#define XML_TAG_ATTR_PATH        "path"
#define XML_TAG_ATTR_NODE        "node"
#define XML_TAG_ATTR_BLOCK       "block"
#define XML_TAG_ATTR_BLOCKSZ     "blocksz"
#define XML_TAG_ATTR_OPTION      "option"


typedef enum {
   CPU_TYPE_GNU32,
   CPU_TYPE_ARM7,
   CPU_TYPE_M16C62
}cpu_type_enum_t;

const char* cpu_type_list[]={
   "CPU_GNU32",
   "CPU_ARM7",
   "CPU_M16C62",
   (char*)0
};

//kernel struct configuration
struct kernel_conf_t{
   char* str_cpu_type;
   char* str_cpu_freq;
   char* str_heap_size;
   char* str_thread_max;
   char* str_process_max;
   char* str_openfiles_max;
   char* str_descriptors_max;
   char* str_network_used;
   char* str_env_path;
   //
   cpu_type_enum_t cpu_type;
   unsigned long cpu_freq;
   int heap_size;
   int thread_max;
   int process_max;
   int openfiles_max;
   int descriptors_max;
   int network_used;
   long cpufs_size;
   int  cpufs_node;
   int  cpufs_block;
   int  cpufs_blocksz;
   char* str_cpufs_option;
};

//boot struct config
struct boot_t{
   char* kb_val;
   char* arg;
   struct boot_t* pprev; 
   struct boot_t* pnext; 
};

//mount struct config
struct mount_t{
   char* fstype;
   char* dev;
   char* mount_point;

   struct mount_t* pprev; 
   struct mount_t* pnext; 
};


#define XML_DEV_VALUE_ON   "ON"
#define XML_DEV_VALUE_OFF  "OFF"

#define MK_DEV_CPU            "DEV_CPU"              // /dev/cpu
#define MK_DEV_FILECPU        "DEV_FILECPU"          // /dev/hd/hda
#define MK_DEV_EEPROM_24XXX   "DEV_EEPROM_24XXX"     // /dev/hd/hd(x)
#define MK_DEV_M16CUART_S0    "DEV_M16CUART_S0"      // /dev/ttys0
#define MK_DEV_M16CUART_S1    "DEV_M16CUART_S1"      // /dev/ttys1
#define MK_DEV_LCDSED1540     "DEV_LCDSED1540"       // /dev/lcd0
#define MK_DEV_RTCM41T81      "DEV_RTCM41T81"        // /dev/rtc0
#define MK_DEV_RTCX1203       "DEV_RTCX1203"         // /dev/rtc1
#define MK_DEV_KBCPLD_A0383   "DEV_KBCPLD_A0383"     // /dev/kb0


//dev struct config
struct mkdev_t{
   char*   dev_map_name;
   struct mkdev_t* pprev; 
   struct mkdev_t* pnext; 
};


//bin struct config
struct mkbin_t{
   int   bin_index;
   char* src_path;
   char* dest_path;
   char* bin_name;
   char* stack;
   char* priority; 
   char* timeslice; 
   struct mkbin_t* pprev; 
   struct mkbin_t* pnext; 
};

//file struct config
struct mkfile_t{
   char* src_file;
   char* dest_path;
   char* file_name;
   struct mkfile_t* pprev; 
   struct mkfile_t* pnext; 
};

//dir struct config
struct mkdir_t{
   char * src_path;
   char * dest_path;
   struct mkdir_t * pprev;
   struct mkdir_t * pnext;
};

//general buffer for xml parser
#define OPT_MSK_A 0x0001   //-a
#define OPT_MSK_B 0x0002   //-b
#define OPT_MSK_D 0x0004   //-d
#define OPT_MSK_F 0x0008   //-f
#define OPT_MSK_K 0x0010   //-k
#define OPT_MSK_T 0x0020   //-t
#define OPT_MSK_R 0x0040   //-r

#define OPT_MSK_ALL OPT_MSK_B|OPT_MSK_D|OPT_MSK_F|OPT_MSK_K|OPT_MSK_R

//dummy add
#define _O_RDONLY     0x0001
#define _O_WRONLY     0x0002
#define _O_RDWR       0x0003 //O_RDONLY|O_WRONLY
#define _O_CREAT      0x0004
#define _O_APPEND     0x0008
#define _O_SYNC       0x0010
#define _O_NONBLOCK   0x0020


#define _S_IREAD 	0400//S_IRUSR
#define _S_IWRITE 0200//S_IWUSR
#define _O_TRUNC	01000

#define MAX_PATH          260



//xml func
int xml_elmt_start_kernel(const char **attr);
int xml_elmt_end_kernel(void);
int xml_elmt_kernel_cpu(const char **attr);
int xml_elmt_kernel_heap(const char **attr);
int xml_elmt_kernel_thread(const char **attr);
int xml_elmt_kernel_process(const char **attr);
int xml_elmt_kernel_openfiles(const char **attr);
int xml_elmt_kernel_descriptors(const char **attr);
int xml_elmt_kernel_cpufs(const char **attr);
int xml_elmt_kernel_env(const char **attr);
int xml_elmt_kernel_network(const char **attr);
int xml_elmt_start_boot(const char **attr);
int xml_elmt_end_boot(void);
int xml_elmt_boot_command(const char **attr);
int xml_elmt_start_mount(const char **attr);
int xml_elmt_end_mount(void);
int xml_elmt_mount_command(const char **attr);
int xml_elmt_start_devices(const char **attr);
int xml_elmt_end_devices(void);
int xml_elmt_dev(const char **attr);
int xml_elmt_start_binaries(const char **attr);
int xml_elmt_end_binaries(void);
int xml_elmt_bin(const char **attr);
int xml_elmt_files(const char **attr);
int xml_elmt_file(const char **attr);
int xml_elmt_mklepton(const char **attr);
int xml_elmt_arch(const char **attr);
int xml_elmt_start_target(const char **attr);
int xml_elmt_end_target(void);

//
int xml_elmt_dirs(const char **attr);
int xml_elmt_dir(const char **attr);

//
static void start(void *data, const char *el, const char **attr);
static void end(void *data, const char *el);

//make elements
int _mk_conf(char* file_conf);
int _mk_binaries(void);
int _mk_file(void);
int _mk_boot(void);
int _mk_mount(void);
int _mk_dev(void);
int _mk_dskimg(int argc, char* argv[]);
int _mk_rtc(void);

//
int _mk_dir(void);

#endif /*MKLEPTON_H_*/
