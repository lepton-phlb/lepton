/*
The contents of this file are subject to the Mozilla Public License Version 1.1
(the "License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License for the
specific language governing rights and limitations under the License.

The Original Code is Lepton.

The Initial Developer of the Original Code is Chauvin-Arnoux.
Portions created by Chauvin-Arnoux are Copyright (C) 2011. All Rights Reserved.

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
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "kernel/core/types.h"
#include "kernel/core/interrupt.h"
#include "kernel/core/system.h"
#include "kernel/core/fcntl.h"
#include "kernel/core/ioctl.h"
#include "kernel/core/malloc.h"
#include "kernel/core/process.h"
#include "kernel/core/ioctl_hd.h"

#include "kernel/fs/vfs/vfsdev.h"
#include "kernel/fs/vfs/vfstypes.h"
#include "kernel/fs/vfs/vfs.h"

#include "lib/libc/stdio/stdio.h"
#include "dev_part.h"
/*============================================
| Global Declaration
==============================================*/
static int dev_part_load(void);
static int dev_part_open(desc_t desc,int o_flag);
static int dev_part_close(desc_t desc);
static int dev_part_isset_read(desc_t desc);
static int dev_part_isset_write(desc_t desc);
static int dev_part_write(desc_t desc, const char* buf,int size);
static int dev_part_read(desc_t desc, char* buf,int size);
static int dev_part_seek(desc_t desc,int offset,int origin);
static int dev_part_ioctl(desc_t desc,int request,va_list ap);

static const char dev_part_name[]="part\0";

dev_map_t dev_part_map={
   dev_part_name,
   S_IFBLK,
   dev_part_load,
   dev_part_open,
   dev_part_close,
   __fdev_not_implemented,
   __fdev_not_implemented,
   dev_part_read,
   dev_part_write,
   dev_part_seek,
   dev_part_ioctl //ioctl
};

//we can handle MAX_DISK sliced
#define  MAX_SLICED_DISK   4
static mbr_disk_t disk_part_tbl[MAX_SLICED_DISK]={0};
static unsigned char disk_part_tbl_count=0;

static unsigned int sector_size=SECTOR_DFLT_SIZE;

static char tauon_mbr_str[] = "tauon MBR signature\0";
static char tauon_part_cmd_add[] = "add\0";
static char tauon_part_cmd_scan[] = "scan\0";
//
static int ascii_2_int(char c);
static int int_2_str(unsigned int src, char *dst);
/*============================================
| Implementation
==============================================*/
/*--------------------------------------------
| Name:        dev_part_load
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int dev_part_load(void) {
   return 0;
}

/*--------------------------------------------
| Name:        dev_part_open
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int dev_part_open(desc_t desc,int o_flag) {
   if(ofile_lst[desc].oflag & O_RDONLY) {
   }

   if(ofile_lst[desc].oflag & O_WRONLY) {
   }

   if(!ofile_lst[desc].nb_writer
      &&!ofile_lst[desc].nb_reader) {
   }

   if(!set_part_infos_4_desc(desc)) {
      //ofile_lst[desc].offset=((mbr_disk_entry_t *)(ofile_lst[desc].p))->starting_sector*sector_size;
   }
   return 0;
}

/*--------------------------------------------
| Name:        dev_part_close
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int dev_part_close(desc_t desc) {
   if(!ofile_lst[desc].nb_reader && !ofile_lst[desc].nb_writer) {
      ofile_lst[desc].p=(void*)0;
   }

   return 0;
}

/*--------------------------------------------
| Name:        dev_part_seek
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int dev_part_seek(desc_t desc,int offset,int origin) {
   switch(origin) {
   case SEEK_SET:
      ofile_lst[desc].offset=offset;
      break;

   case SEEK_CUR:
      ofile_lst[desc].offset+=offset;
      break;

   case SEEK_END:
      if(offset>0)
         return -1;
      ofile_lst[desc].offset+=offset;
      break;
   }
   //
   return ofile_lst[desc].offset;
}

/*--------------------------------------------
| Name:        dev_part_read
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int dev_part_read(desc_t desc, char* buf,int size) {
   pfsop_t pfsop = NULL;
   unsigned int abs_offset=((mbr_disk_entry_t *)(ofile_lst[desc].p))->starting_sector*sector_size;
   int cb=-1;

   if(!buf) return -1;

   //maybe protection by mutex
   //seek on physical device and read on it
   pfsop = (pfsop_t)(((mbr_disk_entry_t *)(ofile_lst[desc].p))->p_underlying_fops); // ->fdev_write(desc, buf,size);
   //we get absolute offset
   ofile_lst[desc].offset += abs_offset;
   cb = pfsop->fdev.fdev_read(desc, buf,size);
   ofile_lst[desc].offset -= abs_offset;

   return cb;
}

/*--------------------------------------------
| Name:        dev_part_write
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int dev_part_write(desc_t desc, const char* buf,int size) {
   int cb;
   pfsop_t pfsop = NULL;
   unsigned int abs_offset=((mbr_disk_entry_t *)(ofile_lst[desc].p))->starting_sector*sector_size;

   if(!buf) return -1;

   //protect partition write
   //kernel_pthread_mutex_lock(&((mbr_disk_entry_t *)(ofile_lst[desc].p))->kernel_pthread_mutex);

   pfsop = (pfsop_t)(((mbr_disk_entry_t *)(ofile_lst[desc].p))->p_underlying_fops);
   //we get absolute offset, write, and set new relative offset
   ofile_lst[desc].offset += abs_offset;
   cb = pfsop->fdev.fdev_write(desc, buf,size);
   ofile_lst[desc].offset -= abs_offset;

   //kernel_pthread_mutex_unlock(&((mbr_disk_entry_t *)(ofile_lst[desc].p))->kernel_pthread_mutex);
   return cb;
}

/*--------------------------------------------
| Name:        dev_part_ioctl
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int dev_part_ioctl(desc_t desc,int request,va_list ap) {

   switch(request) {
   case I_LINK: {
      va_arg(ap, int);
      char * cmd = va_arg(ap, char *);
      char * disk_suffix = va_arg(ap, char *);
      char * part_number = va_arg(ap, char *);
      char * block_size = va_arg(ap, char *);
      char * sector_count = va_arg(ap, char *);
      //from kernel or user program
      //int _pid = _sys_getpid();
      int _desc = ofile_lst[desc].desc_nxt[0]; //(_pid>0)?process_lst[_pid]->desc_tbl[desc]:desc;
      mbr_disk_t * p_md=NULL;

      //we always open dev_part in O_RDWR mode
      if(ofile_lst[desc].oflag != O_RDWR) {
         return -1;
      }

      //if not available command or disk
      if(!cmd || !disk_suffix) return -1;

      if(is_valid_mbr(_desc)<0) {
         //try to create a MBR
         if(create_mbr(_desc)<0) {
            //can't do anything
            return -1;
         }
      }
      //create a disk entry before mbr_entry
      if(!(p_md =create_disk_entry(disk_suffix,&disk_part_tbl[0]))) {
         return -1;
      }
      //add a new partition entry
      if(!strncmp(cmd,tauon_part_cmd_add, strlen(tauon_part_cmd_add))) {
         //if no available parameters
         if(!part_number || !block_size || !sector_count) return -1;

         //create mbr entry
         if(create_mbr_part_tbl_entry(_desc, p_md->disk_name,
                                      atoi(part_number), atoi(sector_count),0x00)<0) {
            return -1;
         }
      }
      //
      if(build_part_tbl(_desc,p_md)<0) {
         //not find MBR signature
         return -1;
      }
      //ofile_lst[desc].desc_nxt[{0|1}] contain underlying module : a physical block device
      create_rootfs_entry(disk_suffix, p_md,
                          ofile_lst[desc].ext.dev, (void *)ofile_lst[_desc].pfsop);
      ///(void *)ofile_lst[ofile_lst[desc].desc_nxt[0]].pfsop);

//         ofile_lst[desc].p = (void *)&disk_part_tbl[0];
   }
   break;

   case I_UNLINK: {
   }
   break;

   case HDGETSZ: {
      long* hdsz_p= va_arg( ap, long*);
      if(!hdsz_p)
         return -1;
      //get size of desc
      if(!ofile_lst[desc].p)
         return -1;
      //get size in bytes
      *hdsz_p = ((mbr_disk_entry_t *)(ofile_lst[desc].p))->sector_count*sector_size;
   }
   break;

   case HDGETSCTRSZ: {
      unsigned int * sector_size=va_arg(ap, unsigned int *);
      if(!sector_size)
         return -1;

      *sector_size = 512;
   }
   break;
   default:
      break;
   }
   //
   return 0;
}

/*--------------------------------------------
| Name:        build_part_tbl
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
----------------------------------------------*/
int build_part_tbl(int desc, mbr_disk_t * md) {
   int cb=-1;
   int i=0;
   //
   if(!md) return -1;

   if(is_valid_mbr(desc)) {
      return -1;
   }

   if(_vfs_lseek(desc,PART_TABLE_OFFSET,SEEK_SET)<0) {
      return -1;
   }
   //get raw partition informations
   for(i=0; i<MAX_PARTITION; i++) {
      if(_vfs_read(desc,(void*)&(md->mbr_part_tbl[i].mbr_part_entry),PART_TABLE_ENTRY_SZ)<0) {
         return -1;
      }
      get_part_info(&md->mbr_part_tbl[i]);
   }
   //
   return 0;
}

/*--------------------------------------------
| Name:        get_part_info
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
----------------------------------------------*/
void get_part_info(mbr_disk_entry_t * mde) {
   unsigned int complete_starting_cylinder = 0;
   unsigned int complete_ending_cylinder = 0;
   char real_starting_rel_sector[5]={0};
   char real_sector_count[5]={0};
   //get cylinder (dummy but get compatibility)
   mde->starting_cylinder =
      (mde->mbr_part_entry.raw_starting_cylinder_part1<<
       8) | mde->mbr_part_entry.raw_starting_cylinder_part2;
   mde->ending_cylinder =
      (mde->mbr_part_entry.raw_ending_cylinder_part1<<
       8) | mde->mbr_part_entry.raw_ending_cylinder_part2;
   //
   bigend_2_litend_4b(mde->mbr_part_entry.raw_starting_sector, real_starting_rel_sector);
   bigend_2_litend_4b(mde->mbr_part_entry.raw_sector_count,real_sector_count);
   mde->starting_sector = str_2_int(real_starting_rel_sector,4);
   mde->sector_count = str_2_int(real_sector_count,4);
}

#define  WORK_BUFFER_SIZE  32
/*--------------------------------------------
| Name:        create_disk_entry
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
----------------------------------------------*/
mbr_disk_t * create_disk_entry(const char * name, mbr_disk_t * head) {
   int i=0;
   char disk_name_suff = PART_NAME_SUFFIX; //or 'p'
   int len=-1;
   mbr_disk_t * md=NULL;
   //
   if(!name || !head) return NULL;

   if(md = find_disk_entry(name, strlen("/dev/"))) {
      return md;
   }

   for(i=0; i<MAX_SLICED_DISK; i++) {
      //we find a valid entry
      if(head[i].disk_name[0] != '/') {
         //make disk name entry like /dev/hdd[x]s
         strcpy(head[i].disk_name,"/dev/");
         strncat(head[i].disk_name, name, strlen(name));
         strncat(head[i].disk_name, &disk_name_suff, 1);
         //create directory
         if(_vfs_mkdir(head[i].disk_name,0)<0) {
            //we can't so reset entry
            memset((void *)&head[i],0,sizeof(mbr_disk_t));
            return NULL;
         }
         return &head[i];
      }
   }
   //
   return NULL;
}

/*--------------------------------------------
| Name:        create_rootfs_entry
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
----------------------------------------------*/
int create_rootfs_entry(const char * name, mbr_disk_t * md, unsigned int dev_no, void * pfops) {
   int i=0;
   char work[WORK_BUFFER_SIZE]={0};
   char disk_name_suff = PART_NAME_SUFFIX; //or 'p'
   int len=-1;
   struct stat s={0};
   //
   if(!name || !md) return -1;
   //
   strcpy(work,md->disk_name);
   strcat(work,"/");
   strncat(work, name, strlen(name));
   strncat(work, &disk_name_suff, 1);

   len = strlen(work);
   //we can create entry for partition
   for(i=0; i<MAX_PARTITION; i++) {
      if(md->mbr_part_tbl[i].sector_count) {
         work[len] = 48+i;

         //create node if it is not already exist
         if(!_vfs_mknod(work,S_IFBLK,dev_no)) {
            _vfs_stat(work, &s);
            md->mbr_part_tbl[i].inode=s.st_ino;
            md->mbr_part_tbl[i].p_underlying_fops = pfops;
         }
      }
   }
   return 0;
}

/*--------------------------------------------
| Name:        set_part_infos_4_desc
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
----------------------------------------------*/
int set_part_infos_4_desc(unsigned int desc) {
   mbr_disk_t * p_disk_part_tbl = &disk_part_tbl[0];
   int i=0,j=0;

   if(desc<0) return -1;

   for(i=0; i<MAX_SLICED_DISK; i++) {
      for(j=0; j<MAX_PARTITION; j++) {
         //compare ofile_lst[desc].inodenb to mbr_disk_entry.inode
         if(ofile_lst[desc].inodenb==p_disk_part_tbl[i].mbr_part_tbl[j].inode) {
            ofile_lst[desc].p = (void*)&p_disk_part_tbl[i].mbr_part_tbl[j];
            return 0;
         }
      }
      //
      p_disk_part_tbl++;
   }
   return -1;
}

/*--------------------------------------------
| Name:        is_valid_mbr
| Description:
| Parameters:
| Return Type:
| Comments: check if disk is already sliced
| See:
----------------------------------------------*/
int is_valid_mbr(int desc) {
   char mbr_signature[3]={0};
   //
   if(desc<0) return -1;

   //verify mbr signature
   if(_vfs_lseek(desc,MBR_SIGNATURE_OFFSET,SEEK_SET)<0) {
      return -1;
   }

   _vfs_read(desc,(void *)mbr_signature, 2);
   if(mbr_signature[0]!= MBR_SIGNATURE_LOW
      && mbr_signature[1]!= MBR_SIGNATURE_HIGH) {
      return -1;
   }
   return 0;
}

/*--------------------------------------------
| Name:        create_mbr
| Description:
| Parameters:
| Return Type:
| Comments: create a mbr with no entry
| See:
----------------------------------------------*/
int create_mbr(int desc) {
   //MAX_PARTITION*PART_TABLE_ENTRY_SZ=4*16=64
   char buf[64]={0};
   if(desc<0) return -1;

   //go to beginning of disk and write tauon special signature
   if(_vfs_lseek(desc,0L,SEEK_SET)<0) {
      return -1;
   }
   if((_vfs_write(desc,tauon_mbr_str,strlen(tauon_mbr_str)))<0) {
      return -1;
   }

   //clear partition table on disk
   if(_vfs_lseek(desc,PART_TABLE_OFFSET,SEEK_SET)<0) {
      return -1;
   }
   if((_vfs_write(desc,buf,64))<0) {
      return -1;
   }

   //go to mbr signature offset and write it
   if(_vfs_lseek(desc,MBR_SIGNATURE_OFFSET,SEEK_SET)<0) {
      return -1;
   }

   buf[0] = MBR_SIGNATURE_LOW;
   buf[1] = MBR_SIGNATURE_HIGH;
   if((_vfs_write(desc,buf,2))<0) {
      return -1;
   }

   return 0;
}

/*--------------------------------------------
| Name:        find_disk_entry
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
----------------------------------------------*/
mbr_disk_t * find_disk_entry(const char * disk_name, int off) {
   int i=0;

   if(!disk_name) return NULL;

   for(i=0; i<MAX_SLICED_DISK; i++) {
      //all physical disk stay in /dev/hd/hd[count_hdd] aka /dev/hd/hdd
      if(!strncmp(disk_name,disk_part_tbl[i].disk_name+off,3)) {
         return &disk_part_tbl[i];
      }
   }
   return NULL;
}

/*--------------------------------------------
| Name:        create_mbr_part_tbl_entry
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
----------------------------------------------*/
int create_mbr_part_tbl_entry(int desc, char *disk_name, unsigned int part_no,
                              unsigned int sector_no,
                              unsigned char bootable) {
   mbr_disk_t * mde=NULL;

   unsigned int prev_starting_sector=1;
   unsigned int prev_sector_count=0;
   unsigned long phy_disk_size=0;

   if(!disk_name || !sector_no) return -1;

   //find disk_entry
   if(!(mde = find_disk_entry(disk_name,0))) {
      return -1;
   }

   if(part_no) {
      //
      prev_starting_sector=mde->mbr_part_tbl[part_no-1].starting_sector;
      prev_sector_count=mde->mbr_part_tbl[part_no-1].sector_count;
   }

   //create
   mde->mbr_part_tbl[part_no].starting_sector=prev_starting_sector+prev_sector_count;
   mde->mbr_part_tbl[part_no].sector_count=sector_no;

   //verify that we are not out of disk or reset entry
   _vfs_ioctl(desc,HDGETSZ,&phy_disk_size);
   if((mde->mbr_part_tbl[part_no].starting_sector*sector_size+
       mde->mbr_part_tbl[part_no].sector_count*sector_size)>phy_disk_size) {
      memset((void*)&mde->mbr_part_tbl[part_no],0,sizeof(mbr_disk_entry_t));
      return -1;
   }

   //transform it to raw value
   int_2_str(mde->mbr_part_tbl[part_no].starting_sector,
             mde->mbr_part_tbl[part_no].mbr_part_entry.raw_starting_sector);
   int_2_str(mde->mbr_part_tbl[part_no].sector_count,
             mde->mbr_part_tbl[part_no].mbr_part_entry.raw_sector_count);

   //bootable to indicate firmware
   mde->mbr_part_tbl[part_no].mbr_part_entry.boot_indicator = bootable;
   //dummy system type
   mde->mbr_part_tbl[part_no].mbr_part_entry.os_fs_type = PART_OS_TYPE;
   //seek to device
   if(_vfs_lseek(desc,PART_TABLE_OFFSET+part_no*PART_TABLE_ENTRY_SZ,SEEK_SET)<0) {
      memset((void*)&mde->mbr_part_tbl[part_no],0,sizeof(mbr_disk_entry_t));
      return -1;
   }
   //write it to physical disk or reset entry
   if((_vfs_write(desc,(void*)&mde->mbr_part_tbl[part_no].mbr_part_entry,
                  PART_TABLE_ENTRY_SZ))<0) {
      memset((void*)&mde->mbr_part_tbl[part_no],0,sizeof(mbr_disk_entry_t));
      return -1;
   }
   return 0;
}

/*--------------------------------------------
| Name:        ascii_2_int
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
----------------------------------------------*/
int ascii_2_int(char c) {
   /*ascii value from '0' to '9'*/
   if(c>='0' && c<='9') {
      return (c-48);
   }
   /*ascii value from 'A' to 'F'*/
   else if(c>='A' && c<='F') {
      return (10+c-65);
   }
   /*ascii value from 'a' to 'f'*/
   else if(c>='a' && c<='f') {
      return (10+c-97);
   }
   return 0;
}

/*--------------------------------------------
| Name:        int_2_str
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
----------------------------------------------*/
int int_2_str(unsigned int src, char *dst) {
   char work[8+1] = {0};
   char tmp[5] = {0};
   int i=0,j=0;
   int low, high;
   //
   if(!dst) {
      return -1;
   }
   //tranform decimal to hexadecimal
   sprintf(work,"%8x",src);

   for(i=0; i<8; i=i+2,j++) {
      high = ascii_2_int(work[i]);
      low = ascii_2_int(work[i+1]);
      tmp[j] = (low)|(high<<4);
   }
   //
   bigend_2_litend_4b(tmp, dst);
   return 0;
}

/*--------------------------------------------
| Name:        bigend_2_litend_4b
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
----------------------------------------------*/
void bigend_2_litend_4b(const char *src, char *dst) {
   dst[0] = src[3];
   dst[1] = src[2];
   dst[2] = src[1];
   dst[3] = src[0];
}

/*--------------------------------------------
| Name:        str_2_int
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
----------------------------------------------*/
unsigned int str_2_int(const char * s, unsigned int size) {
   unsigned int sum=0;
   int i=0;
   char * work = NULL;
   //
   if(!s) return 0;

   for(work=(char *)(s+size-1); i<(size*2); work--) {
      //low
      sum += ((*work) & 0x0f) * (int)pow(16,i++);
      //high
      sum += (((*work) & 0xf0)>>4) * (int)pow(16,i++);
   }
   return sum;
}


/*============================================
| End of Source  : dev_part.c
|---------------------------------------------
| Historic:
|---------------------------------------------
| Authors     | Date     | Comments
| $Log:$
==============================================*/
