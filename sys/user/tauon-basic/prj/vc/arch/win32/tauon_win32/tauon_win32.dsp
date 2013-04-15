# Microsoft Developer Studio Project File - Name="tauon_win32" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=tauon_win32 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "tauon_win32.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "tauon_win32.mak" CFG="tauon_win32 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "tauon_win32 - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "tauon_win32 - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "tauon_win32"
# PROP Scc_LocalPath "..\..\..\..\..\..\.."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "tauon_win32 - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "tauon_win32 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /Zp1 /MDd /W3 /Gm /GX /ZI /Od /I "c:/tauon/sys/root/src" /I "c:/tauon/sys/root/src/net/lwip" /I "c:/tauon/sys/root/src/kernel/net/lwip/include" /I "c:/tauon/sys/root/src/kernel/net/lwip/include/ipv4" /I "c:/tauon/sys/root/src/kernel/net/lwip/ports/win32" /I "c:/tauon/sys/root/src/kernel/net/lwip/ports/win32/include" /I "C:/tauon/sys/root/src/kernel/net/uip2.5" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 Winmm.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "tauon_win32 - Win32 Release"
# Name "tauon_win32 - Win32 Debug"
# Begin Group "tauon"

# PROP Default_Filter ""
# Begin Group "kernel"

# PROP Default_Filter ""
# Begin Group "core"

# PROP Default_Filter ""
# Begin Group "ucore"

# PROP Default_Filter ""
# Begin Group "embOSWIN32_100"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\..\..\..\root\src\kernel\core\ucore\embOSW32_100\main.c
# End Source File
# End Group
# End Group
# Begin Group "arch"

# PROP Default_Filter ""
# Begin Group "win32"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\..\..\..\root\src\kernel\core\arch\win32\bin_mkconf.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\..\root\src\kernel\core\arch\win32\dev_dskimg.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\..\root\src\kernel\core\arch\win32\dev_dskimg.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\..\root\src\kernel\core\arch\win32\dev_mkconf.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\..\root\src\kernel\core\arch\win32\kernel_mkconf.h
# End Source File
# End Group
# End Group
# End Group
# End Group
# End Group
# Begin Group "sys"

# PROP Default_Filter ""
# Begin Group "root"

# PROP Default_Filter ""
# Begin Group "bin "

# PROP Default_Filter ""
# Begin Group "net"

# PROP Default_Filter ""
# Begin Group "shttpd"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\..\..\..\root\src\bin\net\shttpd\auth.c

!IF  "$(CFG)" == "tauon_win32 - Win32 Release"

!ELSEIF  "$(CFG)" == "tauon_win32 - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\..\root\src\bin\net\shttpd\cgi.c

!IF  "$(CFG)" == "tauon_win32 - Win32 Release"

!ELSEIF  "$(CFG)" == "tauon_win32 - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\..\root\src\bin\net\shttpd\compat_rtems.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\..\root\src\bin\net\shttpd\compat_rtems.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\..\root\src\bin\net\shttpd\compat_unix.c

!IF  "$(CFG)" == "tauon_win32 - Win32 Release"

!ELSEIF  "$(CFG)" == "tauon_win32 - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\..\root\src\bin\net\shttpd\compat_unix.h

!IF  "$(CFG)" == "tauon_win32 - Win32 Release"

!ELSEIF  "$(CFG)" == "tauon_win32 - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\..\root\src\bin\net\shttpd\compat_win32.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\..\root\src\bin\net\shttpd\compat_win32.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\..\root\src\bin\net\shttpd\compat_wince.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\..\root\src\bin\net\shttpd\compat_wince.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\..\root\src\bin\net\shttpd\config.h

!IF  "$(CFG)" == "tauon_win32 - Win32 Release"

!ELSEIF  "$(CFG)" == "tauon_win32 - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\..\root\src\bin\net\shttpd\defs.h

!IF  "$(CFG)" == "tauon_win32 - Win32 Release"

!ELSEIF  "$(CFG)" == "tauon_win32 - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\..\root\src\bin\net\shttpd\io.h

!IF  "$(CFG)" == "tauon_win32 - Win32 Release"

!ELSEIF  "$(CFG)" == "tauon_win32 - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\..\root\src\bin\net\shttpd\io_cgi.c

!IF  "$(CFG)" == "tauon_win32 - Win32 Release"

!ELSEIF  "$(CFG)" == "tauon_win32 - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\..\root\src\bin\net\shttpd\io_dir.c

!IF  "$(CFG)" == "tauon_win32 - Win32 Release"

!ELSEIF  "$(CFG)" == "tauon_win32 - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\..\root\src\bin\net\shttpd\io_emb.c

!IF  "$(CFG)" == "tauon_win32 - Win32 Release"

!ELSEIF  "$(CFG)" == "tauon_win32 - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\..\root\src\bin\net\shttpd\io_file.c

!IF  "$(CFG)" == "tauon_win32 - Win32 Release"

!ELSEIF  "$(CFG)" == "tauon_win32 - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\..\root\src\bin\net\shttpd\io_socket.c

!IF  "$(CFG)" == "tauon_win32 - Win32 Release"

!ELSEIF  "$(CFG)" == "tauon_win32 - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\..\root\src\bin\net\shttpd\io_ssi.c

!IF  "$(CFG)" == "tauon_win32 - Win32 Release"

!ELSEIF  "$(CFG)" == "tauon_win32 - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\..\root\src\bin\net\shttpd\io_ssl.c

!IF  "$(CFG)" == "tauon_win32 - Win32 Release"

!ELSEIF  "$(CFG)" == "tauon_win32 - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\..\root\src\bin\net\shttpd\llist.h

!IF  "$(CFG)" == "tauon_win32 - Win32 Release"

!ELSEIF  "$(CFG)" == "tauon_win32 - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\..\root\src\bin\net\shttpd\log.c

!IF  "$(CFG)" == "tauon_win32 - Win32 Release"

!ELSEIF  "$(CFG)" == "tauon_win32 - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\..\root\src\bin\net\shttpd\md5.c

!IF  "$(CFG)" == "tauon_win32 - Win32 Release"

!ELSEIF  "$(CFG)" == "tauon_win32 - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\..\root\src\bin\net\shttpd\md5.h

!IF  "$(CFG)" == "tauon_win32 - Win32 Release"

!ELSEIF  "$(CFG)" == "tauon_win32 - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\..\root\src\bin\net\shttpd\shttpd.c

!IF  "$(CFG)" == "tauon_win32 - Win32 Release"

!ELSEIF  "$(CFG)" == "tauon_win32 - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\..\root\src\bin\net\shttpd\shttpd.h

!IF  "$(CFG)" == "tauon_win32 - Win32 Release"

!ELSEIF  "$(CFG)" == "tauon_win32 - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\..\root\src\bin\net\shttpd\ssl.h

!IF  "$(CFG)" == "tauon_win32 - Win32 Release"

!ELSEIF  "$(CFG)" == "tauon_win32 - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\..\root\src\bin\net\shttpd\standalone.c

!IF  "$(CFG)" == "tauon_win32 - Win32 Release"

!ELSEIF  "$(CFG)" == "tauon_win32 - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\..\root\src\bin\net\shttpd\std_includes.h

!IF  "$(CFG)" == "tauon_win32 - Win32 Release"

!ELSEIF  "$(CFG)" == "tauon_win32 - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\..\root\src\bin\net\shttpd\string.c

!IF  "$(CFG)" == "tauon_win32 - Win32 Release"

!ELSEIF  "$(CFG)" == "tauon_win32 - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "mongoose"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\..\..\..\root\src\bin\net\mongoose\mongoose.c

!IF  "$(CFG)" == "tauon_win32 - Win32 Release"

!ELSEIF  "$(CFG)" == "tauon_win32 - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\..\root\src\bin\net\mongoose\mongoose.h

!IF  "$(CFG)" == "tauon_win32 - Win32 Release"

!ELSEIF  "$(CFG)" == "tauon_win32 - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\..\root\src\bin\net\mongoose\mongoosed.c

!IF  "$(CFG)" == "tauon_win32 - Win32 Release"

!ELSEIF  "$(CFG)" == "tauon_win32 - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "ftpd"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\..\..\..\root\src\bin\net\ftpd\ftpd.c

!IF  "$(CFG)" == "tauon_win32 - Win32 Release"

!ELSEIF  "$(CFG)" == "tauon_win32 - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\..\root\src\bin\net\ftpd\ftpd.h

!IF  "$(CFG)" == "tauon_win32 - Win32 Release"

!ELSEIF  "$(CFG)" == "tauon_win32 - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\..\root\src\bin\net\ftpd\ftpd_priv.h

!IF  "$(CFG)" == "tauon_win32 - Win32 Release"

!ELSEIF  "$(CFG)" == "tauon_win32 - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\..\root\src\bin\net\ftpd\ls.c

!IF  "$(CFG)" == "tauon_win32 - Win32 Release"

!ELSEIF  "$(CFG)" == "tauon_win32 - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\..\..\..\..\root\src\bin\net\lftpd.c

!IF  "$(CFG)" == "tauon_win32 - Win32 Release"

!ELSEIF  "$(CFG)" == "tauon_win32 - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\..\root\src\bin\net\telnetd.c

!IF  "$(CFG)" == "tauon_win32 - Win32 Release"

!ELSEIF  "$(CFG)" == "tauon_win32 - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "tst"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\..\..\..\root\src\bin\net\tstip.c

!IF  "$(CFG)" == "tauon_win32 - Win32 Release"

!ELSEIF  "$(CFG)" == "tauon_win32 - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\..\..\..\..\root\src\bin\test2.c

!IF  "$(CFG)" == "tauon_win32 - Win32 Release"

!ELSEIF  "$(CFG)" == "tauon_win32 - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\..\root\src\bin\udpsrvd.c
# End Source File
# End Group
# End Group
# Begin Group "user"

# PROP Default_Filter ""
# Begin Group "dev"

# PROP Default_Filter ""
# End Group
# Begin Group "bin"

# PROP Default_Filter ""
# Begin Group "net "

# PROP Default_Filter ""
# Begin Group "html"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\..\src\bin\net\html\about.html
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\src\bin\net\html\index.html
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\src\bin\net\html\lepton.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\src\bin\net\html\lepton.png
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\src\bin\net\html\tstcgi2.html
# End Source File
# End Group
# Begin Group "cgi-bin"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\..\..\..\src\bin\net\cgi-bin\tstcgi.sh"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\src\bin\net\cgi-bin\tstcgi2.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\src\bin\net\cgi-bin\tstcgi2.sh"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\src\bin\net\cgi-bin\tstpost.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\src\bin\net\cgi-bin\tstpost.sh"
# End Source File
# End Group
# End Group
# End Group
# End Group
# End Group
# Begin Group "etc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\..\etc\mkconf_tauon_basic.xml
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\..\..\..\..\root\prj\vc\kernel\Debug\tauon.lib
# End Source File
# End Target
# End Project
