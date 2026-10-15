# Microsoft Developer Studio Project File - Name="Launcher" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Launcher - Win32 Final

# cjacksrc: ContractJack.exe, the launcher: NOLF2's Launcher project (MFC
# 7.1 in a DLL) with Contract J.A.C.K.'s files and VC .NET 2003's flags.
# Monolith's VS.NET 2003 project is lost; this is it in the VC6 format
# tools/dsp2mk.py reads, with its Final configuration only.

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Launcher - Win32 Final"

# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\built\Final\Launcher"
# PROP Intermediate_Dir "..\..\built\Final\Launcher"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD CPP /nologo /MD /W3 /O2 /I ".\\" /I "..\\" /I "..\..\Shared" /I "..\..\..\Engine\sdk\inc" /I "..\..\..\libs\regmgr" /I "..\..\..\libs\regmgr32" /I "..\..\..\libs\lith" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /D "_FINAL" /Yu"stdafx.h" /FD /c
# ADD RSC /l 0x409 /i "res\lang\en\\" /i "..\..\built\retail\Launcher" /d "NDEBUG" /d "_AFXDLL"
LINK32=link.exe
# ADD LINK32 /nologo /subsystem:windows /machine:I386 /out:"..\..\built\Final\Launcher\ContractJack.exe"

!ENDIF 

# Begin Target

# Name "Launcher - Win32 Final"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;rc;lib"
# Begin Source File

SOURCE=..\BitmapCheckButton.cpp
# End Source File
# Begin Source File

SOURCE=..\ButtonEx.cpp
# End Source File
# Begin Source File

SOURCE=..\CustomizeDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\DetailSettingsDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\DisplayDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\DisplayMgr.cpp
# End Source File
# Begin Source File

SOURCE=..\DlgEx.cpp
# End Source File
# Begin Source File

SOURCE=..\EditEx.cpp
# End Source File
# Begin Source File

SOURCE=..\Launcher.cpp
# End Source File
# Begin Source File

SOURCE=..\LauncherDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\MessageBoxDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\MoveDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\OptionsDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\RezFind.cpp
# End Source File
# Begin Source File

SOURCE=..\StaticEx.cpp
# End Source File
# Begin Source File

SOURCE=..\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=..\TextCheckBox.cpp
# End Source File
# Begin Source File

SOURCE=..\Utils.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Shared\VersionMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\RES\Lang\EN\Launcher.rc
# End Source File
# Begin Source File

SOURCE=..\..\..\libs\built\Final\RegMgr\regmgr.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\libs\built\Final\RegMgr32\regmgr32.lib
# End Source File
# End Group
# End Target
# End Project
