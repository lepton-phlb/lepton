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
#ifndef _VFSTYPES_H
#define _VFSTYPES_H


/*===========================================
Includes
=============================================*/
#include "kernel/core/kernelconf.h"
#include "kernel/core/types.h"
#include "kernel/core/dirent.h"
#include "kernel/core/pipe.h"
#include "kernel/core/net/socks.h"

#include "kernel/fs/vfs/vfsdev.h"

#include "kernel/core/interrupt.h"
#include "kernel/core/kernel_sem.h"
#include "kernel/core/kernel_pthread.h"

#include "kernel/fs/ufs/ufsinfo.h"
#include "kernel/fs/yaffs/yaffsinfo.h"
#include "kernel/fs/fat/fatinfo.h"

/*===========================================
Declaration
=============================================*/

#define __dot "."
#define __dotdot ".."

#define INVALID_BLOCK_NB   -1
#define INVALID_INODE_NB   -1

//File system type
typedef enum {
#if __KERNEL_VFS_SUPPORT_ROOTFS==1
   fs_rootfs,   //0
#endif
#if __KERNEL_VFS_SUPPORT_UFS==1
   fs_ufs,
#endif
#if __KERNEL_VFS_SUPPORT_UFSX==1
   fs_ufsx,
#endif
#if __KERNEL_VFS_SUPPORT_KOFS==1
   fs_kofs,
#endif
#if __KERNEL_VFS_SUPPORT_MSDOS==1
   fs_msdos,
#endif
#if __KERNEL_VFS_SUPPORT_VFAT==1
   fs_vfat
#endif
}fstype;

#define UFS_ALIGNEMENT  4

struct statvfs {
   unsigned int f_bsize;    //file system block size
   unsigned int f_frsize;   //fundamental filesystem block size

   fsblkcnt_t f_blocks;     //total number of blocks on file system in units of f_frsize
   fsblkcnt_t f_bfree;       //total number of free blocks
   fsblkcnt_t f_bavail;      //number of free blocks available to
                             //non-privileged process

   fsfilcnt_t f_files;       //total number of file serial numbers
   fsfilcnt_t f_ffree;       //total number of free file serial numbers
   fsfilcnt_t f_favail;      //number of file serial numbers available to
                             //non-privileged process

   unsigned int f_fsid;     //file system id
   unsigned int f_flag;     //bit mask of f_flag values
   unsigned int f_namemax;  //maximum filename length
};


/**
 *
 * option de formatage du priphrique de stockage.
 *
 */
struct vfs_formatopt_t {
   int max_node;
   int max_blk;
   int blk_sz;
   long dev_sz;
};


//typedef VFS_FORMATOPT vfs_formatopt_t;

//super block allocation
//WARNING!!! int size beetween 32bits 16bits on makefs,read fs writefs for ufs type
typedef struct {
   uint32_t superblk_size;
   uint32_t alloc_blk_size;
   uint32_t alloc_node_size;

   uint32_t nodeblk_size;
   uint32_t datablk_size;

   uint16_t blk_size;

   char* psuperblk;

   uint32_t superblk_addr;
   uint32_t nodeblk_addr;
   uint32_t datablk_addr;
}superblk_t;

/**
 *
 * nombre maximum de de superblock (voir MAX_MOUNT_DEVICE).
 *
 */
#if !defined(CPU_CORTEXM)
#define MAX_SUPER_BLOCK 8 //6//4 //4
#else
#define MAX_SUPER_BLOCK 4
#endif

extern superblk_t superblk_lst[MAX_SUPER_BLOCK];

/**
 *
 * information specifique au type de systme de fichiers.
 *
 */
typedef union {
   ufs_info_t ufs_info;
   yaffs_info_t   yaffs_info;
   fat_info_t fat_info;
}fsinfo_t;

//mount list
/**
 *
 * structure d'une entre dans la liste des points de montages.
 *
 */
typedef struct mntdev_st {
   desc_t dev_desc;

   //
   inodenb_t mnt_inodenb;
   struct mntdev_st* pmntdev;

   //
   fstype_t fstype;
   superblk_t* psuperblk_info;

   inodenb_t inodetbl_size;
   inodenb_t inodenb_offset;

   fsinfo_t fs_info;
}mntdev_t;

#define MAX_MOUNT_DEVICE   MAX_SUPER_BLOCK

extern mntdev_t mntdev_lst[MAX_MOUNT_DEVICE];

//file system types
typedef int (*FS_LOADFS)(void);
typedef FS_LOADFS PFS_LOADFS;

typedef int (*FS_CHECKFS)(mntdev_t* pmntdev);
typedef FS_CHECKFS PFS_CHECKFS;

typedef int (*FS_MAKEFS)(desc_t dev_desc,struct vfs_formatopt_t* vfs_formatopt);
typedef FS_MAKEFS PFS_MAKEFS;

typedef int (*FS_READFS)(mntdev_t* pmntdev);
typedef FS_READFS PFS_READFS;

typedef int (*FS_WRITEFS)(mntdev_t* pmntdev);
typedef FS_WRITEFS PFS_WRITEFS;

typedef int (*FS_STATFS)(mntdev_t* pmntdev,struct statvfs *statvfs);
typedef FS_STATFS PFS_STATFS;


//
typedef inodenb_t (*FS_GETINODE)(void);
typedef FS_GETINODE PFS_GETINODE;

typedef int (*FS_PUTINODE)(inodenb_t inodenb);
typedef FS_PUTINODE PFS_PUTINODE;

//
typedef int (*FS_MOUNTDIR)(desc_t desc,inodenb_t original_root_node,inodenb_t target_root_node);
typedef FS_MOUNTDIR PFS_MOUNTDIR;

typedef int (*FS_READDIR)(desc_t desc,struct dirent* pdirent);
typedef FS_READDIR PFS_READDIR;

typedef int (*FS_TELLDIR)(desc_t desc);
typedef FS_TELLDIR PFS_TELLDIR;

typedef int (*FS_SEEKDIR)(desc_t desc,int loc);
typedef FS_SEEKDIR PFS_SEEKDIR;

//
typedef inodenb_t (*FS_LOOKUPDIR)(desc_t desc,char* filename);
typedef FS_LOOKUPDIR PFS_LOOKUPDIR;


//create node
typedef int (*FS_MKNOD)(desc_t desc, inodenb_t inodenb,dev_t dev);
typedef FS_MKNOD PFS_MKNOD;

//
typedef inodenb_t (*FS_CREATE)(desc_t desc,char* filename, int attr);
typedef FS_CREATE PFS_CREATE;

typedef int (*FS_OPEN)(desc_t desc);
typedef FS_OPEN PFS_OPEN;

typedef int (*FS_CLOSE)(desc_t desc);
typedef FS_CLOSE PFS_CLOSE;

typedef int (*FS_READ)(desc_t desc, char* buf,int size);
typedef FS_READ PFS_READ;

typedef int (*FS_WRITE)(desc_t desc, char* buf,int size);
typedef FS_WRITE PFS_WRITE;

typedef int (*FS_SEEK)(desc_t desc, int offset, int origin);
typedef FS_SEEK PFS_SEEK;

typedef int (*FS_TRUNCATE)(desc_t desc, off_t length);
typedef FS_TRUNCATE PFS_TRUNCATE;
//
typedef int (*FS_REMOVE)(desc_t desc_ancst,desc_t desc);
typedef FS_REMOVE PFS_REMOVE;

typedef int (*FS_RENAME)(desc_t desc,const char*  old_name, char* new_name);
typedef FS_RENAME PFS_RENAME;


typedef struct {
   PFS_LOADFS loadfs;
   PFS_CHECKFS checkfs;
   PFS_MAKEFS makefs;
   PFS_READFS readfs;
   PFS_WRITEFS writefs;
   PFS_STATFS statfs;

   //
   PFS_MOUNTDIR mountdir;
   PFS_READDIR readdir;
   PFS_TELLDIR telldir;
   PFS_SEEKDIR seekdir;
   PFS_LOOKUPDIR lookupdir;

   //
   PFS_MKNOD mknod;

   //
   PFS_CREATE create;
   PFS_OPEN open;
   PFS_CLOSE close;
   PFS_READ read;
   PFS_WRITE write;
   PFS_SEEK seek;
   PFS_TRUNCATE truncate;
   PFS_REMOVE remove;
   PFS_RENAME rename;
}fs_map_t;

typedef union {
   fs_map_t fs;
   fdev_map_t fdev;
}_ufsopt_t;

typedef const _ufsopt_t fsop_t;
typedef const _ufsopt_t* pfsop_t;
//typedef const FILESYTEM_OPERATION  fsop_t;
//typedef const FILESYTEM_OPERATION* pfsop_t;
typedef struct {
   uint16_t urd : 1;
   uint16_t uwr : 1;
   uint16_t uex : 1;
   uint16_t grd : 1;
   uint16_t gwr : 1;
   uint16_t gex : 1;
   uint16_t ord : 1;
   uint16_t owr : 1;
   uint16_t oex : 1;

   uint16_t uid : 4;
   uint16_t gid : 3;
}bits_tbl_t;

typedef union {
   bits_tbl_t rwx_bits_tbl;
   uint16_t rwxrwxrwx;
}ino_mod_t;

#if !defined(CPU_CORTEXM)
#define MAX_FILESYSTEM  7 //rootfs, ufs, ufsx, kofs, msdos, vfat, (null)
#else
#define MAX_FILESYSTEM  7 //rootfs, ufs, ufsx, kofs, msdos, vfat, (null)
#endif
extern pfsop_t const fsop_lst[MAX_FILESYSTEM];

//
typedef uint8_t file_status_t;

#define MSK_FSTATUS_MODIFIED 0x01
#define IS_FSTATUS_MODIFIED(__status)  (__status&MSK_FSTATUS_MODIFIED)

//Open file list
typedef void*  hext_t;
typedef long vfs_off_t;

typedef struct ofile_s {

   pid_t owner_pid;
   kernel_pthread_t*  owner_pthread_ptr_read;
   kernel_pthread_t*  owner_pthread_ptr_write;
   //
   desc_t desc;
   //for stream link see I_LINK and I_UNLINK
   desc_t desc_nxt[2]; //0:read 1:write
   desc_t desc_prv;

   int used;
   char nb_reader;
   char nb_writer;

   int size;
   int oflag;
   vfs_off_t offset;

   int attr;

   time_t cmtime;      //creation/modification date
   file_status_t status; //MODIFIED

   inodenb_t dir_inodenb;
   inodenb_t inodenb;

   mntdev_t* pmntdev;
   pfsop_t pfsop;
   union {
      hext_t hext;
      dev_t dev;
      pipe_desc_t pipe_desc;
      hsock_t hsock;
   }ext;

   //use for device driver specific structure
   void* p;

   //for io completion
   //kernel_pthread_mutex_t  mutex;
   kernel_sem_t sem_read;
   kernel_sem_t sem_write;


#ifdef KERNEL_PROFILER
   unsigned short _profile_counter;
#endif

}OPEN_FILE;

typedef OPEN_FILE ofile_t;

#ifndef MAX_OPEN_FILE
   #pragma message("warning!!! MAX_OPEN_FILE not defined")
   #define MAX_OPEN_FILE 15 //20
#endif

extern ofile_t ofile_lst[MAX_OPEN_FILE];

pfsop_t _vfs_mntdev2fsop(mntdev_t* pmntdev);



#define __get_dev_desc(desc) ofile_lst[desc].pmntdev->dev_desc


#define __cvt2physnode(desc,logicnode) (logicnode-ofile_lst[desc].pmntdev->inodenb_offset)


#define __cvt2logicnode(desc,physnode) (physnode+ofile_lst[desc].pmntdev->inodenb_offset)


//for io only
#define __lock_io(__pthread_ptr__,__desc__,__oflag__){ \
      if((__oflag__)&O_RDONLY) { \
         if(kernel_sem_wait(&ofile_lst[(__desc__)].sem_read)<0) { \
            return -1; \
         } \
      } \
      if((__oflag__)&O_WRONLY) { \
         if(kernel_sem_wait(&ofile_lst[(__desc__)].sem_write)<0) { \
            return -1; \
         } \
      } \
      __pthread_ptr__->io_desc = (__desc__); \
}

#define __unlock_io(__pthread_ptr__,__desc__,__oflag__){ \
      if((__oflag__)&O_RDONLY) \
         kernel_sem_post(&ofile_lst[(__desc__)].sem_read); \
      if((__oflag__)&O_WRONLY) \
         kernel_sem_post(&ofile_lst[(__desc__)].sem_write); \
      __pthread_ptr__->io_desc = -1; \
}

//#define __trylock_io(__desc__) kernel_pthread_mutex_trylock(&ofile_lst[__desc__].mutex)
#define __trylock_io(__desc__,__oflag__) \
   ( ((__oflag__)&O_RDONLY) ? kernel_sem_trywait(&ofile_lst[(__desc__)].sem_read) : \
     ( ((__oflag__)&O_WRONLY) ? kernel_sem_trywait(&ofile_lst[(__desc__)].sem_write) : -1 ) \
   )


#endif
