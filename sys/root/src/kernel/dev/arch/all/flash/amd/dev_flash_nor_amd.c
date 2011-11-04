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
#include "kernel/core/kernel.h"
#include "kernel/core/system.h"
#include "kernel/core/stat.h"
#include "kernel/core/ioctl_hd.h"
#include "kernel/fs/vfs/vfsdev.h"

#include "kernel/dev/arch/all/flash/amd/lldapi/lld.h"

#include "dev_flash_nor_amd.h"

#define USE_UNLOCK_BYPASS

/*============================================
| Global Declaration 
==============================================*/

const char dev_flash_nor_amd_name[]="hde\0flash_nor_amd\0";

dev_map_t dev_flash_nor_amd_map={
   dev_flash_nor_amd_name,
   S_IFBLK,
   dev_flash_nor_amd_load,
   dev_flash_nor_amd_open,
   dev_flash_nor_amd_close,
   __fdev_not_implemented,
   __fdev_not_implemented,
   dev_flash_nor_amd_read,
   dev_flash_nor_amd_write,
   dev_flash_nor_amd_seek,
   dev_flash_nor_amd_ioctl //ioctl
};

/*============================================
| Implementation 
==============================================*/

/*--------------------------------------------
| Name:        init_flash_nor_amd
| Description: Procedure d'Initialisation de la mémoire FLASH
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int init_flash_nor_amd(void){
 return (0);
}

/*--------------------------------------------
| Name:        read_flash_nor_amd_words
| Description:Lecture dans la memoire flash par mot de 16 bits 
| Parameters:
|           adresse -> adresse de la premiere
|                      case memoire a lire dans la mémoire
|           *donnee -> pointeur sur un buffer 
|                      de récupération des données lues
| 	         nb_mots = nombre de mots a lire	   
| Return Type: none
| Comments: 
| See:         
----------------------------------------------*/
int read_flash_nor_amd_words(desc_t desc, unsigned short * buf, unsigned int cb){
  unsigned int i;
  unsigned short* flash_base_pt=((dev_flash_t*)ofile_lst[desc].p)->flash_base_pt;
  unsigned int offset =ofile_lst[desc].offset;

   //while(FLASH_READY == 0);

   *(flash_base_pt + 0x555) = 0xAA;
   *(flash_base_pt + 0x2AA) = 0x55;	            // passage en mode lecture
   *(flash_base_pt + 0x555) = 0xF0;

   //
   for (i=0; i<cb ; i++){
      //FLASH_PAGE = (char)((adresse >> 15)&0xFF);     // Adresse haute
      *buf = *(flash_base_pt + cb);	    // lecture des données
      buf++;
   }

 return cb;
} 


/*--------------------------------------------
| Name:        write_flash_nor_amd_word
| Description:Ecriture dans la memoire flash par mot de 16 bits 
| Parameters:
|           adresse -> adresse de la premiere
|                      case memoire a lire dans la mémoire
|           *donnee -> pointeur sur un buffer 
|                      de récupération des données lues
| 	         nb_mots = nombre de mots a lire	   
| Return Type: none
| Comments: 
| See:         
----------------------------------------------*/
int write_flash_nor_amd_word(desc_t desc, const unsigned short * buf, unsigned int cb){
  unsigned int i;
  unsigned short* flash_base_pt=((dev_flash_t*)ofile_lst[desc].p)->flash_base_pt;
  unsigned int offset =ofile_lst[desc].offset;

  for (i=0; i<cb ; i++){
     // Attente flash
      //while (FLASH_READY == 0);
      *(flash_base_pt + 0x555) = 0xAA;
      *(flash_base_pt + 0x2AA) = 0x55;  // passage mode ecriture
      *(flash_base_pt + 0x555) = 0xA0;
      *(flash_base_pt + offset) = *buf;	// Ecriture en memoire
      buf++;
      offset++;
   }

   ofile_lst[desc].offset+=cb;
   return cb;
}

/*--------------------------------------------
| Name:        erase_flash_nor_amd_sector
| Description:Effacement d'un bloc de la memoire flash
| Parameters: 
|           ind_bloc = numero du bloc a effacer (de 0 à 255)
| Return Type: none
| Comments:      
| See:         
----------------------------------------------*/
char erase_flash_nor_amd_sector(desc_t desc, unsigned int sector_nb){
   unsigned short * pt_sector;
   unsigned short* flash_base_pt=((dev_flash_t*)ofile_lst[desc].p)->flash_base_pt;


   // Attente fin effacement
   //while (FLASH_READY == 0);
 
	pt_sector = flash_base_pt + (((unsigned int)sector_nb)<<12);
	*(flash_base_pt + 0x555) = 0xAA;
	*(flash_base_pt + 0x2AA) = 0x55;
	*(flash_base_pt + 0x555) = 0x80;
	*(flash_base_pt + 0x555) = 0xAA;
	*(flash_base_pt + 0x2AA) = 0x55;
	*pt_sector = 0x30;
   return 0; 
}

/*--------------------------------------------
| Name:        erase_flash_nor_amd_all
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
char erase_flash_nor_amd_all(desc_t desc){
   unsigned int k;
	
   for (k=0;k<=255;k++){
      //while (FLASH_READY == 0); 	// Attente fin effacement
      erase_flash_nor_amd_sector(desc,k);		// Effacement du bloc k
   }
 return(0);
}


/*--------------------------------------------
| Name:        get_flash_nor_amd_sectorsz
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
unsigned long get_flash_nor_amd_sectorsz(desc_t desc, unsigned long addr){
   //read only mode
   int i=0;

   if(ofile_lst[desc].p){
      //
      flash_sector_t* p_sector_map=(flash_sector_t*)((dev_flash_t*)ofile_lst[desc].p)->p_flash_type->p_sector_map;

      //
      while(p_sector_map[i].addr!=0xFFFFFFFF){
         if( (addr > p_sector_map[i].addr)
            && (addr < p_sector_map[i+1].addr) )
            return p_sector_map[i].size;
         i++;
      }
   }

   return 0;
}


/*-------------------------------------------
| Name:dev_flash_nor_amd_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_flash_nor_amd_load(void){
      return 0;
}

/*-------------------------------------------
| Name:dev_flash_nor_amd_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_flash_nor_amd_open(desc_t desc, int o_flag){

   //
   if(o_flag & O_RDONLY){
   }

   if(o_flag & O_WRONLY){
   }

   ofile_lst[desc].offset=0;

   return 0;
}

/*-------------------------------------------
| Name:dev_flash_nor_amd_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_flash_nor_amd_close(desc_t desc){

   if(!ofile_lst[desc].nb_reader && !ofile_lst[desc].nb_writer){
      ofile_lst[desc].p=(void*)0;

   }

   return 0;
}

/*-------------------------------------------
| Name:dev_flash_nor_amd_isset_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_flash_nor_amd_isset_read(desc_t desc){
  return -1;
}

/*-------------------------------------------
| Name:dev_flash_nor_amd_isset_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_flash_nor_amd_isset_write(desc_t desc){
      return -1;
}
/*-------------------------------------------
| Name:dev_flash_nor_amd_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_flash_nor_amd_read(desc_t desc, char* buf,int size){
   unsigned int pos = ofile_lst[desc].offset;
   unsigned char buf_align[sizeof(FLASHDATA)];
   int cb=-1;
   
   if(ofile_lst[desc].p){
      FLASHDATA* flash_base_pt=((dev_flash_t*)ofile_lst[desc].p)->flash_base_pt;

      if((pos+size)>(((dev_flash_t*)ofile_lst[desc].p)->p_flash_type->size*((dev_flash_t*)ofile_lst[desc].p)->p_flash_type->width))
         if((size=((dev_flash_t*)ofile_lst[desc].p)->p_flash_type->size-pos)<0)
            return -1; 

      cb=0;
      //alignment algorithm for data bus acces size on flash
      //alignment on 16 bits for offset
      //to do: the same things for 32 bits alignment
      if(ofile_lst[desc].offset&0x00000001){
         pos = ofile_lst[desc].offset/sizeof(FLASHDATA);
         memcpy(buf_align,flash_base_pt+pos,sizeof(buf_align));

         //align 16bits
         *buf=buf_align[1];
         //
         ofile_lst[desc].offset++;
         buf++;
         size--;
         cb++;
      }

      //
      pos = ofile_lst[desc].offset/sizeof(FLASHDATA);
      memcpy(buf,flash_base_pt+pos,(size&(~0x00000001)));

      cb+=(size&(~0x00000001));
      buf+=(size&(~0x00000001));
      ofile_lst[desc].offset+=(size&(~0x00000001));

      //alignment algorithm for data bus acces size on flash
      //alignment on 16 bits for size 
      //to do: the same things for 32 bits alignment
      if(size&0x00000001){

         pos = ofile_lst[desc].offset/sizeof(FLASHDATA);
         memcpy(buf_align,flash_base_pt+pos,sizeof(buf_align));

         //align 16bits
         *buf=buf_align[0];
         //
         ofile_lst[desc].offset++;
         cb++;
      }
   }
   return cb;
}

/*-------------------------------------------
| Name:dev_flash_nor_amd_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_flash_nor_amd_write(desc_t desc, const char* buf,int size){
   unsigned int pos = ofile_lst[desc].offset;
   int offset = ofile_lst[desc].offset;

   int cb = -1;

   if(ofile_lst[desc].p){
      //
      DEVSTATUS status;
      //
      FLASHDATA* flash_base_pt=((dev_flash_t*)ofile_lst[desc].p)->flash_base_pt;

      if((pos+size)>(((dev_flash_t*)ofile_lst[desc].p)->p_flash_type->size*((dev_flash_t*)ofile_lst[desc].p)->p_flash_type->width))
         if((size=((dev_flash_t*)ofile_lst[desc].p)->p_flash_type->size-pos)<0)
            return -1; 
      
      #ifdef USE_UNLOCK_BYPASS
      //1)
      lld_UnlockBypassEntryCmd(flash_base_pt);
      #endif

      //alignment algorithm for data bus acces size on flash
      //alignment on 16 bits for offset
      //to do: the same things for 32 bits alignment
      cb=0;
      //
      pos = ofile_lst[desc].offset/sizeof(FLASHDATA);
      //
      while(cb<size){
         //
         FLASHDATA read_data = 0;
         FLASHDATA write_data = 0;
         //
         if((offset&0x00000001) && !cb){
            //begin but not aligned on 16 bits
            //read data
            memcpy(&write_data,flash_base_pt+pos,sizeof(FLASHDATA));
            ((char*)&write_data)[1]=buf[0];
            //
            buf++;
            offset++;
            cb++;
         }else if((int)(cb+sizeof(FLASHDATA))>size){
            //end but not aligned on 16 bits
            //read data
            memcpy(&write_data,flash_base_pt+pos,sizeof(FLASHDATA));
            ((char*)&write_data)[0]=*(buf);
            //
            offset++;
            cb++;
         }else{
            //write_data=*((FLASHDATA*)buf);
            memcpy(&write_data,buf,sizeof(FLASHDATA));
            //
            buf+=sizeof(FLASHDATA);
            offset+=sizeof(FLASHDATA);
            cb+=sizeof(FLASHDATA);
         }
         //write data
         #ifdef USE_UNLOCK_BYPASS
            //2)
            lld_UnlockBypassProgramCmd(flash_base_pt,pos,&write_data);
            //to remove: debug test
            status = lld_Poll(flash_base_pt, pos, &write_data, &read_data, LLD_P_POLL_PGM);
            //
            if(cb==size)
               status = lld_Poll(flash_base_pt, pos, &write_data, &read_data, LLD_P_POLL_PGM);
            else
               status = DEV_NOT_BUSY;
         #else
            status = lld_ProgramOp(flash_base_pt,pos,&write_data);
         #endif

         //check status
         if(status!=DEV_NOT_BUSY){
            #ifdef USE_UNLOCK_BYPASS
            //3.a)
            lld_UnlockBypassResetCmd(flash_base_pt);
            #endif

            return -1;
         }

         pos++;
      }


      #ifdef USE_UNLOCK_BYPASS
      //3.b)
      lld_UnlockBypassResetCmd(flash_base_pt);
      #endif
   }

   ofile_lst[desc].offset=offset;

   return cb;
}

/*-------------------------------------------
| Name:dev_flash_nor_amd_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_flash_nor_amd_seek(desc_t desc,int offset,int origin){

   switch(origin){

      case SEEK_SET:
         ofile_lst[desc].offset=offset;
      break;

      case SEEK_CUR:
         ofile_lst[desc].offset+=offset;
      break;

      case SEEK_END:
         //to do: warning in SEEK_END (+ or -)????
         ofile_lst[desc].offset=((dev_flash_t*)ofile_lst[desc].p)->p_flash_type->size
                   *((dev_flash_t*)ofile_lst[desc].p)->p_flash_type->width;
         ofile_lst[desc].offset+=offset;
         //to do: test check device size
      break;
   }

   if(ofile_lst[desc].offset>=(off_t)(((dev_flash_t*)ofile_lst[desc].p)->p_flash_type->size
                   *((dev_flash_t*)ofile_lst[desc].p)->p_flash_type->width))
      ofile_lst[desc].offset=((dev_flash_t*)ofile_lst[desc].p)->p_flash_type->size
                   *((dev_flash_t*)ofile_lst[desc].p)->p_flash_type->width;


   return ofile_lst[desc].offset;
}

/*-------------------------------------------
| Name:dev_flash_nor_amd_ioctl
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_flash_nor_amd_ioctl(desc_t desc,int request,va_list ap){
   switch(request){

      //
      case HDGETSZ:{
         unsigned long* hdsz_p= va_arg( ap, unsigned long*);

         if(!ofile_lst[desc].p)
            return -1;

         if(!hdsz_p)
            return -1;
         *hdsz_p = ((dev_flash_t*)ofile_lst[desc].p)->p_flash_type->size
                   *((dev_flash_t*)ofile_lst[desc].p)->p_flash_type->width;
      }
      break;

      //
      case HDSETSZ:{
        return -1;
      }
      break;

      //
      case HDCHK:{
      }
      break;

      //
      case HDGETSCTRSZ:{
         unsigned long sector_addr= va_arg( ap, unsigned long);
         unsigned long* sector_sz= va_arg( ap, unsigned long*);
          if(!sector_sz)
            return -1;

          *sector_sz = get_flash_nor_amd_sectorsz(desc,sector_addr);
      }
      break;

      //
      case HDCLRSCTR:{
         unsigned long sector_addr= va_arg( ap, unsigned long);
         unsigned short* flash_base_pt=((dev_flash_t*)ofile_lst[desc].p)->flash_base_pt;
         DEVSTATUS status;
         status = lld_SectorEraseOp((FLASHDATA*)flash_base_pt,(ADDRESS)(sector_addr/sizeof(FLASHDATA)));
         if(status!=DEV_NOT_BUSY)
            return -1;
      }
      break;

      //
      case HDCLRDSK:{
         unsigned short* flash_base_pt=((dev_flash_t*)ofile_lst[desc].p)->flash_base_pt;
         DEVSTATUS status;
         status = lld_ChipEraseOp(flash_base_pt);
         if(status!=DEV_NOT_BUSY)
            return -1;
      }
      break;

      //
      case HDIO:{
         hdio_t* hdio= va_arg( ap, hdio_t*);
         hdio->addr = (hdio_addr_t)((dev_flash_t*)ofile_lst[desc].p)->flash_base_pt;
      }
      break;

      //
      default:
         return -1;

   }

   return 0;
}

/*============================================
| End of Source  : dev_m16c_flash_flash_nor_amd.c
==============================================*/
