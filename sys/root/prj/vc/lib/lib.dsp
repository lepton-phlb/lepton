# Microsoft Developer Studio Project File - Name="lib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=lib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "lib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "lib.mak" CFG="lib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "lib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "lib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "lib"
# PROP Scc_LocalPath "..\..\.."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "lib - Win32 Release"

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

!ELSEIF  "$(CFG)" == "lib - Win32 Debug"

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

# Name "lib - Win32 Release"
# Name "lib - Win32 Debug"
# Begin Group "libc"

# PROP Default_Filter ""
# Begin Group "ctype"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\lib\libc\ctype\ctype.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\lib\libc\ctype\ctype.h
# End Source File
# End Group
# Begin Group "unistd"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\lib\libc\unistd\getopt.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\lib\libc\unistd\getopt.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\lib\libc\unistd\unistd.c
# End Source File
# End Group
# Begin Group "stdio"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\lib\libc\stdio\printf.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\lib\libc\stdio\scanf.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\lib\libc\stdio\stdio.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\lib\libc\stdio\stdio.h
# End Source File
# End Group
# Begin Group "termios"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\lib\libc\termios\tcgetattr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\lib\libc\termios\tcsetattr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\lib\libc\termios\termios.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\lib\libc\termios\termios.h
# End Source File
# End Group
# Begin Group "misc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\lib\libc\misc\crc.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\lib\libc\misc\crc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\lib\libc\misc\dtostr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\lib\libc\misc\ftoa.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\lib\libc\misc\ftoa.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\lib\libc\misc\itoa.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\lib\libc\misc\ltostr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\lib\libc\misc\ltostr.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\lib\libc\misc\prsopt.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\lib\libc\misc\prsopt.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\lib\libc\misc\strto_l.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\lib\libc\misc\strto_ll.c

!IF  "$(CFG)" == "lib - Win32 Release"

!ELSEIF  "$(CFG)" == "lib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "string"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\lib\libc\string\string.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\lib\libc\string\string.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\src\lib\libc\libc.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\lib\libc\libc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\lib\libc\stdint.h
# End Source File
# End Group
# Begin Group "pthread"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\lib\pthread\pthread.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\lib\pthread\pthread.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\lib\pthread\pthread_cond.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\lib\pthread\pthread_mutex.c
# End Source File
# End Group
# Begin Group "librt"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\lib\librt\mq.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\lib\librt\mqueue.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\lib\librt\sem.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\lib\librt\semaphore.h
# End Source File
# End Group
# End Target
# End Project
