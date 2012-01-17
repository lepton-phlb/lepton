#connect JLink (Modifie) (OK)
target remote localhost:3333

#for ignoring packet errors...
#set remotetimeout 1000

# Set GDB in little-endian (OK)
set endian little

#Enable softbreak point 
#monitor gdb_breakpoint_override	soft

#symbol-file tauon_k60n512.elf

#k60 peripherals maps
source ../etc/scripts/gdb_k60_regs_map.gdb 
