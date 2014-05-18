#connect JLink (Modifie) (OK)
target remote localhost:3333
#for ignoring packet errors...
set remotetimeout 1000

# Set GDB in little-endian (OK)
set endian little

monitor halt
monitor soft_reset_halt
monitor halt

#User RESET init
monitor mww	0xfffffd08	0xa5000001
#Watchdog disable
monitor mww	0xfffffd44	0x00008000

#PLL settings
monitor mww	0xfffffc20	0x00002001
monitor sleep 20
monitor mdw 0xfffffc68
monitor sleep 20
monitor mdw 0xfffffc68
monitor sleep 10

##PLLA settings
monitor mww     0xfffffc28      0x208c800d
monitor mdw 0xfffffc68
monitor sleep 50
monitor mdw 0xfffffc68
monitor sleep 50
##
monitor mww 0xfffffc30  0x00000102

#sleep 1 ms
monitor sleep 10
echo Configuring PLL done...\n

# Increase JTAG Speed to 6 MHz
#monitor jtag_rclk 32000
monitor jtag_rclk 0

#Init SDRAM
#Activate CS1 for SDRAM
monitor mww	0xffffee30	0x00000002
#Upper 16 lines are data lines for SDRAM
monitor mww	0xfffff870	0xffff0000
#Disable PIO for upper 16 bits
monitor mww	0xfffff804	0xffff0000
#Initialize SDRAM controller
monitor mww	0xffffea08	0x85227259
monitor sleep 1
#
monitor mww	0xffffea00	0x00000002
monitor mdw 0x20000000	1
monitor sleep 1
#
monitor mww	0xffffea00	0x00000004
monitor mdw 0x20000000	1
monitor sleep 1
monitor mww	0xffffea00	0x00000004
monitor mdw 0x20000000	1
monitor sleep 1
monitor mww	0xffffea00	0x00000004
monitor mdw 0x20000000	1
monitor sleep 1
monitor mww	0xffffea00	0x00000004
monitor mdw 0x20000000	1
monitor sleep 1
monitor mww	0xffffea00	0x00000004
monitor mdw 0x20000000	1
monitor sleep 1
monitor mww	0xffffea00	0x00000004
monitor mdw 0x20000000	1
monitor sleep 1
monitor mww	0xffffea00	0x00000004
monitor mdw 0x20000000	1
monitor sleep 1
monitor mww	0xffffea00	0x00000004
monitor mdw 0x20000000	1
monitor sleep 1
#
monitor mww	0xffffea00	0x00000003
monitor mww	0x20000020	0xcafedede
monitor mww	0xffffea04	0x000002bf
monitor mww	0xffffea00	0x00000000
monitor mdw 0x20000000	1
echo Configuring SDRAM done...\n


#REMAP
echo REMAP done...\n

#AIC init
monitor mww 0xfffff124	0xffffffff
#
monitor sleep 1
monitor mdw	0xfffff130
monitor sleep 1
monitor mdw	0xfffff130
monitor mdw	0xfffff130
monitor sleep 1
monitor mdw	0xfffff130
monitor sleep 1
monitor mdw	0xfffff130
monitor sleep 1
monitor mdw	0xfffff130
monitor sleep 1
monitor mdw	0xfffff130
monitor sleep 1
monitor mdw	0xfffff130
echo Configuring AIC done...\n


# Set the processor mode
monitor reg cpsr 0xd3
#monitor reg pc 0x20000000

#for ignoring packet errors...
set remotetimeout 1000

#Enable softbreak point 
monitor gdb_breakpoint_override	soft

monitor	arm7_9 dcc_downloads enable
monitor arm7_9 fast_memory_access enable

load
