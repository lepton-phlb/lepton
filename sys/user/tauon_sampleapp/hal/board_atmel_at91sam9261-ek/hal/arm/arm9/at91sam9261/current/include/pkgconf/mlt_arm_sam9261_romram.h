// eCos memory layout - Fri Oct 20 05:43:59 2000

// This is a generated file - do not edit

#ifndef __ASSEMBLER__
   #include <cyg/infra/cyg_type.h>
   #include <stddef.h>

#endif
#define CYGMEM_REGION_ram                       (0x20000000)
#define CYGMEM_REGION_ram_SIZE          (0x03C00000)                            // 64-4 MB
#define CYGMEM_REGION_ram_ATTR          (CYGMEM_REGION_ATTR_R | CYGMEM_REGION_ATTR_W)

#define CYGMEM_REGION_sram                      (0x00000100)
#define CYGMEM_REGION_sram_SIZE         (0x00023F00)                            // 160 KB - 256 bytes
#define CYGMEM_REGION_sram_ATTR         (CYGMEM_REGION_ATTR_R | CYGMEM_REGION_ATTR_W)

#define CYGMEM_REGION_rom               (0x23C00000)
#define CYGMEM_REGION_rom_SIZE          (0x00400000)                            // 4 MB
#define CYGMEM_REGION_rom_ATTR          (CYGMEM_REGION_ATTR_R)

#ifndef __ASSEMBLER__
extern char CYG_LABEL_NAME (__heap1) [];
#endif

#define CYGMEM_SECTION_heap1            (CYG_LABEL_NAME (__heap1))
#define CYGMEM_SECTION_heap1_SIZE       (0x23C00000 - (size_t) CYG_LABEL_NAME (__heap1))



