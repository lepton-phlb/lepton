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
#include "kernel/core/system.h"
#include "kernel/core/ioctl_hd.h"

#include "kernel/fs/vfs/vfsdev.h"
#include "kernel/fs/vfs/vfstypes.h"

#include "kernel/dev/arch/gnu32/common/dev_linux_stub.h"

#include "kernel/dev/arch/all/flash/flash.h"

#include <string.h>
#include <stdlib.h>

/*============================================
| Global Declaration
==============================================*/

static const char dev_linux_flash_name[]= "hdc\0linux_flash\0";

static int dev_linux_flash_load(void);
static int dev_linux_flash_open(desc_t desc, int o_flag);
static int dev_linux_flash_close(desc_t desc);
static int dev_linux_flash_isset_read(desc_t desc);
static int dev_linux_flash_isset_write(desc_t desc);
static int dev_linux_flash_read(desc_t desc, char* buf,int size);
static int dev_linux_flash_write(desc_t desc, const char* buf,int size);
static int dev_linux_flash_seek(desc_t desc,int offset,int origin);
static int dev_linux_flash_ioctl(desc_t desc,int request,va_list ap);

dev_map_t dev_linux_flash_map={
   dev_linux_flash_name,
   S_IFBLK,
   dev_linux_flash_load,
   dev_linux_flash_open,
   dev_linux_flash_close,
   __fdev_not_implemented,
   __fdev_not_implemented,
   dev_linux_flash_read,
   dev_linux_flash_write,
   dev_linux_flash_seek,
   dev_linux_flash_ioctl //ioctl
};


//
#define DFLT_FILEFLASH_MEMORYSIZE  32*1024*1024//(32Mio) //2048*1024 //(2Mio)
static char* pmemory;
static long memory_size = DFLT_FILEFLASH_MEMORYSIZE;
static int fh=-1;

//static const char memory[DFLT_FILEFLASH_MEMORYSIZE]={0};
static int instance_counter=0;

static const dev_flash_t dev_flash_am29dlxxxx_linux={
   (unsigned short *)0x10000000L,// chip base address
   (flash_type_t*)&flash_type_lst[FLASH_S29Gl256P]
};

//
typedef  uint32_t addr_t;
typedef  uchar8_t sector_data_t;
typedef  uint32_t sector_addr_t;
typedef  int16_t  sector_no_t;
typedef  uint32_t sector_sz_t;
typedef  uint32_t stat_t;



//
typedef struct linux_flash_sector_map_st{
   sector_no_t      sector_no;
   sector_addr_t    sector_addr;
   sector_sz_t      sector_sz;
   struct linux_flash_sector_map_st* prev;
   struct linux_flash_sector_map_st* next;
}linux_flash_sector_map_t;

sector_sz_t      g_sector_sz_max = 0;

sector_data_t * p_internal_sector_buffer= (sector_data_t *)0;

static linux_flash_sector_map_t  linux_flash_sector_map={0};
static linux_flash_sector_map_t* g_p_linux_flash_sector_map=&linux_flash_sector_map;

#define ERASE_BUFFER_SIZE     2048
static unsigned char erase_flash_buffer[ERASE_BUFFER_SIZE];
//
extern int cyg_hal_sys_fsync(int desc);
/*============================================
| Implementation
==============================================*/
/*--------------------------------------------
| Name:        _flash_make_sector_map
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
static int _flash_make_sector_map(void){
   int i=0;
   addr_t sector_addr=0;
   addr_t sector_offs=0;
   unsigned long dev_sz= DFLT_FILEFLASH_MEMORYSIZE;
   sector_no_t sector_no =0;
   sector_sz_t sector_sz =0;

   flash_sector_t* p_sector_map = (flash_sector_t*)((flash_type_t*)dev_flash_am29dlxxxx_linux.p_flash_type)->p_sector_map;

   linux_flash_sector_map_t* p_linux_flash_sector_map = &linux_flash_sector_map;

   while(sector_addr<dev_sz){
      if(!sector_sz || !((sector_offs)%sector_sz)){
         //
         linux_flash_sector_map_t* p_linux_flash_sector_map_next;
         //
         if((sector_addr>0) && (sector_addr >= p_sector_map[i+1].addr))
            i++;
         //
         if(p_sector_map[i].addr==0xFFFFFFFF)
            break;
         //
         if((sector_sz = p_sector_map[i].size)>g_sector_sz_max)
            g_sector_sz_max = sector_sz;
         sector_no++;

         p_linux_flash_sector_map_next = p_linux_flash_sector_map;

         p_linux_flash_sector_map = (linux_flash_sector_map_t*)malloc(sizeof(linux_flash_sector_map_t));
         p_linux_flash_sector_map->next = p_linux_flash_sector_map_next;
         p_linux_flash_sector_map->next->prev = p_linux_flash_sector_map;
         p_linux_flash_sector_map->prev = (linux_flash_sector_map_t*)0;

         p_linux_flash_sector_map->sector_no   = sector_no;
         p_linux_flash_sector_map->sector_addr = sector_addr;
         p_linux_flash_sector_map->sector_sz   = sector_sz;

         sector_addr+=sector_sz;
         sector_offs=sector_sz;

      }
   }

   p_internal_sector_buffer = (sector_data_t*)malloc(g_sector_sz_max);

   return 0;
}

/*--------------------------------------------
| Name:        _flash_get_sector_map
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
static linux_flash_sector_map_t* _flash_get_sector_map(sector_addr_t addr){
   int i=0;

   linux_flash_sector_map_t* p_linux_flash_sector_map = g_p_linux_flash_sector_map;

   while(p_linux_flash_sector_map){
      if(   addr>= p_linux_flash_sector_map->sector_addr
         && addr< p_linux_flash_sector_map->sector_addr+p_linux_flash_sector_map->sector_sz)
         return (p_linux_flash_sector_map);

      p_linux_flash_sector_map = p_linux_flash_sector_map->prev;
   }

   return p_linux_flash_sector_map;
}

/*--------------------------------------------
| Name:        _flash_erase_sector
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
static int _flash_erase_sector(sector_addr_t addr){
   sector_data_t _erase_pattern=0xff;
   sector_addr_t sector_addr = 0;
   sector_addr_t w=0;

   linux_flash_sector_map_t* p_linux_flash_sector_map = _flash_get_sector_map(addr);
   if(!p_linux_flash_sector_map)
      return -1;

   sector_addr = p_linux_flash_sector_map->sector_addr;

   //sector_addr = sector_addr>>2;
   //sector_addr = sector_addr<<2;

   cyg_hal_sys_lseek( fh, sector_addr, SEEK_SET);

   //while( (w+= cyg_hal_sys_write( fh,&_erase_pattern,sizeof(_erase_pattern)))<p_linux_flash_sector_map->sector_sz);
   //speed up erasing use dummy buffer
   while( (w+= cyg_hal_sys_write( fh,erase_flash_buffer,ERASE_BUFFER_SIZE))<p_linux_flash_sector_map->sector_sz);

   cyg_hal_sys_fdatasync(fh);

   return 0;
}

/*--------------------------------------------
| Name:        _flash_erase_all
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
static int _flash_erase_all(void){
   sector_data_t _erase_pattern=0xff;
   sector_addr_t sector_addr = 0;
   sector_addr_t w=0;

   linux_flash_sector_map_t* p_linux_flash_sector_map = g_p_linux_flash_sector_map;
   if(!p_linux_flash_sector_map)
      return -1;

   while(p_linux_flash_sector_map){
      sector_addr = p_linux_flash_sector_map->sector_addr;

      //sector_addr = sector_addr>>2;
      //sector_addr = sector_addr<<2;

      cyg_hal_sys_lseek( fh,sector_addr, SEEK_SET);
      //printf("@=0x%08x [%08d]\r\n",sector_addr,p_win32_flash_sector_map->sector_sz);
      w=0;

      //while( (w+= cyg_hal_sys_write( fh,&_erase_pattern,sizeof(_erase_pattern)))<p_linux_flash_sector_map->sector_sz);
      //speed up erasing use dummy buffer
      while( (w+= cyg_hal_sys_write( fh,erase_flash_buffer,ERASE_BUFFER_SIZE))<p_linux_flash_sector_map->sector_sz);
      //cyg_hal_sys_fdatasync(fh);

      p_linux_flash_sector_map = p_linux_flash_sector_map->prev;
   }

   cyg_hal_sys_fdatasync(fh);
   return 0;
}

/*-------------------------------------------
| Name:dev_linux_flash_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_flash_load(void){
   sector_data_t _erase_pattern=0xff;
   if(fh==-1){

      if( (fh = cyg_hal_sys_open( ".dev_linux_flash.o",_O_RDWR|_O_CREAT|_O_EXCL|_O_SYNC,_S_IREAD|_S_IWRITE)) == -1 ){
         if( (fh = cyg_hal_sys_open( ".dev_linux_flash.o",_O_RDWR |_O_SYNC,_S_IREAD|_S_IWRITE )) == -1 )
            return -1;

         cyg_hal_sys_lseek( fh, 0, SEEK_SET );

      }else{
         int w=0;

         cyg_hal_sys_close(fh);
         if( (fh = cyg_hal_sys_open( ".dev_linux_flash.o",_O_RDWR|_O_SYNC,_S_IREAD|_S_IWRITE)) == -1 )
            return -1;

         cyg_hal_sys_lseek(fh,0,SEEK_SET );
         cyg_hal_sys_lseek(fh,memory_size,SEEK_SET);
         //cyg_hal_sys_write(fh,memory,4);
      }
   }

   //
   _flash_make_sector_map();

   //use 1ko buffer for erasing
   memset(erase_flash_buffer,_erase_pattern, ERASE_BUFFER_SIZE);
   return 0;
}

/*-------------------------------------------
| Name:dev_linux_flash_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_flash_open(desc_t desc, int o_flag){
   if(o_flag & O_RDONLY){
   }

   if(o_flag & O_WRONLY){
   }

   ofile_lst[desc].offset = 0;
   //
   ofile_lst[desc].p=(void*)&dev_flash_am29dlxxxx_linux;
   instance_counter++;

   return 0;
}

/*-------------------------------------------
| Name:dev_linux_flash_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_flash_close(desc_t desc){

   if(fh==-1)
       return -1;

   instance_counter--;

   if(instance_counter<0){
      instance_counter=0;
      cyg_hal_sys_close(fh);
      fh = -1;
   }

   return 0;
}

/*-------------------------------------------
| Name:dev_linux_flash_isset_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_flash_isset_read(desc_t desc){
  return -1;
}

/*-------------------------------------------
| Name:dev_linux_flash_isset_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_flash_isset_write(desc_t desc){
      return -1;
}
/*-------------------------------------------
| Name:dev_linux_flash_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_flash_read(desc_t desc, char* buf,int size){
   int r;
   if(ofile_lst[desc].offset>memory_size)
      return -1;

   cyg_hal_sys_lseek( fh, ofile_lst[desc].offset, SEEK_SET);
   r= cyg_hal_sys_read( fh,buf,size);
   ofile_lst[desc].offset = cyg_hal_sys_lseek( fh, 0, SEEK_CUR);

   return r;
}

/*-------------------------------------------
| Name:dev_linux_flash_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_flash_write(desc_t desc, const char* buf,int size){
   int w;
   if(ofile_lst[desc].offset>memory_size)
      return -1;

   cyg_hal_sys_lseek( fh, ofile_lst[desc].offset, SEEK_SET);
   w = cyg_hal_sys_write( fh,buf,size);
   cyg_hal_sys_fsync(fh);
   ofile_lst[desc].offset = cyg_hal_sys_lseek( fh, 0, SEEK_CUR);

   return w;
}

/*-------------------------------------------
| Name:dev_linux_flash_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_flash_seek(desc_t desc,int offset,int origin){
   if(ofile_lst[desc].offset>memory_size)
      return -1;

   ofile_lst[desc].offset = cyg_hal_sys_lseek( fh, offset, origin);

   return ofile_lst[desc].offset;
}

/*-------------------------------------------
| Name:dev_linux_flash_ioctl
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_linux_flash_ioctl(desc_t desc,int request,va_list ap){
   switch(request){

      case HDGETSZ:{
         long* hdsz_p= va_arg( ap, long*);
         if(!hdsz_p)
            return -1;

         *hdsz_p = memory_size;
      }
      break;

      case HDSETSZ:{
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
         linux_flash_sector_map_t* p_linux_flash_sector_map;
         if(!sector_sz)
            return -1;

         if( !(p_linux_flash_sector_map = _flash_get_sector_map(sector_addr)) )
            return -1;

         *sector_sz = p_linux_flash_sector_map->sector_sz;
      }
      break;

      //
      case HDCLRSCTR:{
         unsigned long sector_addr= va_arg( ap, unsigned long);
         int r = _flash_erase_sector(sector_addr);
         return r;
      }
      break;

      //
      case HDCLRDSK:{
         int r = _flash_erase_all();
         return r;
      }
      break;

      //
      case HDIO:{
         hdio_t* hdio= va_arg( ap, hdio_t*);
         hdio->addr = (hdio_addr_t)((dev_flash_t*)ofile_lst[desc].p)->flash_base_pt;
      }
      break;

      default:
         return -1;

   }

   return 0;
}


/*============================================
| End of Source  : dev_linux_flash.c
|---------------------------------------------
| Historic:
|---------------------------------------------
| Authors     | Date     | Comments
| $Log: not supported by cvs2svn $
==============================================*/
