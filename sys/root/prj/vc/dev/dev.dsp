# Microsoft Developer Studio Project File - Name="dev" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=dev - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "dev.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dev.mak" CFG="dev - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dev - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "dev - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "dev"
# PROP Scc_LocalPath "..\..\..\..\.."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "dev - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "dev - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /Zp1 /MDd /W3 /Gm /GX /ZI /Od /I "c:/tauon/sys/root/src" /I "c:/tauon/tools/host/win32" /D "_LIB" /D "WIN32" /D "_DEBUG" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "dev - Win32 Release"
# Name "dev - Win32 Debug"
# Begin Group "kernel"

# PROP Default_Filter ""
# Begin Group "dev"

# PROP Default_Filter ""
# Begin Group "arch"

# PROP Default_Filter ""
# Begin Group "all"

# PROP Default_Filter ""
# Begin Group "flash"

# PROP Default_Filter ""
# Begin Group "amd"

# PROP Default_Filter ""
# Begin Group "lldapi"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\arch\all\flash\amd\lldapi\lld.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\arch\all\flash\amd\lldapi\lld.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\arch\all\flash\amd\lldapi\lld_bdsxxx_asp.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\arch\all\flash\amd\lldapi\lld_bdsxxx_asp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\arch\all\flash\amd\lldapi\lld_target_specific.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\arch\all\flash\amd\lldapi\trace.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\arch\all\flash\amd\lldapi\trace.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\arch\all\flash\amd\dev_flash_am29dlxxxx_1.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\arch\all\flash\amd\dev_flash_am29dlxxxx_2.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\arch\all\flash\amd\dev_flash_am29dlxxxx_3.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\arch\all\flash\amd\dev_flash_nor_amd.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\arch\all\flash\amd\dev_flash_nor_amd.h
# End Source File
# End Group
# Begin Group "dev_ftl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\arch\all\flash\dev_ftl\dev_ftl.c
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\arch\all\flash\flash.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\arch\all\flash\flash.h
# End Source File
# End Group
# Begin Group "i2c"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\arch\all\i2c\dev_eeprom_24xxx.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\arch\all\i2c\dev_eeprom_24xxx.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\arch\all\i2c\dev_eeprom_24xxx_0.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\arch\all\i2c\dev_eeprom_24xxx_0.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\arch\all\i2c\dev_eeprom_24xxx_1.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\arch\all\i2c\dev_eeprom_24xxx_1.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\arch\all\i2c\dev_eeprom_24xxx_2.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\arch\all\i2c\dev_eeprom_24xxx_2.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\arch\all\i2c\dev_rtc_m41t81.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\arch\all\i2c\dev_rtc_m41t81.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\arch\all\i2c\dev_rtc_x1203.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\arch\all\i2c\dev_rtc_x1203.h
# End Source File
# End Group
# Begin Group "lcd"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\arch\all\lcd\dev_lcd_jrc_nju6433.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\arch\all\lcd\dev_lcd_jrc_nju6433.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\arch\all\lcd\dev_lcd_sanyo_lc75813.c
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "sdcard"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\arch\all\sdcard\dev_sdcard.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\arch\all\sdcard\dev_sdcard.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\arch\all\sdcard\drv_sdcard.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\arch\all\sdcard\drv_sdcard.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\arch\all\sdcard\drv_sdcard_arch.h
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "slip"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\arch\all\slip\dev_slip.c
# End Source File
# End Group
# Begin Group "eth"

# PROP Default_Filter ""
# Begin Group "dev_eth_dm9000a"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\arch\all\eth\dev_eth_dm9000a\dev_eth_dm9000a.c
# PROP Exclude_From_Build 1
# End Source File
# End Group
# End Group
# End Group
# Begin Group "win32"

# PROP Default_Filter ""
# Begin Group "dev_win32_board"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\arch\win32\dev_win32_board\dev_win32_board.c
# End Source File
# End Group
# Begin Group "dev_win32_com0"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\arch\win32\dev_win32_com0\dev_socketsrv_0.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\arch\win32\dev_win32_com0\dev_socketsrv_0.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\arch\win32\dev_win32_com0\dev_win32_com0.c
# End Source File
# End Group
# Begin Group "dev_win32_com1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\arch\win32\dev_win32_com1\dev_win32_com1.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\arch\win32\dev_win32_com1\dev_win32uart2.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\arch\win32\dev_win32_com1\dev_win32uart2.h
# End Source File
# End Group
# Begin Group "dev_win32_com2"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\arch\win32\dev_win32_com2\dev_win32_com2.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\arch\win32\dev_win32_com2\dev_win32uart.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\arch\win32\dev_win32_com2\dev_win32uart.h
# End Source File
# End Group
# Begin Group "dev_win32_eth"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\arch\win32\dev_win32_eth\dev_win32_eth.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\arch\win32\dev_win32_eth\win32_core_pkt.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\arch\win32\dev_win32_eth\win32_core_pkt.h
# End Source File
# End Group
# Begin Group "dev_win32_fileflash"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\arch\win32\dev_win32_fileflash\dev_win32_fileflash.c
# End Source File
# End Group
# Begin Group "dev_win32_filerom"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\arch\win32\dev_win32_filerom\dev_win32_filerom.c
# End Source File
# End Group
# Begin Group "dev_win32_flash"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\arch\win32\dev_win32_flash\dev_win32_flash.c
# End Source File
# End Group
# Begin Group "dev_win32_sdcard"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\arch\win32\dev_win32_sdcard\dev_win32_sdcard.c
# End Source File
# End Group
# Begin Group "dev_win32_rtc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\arch\win32\dev_win32_rtc\dev_win32_rtc.c
# End Source File
# End Group
# Begin Group "dev_win32_kb"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\arch\win32\dev_win32_kb\dev_win32_kb.c
# End Source File
# End Group
# Begin Group "dev_win32_lcd"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\arch\win32\dev_win32_lcd\dev_win32_lcd.c
# End Source File
# End Group
# Begin Group "dev_win32_lcd_vga"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\arch\win32\dev_win32_lcd_vga\dev_win32_lcd_vga.c
# End Source File
# End Group
# End Group
# End Group
# Begin Group "dev_cpufs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\dev_cpufs\dev_cpufs.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\dev_cpufs\dev_cpufs.h
# End Source File
# End Group
# Begin Group "dev_null"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\dev_null\dev_null.c
# End Source File
# End Group
# Begin Group "dev_proc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\dev_proc\dev_proc.c
# End Source File
# End Group
# Begin Group "dev_tty"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\dev_tty\dev_tty.c
# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\dev\dev_tty\tty_font-8x16.c"
# End Source File
# End Group
# Begin Group "dev_head"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\kernel\dev\dev_head\dev_head.c
# End Source File
# End Group
# End Group
# End Group
# Begin Source File

SOURCE=..\..\..\..\..\tools\host\win32\WpdPack_3_1\WpdPack\Lib\Packet.lib
# End Source File
# End Target
# End Project
