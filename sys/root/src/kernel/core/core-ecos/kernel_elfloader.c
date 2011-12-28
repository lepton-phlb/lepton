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
#include <stdarg.h>

#include "kernel/core/kernelconf.h"

#include "kernel/core/errno.h"
#include "kernel/core/types.h"
#include "kernel/core/kernel.h"
#include "kernel/core/system.h"
#include "kernel/core/statvfs.h"
#include "kernel/core/time.h"
#include "kernel/core/ioctl.h"
#include "kernel/core/errno.h"
#include "kernel/core//ioctl_lcd.h"
#include "kernel/fs/vfs/vfs.h"
#include "kernel/fs/vfs/vfskernel.h"
#include "kernel/core/kernel_elfloader.h"

#include "lib/libc/misc/ltostr.h"
#include "lib/libc/stdio/stdio.h"

/*============================================
| Global Declaration 
==============================================*/

#define ELF_MEMORY_ADDRESS (0x10100000) //firmware address (with samba address at 0x00100000)

static const char _lepton_bootstrap_banner[] = "\
lepton bootstrap v 0.0.6\r\n\
$Revision: 1.2 $ $Date: 2010-03-03 09:27:12 $\r\n\
";

#define MAX_PHDR 8
//#define CYGOPT_REDBOOT_ELF_VIRTUAL_ADDRESS
#define BMP_MAX_LINE 1
#define DEBUG
//#define USE_SPLASH_SCREEN
#define USE_OPTIMIZED_READELF

static unsigned long load_address;
static unsigned long load_address_end;
static unsigned long entry_address;

#define AT91SAM9261_REMAP_REGISTER ((const unsigned long)(0x00000003))

static unsigned long elf_flash_offset=0;

typedef void (*BOOT_HANDLER)(void); 
typedef BOOT_HANDLER boot_handler_t; 

#define  MAX_ELF_PRINTF_BUFFER 256

#define va_strt      va_start
extern int __vprintf(const char *fmt,va_list ap);

desc_t _ELF_STDOUT_DESCNO;

//bmp structure
typedef struct {unsigned char byte0,byte1,byte2,byte3;} four_byte_t;

typedef struct {unsigned char byte0,byte1;} two_byte_t;

typedef struct { 
   two_byte_t magic_nr;
   four_byte_t file_length;
   four_byte_t reserved_area;
   four_byte_t data_pointer;
} bmp_file_header_t;

typedef struct { 
   four_byte_t format_header_length;
   four_byte_t picture_width;
   four_byte_t picture_height;
   two_byte_t  planes_count;
   two_byte_t  bits_per_pixel;
   four_byte_t compression;
   four_byte_t imagesize;
   four_byte_t x_pixel_per_meter;
   four_byte_t y_pixel_per_meter;
   four_byte_t amount_used_colors;
   four_byte_t amount_important_colors;
} bmp_format_header_t;

typedef struct { 
   bmp_file_header_t    file_header;
   bmp_format_header_t  format_header;
} bmp_header_t;


fbcmap_t g_fbcmap[256];

/*============================================
| Implementation 
==============================================*/
/*--------------------------------------------
| Name:        _at91sam9261_remap_internal_ram
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int _at91sam9261_remap_internal_ram(){
   unsigned long* p = (unsigned long*)0x00000000;
   unsigned long* p_at91sam9261_matrix_remap_register = (unsigned long*)0xFFFFEE00;
   *p=0xAAAAAAAA;
   if((*p)==0xAAAAAAAA){
      *p=0x55555555;
      if((*p)==0x55555555)
         return -1; //remap internal remap already done
   }
   //remap internal remap
   *p_at91sam9261_matrix_remap_register = AT91SAM9261_REMAP_REGISTER;
   //
   return 0;
}

/*--------------------------------------------
| Name:        _elf_printf
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int _elf_printf(const char * fmt, ...){
   static char buf[MAX_ELF_PRINTF_BUFFER];
   int cb=0;
   FILE  string[1] =
   {
      {0, 0, (char*)(unsigned) -1, 0, (char*) (unsigned) -1, -1,
       _IOFBF | __MODE_WRITE}
   };

  va_list ptr;
  int rv;

  if(_ELF_STDOUT_DESCNO<0)
     return -1;
  
  va_strt(ptr, fmt);
  string->bufpos = buf;
  string->bufend = buf+sizeof(buf);
  rv = __vfprintf(string,fmt,ptr);
  va_end(ptr);
  *(string->bufpos) = 0;
 

  while(cb<rv){
     int r=-1;
     if((r=ofile_lst[_ELF_STDOUT_DESCNO].pfsop->fdev.fdev_write(_ELF_STDOUT_DESCNO,buf+cb,rv-cb))<0)
        return r;
     cb+=r;
  }

  return cb;
}

/*--------------------------------------------
| Name:        elf_getc
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
static int _elf_getc(unsigned long flash_base){
   int c=0x00;
   unsigned char* p=(unsigned char*)(flash_base+elf_flash_offset);
   //
   c=*(p);
   elf_flash_offset++;
   //      
   return c;
}

/*--------------------------------------------
| Name:        elf_read
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
static int _elf_read(unsigned long flash_base, char* buffer, int size){
   unsigned char* p=(unsigned char*)(flash_base+elf_flash_offset);
   //
   memcpy(buffer,p,size);
   elf_flash_offset+=size;
   //
   return size;
}

/*--------------------------------------------
| Name:        _elf_lseek
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
static int _elf_lseek(unsigned long flash_base,int offset, int origin){
   elf_flash_offset=offset;
   return elf_flash_offset;
}

/*--------------------------------------------
| Name:        _kernel_load_splash_screen
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
static int _kernel_load_splash_screen(desc_t desc_fb,char * path){
   desc_t desc;
   bmp_header_t header;
   int i;
   int xdim,ydim;
   int l;
   int offset;
   static unsigned char l_buf[640*BMP_MAX_LINE];//640 8 bits per pixel 
   unsigned long fb_addr;


   if((desc=_vfs_open(path,O_RDONLY,0))<0)
      return -1;

   _vfs_read(desc,&header.file_header.magic_nr.byte0,1);
   _vfs_read(desc,&header.file_header.magic_nr.byte1,1);
    

   _vfs_read(desc,&header.file_header.file_length.byte0,1);
   _vfs_read(desc,&header.file_header.file_length.byte1,1);
   _vfs_read(desc,&header.file_header.file_length.byte2,1);
   _vfs_read(desc,&header.file_header.file_length.byte3,1);
   
   
   _vfs_read(desc,&header.file_header.reserved_area.byte0,1);
   _vfs_read(desc,&header.file_header.reserved_area.byte1,1);
   _vfs_read(desc,&header.file_header.reserved_area.byte2,1);
   _vfs_read(desc,&header.file_header.reserved_area.byte3,1);

   _vfs_read(desc,&header.file_header.data_pointer.byte0,1);
   _vfs_read(desc,&header.file_header.data_pointer.byte1,1);
   _vfs_read(desc,&header.file_header.data_pointer.byte2,1);
   _vfs_read(desc,&header.file_header.data_pointer.byte3,1);
   

   _vfs_read(desc,&header.format_header.format_header_length.byte0,1);
   _vfs_read(desc,&header.format_header.format_header_length.byte1,1);
   _vfs_read(desc,&header.format_header.format_header_length.byte2,1);
   _vfs_read(desc,&header.format_header.format_header_length.byte3,1);

   _vfs_read(desc,&header.format_header.picture_width.byte0,1);
   _vfs_read(desc,&header.format_header.picture_width.byte1,1);
   _vfs_read(desc,&header.format_header.picture_width.byte2,1);
   _vfs_read(desc,&header.format_header.picture_width.byte3,1);

   _vfs_read(desc,&header.format_header.picture_height.byte0,1);
   _vfs_read(desc,&header.format_header.picture_height.byte1,1);
   _vfs_read(desc,&header.format_header.picture_height.byte2,1);
   _vfs_read(desc,&header.format_header.picture_height.byte3,1);
  
   _vfs_read(desc,&header.format_header.planes_count.byte0,1);
   _vfs_read(desc,&header.format_header.planes_count.byte1,1);
   
   _vfs_read(desc,&header.format_header.bits_per_pixel.byte0,1);
   _vfs_read(desc,&header.format_header.bits_per_pixel.byte1,1);

   _vfs_read(desc,&header.format_header.compression.byte0,1);
   _vfs_read(desc,&header.format_header.compression.byte1,1);
   _vfs_read(desc,&header.format_header.compression.byte2,1);
   _vfs_read(desc,&header.format_header.compression.byte3,1);

   _vfs_read(desc,&header.format_header.imagesize.byte0,1);
   _vfs_read(desc,&header.format_header.imagesize.byte1,1);
   _vfs_read(desc,&header.format_header.imagesize.byte2,1);
   _vfs_read(desc,&header.format_header.imagesize.byte3,1);

   _vfs_read(desc,&header.format_header.x_pixel_per_meter.byte0,1);
   _vfs_read(desc,&header.format_header.x_pixel_per_meter.byte1,1);
   _vfs_read(desc,&header.format_header.x_pixel_per_meter.byte2,1);
   _vfs_read(desc,&header.format_header.x_pixel_per_meter.byte3,1);

   _vfs_read(desc,&header.format_header.y_pixel_per_meter.byte0,1);
   _vfs_read(desc,&header.format_header.y_pixel_per_meter.byte1,1);
   _vfs_read(desc,&header.format_header.y_pixel_per_meter.byte2,1);
   _vfs_read(desc,&header.format_header.y_pixel_per_meter.byte3,1);

   _vfs_read(desc,&header.format_header.amount_used_colors.byte0,1);
   _vfs_read(desc,&header.format_header.amount_used_colors.byte1,1);
   _vfs_read(desc,&header.format_header.amount_used_colors.byte2,1);
   _vfs_read(desc,&header.format_header.amount_used_colors.byte3,1);

   _vfs_read(desc,&header.format_header.amount_important_colors.byte0,1);
   _vfs_read(desc,&header.format_header.amount_important_colors.byte1,1);
   _vfs_read(desc,&header.format_header.amount_important_colors.byte2,1);
   _vfs_read(desc,&header.format_header.amount_important_colors.byte3,1);

   //get palette
   for(i=0;i<256;i++)
   {
      _vfs_read(desc,&g_fbcmap[i].blue,1);
      _vfs_read(desc,&g_fbcmap[i].green,1);
      _vfs_read(desc,&g_fbcmap[i].red,1);
      _vfs_read(desc,&g_fbcmap[i].pe_flags,1);
   }
   //set palette
   _vfs_ioctl(desc_fb,FBIOPUTCMAP,g_fbcmap);

   //draw bit map
   xdim= header.format_header.picture_width.byte0
   + header.format_header.picture_width.byte1 * 256
   + header.format_header.picture_width.byte2 * 256*256
   + header.format_header.picture_width.byte3 * 256*256*256;

   ydim= header.format_header.picture_height.byte0
   + header.format_header.picture_height.byte1 * 256
   + header.format_header.picture_height.byte2 * 256*256
   + header.format_header.picture_height.byte3 * 256*256*256;

   offset = header.file_header.data_pointer.byte0
   + header.file_header.data_pointer.byte1 *256
   + header.file_header.data_pointer.byte2 *256*256
   + header.file_header.data_pointer.byte3 *256*256*256;



   _vfs_lseek(desc,offset,SEEK_SET);

   _vfs_ioctl(desc_fb,LCDGETVADDR,&fb_addr);

   /*
   //not optimized 
   for(l=0;l<ydim;l++){
      unsigned char* _p_fb=(unsigned char*)fb_addr;
      int cb=0;
      if((cb=_vfs_read(desc,l_buf,xdim))<=0)
         break;
      _p_fb+=((ydim-l)*xdim);
      memcpy(_p_fb,l_buf,cb);
   }*/
   //optimized draft version
   for(l=BMP_MAX_LINE;l<ydim;l+=BMP_MAX_LINE){
      unsigned char* _p_fb=(unsigned char*)fb_addr;
      int cb=0;
      _p_fb+=((ydim-l)*xdim);
      if((cb=_vfs_read(desc,_p_fb,xdim*BMP_MAX_LINE))<=0)
         break;
      
      //memcpy(_p_fb,l_buf,cb);
   }

   _vfs_close(desc);

   return 0;
}

/*--------------------------------------------
| Name:        _kernel_elfloader
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
unsigned long _kernel_elfloader(unsigned long flash_base, unsigned long base)
{
    Elf32_Ehdr ehdr;

    Elf32_Phdr phdr[MAX_PHDR];
    unsigned long offset = 0;
    int phx=0;
    int len=0;
    int ch=0;
    unsigned char *addr;
    unsigned long addr_offset = 0;
    unsigned long highest_address = 0;
    unsigned long lowest_address = 0xFFFFFFFF;
    unsigned char *SHORT_DATA = "error: short data reading elf file\r\n";

    //
    int cb=0;
    boot_handler_t boot_handler = (boot_handler_t)0x00000000;

    #ifdef DEBUG
      _elf_printf("read elf file at 0x%x\r\n",flash_base);
    #endif
    _at91sam9261_remap_internal_ram();

    // Read the header
    _elf_printf("read elf header informations:\r\n" );
    if (_elf_read(flash_base, (unsigned char *)&ehdr, sizeof(ehdr)) != sizeof(ehdr)) {
        #ifdef DEBUG
        _elf_printf("error: can't read elf header\r\n");
        #endif
        return 0;
    }
    offset += sizeof(ehdr);    
    //
    #ifdef DEBUG
    _elf_printf("type: %d, machine: %d, version: %d\r\nentry: %p, PHoff: %p/%d/%d, SHoff: %p/%d/%d\r\n\r\n",
            ehdr.e_type, ehdr.e_machine, ehdr.e_version, ehdr.e_entry, 
            ehdr.e_phoff, ehdr.e_phentsize, ehdr.e_phnum,
            ehdr.e_shoff, ehdr.e_shentsize, ehdr.e_shnum);
    #endif
    //
    if (ehdr.e_type != ET_EXEC) {
        #ifdef DEBUG
        _elf_printf("error: only absolute elf images supported\r\n");
        #endif
        return 0;
    }
    //
    if (ehdr.e_phnum > MAX_PHDR) {
        #ifdef DEBUG
        _elf_printf("error: too many firmware headers\r\n");
        #endif
        return 0;
    }
    #ifdef DEBUG
      _elf_printf("jump to offset 0x%x wait some seconds... ",ehdr.e_phoff);
    #endif
    //jump to offset
    #ifdef USE_OPTIMIZED_READELF
      //optimized code
      #if !defined(GNU_GCC)
      _elf_lseek(flash_base,ehdr.e_phoff,0);
      offset+=ehdr.e_phoff;
      #endif
    #else
      //not optimized original code from reboot
      while (offset < ehdr.e_phoff) {
         if (_elf_getc(flash_base) < 0) {
            #ifdef DEBUG
            printf(SHORT_DATA);
            #endif
            return 0;
         }
         offset++;
      }
    #endif

    //
    #ifdef DEBUG
      _elf_printf("done\r\n",ehdr.e_phoff);
    #endif

    #ifdef DEBUG
      _elf_printf("read elf section header\r\n");
    #endif
    //
    for (phx = 0;  phx < ehdr.e_phnum;  phx++) {
        if (_elf_read(flash_base, (unsigned char *)&phdr[phx], sizeof(phdr[0])) != sizeof(phdr[0])) {
            #ifdef DEBUG
            _elf_printf("error: can't read ELF program header\r\n");
            #endif
            return 0;
        }
        #ifdef DEBUG
        /*_elf_printf("section header: type: %d, off: %p\r\nva: %p, pa: %p, len: %d/%d, flags: %d\r\n",
                phdr[phx].p_type, phdr[phx].p_offset, phdr[phx].p_vaddr, phdr[phx].p_paddr,
                phdr[phx].p_filesz, phdr[phx].p_memsz, phdr[phx].p_flags);*/
        #endif
        offset += sizeof(phdr[0]);
    }

    if (base) {
        // Set address offset based on lowest address in file.
        addr_offset = 0xFFFFFFFF;
        for (phx = 0;  phx < ehdr.e_phnum;  phx++) {
            #ifdef CYGOPT_REDBOOT_ELF_VIRTUAL_ADDRESS     
               if ((phdr[phx].p_type == PT_LOAD) && (phdr[phx].p_vaddr < addr_offset)) {
                  addr_offset = phdr[phx].p_vaddr;
            #else
               if ((phdr[phx].p_type == PT_LOAD) && (phdr[phx].p_paddr < addr_offset)) {
                  addr_offset = phdr[phx].p_paddr;
            #endif
               }
        }
        addr_offset = (unsigned long)base - addr_offset;
    }else{
      addr_offset = 0;
    }
    //phlb modif
#if !defined(GNU_GCC)
    _elf_lseek(flash_base,sizeof(ehdr),0);
#endif
    //_elf_printf("3)fl_off:%d-offset:%d\r\n",elf_flash_offset,offset);
    //
    
    #ifdef DEBUG
      _elf_printf("copy firmware in ram started:\r\n");
    #endif

    for (phx = 0;  phx < ehdr.e_phnum;  phx++) {

       //
       if (phdr[phx].p_type == PT_LOAD) {
            // Loadable segment
            #ifdef CYGOPT_REDBOOT_ELF_VIRTUAL_ADDRESS
               addr = (unsigned char *)phdr[phx].p_vaddr;
            #else     
               addr = (unsigned char *)phdr[phx].p_paddr;
            #endif

            //
            len = phdr[phx].p_filesz;
            if ((unsigned long)addr < lowest_address) {
                lowest_address = (unsigned long)addr;
            }
            //
            addr += addr_offset;
            if (offset > phdr[phx].p_offset) {
               /*
                if ((phdr[phx].p_offset + len) < offset) {
                    printf("Can't load ELF file - program headers out of order\r\n");
                    return 0;
                }
                */
                /*addr += offset - phdr[phx].p_offset;*/
            } else {
                while (offset < phdr[phx].p_offset) {
                    if (_elf_getc(flash_base) < 0) {
                        #ifdef DEBUG
                        printf(SHORT_DATA);
                        #endif
                        return 0;
                    }
                    offset++;
                }
            }

            #ifdef DEBUG
            /*_elf_printf("program header: type: %d, off: %p, va: %p, pa: %p, len: %d/%d, flags: %d\r\n",
                phdr[phx].p_type, phdr[phx].p_offset, phdr[phx].p_vaddr, phdr[phx].p_paddr,
                phdr[phx].p_filesz, phdr[phx].p_memsz, phdr[phx].p_flags);*/

            //_elf_printf("addr after offset : %p 0x%x", addr, addr_offset);
            #endif

            // Copy data into memory
            #ifndef USE_OPTIMIZED_READELF
            while (len-- > 0) {

                if ((ch = _elf_getc(flash_base)) < 0) {
                    #ifdef DEBUG
                    printf(SHORT_DATA);
                    #endif
                    return 0;
                }
                #ifdef CYGSEM_REDBOOT_VALIDATE_USER_RAM_LOADS
                if (valid_address(addr)) 
                #endif
                  *addr = ch;//original code
                  #ifdef DEBUG
                     if(!(((unsigned long)offset)%(80*1*1024)))
                        _elf_printf(".");
                  #endif

                addr++;
                offset++;
                if ((unsigned long)(addr-addr_offset) > highest_address) {
                    highest_address = (unsigned long)(addr - addr_offset);
                }
            }
            #endif

            #ifdef USE_OPTIMIZED_READELF
            //
            #if defined(GNU_GCC)
            _elf_lseek(flash_base,phdr[phx].p_offset,0);
            _elf_printf("offset:%d addr:0x%x fl_offset:%d\r\n", offset, addr, elf_flash_offset);
            #endif

            cb=0;
            while((len-cb)){
               unsigned char elf_buffer[4096]={0};
               int sz=0;

               if((len-cb)>=sizeof(elf_buffer))
                  sz=sizeof(elf_buffer);
               else
                  sz=(len-cb);

               //cb += read(fd,elf_buffer,sz);
               cb+=_elf_read(flash_base, addr, sz);
               /*
               #ifdef DEBUG
               lseek(fd_bin,(unsigned long)addr,SEEK_SET);
               write(fd_bin,elf_buffer,sz);
               #endif
               */
               #ifdef CYGSEM_REDBOOT_VALIDATE_USER_RAM_LOADS
               if (valid_address(addr)) 
               #endif
                  //memcpy(addr,elf_buffer,sz);

               //
               addr+=sz;
               offset+=sz;
               if ((unsigned long)(addr-addr_offset) > highest_address) {
                  highest_address = (unsigned long)(addr - addr_offset);
               }
            }
            #endif
        }
    }

    // Save load base/top and entry
    if (base) {
        load_address = base;
        load_address_end = base + (highest_address - lowest_address);
        entry_address = base + (ehdr.e_entry - lowest_address);
    } else {
        load_address = lowest_address;
        load_address_end = highest_address;
        entry_address = ehdr.e_entry;
    }

    // nak everything to stop the transfer, since redboot
    // usually doesn't read all the way to the end of the
    // elf files.
    #ifdef DEBUG
      _elf_printf("\r\ncopy firmware in ram done\r\n");
      if (addr_offset) 
         _elf_printf("address offset = %p\n", (void *)addr_offset);
      _elf_printf("firmware entry point: %p, address range: %p-%p\r\n",(void*)entry_address, (void *)load_address, (void *)load_address_end);
      _elf_printf("ready to rumble? ;)\r\nboot on firmware\r\n");
   #endif
   
   boot_handler = (boot_handler_t)entry_address;

   //boot!!!!
   boot_handler();
   __asm__("nop");
   __asm__("nop");
   __asm__("nop");
   __asm__("nop");
   __asm__("nop");
   __asm__("nop");
   for(;;);

   return 1;
}

/*--------------------------------------------
| Name:        _kernel_warmup_elfloader
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int _kernel_warmup_elfloader(void){
   desc_t desc_lcd0=-1;
   desc_t desc_tty0=-1;

   _ELF_STDOUT_DESCNO=-1;

   desc_lcd0 = _vfs_open("/dev/lcd0",O_WRONLY,0);
   #ifdef DEBUG
      desc_tty0 = _vfs_open("/dev/tty0",O_WRONLY,0);

      _vfs_ioctl(desc_tty0,I_LINK,desc_lcd0);

      _ELF_STDOUT_DESCNO = desc_tty0; 
   #endif

   _elf_printf(_lepton_bootstrap_banner);
   _elf_printf("compilation date %s-%s\r\n",__kernel_date__,__kernel_time__);
   
   #ifdef DEBUG
      _elf_printf("kernel warmup elfloader from 0x%x\r\n",ELF_MEMORY_ADDRESS);
   #endif
   #ifdef USE_SPLASH_SCREEN
      _kernel_load_splash_screen(desc_lcd0,"/usr/etc/splash.bmp");
   #endif
   //
   _kernel_elfloader(ELF_MEMORY_ADDRESS,0x00000000);

   return 0;
}

/*============================================
| End of Source  : kernel_elfloader.c
==============================================*/
