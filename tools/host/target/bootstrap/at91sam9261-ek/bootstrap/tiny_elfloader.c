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
#include "tiny_elfloader.h"
#include <stdio.h>

/*============================================
| Global Declaration 
==============================================*/

#define ELF_MEMORY_ADDRESS_BOOTLOADER     (0x10100000) //bootloader address
#define ELF_MEMORY_ADDRESS_APPLICATION    (0x10200000) //bootloader address

static const char _lepton_bootstrap_banner[] = "\
lepton bootstrap v 0.0.6\r\n\
Chauvin-Arnoux Copyright(C) 2008\r\n\
$Revision: 1.2 $ $Date: 2010-06-22 09:22:49 $\r\n\
";

#define MAX_PHDR 8
//#define CYGOPT_REDBOOT_ELF_VIRTUAL_ADDRESS
#define BMP_MAX_LINE 1
#define DEBUG

#define USE_OPTIMIZED_READELF
#define  MAX_ELF_PRINTF_BUFFER 256

#define va_strt      va_start
#define _elf_printf  printf


static char * __kernel_date__=__DATE__;
static char * __kernel_time__=__TIME__;

static unsigned long load_address;
static unsigned long load_address_end;
static unsigned long entry_address;

static unsigned long elf_flash_offset=0;

typedef void (*BOOT_HANDLER)(void); 
typedef BOOT_HANDLER boot_handler_t; 

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

//
#define  ELF_MAGIC_SIZE       16

unsigned long find_elf(void);

/*============================================
| Implementation 
==============================================*/

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
| Name:        _tiny_elfloader
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
unsigned long _tiny_elfloader(unsigned long flash_base, unsigned long base)
{
    Elf32_Ehdr ehdr;

    Elf32_Phdr phdr[MAX_PHDR];
    unsigned long offset = 0;
    int phx=0;
    int len=0;
    unsigned char ch;
    unsigned char *addr;
    unsigned long addr_offset = 0;
    unsigned long highest_address = 0;
    unsigned long lowest_address = 0xFFFFFFFF;
    unsigned char *SHORT_DATA = "error: short data reading elf file\r\n";

    //
    int cb=0;
    boot_handler_t boot_handler = (boot_handler_t)0x00000000;

    #ifdef DEBUG
      _elf_printf("read elf file at 0x%x\r\n", (unsigned int)flash_base);
    #endif
    //ALREADY DONE
    //_at91sam9261_remap_internal_ram();

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
    _elf_printf("type: %d, machine: %d, version: %d\r\nentry: 0x%x, PHoff: 0x%x/%d/%d, SHoff: 0x%x/%d/%d\r\n\r\n",
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
      #if !defined(__GNUC__)
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
      _elf_printf("done\r\n");
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
        _elf_printf("section header: type: %d, off: 0x%x\r\nva: 0x%x, pa: 0x%x, len: %d/%d, flags: %d\r\n",
                phdr[phx].p_type, phdr[phx].p_offset, phdr[phx].p_vaddr, phdr[phx].p_paddr,
                phdr[phx].p_filesz, phdr[phx].p_memsz, phdr[phx].p_flags);
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
#if !defined(__GNUC__)
    _elf_lseek(flash_base,sizeof(ehdr),0);
#endif
    
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
            _elf_printf("program header: type: %d, off: 0x%x, va: 0x%x, pa:0x%x, len: %d/%d, flags: %d\r\n",
                phdr[phx].p_type, phdr[phx].p_offset, phdr[phx].p_vaddr, phdr[phx].p_paddr,
                phdr[phx].p_filesz, phdr[phx].p_memsz, phdr[phx].p_flags);

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
            #if defined(__GNUC__)
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
         _elf_printf("address offset = 0x%x\n", addr_offset);
      _elf_printf("firmware entry point: 0x%x, address range: 0x%x-0x%x\r\n",entry_address, load_address, load_address_end);
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
| Name:        tiny_elfloader
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int tiny_elfloader(void){
   unsigned long addr=0;

   _elf_printf(_lepton_bootstrap_banner);
   _elf_printf("compilation date %s-%s\r\n",__kernel_date__,__kernel_time__);

   //
   if(!(addr=find_elf())) {
      _elf_printf("No ELF signature found at 0x%x and 0x%x\r\n",
            ELF_MEMORY_ADDRESS_BOOTLOADER, ELF_MEMORY_ADDRESS_APPLICATION);
      return 0;
   }
   //
   _tiny_elfloader(addr,0x00000000);

   return 0;
}

/*--------------------------------------------
| Name:        find_elf
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
unsigned long find_elf(void) {
   char elf_magic[] = {0x7f,0x45,0x4c,0x46,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
   void * bootloader_addr = ELF_MEMORY_ADDRESS_BOOTLOADER;
   void * application_addr = ELF_MEMORY_ADDRESS_APPLICATION;
   //search app and then bootlaoder
   if(!memcmp((void *)elf_magic,application_addr,ELF_MAGIC_SIZE)) {
      return ELF_MEMORY_ADDRESS_APPLICATION;
   }
   else if(!memcmp((void *)elf_magic,bootloader_addr,ELF_MAGIC_SIZE)) {
      return ELF_MEMORY_ADDRESS_BOOTLOADER;
   }
   return 0;
}

/*============================================
| End of Source  : kernel_elfloader.c
==============================================*/
