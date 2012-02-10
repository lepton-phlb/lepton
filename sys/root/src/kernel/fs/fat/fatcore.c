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

#include "fatcore.h"
#include "fat16.h"
/*============================================
| Global Declaration
==============================================*/

/*============================================
| Implementation
==============================================*/
/*-------------------------------------------
| Name:_fat16_init
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _fat16_init(fat16_core_info_t * fat_info, desc_t dev_desc) {
   unsigned char buf[FAT16_BS_SIZE];
   unsigned int nb_clus;

   //get boot sector informations
   if(_fat_read_data(dev_desc,FAT16_BS_ADDR,buf,FAT16_BS_SIZE)<0) {
      return -1;
   }
   //init fat info
   if(buf[FAT16_BS_NBSEC16_OFF]==0 && buf[FAT16_BS_NBSEC16_OFF+1]==0) {
      fat_info->media_size   =
         (buf[FAT16_BS_NBSEC32_OFF]+
          ((fat16_u32_t)buf[FAT16_BS_NBSEC32_OFF+
                            1]<<
           8)+
          ((fat16_u32_t)buf[FAT16_BS_NBSEC32_OFF+
                            2]<<
           16)+((fat16_u32_t)buf[FAT16_BS_NBSEC32_OFF+3]<<24))*FAT16_BS_BPS_VAL;                                                                                                                                          // Taille totale du media en octets
   }
   else {
      fat_info->media_size   =
         (buf[FAT16_BS_NBSEC16_OFF]+((fat16_u32_t)buf[FAT16_BS_NBSEC16_OFF+1]<<8))*FAT16_BS_BPS_VAL;                        // Taille totale du media en octets
   }

   //nd sectors per cluster
   fat_info->nbsec_per_clus = buf[FAT16_BS_SPC_OFF];
   //boot sector address and size
   fat_info->bs_addr  = FAT16_BS_ADDR;
   fat_info->bs_size =
      (buf[FAT16_BS_RSVSEC_OFF]+((fat16_u32_t)buf[FAT16_BS_RSVSEC_OFF+1]<<8))*FAT16_BS_BPS_VAL;
   ////FOR MOMENT IGNORING THIS VALUE
   fat_info->hs_addr   = fat_info->bs_addr + fat_info->bs_size;
   fat_info->hs_size = 0;

   //fat address and size
   fat_info->fat_addr   = fat_info->hs_addr + fat_info->hs_size;
   fat_info->fat_size  =
      (buf[FAT16_BS_FATBS_OFF] + ((fat16_u32_t)buf[FAT16_BS_FATBS_OFF+1]<<8))*FAT16_BS_BPS_VAL;
   //root directory address and size
   fat_info->rd_addr = fat_info->fat_addr + (fat_info->fat_size * FAT16_BS_NBFAT_VAL);
   fat_info->rd_size = RD_SIZE *
                       (buf[FAT16_BS_NBROOTENT_OFF]+((fat16_u32_t)buf[FAT16_BS_NBROOTENT_OFF+1]<<8));
   //user dara address and size
   fat_info->ud_addr = fat_info->rd_addr + fat_info->rd_size;
   fat_info->ud_size = fat_info->media_size - fat_info->ud_addr;

   //check cluster number
   nb_clus = (fat_info->media_size/fat_info->nbsec_per_clus)/FAT16_BS_BPS_VAL;
   if ((nb_clus>FAT16_NBCLUSMAX)||(nb_clus<FAT16_NBCLUSMIN))
      return -1;

   //check bytes per sector : must be equal to BS_BYTSPERSEC
   if ((buf[FAT16_BS_BPS_OFF] + ((fat16_u16_t)buf[FAT16_BS_BPS_OFF+1]<<8)) != FAT16_BS_BPS_VAL)
      return -1;

   //check number of fat : must be 2
   if (buf[FAT16_BS_NBFAT_OFF] != FAT16_BS_NBFAT_VAL)
      return -1;

   //check fat type : must be FAT or FAT16
   if((buf[FAT16_BS_FSTYPE_OFF]!='F')||(buf[FAT16_BS_FSTYPE_OFF+1]!='A')||
      (buf[FAT16_BS_FSTYPE_OFF+2]!='T')) return(-1);
   if((buf[FAT16_BS_FSTYPE_OFF+4]=='1')&&(buf[FAT16_BS_FSTYPE_OFF+5]!='6')) return(-1);

   //add root directory to fat16 open file
   fat16_ofile_lst[0].entry_phys_addr     = fat_info->rd_addr;
   fat16_ofile_lst[0].entry_data_cluster = RD_CLUSEMPTY;
   fat16_ofile_lst[0].entry_attr        = RD_ATTR_VOLUMEID;

   //
   return nb_clus;
}


/*-------------------------------------------
| Name:_fat16_cluster_add
| Description:return cluster address in user data
| Parameters:
| Return Type:0 K0, address OK
| Comments:
| See:
---------------------------------------------*/
fat16_u32_t _fat16_cluster_add(fat16_core_info_t * fat_info, fat16_u16_t cluster) {
   fat16_u32_t addr = fat_info->ud_addr;

   //invalid cluster
   if ((cluster<FAT16_CLUSMIN) || (cluster>FAT16_CLUSMAX))
      return 0;

   addr = addr + ((fat16_u32_t)fat_info->nbsec_per_clus*FAT16_BS_BPS_VAL*(cluster-FAT16_CLUSMIN));
   return addr;
}

/*-------------------------------------------
| Name:_fat16_cluster_suiv
| Description:return next cluster of file or directory
| Parameters:
| Return Type:0 K0, next cluster OK
| Comments:
| See:
---------------------------------------------*/
fat16_u16_t _fat16_cluster_suiv(fat16_core_info_t * fat_info, desc_t dev_desc,
                                fat16_u16_t cluster) {
   fat16_u8_t buf[FAT16_CLUSSZ];
   fat16_u16_t cluster_suiv;

   //invalid cluster
   if ((cluster<FAT16_CLUSMIN) || (cluster>FAT16_CLUSMAX))
      return 0;

   //
#ifdef FAT_CACHE_FAT
   memcpy((void *)buf, fat_info->fat_cache+(cluster*FAT16_CLUSSZ), FAT16_CLUSSZ);
#else
   if(_fat_read_data(dev_desc,fat_info->fat_addr+(cluster*FAT16_CLUSSZ),buf,FAT16_CLUSSZ)<0) {
      return 0;
   }
#endif //FAT_CACHE_FAT

   cluster_suiv = ((fat16_u16_t)buf[1]<<8)+buf[0];
   if ((cluster_suiv<FAT16_CLUSMIN) || (cluster_suiv>FAT16_CLUSMAX)) {
      //invalid cluster
      if ((cluster_suiv<FAT16_LCLUSMIN) || (cluster_suiv>FAT16_LCLUSMAX))
         return(0);
      else //last cluster
         return(FAT16_LCLUSMAX);
   }
   return(cluster_suiv);
}

/*-------------------------------------------
| Name:_fat16_getclus
| Description:return first free cluster and set it as last and used
| Parameters:
| Return Type:0 K0, cluster number OK
| Comments:
| See:
---------------------------------------------*/
fat16_u16_t _fat16_getclus(fat16_core_info_t * fat_info, desc_t dev_desc,
                           bios_param_block_t * bpb) {
   fat16_u16_t cluster = FAT16_CLUSMIN-1;
   fat16_u8_t buf[FAT16_CLUSSZ];
   int i,j;
   unsigned char clean_buf[FAT_16_CLEAN_BUFFER_SIZE]={0};
   unsigned short * p_cluster = NULL;
   char found=0;

#ifdef FAT_CACHE_FAT
   //looking in cache fat
   p_cluster = (unsigned short *)fat_info->fat_cache;
   p_cluster += FAT16_CLUSMIN;
   for(cluster=FAT16_CLUSMIN; cluster<FAT16_CLUSMAX; cluster++, p_cluster++) {
      if(!(*p_cluster)) {
         break;
      }
   }

#else
   //looking for first free cluster in FAT
   for(i=0; i<(fat_info->fat_size/FAT_16_CLEAN_BUFFER_SIZE); i++) {
      //read block in cache
      if(_fat_read_data(dev_desc,fat_info->fat_addr+(i*FAT_16_CLEAN_BUFFER_SIZE),clean_buf,
                        FAT_16_CLEAN_BUFFER_SIZE)<0) {
         return 0;
      }
      //
      p_cluster = (unsigned short *)clean_buf;
      for(j=0; j<FAT_16_CLEAN_BUFFER_SIZE; j+=2) {
         //free cluster found
         if(!(*p_cluster)) {
            found = 1;
            break;
         }
         p_cluster++;
      }

      if(found) {
         cluster = i*FAT_16_CLEAN_BUFFER_SIZE/FAT16_CLUSSZ + j/2;
         break;
      }
   }
#endif //FAT_CACHE_FAT

   //no available cluster
   if (cluster == (fat_info->fat_size/FAT16_CLUSSZ))
      return 0;

   //allocate cluster in 2 FATs
   buf[0] = (fat16_u8_t)FAT16_LCLUSMAX;
   buf[1] = (FAT16_LCLUSMAX>>8);

   if(_fat_write_data(dev_desc,fat_info->fat_addr+(cluster*FAT16_CLUSSZ),buf,FAT16_CLUSSZ)<0) {
      return 0;
   }
   if(_fat_write_data(dev_desc,fat_info->fat_addr+fat_info->fat_size+(cluster*FAT16_CLUSSZ),buf,
                      FAT16_CLUSSZ)<0) {
      return 0;
   }

#ifdef FAT_CACHE_FAT
   //update fat cache
   memcpy((void *)p_cluster, buf, FAT16_CLUSSZ);
#endif //FAT_CACHE_FAT

   return cluster;
}

/*-------------------------------------------
| Name:_fat16_putclus
| Description:free cluster number
| Parameters:
| Return Type:-1 K0, 0 OK
| Comments:
| See:
---------------------------------------------*/
int _fat16_putclus(fat16_core_info_t * fat_info, desc_t dev_desc, fat16_u16_t cluster) {
   fat16_u8_t buf[FAT16_CLUSSZ];

   //free cluster in 2 FATs
   buf[0] = RD_CLUSEMPTY;
   buf[1] = (RD_CLUSEMPTY>>8);

   //
   if(_fat_write_data(dev_desc,fat_info->fat_addr+(cluster*FAT16_CLUSSZ),buf,FAT16_CLUSSZ)<0) {
      return -1;
   }
   if(_fat_write_data(dev_desc,fat_info->fat_addr+fat_info->fat_size+(cluster*FAT16_CLUSSZ),buf,
                      FAT16_CLUSSZ)<0) {
      return -1;
   }

#ifdef FAT_CACHE_FAT
   //update fat cache
   memcpy((void *)(fat_info->fat_cache + cluster*FAT16_CLUSSZ), buf, FAT16_CLUSSZ);
#endif //FAT_CACHE_FAT

   return 0;
}

/*-------------------------------------------
| Name:_fat16_chainclus
| Description:link to cluster
| Parameters:
| Return Type:0 K0, cluster number OK
| Comments:
| See:
---------------------------------------------*/
int _fat16_chainclus(fat16_core_info_t * fat_info, desc_t dev_desc, fat16_u16_t cluster_curr,
                     fat16_u16_t cluster_next) {
   fat16_u32_t addr = 0;
   unsigned char buf[FAT16_CLUSSZ]={0};

   //check cluster values
   if ((cluster_curr<FAT16_CLUSMIN) || (cluster_curr>FAT16_CLUSMAX))
      return -1;
   if ((cluster_next<FAT16_CLUSMIN) || (cluster_next>FAT16_CLUSMAX))
      return -1;

   addr = fat_info->fat_addr + (cluster_curr*FAT16_CLUSSZ);

   buf[0] = (fat16_u8_t)cluster_next;
   buf[1] = (fat16_u8_t)(cluster_next>>8);

   if(_fat_write_data(dev_desc,addr,buf,FAT16_CLUSSZ)<0) {
      return -1;
   }
   if(_fat_write_data(dev_desc,addr+fat_info->fat_size,buf,FAT16_CLUSSZ)<0) {
      return -1;
   }

#ifdef FAT_CACHE_FAT
   //update fat cache
   memcpy((void *)(fat_info->fat_cache + cluster_curr*FAT16_CLUSSZ), buf, FAT16_CLUSSZ);
#endif //FAT_CACHE_FAT

   return 0;
}

/*-------------------------------------------
| Name:_fat16_delcluslist
| Description:free a list of clusters, provide first one
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void _fat16_delcluslist(fat16_core_info_t * fat_info, desc_t desc, fat16_u16_t cluster) {
   fat16_u16_t cluster_suiv;

   do {
      //keep next cluster and free it
      cluster_suiv = _fat16_cluster_suiv(fat_info,__get_dev_desc(desc),cluster);
      _fat16_putclus(fat_info,__get_dev_desc(desc),cluster);
      cluster = cluster_suiv;
   } while (cluster != FAT16_LCLUSMAX); //while cluster remain
   return;
}

/*-------------------------------------------
| Name:_fat16_checkdirempty
| Description:check if a directory is empty
| Parameters:
| Return Type:-1 KO, 0 OK
| Comments:
| See:
---------------------------------------------*/
int _fat16_checkdirempty(fat16_core_info_t * fat_info, desc_t desc) {
   fat16_u8_t buf;
   fat16_u16_t cluster = fat16_ofile_lst[desc].entry_data_cluster;
   fat16_u32_t addr;
   int flag_fin=0, nument;

   do {
      addr = _fat16_cluster_add(fat_info, cluster);
      if(addr == 0)
         return -1;

      nument=0;
      do {
         if(_fat_read_data(__get_dev_desc(desc),addr+(RD_SIZE*nument),&buf,1)<0) {
            return -1;
         }

         //error:directory is not empty
         if((buf!=RD_ENT_FREE)&&(buf!=RD_ENT_REMOVE)&&(buf!='.')) {
            return -1;
         }
         // end of directory : directory empty
         if(buf==RD_ENT_FREE) {
            flag_fin=1;
         }
         nument++;
      } while((flag_fin==0)&&
              (nument<((fat16_u16_t)fat_info->nbsec_per_clus/RD_SIZE*FAT16_BS_BPS_VAL)));

      //
      cluster = _fat16_cluster_suiv(fat_info, __get_dev_desc(desc), cluster);
      if(!cluster) {
         return -1;
      }
      if (cluster==FAT16_LCLUSMAX) {
         flag_fin=1;
      }
   } while(flag_fin!=1);

   return 0;
}

/*-------------------------------------------
| Name:_fat16_offsetinfo
| Description:return current cluster containing offset and current offset
| Parameters:
| Return Type:-1 KO, 0 OK
| Comments:
| See:
---------------------------------------------*/
int _fat16_offsetinfo(fat16_core_info_t * fat_info, desc_t desc, fat16_u16_t* clus,
                      fat16_u16_t* offset,
                      unsigned char prev_flag) {
   fat16_u32_t off = ofile_lst[desc].offset;
   fat16_u16_t prev;

   *clus = fat16_ofile_lst[desc].entry_data_cluster;
   //no data
   if(*clus == 0)
      return -1;

   while (off>((fat_info->nbsec_per_clus*FAT16_BS_BPS_VAL)-1)) {
      prev = *clus;

      *clus = _fat16_cluster_suiv(fat_info, __get_dev_desc(desc), *clus);
      //no more data
      if(*clus == 0)
         return -1;
      off = off-(fat_info->nbsec_per_clus*FAT16_BS_BPS_VAL);
   }

   *offset = off;

   if(!off && prev_flag) {
      *clus = prev;
   }

   return(0);
}

/*-------------------------------------------
| Name:_fat16_adddir
| Description:return physical address of current directory item
| Parameters:
| Return Type:-1 KO, 0 OK
| Comments:
| See:
---------------------------------------------*/
fat16_u32_t _fat16_adddir(fat16_core_info_t * fat_info, desc_t desc) {
   fat16_u32_t current_addr;
   fat16_u16_t current_clus;
   fat16_u8_t buf;
   int offset=ofile_lst[desc].offset;

   current_clus=fat16_ofile_lst[desc].entry_data_cluster;
   if(current_clus == FAT16_LCLUSMAX)
      return 0;

   current_addr=_fat16_cluster_add(fat_info, current_clus);
   if(current_addr == 0)
      return 0;

   while(offset>=(fat_info->nbsec_per_clus*FAT16_BS_BPS_VAL)) {
      current_clus = _fat16_cluster_suiv(fat_info, __get_dev_desc(desc), current_clus);
      if(current_clus == FAT16_LCLUSMAX)
         return 0;

      current_addr=_fat16_cluster_add(fat_info, current_clus);
      if(current_addr == 0)
         return 0;
      //
      offset-= (fat_info->nbsec_per_clus*FAT16_BS_BPS_VAL);
   }

   //add offset and read media
   current_addr+=offset;
   if(_fat_read_data(__get_dev_desc(desc),current_addr,&buf,1)<0) {
      return 0;
   }

   //error : end of directory, offset out of limit
   if (buf==0x00)
      return 0;

   return(current_addr);
}

/*-------------------------------------------
| Name:fat16_lastclus
| Description:free cluster in FATs
| Parameters:
| Return Type:-1 KO, 0 OK
| Comments:
| See:
---------------------------------------------*/
int _fat16_lastclus(fat16_core_info_t * fat_info, desc_t dev_desc, fat16_u16_t cluster) {
   fat16_u8_t buf[FAT16_CLUSSZ];

   //free cluster in 2 FATs
   buf[0] = (fat16_u8_t)FAT16_LCLUSMAX;
   buf[1] = (fat16_u8_t)(FAT16_LCLUSMAX>>8);

   //update 2 FATs
   if(_fat_write_data(dev_desc,fat_info->fat_addr+(cluster*FAT16_CLUSSZ),buf,FAT16_CLUSSZ)<0) {
      return -1;
   }
   if(_fat_write_data(dev_desc,fat_info->fat_addr+fat_info->fat_size+(cluster*FAT16_CLUSSZ),buf,
                      FAT16_CLUSSZ)<0) {
      return -1;
   }

#ifdef FAT_CACHE_FAT
   //update fat cache
   memcpy((void *)(fat_info->fat_cache + cluster*FAT16_CLUSSZ), buf, FAT16_CLUSSZ);
#endif //FAT_CACHE_FAT

   return 0;
}

/*-------------------------------------------
| Name:_fat16_cleanclus
| Description:free cluster in FATs
| Parameters:
| Return Type:-1 KO, 0 OK
| Comments:
| See:
---------------------------------------------*/
int _fat16_cleanclus(fat16_core_info_t * fat_info, desc_t dev_desc, bios_param_block_t * bpb,
                     fat16_u16_t cluster) {

   fat16_u32_t addr;
   int i;
   unsigned char clean_buf[FAT_16_CLEAN_BUFFER_SIZE]={0};
   int count=0;

   //erase cluster content
   addr = _fat16_cluster_add(fat_info, cluster);
   memset((void *)clean_buf, 0, FAT_16_CLEAN_BUFFER_SIZE);

   //
   count = (bpb->BPB_BytesPerSec*bpb->BPB_SecPerClust)/FAT_16_CLEAN_BUFFER_SIZE;
   for(i=0; i<count; i++) {
      if(_fat_write_data(dev_desc,addr+(i*FAT_16_CLEAN_BUFFER_SIZE),clean_buf,
                         FAT_16_CLEAN_BUFFER_SIZE)<0) {
         return 0;
      }
   }
   return 0;
}

/*-------------------------------------------
| Name:_fat16_getdatetime
| Description:
| Parameters:
| Return Type:
| Comments:
|          Bits  15-9 : Annee-1980
|          Bits   8-5 : Mois (1 a 12)
|          Bits   4-0 : Jour (1 a 31)
|          2nd octet
|          Bits 15-11 : Heures (0 a 23)
|          Bits  10-5 : Minutes (0 a 59)
|          Bits   4-0 : Secondes/2 (0 a 29)
|
| See:
---------------------------------------------*/
extern void __tm_conv(struct tm *tmbuf,const time_t *t,time_t offset);
void _fat16_getdatetime(unsigned char *buf_date, unsigned char *buf_time) {
   fat16_u32_t datetime = 0;
   struct __timeval now_tv = {0};
   struct tm now_tm = {0};
   struct timezone tz = {0};
   time_t offt = 0;
   unsigned short convert=0;

   //
   _sys_gettimeofday(&now_tv,&tz);
   offt = (time_t)(-tz.tz_minuteswest*60L);
   __tm_conv(&now_tm, &now_tv.tv_sec, offt);

   //
   if(buf_time) {
      convert = now_tm.tm_sec/2+(now_tm.tm_min<<5)+(now_tm.tm_hour<<11);
      memcpy((void *)buf_time,(void *)&convert,2);
   }

   convert = now_tm.tm_mday+((now_tm.tm_mon+1)<<5)+(((now_tm.tm_year%100)+20)<<9);
   memcpy((void *)buf_date,(void *)&convert,2);
}

/*-------------------------------------------
| Name:_fat16_converttime
| Description:
| Parameters:
| Return Type:
| Comments:
| See:_fat16_getdatetime
---------------------------------------------*/
time_t _fat16_converttime(const unsigned char *buf_date, const unsigned char *buf_time) {
   struct tm tm={0};
   unsigned short convert=0;

   //
   memcpy((void *)&convert, (void *)buf_time, 2);
   tm.tm_sec = convert & 0x1f;
   tm.tm_min = (convert & 0x7e0)>>5;
   tm.tm_hour = (convert & 0xf800)>>11;

   //
   memcpy((void *)&convert, (void *)buf_date, 2);
   tm.tm_mday = convert & 0x1f;
   tm.tm_mon = ((convert & 0x1e0)-1)>>5;
   tm.tm_year = (((convert & 0xfe00))>>9)+80;
   tm.tm_isdst = -1;

   return __mktime(&tm);
}

/*-------------------------------------------
| Name:_fat16_create_fat_entry
| Description:create fat short entry containing all metadata infos
| Parameters:
| Return Type:
| Comments:
| See:_fat16_getdatetime
---------------------------------------------*/
fat16_u16_t _fat16_create_fat_entry(fat_msdos_dir_entry_t *msdos_entry, desc_t desc, int attr) {
   fat16_u16_t cluster=0;

   if(attr == S_IFDIR) {
      msdos_entry->DIR_Attr = RD_ATTR_DIRECTORY;
      cluster =
         _fat16_getclus(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info, __get_dev_desc(
                           desc), &ofile_lst[desc].pmntdev->fs_info.fat_info.fat_boot_info->bpb);
      //error
      if(!cluster)
         return INVALID_INODE_NB;
      //
      msdos_entry->DIR_FstClusLO[0] = cluster;
      msdos_entry->DIR_FstClusLO[1] = cluster>>8;
   }
   else {
      msdos_entry->DIR_Attr = RD_ATTR_ARCHIVE;
      msdos_entry->DIR_FstClusLO[0] = msdos_entry->DIR_FstClusLO[1] = 0x00;
   }

   msdos_entry->DIR_NTRes = RD_RSVD_VAL;
   msdos_entry->DIR_CrtTimeTenth = 0x00;
   //creation time
   _fat16_getdatetime(msdos_entry->DIR_CrtDate, msdos_entry->DIR_CrtTime);
   msdos_entry->DIR_LstAccDate[0] = msdos_entry->DIR_CrtDate[0];
   msdos_entry->DIR_LstAccDate[1] = msdos_entry->DIR_CrtDate[1];
   //always 0
   msdos_entry->DIR_FstClusHI[0] = msdos_entry->DIR_FstClusHI[1] = 0x00;
   //write time
   msdos_entry->DIR_WrtTime[0] = msdos_entry->DIR_CrtTime[0];
   msdos_entry->DIR_WrtTime[1] = msdos_entry->DIR_CrtTime[1];
   msdos_entry->DIR_WrtDate[0] = msdos_entry->DIR_CrtDate[0];
   msdos_entry->DIR_WrtDate[1] = msdos_entry->DIR_CrtDate[1];
   //size
   msdos_entry->DIR_FileSize = 0;

   return cluster;
}

/*-------------------------------------------
| Name:_fat16_create_fat_directory_entry
| Description:create . and .. for a directory entry
| Parameters:
| Return Type:
| Comments:
| See:_fat16_getdatetime
---------------------------------------------*/
int _fat16_create_fat_directory_entry(fat_msdos_dir_entry_t *msdos_entry, desc_t desc,
                                      fat16_u16_t cluster,
                                      fat16_u16_t parent_cluster) {
   fat16_u32_t addr = _fat16_cluster_add(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info,
                                         cluster);

   if(!addr)
      return INVALID_INODE_NB;

   memset((void *)msdos_entry,0,sizeof(fat_msdos_dir_entry_t));
   memset((void *)&msdos_entry->DIR_Name, 0x20, FAT16_MAX_NAME_FIL+FAT16_MAX_EXT_FIL);

   //dot
   msdos_entry->DIR_Name[0] = '.';
   msdos_entry->DIR_FstClusLO[0] = cluster;
   msdos_entry->DIR_FstClusLO[1] = cluster>>8;

   if(_fat_write_data(__get_dev_desc(desc),addr,(unsigned char *)msdos_entry,RD_SIZE)<0) {
      return INVALID_INODE_NB;
   }

   //dotdot
   msdos_entry->DIR_Name[0] = '.';
   msdos_entry->DIR_Name[1] = '.';
   msdos_entry->DIR_Attr = RD_ATTR_DIRECTORY;
   msdos_entry->DIR_FstClusLO[0] = parent_cluster;
   msdos_entry->DIR_FstClusLO[1] = parent_cluster>>8;

   if(_fat_write_data(__get_dev_desc(desc),addr+RD_SIZE,(unsigned char *)msdos_entry,RD_SIZE)<0) {
      return INVALID_INODE_NB;
   }
   return 0;
}

/*-------------------------------------------
| Name:_fat_write_data
| Description:write data on media
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _fat_write_data(desc_t dev_desc, unsigned long addr, unsigned char *data, unsigned int size) {
   ofile_lst[dev_desc].pfsop->fdev.fdev_seek(dev_desc,addr,SEEK_SET);
   return ofile_lst[dev_desc].pfsop->fdev.fdev_write(dev_desc,(char *)data, size);
}

/*-------------------------------------------
| Name:_fat_read_data
| Description:read data on media
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _fat_read_data(desc_t dev_desc, unsigned long addr, unsigned char *data, unsigned int size) {
   ofile_lst[dev_desc].pfsop->fdev.fdev_seek(dev_desc,addr,SEEK_SET);
   return ofile_lst[dev_desc].pfsop->fdev.fdev_read(dev_desc,(char *)data, size);
}

/*-------------------------------------------
| Name:_fat16_getdesc
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _fat16_getdesc(desc_t desc, fat16_u32_t addr) {
   fat_dir_entry_t fat_entry={0};

   //fill entry in fat16 open files
   fat16_ofile_lst[desc].entry_phys_addr = addr;

   if(_fat_read_data(__get_dev_desc(desc),fat16_ofile_lst[desc].entry_infos_addr,
                     (unsigned char *)&fat_entry.msdos_entry,RD_SIZE)<0) {
      return -1;
   }

   fat16_ofile_lst[desc].entry_data_cluster = fat_entry.msdos_entry.DIR_FstClusLO[0]+
                                              ((fat16_u16_t)fat_entry.msdos_entry.DIR_FstClusLO[1]
                                               <<8);
   fat16_ofile_lst[desc].entry_attr = fat_entry.msdos_entry.DIR_Attr;

   if (fat_entry.msdos_entry.DIR_Attr == RD_ATTR_ARCHIVE) {
      ofile_lst[desc].attr = S_IFREG;
   }
   else {
      ofile_lst[desc].attr = S_IFDIR;
   }

   ofile_lst[desc].size = fat_entry.msdos_entry.DIR_FileSize;
   //need to calculate ctime
   ofile_lst[desc].cmtime = _fat16_converttime(fat_entry.msdos_entry.DIR_WrtDate,
                                               fat_entry.msdos_entry.DIR_WrtTime);
   ofile_lst[desc].offset = 0;

   return 0;
}
/*============================================
| End of Source  : fatcore.c
|---------------------------------------------
| Historic:
|---------------------------------------------
| Authors     | Date     | Comments
| $Log:$
==============================================*/
