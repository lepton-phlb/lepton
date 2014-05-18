# Microsoft Developer Studio Project File - Name="sbin" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=sbin - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "sbin.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "sbin.mak" CFG="sbin - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "sbin - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "sbin - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "sbin"
# PROP Scc_LocalPath "..\..\.."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "sbin - Win32 Release"

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

!ELSEIF  "$(CFG)" == "sbin - Win32 Debug"

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
# ADD CPP /nologo /Zp1 /MDd /W3 /Gm /GX /ZI /Od /I "c:/tauon/sys/root/src" /D "_LIB" /D "WIN32" /D "_DEBUG" /D "_MBCS" /YX /FD /GZ /c
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

# Name "sbin - Win32 Release"
# Name "sbin - Win32 Debug"
# Begin Group "sbin"

# PROP Default_Filter ""
# Begin Group "sh"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\sbin\sh\mkhd
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sbin\sh\mkhdb
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sbin\sh\mkhdc
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sbin\sh\prod
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sbin\sh\shutdown
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sbin\sh\unprod
# End Source File
# End Group
# Begin Group "net"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\sbin\net\ifconfig.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sbin\net\inetd.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sbin\net\slipd.c
# End Source File
# End Group
# Begin Group "lp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\sbin\lp\lpd.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sbin\lp\lpr.c
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\src\sbin\btb.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sbin\cat.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sbin\compress.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sbin\cp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sbin\date.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sbin\df.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sbin\ecat.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sbin\echo.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sbin\initd.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sbin\kill.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sbin\lprintf.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sbin\ls.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sbin\lsh.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sbin\mkdir.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sbin\mkfifo.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sbin\mkfs.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sbin\more.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sbin\mount.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sbin\mv.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sbin\od.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sbin\ps.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sbin\pwd.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sbin\rm.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sbin\rmdir.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sbin\shutdown.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sbin\sleep.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sbin\stty.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sbin\sync.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sbin\touch.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sbin\umount.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sbin\uname.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sbin\wrapr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sbin\xmodem.c
# End Source File
# End Group
# End Target
# End Project
