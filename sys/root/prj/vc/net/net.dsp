# Microsoft Developer Studio Project File - Name="net" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=net - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "net.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "net.mak" CFG="net - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "net - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "net - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "net"
# PROP Scc_LocalPath "..\..\.."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "net - Win32 Release"

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

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

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
# ADD CPP /nologo /Zp1 /MDd /W3 /Gm /GX /ZI /Od /I "c:/tauon/sys/root/src" /I "c:/tauon/sys/root/src/net/lwip" /I "c:/tauon/sys/root/src/kernel/net/lwip/include" /I "c:/tauon/sys/root/src/kernel/net/lwip/include/ipv4" /I "c:/tauon/sys/root/src/kernel/net/lwip/ports/win32" /I "c:/tauon/sys/root/src/kernel/net/lwip/ports/win32/include" /I "C:/tauon/sys/root/src/kernel/net/uip2.5" /D "_LIB" /D "WIN32" /D "_DEBUG" /D "_MBCS" /YX /FD /GZ /c
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

# Name "net - Win32 Release"
# Name "net - Win32 Debug"
# Begin Group "kernel"

# PROP Default_Filter ""
# Begin Group "lwip"

# PROP Default_Filter ""
# Begin Group "api"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\api\api_lib.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\api\api_msg.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\api\err.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\api\netbuf.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\api\netdb.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\api\netifapi.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\api\tcpip.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "core"

# PROP Default_Filter ""
# Begin Group "ipv4"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\ipv4\autoip.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\ipv4\icmp.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\ipv4\igmp.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\ipv4\inet.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\ipv4\inet_chksum.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\ipv4\ip.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\ipv4\ip_addr.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\ipv4\ip_frag.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "ipv6"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\ipv6\icmp6.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\ipv6\inet6.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\ipv6\ip6.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\ipv6\ip6_addr.c
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "snmp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\snmp\asn1_dec.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\snmp\asn1_enc.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\snmp\mib2.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\snmp\mib_structs.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\snmp\msg_in.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\snmp\msg_out.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\dhcp.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\dns.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\init.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\mem.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\memp.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\netif.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\pbuf.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\raw.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\stats.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\sys.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\tcp.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\tcp_in.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\tcp_out.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\udp.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "ethif"

# PROP Default_Filter ""
# End Group
# Begin Group "netif"

# PROP Default_Filter ""
# Begin Group "ppp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\netif\ppp\auth.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\netif\ppp\auth.h

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\netif\ppp\chap.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\netif\ppp\chap.h

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\netif\ppp\chpms.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\netif\ppp\chpms.h

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\netif\ppp\fsm.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\netif\ppp\fsm.h

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\netif\ppp\ipcp.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\netif\ppp\ipcp.h

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\netif\ppp\lcp.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\netif\ppp\lcp.h

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\netif\ppp\magic.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\netif\ppp\magic.h

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\netif\ppp\md5.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\netif\ppp\md5.h

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\netif\ppp\pap.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\netif\ppp\pap.h

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\netif\ppp\ppp.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\netif\ppp\ppp.h

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\netif\ppp\pppdebug.h

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\netif\ppp\randm.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\netif\ppp\randm.h

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\netif\ppp\vj.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\netif\ppp\vj.h

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\netif\ppp\vjbsdhdr.h

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\netif\etharp.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\netif\ethernetif.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\netif\loopif.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\netif\slipif.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "ports"

# PROP Default_Filter ""
# Begin Group "win32"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\ports\win32\include\arch\bpstruct.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\ports\win32\include\arch\cc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\ports\win32\include\arch\epstruct.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\ports\win32\lwipopts.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\ports\win32\include\arch\perf.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\ports\win32\sys_arch.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\ports\win32\include\arch\sys_arch.h
# End Source File
# End Group
# Begin Group "arm"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\ports\arm\include\arch\bpstruct.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\ports\arm\include\arch\cc.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\ports\arm\include\arch\epstruct.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\ports\arm\lwipopts.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\ports\arm\include\arch\perf.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\ports\arm\sys_arch.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\ports\arm\include\arch\sys_arch.h
# PROP Exclude_From_Build 1
# End Source File
# End Group
# End Group
# Begin Group "include"

# PROP Default_Filter ""
# Begin Group " ipv4"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\include\ipv4\lwip\icmp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\include\ipv4\lwip\inet.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\include\ipv4\lwip\ip.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\include\ipv4\lwip\ip_addr.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\include\ipv4\lwip\ip_frag.h
# End Source File
# End Group
# Begin Group " ipv6"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\include\ipv6\lwip\icmp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\include\ipv6\lwip\inet.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\include\ipv6\lwip\ip.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\include\ipv6\lwip\ip_addr.h
# End Source File
# End Group
# Begin Group " netif"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\include\netif\etharp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\include\netif\loopif.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\include\netif\slipif.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\include\lwip\api.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\include\lwip\api_msg.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\include\lwip\arch.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\include\lwip\debug.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\include\lwip\def.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\include\lwip\dhcp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\include\lwip\err.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\include\lwip\mem.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\include\lwip\memp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\include\lwip\netif.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\include\lwip\opt.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\include\lwip\pbuf.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\include\lwip\raw.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\include\lwip\sio.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\include\lwip\snmp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\include\lwip\snmp_asn1.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\include\lwip\snmp_msg.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\include\lwip\snmp_structs.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\include\lwip\sockets.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\include\lwip\stats.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\include\lwip\sys.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\include\lwip\tcp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\include\lwip\tcpip.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\include\lwip\udp.h
# End Source File
# End Group
# End Group
# Begin Group "uip1.0"

# PROP Default_Filter ""
# Begin Group "net"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip1.0\net\clock.h

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\net\uip1.0\net\lc-addrlabels.h"

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\net\uip1.0\net\lc-switch.h"

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip1.0\net\lc.h

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip1.0\net\psock.h

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip1.0\net\pt.h

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip1.0\net\timer.h

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\net\uip1.0\net\uip-conf.h"

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\net\uip1.0\net\uip-fw.c"

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\net\uip1.0\net\uip-fw.h"

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\net\uip1.0\net\uip-neighbor.c"

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\net\uip1.0\net\uip-neighbor.h"

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\net\uip1.0\net\uip-split.c"

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\net\uip1.0\net\uip-split.h"

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip1.0\net\uip.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip1.0\net\uip.h

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip1.0\net\uip_arch.h

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip1.0\net\uip_arp.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip1.0\net\uip_arp.h

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip1.0\net\uiplib.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip1.0\net\uiplib.h

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip1.0\net\uipopt.h

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# End Group
# Begin Group "uip2.5"

# PROP Default_Filter ""
# Begin Group "uip_sys"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip2.5\sys\cc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip2.5\sys\clock.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip2.5\sys\clock.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip2.5\sys\ctimer.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip2.5\sys\ctimer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip2.5\sys\etimer.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip2.5\sys\etimer.h
# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\net\uip2.5\sys\lc-addrlabels.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\net\uip2.5\sys\lc-switch.h"
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip2.5\sys\lc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip2.5\sys\process.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip2.5\sys\pt.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip2.5\sys\rtimer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip2.5\sys\stimer.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip2.5\sys\stimer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip2.5\sys\timer_ip.c
# End Source File
# End Group
# Begin Group "uip_lib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip2.5\lib\list.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip2.5\lib\list.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip2.5\lib\memb.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip2.5\lib\memb.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip2.5\lib\random.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip2.5\lib\random.h
# End Source File
# End Group
# Begin Group "uip_net"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip2.5\net\dhcpc.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip2.5\net\dhcpc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip2.5\net\hc.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip2.5\net\hc.h
# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\net\uip2.5\net\neighbor-attr.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\net\uip2.5\net\neighbor-attr.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\net\uip2.5\net\neighbor-info.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\net\uip2.5\net\neighbor-info.h"
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip2.5\net\netstack.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip2.5\net\netstack.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip2.5\net\packetbuf.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip2.5\net\packetbuf.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip2.5\net\packetqueue.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip2.5\net\packetqueue.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip2.5\net\psock.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip2.5\net\psock.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip2.5\net\queuebuf.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip2.5\net\queuebuf.h
# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\net\uip2.5\net\rawpacket-udp.c"

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\net\uip2.5\net\rawpacket-udp.h"
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip2.5\net\rawpacket.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip2.5\net\resolv.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip2.5\net\resolv.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip2.5\net\rime.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip2.5\net\sicslowpan.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip2.5\net\sicslowpan.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip2.5\net\slipdev.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip2.5\net\slipdev.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip2.5\net\tcpdump.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip2.5\net\tcpdump.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip2.5\net\tcpip.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip2.5\net\tcpip.h
# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\net\uip2.5\net\uaodv-def.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\net\uip2.5\net\uaodv-rt.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\net\uip2.5\net\uaodv-rt.h"
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip2.5\net\uaodv.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip2.5\net\uaodv.h
# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\net\uip2.5\net\uip-debug.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\net\uip2.5\net\uip-debug.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\net\uip2.5\net\uip-ds6.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\net\uip2.5\net\uip-ds6.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\net\uip2.5\net\uip-fw-drv.c"

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\net\uip2.5\net\uip-fw-drv.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\net\uip2.5\net\uip-fw.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\net\uip2.5\net\uip-fw.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\net\uip2.5\net\uip-icmp6.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\net\uip2.5\net\uip-icmp6.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\net\uip2.5\net\uip-nd6.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\net\uip2.5\net\uip-nd6.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\net\uip2.5\net\uip-neighbor.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\net\uip2.5\net\uip-neighbor.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\net\uip2.5\net\uip-over-mesh.c"

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\net\uip2.5\net\uip-over-mesh.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\net\uip2.5\net\uip-packetqueue.c"

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\net\uip2.5\net\uip-packetqueue.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\net\uip2.5\net\uip-split.c"

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\net\uip2.5\net\uip-split.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\net\uip2.5\net\uip-udp-packet.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\net\uip2.5\net\uip-udp-packet.h"
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip2.5\net\uip.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip2.5\net\uip.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip2.5\net\uip6.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip2.5\net\uip_arch.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip2.5\net\uip_arp.c

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip2.5\net\uip_arp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip2.5\net\uiplib.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip2.5\net\uiplib.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip2.5\net\uipopt.h
# End Source File
# End Group
# Begin Source File

SOURCE="..\..\..\src\kernel\net\uip2.5\contiki-conf.h"

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\net\uip2.5\contiki-lib.h"

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\net\uip2.5\contiki-net.h"

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\net\uip2.5\contiki-version.h"

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip2.5\contiki.h

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\net\uip2.5\rtimer-arch.h"

!IF  "$(CFG)" == "net - Win32 Release"

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# End Group
# End Target
# End Project
