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
| Compiler Directive
==============================================*/
#ifndef _KERNEL_STUB_H_
#define _KERNEL_STUB_H_

/*============================================
| Includes
==============================================*/

/*============================================
| Declaration
==============================================*/
typedef short _vfs_mode_t;
//mount
typedef enum{
   fs_rootfs,   //0
   fs_ufs
}_vfs_fstype;

//mkfs
struct _vfs_formatopt_t{
   int  max_node;
   int  max_blk;
   int  blk_sz;
   long dev_sz;
};

typedef short _vfs_fstype_t;
typedef struct {
	_vfs_fstype_t fstype;
	char * dev_path;
	struct _vfs_formatopt_t vfs_formatopt;
	int ret;
}_vfs_mkfs_t;


//statvfs
typedef unsigned short _vfs_fsblkcnt_t;
typedef unsigned short _vfs_fsfilcnt_t;
struct _vfs_statvfs_st{
   unsigned int f_bsize;    //file system block size
   unsigned int f_frsize;   //fundamental filesystem block size

   _vfs_fsblkcnt_t    f_blocks;  //total number of blocks on file system in units of f_frsize
   _vfs_fsblkcnt_t    f_bfree;    //total number of free blocks
   _vfs_fsblkcnt_t    f_bavail;   //number of free blocks available to
                            //non-privileged process

   _vfs_fsfilcnt_t    f_files;    //total number of file serial numbers
   _vfs_fsfilcnt_t    f_ffree;    //total number of free file serial numbers
   _vfs_fsfilcnt_t    f_favail;   //number of file serial numbers available to
                            //non-privileged process

   unsigned int f_fsid;     //file system id
   unsigned int f_flag;     //bit mask of f_flag values
   unsigned int f_namemax;  //maximum filename length
};

typedef struct {
	int fildes;
	struct _vfs_statvfs_st statvfs;
}_vfs_statvfs_t;

//dummy desc type
typedef short _vfs_desc_t;
typedef int _vfs_size_t;

//vfs function
int _vfs(void);
_vfs_desc_t _vfs_open(const char* ref, int oflag, _vfs_mode_t mode);
int _vfs_close(_vfs_desc_t desc);
int _vfs_mount(_vfs_fstype_t fstype,const char* dev_path,const char* mount_path);
int _vfs_umount(const char* mount_path);
int _vfs_mkdir(const char* ref, _vfs_mode_t mode);
int _vfs_ioctl(_vfs_desc_t desc, int request, ... );
int _vfs_ls(char* ref);
int _vfs_makefs(_vfs_fstype_t fstype,const char* dev_path,struct _vfs_formatopt_t* vfs_formatopt);
int _vfs_statvfs(const char *path, struct _vfs_statvfs_st *buf);
int _vfs_write(_vfs_desc_t desc,char* buf, _vfs_size_t size);

//warmup functions
void _kernel_warmup_rootfs(void);
void _kernel_warmup_dev(void);
int _kernel_warmup_rtc(void);
int _kernel_warmup_mount(void);


//from ioctl_hd.h
#define HDGETSZ         1
#define HDSETSZ         2
#define HDCHK           3
#define HDGETSCTRSZ     4
#define HDCLRSCTR       5
#define HDCLRDSK        6
#define HDIO            7

//io informations
typedef unsigned long hdio_addr_t;

typedef struct hdio_st{
   hdio_addr_t addr;
}hdio_t;


//from bin.h
#define EXEC_SIGNT   0x1b //'esc'

typedef unsigned char pthread_priority_t;
typedef short pthread_stacksize_t;
typedef short pthread_timeslice_t;

//
typedef struct {
   char signature;
   pthread_priority_t   priority;
   pthread_stacksize_t  stacksize;
   pthread_timeslice_t  timeslice;
   int16_t              index;
}exec_file_t;


//from time.h
struct __k_timeval {
	unsigned int tv_sec;
	unsigned int tv_usec;
};

#define k_timeval __k_timeval

struct k_tm {
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
	int tm_yday;
	int tm_isdst;
};

extern struct __k_timeval xtime;

typedef unsigned int k_time_t;
k_time_t __mktime(struct k_tm *timeptr);
extern void __wrpr_kernel_dev_gettime(_vfs_desc_t desc, char * buf, int size);
#endif /*_KERNEL_STUB_H_*/

/*
|---------------------------------------------
| Historic:
|---------------------------------------------
| Authors     | Date     | Comments
| $Log: not supported by cvs2svn $
| Revision 1.1  2009/03/30 11:06:13  jjp
| First import of tauon
|
| Revision 1.1.1.1  2009/02/05 15:29:50  jjp
| First import in CVS
|
|---------------------------------------------*/
