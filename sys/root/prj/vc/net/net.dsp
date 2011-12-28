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
# ADD CPP /nologo /Zp1 /MDd /W3 /Gm /GX /ZI /Od /I "c:/tauon/sys/root/src" /I "c:/tauon/sys/root/src/net/lwip" /I "c:/tauon/sys/root/src/kernel/net/lwip/include" /I "c:/tauon/sys/root/src/kernel/net/lwip/include/ipv4" /I "c:/tauon/sys/root/src/kernel/net/lwip/ports/win32" /I "c:/tauon/sys/root/src/kernel/net/lwip/ports/win32/include" /D "_LIB" /D "WIN32" /D "_DEBUG" /D "_MBCS" /YX /FD /GZ /c
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
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\api\api_msg.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\api\err.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\api\netbuf.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\api\netdb.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\api\netifapi.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\api\sockets.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\api\tcpip.c
# End Source File
# End Group
# Begin Group "core"

# PROP Default_Filter ""
# Begin Group "ipv4"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\ipv4\autoip.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\ipv4\icmp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\ipv4\igmp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\ipv4\inet.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\ipv4\inet_chksum.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\ipv4\ip.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\ipv4\ip_addr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\ipv4\ip_frag.c
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
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\snmp\asn1_enc.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\snmp\mib2.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\snmp\mib_structs.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\snmp\msg_in.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\snmp\msg_out.c
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\dhcp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\dns.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\inet.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\inet6.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\init.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\mem.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\memp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\netif.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\pbuf.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\raw.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\stats.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\sys.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\tcp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\tcp_in.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\tcp_out.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\core\udp.c
# End Source File
# End Group
# Begin Group "ethif"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\ethif\ethif.c
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "netif"

# PROP Default_Filter ""
# Begin Group "ppp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\netif\ppp\auth.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\netif\ppp\auth.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\netif\ppp\chap.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\netif\ppp\chap.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\netif\ppp\chpms.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\netif\ppp\chpms.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\netif\ppp\fsm.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\netif\ppp\fsm.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\netif\ppp\ipcp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\netif\ppp\ipcp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\netif\ppp\lcp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\netif\ppp\lcp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\netif\ppp\magic.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\netif\ppp\magic.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\netif\ppp\md5.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\netif\ppp\md5.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\netif\ppp\pap.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\netif\ppp\pap.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\netif\ppp\ppp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\netif\ppp\ppp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\netif\ppp\pppdebug.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\netif\ppp\randm.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\netif\ppp\randm.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\netif\ppp\vj.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\netif\ppp\vj.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\netif\ppp\vjbsdhdr.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\netif\etharp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\netif\ethernetif.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\netif\loopif.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\lwip\netif\slipif.c
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
# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\net\uip1.0\net\lc-addrlabels.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\net\uip1.0\net\lc-switch.h"
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip1.0\net\lc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip1.0\net\psock.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip1.0\net\psock.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip1.0\net\pt.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip1.0\net\timer.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip1.0\net\timer.h
# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\net\uip1.0\net\uip-conf.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\net\uip1.0\net\uip-fw.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\net\uip1.0\net\uip-fw.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\net\uip1.0\net\uip-neighbor.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\net\uip1.0\net\uip-neighbor.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\net\uip1.0\net\uip-split.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\kernel\net\uip1.0\net\uip-split.h"
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip1.0\net\uip.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip1.0\net\uip.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip1.0\net\uip_arch.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip1.0\net\uip_arp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip1.0\net\uip_arp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip1.0\net\uiplib.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip1.0\net\uiplib.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\net\uip1.0\net\uipopt.h
# End Source File
# End Group
# End Group
# End Group
# End Target
# End Project
