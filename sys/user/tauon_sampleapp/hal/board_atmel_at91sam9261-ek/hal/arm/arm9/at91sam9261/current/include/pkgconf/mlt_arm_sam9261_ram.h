// eCos memory layout - Fri Oct 20 05:43:59 2000

// This is a generated file - do not edit

#ifndef __ASSEMBLER__
   #include <cyg/infra/cyg_type.h>
   #include <stddef.h>

#endif
#define CYGMEM_REGION_ram      (0x20000000)
#define CYGMEM_REGION_ram_SIZE (0x04000000)                     // 64 MB
#define CYGMEM_REGION_ram_ATTR (CYGMEM_REGION_ATTR_R | CYGMEM_REGION_ATTR_W)
//#define CYGMEM_REGION_sram     (0x00000100)
#define CYGMEM_REGION_sram     (0x00300000)
//#define CYGMEM_REGION_sram_SIZE (0x23F00)			// 160 KB - 256 bytes
#define CYGMEM_REGION_sram_SIZE (0x28000)                       // 160 KB - 256 bytes
#define CYGMEM_REGION_sram_ATTR (CYGMEM_REGION_ATTR_R | CYGMEM_REGION_ATTR_W)
#ifndef __ASSEMBLER__
extern char CYG_LABEL_NAME (__heap1) [];
#endif
#define CYGMEM_SECTION_heap1 (CYG_LABEL_NAME (__heap1))
#define CYGMEM_SECTION_heap1_SIZE (0x23F00000 - (size_t) CYG_LABEL_NAME (__heap1))
