# Microsoft Developer Studio Project File - Name="snddrv" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=snddrv - Win32 Final
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "snddrv.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "snddrv.mak" CFG="snddrv - Win32 Final"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "snddrv - Win32 Final" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# cjacksrc: SndDrv.dll, the engine's sound driver (DirectSound 8), which the
# engine loads and asks for its ILTSoundSys through the exports SoundSysMake
# and SoundSysDesc. Monolith's release has none of its source: written for
# this project, reconstructed from the retail binary (snddrv.dsw). The
# flags are the ones Contract J.A.C.K.'s retail code shows: /Ox, not /O2:
# no /Gy, so every out-of-line function of an object stays in its one
# .text section in the order the compiler finished them (the counter
# functions nothing calls are in the image, at its start), and only inline
# and template functions are COMDATs the linker may drop; /GF, the
# literals read-only. No /GX: the scoped locks (CWinSync_CSAuto) and the
# objects with destructors get no unwind frames, and the only exception
# frames are the STL's own try blocks (warning C4530). s_dx8.cpp's
# precompiled header holds windows.h alone: compiled so, the compiler
# emits the header's inline virtual functions (ILTSoundSys's and
# logic_error's deleting destructors, what() with basic_string::c_str
# inlined) ahead of the file's own code and compiles the functions in the
# order retail lays them out; through one that takes in s_dx8.h and the
# STL (a /YX header ending at counter.h, as nolf2src built it) they come
# after it, c_str is imported and a dozen functions move to a later pass.
# The project's precompiled header
# is stdafx.h (windows.h), made by stdafx.cpp (/Yc) and used by
# parsewavefile.cpp and s_dx8.cpp (/Yu), which include it first;
# counter.cpp, from the engine's tree, uses none. Through the precompiled
# header an object keeps the header constants (math_phys.h's PI and the
# rest) in .bss in the order they are defined, as retail's parsewavefile and
# s_dx8 objects have them (0x1000c1bc.., 0x1000c1e4..); compiled on its own,
# as counter.obj, in the compiler's own order (0x1000c1a0..). The link
# names the libraries as a Visual Studio .NET 2003 project passes them, the
# project's own (ltmem, winmm, msacm32, dxguid) ahead of the IDE's default
# list (kernel32 .. odbccp32): the libraries the linker searches, and their
# order, decide the order of each DLL's imports in the import table, which
# this way is retail's.

!IF  "$(CFG)" == "snddrv - Win32 Final"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Final"
# PROP BASE Intermediate_Dir "Final"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\built\Final\snddrv"
# PROP Intermediate_Dir "..\built\Final\snddrv"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_FINAL" /FD /c
# ADD CPP /nologo /MD /W3 /Ox /GF /I "." /I "..\sdk\inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_FINAL" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 ltmem.lib winmm.lib msacm32.lib dxguid.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"..\built\Final\snddrv\SndDrv.dll" /libpath:"..\built\Final\ltmem"
!ENDIF 

# Begin Target

# Name "snddrv - Win32 Final"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\counter.cpp
# End Source File
# Begin Source File

SOURCE=.\parsewavefile.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\s_dx8.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\stdafx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\counter.h
# End Source File
# Begin Source File

SOURCE=.\eax.h
# End Source File
# Begin Source File

SOURCE=.\iltsound.h
# End Source File
# Begin Source File

SOURCE=.\s_dx8.h
# End Source File
# Begin Source File

SOURCE=.\stdafx.h
# End Source File
# Begin Source File

SOURCE=.\winsync.h
# End Source File
# End Group
# End Target
# End Project
