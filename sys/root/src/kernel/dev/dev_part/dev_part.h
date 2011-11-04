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
#ifndef _DEV_PART_H
#define _DEV_PART_H

/*============================================
| Includes
==============================================*/


/*============================================
| Declaration
==============================================*/
//see http://www.rayknights.org/pc_boot/w95b_mbr.htm or http://en.wikipedia.org/wiki/Master_boot_record
typedef struct mbr_part_entry_st {
   unsigned char boot_indicator:8;
   unsigned char starting_head:8;

   unsigned char raw_starting_cylinder_part1:2;
   unsigned char starting_sector:6;
   unsigned char raw_starting_cylinder_part2:8;

   unsigned char os_fs_type:8;
   unsigned char ending_head:8;

   unsigned char raw_ending_cylinder_part1:2;
   unsigned char ending_sector:6;
   unsigned char raw_ending_cylinder_part2:8;

   unsigned char raw_starting_sector[4];//need to transform to be really used
   unsigned char raw_sector_count[4];//need to transform to be really used

}mbr_part_entry_t;

//
typedef struct mbr_disk_entry_st {
   mbr_part_entry_t mbr_part_entry;
   unsigned int starting_sector; //really used (it is raw_starting_sector transformed)
   unsigned int sector_count; //really used (it is raw_sector_count transformed)
   unsigned int starting_cylinder;
   unsigned int ending_cylinder;
   //dummy fields
   void * p_underlying_fops;//filesystem operation
   unsigned int inode;//inode number to link physical partition to descriptor
   kernel_pthread_mutex_t  kernel_pthread_mutex;
}mbr_disk_entry_t;

#define  DISK_NAME_SIZE          15
#define  MAX_PARTITION           4 //we handle only primary partition
#define  SECTOR_DFLT_SIZE        512

//we can have more than one sliced disk
typedef struct mbr_disk_st {
   mbr_disk_entry_t mbr_part_tbl[MAX_PARTITION];//a MBR disk partition table have 4 entry (primary for us)
   char disk_name[DISK_NAME_SIZE+1]; //with suffix s or p for partition
}mbr_disk_t;


#define  MBR_SIZE                512
#define  MBR_SIGNATURE_OFFSET    510
#define  MBR_SIGNATURE_LOW       0x55
#define  MBR_SIGNATURE_HIGH      0xAA

#define  PART_TABLE_OFFSET       446
#define  PART_TABLE_ENTRY_SZ     16
#define  PART_NAME_SUFFIX        's' // or 'p'
//#define  PART_OS_TYPE            0xa5 //FreeBSD see http://fr.wikipedia.org/wiki/Partition_de_disque_dur
#define  PART_OS_TYPE            0x06 //FAT16

#define DEFAULT_PART_TYPE        0x00 //empty

int build_part_tbl(int desc, mbr_disk_t * md);
void get_part_info(mbr_disk_entry_t * mde);
void bigend_2_litend_4b(const char *src, char *dst);
unsigned int str_2_int(const char * src, unsigned int size);
//
int create_rootfs_entry(const char * name, mbr_disk_t * md, unsigned int dev_no, void * pfops);
int set_part_infos_4_desc(unsigned int desc);

//
int is_valid_mbr(int desc);
int create_mbr(int desc);

//
mbr_disk_t * create_disk_entry(const char * name, mbr_disk_t * head);
mbr_disk_t * find_disk_entry(const char * disk_name, int off);
int create_mbr_part_tbl_entry(int desc, char *disk_name, unsigned int part_no, unsigned int sector_no, unsigned char bootable);
int create_disk_rootfs_entry(int part_no, const char * name, mbr_disk_t * md, unsigned int dev_no, void * pfops);
#endif /*_DEV_PART_H*/
