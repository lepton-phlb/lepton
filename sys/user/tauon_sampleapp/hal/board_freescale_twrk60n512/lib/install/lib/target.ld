STARTUP(vectors.o)
ENTRY(reset_vector)
INPUT(extras.o)
GROUP(libtarget.a libgcc.a libsupc++.a)



MEMORY
{
    ram : ORIGIN = 0x20000000 - 0x00010000, LENGTH = 0x00020000 -4096
    mram : ORIGIN = 0x60000000, LENGTH = 512K
}

SECTIONS
{
    .debug_aranges 0 : { *(.debug_aranges) } .debug_pubnames 0 : { *(.debug_pubnames) } .debug_info 0 : { *(.debug_info) } .debug_abbrev 0 : { *(.debug_abbrev) } .debug_line 0 : { *(.debug_line) } .debug_frame 0 : { *(.debug_frame) } .debug_str 0 : { *(.debug_str) } .debug_loc 0 : { *(.debug_loc) } .debug_macinfo 0 : { *(.debug_macinfo) } .note.arm.ident 0 : { KEEP (*(.note.arm.ident)) } /DISCARD/ 0 : { *(.fini_array*) }
    .rom_vectors 0x60000000 : { __rom_vectors_vma = ABSOLUTE(.); . = .; KEEP (*(.vectors)) } > mram __rom_vectors_lma = LOADADDR(.rom_vectors);
    .rel.text : { *(.rel.text) *(.rel.text.*) *(.rel.gnu.linkonce.t*) } > mram .rela.text : { *(.rela.text) *(.rela.text.*) *(.rela.gnu.linkonce.t*) } > mram .rel.data : { *(.rel.data) *(.rel.data.*) *(.rel.gnu.linkonce.d*) } > mram .rela.data : { *(.rela.data) *(.rela.data.*) *(.rela.gnu.linkonce.d*) } > mram .rel.rodata : { *(.rel.rodata) *(.rel.rodata.*) *(.rel.gnu.linkonce.r*) } > mram .rela.rodata : { *(.rela.rodata) *(.rela.rodata.*) *(.rela.gnu.linkonce.r*) } > mram .rel.got : { *(.rel.got) } > mram .rela.got : { *(.rela.got) } > mram .rel.ctors : { *(.rel.ctors) } > mram .rela.ctors : { *(.rela.ctors) } > mram .rel.dtors : { *(.rel.dtors) } > mram .rela.dtors : { *(.rela.dtors) } > mram .rel.init : { *(.rel.init) } > mram .rela.init : { *(.rela.init) } > mram .rel.fini : { *(.rel.fini) } > mram .rela.fini : { *(.rela.fini) } > mram .rel.bss : { *(.rel.bss) } > mram .rela.bss : { *(.rela.bss) } > mram .rel.plt : { *(.rel.plt) } > mram .rela.plt : { *(.rela.plt) } > mram .rel.dyn : { *(.rel.dyn) } > mram
    .ARM.extab ALIGN (0x8) : { PROVIDE (__stext = ABSOLUTE(.));_stext = ABSOLUTE(.) ; . = .; *(.ARM.extab* .gnu.linkonce.armextab.*) } > mram . = ALIGN(8); __exidx_start = ABSOLUTE(.); .ARM.exidx ALIGN(8) : AT ((LOADADDR (.ARM.extab) + SIZEOF (.ARM.extab) + (8) - 1) & ~ ((8) - 1)) { *(.ARM.exidx* .gnu.linkonce.armexidx.*) . = .; } > mram __exidx_end = ABSOLUTE(.); .text ALIGN(8) : AT ((LOADADDR (.ARM.exidx) + SIZEOF (.ARM.exidx) + (8) - 1) & ~ ((8) - 1)) { *(.text*) *(.gnu.warning) *(.gnu.linkonce.t.*) *(.init) *(.glue_7) *(.glue_7t) __CTOR_LIST__ = ABSOLUTE (.); KEEP (*(SORT (.ctors*))) __CTOR_END__ = ABSOLUTE (.); __DTOR_LIST__ = ABSOLUTE (.); KEEP (*(SORT (.dtors*))) __DTOR_END__ = ABSOLUTE (.); } > mram _etext = .; PROVIDE (__etext = .);
    .fini ALIGN (0x8) : { . = .; *(.fini) } > mram
    .rodata ALIGN (0x8) : { . = .; *(.rodata*) *(.gnu.linkonce.r.*) } > mram
    .rodata1 ALIGN (0x8) : { . = .; *(.rodata1) } > mram
    .fixup ALIGN (0x8) : { . = .; *(.fixup) } > mram
    .gcc_except_table ALIGN (0x8) : { . = .; KEEP(*(.gcc_except_table)) *(.gcc_except_table.*) } > mram
    .eh_frame ALIGN (0x8) : { . = .; __EH_FRAME_BEGIN__ = .; KEEP(*(.eh_frame)) __FRAME_END__ = .; . = . + 8; } > mram = 0
    .got ALIGN (0x8) : { . = .; *(.got.plt) *(.got) _GOT1_START_ = ABSOLUTE (.); *(.got1) _GOT1_END_ = ABSOLUTE (.); _GOT2_START_ = ABSOLUTE (.); *(.got2) _GOT2_END_ = ABSOLUTE (.); } > mram
    .data 0x20000000 - 0x00010000 + 0x400 : AT ((LOADADDR (.got) + SIZEOF (.got) + (4) - 1) & ~ ((4) - 1)) { __ram_data_start = ABSOLUTE (.); *(.data*) *(.data1) *(.gnu.linkonce.d.*) . = ALIGN (4); KEEP(*( SORT (.ecos.table.*))) ; . = ALIGN (4); __init_array_start__ = ABSOLUTE (.); KEEP (*(SORT (.init_array.*))) KEEP (*(SORT (.init_array))) __init_array_end__ = ABSOLUTE (.); *(.dynamic) *(.sdata*) *(.gnu.linkonce.s.*) . = ALIGN (4); *(.2ram.*) } > ram __rom_data_start = LOADADDR (.data); __ram_data_end = .; PROVIDE (__ram_data_end = .); _edata = .; PROVIDE (edata = .); PROVIDE (__rom_data_end = LOADADDR (.data) + SIZEOF(.data));
    .sram ALIGN (0x8) : { __sram_data_start = ABSOLUTE (.); *(.sram*) . = ALIGN (4); } > ram __srom_data_start = LOADADDR (.sram); __sram_data_end = .; PROVIDE (__sram_data_end = .); PROVIDE (__srom_data_end = LOADADDR (.sram) + SIZEOF(.sram));
    .bss ALIGN (0x8) : { __bss_start = ABSOLUTE (.); *(.scommon) *(.dynsbss) *(.sbss*) *(.gnu.linkonce.sb.*) *(.dynbss) *(.bss*) *(.gnu.linkonce.b.*) *(COMMON) __bss_end = ABSOLUTE (.); } > ram
    __heap1 = ALIGN (0x8);
    . = ALIGN(4); _end = .; PROVIDE (end = .);
}

hal_vsr_table = (0x20000000 - 0x00010000);
hal_virtual_vector_table = hal_vsr_table + 128*4;
hal_startup_stack = 0x20000000 + 0x00010000;
