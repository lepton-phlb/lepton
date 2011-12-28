STARTUP(vectors.o)
ENTRY(_start)
INPUT(extras.o)
GROUP(libtarget.a libgcc.a libsupc++.a)



MEMORY
{
    rom : ORIGIN = 0x01000000, LENGTH = 0x1000000
    ram : ORIGIN = 0x02000000, LENGTH = 0x1000000
}

SECTIONS
{
   
    .vectors 0x01000000 : { . = .; KEEP(*(.vectors)) } > rom
/*Add*/
    .interp ALIGN (0x4) : { *(.interp) } > rom
    .dynsym ALIGN (0x4) : { *(.dynsym) } > rom
    .dynstr ALIGN (0x4) : { *(.dynstr) } > rom
/*End add*/
    .text ALIGN (0x4) : { _stext = .; *(.text*) *(.gnu.warning) *(.gnu.linkonce.t.*) *(.init) } > rom _etext = .; PROVIDE (etext = .);
    .fini ALIGN (0x4) : { . = .; *(.fini) } > rom
    .rodata1 ALIGN (0x8) : { . = .; *(.rodata1) } > rom
    .rodata ALIGN (0x8) : { . = .; *(.rodata*) *(.gnu.linkonce.r.*) } > rom
    .fixup ALIGN (0x4) : { _FIXUP_START_ = ABSOLUTE(.); *(.fixup) _FIXUP_END_ = ABSOLUTE(.);} > rom
    .rel.text : { *(.rel.text) *(.rel.text.*) *(.rel.gnu.linkonce.t*) } > rom .rela.text : { *(.rela.text) *(.rela.text.*) *(.rela.gnu.linkonce.t*) } > rom .rel.data : { *(.rel.data) *(.rel.data.*) *(.rel.gnu.linkonce.d*) } > rom .rela.data : { *(.rela.data) *(.rela.data.*) *(.rela.gnu.linkonce.d*) } > rom .rel.rodata : { *(.rel.rodata) *(.rel.rodata.*) *(.rel.gnu.linkonce.r*) } > rom .rela.rodata : { *(.rela.rodata) *(.rela.rodata.*) *(.rela.gnu.linkonce.r*) } > rom .rel.got : { *(.rel.got) } > rom .rela.got : { *(.rela.got) } > rom .rel.ctors : { *(.rel.ctors) } > rom .rela.ctors : { *(.rela.ctors) } > rom .rel.dtors : { *(.rel.dtors) } > rom .rela.dtors : { *(.rela.dtors) } > rom .rel.init : { *(.rel.init) } > rom .rela.init : { *(.rela.init) } > rom .rel.fini : { *(.rel.fini) } > rom .rela.fini : { *(.rela.fini) } > rom .rel.bss : { *(.rel.bss) } > rom .rela.bss : { *(.rela.bss) } > rom .rel.plt : { *(.rel.plt) } > rom .rela.plt : { *(.rela.plt) } > rom .rel.dyn : { *(.rel.dyn) } > rom
    .eh_frame ALIGN (0x4) : { . = .; __EH_FRAME_BEGIN__ = .; KEEP(*(.eh_frame*)) __FRAME_END__ = .; . = . + 8; } > rom = 0
    .rel.got ALIGN (0x1) : { *(.rel.got) } > rom
    /*Ajout*/
    .gnu.hash ALIGN (0x1) : { *(.gnu.hash) } > rom
    .note.gnu.build-id ALIGN (0x1) : { *(.note.gnu.build-id) } > rom
    /*End ajout*/
    .gcc_except_table ALIGN (0x1) : { _EXCEPT_START_ = ABSOLUTE(.); *(.gcc_except_table.*) _EXCEPT_END_ = ABSOLUTE(.);} > rom
    /*Ajout*/
    .tbss		  : { *(.tbss .tbss.* .gnu.linkonce.tb.*) *(.tcommon) }
    /*End ajout*/
    .data 0x02000000 : { __ram_data_start = ABSOLUTE(.); *(.data*) *(.gnu.linkonce.d.*) _GOT1_START_ = ABSOLUTE(.); *(.got1) _GOT1_END_ = ABSOLUTE(.); . = ALIGN(8); __CTOR_LIST__ = ABSOLUTE(.); KEEP(*(SORT(.ctors*))) __CTOR_END__ = ABSOLUTE(.); __DTOR_LIST__ = ABSOLUTE(.); KEEP(*(SORT(.dtors*))) __DTOR_END__ = ABSOLUTE(.); . = ALIGN(32); KEEP(*( SORT (.ecos.table.*))); _GOT2_START_ = ABSOLUTE(.); *(.got2) _GOT2_END_ = ABSOLUTE(.); _GOT_START_ = ABSOLUTE(.); _GLOBAL_OFFSET_TABLE_ = ABSOLUTE(. + 32768); _SDA_BASE_ = ABSOLUTE(.); *(.got.plt) *(.got) _GOT_END_ = ABSOLUTE(.); *(.dynamic) _SDATA_START_ = ABSOLUTE(.); *(.sdata*) *(.gnu.linkonce.s.*) } > ram __rom_data_start = LOADADDR(.data); __ram_data_end = .; PROVIDE(__ram_data_end = .); _edata = .; PROVIDE (edata = .);
    .sbss ALIGN (0x4) : { __bss_start = ABSOLUTE (.); _SBSS_START_ = ABSOLUTE(.); *(.sbss*) *(.gnu.linkonce.sb.*) _SBSS_END_ = ABSOLUTE(.); *(.scommon*) } > ram
    .bss ALIGN (0x10) : { . = .; *(.dynbss*) *(.bss*) *(COMMON) *(.gnu.linkonce.b.*) } > ram __bss_end = .;
   /*add*/
/* Thread Local Storage sections  */
  /*.tdata	  : { *(.tdata .tdata.* .gnu.linkonce.td.*) }*/
  /*end add*/
   __heap1 = ALIGN (0x10);
    .synth_heap __heap1 : { . = .; . = 0x03000000 - __heap1 ; } > ram
    . = ALIGN(4); _end = .; PROVIDE (end = .);
}
