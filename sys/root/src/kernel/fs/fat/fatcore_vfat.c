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
#include <string.h>

#include "kernel/core/system.h"
#include "kernel/core/time.h"
#include "kernel/core/systime.h"

#include "kernel/fs/vfs/vfstypes.h"
#include "kernel/fs/vfs/vfskernel.h"
#include "kernel/fs/vfs/vfs.h"

#include "fat16.h"
#include "fatcore.h"
#include "fatcore_vfat.h"
/*============================================
| Global Declaration
==============================================*/


/*============================================
| Implementation
==============================================*/
/*-------------------------------------------
| Name:_fat16_vfat_lookupname_ud
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _fat16_vfat_lookupname_ud(fat16_core_info_t * fat_info, desc_t desc_new, char* filename) {
   int i=0, found=0;
   fat16_u32_t addr;
   fat16_u16_t cluster = fat16_ofile_lst[desc_new].entry_data_cluster;
   fat16_u32_t cluster_addr = _fat16_cluster_add(fat_info, cluster);
   fat_dir_entry_t fat_entry={0};
   fat16_u8_t nb_ent=0;
   fat16_u8_t longname[FAT16_MAX_NAME_FIL_VFAT+1]={0};

   while(!found) {
      addr = _fat16_cluster_add(fat_info,cluster);
      i=0;
      //
      while((i<((fat_info->nbsec_per_clus * FAT16_BS_BPS_VAL)/RD_SIZE))&&(!found)) {
         if(_fat_read_data(__get_dev_desc(desc_new),addr,(unsigned char *)&fat_entry.msdos_entry,
                           RD_SIZE)<0) {
            return -1;
         }

         //end of directory
         if (fat_entry.msdos_entry.DIR_Name[0] == RD_ENT_FREE)
            return -1;

         //remove entry
         if (fat_entry.msdos_entry.DIR_Name[0] == RD_ENT_REMOVE) {
            addr +=RD_SIZE;
            i++;
            continue;
         }

         //is it a long name
         if((fat_entry.vfat_entry.LDIR_Ord&FAT16_LAST_LONGENTRY_MSK)
            && (fat_entry.vfat_entry.LDIR_Attr&FAT16_LONGNAME_MSK)==RD_ATTR_LONGNAME) {

            //rewind offset
            ofile_lst[__get_dev_desc(desc_new)].pfsop->fdev.fdev_seek(__get_dev_desc(
                                                                         desc_new),addr-RD_SIZE,
                                                                      SEEK_SET);
            nb_ent = __vfat_get_nb_long_entries(fat_entry.vfat_entry);

            _fat16_vfat_build_oob_name(fat_info, __get_dev_desc(
                                          desc_new), longname, nb_ent, cluster);
            //entry found
            if(!strncmp(longname,filename,FAT16_MAX_NAME_FIL_VFAT)) {
               found=1;
            }
            else {
               //+1 => skip short dir entry
               addr += RD_SIZE*(nb_ent+1);
               i += (nb_ent+1);
            }
         }
         else {
            addr +=RD_SIZE;
            i++;
         }
      }
      if(!found) {
         cluster = _fat16_cluster_suiv(fat_info, __get_dev_desc(desc_new), cluster);
         cluster_addr = _fat16_cluster_add(fat_info, cluster);
         //end of data
         if(cluster == FAT16_LCLUSMAX) return -1;
      }
   }
   //
   _fat16_vfat_get_infos_addr(desc_new, cluster,
                              (fat_entry.vfat_entry.LDIR_Ord&~FAT16_LAST_LONGENTRY_MSK),&addr);
   _fat16_getdesc(desc_new, addr);
   return 0;
}

/*-------------------------------------------
| Name:_fat16_vfat_lookupname_rd
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _fat16_vfat_lookupname_rd(fat16_core_info_t * fat_info, desc_t desc_new, char* filename) {
   int i=0, found=0;
   fat16_u32_t addr=fat_info->rd_addr + RD_SIZE;
   fat16_u8_t longname[FAT16_MAX_NAME_FIL_VFAT+1]={0};
   fat_dir_entry_t fat_entry={0};
   fat16_u8_t nb_ent=0;

   while(!found) {
      if(_fat_read_data(__get_dev_desc(desc_new),addr,(unsigned char *)&fat_entry.msdos_entry,
                        RD_SIZE)<0) {
         return -1;
      }

      //end of directory
      if (fat_entry.msdos_entry.DIR_Name[0] == RD_ENT_FREE)
         return -1;

      //remove entry
      if (fat_entry.msdos_entry.DIR_Name[0] == RD_ENT_REMOVE) {
         addr +=RD_SIZE;
         continue;
      }

      //is it a long name
      if((fat_entry.vfat_entry.LDIR_Ord&FAT16_LAST_LONGENTRY_MSK)
         && (fat_entry.vfat_entry.LDIR_Attr&FAT16_LONGNAME_MSK)==RD_ATTR_LONGNAME) {

         //rewind offset
         ofile_lst[__get_dev_desc(desc_new)].pfsop->fdev.fdev_seek(__get_dev_desc(
                                                                      desc_new),addr-RD_SIZE,
                                                                   SEEK_SET);
         nb_ent = __vfat_get_nb_long_entries(fat_entry.vfat_entry);

         if(_fat16_vfat_build_name(__get_dev_desc(desc_new),longname,nb_ent) < 0)
            return -1;

         //entry found
         if(!strncmp(longname,filename,FAT16_MAX_NAME_FIL_VFAT)) {
            found=1;
         }
         else {
            //+1 => skip short dir entry
            addr += RD_SIZE*(nb_ent+1);
         }
      }

      //out of root dir?
      if(!found &&
         (addr >= fat_info->ud_addr))
         return -1;
   }

   //
   fat16_ofile_lst[desc_new].entry_infos_addr = addr+nb_ent*RD_SIZE;
   _fat16_getdesc(desc_new, addr);

   return 0;
}

/*-------------------------------------------
| Name:_fat16_vfat_build_name
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _fat16_vfat_build_name(desc_t dev_desc, unsigned char * dst_longname, int nb_ent) {
   fat_vfat_dir_entry_t vfat_entry={0};
   int i=0;
   //move to first entry
   fat16_u32_t addr = ofile_lst[dev_desc].pfsop->fdev.fdev_seek(dev_desc,RD_SIZE*nb_ent,SEEK_CUR);

   if(!dst_longname)
      return -1;

   memset(dst_longname,0,FAT16_MAX_NAME_FIL_VFAT);

   //name have to much entries
   if(nb_ent>=FAT16_MAX_ENTRIES_TEST)
      return -1;

   for(i=0; i<nb_ent; i++) {
      if(_fat_read_data(dev_desc,addr-RD_SIZE*i,(unsigned char *)&vfat_entry,RD_SIZE)<0) {
         return -1;
      }

      _fat16_vfat_extract_name(dst_longname+i*FAT16_LONGNAME_SUM,
                               (const fat_vfat_dir_entry_t *)&vfat_entry);
   }
   return 0;
}

/*-------------------------------------------
| Name:_fat16_vfat_build_oob_name
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _fat16_vfat_build_oob_name(fat16_core_info_t * fat_info, desc_t dev_desc,
                               unsigned char * dst_longname, int nb_ent,
                               fat16_u16_t cluster) {
   int nb_entry_close=0;
   int nb_entry_far=0;
   fat16_u32_t cluster_addr=_fat16_cluster_add(fat_info, cluster);
   fat16_u32_t current_addr = ofile_lst[dev_desc].pfsop->fdev.fdev_seek(dev_desc,0,SEEK_CUR);
   fat16_u16_t cluster_suiv=0;
   int i=0;

   //-RD_SIZE : avant dernier entrée
   nb_entry_close =
      ((cluster_addr+fat_info->nbsec_per_clus * FAT16_BS_BPS_VAL-RD_SIZE)-current_addr)/RD_SIZE;

   //name have to much entries
   if(nb_ent>=FAT16_MAX_ENTRIES_TEST)
      return -1;

   //can we read all long entries
   if(nb_entry_close>=nb_ent) {
      return _fat16_vfat_build_name(dev_desc, dst_longname, nb_ent);
   }
   //or long entries are splitted between 2 clusters
   else {
      nb_entry_far = nb_ent-nb_entry_close;
   }

   //get "far" name part
   cluster_suiv = _fat16_cluster_suiv(fat_info, dev_desc, cluster);
   cluster_addr = _fat16_cluster_add(fat_info, cluster_suiv);
   ofile_lst[dev_desc].pfsop->fdev.fdev_seek(dev_desc,cluster_addr,SEEK_SET);
   if(_fat16_vfat_build_name(dev_desc, dst_longname, nb_entry_far)<0)
      return -1;

   //get "close" name part
   ofile_lst[dev_desc].pfsop->fdev.fdev_seek(dev_desc,current_addr,SEEK_SET);
   if(_fat16_vfat_build_name(dev_desc, &dst_longname[strlen(dst_longname)], nb_entry_close)<0)
      return -1;

   //indicate we treat long name in 2 parts
   return 0;
}

/*-------------------------------------------
| Name:_fat16_vfat_extract_name
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _fat16_vfat_extract_name(unsigned char * dst, const fat_vfat_dir_entry_t * pvfat_entry) {
   int i=0;
   if(!dst || !pvfat_entry)
      return -1;

   //name1
   for(i=0; i<FAT16_LONGNAME_NAME1; i++) {
      *dst = pvfat_entry->LDIR_Name1[2*i];
      if(*dst++=='\0') return 0;
   }

   //name2
   for(i=0; i<FAT16_LONGNAME_NAME2; i++) {
      *dst = pvfat_entry->LDIR_Name2[2*i];
      if(*dst++=='\0') return 0;
   }

   //name3
   for(i=0; i<FAT16_LONGNAME_NAME3; i++) {
      *dst = pvfat_entry->LDIR_Name3[2*i];
      if(*dst++=='\0') return 0;
   }
   return 0;
}

/*-------------------------------------------
| Name:_fat16_vfat_get_infos_addr
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _fat16_vfat_get_infos_addr(desc_t desc_new, fat16_u16_t cluster, int nb_entries,
                               fat16_u32_t *current_addr) {
   fat16_u32_t cluster_addr=0;
   fat16_u16_t cluster_suiv=0;
   int nb_ent_close=0;

   cluster_addr=_fat16_cluster_add(ofile_lst[desc_new].pmntdev->fs_info.fat_info.fat_core_info,
                                   cluster);
   nb_ent_close =
      ((cluster_addr+
        (ofile_lst[desc_new].pmntdev->fs_info.fat_info.fat_boot_info->bpb.BPB_SecPerClust*
         ofile_lst[desc_new].pmntdev->fs_info.fat_info.fat_boot_info->bpb.BPB_BytesPerSec)-
        RD_SIZE)-*current_addr)/RD_SIZE;

   //maybe entry is splitted between 2 clusters
   if(nb_ent_close<nb_entries) {
      cluster_suiv = _fat16_cluster_suiv(
         ofile_lst[desc_new].pmntdev->fs_info.fat_info.fat_core_info, __get_dev_desc(
            desc_new), cluster);
      cluster_addr = _fat16_cluster_add(ofile_lst[desc_new].pmntdev->fs_info.fat_info.fat_core_info,
                                        cluster_suiv);

      fat16_ofile_lst[desc_new].entry_infos_addr = cluster_addr+(nb_entries-nb_ent_close-1)*RD_SIZE;
   }
   else {
      fat16_ofile_lst[desc_new].entry_infos_addr = *current_addr+nb_entries*RD_SIZE;
   }

   return fat16_ofile_lst[desc_new].entry_infos_addr;
}

/*-------------------------------------------
| Name:_fat16_vfat_get_addr_dot_items
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
fat16_u32_t _fat16_vfat_get_addr_dot_items(fat16_core_info_t * fat_info, desc_t desc,
                                           fat16_u16_t parent_cluster,
                                           fat16_u16_t pattern_cluster) {
   fat16_u32_t current_addr = 0;
   fat16_u16_t cluster = 0;
   fat_dir_entry_t fat_entry={0};
   int found=0,j=0;
   fat16_u32_t return_addr = 0;

   current_addr = _fat16_cluster_add(fat_info, parent_cluster);

   //we are after dotdot we need to get dotdot
   if(_fat_read_data(__get_dev_desc(desc),current_addr+RD_SIZE,
                     (unsigned char *)&fat_entry.msdos_entry,RD_SIZE)<0) {
      return 0;
   }

   cluster = fat_entry.msdos_entry.DIR_FstClusLO[0] +
             ((fat16_u16_t)fat_entry.msdos_entry.DIR_FstClusLO[1]<<8);
   //2 cases
   //dot is in rootdir
   if(!cluster) {
      current_addr = fat_info->rd_addr+RD_SIZE;
      j=1;
      while((j<((fat_info->nbsec_per_clus * FAT16_BS_BPS_VAL)/RD_SIZE))) {
         if(_fat_read_data(__get_dev_desc(desc),current_addr,(unsigned char *)&fat_entry.vfat_entry,
                           RD_SIZE)<0) {
            return 0;
         }

         //end of directory
         if (fat_entry.msdos_entry.DIR_Name[0] == RD_ENT_FREE)
            return 0;

         //remove entry
         if (fat_entry.msdos_entry.DIR_Name[0] == RD_ENT_REMOVE) {
            current_addr += RD_SIZE;
            j++;
            continue;
         }

         //is it a long name
         if((fat_entry.vfat_entry.LDIR_Ord&FAT16_LAST_LONGENTRY_MSK)
            && (fat_entry.vfat_entry.LDIR_Attr&FAT16_LONGNAME_MSK)==RD_ATTR_LONGNAME) {

            return_addr = current_addr;
            current_addr += RD_SIZE*(__vfat_get_nb_long_entries(fat_entry.vfat_entry));
            j += __vfat_get_nb_long_entries(fat_entry.vfat_entry);
            continue;
         }

         if (fat_entry.msdos_entry.DIR_Name[0]!= '.'
             && (pattern_cluster ==
                 (fat_entry.msdos_entry.DIR_FstClusLO[0] +
                  ((fat16_u16_t)fat_entry.msdos_entry.DIR_FstClusLO[1]<<8)))) {
            return return_addr;
         }
         //
         return_addr = current_addr;
         current_addr += RD_SIZE;
         j++;
      }
   }
   //or not
   else {
      while(!found) {
         current_addr = _fat16_cluster_add(fat_info, cluster);
         j=0;
         while((j<((fat_info->nbsec_per_clus * FAT16_BS_BPS_VAL)/RD_SIZE))&&(!found)) {
            if(_fat_read_data(__get_dev_desc(desc),current_addr,
                              (unsigned char *)&fat_entry.vfat_entry,RD_SIZE)<0) {
               return 0;
            }

            //end of directory
            if (fat_entry.msdos_entry.DIR_Name[0] == RD_ENT_FREE)
               return 0;

            //remove entry
            if (fat_entry.msdos_entry.DIR_Name[0] == RD_ENT_REMOVE) {
               current_addr += RD_SIZE;
               j++;
               continue;
            }

            //is it a long name
            if((fat_entry.vfat_entry.LDIR_Ord&FAT16_LAST_LONGENTRY_MSK)
               && (fat_entry.vfat_entry.LDIR_Attr&FAT16_LONGNAME_MSK)==RD_ATTR_LONGNAME) {

               return_addr = current_addr;
               current_addr += RD_SIZE*(__vfat_get_nb_long_entries(fat_entry.vfat_entry));
               j += __vfat_get_nb_long_entries(fat_entry.vfat_entry);
               continue;
            }

            //cluster match?
            if (fat_entry.msdos_entry.DIR_Name[0]!= '.'
                && (pattern_cluster ==
                    (fat_entry.msdos_entry.DIR_FstClusLO[0] +
                     ((fat16_u16_t)fat_entry.msdos_entry.DIR_FstClusLO[1]<<8)))) {
               return return_addr;
            }
            //
            return_addr = current_addr;
            current_addr += RD_SIZE;
            j++;
         }
         cluster = _fat16_cluster_suiv(fat_info, __get_dev_desc(desc), cluster);
         //end of data
         if(cluster == FAT16_LCLUSMAX) {
            return 0;
         }
      }
   }
   //
   return 0;
}

/*-------------------------------------------
| Name:_fat16_vfat_chksum
| Description:
| Parameters:
| Return Type:
| Comments:
| See:General Overview of On-Disk Format p.28
---------------------------------------------*/
unsigned char _fat16_vfat_chksum(unsigned char * short_name) {
   short short_name_length=(FAT16_MAX_NAME_FIL+FAT16_MAX_EXT_FIL);
   unsigned char chksum = 0;

   for(; short_name_length!=0; short_name_length--) {
      // NOTE: The operation is an unsigned char rotate right
      chksum = ((chksum & 1) ? 0x80 : 0) + (chksum >> 1) + *short_name++;
   }
   return chksum;
}

/*-------------------------------------------
| Name:_fat16_vfat_create_short_name
| Description:
| Parameters:
| Return Type:
| Comments:
| See: from http://lwn.net/Articles/338942/
---------------------------------------------*/
int _fat16_vfat_create_short_name(unsigned char * short_name) {
   int rand_num = 0;
   int i=2;
   struct __timeval now_tv = {0};

   //generate a part of name with timestamp
   _sys_gettimeofday(&now_tv,NULL);
   rand_num = (int)(now_tv.tv_sec) ? now_tv.tv_sec : 1;

   short_name[0] = ' ';
   short_name[1] = '\0';
   short_name[8] = '/';
   short_name[9] = short_name[10] = '\0';

   for(; i<FAT16_MAX_NAME_FIL; i++) {
      short_name[i] = rand_num & 0x1F;
      rand_num >>= 5;
   }
   return 0;
}

/*-------------------------------------------
| Name:_fat16_vfat_create_long_name
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _fat16_vfat_create_long_name(unsigned char * long_name, fat_vfat_dir_entry_t *ptab_entry,
                                 unsigned char chksum) {
   int i=0;
   int j=0;
   unsigned char * plong_entry = (unsigned char *)&ptab_entry[j].LDIR_Name1;

   if(!long_name || !ptab_entry) {
      return -1;
   }
   //name too long
   if(strlen(long_name)>FAT16_MAX_NAME_FIL_VFAT) {
      return -1;
   }

   //
   for(; long_name[i]; i++) {
      *plong_entry = long_name[i];
      plong_entry += 2;

      //
      if(plong_entry == ptab_entry[j].LDIR_Name1+FAT16_LONGNAME_NAME1*2) {
         //move to LDIR_Name2
         plong_entry = (unsigned char *)&ptab_entry[j].LDIR_Name2;
      }
      else if(plong_entry == ptab_entry[j].LDIR_Name2+FAT16_LONGNAME_NAME2*2) {
         //move to LDIR_Name3
         plong_entry = (unsigned char *)&ptab_entry[j].LDIR_Name3;
      }
      else if(plong_entry == (unsigned char *)&ptab_entry[j+1]) {
         //move to next LDIR_Name1
         plong_entry = (unsigned char *)&ptab_entry[++j].LDIR_Name1;
      }
   }

   *plong_entry++ = 0x00; *plong_entry++ = 0x00;
   //padded with 0xffff if last entry is not full
   if(i%13) {
      while(plong_entry != (unsigned char *)&ptab_entry[j+1]) {
         //
         if(plong_entry == ptab_entry[j].LDIR_Name1+FAT16_LONGNAME_NAME1*2) {
            //move to LDIR_Name2
            plong_entry = (unsigned char *)&ptab_entry[j].LDIR_Name2;
         }
         else if(plong_entry == ptab_entry[j].LDIR_Name2+FAT16_LONGNAME_NAME2*2) {
            //move to LDIR_Name3
            plong_entry = (unsigned char *)&ptab_entry[j].LDIR_Name3;
         }
         //
         *plong_entry++ = 0xff; *plong_entry++ = 0xff;
      }
   }
   else {
      j--;
   }

   //update LDIR_Ord,LDIR_Attr,LDIR_Chksum of last entry
   ptab_entry[j].LDIR_Ord = FAT16_LAST_LONGENTRY_MSK|j+1;
   ptab_entry[j].LDIR_Attr = RD_ATTR_LONGNAME;
   ptab_entry[j].LDIR_Chksum = chksum;

   //update LDIR_Ord,LDIR_Attr,LDIR_Chksum of others entries
   for(i=j-1; i>=0; i--) {
      ptab_entry[i].LDIR_Ord = i+1;
      ptab_entry[i].LDIR_Attr = RD_ATTR_LONGNAME;
      ptab_entry[i].LDIR_Chksum = chksum;
   }
   return j+1;
}

/*-------------------------------------------
| Name:_fat16_vfat_write_long_entries
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
fat16_u32_t _fat16_vfat_write_long_entries(desc_t dev_desc, fat16_u32_t addr,
                                           fat_vfat_dir_entry_t *ptab_entry, int pos,
                                           int count) {
   int i = pos;
   //write long entries
   while(count--) {
      if(_fat_write_data(dev_desc,addr,(unsigned char *)&ptab_entry[i--],RD_SIZE)<0) {
         return 0;
      }
      addr+=RD_SIZE;
   }

   return addr;
}

/*-------------------------------------------
| Name:_fat16_vfat_get_next_long_entry
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _fat16_vfat_get_next_long_entry(desc_t desc_new, fat16_u16_t *cluster, int nb_entries,
                                    fat16_u32_t *current_addr) {
   fat16_u32_t cluster_addr=0;
   fat16_u16_t cluster_suiv=0;
   int nb_ent_close=0;
   fat16_u32_t addr=0;

   cluster_addr=_fat16_cluster_add(ofile_lst[desc_new].pmntdev->fs_info.fat_info.fat_core_info,
                                   *cluster);
   nb_ent_close =
      ((cluster_addr+
        (ofile_lst[desc_new].pmntdev->fs_info.fat_info.fat_boot_info->bpb.BPB_SecPerClust*
         ofile_lst[desc_new].pmntdev->fs_info.fat_info.fat_boot_info->bpb.BPB_BytesPerSec)-
        RD_SIZE)-*current_addr)/RD_SIZE;

   //maybe entry is splitted between 2 clusters
   if(nb_ent_close<nb_entries) {
      //we need to allocate a new cluster
      if(!(cluster_suiv =
              _fat16_cluster_suiv(ofile_lst[desc_new].pmntdev->fs_info.fat_info.fat_core_info,
                                  __get_dev_desc(desc_new), *cluster))
         || cluster_suiv == FAT16_LCLUSMAX)
         return 0;

      *cluster = cluster_suiv;
      cluster_addr = _fat16_cluster_add(ofile_lst[desc_new].pmntdev->fs_info.fat_info.fat_core_info,
                                        cluster_suiv);

      addr = cluster_addr+(nb_entries-nb_ent_close-1)*RD_SIZE;
   }
   else {
      addr = *current_addr+nb_entries*RD_SIZE;
   }

   return addr;
}

/*-------------------------------------------
| Name:_fat16_vfat_request_entries
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _fat16_vfat_request_entries(desc_t desc, fat16_u32_t addr_begin, fat16_u32_t addr_end,
                                int nb_ent) {
   int i=0;
   fat_dir_entry_t fat_entry={0};

   if(!addr_begin || !addr_end)
      return -1;
   if(addr_begin>addr_end)
      return -1;

   //name have to much entries
   if(nb_ent>=FAT16_MAX_ENTRIES_TEST)
      return -1;

   for(; i<nb_ent; i++) {
      if(_fat_read_data(__get_dev_desc(desc),addr_begin,
                        (unsigned char *)&fat_entry.msdos_entry.DIR_Name[0],1)<0) {
         return -1;
      }

      //not enough
      if((fat_entry.msdos_entry.DIR_Name[0]!=RD_ENT_FREE && fat_entry.msdos_entry.DIR_Name[0]!=
          RD_ENT_REMOVE)
         || addr_begin == addr_end) {
         return i;
      }
      addr_begin += RD_SIZE;
   }
   //
   return 0;
}

/*-------------------------------------------
| Name:_fat16_vfat_remove_entries
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _fat16_vfat_remove_entries(fat16_core_info_t * fat_info, desc_t desc) {
   fat_dir_entry_t fat_entry={0};
   int nb_entries=0;
   inodenb_t _ino_phys= __cvt2physnode(desc,ofile_lst[desc].inodenb);
   fat16_u8_t rem_pattern=RD_ENT_REMOVE;

   //read entry
   if(_fat_read_data(__get_dev_desc(desc),fat16_ofile_lst[desc].entry_phys_addr,
                     (unsigned char *)&fat_entry,RD_SIZE)<0) {
      return -1;
   }
   nb_entries = __vfat_get_nb_long_entries(fat_entry.vfat_entry);

   //2 cases
   //root directory
   if(_ino_phys<__get_BPB_RootEntCnt(ofile_lst[desc].pmntdev)) {
      int i=0;
      //remove long entries
      for(; i<nb_entries; i++) {
         if(_fat_write_data(__get_dev_desc(desc),fat16_ofile_lst[desc].entry_phys_addr+i*RD_SIZE,
                            &rem_pattern,1)<0) {
            return -1;
         }
      }
   }
   //or user data
   else {
      //get current cluster number
      fat16_u16_t current_cluster =
         (fat16_ofile_lst[desc].entry_phys_addr-__get_ud_addr(ofile_lst[desc].pmntdev))/
         (__get_BPB_SecPerClus(ofile_lst[desc].pmntdev)*
          __get_BPB_BytesPerSec(ofile_lst[desc].pmntdev))
         +FAT16_CLUSMIN;
      fat16_u32_t current_addr=_fat16_cluster_add(fat_info, current_cluster);

      //how many entries before end of cluster
      int nb_entry_close =
         ((current_addr+fat_info->nbsec_per_clus *
           FAT16_BS_BPS_VAL)-fat16_ofile_lst[desc].entry_phys_addr)/RD_SIZE;

      //
      if(nb_entry_close>=nb_entries) {
         int i=0;
         //remove long entries
         for(; i<nb_entries; i++) {
            if(_fat_write_data(__get_dev_desc(desc),fat16_ofile_lst[desc].entry_phys_addr+i*RD_SIZE,
                               &rem_pattern,1)<0) {
               return -1;
            }
         }
      }
      else {
         int nb_entry_far = nb_entries-nb_entry_close;
         fat16_u16_t cluster_suiv=_fat16_cluster_suiv(fat_info, __get_dev_desc(
                                                         desc), current_cluster);
         fat16_u32_t addr = _fat16_cluster_add(fat_info, cluster_suiv);
         int i=0;

         //remove first part
         for(; i<nb_entry_close; i++) {
            if(_fat_write_data(__get_dev_desc(desc),fat16_ofile_lst[desc].entry_phys_addr+i*RD_SIZE,
                               &rem_pattern,1)<0) {
               return -1;
            }
         }

         //remove second part
         for(i=0; i<nb_entry_far; i++) {
            if(_fat_write_data(__get_dev_desc(desc),addr+i*RD_SIZE,&rem_pattern,1)<0) {
               return -1;
            }
         }
      }
   }

   return 0;
}


/*-------------------------------------------
| Name:_fat16_vfat_request_free_rd
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
inodenb_t _fat16_vfat_request_free_rd(desc_t desc, fat16_u32_t * addr, int req_ent) {
   fat_dir_entry_t fat_entry={0};
   inodenb_t _ino_logic=1;
   int avail_ent=0;

   do {
      //error root directory is FULL
      if ((*addr+req_ent) >= __get_ud_addr(ofile_lst[desc].pmntdev))
         return INVALID_INODE_NB;
      //
      if(_fat_read_data(__get_dev_desc(desc),*addr,(unsigned char *)&fat_entry.msdos_entry,
                        RD_SIZE)<0) {
         return INVALID_INODE_NB;
      }

      //is it a long name
      if( /*(fat_entry.vfat_entry.LDIR_Ord&FAT16_LAST_LONGENTRY_MSK)*/
         fat_entry.vfat_entry.LDIR_Ord != RD_ENT_REMOVE
         && (fat_entry.vfat_entry.LDIR_Attr&FAT16_LONGNAME_MSK)==RD_ATTR_LONGNAME) {

         *addr += RD_SIZE*(__vfat_get_nb_long_entries(fat_entry.vfat_entry)+1);
         _ino_logic += __vfat_get_nb_long_entries(fat_entry.vfat_entry)+1;
      }
      else {
         //how contiguous available entries
         avail_ent = _fat16_vfat_request_entries(desc, *addr, __get_ud_addr(ofile_lst[desc].pmntdev),
                                                 req_ent);

         //not enough space
         if(avail_ent) {
            *addr += RD_SIZE*req_ent;
         }
      }
   } while ((fat_entry.msdos_entry.DIR_Name[0]!=RD_ENT_FREE && fat_entry.msdos_entry.DIR_Name[0]!=
             RD_ENT_REMOVE)
            || avail_ent);

   return _ino_logic;
}


/*-------------------------------------------
| Name:_fat16_vfat_request_free_ud
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
inodenb_t _fat16_vfat_request_free_ud(desc_t desc, fat16_u32_t * addr, fat16_u16_t * cluster,
                                      fat16_u32_t *prev_addr, fat16_u16_t * prev_cluster,
                                      int req_ent,
                                      int * rem_ent) {
   fat_dir_entry_t fat_entry={0};
   int found=0;
   fat16_u32_t cluster_addr=0, cluster_suiv_addr=0;
   int avail_ent=0;

   while(!found) {
      //
      while(1) {
         if(_fat_read_data(__get_dev_desc(desc),*addr,(unsigned char *)&fat_entry.msdos_entry,
                           RD_SIZE)<0) {
            return INVALID_INODE_NB;
         }

         //is it a long name
         if(( /*fat_entry.vfat_entry.LDIR_Ord&FAT16_LAST_LONGENTRY_MSK)*/
               fat_entry.vfat_entry.LDIR_Ord != RD_ENT_REMOVE)
            && (fat_entry.vfat_entry.LDIR_Attr&FAT16_LONGNAME_MSK)==RD_ATTR_LONGNAME) {

            *prev_addr = *addr;
            *prev_cluster = *cluster;

            if(!(*addr =
                    _fat16_vfat_get_next_long_entry(desc, cluster,
                                                    (fat_entry.vfat_entry.LDIR_Ord&
                                                     ~FAT16_LAST_LONGENTRY_MSK)+1,addr))) {
               //allocate a new cluster
               if(!(*cluster=
                       _fat16_getclus(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info,
                                      __get_dev_desc(desc),
                                      &ofile_lst[desc].pmntdev->fs_info.fat_info.fat_boot_info->bpb)))
               {
                  //FAT is full
                  return INVALID_INODE_NB;
               }
               //
               _fat16_chainclus(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info,
                                __get_dev_desc(
                                   desc), *prev_cluster, *cluster);
               *addr = _fat16_cluster_add(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info,
                                          *cluster);
               *prev_addr = *addr;
               *prev_cluster = *cluster;
               //done ;)
               found = 1;
               break;

            }
         }
         else if (fat_entry.msdos_entry.DIR_Name[0] == RD_ENT_FREE
                  || fat_entry.msdos_entry.DIR_Name[0] == RD_ENT_REMOVE) {
            *prev_addr = *addr;
            *prev_cluster = *cluster;
            //
            if(*prev_cluster == *cluster) {
               cluster_addr = _fat16_cluster_add(
                  ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info, *cluster);
               avail_ent = _fat16_vfat_request_entries(
                  desc, *addr, cluster_addr+__get_nbsec_per_clus(
                     ofile_lst[desc].pmntdev) * FAT16_BS_BPS_VAL,
                  req_ent);
               *rem_ent = req_ent-avail_ent;
               //enough place in cluster to store entries
               if(!avail_ent) {
                  //done ;)
                  found=1;
                  break;
               }
               else {
                  fat16_u16_t cluster_suiv=_fat16_cluster_suiv(
                     ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info,  __get_dev_desc(
                        desc), *cluster);
                  //last cluster
                  if(cluster_suiv==FAT16_LCLUSMAX) {
                     //allocate a new cluster
                     if(!(cluster_suiv =
                             _fat16_getclus(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info,
                                            __get_dev_desc(desc),
                                            &ofile_lst[desc].pmntdev->fs_info.fat_info.
                                            fat_boot_info->bpb))) {
                        //FAT is full
                        return INVALID_INODE_NB;
                     }
                     //
                     _fat16_chainclus(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info,
                                      __get_dev_desc(
                                         desc), *cluster, cluster_suiv);

                     //done ;)
                     *cluster = cluster_suiv;
                     *addr = _fat16_cluster_add(
                        ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info, *cluster);
                     found = 1;
                     break;
                  }
                  //left space in next cluster?
                  else {
                     int rem_ent2=0, avail_ent2=0;
                     cluster_suiv_addr = _fat16_cluster_add(
                        ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info, cluster_suiv);
                     avail_ent2 = _fat16_vfat_request_entries(
                        desc, cluster_suiv_addr, cluster_suiv_addr+
                        __get_nbsec_per_clus(ofile_lst[desc].pmntdev) * FAT16_BS_BPS_VAL,
                        *rem_ent);

                     rem_ent2 = *rem_ent-avail_ent2;
                     //not enough space in beginning of next cluster
                     if(rem_ent2) {
                        *cluster = cluster_suiv;
                        *addr = cluster_suiv_addr+*rem_ent*RD_SIZE;
                     }
                     else {
                        //done ;)
                        avail_ent = *rem_ent;
                        *rem_ent = rem_ent2;
                        *cluster = cluster_suiv;
                        *addr = cluster_suiv_addr;
                        found=1;
                        break;
                     }
                  }
               }
            }
            else {
               //no valid chained cluster
               if(!*cluster) {
                  //allocate a new cluster
                  if(!(*cluster=
                          _fat16_getclus(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info,
                                         __get_dev_desc(desc),
                                         &ofile_lst[desc].pmntdev->fs_info.fat_info.fat_boot_info->
                                         bpb))) {
                     //FAT is full
                     return INVALID_INODE_NB;
                  }

                  //
                  _fat16_chainclus(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info,
                                   __get_dev_desc(
                                      desc), *prev_cluster, *cluster);
                  *addr = _fat16_cluster_add(
                     ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info, *cluster);
                  //done ;)
                  found = 1;
                  break;
               }
               else {
                  int rem_ent2=0, avail_ent2=0;

                  //avalaible entries in current_cluster
                  cluster_addr = _fat16_cluster_add(
                     ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info, *prev_cluster);
                  avail_ent = _fat16_vfat_request_entries(
                     desc, *prev_addr, cluster_addr+__get_nbsec_per_clus(
                        ofile_lst[desc].pmntdev) * FAT16_BS_BPS_VAL,
                     req_ent);

                  *rem_ent = req_ent-avail_ent;
                  //available entries in next cluster
                  cluster_addr = _fat16_cluster_add(
                     ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info, *cluster);
                  avail_ent2 = _fat16_vfat_request_entries(
                     desc, *addr, cluster_addr+__get_nbsec_per_clus(
                        ofile_lst[desc].pmntdev) * FAT16_BS_BPS_VAL,
                     *rem_ent);

                  rem_ent2 = *rem_ent-avail_ent2;
                  //enough space
                  if(!rem_ent2) {
                     //done ;)
                     found = 1;
                     break;
                  }
               }
            }
         }
         //for dot and dotdot
         else {
            *addr +=RD_SIZE;
         }
      }
   }

   return __get_BPB_RootEntCnt(ofile_lst[desc].pmntdev)+
          (*addr-__get_ud_addr(ofile_lst[desc].pmntdev))/RD_SIZE;
}
/*============================================
| End of Source  : fatcore_vfat.c
|---------------------------------------------
| Historic:
|---------------------------------------------
| Authors     | Date     | Comments
| $Log:$
==============================================*/
