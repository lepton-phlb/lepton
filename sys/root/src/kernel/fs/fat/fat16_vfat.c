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
#include <stdlib.h>
#include <string.h>

#include "kernel/core/system.h"
#include "kernel/fs/vfs/vfstypes.h"
#include "kernel/fs/vfs/vfskernel.h"
#include "kernel/fs/vfs/vfs.h"

#include "kernel/core/time.h"
#include "kernel/core/systime.h"

#include "fat16.h"
#include "fatcore.h"
#include "fatcore_msdos.h"
#include "fatcore_vfat.h"
/*============================================
| Global Declaration
==============================================*/
int _fat_vfat_statfs(mntdev_t* pmntdev,struct statvfs *statvfs);
int _fat_vfat_readdir(desc_t desc,dirent_t* dirent);
inodenb_t _fat_vfat_lookupdir(desc_t desc,char* filename);
inodenb_t _fat_vfat_create(desc_t desc,char* filename, int attr);
int _fat_vfat_open(desc_t desc);
int _fat_vfat_remove(desc_t desc_ancst,desc_t desc);
int _fat_vfat_rename(desc_t desc,const char*  old_name, char* new_name);

//file system operations
fsop_t fat_vfat_op={
      _fat_loadfs,
      _fat_checkfs,
      _fat_makefs,
      _fat_readfs,
      _fat_writefs,
      _fat_vfat_statfs,
      _fat_mountdir,
      _fat_vfat_readdir,
      _fat_telldir,
      _fat_seekdir,
      _fat_vfat_lookupdir,
      _fat_mknod,
      _fat_vfat_create,
      _fat_vfat_open,
      _fat_close,
      _fat_read,
      _fat_write,
      _fat_seek,
      _fat_truncate,
      _fat_vfat_remove,
      _fat_vfat_rename
};

#define FAT16_MAX_LONGNAME_TAB_ENTRY   (FAT16_MAX_NAME_FIL_VFAT/FAT16_LONGNAME_SUM+1)
static fat_vfat_dir_entry_t vfat_tab_long_entry[FAT16_MAX_LONGNAME_TAB_ENTRY]={0};

/*============================================
| Implementation
==============================================*/
/*-------------------------------------------
| Name:_fat_vfat_statfs
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _fat_vfat_statfs(mntdev_t* pmntdev,struct statvfs *statvfs) {
   int i=0;
   unsigned short cluster_value=0;
   memset(statvfs,0,sizeof(struct statvfs));
   //

   statvfs->f_bsize = __get_BPB_SecPerClus(pmntdev)*__get_BPB_BytesPerSec(pmntdev);
   //number of cluster
   statvfs->f_blocks = __get_BPB_TotSec16(pmntdev)?__get_BPB_TotSec16(pmntdev):__get_BPB_TotSec32(pmntdev);
   statvfs->f_namemax = FAT16_MAX_NAME_FIL_VFAT;

   //number of free blocks
   for(i=0;i<(__get_fat_size(pmntdev)/FAT16_CLUSSZ);i++) {
      if(_fat_read_data(pmntdev->dev_desc,__get_fat_addr(pmntdev)+(i*FAT16_CLUSSZ),(unsigned char *)&cluster_value,FAT16_CLUSSZ)<0) {
         return -1;
      }
      if(cluster_value == RD_CLUSEMPTY) {
         statvfs->f_bfree++;
      }
   }
   return 0;
}
/*-------------------------------------------
| Name:_fat_vfat_readdir
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _fat_vfat_readdir(desc_t desc,dirent_t* dirent) {
   fat16_u32_t current_addr;
   fat_dir_entry_t fat_entry={0};
   int i=0,j=0;
   inodenb_t _ino_phys=__cvt2physnode(desc, ofile_lst[desc].inodenb);

   //we are in rootdir
   if(!_ino_phys) {
      do {
         //we reach userdata
         if((current_addr = fat16_ofile_lst[desc].entry_phys_addr+ofile_lst[desc].offset)==__get_ud_addr(ofile_lst[desc].pmntdev)) {
            return -1;
         }

         //read medium
         if(_fat_read_data(__get_dev_desc(desc),current_addr,(unsigned char *)&fat_entry.msdos_entry,RD_SIZE)<0) {
            return -1;
         }

         //entry empty
         if(fat_entry.msdos_entry.DIR_Name[0]==RD_ENT_FREE) {
            return -1;
         }
         // or entry  remove
         if(fat_entry.msdos_entry.DIR_Name[0]==RD_ENT_REMOVE) {
            ofile_lst[desc].offset += RD_SIZE;
            continue;
         }
         //

         //or long name entry
         if((fat_entry.vfat_entry.LDIR_Attr&FAT16_LONGNAME_MSK)==RD_ATTR_LONGNAME) {
            //rewind offset anf get name
            ofile_lst[__get_dev_desc(desc)].pfsop->fdev.fdev_seek(__get_dev_desc(desc),current_addr-RD_SIZE,SEEK_SET);
            if(_fat16_vfat_build_name(__get_dev_desc(desc),dirent->d_name,fat_entry.vfat_entry.LDIR_Ord&~FAT16_LAST_LONGENTRY_MSK) < 0)
               return -1;

            dirent->inodenb = __cvt2logicnode(desc,((current_addr-__get_rd_addr(ofile_lst[desc].pmntdev))/RD_SIZE));
            //inc offset
            ofile_lst[desc].offset += RD_SIZE*(__vfat_get_nb_long_entries(fat_entry.vfat_entry)+1);
            break;
         }

         //for volume name
         ofile_lst[desc].offset += RD_SIZE;
      } while(1);
   }
   //or entry  is in rootdir
   else if(_ino_phys<__get_BPB_RootEntCnt(ofile_lst[desc].pmntdev)){
      do {
         //we chain cluster
         if(!(current_addr = _fat16_adddir(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info, desc))) {
            return -1;
         }

         //read medium
         if(_fat_read_data(__get_dev_desc(desc),current_addr,(unsigned char *)&fat_entry.msdos_entry,RD_SIZE)<0) {
            return -1;
         }

         //entry empty
         if(fat_entry.msdos_entry.DIR_Name[0]==RD_ENT_FREE) {
            return -1;
         }
         // or entry  remove
         if(fat_entry.msdos_entry.DIR_Name[0]==RD_ENT_REMOVE) {
            ofile_lst[desc].offset += RD_SIZE;
            continue;
         }

         //or long name entry
         if((fat_entry.vfat_entry.LDIR_Ord&FAT16_LAST_LONGENTRY_MSK)
                        && (fat_entry.vfat_entry.LDIR_Attr&FAT16_LONGNAME_MSK)==RD_ATTR_LONGNAME) {
            //rewind offset and get name
            ofile_lst[__get_dev_desc(desc)].pfsop->fdev.fdev_seek(__get_dev_desc(desc),current_addr-RD_SIZE,SEEK_SET);
            if(_fat16_vfat_build_oob_name(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info,
                  __get_dev_desc(desc),dirent->d_name,fat_entry.vfat_entry.LDIR_Ord&~FAT16_LAST_LONGENTRY_MSK,
                  fat16_ofile_lst[desc].entry_data_cluster) < 0)
               return -1;

            dirent->inodenb = __cvt2logicnode(desc,(__get_BPB_RootEntCnt(ofile_lst[desc].pmntdev)+(current_addr-__get_ud_addr(ofile_lst[desc].pmntdev))/RD_SIZE));
            //inc offset
            ofile_lst[desc].offset += RD_SIZE*(__vfat_get_nb_long_entries(fat_entry.vfat_entry)+1);
            break;
         }

         //compute inode
         //two specials case "." and ".."
         if(!strncmp(fat_entry.msdos_entry.DIR_Name,". ",2)) {
            strncpy(dirent->d_name, ".\0", 2);
            dirent->inodenb = ofile_lst[desc].inodenb;
            //
            ofile_lst[desc].offset += RD_SIZE;
            break;
         }
         else if(!strncmp(fat_entry.msdos_entry.DIR_Name,".. ",3)) {
            strncpy(dirent->d_name, "..\0", 3);
            dirent->inodenb = ofile_lst[desc].pmntdev->inodenb_offset;
            //
            ofile_lst[desc].offset += RD_SIZE;
            break;
         }
      }while(1);
   }
   //or entry is in user data
   else {
      do {
         ////
         //we chain cluster
         if(!(current_addr = _fat16_adddir(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info, desc))) {
            return -1;
         }

         //read medium
         if(_fat_read_data(__get_dev_desc(desc),current_addr,(unsigned char *)&fat_entry.msdos_entry,RD_SIZE)<0) {
            return -1;
         }

         //entry empty
         if(fat_entry.msdos_entry.DIR_Name[0]==RD_ENT_FREE) {
            return -1;
         }
         // or entry  remove
         if(fat_entry.msdos_entry.DIR_Name[0]==RD_ENT_REMOVE) {
            ofile_lst[desc].offset += RD_SIZE;
            continue;
         }

         //or long name entry
         if((fat_entry.vfat_entry.LDIR_Ord&FAT16_LAST_LONGENTRY_MSK)
               && (fat_entry.vfat_entry.LDIR_Attr&FAT16_LONGNAME_MSK)==RD_ATTR_LONGNAME) {
            //rewind offset and get name
            ofile_lst[__get_dev_desc(desc)].pfsop->fdev.fdev_seek(__get_dev_desc(desc),current_addr-RD_SIZE,SEEK_SET);
            if(_fat16_vfat_build_oob_name(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info,
                  __get_dev_desc(desc),dirent->d_name,__vfat_get_nb_long_entries(fat_entry.vfat_entry),
                  fat16_ofile_lst[desc].entry_data_cluster) < 0)
               return -1;

            dirent->inodenb = __cvt2logicnode(desc,(__get_BPB_RootEntCnt(ofile_lst[desc].pmntdev)+(current_addr-__get_ud_addr(ofile_lst[desc].pmntdev))/RD_SIZE));
            //inc offset
            ofile_lst[desc].offset += RD_SIZE*(__vfat_get_nb_long_entries(fat_entry.vfat_entry)+1);
            break;
         }
         //
         //compute inode
         //two specials case "."
         if(!strncmp(fat_entry.msdos_entry.DIR_Name,". ",2)) {
            fat16_u32_t Add = _fat16_vfat_get_addr_dot_items(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info,
                  desc, fat16_ofile_lst[desc].entry_data_cluster,
                  (fat_entry.msdos_entry.DIR_FstClusLO[0] + ((fat16_u16_t)fat_entry.msdos_entry.DIR_FstClusLO[1]<<8)));

            //rootdir
            if(Add < __get_rd_addr(ofile_lst[desc].pmntdev)+__get_BPB_RootEntCnt(ofile_lst[desc].pmntdev)*RD_SIZE) {
               //calculate real inode
               dirent->inodenb = __cvt2logicnode(desc,(Add-__get_rd_addr(ofile_lst[desc].pmntdev))/RD_SIZE);
            }
            //or not
            else {
               dirent->inodenb = __cvt2logicnode(desc,(__get_BPB_RootEntCnt(ofile_lst[desc].pmntdev)+(Add-__get_ud_addr(ofile_lst[desc].pmntdev))/RD_SIZE));
            }
            //
            strncpy(dirent->d_name, ".\0", 2);
            ofile_lst[desc].offset += RD_SIZE;
            break;
         }
         //and ".."
         else if(!strncmp(fat_entry.msdos_entry.DIR_Name,".. ",3)) {
            fat16_u32_t Add = _fat16_cluster_add(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info, (fat_entry.msdos_entry.DIR_FstClusLO[0] + ((fat16_u16_t)fat_entry.msdos_entry.DIR_FstClusLO[1]<<8)));

            //parent can be in rootdir
            if(!Add) {
               Add = _fat16_vfat_get_addr_dot_items(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info,
                                 desc,
                                 fat16_ofile_lst[desc].entry_data_cluster,
                                 (fat_entry.msdos_entry.DIR_FstClusLO[0] + ((fat16_u16_t)fat_entry.msdos_entry.DIR_FstClusLO[1]<<8)));
               dirent->inodenb = __cvt2logicnode(desc,(Add-__get_rd_addr(ofile_lst[desc].pmntdev))/RD_SIZE);
            }
            //or not
            else {
               Add = _fat16_vfat_get_addr_dot_items(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info,
                                                desc,
                                                (fat_entry.msdos_entry.DIR_FstClusLO[0] + ((fat16_u16_t)fat_entry.msdos_entry.DIR_FstClusLO[1]<<8)),
                                                (fat_entry.msdos_entry.DIR_FstClusLO[0] + ((fat16_u16_t)fat_entry.msdos_entry.DIR_FstClusLO[1]<<8))
                                                );
               //rootdir
               if(Add < (__get_rd_addr(ofile_lst[desc].pmntdev)+__get_BPB_RootEntCnt(ofile_lst[desc].pmntdev)*RD_SIZE)) {
                  dirent->inodenb = __cvt2logicnode(desc,(Add-__get_rd_addr(ofile_lst[desc].pmntdev))/RD_SIZE);
               }
               //or not
               else {
                  dirent->inodenb = __cvt2logicnode(desc,(__get_BPB_RootEntCnt(ofile_lst[desc].pmntdev)+(Add-__get_ud_addr(ofile_lst[desc].pmntdev))/RD_SIZE));
               }
            }
            //
            strncpy(dirent->d_name, "..\0", 3);
            ofile_lst[desc].offset += RD_SIZE;
            break;
         }

      }while(1);
   }
   return 0;
}


/*-------------------------------------------
| Name:_fat_vfat_lookupdir
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
inodenb_t _fat_vfat_lookupdir(desc_t desc,char* filename) {
   inodenb_t _ino_logic=INVALID_INODE_NB;
   inodenb_t _ino_phys=__cvt2physnode(desc,ofile_lst[desc].inodenb);
   fat16_u32_t off=0;
   fat16_u32_t addr=0;
   fat_dir_entry_t fat_entry={0};
   int nb_ent=0;

   //root of rootdir
   if(!_ino_phys) {
      if(!strncmp(filename,"..",2)) {
         _ino_logic = 0;//ofile_lst[desc].pmntdev->inodenb_offset;
      }
      else if(!strncmp(filename,".",1)) {
         _ino_logic = ofile_lst[desc].inodenb;
      }
      else {
         if(_fat16_vfat_lookupname_rd(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info,desc,filename)==-1)
            return INVALID_INODE_NB;

         //affect logic inode
         off = fat16_ofile_lst[desc].entry_phys_addr-__get_rd_addr(ofile_lst[desc].pmntdev);
         _ino_logic = __cvt2logicnode(desc,(off/RD_SIZE));
      }
   }
   //or entry  is in rootdir
   else if(_ino_phys<__get_BPB_RootEntCnt(ofile_lst[desc].pmntdev)){
      if(!strncmp(filename,"..",2)) {
         _ino_logic = ofile_lst[desc].pmntdev->inodenb_offset;
      }
      else if(!strncmp(filename,".",1)) {
         _ino_logic = ofile_lst[desc].inodenb;
      }
      else {
         //read dot and fill DataCluster
         addr = __get_rd_addr(ofile_lst[desc].pmntdev)+RD_SIZE*_ino_phys;
         if(_fat_read_data(__get_dev_desc(desc),addr,(unsigned char *)&fat_entry.msdos_entry,RD_SIZE)<0) {
            return INVALID_INODE_NB;
         }


         //extract nb long entry and move to short entry
         if((fat_entry.vfat_entry.LDIR_Ord&FAT16_LAST_LONGENTRY_MSK)
               && (fat_entry.vfat_entry.LDIR_Attr&FAT16_LONGNAME_MSK)==RD_ATTR_LONGNAME) {
            nb_ent = __vfat_get_nb_long_entries(fat_entry.vfat_entry);

            //already one entry skip by previous read
            if(_fat_read_data(__get_dev_desc(desc),addr+(nb_ent)*RD_SIZE,(unsigned char *)&fat_entry.msdos_entry,RD_SIZE)<0) {
               return INVALID_INODE_NB;
            }
         }

         //
         fat16_ofile_lst[desc].entry_data_cluster = (fat_entry.msdos_entry.DIR_FstClusLO[0] + ((fat16_u16_t)fat_entry.msdos_entry.DIR_FstClusLO[1]<<8));
         if(_fat16_vfat_lookupname_ud(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info,desc,filename)==-1)
            return INVALID_INODE_NB;

         //affect logic inode
         off = __get_BPB_RootEntCnt(ofile_lst[desc].pmntdev)+(fat16_ofile_lst[desc].entry_phys_addr-__get_ud_addr(ofile_lst[desc].pmntdev))/RD_SIZE;
         _ino_logic = __cvt2logicnode(desc,off);
      }
   }
   else {
      if(!strncmp(filename,"..",2)) {
         fat16_u16_t cluster=0;
         //get long entry
         addr = __get_ud_addr(ofile_lst[desc].pmntdev) + (_ino_phys-__get_BPB_RootEntCnt(ofile_lst[desc].pmntdev))*RD_SIZE;
         if(_fat_read_data(__get_dev_desc(desc),addr,(unsigned char *)&fat_entry.msdos_entry,RD_SIZE)<0) {
            return INVALID_INODE_NB;
         }

         //get infos of long entry
         if((fat_entry.vfat_entry.LDIR_Attr&FAT16_LONGNAME_MSK)==RD_ATTR_LONGNAME) {
            cluster = (addr-__get_ud_addr(ofile_lst[desc].pmntdev))/
                  (__get_BPB_SecPerClus(ofile_lst[desc].pmntdev)*__get_BPB_BytesPerSec(ofile_lst[desc].pmntdev))
                  +FAT16_CLUSMIN;
            _fat16_vfat_get_infos_addr(desc, cluster, __vfat_get_nb_long_entries(fat_entry.vfat_entry),&addr);
         }
         //and read it
         if(_fat_read_data(__get_dev_desc(desc),fat16_ofile_lst[desc].entry_infos_addr,(unsigned char *)&fat_entry.msdos_entry,RD_SIZE)<0) {
            return INVALID_INODE_NB;
         }

         //move to cluster and get .. addr
         addr = _fat16_cluster_add(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info,(fat_entry.msdos_entry.DIR_FstClusLO[0] + ((fat16_u16_t)fat_entry.msdos_entry.DIR_FstClusLO[1]<<8)));
         if(_fat_read_data(__get_dev_desc(desc),addr+RD_SIZE,(unsigned char *)&fat_entry.msdos_entry,RD_SIZE)<0) {
            return INVALID_INODE_NB;
         }

         //get real addr of dotdot
         addr = _fat16_vfat_get_addr_dot_items(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info,
                           desc,
                           (fat_entry.msdos_entry.DIR_FstClusLO[0] + ((fat16_u16_t)fat_entry.msdos_entry.DIR_FstClusLO[1]<<8)),
                           (fat_entry.msdos_entry.DIR_FstClusLO[0] + ((fat16_u16_t)fat_entry.msdos_entry.DIR_FstClusLO[1]<<8))
                           );
         //rootdir
         if(addr<(__get_rd_addr(ofile_lst[desc].pmntdev)+__get_BPB_RootEntCnt(ofile_lst[desc].pmntdev)*RD_SIZE)) {
            _ino_logic = __cvt2logicnode(desc,((addr-__get_rd_addr(ofile_lst[desc].pmntdev))/RD_SIZE));
         }
         //or not
         else {
            _ino_logic = __cvt2logicnode(desc,(__get_BPB_RootEntCnt(ofile_lst[desc].pmntdev)+(addr-__get_ud_addr(ofile_lst[desc].pmntdev))/RD_SIZE));
         }
      }
      else if(!strncmp(filename,".",1)) {
         _ino_logic = ofile_lst[desc].inodenb;
      }
      else {
         fat16_u16_t cluster=0;

         //get first long entry
         addr = ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info->ud_addr + (_ino_phys-ofile_lst[desc].pmntdev->fs_info.fat_info.fat_boot_info->bpb.BPB_RootEntCnt)*RD_SIZE;
         if(_fat_read_data(__get_dev_desc(desc),addr,(unsigned char *)&fat_entry.msdos_entry,RD_SIZE)<0) {
            return INVALID_INODE_NB;
         }

         //get infos aka short entry address
         if((fat_entry.vfat_entry.LDIR_Attr&FAT16_LONGNAME_MSK)==RD_ATTR_LONGNAME) {
            cluster = (addr-__get_ud_addr(ofile_lst[desc].pmntdev))/
                  (__get_BPB_SecPerClus(ofile_lst[desc].pmntdev)*__get_BPB_BytesPerSec(ofile_lst[desc].pmntdev))
                  +FAT16_CLUSMIN;
            addr = _fat16_vfat_get_infos_addr(desc, cluster, __vfat_get_nb_long_entries(fat_entry.vfat_entry),&addr);
         }

         //get cluster no
         if(_fat_read_data(__get_dev_desc(desc),addr,(unsigned char *)&fat_entry.msdos_entry,RD_SIZE)<0) {
            return INVALID_INODE_NB;
         }
         fat16_ofile_lst[desc].entry_data_cluster = (fat_entry.msdos_entry.DIR_FstClusLO[0] + ((fat16_u16_t)fat_entry.msdos_entry.DIR_FstClusLO[1]<<8));

         //
         if(_fat16_vfat_lookupname_ud(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info,desc,filename)==-1)
            return INVALID_INODE_NB;

         //affect logic inode
         off = __get_BPB_RootEntCnt(ofile_lst[desc].pmntdev)+(fat16_ofile_lst[desc].entry_phys_addr-__get_ud_addr(ofile_lst[desc].pmntdev))/RD_SIZE;
         _ino_logic = __cvt2logicnode(desc,off);
      }
   }

   return _ino_logic;
}

/*-------------------------------------------
| Name:_fat_vfat_create
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
inodenb_t _fat_vfat_create(desc_t desc,char* filename, int attr) {
   fat_dir_entry_t fat_entry={0};
   unsigned char chksum=0;
   int count_req_ent = 0;
   int i=0;

   fat16_u32_t addr=0, prev_addr=0;
   inodenb_t _ino_logic=0;
   inodenb_t _ino_phys = __cvt2physnode(desc,ofile_lst[desc].inodenb);
   fat16_u16_t cluster=0, parent_cluster=0, prev_cluster=0;
   fat16_u16_t tmp_cluster=0;

   int rem_ent=0;
   int avail_ent=0;

   //how many entries + short entry
   count_req_ent = (!(strlen(filename)%FAT16_LONGNAME_SUM))?strlen(filename)/FAT16_LONGNAME_SUM:strlen(filename)/FAT16_LONGNAME_SUM+1;
   count_req_ent++;

   //root directory is parent
   if(!_ino_phys) {
      //it's root directory
      parent_cluster=0;
      addr = __get_rd_addr(ofile_lst[desc].pmntdev)+RD_SIZE;
      if((_ino_logic = _fat16_vfat_request_free_rd(desc, &addr, count_req_ent))<0) {
         return INVALID_INODE_NB;
      }
   }
   //parent is user data
   else {
      cluster = fat16_ofile_lst[desc].entry_data_cluster;
      prev_cluster = cluster;
      parent_cluster = cluster;
      addr = _fat16_cluster_add(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info, cluster);

      if((_ino_logic =  _fat16_vfat_request_free_ud(desc, &addr, &cluster, &prev_addr, &prev_cluster, count_req_ent, &rem_ent))<0)
         return INVALID_INODE_NB;
      avail_ent = count_req_ent-rem_ent;
   }


   //create short, checksum and longname
   memset((void *)vfat_tab_long_entry, 0, FAT16_MAX_LONGNAME_TAB_ENTRY*sizeof(fat_vfat_dir_entry_t));
   _fat16_vfat_create_short_name(fat_entry.msdos_entry.DIR_Name);
   chksum = _fat16_vfat_chksum(fat_entry.msdos_entry.DIR_Name);
   if((count_req_ent = _fat16_vfat_create_long_name(filename,vfat_tab_long_entry, chksum))<0)
         return -1;

   //fill entry
   tmp_cluster = _fat16_create_fat_entry(&fat_entry.msdos_entry, desc, attr);

   //
   if(prev_cluster == cluster) {
      addr = _fat16_vfat_write_long_entries(__get_dev_desc(desc), addr, vfat_tab_long_entry, count_req_ent-1, count_req_ent);
   }
   else {
      //short entry remain
      if(rem_ent==1) {
         _fat16_vfat_write_long_entries(__get_dev_desc(desc), prev_addr, vfat_tab_long_entry, count_req_ent-1, avail_ent);
      }
      else {
         //or write long name in 2 different clusters
         _fat16_vfat_write_long_entries(__get_dev_desc(desc), prev_addr, vfat_tab_long_entry, count_req_ent-1, avail_ent);
         addr = _fat16_vfat_write_long_entries(__get_dev_desc(desc), addr, vfat_tab_long_entry, count_req_ent-1-avail_ent, rem_ent-1);
      }
   }

   //write short entry
   if(_fat_write_data(__get_dev_desc(desc),addr,(unsigned char *)&fat_entry.msdos_entry,RD_SIZE)<0) {
      return INVALID_INODE_NB;
   }


   //create dot and dot dot for directory
   if(attr == S_IFDIR) {
      if(_fat16_create_fat_directory_entry(&fat_entry.msdos_entry, desc, tmp_cluster, parent_cluster)<0)
         return INVALID_INODE_NB;
   }

   //affect inode number
   ofile_lst[desc].inodenb = __cvt2logicnode(desc,_ino_logic);
   return _ino_logic;
}

/*-------------------------------------------
| Name:_fat_vfat_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _fat_vfat_open(desc_t desc) {
   inodenb_t _ino_phys= __cvt2physnode(desc,ofile_lst[desc].inodenb);
   fat_dir_entry_t fat_entry={0};
   fat16_u32_t addr=0;
   fat16_u16_t cluster=0;

   //mount point
   if(!_ino_phys) {
      fat16_ofile_lst[desc].entry_phys_addr = __get_rd_addr(ofile_lst[desc].pmntdev)+RD_SIZE;
      fat16_ofile_lst[desc].entry_data_cluster = 0;
      fat16_ofile_lst[desc].entry_attr = RD_ATTR_VOLUMEID;
      fat16_ofile_lst[desc].entry_infos_addr = 0;

      ofile_lst[desc].attr = S_IFDIR;
      ofile_lst[desc].offset = 0;
      return -1;
   }
   else {
      //metadatas are in root dir
      if(_ino_phys<__get_BPB_RootEntCnt(ofile_lst[desc].pmntdev)) {
         addr = __get_rd_addr(ofile_lst[desc].pmntdev)+RD_SIZE*_ino_phys;

         if(_fat_read_data(__get_dev_desc(desc),addr,(unsigned char *)&fat_entry.msdos_entry,RD_SIZE)<0) {
            return -1;
         }

         if((fat_entry.vfat_entry.LDIR_Attr&FAT16_LONGNAME_MSK)==RD_ATTR_LONGNAME) {
            //
            fat16_ofile_lst[desc].entry_infos_addr = addr+((fat_entry.vfat_entry.LDIR_Ord&~FAT16_LAST_LONGENTRY_MSK))*RD_SIZE;
            _fat16_getdesc(desc, addr);
         }
      }
      //metadatas or in user data
      else {
         //
         addr = __get_ud_addr(ofile_lst[desc].pmntdev) + (_ino_phys-__get_BPB_RootEntCnt(ofile_lst[desc].pmntdev))*RD_SIZE;
         //
         if(_fat_read_data(__get_dev_desc(desc),addr,(unsigned char *)&fat_entry.msdos_entry,RD_SIZE)<0) {
            return -1;
         }

         if((fat_entry.vfat_entry.LDIR_Attr&FAT16_LONGNAME_MSK)==RD_ATTR_LONGNAME) {
            cluster = (addr-__get_ud_addr(ofile_lst[desc].pmntdev))/
                 (__get_BPB_SecPerClus(ofile_lst[desc].pmntdev)*__get_BPB_BytesPerSec(ofile_lst[desc].pmntdev))
                 +FAT16_CLUSMIN;
            //
            _fat16_vfat_get_infos_addr(desc, cluster, __vfat_get_nb_long_entries(fat_entry.vfat_entry),&addr);
            _fat16_getdesc(desc, addr);
         }
      }
   }
   return 0;
}


/*-------------------------------------------
| Name:_fat_vfat_remove
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _fat_vfat_remove(desc_t desc_ancst,desc_t desc) {
   fat16_u8_t rem_pattern = RD_ENT_REMOVE;

   //check if directory is empty
   if (fat16_ofile_lst[desc].entry_attr == RD_ATTR_DIRECTORY) {
      if (_fat16_checkdirempty(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info,desc)==-1) {
         return -1;
      }
   }
   //free all cluster
   if (fat16_ofile_lst[desc].entry_data_cluster != RD_CLUSEMPTY) {
      _fat16_delcluslist(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info, desc, fat16_ofile_lst[desc].entry_data_cluster);
   }

   //remove long entries
   if(_fat16_vfat_remove_entries(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info, desc)<0) {
      return -1;
   }

   //remove short entry
   if(_fat_write_data(__get_dev_desc(desc),fat16_ofile_lst[desc].entry_infos_addr,&rem_pattern,1)<0) {
      return -1;
   }

   return 0;
}

/*-------------------------------------------
| Name:_fat_vfat_rename
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _fat_vfat_rename(desc_t desc,const char*  old_name, char* new_name) {
   inodenb_t _ino_phys=__cvt2physnode(desc, ofile_lst[desc].inodenb);
   desc_t _desc_old=INVALID_INODE_NB;
   fat16_u8_t rem_pattern = RD_ENT_REMOVE;
   fat16_u32_t addr=0;
   fat_dir_entry_t fat_entry={0};

   int count_req_ent = (!(strlen(new_name)%FAT16_LONGNAME_SUM))?strlen(new_name)/FAT16_LONGNAME_SUM:strlen(new_name)/FAT16_LONGNAME_SUM+1;
   count_req_ent++;

   _desc_old = _vfs_getdesc(_desc_old,desc);
   memcpy((void *)&fat16_ofile_lst[_desc_old], (void *)&fat16_ofile_lst[desc], sizeof(fat16_ofile_t));

   //root directory
   if(!_ino_phys) {
      if(_fat16_vfat_lookupname_rd(ofile_lst[_desc_old].pmntdev->fs_info.fat_info.fat_core_info, _desc_old, (char*)old_name)<0)
         return -1;

      //save short entry
      if(_fat_read_data(__get_dev_desc(_desc_old),fat16_ofile_lst[_desc_old].entry_infos_addr,(unsigned char *)&fat_entry.msdos_entry,RD_SIZE)<0)
         return -1;

      //remove long entries and short entry
      if(_fat16_vfat_remove_entries(ofile_lst[_desc_old].pmntdev->fs_info.fat_info.fat_core_info, _desc_old)<0) {
         return -1;
      }
      if(_fat_write_data(__get_dev_desc(_desc_old),fat16_ofile_lst[_desc_old].entry_infos_addr,&rem_pattern,1)<0) {
         return -1;
      }

      //not enough place to store new_name in root directory
      addr = __get_rd_addr(ofile_lst[_desc_old].pmntdev)+RD_SIZE;
      if(_fat16_vfat_request_free_rd(_desc_old, &addr, count_req_ent)<0) {
         return -1;
      }

      //write new long entries
      memset((void *)vfat_tab_long_entry, 0, FAT16_MAX_LONGNAME_TAB_ENTRY*sizeof(fat_vfat_dir_entry_t));
      if((count_req_ent = _fat16_vfat_create_long_name(new_name,vfat_tab_long_entry, fat_entry.vfat_entry.LDIR_Chksum))<0)
         return -1;

      addr = _fat16_vfat_write_long_entries(__get_dev_desc(_desc_old), addr, vfat_tab_long_entry, count_req_ent-1, count_req_ent);
   }
   else {
      fat16_u16_t cluster=0, prev_cluster=0;
      fat16_u32_t prev_addr=0;
      int avail_ent=0, rem_ent=0;

      //
      if(_fat16_vfat_lookupname_ud(ofile_lst[_desc_old].pmntdev->fs_info.fat_info.fat_core_info, _desc_old, (char*)old_name)<0)
         return -1;

      //save short entry
      if(_fat_read_data(__get_dev_desc(_desc_old),fat16_ofile_lst[_desc_old].entry_infos_addr,(unsigned char *)&fat_entry.msdos_entry,RD_SIZE)<0)
         return -1;

      //remove long entries and short entry
      if(_fat16_vfat_remove_entries(ofile_lst[_desc_old].pmntdev->fs_info.fat_info.fat_core_info, _desc_old)<0) {
         return -1;
      }
      if(_fat_write_data(__get_dev_desc(_desc_old),fat16_ofile_lst[_desc_old].entry_infos_addr,&rem_pattern,1)<0) {
         return -1;
      }

      //
      cluster = fat16_ofile_lst[desc].entry_data_cluster;
      prev_cluster = cluster;
      addr = _fat16_cluster_add(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info, cluster);

      if(_fat16_vfat_request_free_ud(_desc_old, &addr, &cluster, &prev_addr, &prev_cluster, count_req_ent, &rem_ent)<0)
         return INVALID_INODE_NB;
      avail_ent = count_req_ent-rem_ent;

      //create new long entries and write it
      memset((void *)vfat_tab_long_entry, 0, FAT16_MAX_LONGNAME_TAB_ENTRY*sizeof(fat_vfat_dir_entry_t));
      if((count_req_ent = _fat16_vfat_create_long_name(new_name,vfat_tab_long_entry, fat_entry.vfat_entry.LDIR_Chksum))<0)
         return -1;

      if(prev_cluster == cluster) {
         addr = _fat16_vfat_write_long_entries(__get_dev_desc(desc), addr, vfat_tab_long_entry, count_req_ent-1, count_req_ent);
      }
      else {
         if(rem_ent==1) {
            _fat16_vfat_write_long_entries(__get_dev_desc(_desc_old), prev_addr, vfat_tab_long_entry, count_req_ent-1, avail_ent);
         }
         else {
            //or write long name in 2 different clusters
            _fat16_vfat_write_long_entries(__get_dev_desc(_desc_old), prev_addr, vfat_tab_long_entry, count_req_ent-1, avail_ent);
            addr = _fat16_vfat_write_long_entries(__get_dev_desc(_desc_old), addr, vfat_tab_long_entry, count_req_ent-1-avail_ent, rem_ent-1);
         }
      }
   }

   //and short entry
   if(_fat_write_data(__get_dev_desc(_desc_old),addr,(unsigned char *)&fat_entry.msdos_entry,RD_SIZE)<0) {
      return -1;
   }
   _vfs_putdesc(_desc_old);
   return 0;
}

/*============================================
| End of Source  : fat16_vfat.c
|---------------------------------------------
| Historic:
|---------------------------------------------
| Authors     | Date     | Comments
| $Log:$
==============================================*/
