# Connect to openocd
target remote localhost:3333

#for ignoring packet errors...
set remotetimeout 1000

# prefer hardware breakpoint
monitor gdb_breakpoint_override hard

# reset board
monitor reset
monitor halt

# get flash info
#monitor flash info

# get register
monitor reg

# get binary to debug
symbol-file /home/shiby/tauon/sys/user/tauon_sampleapp/prj/scons/tauon_at91samd20.elf

# set bp
#hbreak Reset_Handler
hbreak HardFault_Handler
hbreak _start_kernel
#hbreak main
display xTotalHeapSize
display xFreeBytesRemaining
display xMinimumEverFreeBytesRemaining
display _dummy_initd_flag

c
