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
#ifndef _FAT_CORE_H
#define _FAT_CORE_H

/*============================================
| Includes
==============================================*/
#include "kernel/fs/vfs/vfstypes.h"

/*============================================
| Declaration
==============================================*/
//From Aurélie Roche Impl modify by JJP
// Definitions diverses

// FAT16
//cluster size
#define FAT16_CLUSSZ             2
//minimum value for a cluster
#define FAT16_CLUSMIN            0x0002
//maximum value for a cluster
#define FAT16_CLUSMAX            0xFFEF
//
#define FAT16_LCLUSMIN           0xFFF8                // Valeur minimale d'un numero de cluster
#define FAT16_LCLUSMAX           0xFFFF                // Valeur maximale d'un numero de cluster
//nb cluster max for FAT16
#define FAT16_NBCLUSMAX          65525
//nb cluster min for FAT16
#define FAT16_NBCLUSMIN          4085
//minimum of sectors per cluster for FAT16
#define FAT16_SECMIN             1
//maximum of sectors per cluster for FAT16
#define FAT16_SECMAX             128

// BOOT SECTOR
//boot sector offset
#define FAT16_BS_ADDR            0
//boot sector size
#define FAT16_BS_SIZE            62
//boot program offset, size, value
#define FAT16_BS_JUMPBOOT_OFF    0
#define FAT16_BS_JUMPBOOT_SIZE   3
#define FAT16_BS_JUMPBOOT_VAL    0xEB3C90
//what program format media
#define FAT16_BS_OEM_OFF         3
#define FAT16_BS_OEM_SIZE        8
#define FAT16_BS_OEM_VAL         "PJTTAUON"
//nb bytes per sector
#define FAT16_BS_BPS_OFF         11
#define FAT16_BS_BPS_SIZE        2
#define FAT16_BS_BPS_VAL         512
//nb sectors per cluster
#define FAT16_BS_SPC_OFF         13
#define FAT16_BS_SPC_SIZE        1
//nb reserved sectors (included boot sector)
#define FAT16_BS_RSVSEC_OFF      14
#define FAT16_BS_RSVSEC_SIZE     2
#define FAT16_BS_RSVSEC_VAL      1
//nb FATS
#define FAT16_BS_NBFAT_OFF       16
#define FAT16_BS_NBFAT_SIZE      1
#define FAT16_BS_NBFAT_VAL       2
//nb entries in root directory
#define FAT16_BS_NBROOTENT_OFF   17
#define FAT16_BS_NBROOTENT_SIZE  2
#define FAT16_BS_NBROOTENT_VAL   512
//nb sectors count
#define FAT16_BS_NBSEC16_OFF     19
#define FAT16_BS_NBSEC16_SIZE    2
#define FAT16_BS_NBSEC16_VAL     0
//media type
#define FAT16_BS_MEDIA_OFF       21
#define FAT16_BS_MEDIA_SIZE      1
#define FAT16_BS_MEDIA_VAL       0xF8
//size of FAT in sectors
#define FAT16_BS_FATBS_OFF       22
#define FAT16_BS_FATBS_SIZE      2
//nb sectors per tracks
#define FAT16_BS_TCKPS_OFF       24
#define FAT16_BS_TCKPS_SIZE      2
#define FAT16_BS_TCKPS_VAL       63
//nb heads
#define FAT16_BS_NBH_OFF         26
#define FAT16_BS_NBH_SIZE        2
#define FAT16_BS_NBH_VAL         255
//nb hidding sectors count
#define FAT16_BS_NBSHID_OFF      28
#define FAT16_BS_NBSHID_SIZE     4
#define FAT16_BS_NBSHID_VAL      0
//nb sectors count
#define FAT16_BS_NBSEC32_OFF     32
#define FAT16_BS_NBSEC32_SIZE    4
//drive number of media!
#define FAT16_BS_DRVNUM_OFF     36
#define FAT16_BS_DRVNUM_SIZE    1
#define FAT16_BS_DRVNUM_VAL     0x00
//used by NT
#define FAT16_BS_RSV1_OFF       37
#define FAT16_BS_RSV1_SIZE      1
#define FAT16_BS_RSV1_VAL       0x00
//boot signature
#define FAT16_BS_BOOTSIG_OFF    38
#define FAT16_BS_BOOTSIG_SIZE   1
#define FAT16_BS_BOOTSIG_VAL    0x29
//disk serial number
#define FAT16_BS_VOLID_OFF      39
#define FAT16_BS_VOLID_SIZE     4
//disk label
#define FAT16_BS_VOLLAB_OFF     43
#define FAT16_BS_VOLLAB_SIZE    11
#define FAT16_BS_VOLLAB_VAL     "TAUONVOLUME"
//file system type
#define FAT16_BS_FSTYPE_OFF     54
#define FAT16_BS_FSTYPE_SIZE    8
#define FAT16_BS_FSTYPE_VAL     "FAT16   "

// ROOT DIRECTORY
//root entry size
#define RD_SIZE                  32
//name of entry (file or directory)
#define RD_ENTNAME_OFF           0
#define RD_ENTNAMR_SIZE          8
//extension of entry
#define RD_ENTEXT_OFF            8
#define RD_ENTEXT_SIZE           3
//attribute
#define RD_ATTR_OFF              11
#define RD_ATTR_SIZE             1
//reserved by NT
#define RD_RSVD_OFF              12
#define RD_RSVD_SIZE             1
#define RD_RSVD_VAL              0x00
//count of tenth of a second
#define RD_CRTTIMETTH_OFF        13
#define RD_CRTTIMETTH_SIZE       1
//time of creation
#define RD_CRTTIME_OFF           14
#define RD_CRTTIME_SIZE          2
//date of creation
#define RD_CRTDATE_OFF           16
#define RD_CRTDATE_SIZE          2
//last access date
#define RD_LSTDATEACC_OFF        18
#define RD_LSTDATEACC_SIZE       2
//cluster number (high part:always 0)
#define RD_HICLUSNO_OFF          20
#define RD_HICLUSNO_SIZE         2
//time of last write
#define RD_LSTTIMEWRT_OFF        22
#define RD_LSTTIMEWRT_SIZE       2
//date of last write
#define RD_LSTDATEWRT_OFF        24
#define RD_LSTDATEWRT_SIZE       2
//cluster number
#define RD_LWCLUSNO_OFF          26
#define RD_LWCLUSNO_SIZE         2
//file size
#define RD_FILESIZE_OFF          28
#define RD_FILESIZE_SIZE         4

///entry attribute
//read only file
#define RD_ATTR_READONLY         0x01
//hide file
#define RD_ATTR_HIDDEN           0x02
//system file
#define RD_ATTR_SYSTEM           0x04
//volume name
#define RD_ATTR_VOLUMEID         0x08
//directory
#define RD_ATTR_DIRECTORY        0x10
//file
#define RD_ATTR_ARCHIVE          0x20

//long name
#define RD_ATTR_LONGNAME         (RD_ATTR_READONLY|RD_ATTR_HIDDEN|RD_ATTR_SYSTEM|RD_ATTR_VOLUMEID)

//long name mask
#define FAT16_LONGNAME_MSK       (RD_ATTR_READONLY|RD_ATTR_HIDDEN|RD_ATTR_SYSTEM|RD_ATTR_VOLUMEID|RD_ATTR_DIRECTORY|RD_ATTR_ARCHIVE)
//long name last long entry
#define FAT16_LAST_LONGENTRY_MSK 0x40
//part of name if long entry
#define FAT16_LONGNAME_NAME1     5
#define FAT16_LONGNAME_NAME2     6
#define FAT16_LONGNAME_NAME3     2
#define FAT16_LONGNAME_SUM       (FAT16_LONGNAME_NAME1+FAT16_LONGNAME_NAME2+FAT16_LONGNAME_NAME3)
//

//empty data cluter value
#define RD_CLUSEMPTY             0x0000
//available entry + last entry
#define RD_ENT_FREE              0x00
//remove entry
#define RD_ENT_REMOVE            0xE5

//
#define FAT16_MAX_NAME_FIL       8
#define FAT16_MAX_EXT_FIL        3

#define FAT16_MAX_ENTRIES        3
#define FAT16_MAX_ENTRIES_TEST   (FAT16_MAX_ENTRIES+1)
#define FAT16_MAX_NAME_FIL_VFAT  32//FAT16_LONGNAME_SUM*FAT16_MAX_ENTRIES//32

//
#define FAT16_VOLUME_NAME        "TAUONVOL\0"
//to clean cluster content
#define FAT_16_CLEAN_BUFFER_SIZE        128

//tauon definition for FAT16
typedef unsigned long   fat16_u32_t;
typedef unsigned int    fat16_u16_t;
typedef unsigned char   fat16_u8_t;

typedef struct fat16_core_info_st{
   fat16_u8_t          nbsec_per_clus; //nb sector per cluster
   fat16_u32_t         media_size; //media size in bytes
   fat16_u32_t         bs_addr; // boot sector address
   fat16_u32_t         bs_size; //boot sector size in bytes
   fat16_u32_t         hs_addr; //hidden sector(s) addr
   fat16_u32_t         hs_size; //hidden sector(s) size in bytes
   fat16_u32_t         fat_addr; //first FAT address
   fat16_u32_t         fat_size; //FAT size
   fat16_u32_t         rd_addr; //root directory address
   fat16_u32_t         rd_size; //root directory size in bytes
   fat16_u32_t         ud_addr; //user data address
   fat16_u32_t         ud_size; //user data size in bytes
}fat16_core_info_t;

//
typedef struct fat16_ofile_st{
   fat16_u32_t         entry_phys_addr; // physical address of entry
   fat16_u16_t         entry_data_cluster; //first cluster of entry
   fat16_u8_t          entry_attr;   //attribute of entry (volum, file or directory)
   fat16_u32_t         entry_infos_addr;
}fat16_ofile_t;

extern fat16_ofile_t fat16_ofile_lst[]; // Table des fichiers ouverts, param fat16

//Following structure are defined in standard
//boot sector
typedef struct __attribute__((packed)) boot_sector_st {
   unsigned char  BS_jmpBoot[3];              // jump inst E9xxxx or EBxx90
   char           BS_OEMName[8];           // OEM name and version
}boot_sector_t;

//Bios Parameter Block
typedef struct __attribute__((packed)) bios_param_block_st {
   unsigned short    BPB_BytesPerSec;   // bytes per sector
   unsigned char     BPB_SecPerClust;   // sectors per cluster
   unsigned short    BPB_RsvdSecCnt; // number of reserved sectors
   unsigned char     BPB_NumFATs;    // number of FATs
   unsigned short    BPB_RootEntCnt;   // number of root directory entries
   unsigned short    BPB_TotSec16;    // total number of sectors
   unsigned char     BPB_Media;      // media descriptor
   unsigned short    BPB_FATSz16;    // number of sectors per FAT
   unsigned short    BPB_SecPerTrk;   // sectors per track
   unsigned short    BPB_NumHeads;      // number of heads
   unsigned int      BPB_HiddSec; // # of hidden sectors
   unsigned int      BPB_TotSec32;   // # of sectors if BPB_Sectors == 0
}bios_param_block_t;

typedef struct __attribute__((packed)) bpb_16_offset_36_st {
   unsigned char     BS_DrvNum;
   unsigned char     BS_Reserved1;
   unsigned char     BS_BootSig;
   unsigned char     BS_VolID[4];
   unsigned char     BS_VolLabel[11];
   unsigned char     BS_FilSysType[8];
}bpb_offset_36_t;

//structure to get core info
typedef struct __attribute__((packed)) fat16_boot_core_info_st {
   boot_sector_t        bs;
   bios_param_block_t   bpb;
   bpb_offset_36_t      bpb_16_offset_36;
}fat16_boot_core_info_t;

//msdos dir entry
typedef struct __attribute__((packed)) fat_msdos_dir_entry_st {
   unsigned char DIR_Name[FAT16_MAX_NAME_FIL+FAT16_MAX_EXT_FIL];//name of entry (file or directory)
   unsigned char DIR_Attr;//attribute
   unsigned char DIR_NTRes;
   unsigned char DIR_CrtTimeTenth;//count of tenth of a second
   unsigned char DIR_CrtTime[2];//time of creation
   unsigned char DIR_CrtDate[2];//date of creation
   unsigned char DIR_LstAccDate[2];//last access date
   unsigned char DIR_FstClusHI[2];
   unsigned char DIR_WrtTime[2];//time of last write
   unsigned char DIR_WrtDate[2];//date of last write
   unsigned char DIR_FstClusLO[2];////cluster numbercluster number
   unsigned int  DIR_FileSize;//file size
}fat_msdos_dir_entry_t;

typedef struct __attribute__((packed)) fat_vfat_dir_entry_st {
   unsigned char LDIR_Ord;//indicate entry number. last entry add FAT16_LAST_LONGENTRY_MSK
   unsigned char LDIR_Name1[10];//first 5 char in Unicode
   unsigned char LDIR_Attr;//must be RD_ATTR_LONGNAME
   unsigned char LDIR_Type;//?
   unsigned char LDIR_Chksum;//checksum of short dir entry associated with long name
   unsigned char LDIR_Name2[12];//6-11 char in Unicode
   unsigned char LDIR_FstClusLO[2];//must be zero
   unsigned char LDIR_Name3[4];//12-13 char in Unicode
}fat_vfat_dir_entry_t;

typedef union {
   fat_msdos_dir_entry_t   msdos_entry;
   fat_vfat_dir_entry_t    vfat_entry;
}fat_dir_entry_t;

///
//prototypes for internal common functions
int _fat16_init(fat16_core_info_t * fat_info, desc_t dev_desc);

//
fat16_u32_t _fat16_cluster_add(fat16_core_info_t * fat_info, fat16_u16_t cluster);
fat16_u16_t _fat16_cluster_suiv(fat16_core_info_t * fat_info, desc_t dev_desc, fat16_u16_t cluster);
fat16_u16_t _fat16_getclus(fat16_core_info_t * fat_info, desc_t dev_desc, bios_param_block_t * bpb);
int _fat16_putclus(fat16_core_info_t * fat_info, desc_t dev_desc, fat16_u16_t cluster);
int _fat16_chainclus(fat16_core_info_t * fat_info, desc_t dev_desc, fat16_u16_t cluster_curr, fat16_u16_t cluster_next);
void _fat16_delcluslist(fat16_core_info_t * fat_info, desc_t desc, fat16_u16_t cluster);
int _fat16_lastclus(fat16_core_info_t * fat_info, desc_t dev_desc, fat16_u16_t cluster);

//
int _fat16_offsetinfo(fat16_core_info_t * fat_info, desc_t desc, fat16_u16_t* clus, fat16_u16_t* offset);
fat16_u32_t _fat16_adddir(fat16_core_info_t * fat_info, desc_t desc);
int _fat16_checkdirempty(fat16_core_info_t * fat_info, desc_t desc);

//
void _fat16_getdatetime(unsigned char *buf_date, unsigned char *buf_time);
time_t _fat16_converttime(const unsigned char *buf_date, const unsigned char *buf_time);

//
fat16_u16_t _fat16_create_fat_entry(fat_msdos_dir_entry_t *msdos_entry, desc_t desc, int attr);
int _fat16_create_fat_directory_entry(fat_msdos_dir_entry_t *msdos_entry, desc_t desc, fat16_u16_t cluster, fat16_u16_t parent_cluster);

//
int _fat_write_data(desc_t dev_desc, unsigned long addr, unsigned char *data, unsigned int size);
int _fat_read_data(desc_t dev_desc, unsigned long addr, unsigned char *data, unsigned int size);

//
int _fat16_getdesc(desc_t desc, fat16_u32_t addr);

#endif //_FAT_CORE_H
