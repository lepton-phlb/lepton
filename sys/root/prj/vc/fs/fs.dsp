# Microsoft Developer Studio Project File - Name="fs" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=fs - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "fs.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "fs.mak" CFG="fs - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "fs - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "fs - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "lepton"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "fs - Win32 Release"

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

!ELSEIF  "$(CFG)" == "fs - Win32 Debug"

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
# ADD CPP /nologo /Zp1 /MDd /W3 /Gm /GX /ZI /Od /I "c:/tauon/sys/root/src" /I "c:/tauon/sys/root/src/kernel/fs/yaffs/core" /I "c:/tauon/sys/root/src/kernel/fs/yaffs/core/direct" /D "_LIB" /D "WIN32" /D "_DEBUG" /D "_MBCS" /YX /FD /GZ /c
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

# Name "fs - Win32 Release"
# Name "fs - Win32 Debug"
# Begin Group "kernel"

# PROP Default_Filter ""
# Begin Group "vfs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\vfs\vfs.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\vfs\vfs.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\vfs\vfscore.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\vfs\vfscore.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\vfs\vfsdev.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\vfs\vfsdev.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\vfs\vfskernel.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\vfs\vfskernel.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\vfs\vfstypes.h
# End Source File
# End Group
# Begin Group "rootfs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\rootfs\rootfs.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\rootfs\rootfs.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\rootfs\rootfscore.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\rootfs\rootfscore.h
# End Source File
# End Group
# Begin Group "ufs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\ufs\ufs.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\ufs\ufs.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\ufs\ufscore.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\ufs\ufscore.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\ufs\ufsdriver.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\ufs\ufsdriver.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\ufs\ufsdriver_1_3.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\ufs\ufsdriver_1_3.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\ufs\ufsdriver_1_4.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\ufs\ufsdriver_1_4.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\ufs\ufsdriver_1_5.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\ufs\ufsdriver_1_5.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\ufs\ufsinfo.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\ufs\ufsx.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\ufs\ufsx.h
# End Source File
# End Group
# Begin Group "kofs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\kofs\kofs.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\kofs\kofs.h
# End Source File
# End Group
# Begin Group "yaffs"

# PROP Default_Filter ""
# Begin Group "core"

# PROP Default_Filter ""
# Begin Group "direct"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\yaffs\core\direct\yaffs_flashif.c

!IF  "$(CFG)" == "fs - Win32 Release"

!ELSEIF  "$(CFG)" == "fs - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\yaffs\core\direct\yaffs_flashif.h

!IF  "$(CFG)" == "fs - Win32 Release"

!ELSEIF  "$(CFG)" == "fs - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\yaffs\core\direct\yaffs_flashif2.h

!IF  "$(CFG)" == "fs - Win32 Release"

!ELSEIF  "$(CFG)" == "fs - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\yaffs\core\direct\yaffs_malloc.h

!IF  "$(CFG)" == "fs - Win32 Release"

!ELSEIF  "$(CFG)" == "fs - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\yaffs\core\direct\yaffs_nandif.c

!IF  "$(CFG)" == "fs - Win32 Release"

!ELSEIF  "$(CFG)" == "fs - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\yaffs\core\direct\yaffs_nandif.h

!IF  "$(CFG)" == "fs - Win32 Release"

!ELSEIF  "$(CFG)" == "fs - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\yaffs\core\direct\yaffs_norif1.c

!IF  "$(CFG)" == "fs - Win32 Release"

!ELSEIF  "$(CFG)" == "fs - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\yaffs\core\direct\yaffs_norif1.h

!IF  "$(CFG)" == "fs - Win32 Release"

!ELSEIF  "$(CFG)" == "fs - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\yaffs\core\direct\yaffs_ramdisk.c

!IF  "$(CFG)" == "fs - Win32 Release"

!ELSEIF  "$(CFG)" == "fs - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\yaffs\core\direct\yaffs_ramdisk.h

!IF  "$(CFG)" == "fs - Win32 Release"

!ELSEIF  "$(CFG)" == "fs - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\yaffs\core\direct\yaffs_ramem2k.c

!IF  "$(CFG)" == "fs - Win32 Release"

!ELSEIF  "$(CFG)" == "fs - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\yaffs\core\direct\ydirectenv.h

!IF  "$(CFG)" == "fs - Win32 Release"

!ELSEIF  "$(CFG)" == "fs - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\yaffs\core\direct\ynorsim.c

!IF  "$(CFG)" == "fs - Win32 Release"

!ELSEIF  "$(CFG)" == "fs - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\yaffs\core\direct\ynorsim.h

!IF  "$(CFG)" == "fs - Win32 Release"

!ELSEIF  "$(CFG)" == "fs - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\yaffs\core\devextras.h

!IF  "$(CFG)" == "fs - Win32 Release"

!ELSEIF  "$(CFG)" == "fs - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\yaffs\core\yaffs_checkptrw.c

!IF  "$(CFG)" == "fs - Win32 Release"

!ELSEIF  "$(CFG)" == "fs - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\yaffs\core\yaffs_checkptrw.h

!IF  "$(CFG)" == "fs - Win32 Release"

!ELSEIF  "$(CFG)" == "fs - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\yaffs\core\yaffs_ecc.c

!IF  "$(CFG)" == "fs - Win32 Release"

!ELSEIF  "$(CFG)" == "fs - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\yaffs\core\yaffs_ecc.h

!IF  "$(CFG)" == "fs - Win32 Release"

!ELSEIF  "$(CFG)" == "fs - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\yaffs\core\yaffs_getblockinfo.h

!IF  "$(CFG)" == "fs - Win32 Release"

!ELSEIF  "$(CFG)" == "fs - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\yaffs\core\yaffs_guts.c

!IF  "$(CFG)" == "fs - Win32 Release"

!ELSEIF  "$(CFG)" == "fs - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\yaffs\core\yaffs_guts.h

!IF  "$(CFG)" == "fs - Win32 Release"

!ELSEIF  "$(CFG)" == "fs - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\yaffs\core\yaffs_nand.c

!IF  "$(CFG)" == "fs - Win32 Release"

!ELSEIF  "$(CFG)" == "fs - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\yaffs\core\yaffs_nand.h

!IF  "$(CFG)" == "fs - Win32 Release"

!ELSEIF  "$(CFG)" == "fs - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\yaffs\core\yaffs_nandemul2k.h

!IF  "$(CFG)" == "fs - Win32 Release"

!ELSEIF  "$(CFG)" == "fs - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\yaffs\core\yaffs_packedtags1.c

!IF  "$(CFG)" == "fs - Win32 Release"

!ELSEIF  "$(CFG)" == "fs - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\yaffs\core\yaffs_packedtags1.h

!IF  "$(CFG)" == "fs - Win32 Release"

!ELSEIF  "$(CFG)" == "fs - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\yaffs\core\yaffs_packedtags2.c

!IF  "$(CFG)" == "fs - Win32 Release"

!ELSEIF  "$(CFG)" == "fs - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\yaffs\core\yaffs_packedtags2.h

!IF  "$(CFG)" == "fs - Win32 Release"

!ELSEIF  "$(CFG)" == "fs - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\yaffs\core\yaffs_qsort.c

!IF  "$(CFG)" == "fs - Win32 Release"

!ELSEIF  "$(CFG)" == "fs - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\yaffs\core\yaffs_qsort.h

!IF  "$(CFG)" == "fs - Win32 Release"

!ELSEIF  "$(CFG)" == "fs - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\yaffs\core\yaffs_tagscompat.c

!IF  "$(CFG)" == "fs - Win32 Release"

!ELSEIF  "$(CFG)" == "fs - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\yaffs\core\yaffs_tagscompat.h

!IF  "$(CFG)" == "fs - Win32 Release"

!ELSEIF  "$(CFG)" == "fs - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\yaffs\core\yaffs_tagsvalidity.c

!IF  "$(CFG)" == "fs - Win32 Release"

!ELSEIF  "$(CFG)" == "fs - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\yaffs\core\yaffs_tagsvalidity.h

!IF  "$(CFG)" == "fs - Win32 Release"

!ELSEIF  "$(CFG)" == "fs - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\yaffs\core\yaffs_trace.h

!IF  "$(CFG)" == "fs - Win32 Release"

!ELSEIF  "$(CFG)" == "fs - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\yaffs\core\yaffsinterface.h

!IF  "$(CFG)" == "fs - Win32 Release"

!ELSEIF  "$(CFG)" == "fs - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\yaffs\core\yportenv.h

!IF  "$(CFG)" == "fs - Win32 Release"

!ELSEIF  "$(CFG)" == "fs - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\yaffs\yaffs.c

!IF  "$(CFG)" == "fs - Win32 Release"

!ELSEIF  "$(CFG)" == "fs - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\yaffs\yaffs.h

!IF  "$(CFG)" == "fs - Win32 Release"

!ELSEIF  "$(CFG)" == "fs - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\yaffs\yaffscore.c

!IF  "$(CFG)" == "fs - Win32 Release"

!ELSEIF  "$(CFG)" == "fs - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\yaffs\yaffscore.h

!IF  "$(CFG)" == "fs - Win32 Release"

!ELSEIF  "$(CFG)" == "fs - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\yaffs\yaffsdriver_nor_flash.c

!IF  "$(CFG)" == "fs - Win32 Release"

!ELSEIF  "$(CFG)" == "fs - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\yaffs\yaffsdriver_nor_flash.h

!IF  "$(CFG)" == "fs - Win32 Release"

!ELSEIF  "$(CFG)" == "fs - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\yaffs\yaffsinfo.h

!IF  "$(CFG)" == "fs - Win32 Release"

!ELSEIF  "$(CFG)" == "fs - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "fat"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\fat\fat16.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\fat\fat16.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\fat\fat16_msdos.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\fat\fat16_vfat.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\fat\fatcore.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\fat\fatcore.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\fat\fatcore_msdos.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\fat\fatcore_msdos.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\fat\fatcore_vfat.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\fat\fatcore_vfat.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\kernel\fs\fat\fatinfo.h
# End Source File
# End Group
# End Group
# End Target
# End Project
