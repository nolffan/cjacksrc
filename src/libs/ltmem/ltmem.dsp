# Microsoft Developer Studio Project File - Name="ltmem" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=ltmem - Win32 Final
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ltmem.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ltmem.mak" CFG="ltmem - Win32 Final"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ltmem - Win32 Final" (based on "Win32 (x86) Static Library")
!MESSAGE 

# nolf2src: ltmem.lib, the LithTech memory library the engine's modules and
# Monolith's game DLLs link (TO2.dsw names it as a dependency; the source
# release does not have it). The Jupiter (2006) tree's libs/ltmem: ltmem.cpp
# without USELTMEM (all the sound driver links: retail SndDrv.dll
# 0x100085d0..0x10008632), and what the engine adds through LTMemConsole,
# Lithtech.exe 0x49f600..0x4a0d2d and server.dll 0x1005dbf0..0x1005f31d,
# objects in the order the link pulls them: ltmem.obj, ltmemstats.obj,
# ltmemtrack.obj, ltmemheap.obj. Not ltmemdebug.cpp (nothing references it).
# /Ox, not /O2: no /Gy, so each object's code is one section the link keeps
# whole (retail keeps LTMemHeapInit and LTMemReAlloc, which nothing calls);
# /GF, as retail pools these objects' strings with the engine's ("unknown",
# "world", "wt", "\n"). ltmem.cpp has no strings, so /GF leaves the sound
# driver's ltmem.obj as it was. The engine's headers (bdefs.h) only for the
# two files that include them. Every file through stdafx.h's precompiled
# header, as the Developer Studio static library template has it.
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ltmem - Win32 Final"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Final"
# PROP BASE Intermediate_Dir "Final"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\Engine\built\Final\ltmem"
# PROP Intermediate_Dir "..\..\Engine\built\Final\ltmem"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Ox /Ob2 /GF /I "..\..\Engine\sdk\inc" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\Engine\built\Final\ltmem\ltmem.lib"
!ENDIF 

# Begin Target

# Name "ltmem - Win32 Final"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ltmem.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\ltmemheap.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\ltmemstats.cpp
# ADD CPP /I "..\..\Engine\runtime\shared\src" /I "..\..\Engine\runtime\kernel\src" /I "..\..\Engine\runtime\kernel\mem\src" /I "..\stdlith" /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\ltmemtrack.cpp
# ADD CPP /I "..\..\Engine\runtime\shared\src" /I "..\..\Engine\runtime\kernel\src" /I "..\..\Engine\runtime\kernel\mem\src" /I "..\stdlith" /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\stdafx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\generalheap.h
# End Source File
# Begin Source File

SOURCE=.\generalheapgroup.h
# End Source File
# Begin Source File

SOURCE=.\lilfixedheap.h
# End Source File
# Begin Source File

SOURCE=.\lilfixedheapgroup.h
# End Source File
# Begin Source File

SOURCE=.\ltmemheap.h
# End Source File
# Begin Source File

SOURCE=.\ltmemtrack.h
# End Source File
# Begin Source File

SOURCE=.\stdafx.h
# End Source File
# End Group
# End Target
# End Project
