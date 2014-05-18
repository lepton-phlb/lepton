# Microsoft Developer Studio Project File - Name="kernel" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=kernel - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "kernel.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "kernel.mak" CFG="kernel - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "kernel - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "kernel - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "lepton"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "kernel - Win32 Release"

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

!ELSEIF  "$(CFG)" == "kernel - Win32 Debug"

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
# ADD CPP /nologo /Zp1 /MDd /W3 /Gm /GX /ZI /Od /I "c:/tauon/sys/root/src" /I "c:/tauon/sys/root/src/net/lwip" /I "c:/tauon/sys/root/src/kernel/net/lwip/include" /I "c:/tauon/sys/root/src/kernel/net/lwip/include/ipv4" /I "c:/tauon/sys/root/src/kernel/net/lwip/ports/win32" /I "c:/tauon/sys/root/src/kernel/net/lwip/ports/win32/include" /D "_LIB" /D "WIN32" /D "_DEBUG" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Debug\tauon.lib"

!ENDIF 

# Begin Target

# Name "kernel - Win32 Release"
# Name "kernel - Win32 Debug"
# Begin Group "kernel"

# PROP Default_Filter ""
# Begin Group "core"

# PROP Default_Filter ""
# Begin Group "arch"

# PROP Default_Filter ""
# Begin Group "win32"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\kernel\core\arch\win32\dev_dskimg.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\arch\win32\kernel_mkconf.h
# End Source File
# End Group
# End Group
# Begin Group "net"

# PROP Default_Filter ""
# Begin Group "lwip_core"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\kernel\core\net\lwip_core\ethif_core.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\net\lwip_core\lwip_core.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\net\lwip_core\lwip_core.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\net\lwip_core\lwip_socket.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\net\lwip_core\lwip_socket.h
# End Source File
# End Group
# Begin Group "uip_core"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\kernel\core\net\uip_core\uip_core.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\net\uip_core\uip_core.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\net\uip_core\uip_slip.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\net\uip_core\uip_slip.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\net\uip_core\uip_sock.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\net\uip_core\uip_sock.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\net\uip_core\uip_socket.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\net\uip_core\uip_socket.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\src\kernel\core\net\socks.h
# End Source File
# End Group
# Begin Group "sys"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\kernel\core\sys\mqueue.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\sys\pthread.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\sys\semaphore.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\sys\siginfo.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\sys\sysctl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\sys\utsname.h
# End Source File
# End Group
# Begin Group "ucore"

# PROP Default_Filter ""
# Begin Group "embOSW32_100"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\kernel\core\ucore\embOSW32_100\segger_intr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\ucore\embOSW32_100\segger_intr.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\ucore\embOSW32_100\seggercsemaphore.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\ucore\embOSW32_100\seggerevent.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\ucore\embOSW32_100\seggerinterrupt.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\ucore\embOSW32_100\seggermailbox.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\ucore\embOSW32_100\seggersemaphore.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\ucore\embOSW32_100\seggersofttimer.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\ucore\embOSW32_100\seggertask.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\ucore\embOSW32_100\seggerwin32.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\ucore\embOSW32_100\seggerwin32.h
# End Source File
# End Group
# End Group
# Begin Group "core-segger"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\..\src\kernel\core\core-segger\core_rttimer.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\core\core-segger\fork.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\core\core-segger\kernel.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\core\core-segger\kernel_clock.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\core\core-segger\kernel_elfloader.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\core\core-segger\kernel_object.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\core\core-segger\kernel_pthread.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\core\core-segger\kernel_pthread_mutex.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\core\core-segger\kernel_sem.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\core\core-segger\kernel_sigqueue.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\core\core-segger\kernel_timer.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\core\core-segger\process.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\core\core-segger\signal.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\core\core-segger\syscall.c"
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\src\kernel\core\bin.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\bin.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\board.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\core_rttimer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\cpu.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\cpu.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\devconf.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\devio.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\devio.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\dirent.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\dirent.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\env.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\env.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\errno.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\etypes.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\fcntl.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\fcntl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\flock.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\flock.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\fork.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\interrupt.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\io.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\ioctl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\ioctl_board.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\ioctl_cpu.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\ioctl_eth.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\ioctl_hd.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\ioctl_if.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\ioctl_keyb.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\ioctl_lcd.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\ioctl_proc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\ioctl_pwr.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\kal.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\kal.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\kernel.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\kernel_clock.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\kernel_elfloader.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\kernel_object.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\kernel_pthread.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\kernel_pthread_mutex.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\kernel_sem.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\kernel_sigqueue.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\kernel_timer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\kernelconf.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\lib.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\lib.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\libstd.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\malloc.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\malloc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\pipe.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\pipe.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\posix_mqueue.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\posix_mqueue.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\process.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\rttimer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\select.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\select.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\signal.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\stat.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\stat.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\statvfs.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\statvfs.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\stropts.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\syscall.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\sysctl.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\sysctl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\system.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\system.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\systime.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\systime.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\time.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\time.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\timer.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\timer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\truncate.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\types.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\ver.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\wait.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\wait.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\windef.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\windows.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\core\winnt.h
# End Source File
# End Group
# End Group
# Begin Source File

SOURCE="..\..\..\..\..\change-log.txt"
# End Source File
# End Target
# End Project
