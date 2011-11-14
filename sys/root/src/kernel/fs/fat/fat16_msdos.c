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
/*============================================
| Global Declaration
==============================================*/
int _fat_msdos_statfs(mntdev_t* pmntdev,struct statvfs *statvfs);
int _fat_msdos_readdir(desc_t desc,dirent_t* dirent);
inodenb_t _fat_msdos_lookupdir(desc_t desc,char* filename);
inodenb_t _fat_msdos_create(desc_t desc,char* filename, int attr);
int _fat_msdos_open(desc_t desc);
int _fat_msdos_remove(desc_t desc_ancst,desc_t desc);
int _fat_msdos_rename(desc_t desc,const char*  old_name, char* new_name);

//file system operations
fsop_t fat_msdos_op={
      _fat_loadfs,
      _fat_checkfs,
      _fat_makefs,
      _fat_readfs,
      _fat_writefs,
      _fat_msdos_statfs,
      _fat_mountdir,
      _fat_msdos_readdir,
      _fat_telldir,
      _fat_seekdir,
      _fat_msdos_lookupdir,
      _fat_mknod,
      _fat_msdos_create,
      _fat_msdos_open,
      _fat_close,
      _fat_read,
      _fat_write,
      _fat_seek,
      _fat_truncate,
      _fat_msdos_remove,
      _fat_msdos_rename
};

/*============================================
| Implementation
==============================================*/
/*-------------------------------------------
| Name:_fat_msdos_statfs
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _fat_msdos_statfs(mntdev_t* pmntdev,struct statvfs *statvfs) {
   int i=0;
   unsigned short cluster_value=0;
   memset(statvfs,0,sizeof(struct statvfs));
   //

   statvfs->f_bsize = __get_BPB_SecPerClus(pmntdev)*__get_BPB_BytesPerSec(pmntdev);
   //number of cluster
   statvfs->f_blocks = __get_BPB_TotSec16(pmntdev)?__get_BPB_TotSec16(pmntdev):__get_BPB_TotSec32(pmntdev);
   statvfs->f_namemax = FAT16_MAX_NAME_FIL+FAT16_MAX_EXT_FIL;

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
| Name:_fat_msdos_readdir
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _fat_msdos_readdir(desc_t desc,dirent_t* dirent) {
   fat16_u32_t current_addr;
   fat_msdos_dir_entry_t fat_entry={0};
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
         if(_fat_read_data(__get_dev_desc(desc),current_addr,(unsigned char *)&fat_entry,RD_SIZE)<0) {
            return -1;
         }

         //entry empty
         if(fat_entry.DIR_Name[0]==RD_ENT_FREE) {
            return -1;
         }
         // or entry  remove
         if(fat_entry.DIR_Name[0]==RD_ENT_REMOVE) {
            ofile_lst[desc].offset += RD_SIZE;
            continue;
         }

         //fill dirent
         _fat16_msdos_fillname(dirent->d_name, fat_entry.DIR_Name);
         dirent->inodenb = __cvt2logicnode(desc,((current_addr-__get_rd_addr(ofile_lst[desc].pmntdev))/RD_SIZE));
         ofile_lst[desc].offset += RD_SIZE;

         break;
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
         if(_fat_read_data(__get_dev_desc(desc),current_addr,(unsigned char *)&fat_entry,RD_SIZE)<0) {
            return -1;
         }

         //entry empty
         if(fat_entry.DIR_Name[0]==RD_ENT_FREE) {
            return -1;
         }
         // or entry  remove
         if(fat_entry.DIR_Name[0]==RD_ENT_REMOVE) {
            ofile_lst[desc].offset += RD_SIZE;
            continue;
         }

         //compute inode
         //two specials case "." and ".."
         if(!strncmp(fat_entry.DIR_Name,". ",2)) {
            dirent->inodenb = ofile_lst[desc].inodenb;
         }
         else if(!strncmp(fat_entry.DIR_Name,".. ",3)) {
            dirent->inodenb = ofile_lst[desc].pmntdev->inodenb_offset;
         }
         else {
            dirent->inodenb = dirent->inodenb = __cvt2logicnode(desc,(__get_BPB_RootEntCnt(ofile_lst[desc].pmntdev)+(current_addr-__get_ud_addr(ofile_lst[desc].pmntdev))/RD_SIZE));
         }
         //copy name
         _fat16_msdos_fillname(dirent->d_name, fat_entry.DIR_Name);
         ofile_lst[desc].offset += RD_SIZE;

         break;
      }while(1);
   }
   //or entry in in userdata
   else {
      do {
         //we chain cluster
         if(!(current_addr = _fat16_adddir(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info, desc))) {
            return -1;
         }

         //read medium
         if(_fat_read_data(__get_dev_desc(desc),current_addr,(unsigned char *)&fat_entry,RD_SIZE)<0) {
            return -1;
         }

         //entry empty
         if(fat_entry.DIR_Name[0]==RD_ENT_FREE) {
            return -1;
         }
         // or entry  remove
         if(fat_entry.DIR_Name[0]==RD_ENT_REMOVE) {
            ofile_lst[desc].offset += RD_SIZE;
            continue;
         }

         //compute inode
         //two specials case "."
         if(!strncmp(fat_entry.DIR_Name,". ",2)) {
            fat16_u32_t Add = _fat16_msdos_get_addr_dot_items(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info,
                              desc, fat16_ofile_lst[desc].entry_data_cluster,
                              (fat_entry.DIR_FstClusLO[0] + ((fat16_u16_t)fat_entry.DIR_FstClusLO[1]<<8)));

            //rootdir
            if(Add < (__get_rd_addr(ofile_lst[desc].pmntdev)+__get_BPB_RootEntCnt(ofile_lst[desc].pmntdev)*RD_SIZE)) {
               //calculate real inode
               dirent->inodenb = __cvt2logicnode(desc,((Add-__get_rd_addr(ofile_lst[desc].pmntdev))/RD_SIZE));
            }
            //or not
            else {
               dirent->inodenb = __cvt2logicnode(desc,(__get_BPB_RootEntCnt(ofile_lst[desc].pmntdev)+(Add-__get_ud_addr(ofile_lst[desc].pmntdev))/RD_SIZE));
            }
         }
         //and ".."
         else if(!strncmp(fat_entry.DIR_Name,".. ",3)) {
            fat16_u32_t Add = _fat16_cluster_add(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info, (fat_entry.DIR_FstClusLO[0] + ((fat16_u16_t)fat_entry.DIR_FstClusLO[1]<<8)));

            //parent can be in rootdir
            if(!Add) {
               Add = _fat16_msdos_get_addr_dot_items(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info,
                                 desc,
                                 fat16_ofile_lst[desc].entry_data_cluster,
                                 (fat_entry.DIR_FstClusLO[0] + ((fat16_u16_t)fat_entry.DIR_FstClusLO[1]<<8)));
               dirent->inodenb = __cvt2logicnode(desc,((Add-__get_rd_addr(ofile_lst[desc].pmntdev))/RD_SIZE));
            }
            //or not
            else {
               Add = _fat16_msdos_get_addr_dot_items(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info,
                                 desc,
                                 (fat_entry.DIR_FstClusLO[0] + ((fat16_u16_t)fat_entry.DIR_FstClusLO[1]<<8)),
                                 (fat_entry.DIR_FstClusLO[0] + ((fat16_u16_t)fat_entry.DIR_FstClusLO[1]<<8))
                                 );

               //rootdir
               if(Add < (__get_rd_addr(ofile_lst[desc].pmntdev)+__get_BPB_RootEntCnt(ofile_lst[desc].pmntdev)*RD_SIZE)) {
                  dirent->inodenb = __cvt2logicnode(desc,((Add-__get_rd_addr(ofile_lst[desc].pmntdev))/RD_SIZE));
               }
               //or not
               else {
                  dirent->inodenb = __cvt2logicnode(desc,(__get_BPB_RootEntCnt(ofile_lst[desc].pmntdev)+(Add-__get_ud_addr(ofile_lst[desc].pmntdev))/RD_SIZE));
               }
            }
         }
         else {
            dirent->inodenb = __cvt2logicnode(desc,(__get_BPB_RootEntCnt(ofile_lst[desc].pmntdev)+(current_addr-__get_ud_addr(ofile_lst[desc].pmntdev))/RD_SIZE));
         }
         //copy name
         _fat16_msdos_fillname(dirent->d_name, fat_entry.DIR_Name);
         ofile_lst[desc].offset += RD_SIZE;

         break;
      }while(1);
   }

   return 0;
}


/*-------------------------------------------
| Name:_fat_msdos_lookupdir
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
inodenb_t _fat_msdos_lookupdir(desc_t desc,char* filename) {
   inodenb_t _ino_logic=INVALID_INODE_NB;
   inodenb_t _ino_phys=__cvt2physnode(desc,ofile_lst[desc].inodenb);
   fat16_u32_t off=0;
   char fat_filename[FAT16_MAX_NAME_FIL+FAT16_MAX_EXT_FIL+1]={0};
   fat16_u32_t addr=0;
   fat_msdos_dir_entry_t fat_entry={0};

   //name too long
   if(_fat16_msdos_getname(filename, fat_filename))
      return INVALID_INODE_NB;

   //root of rootdir
   if(!_ino_phys) {
      if(!strncmp(fat_filename,".. ",3)) {
         _ino_logic = 0;//ofile_lst[desc].pmntdev->inodenb_offset;
      }
      else if(!strncmp(fat_filename,". ",2)) {
         _ino_logic = ofile_lst[desc].inodenb;
      }
      else {
         if(_fat16_msdos_lookupname_rd(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info,desc,fat_filename)==-1)
            return INVALID_INODE_NB;

         //affect logic inode
         off = fat16_ofile_lst[desc].entry_phys_addr-__get_rd_addr(ofile_lst[desc].pmntdev);
         _ino_logic = __cvt2logicnode(desc,(off/RD_SIZE));
      }
   }
   //or entry  is in rootdir
   else if(_ino_phys<__get_BPB_RootEntCnt(ofile_lst[desc].pmntdev)){
      if(!strncmp(fat_filename,".. ",3)) {
         _ino_logic = ofile_lst[desc].pmntdev->inodenb_offset;
      }
      else if(!strncmp(fat_filename,". ",2)) {
         _ino_logic = ofile_lst[desc].inodenb;
      }
      else {
         //read dot and fill DataCluster
         addr = __get_rd_addr(ofile_lst[desc].pmntdev)+RD_SIZE*_ino_phys;
         if(_fat_read_data(__get_dev_desc(desc),addr,(unsigned char *)&fat_entry,RD_SIZE)<0) {
            return INVALID_INODE_NB;
         }

         fat16_ofile_lst[desc].entry_data_cluster = (fat_entry.DIR_FstClusLO[0] + ((fat16_u16_t)fat_entry.DIR_FstClusLO[1]<<8));
         if(_fat16_msdos_lookupname_ud(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info,desc,fat_filename)==-1)
            return INVALID_INODE_NB;

         //affect logic inode
         off = __get_BPB_RootEntCnt(ofile_lst[desc].pmntdev)+(fat16_ofile_lst[desc].entry_phys_addr-__get_ud_addr(ofile_lst[desc].pmntdev))/RD_SIZE;
         _ino_logic = __cvt2logicnode(desc,off);
      }
   }
   else {
      if(!strncmp(fat_filename,".. ",3)) {
         //read dotdot and fill DataCluster
         addr = __get_ud_addr(ofile_lst[desc].pmntdev) + (_ino_phys-__get_BPB_RootEntCnt(ofile_lst[desc].pmntdev))*RD_SIZE;
         if(_fat_read_data(__get_dev_desc(desc),addr,(unsigned char *)&fat_entry,RD_SIZE)<0) {
            return INVALID_INODE_NB;
         }

         addr = _fat16_cluster_add(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info, (fat_entry.DIR_FstClusLO[0] + ((fat16_u16_t)fat_entry.DIR_FstClusLO[1]<<8)));
         if(_fat_read_data(__get_dev_desc(desc),addr+RD_SIZE,(unsigned char *)&fat_entry,RD_SIZE)<0) {
            return INVALID_INODE_NB;
         }

         //get real addr of dotdot
         addr = _fat16_msdos_get_addr_dot_items(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info,
                           desc,
                           (fat_entry.DIR_FstClusLO[0] + ((fat16_u16_t)fat_entry.DIR_FstClusLO[1]<<8)),
                           (fat_entry.DIR_FstClusLO[0] + ((fat16_u16_t)fat_entry.DIR_FstClusLO[1]<<8))
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
      else if(!strncmp(fat_filename,". ",2)) {
         _ino_logic = ofile_lst[desc].inodenb;
      }
      else {
         addr = __get_ud_addr(ofile_lst[desc].pmntdev) + (_ino_phys-__get_BPB_RootEntCnt(ofile_lst[desc].pmntdev))*RD_SIZE;
         //read dotdot and file DataCluster
         if(_fat_read_data(__get_dev_desc(desc),addr,(unsigned char *)&fat_entry,RD_SIZE)<0) {
            return INVALID_INODE_NB;
         }
         fat16_ofile_lst[desc].entry_data_cluster = (fat_entry.DIR_FstClusLO[0] + ((fat16_u16_t)fat_entry.DIR_FstClusLO[1]<<8));

         if(_fat16_msdos_lookupname_ud(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info,desc,fat_filename)==-1)
            return INVALID_INODE_NB;

         //affect logic inode
         off = __get_BPB_RootEntCnt(ofile_lst[desc].pmntdev)+(fat16_ofile_lst[desc].entry_phys_addr-__get_ud_addr(ofile_lst[desc].pmntdev))/RD_SIZE;
         _ino_logic = __cvt2logicnode(desc,off);
      }
   }

   return _ino_logic;
}

/*-------------------------------------------
| Name:_fat_msdos_create
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
inodenb_t _fat_msdos_create(desc_t desc,char* filename, int attr) {
   fat16_u32_t addr=0,cluster_addr=0;
   fat16_u16_t cluster=0, parent_cluster=0;
   fat16_u16_t tmp_cluster=0;
   int i=0;

   inodenb_t _ino_logic=0;
   inodenb_t _ino_phys = __cvt2physnode(desc,ofile_lst[desc].inodenb);
   char fat_name[FAT16_MAX_NAME_FIL+FAT16_MAX_EXT_FIL+1]={0};
   fat_msdos_dir_entry_t fat_entry={0};

   //root directory is parent
   if(!_ino_phys) {
      parent_cluster=0;

      addr = __get_rd_addr(ofile_lst[desc].pmntdev);
      do{
         addr = addr + RD_SIZE;
         //error root directory is full
         if (addr == __get_ud_addr(ofile_lst[desc].pmntdev))
            return INVALID_INODE_NB;

         if(_fat_read_data(__get_dev_desc(desc),addr,(unsigned char *)&fat_entry.DIR_Name[0],1)<0) {
            return INVALID_INODE_NB;
         }
         //
         _ino_logic++;
      }
      while (fat_entry.DIR_Name[0]!=RD_ENT_FREE && fat_entry.DIR_Name[0]!=RD_ENT_REMOVE);
   }
   //parent is in user data
   else {
      //get data of inode
//      if(_ino_phys<__get_BPB_RootEntCnt(ofile_lst[desc].pmntdev)) {
//         addr = __get_rd_addr(ofile_lst[desc].pmntdev)+RD_SIZE*_ino_phys;
//      }
//      else {
//         addr = __get_ud_addr(ofile_lst[desc].pmntdev) + (_ino_phys-__get_BPB_RootEntCnt(ofile_lst[desc].pmntdev))*RD_SIZE;
//      }
//      //
//      if(_fat_read_data(__get_dev_desc(desc),addr,(unsigned char *)&fat_entry,RD_SIZE)<0) {
//         return INVALID_INODE_NB;
//      }
//      cluster = fat_entry.DIR_FstClusLO[0] + ((fat16_u16_t)fat_entry.DIR_FstClusLO[1]<<8);

      cluster = fat16_ofile_lst[desc].entry_data_cluster;
      //JUST FIRST CLUSTER OF DIRECTORY IS IMPORTANT
      //affect parent cluster
      parent_cluster = cluster;

      do{
         if(!(cluster_addr = _fat16_cluster_add(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info, cluster))) {
            return INVALID_INODE_NB;
         }

         addr = cluster_addr;
         //find free or remove entry
         do {
            if(_fat_read_data(__get_dev_desc(desc),addr,(unsigned char *)&fat_entry.DIR_Name[0],1)<0) {
               return INVALID_INODE_NB;
            }

            if(fat_entry.DIR_Name[0]!=RD_ENT_FREE && fat_entry.DIR_Name[0]!=RD_ENT_REMOVE) {
               addr = addr + RD_SIZE;
            }
         } while((fat_entry.DIR_Name[0]!=RD_ENT_FREE && fat_entry.DIR_Name[0]!=RD_ENT_REMOVE)
               && ((addr-cluster_addr-RD_SIZE)<(FAT16_BS_BPS_VAL*__get_nbsec_per_clus(ofile_lst[desc].pmntdev))));

         cluster = _fat16_cluster_suiv(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info, __get_dev_desc(desc), cluster);

      } while((fat_entry.DIR_Name[0]!=RD_ENT_FREE && fat_entry.DIR_Name[0]!=RD_ENT_REMOVE)
            && (cluster!=FAT16_LCLUSMAX));

      //free entry not found, get a new cluster
      if(fat_entry.DIR_Name[0]!=RD_ENT_FREE && fat_entry.DIR_Name[0]!=RD_ENT_REMOVE) {
         fat16_u16_t cluster_suiv = 0;
         cluster = fat16_ofile_lst[desc].entry_data_cluster;

         //find last cluster of directory
         while (_fat16_cluster_suiv(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info, __get_dev_desc(desc), cluster)!=FAT16_LCLUSMAX) {
            cluster = _fat16_cluster_suiv(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info, __get_dev_desc(desc), cluster);
         }

         //allocate new cluster
         if(!(cluster_suiv = _fat16_getclus(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info, __get_dev_desc(desc), &ofile_lst[desc].pmntdev->fs_info.fat_info.fat_boot_info->bpb))) {
            return INVALID_INODE_NB;
         }

         //
         _fat16_chainclus(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info, __get_dev_desc(desc), cluster, cluster_suiv);
         addr = _fat16_cluster_add(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info, cluster_suiv);
      }

      //define logic inode
      _ino_logic = __get_BPB_RootEntCnt(ofile_lst[desc].pmntdev)+(addr-__get_ud_addr(ofile_lst[desc].pmntdev))/RD_SIZE;
   }

   //get fat name
   if(_fat16_msdos_getname(filename, fat_entry.DIR_Name) < 0) {
      return INVALID_INODE_NB;
   }
   //fill entry
   tmp_cluster = _fat16_create_fat_entry(&fat_entry, desc, attr);

   //write short entry
   if(_fat_write_data(__get_dev_desc(desc),addr,(unsigned char *)&fat_entry,RD_SIZE)<0) {
      return INVALID_INODE_NB;
   }


   //create dot and dot dot for directory
   if(attr == S_IFDIR) {
      if(_fat16_create_fat_directory_entry(&fat_entry, desc, tmp_cluster, parent_cluster)<0)
         return INVALID_INODE_NB;
   }

   //affect inode number
   ofile_lst[desc].inodenb = __cvt2logicnode(desc,_ino_logic);

   return _ino_logic;
}

/*-------------------------------------------
| Name:_fat_msdos_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _fat_msdos_open(desc_t desc) {
   inodenb_t _ino_phys= __cvt2physnode(desc,ofile_lst[desc].inodenb);
   fat_msdos_dir_entry_t fat_entry={0};
   fat16_u32_t addr=0;

   //mount point
   if(!_ino_phys) {
      fat16_ofile_lst[desc].entry_phys_addr = __get_rd_addr(ofile_lst[desc].pmntdev)+RD_SIZE;
      fat16_ofile_lst[desc].entry_data_cluster = 0;
      fat16_ofile_lst[desc].entry_attr = RD_ATTR_VOLUMEID;
      fat16_ofile_lst[desc].entry_infos_addr = 0;
      ofile_lst[desc].attr = S_IFDIR;
      return -1;
   }
   else {
      //we read _ino_phy entry in ROOTDIR
      //if(!_ino_par_phys) {
      //metadatas are in ROOTDIR
      if(_ino_phys<__get_BPB_RootEntCnt(ofile_lst[desc].pmntdev)) {
         addr = __get_rd_addr(ofile_lst[desc].pmntdev)+RD_SIZE*_ino_phys;
      }
      //metadatas are in USERDATA
      //maybe do a range test for inode number
      else {
         addr = __get_ud_addr(ofile_lst[desc].pmntdev) + (_ino_phys-__get_BPB_RootEntCnt(ofile_lst[desc].pmntdev))*RD_SIZE;
      }
      //read entry on disk
      if(_fat_read_data(__get_dev_desc(desc),addr,(unsigned char *)&fat_entry,RD_SIZE)<0) {
         return -1;
      }

      //fill fat16_ofile_lst
      fat16_ofile_lst[desc].entry_infos_addr = addr;
      _fat16_getdesc(desc, fat16_ofile_lst[desc].entry_infos_addr);
   }
   return 0;
}


/*-------------------------------------------
| Name:_fat_msdos_remove
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _fat_msdos_remove(desc_t desc_ancst,desc_t desc) {
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

   //remove short entry
   if(_fat_write_data(__get_dev_desc(desc),fat16_ofile_lst[desc].entry_infos_addr,&rem_pattern,1)<0) {
      return -1;
   }

   return 0;
}

/*-------------------------------------------
| Name:_fat_msdos_rename
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _fat_msdos_rename(desc_t desc,const char*  old_name, char* new_name) {
   char fat_new_name[FAT16_MAX_NAME_FIL+FAT16_MAX_EXT_FIL+1]={0};
   char fat_old_name[FAT16_MAX_NAME_FIL+FAT16_MAX_EXT_FIL+1]={0};
   fat_msdos_dir_entry_t fat_entry={0};
   inodenb_t _ino_phys=__cvt2physnode(desc, ofile_lst[desc].inodenb);
   fat16_u32_t current_addr=0;
   int off=0;

   if(_fat16_msdos_getname(new_name,fat_new_name)<0 || _fat16_msdos_getname(old_name,fat_old_name)) {
      return -1;
   }

   //rootdir
   if(!_ino_phys){
      do {
         //we reach userdata
         if((current_addr = fat16_ofile_lst[desc].entry_phys_addr+off)==__get_ud_addr(ofile_lst[desc].pmntdev)) {
            return -1;
         }

         //read medium
         if(_fat_read_data(__get_dev_desc(desc),current_addr,(unsigned char *)&fat_entry,RD_SIZE)<0) {
            return -1;
         }
         off += RD_SIZE;

         //entry if empty or remove
         if(fat_entry.DIR_Name[0]==RD_ENT_FREE) {
            return -1;
         }
         if(fat_entry.DIR_Name[0]==RD_ENT_REMOVE) {
            continue;
         }

         //we reach entry to modifiy
         if(!strncmp(fat_entry.DIR_Name,fat_old_name,FAT16_MAX_NAME_FIL+FAT16_MAX_EXT_FIL)) {
            //a dir can't have extension
            if((fat_entry.DIR_Attr==RD_ATTR_DIRECTORY)&&((fat_new_name[RD_ENTEXT_OFF]!=0x20)
                  ||(fat_new_name[RD_ENTEXT_OFF+1]!=0x20)||(fat_new_name[RD_ENTEXT_OFF+2]!=0x20))) {
               return -1;
            }

            //
            if(_fat_write_data(__get_dev_desc(desc),fat16_ofile_lst[desc].entry_phys_addr+off-RD_SIZE,fat_new_name,FAT16_MAX_NAME_FIL+FAT16_MAX_EXT_FIL)<0) {
               return -1;
            }
            return 0;
         }
      } while(1);
   }
   //or userdata
   else {
      int found=0;
      fat16_u16_t cluster = fat16_ofile_lst[desc].entry_data_cluster;
      int i = 0;
      while(1) {
         current_addr = _fat16_cluster_add(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info,cluster);
         i=0;
         while((i < ((__get_nbsec_per_clus(ofile_lst[desc].pmntdev) * FAT16_BS_BPS_VAL)/RD_SIZE))) {

            if(_fat_read_data(__get_dev_desc(desc),current_addr,(unsigned char *)&fat_entry,RD_SIZE)<0) {
               return -1;
            }
            //
            current_addr += RD_SIZE;
            i++;

            //entry if empty or remove
            if(fat_entry.DIR_Name[0]==RD_ENT_FREE) {
               return -1;
            }
            if(fat_entry.DIR_Name[0]==RD_ENT_REMOVE) {
               continue;
            }

            //we reach entry to modifiy
            if(!strncmp(fat_entry.DIR_Name,fat_old_name,FAT16_MAX_NAME_FIL+FAT16_MAX_EXT_FIL)) {
               //a dir can't have extension
               if((fat_entry.DIR_Attr==RD_ATTR_DIRECTORY)&&((fat_new_name[RD_ENTEXT_OFF]!=0x20)
                     ||(fat_new_name[RD_ENTEXT_OFF+1]!=0x20)||(fat_new_name[RD_ENTEXT_OFF+2]!=0x20))) {
                  return -1;
               }

               //
               if(_fat_write_data(__get_dev_desc(desc),current_addr-RD_SIZE,fat_new_name,FAT16_MAX_NAME_FIL+FAT16_MAX_EXT_FIL)<0) {
                  return -1;
               }

               return 0;
            }
         }
         cluster = _fat16_cluster_suiv(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info, __get_dev_desc(desc), cluster);
         //end of data
         if(cluster == FAT16_LCLUSMAX){
            return(-1);
         }
      }
   }
   return 0;
}
/*============================================
| End of Source  : fat16_msdos.c
|---------------------------------------------
| Historic:
|---------------------------------------------
| Authors     | Date     | Comments
| $Log:$
==============================================*/
