STARTUP(vectors.o)
ENTRY(reset_vector)
INPUT(extras.o)
GROUP( libtarget.a libgcc.a libsupc++.a )




MEMORY
{
   ram : ORIGIN = 0x20000000, LENGTH = 0x3F00000
   ram2 : ORIGIN = 0x23F00000, LENGTH = 0x0100000

   sram : ORIGIN = 0x00200000, LENGTH = 0x1000
}

SECTIONS
{
    .debug_aranges 0 : { *(.debug_aranges) } .debug_pubnames 0 : { *(.debug_pubnames) } .debug_info 0 : { *(.debug_info) } .debug_abbrev 0 : { *(.debug_abbrev) } .debug_line 0 : { *(.debug_line) } .debug_frame 0 : { *(.debug_frame) } .debug_str 0 : { *(.debug_str) } .debug_loc 0 : { *(.debug_loc) } .debug_macinfo 0 : { *(.debug_macinfo) } .note.arm.ident 0 : { KEEP (*(.note.arm.ident)) } /DISCARD/ 0 : { *(.fini_array*) }


  .fixed_vectors 0x20000000 : { . = .; KEEP (*(.fixed_vectors)) } > ram


    .rom_vectors 0x20000200 : { __rom_vectors_vma = ABSOLUTE(.); . = .; KEEP (*(.vectors)) } > ram __rom_vectors_lma = LOADADDR(.rom_vectors);


    .ARM.extab ALIGN (0x4) : { PROVIDE (__stext = ABSOLUTE(.)); _stext = ABSOLUTE(.) ; . = .; } > ram /DISCARD/ 0 : { *(.ARM.extab* .gnu.linkonce.armextab.*) } . = ALIGN(8); . = ALIGN(8); __exidx_start = ABSOLUTE(.); .ARM.exidx ALIGN(8) : AT ((LOADADDR (.ARM.extab) + SIZEOF (.ARM.extab) + (8) - 1) & ~ ((8) - 1)) { . = .; } > ram __exidx_end = ABSOLUTE(.); /DISCARD/ 0 : { *(.ARM.exidx* .gnu.linkonce.armexidx.*) } .text ALIGN(8) : AT ((LOADADDR (.ARM.exidx) + SIZEOF (.ARM.exidx) + (8) - 1) & ~ ((8) - 1)) { *(.text*) *(.gnu.warning) *(.gnu.linkonce.t.*) *(.init) *(.glue_7) *(.glue_7t) __CTOR_LIST__ = ABSOLUTE (.); KEEP (*(SORT (.ctors*))) __CTOR_END__ = ABSOLUTE (.); __DTOR_LIST__ = ABSOLUTE (.); KEEP (*(SORT (.dtors*))) __DTOR_END__ = ABSOLUTE (.); } > ram _etext = .; PROVIDE (__etext = .);
    .fini ALIGN (0x4) : { . = .; *(.fini) } > ram
    .rodata ALIGN (0x4) : { . = .; *(.rodata*) *(.gnu.linkonce.r.*) } > ram
    .rodata1 ALIGN (0x4) : { . = .; *(.rodata1) } > ram
    .got ALIGN (0x4) : { . = .; *(.got.plt) *(.got) _GOT1_START_ = ABSOLUTE (.); *(.got1) _GOT1_END_ = ABSOLUTE (.); _GOT2_START_ = ABSOLUTE (.); *(.got2) _GOT2_END_ = ABSOLUTE (.); } > ram
    .fixup ALIGN (0x4) : { . = .; *(.fixup) } > ram
    .gcc_except_table ALIGN (0x4) : { . = .; KEEP(*(.gcc_except_table)) *(.gcc_except_table.*) } > ram
    .data ALIGN (0x4) : { __ram_data_start = ABSOLUTE (.); *(.data*) *(.data1) *(.gnu.linkonce.d.*) . = ALIGN (4); KEEP(*( SORT (.ecos.table.*))) ; . = ALIGN (4); __init_array_start__ = ABSOLUTE (.); KEEP (*(SORT (.init_array.*))) KEEP (*(SORT (.init_array))) __init_array_end__ = ABSOLUTE (.); *(.dynamic) *(.sdata*) *(.gnu.linkonce.s.*) . = ALIGN (4); *(.2ram.*) } > ram __rom_data_start = LOADADDR (.data); __ram_data_end = .; PROVIDE (__ram_data_end = .); _edata = .; PROVIDE (edata = .); PROVIDE (__rom_data_end = LOADADDR (.data) + SIZEOF(.data));
    .bss ALIGN (0x4) : { __bss_start = ABSOLUTE (.); *(.scommon) *(.dynsbss) *(.sbss*) *(.gnu.linkonce.sb.*) *(.dynbss) *(.bss*) *(.gnu.linkonce.b.*) *(COMMON) __bss_end = ABSOLUTE (.); } > ram
    .no_init ALIGN (0x4) : { . = .; *(.no_init)} > ram
    __heap1 = ALIGN (0x8);
    .no_cache  0x23F00000 : { . = .; *(.no_cache)} > ram2
    . = ALIGN(4); _end = .; PROVIDE (end = .);
}
