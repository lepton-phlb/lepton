#connect JLink (Modifie) (OK)
target remote localhost:3333

#for ignoring packet errors...
set remotetimeout 1000

# Set GDB in little-endian (OK)
set endian little

monitor reset
monitor sleep 100
monitor halt

####

#MCG_C2 (RANGE(1) | HGO | EREFS)
monitor mwb 0x40064001   0x20
monitor mdb 0x40064006
monitor sleep 5

#MCG_C1 (CLKS(2)|FRDIV(3))
monitor mwb 0x40064000   0xa8
monitor sleep 10
monitor mdb 0x40064006

#SIM_CLKDIV1
monitor mww 0x40048044  0x01130000
monitor sleep 1
monitor mdb 0x40064006

#SIM_CLKDIV2
monitor mwb 0x40048048  0x02
monitor sleep 1
monitor mdb 0x40064006

#MCG_C5 (0x18|0x20)
#monitor mwb 0x40064004  0x38
#monitor mwb 0x40064004  0x36
monitor mwb 0x40064004  0x37
monitor sleep 10
monitor mdb 0x40064006

#MCG_C6 (0x18|0x40)
monitor mwb 0x40064005  0x58
monitor sleep 10
monitor mdb 0x40064006

#reconfigure MCG_C1
monitor mwb 0x40064000   0x28
monitor sleep 1
monitor mdb 0x40006406

#enable clocking for PLL
monitor mww 0x40048004  0x11000
monitor mdb 0x40064006

echo Configuring clocks done...\n
##

#configure MRAM
#Chip Select 0 config
#FB_CSAR0
monitor mww 0x4000c000  0x60000000

#FB_CSCR0
monitor mww 0x4000c008  0x00100540

#FB_CSMR0
monitor mww 0x4000c004  0x00070001

#FB_CSPMCR
#monitor mww 0x4000c060  0x02200000

#SIM_CLKDIV1
monitor mww 0x40048044  0x01330000

#IO config
#address
#PORTB (11) alt. config 5
monitor mww 0x4004a02c  0x00000500
#PORTB (16) alt. config 5
monitor mww 0x4004a040  0x00000500
#PORTB (17) alt. config 5
monitor mww 0x4004a044  0x00000500
#PORTB (18) alt. config 5
monitor mww 0x4004a048  0x00000500
#PORTC (0) alt. config 5
monitor mww 0x4004b000  0x00000500
#PORTC (1) alt. config 5
monitor mww 0x4004b004  0x00000500
#PORTC (2) alt. config 5
monitor mww 0x4004b008  0x00000500
#PORTC (4) alt. config 5
monitor mww 0x4004b010  0x00000500
#PORTC (5) alt. config 5
monitor mww 0x4004b014  0x00000500
#PORTC (6) alt. config 5
monitor mww 0x4004b018  0x00000500
#PORTC (7) alt. config 5
monitor mww 0x4004b01c  0x00000500
#PORTC (8) alt. config 5
monitor mww 0x4004b020  0x00000500
#PORTC (9) alt. config 5
monitor mww 0x4004b024  0x00000500
#PORTC (10) alt. config 5
monitor mww 0x4004b028  0x00000500
#PORTD (2) alt. config 5
monitor mww 0x4004c008  0x00000500
#PORTD (3) alt. config 5
monitor mww 0x4004c00c  0x00000500
#PORTD (4) alt. config 5
monitor mww 0x4004c010  0x00000500
#PORTD (5) alt. config 5
monitor mww 0x4004c014  0x00000500
#PORTD (6) alt. config 5
monitor mww 0x4004c018  0x00000500

#data
#PORTB (20) alt. config 5
monitor mww 0x4004a050  0x00000500
#PORTB (21) alt. config 5
monitor mww 0x4004a054  0x00000500
#PORTB (22) alt. config 5
monitor mww 0x4004a058  0x00000500
#PORTB (23) alt. config 5
monitor mww 0x4004a05c  0x00000500
#PORTC (12) alt. config 5
monitor mww 0x4004b030  0x00000500
#PORTC (13) alt. config 5
monitor mww 0x4004b034  0x00000500
#PORTC (14) alt. config 5
monitor mww 0x4004b038  0x00000500
#PORTC (15) alt. config 5
monitor mww 0x4004b03c  0x00000500

#control signals
#PORTB (19) alt. config 5
monitor mww 0x4004a04c  0x00000500
#PORTC (11) alt. config 5
monitor mww 0x4004b02c  0x00000500
#PORTD (1) alt. config 5
monitor mww 0x4004c004  0x00000500
#PORTD (0) alt. config 5
monitor mww 0x4004c000  0x00000500
#
echo Configuring CS0 for MRAM done...\n

# Increase JTAG Speed
monitor jtag_khz 8000

echo Configuring JTAG fast done...\n

#for ignoring packet errors...
set remotetimeout 1000

#Enable softbreak point 
monitor gdb_breakpoint_override	soft

load
