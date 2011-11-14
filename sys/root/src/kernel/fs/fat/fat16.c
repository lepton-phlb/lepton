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

#include "fatcore.h"
#include "fat16.h"
/*============================================
| Global Declaration
==============================================*/
//
static fat16_boot_core_info_t fat16_boot_core_info = {0};
static fat16_core_info_t fat16_core_info={0};

fat16_ofile_t fat16_ofile_lst[MAX_OPEN_FILE]={0};
/*============================================
| Implementation
==============================================*/
/*-------------------------------------------
| Name:_fat_loadfs
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _fat_loadfs(void) {
   return 0;
}

/*-------------------------------------------
| Name:_fat_makefs
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _fat_makefs(desc_t dev_desc,struct vfs_formatopt_t* vfs_formatopt) {
   unsigned int nbblocks=0;
   unsigned char buf[FAT16_BS_BPS_VAL]= {0};
   int i=0;
   unsigned char volume_name[]=FAT16_VOLUME_NAME;

   /* CREATION DE LA STRUCTURE DE LA FAT */
   nbblocks = vfs_formatopt->dev_sz / FAT16_BS_BPS_VAL;                 // On calcule le nb de secteurs par cluster
   if (nbblocks<FAT16_NBCLUSMIN) return(-1);       // ERREUR : Pas assez de blocs pour une Fat16
   fat16_core_info.nbsec_per_clus = FAT16_SECMIN;

   while((nbblocks/fat16_core_info.nbsec_per_clus)>FAT16_NBCLUSMAX) fat16_core_info.nbsec_per_clus = fat16_core_info.nbsec_per_clus<<1;

   if(fat16_core_info.nbsec_per_clus > FAT16_SECMAX) return(-1);       // ERREUR : Trop de clusters pour une Fat16

   fat16_core_info.media_size    = vfs_formatopt->dev_sz;
   fat16_core_info.bs_addr   = FAT16_BS_ADDR;
   fat16_core_info.bs_size  = FAT16_BS_RSVSEC_VAL * FAT16_BS_BPS_VAL;
   fat16_core_info.hs_addr    = fat16_core_info.bs_size;
   fat16_core_info.hs_size   = FAT16_BS_NBSHID_VAL * FAT16_BS_BPS_VAL;
   fat16_core_info.fat_addr    = fat16_core_info.hs_addr + fat16_core_info.hs_size;

   //from http://support.microsoft.com/kb/451012/fr
#ifndef USE_KERNEL_STATIC
   //fat16.SizeF   = (ceil(((2*(nbblocks-BS_RSVD-(BS_NBRD*RD_SIZE/BS_BYTS)+2*fat16.NbSect))/((2*BS_NBFAT)+(BS_BYTS*fat16.NbSect)))+0.5))*BS_BYTS;
   fat16_core_info.fat_size = (((2*(nbblocks-FAT16_BS_RSVSEC_VAL-(FAT16_BS_NBROOTENT_VAL*RD_SIZE/FAT16_BS_BPS_VAL)+2*fat16_core_info.nbsec_per_clus))/((2*FAT16_BS_NBFAT_VAL)+(FAT16_BS_BPS_VAL*fat16_core_info.nbsec_per_clus)))+0.5);
   fat16_core_info.fat_size *= FAT16_BS_BPS_VAL;
#else
   fat16_core_info.fat_size   = 0;
#endif

   fat16_core_info.rd_addr   = fat16_core_info.fat_addr + (2*fat16_core_info.fat_size);
   fat16_core_info.rd_size  = RD_SIZE * FAT16_BS_NBROOTENT_VAL;
   fat16_core_info.ud_addr   = fat16_core_info.rd_addr + fat16_core_info.rd_size;
   fat16_core_info.ud_size  = vfs_formatopt->dev_sz - fat16_core_info.ud_addr;

   //clear media until user data
   for(i=0;i<FAT16_BS_BPS_VAL;i++) buf[i]=0x00;
   for(i=0;i<(fat16_core_info.ud_addr/FAT16_BS_BPS_VAL);i++) {
      if(_fat_write_data(dev_desc,fat16_core_info.bs_addr+((fat16_u32_t)i*FAT16_BS_BPS_VAL),buf,FAT16_BS_BPS_VAL)<0) {
         return -1;
      }
   }

   //create boot sector
   for(i=0;i<FAT16_BS_JUMPBOOT_SIZE;i++)   buf[FAT16_BS_JUMPBOOT_OFF+i] = (FAT16_BS_JUMPBOOT_VAL>>(16-(8*i))) & 0xFF;
   for(i=0;i<FAT16_BS_OEM_SIZE;i++)    buf[FAT16_BS_OEM_OFF+i] = FAT16_BS_OEM_VAL[i];
   for(i=0;i<FAT16_BS_BPS_SIZE;i++)   buf[FAT16_BS_BPS_OFF+i] = (FAT16_BS_BPS_VAL>>(8*i)) & 0xFF;

   buf[FAT16_BS_SPC_OFF] = fat16_core_info.nbsec_per_clus;


   for(i=0;i<FAT16_BS_RSVSEC_SIZE;i++)   buf[FAT16_BS_RSVSEC_OFF+i] = (FAT16_BS_RSVSEC_VAL>>(8*i)) & 0xFF;
   buf[FAT16_BS_NBFAT_OFF] = FAT16_BS_NBFAT_VAL;

   for(i=0;i<FAT16_BS_NBROOTENT_SIZE;i++)   buf[FAT16_BS_NBROOTENT_OFF+i] = (FAT16_BS_NBROOTENT_VAL>>(8*i)) & 0xFF;

   buf[FAT16_BS_MEDIA_OFF] = FAT16_BS_MEDIA_VAL;

   for(i=0;i<FAT16_BS_FATBS_SIZE;i++)  buf[FAT16_BS_FATBS_OFF+i] = ((fat16_core_info.fat_size/FAT16_BS_BPS_VAL)>>(8*i)) & 0xFF;
   for(i=0;i<FAT16_BS_TCKPS_SIZE;i++)    buf[FAT16_BS_TCKPS_OFF+i] = (FAT16_BS_TCKPS_VAL>>(8*i)) & 0xFF;
   for(i=0;i<FAT16_BS_NBH_SIZE;i++)  buf[FAT16_BS_NBH_OFF+i] = (FAT16_BS_NBH_VAL>>(8*i)) & 0xFF;
   for(i=0;i<FAT16_BS_NBSHID_SIZE;i++)   buf[FAT16_BS_NBSHID_OFF+i] = (FAT16_BS_NBSHID_VAL>>(8*i)) & 0xFF;

   //determine number of sector
   if(nbblocks<0x10000) {
      for(i=0;i<FAT16_BS_NBSEC16_SIZE;i++)  buf[FAT16_BS_NBSEC16_OFF+i] = (nbblocks>>(8*i)) & 0xFF;
   }
   else {
      for(i=0;i<FAT16_BS_NBSEC32_SIZE;i++)  buf[FAT16_BS_NBSEC32_OFF+i] = (nbblocks>>(8*i)) & 0xFF;
   }

   buf[FAT16_BS_DRVNUM_OFF] = FAT16_BS_DRVNUM_VAL;
   buf[FAT16_BS_RSV1_OFF] = FAT16_BS_RSV1_VAL;
   buf[FAT16_BS_BOOTSIG_OFF] = FAT16_BS_BOOTSIG_VAL;

   //maybe time
   //for(i=0;i<BS_VOLID_S;i++)  Buffer[BS_VOLID_I+i] = volid[i];
   for(i=0;i<FAT16_BS_VOLID_SIZE;i++)  buf[FAT16_BS_VOLID_OFF+i] = '0';
   for(i=0;i<FAT16_BS_VOLLAB_SIZE;i++) buf[FAT16_BS_VOLLAB_OFF+i] = FAT16_BS_VOLLAB_VAL[i];
   for(i=0;i<FAT16_BS_FSTYPE_SIZE;i++)  buf[FAT16_BS_FSTYPE_OFF+i] = FAT16_BS_FSTYPE_VAL[i];
   if(_fat_write_data(dev_desc,FAT16_BS_ADDR,buf,FAT16_BS_SIZE)<0) {
      return -1;
   }

   //update FATs
   buf[0] = 0xF8;
   buf[1] = 0xFF;
   buf[2] = 0xFF;
   buf[3] = 0xFF;

   if(_fat_write_data(dev_desc,fat16_core_info.fat_addr,buf,4)<0) {
      return -1;
   }
   if(_fat_write_data(dev_desc,fat16_core_info.fat_addr+fat16_core_info.fat_size,buf,4)<0) {
      return -1;
   }

   //update root directory and create volume entry
   for(i=0;i<(RD_ENTNAMR_SIZE+RD_ENTEXT_SIZE);i++) buf[RD_ENTNAME_OFF+i]=0x20; // Nom du volume
   i=0;
   while ((volume_name[i] != '\0') && (i<(RD_ENTNAMR_SIZE+RD_ENTEXT_SIZE))) {
      buf[RD_ENTNAME_OFF+i] = volume_name[i];
      i++;
   }
   buf[RD_ATTR_OFF] = RD_ATTR_VOLUMEID;
   if(_fat_write_data(dev_desc,fat16_core_info.rd_addr,buf,(RD_ENTNAMR_SIZE+RD_ENTEXT_SIZE+RD_ATTR_SIZE))<0) {
      return -1;
   }

   //fill fat16_core_info
   if(_fat_read_data(dev_desc,0,(unsigned char *)&fat16_boot_core_info,sizeof(fat16_boot_core_info))<0) {
      return -1;
   }

   //clean rootdir (skip first entry)
   memset(buf,0,RD_SIZE);

   for(i=1;i<(fat16_boot_core_info.bpb.BPB_RootEntCnt-1);i++) {
      if(_fat_write_data(dev_desc,fat16_core_info.rd_addr+RD_SIZE*i,buf,RD_SIZE)<0) {
         return -1;
      }
   }

   return 0;
}

/*-------------------------------------------
| Name:_fat_readfs
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _fat_readfs(mntdev_t* pmntdev) {
   fat16_boot_core_info_t fat16_info={0};
   unsigned int nb_clus=0;

   //get essential fs informations
   pmntdev->fs_info.fat_info.fat_boot_info = &fat16_boot_core_info;
   pmntdev->fs_info.fat_info.fat_core_info = &fat16_core_info;

   //get boot sector information from device
   if((nb_clus=_fat16_init(pmntdev->fs_info.fat_info.fat_core_info,pmntdev->dev_desc))<0) {
      return -1;
   }

   //fill our bootsector struct
   if(_fat_read_data(pmntdev->dev_desc,0,(unsigned char *)pmntdev->fs_info.fat_info.fat_boot_info,sizeof(fat16_boot_core_info_t))<0) {
      return -1;
   }

   //511 empty in rootdir + nbcluster*cluster_size/entry_size
   pmntdev->inodetbl_size = (pmntdev->fs_info.fat_info.fat_boot_info->bpb.BPB_RootEntCnt-1)
         +(nb_clus*pmntdev->fs_info.fat_info.fat_boot_info->bpb.BPB_SecPerClust*pmntdev->fs_info.fat_info.fat_boot_info->bpb.BPB_BytesPerSec)/RD_SIZE;
   return 0;
}

/*-------------------------------------------
| Name:_fat_mountdir
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _fat_mountdir(desc_t desc,inodenb_t original_root_node,inodenb_t target_root_node) {
   return 0;
}


/*-------------------------------------------
| Name:_fat_telldir
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _fat_telldir(desc_t desc) {
   return (ofile_lst[desc].offset/RD_SIZE);
}

/*-------------------------------------------
| Name:_fat_seekdir
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _fat_seekdir(desc_t desc,int loc) {
   fat16_u32_t current_addr=0;
   int old_offset = ofile_lst[desc].offset;
   fat16_u8_t buf=0;

   //seek in rootdir
   if(!fat16_ofile_lst[desc].entry_data_cluster) {
      if(loc<__get_rd_size(ofile_lst[desc].pmntdev)) {
         //skip volume name
         if(_fat_read_data(__get_dev_desc(desc),__get_rd_addr(ofile_lst[desc].pmntdev)+RD_SIZE+loc*RD_SIZE,&buf,1)<0) {
            return -1;
         }
         if(buf==RD_ENT_FREE) {
            return -1;
         }
         ofile_lst[desc].offset=loc*RD_SIZE;
         return 0;
      }
      return -1;
   }

   //or in UserData
   ofile_lst[desc].offset = loc*RD_SIZE;
   current_addr = _fat16_adddir(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info,desc);
   if(current_addr) {
      return 0;
   }

   ofile_lst[desc].offset = old_offset;
   return -1;
}


/*-------------------------------------------
| Name:_fat_mknod
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _fat_mknod(desc_t desc,inodenb_t inodenb,dev_t dev) {
   return 0;
}


/*-------------------------------------------
| Name:_fat_seek
| Description:
| Parameters:
| Return Type:
| Comments: fill gap between old offset and new offset by 0
| See:
---------------------------------------------*/
int _fat_seek(desc_t desc, int offset, int origin) {
   int result = 0;
   int new_offset=0;

   switch(origin) {
      case SEEK_SET:
         new_offset=offset;
      break;

      case SEEK_CUR:
         new_offset=ofile_lst[desc].offset+offset;
      break;

      case SEEK_END:
         new_offset = ofile_lst[desc].size+offset;
      break;

   default:
      return -1;
   }

   //add zero to end of file
   if(new_offset > ofile_lst[desc].size) {
      unsigned char clean_buf[FAT_16_CLEAN_BUFFER_SIZE]={0};
      int clean_buf_size=0;

      ofile_lst[desc].offset = ofile_lst[desc].size;
      while (new_offset > ofile_lst[desc].offset) {
         if ((new_offset-ofile_lst[desc].offset)<FAT_16_CLEAN_BUFFER_SIZE) {
            clean_buf_size = new_offset-ofile_lst[desc].offset;
         }
         else {
            clean_buf_size = FAT_16_CLEAN_BUFFER_SIZE;
         }
         //
         result = ofile_lst[desc].pfsop->fs.write(desc,clean_buf,clean_buf_size);
         //
         if (result != clean_buf_size) {
            return -1;
         }
         ofile_lst[desc].offset = ofile_lst[desc].offset + clean_buf_size;
      }
   }
   else {
      //set new offset
      ofile_lst[desc].offset = new_offset;
   }
   //
   return ofile_lst[desc].offset;
}


/*-------------------------------------------
| Name:_fat_checkfs
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _fat_checkfs(mntdev_t* pmntdev) {
   //compare the two Fats
   return 0;
}

/*-------------------------------------------
| Name:_fat_writefs
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _fat_writefs(mntdev_t* pmntdev) {
   return 0;
}

/*-------------------------------------------
| Name:_fat_vfat_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _fat_close(desc_t desc) {
   unsigned char buffer[RD_LSTDATEACC_SIZE]={0};

   //set last access date
   struct __timeval tv={0};//common

   if(IS_FSTATUS_MODIFIED(ofile_lst[desc].status)){
      //modify time of last write access => DIR_LstAccDate
      _fat16_getdatetime(buffer,NULL);

      if(_fat_write_data(__get_dev_desc(desc),fat16_ofile_lst[desc].entry_infos_addr+RD_LSTDATEACC_OFF,buffer,RD_LSTDATEACC_SIZE)<0) {
         return -1;
      }
   }

   return 0;
}

/*-------------------------------------------
| Name:_fat_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _fat_read(desc_t desc,char* buffer,int size) {
   fat16_u32_t Add;
   fat16_u16_t clus, offset;
   int readsize;
   //
   struct __timeval tv={0};
   unsigned char buf[RD_LSTDATEACC_SIZE]={0};

   //nothing to read
   if(ofile_lst[desc].size == 0)
      return 0;

   //how much datas are available
   if((ofile_lst[desc].offset+size)>ofile_lst[desc].size)
      size = ofile_lst[desc].size-ofile_lst[desc].offset;

   //get current offset in cluster
   if(_fat16_offsetinfo(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info,desc,&clus,&offset)==-1)
      return -1;

   //get addr of current offset
   Add = _fat16_cluster_add(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info, clus)+offset;

   if(size>((__get_nbsec_per_clus(ofile_lst[desc].pmntdev)*FAT16_BS_BPS_VAL)-offset))
      readsize = (__get_nbsec_per_clus(ofile_lst[desc].pmntdev)*FAT16_BS_BPS_VAL)-offset;
   else
      readsize = size;

   if(_fat_read_data(__get_dev_desc(desc),Add,buffer,readsize)<0) {
      return -1;
   }

   //read data in other clusters
   while (readsize != size) {
      clus = _fat16_cluster_suiv(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info, __get_dev_desc(desc), clus);          // On calcule l'adresse de l'offset courant
      Add = _fat16_cluster_add(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info, clus);

      //error
      if (Add == 0)
         return readsize;

      if ((size-readsize)>(__get_nbsec_per_clus(ofile_lst[desc].pmntdev)*FAT16_BS_BPS_VAL)) {
         if(_fat_read_data(__get_dev_desc(desc),Add,buffer+readsize,(__get_nbsec_per_clus(ofile_lst[desc].pmntdev)*FAT16_BS_BPS_VAL))<0) {
            break;//return readsize;
         }

         readsize = readsize + (__get_nbsec_per_clus(ofile_lst[desc].pmntdev)*FAT16_BS_BPS_VAL); // Lecture
      }
      else {
         if(_fat_read_data(__get_dev_desc(desc),Add,buffer+readsize,size-readsize)<0) {
            break;//return readsize;
         }

         readsize = size;
      }
   }

   //set last access date
   //modify time of last write access => DIR_LstAccDate
   _fat16_getdatetime(&buf[RD_LSTDATEACC_OFF],NULL);

   if(_fat_write_data(__get_dev_desc(desc),fat16_ofile_lst[desc].entry_infos_addr+RD_LSTDATEACC_OFF,buf,RD_LSTDATEACC_SIZE)<0) {
      return -1;
   }

   //increment offset
   ofile_lst[desc].offset+=readsize;
   return readsize;
}

/*-------------------------------------------
| Name:_fat_vfat_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _fat_write(desc_t desc,char* buffer,int size) {
   fat16_u8_t Buffer[RD_FILESIZE_SIZE];
   fat16_u16_t curclus,curoffset;
   fat16_u32_t curadd, tempadd;
   int writtensize=0;

   //
   struct __timeval tv={0};//common
   fat_msdos_dir_entry_t entry={0};
   char *ptr=NULL;

   //allocate cluster if needed
   if (fat16_ofile_lst[desc].entry_data_cluster == RD_CLUSEMPTY) {
      fat16_ofile_lst[desc].entry_data_cluster = _fat16_getclus(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info, __get_dev_desc(desc), &ofile_lst[desc].pmntdev->fs_info.fat_info.fat_boot_info->bpb);
      //error
      if (fat16_ofile_lst[desc].entry_data_cluster == 0)
         return -1;
      Buffer[0] = (fat16_u8_t) fat16_ofile_lst[desc].entry_data_cluster;
      Buffer[1] = (fat16_u8_t) (fat16_ofile_lst[desc].entry_data_cluster>>8);

      if(_fat_write_data(__get_dev_desc(desc),fat16_ofile_lst[desc].entry_infos_addr+RD_LWCLUSNO_OFF,Buffer,RD_LWCLUSNO_SIZE)<0) {
         return -1;
      }
   }

   //get current offset in cluster
   if (_fat16_offsetinfo(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info,desc,&curclus,&curoffset)==-1)
      return -1;

   //get addr of current offset
   curadd = _fat16_cluster_add(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info,curclus)+curoffset;

   if (!(curadd > (__get_nbsec_per_clus(ofile_lst[desc].pmntdev)*FAT16_BS_BPS_VAL)))
      return -1;

   //write data in file
   while (size != writtensize) {
      //last cluster have enough space for datas
      if ((curoffset + size) < (__get_nbsec_per_clus(ofile_lst[desc].pmntdev)*FAT16_BS_BPS_VAL)) {
         if(_fat_write_data(__get_dev_desc(desc),curadd,buffer+writtensize,size-writtensize)<0) {
            break;//return writtensize;
         }

         writtensize = size;
      }
      //or not
      else {
         if(_fat_write_data(__get_dev_desc(desc),curadd,buffer+writtensize,(__get_nbsec_per_clus(ofile_lst[desc].pmntdev)*FAT16_BS_BPS_VAL)-curoffset)<0) {
            break;//return writtensize;
         }

         writtensize = writtensize + (__get_nbsec_per_clus(ofile_lst[desc].pmntdev)*FAT16_BS_BPS_VAL)-curoffset;
         curoffset = 0;
         tempadd = __get_fat_addr(ofile_lst[desc].pmntdev) + (FAT16_CLUSSZ*curclus);
         curclus = _fat16_cluster_suiv(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info, __get_dev_desc(desc), curclus);
         //if (curclus == 0) {
         //cluster empty or final cluster
         if(curclus == 0 || curclus==FAT16_LCLUSMAX) {
            ///!TODO add more clean code :)
            curclus = _fat16_getclus(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info, __get_dev_desc(desc), &ofile_lst[desc].pmntdev->fs_info.fat_info.fat_boot_info->bpb);
            if(curclus == 0)
               return -1;
            Buffer[0] = (fat16_u8_t)curclus;
            Buffer[1] = (fat16_u8_t)(curclus>>8);

            if(_fat_write_data(__get_dev_desc(desc),tempadd,Buffer,FAT16_CLUSSZ)<0) {
               return -1;
            }
            if(_fat_write_data(__get_dev_desc(desc),tempadd+__get_fat_size(ofile_lst[desc].pmntdev),Buffer,FAT16_CLUSSZ)<0) {
               return -1;
            }

            //error
            curadd = _fat16_cluster_add(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info, curclus);
            if (curadd == 0) {
               return -1;
            }
         }
         //
         curadd = _fat16_cluster_add(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info, curclus);
         if (curadd == 0) {
            return(writtensize);
         }
      }
   }

   //update size
   if (!(ofile_lst[desc].size > (ofile_lst[desc].offset + writtensize))) {
      ofile_lst[desc].size = ofile_lst[desc].offset + writtensize;
      Buffer[0] = (fat16_u8_t) ofile_lst[desc].size;
      Buffer[1] = (fat16_u8_t) (ofile_lst[desc].size>>8);
      Buffer[2] = (fat16_u8_t) (ofile_lst[desc].size>>16);
      Buffer[3] = (fat16_u8_t) (ofile_lst[desc].size>>24);

      if(_fat_write_data(__get_dev_desc(desc),fat16_ofile_lst[desc].entry_infos_addr+RD_FILESIZE_OFF,Buffer,RD_FILESIZE_SIZE)<0) {
         return -1;
      }
   }

   //modify time of last write access, last write date + last write time
   _fat16_getdatetime(entry.DIR_LstAccDate,NULL);
   _fat16_getdatetime(entry.DIR_WrtDate,entry.DIR_WrtTime);

   //hi cluster addr => DIR_FstClusHI (always 0 for FAT16)
   ptr = (char *)&entry;
   ptr += RD_LSTDATEACC_OFF;

   if(_fat_write_data(__get_dev_desc(desc),fat16_ofile_lst[desc].entry_infos_addr+RD_LSTDATEACC_OFF,ptr,RD_LSTDATEACC_SIZE+RD_HICLUSNO_SIZE+RD_LSTTIMEWRT_SIZE+RD_LSTDATEWRT_SIZE)<0) {
      return -1;
   }

   //set modified status and update offset
   ofile_lst[desc].status|=MSK_FSTATUS_MODIFIED;
   ofile_lst[desc].offset+=writtensize;

   return writtensize;
}

/*-------------------------------------------
| Name:_fat_truncate
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _fat_truncate(desc_t desc, off_t length) {
   fat16_u32_t nbclus;
   fat16_u16_t clus1,clus2;
   fat16_u8_t Buf[RD_FILESIZE_SIZE];
   int i=0;

   //no need to truncate
   if(ofile_lst[desc].size == length) {
      return -1;
   }

   //new size is 0
   if (!length) {
      clus2 = fat16_ofile_lst[desc].entry_data_cluster;
      //we free all clusters file
      do {
         clus1 = clus2;
         clus2 = _fat16_cluster_suiv(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info, __get_dev_desc(desc), clus1);
         _fat16_putclus(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info, __get_dev_desc(desc), clus1);
      } while(clus2 < FAT16_LCLUSMAX && clus2 > FAT16_CLUSMIN);//while(clus2 != FAT16_LCLUSMAX);

      //update desc
      fat16_ofile_lst[desc].entry_data_cluster = RD_CLUSEMPTY;

      //remove cluster number
      for(i=0;i<RD_LWCLUSNO_SIZE;i++)
         Buf[i] = RD_CLUSEMPTY>>(8*i);

      if(_fat_write_data(__get_dev_desc(desc),fat16_ofile_lst[desc].entry_infos_addr+RD_LWCLUSNO_OFF,Buf,RD_LWCLUSNO_SIZE)<0) {
         return -1;
      }
   }
   //new size not 0
   else {
      //decrease
      if(ofile_lst[desc].size > length) {
         //calculate new cluster number
         nbclus = length/(__get_nbsec_per_clus(ofile_lst[desc].pmntdev) * FAT16_BS_BPS_VAL);
         if((length%(__get_nbsec_per_clus(ofile_lst[desc].pmntdev) * FAT16_BS_BPS_VAL)) != 0) nbclus++;
         clus1 = fat16_ofile_lst[desc].entry_data_cluster;

         //find new last cluster and set it
         for(i=1;i<nbclus;i++)
            clus1 = _fat16_cluster_suiv(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info, __get_dev_desc(desc), clus1);

         clus2 = _fat16_cluster_suiv(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info, __get_dev_desc(desc), clus1);
         _fat16_lastclus(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info, __get_dev_desc(desc), clus1);

         //free all previous cluster
         while(clus2 < FAT16_LCLUSMAX && clus2 > FAT16_CLUSMIN) {
            clus1 = clus2;
            clus2 = _fat16_cluster_suiv(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info, __get_dev_desc(desc), clus1);
            _fat16_putclus(ofile_lst[desc].pmntdev->fs_info.fat_info.fat_core_info, __get_dev_desc(desc), clus1);
         }
      }
      //or increase
      else {
         //save previous offset
         int save_offset = ofile_lst[desc].offset;

         //and SEEK_END with difference between ofile_lst[desc].size and length
         ofile_lst[desc].pfsop->fs.seek(desc,length-ofile_lst[desc].size,SEEK_END);

         //restore old offset
         ofile_lst[desc].offset = save_offset;
      }
   }
   //update open file table
   ofile_lst[desc].size = length;

   //update file size
   for(i=0;i<RD_FILESIZE_SIZE;i++)
      Buf[i] = (fat16_u8_t)(length>>(8*i));

   if(_fat_write_data(__get_dev_desc(desc),fat16_ofile_lst[desc].entry_infos_addr+RD_FILESIZE_OFF,Buf,RD_FILESIZE_SIZE)<0) {
      return -1;
   }

   //update write date and time of file
   _fat16_getdatetime(&Buf[2], &Buf[0]);
   if(_fat_write_data(__get_dev_desc(desc),fat16_ofile_lst[desc].entry_infos_addr+RD_LSTTIMEWRT_OFF,Buf,RD_LSTTIMEWRT_SIZE+RD_LSTDATEWRT_SIZE)<0) {
      return -1;
   }

   ofile_lst[desc].status|=MSK_FSTATUS_MODIFIED;

   return 0;
}

/*============================================
| End of Source  : fat16.c
|---------------------------------------------
| Historic:
|---------------------------------------------
| Authors     | Date     | Comments
| $Log:$
==============================================*/
