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
#include <string.h>

#include "kernel/core/system.h"
#include "kernel/core/time.h"
#include "kernel/core/systime.h"

#include "kernel/fs/vfs/vfstypes.h"
#include "kernel/fs/vfs/vfskernel.h"
#include "kernel/fs/vfs/vfs.h"

#include "fat16.h"
#include "fatcore.h"
#include "fatcore_msdos.h"
/*============================================
| Global Declaration
==============================================*/


/*============================================
| Implementation
==============================================*/
/*-------------------------------------------
| Name:_fat16_checkchar
| Description:check if caract is in limit
| Parameters:
| Return Type:-1 KO, 0 OK
| Comments:
| See:
---------------------------------------------*/
int _fat16_msdos_checkchar(char caract) {
   if ((caract>='0') && (caract<='9'))
      return 0;
   if ((caract>='A') && (caract<='Z'))
      return 0;
   if ((caract=='~')||(caract=='!')||(caract=='#')||
      (caract=='$')||(caract=='%')||(caract=='&')||
      (caract=='(')||(caract==')')||(caract=='-')||
      (caract=='@')||(caract=='^')||(caract=='_')||
      (caract=='`')||(caract=='{')||(caract=='}'))
      return 0;

   return -1;
}

/*-------------------------------------------
| Name:_fat16_msdos_lookupnameCluster
| Description:search filename in a cluster or in list of clusters in user data
| Parameters:
| Return Type:-1 KO, 0 OK
| Comments:
| See:
---------------------------------------------*/
int _fat16_msdos_lookupname_ud(fat16_core_info_t * fat_info, desc_t desc_new, char* filename) {
   int i=0, j=0,found=0;
   fat16_u32_t addr;
   fat16_u16_t cluster = fat16_ofile_lst[desc_new].entry_data_cluster;
   fat_msdos_dir_entry_t fat_entry={0};

   while(!found) {
      addr = _fat16_cluster_add(fat_info,cluster);
      i=0;
      while((i<((fat_info->nbsec_per_clus * FAT16_BS_BPS_VAL)/RD_SIZE))&&(!found)) {
         ofile_lst[__get_dev_desc(desc_new)].pfsop->fdev.fdev_seek(__get_dev_desc(desc_new),addr,SEEK_SET);
         if(ofile_lst[__get_dev_desc(desc_new)].pfsop->fdev.fdev_read(__get_dev_desc(desc_new),(char *)&fat_entry,RD_SIZE) < 0)
            return -1;

         //end of directory
         if (fat_entry.DIR_Name[0] == RD_ENT_FREE)
            return -1;

         //remove entry
         if (fat_entry.DIR_Name[0] == RD_ENT_REMOVE) {
            found = 0;
            addr += RD_SIZE;
            i++;
            continue;
         }

//         for (j=0;j<(FAT16_MAX_NAME_FIL+FAT16_MAX_EXT_FIL);j++)
//            if (fat_entry.DIR_Name[j]==filename[j])
//               found++;
//
//         //is name matching
//         if (found != (FAT16_MAX_NAME_FIL+FAT16_MAX_EXT_FIL)) {
//            found = 0;
//            addr += RD_SIZE;
//         }
//         i++;
         //entry found
         if(!strncmp(fat_entry.DIR_Name,filename,FAT16_MAX_NAME_FIL+FAT16_MAX_EXT_FIL)) {
            found=1;
         }
         else {
            //+1 => skip short dir entry
            addr += RD_SIZE;
            i++;
         }
      }
      cluster = _fat16_cluster_suiv(fat_info, __get_dev_desc(desc_new), cluster);
      if(!found) {
         //end of data
         if(cluster == FAT16_LCLUSMAX) return -1;
      }
   }

   //fill entry in fat16 open files
   fat16_ofile_lst[desc_new].entry_infos_addr = addr;
   _fat16_getdesc(desc_new, addr);

   return 0;
}

/*-------------------------------------------
| Name:_fat16_msdos_lookupname_rd
| Description:search filename in a cluster or in list of clusters in root directory
| Parameters:
| Return Type:-1 KO, 0 OK
| Comments:
| See:
---------------------------------------------*/
int _fat16_msdos_lookupname_rd(fat16_core_info_t * fat_info, desc_t desc_new, char* filename) {
   int j=0,found=0;
   //skip volume name
   fat16_u32_t addr = fat_info->rd_addr + RD_SIZE;
   fat_msdos_dir_entry_t fat_entry={0};

   //find entry in rootdir
   while(!found){
      ofile_lst[__get_dev_desc(desc_new)].pfsop->fdev.fdev_seek(__get_dev_desc(desc_new),addr,SEEK_SET);
      if(ofile_lst[__get_dev_desc(desc_new)].pfsop->fdev.fdev_read(__get_dev_desc(desc_new),(char *)&fat_entry,RD_SIZE) < 0)
         return -1;

      //end of directory
      if (fat_entry.DIR_Name[0] == RD_ENT_FREE)
         return -1;

      //remove entry
      if (fat_entry.DIR_Name[0] == RD_ENT_REMOVE) {
         addr += RD_SIZE;
         found = 0;
         //end of root directory
         if (addr == fat_info->ud_addr){
            return -1;
         }
         continue;
      }

      //
//      for (j=0;j<(FAT16_MAX_NAME_FIL+FAT16_MAX_EXT_FIL);j++)
//         if (fat_entry.DIR_Name[j]==filename[j])
//            found++;
//
//      if (found !=  (FAT16_MAX_NAME_FIL+FAT16_MAX_EXT_FIL)) {
//         addr += RD_SIZE;
//         found = 0;
//         //end of root directory
//         if (addr == fat_info->ud_addr){
//            return -1;
//         }
//      }
      //entry found
      if(!strncmp(fat_entry.DIR_Name,filename,FAT16_MAX_NAME_FIL+FAT16_MAX_EXT_FIL)) {
         found=1;
      }
      else {
         //+1 => skip short dir entry
         addr += RD_SIZE;
         if (addr == fat_info->ud_addr){
            return -1;
         }
      }

   }

   //fill entry in fat16 open files
   fat16_ofile_lst[desc_new].entry_infos_addr = addr;
   _fat16_getdesc(desc_new, addr);
   return 0;
}

/*-------------------------------------------
| Name:_fat16_msdos_getname
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _fat16_msdos_getname(const char *filename, char *fat_name) {
   char *token=NULL;
   char *saveptr;
   //+2 for \0 and maybe "."
   char local_filename[FAT16_MAX_NAME_FIL+FAT16_MAX_EXT_FIL+2]={0};

   int size=0;
   int i=0;

   //
   memset((void *)fat_name, 0x20, FAT16_MAX_NAME_FIL+FAT16_MAX_EXT_FIL);

   //special case for "." and ".."
   if(filename[0]=='.') {
      fat_name[0]=filename[0];
      if(filename[1]=='.') {
         fat_name[1]=filename[1];
      }
      return 0;
   }
   //save filename in local var
   strncpy(local_filename,filename,FAT16_MAX_NAME_FIL+FAT16_MAX_EXT_FIL+1);

   //seperate name and extension for comparison
   token = strtok_r(local_filename,".", &saveptr);
   if((size=strlen(token))>FAT16_MAX_NAME_FIL) {
      return -1;
   }

   //name
   for(i=0;i<FAT16_MAX_NAME_FIL;i++) {
      if(i<size) {
         fat_name[i]=token[i];
      }
      else {
         break;
      }
   }

   //extension
   if((token=strtok_r(NULL,"\0",(char **)&saveptr))) {
      if((size=strlen(token))>FAT16_MAX_EXT_FIL) {
         return -1;
      }
      for(i=FAT16_MAX_NAME_FIL;i<(FAT16_MAX_NAME_FIL+FAT16_MAX_EXT_FIL);i++) {
         if(i<(size+FAT16_MAX_NAME_FIL)) {
            fat_name[i]=token[i-FAT16_MAX_NAME_FIL];
         }
         else {
            break;
         }
      }
   }

   //
   return 0;
}

/*-------------------------------------------
| Name:_fat16_msdos_fillname
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _fat16_msdos_fillname(unsigned char *dst, unsigned char *src) {
   int i=0,j=0;
   if(!dst || !src)  return -1;

   //copy name
   for(i=0;i<FAT16_MAX_NAME_FIL && src[i]!=0x20;i++) {
      dst[i] = src[i];
   }

   if(src[FAT16_MAX_NAME_FIL]==0x20) {
      //no extension
      dst[i] = '\0';
      return 0;
   }

   //copy extension
   dst[i++]='.';

   for(j=FAT16_MAX_NAME_FIL;j<(FAT16_MAX_NAME_FIL+FAT16_MAX_EXT_FIL) || src[j]!=0x20; j++) {
      dst[i++] = src[j];
   }
   dst[i] = '\0';
   return 0;
}

/*-------------------------------------------
| Name:_fat16_msdos_get_addr_dot_items
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
fat16_u32_t _fat16_msdos_get_addr_dot_items(fat16_core_info_t * fat_info, desc_t desc, fat16_u16_t parent_cluster, fat16_u16_t pattern_cluster) {
   fat16_u32_t current_addr = 0;
   fat16_u16_t cluster = 0;
   fat_msdos_dir_entry_t fat_entry={0};
   int found=0,j=0;
   fat16_u32_t return_addr = 0;

   current_addr = _fat16_cluster_add(fat_info, parent_cluster);

   //we are after dotdot we need to get dotdot
   if(_fat_read_data(__get_dev_desc(desc),current_addr+RD_SIZE,(unsigned char *)&fat_entry,RD_SIZE)<0) {
      return 0;
   }

   cluster = fat_entry.DIR_FstClusLO[0] + ((fat16_u16_t)fat_entry.DIR_FstClusLO[1]<<8);
   //2 cases
   //dot is in rootdir
   if(!cluster) {
      current_addr = fat_info->rd_addr+RD_SIZE;
      j=1;
      while((j<((fat_info->nbsec_per_clus * FAT16_BS_BPS_VAL)/RD_SIZE))) {
         if(_fat_read_data(__get_dev_desc(desc),current_addr,(unsigned char *)&fat_entry,RD_SIZE)<0) {
            return 0;
         }

         //end of directory
         if (fat_entry.DIR_Name[0] == RD_ENT_FREE)
            return 0;

         //remove entry
         if (fat_entry.DIR_Name[0] == RD_ENT_REMOVE) {
            current_addr += RD_SIZE;
            j++;
            continue;
         }

         if (fat_entry.DIR_Name[0]!= '.'
               && (pattern_cluster == (fat_entry.DIR_FstClusLO[0] + ((fat16_u16_t)fat_entry.DIR_FstClusLO[1]<<8)))) {
            return current_addr;
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
            if(_fat_read_data(__get_dev_desc(desc),current_addr,(unsigned char *)&fat_entry,RD_SIZE)<0) {
               return 0;
            }

            //end of directory
            if (fat_entry.DIR_Name[0] == RD_ENT_FREE)
               return 0;

            //remove entry
            if (fat_entry.DIR_Name[0] == RD_ENT_REMOVE) {
               current_addr += RD_SIZE;
               j++;
               continue;
            }

            //cluster match?
            if (fat_entry.DIR_Name[0]!= '.'
                  && (pattern_cluster == (fat_entry.DIR_FstClusLO[0] + ((fat16_u16_t)fat_entry.DIR_FstClusLO[1]<<8)))) {
               return current_addr;
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
/*============================================
| End of Source  : fatcore_msdos.c
|---------------------------------------------
| Historic:
|---------------------------------------------
| Authors     | Date     | Comments
| $Log:$
==============================================*/
