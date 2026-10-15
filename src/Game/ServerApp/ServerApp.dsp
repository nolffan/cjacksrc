# Microsoft Developer Studio Project File - Name="ServerApp" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=ServerApp - Win32 Final

# cjacksrc: ContractJackServer.exe, the dedicated server: NOLF2's ServerApp
# project (MFC 7.1 in a DLL, server.dll's import library, ButeMgr and
# CryptMgr built against MFC) with Contract J.A.C.K.'s files and VC .NET
# 2003's flags. Monolith's VS.NET 2003 project is lost; this is it in the
# VC6 format tools/dsp2mk.py reads, with its Final configuration only.

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ServerApp - Win32 Final"

# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\built\Final\ServerApp"
# PROP Intermediate_Dir "..\built\Final\ServerApp"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD CPP /nologo /MD /W3 /GR /Ox /GF /I ".\\" /I "..\..\Engine\sdk\inc" /I "..\shared" /I "..\shared\lang\en" /I "..\..\libs\regmgr" /I "..\..\libs\regmgr32" /I "..\..\libs\lith" /I "..\..\libs\butemgr" /I "..\..\libs\cryptmgr" /I "..\..\libs\stdlith" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /D "_FINAL" /Yu"stdafx.h" /FD /c
# ADD RSC /l 0x409 /i ".\\" /i "res\lang\en" /i "..\shared" /i "..\shared\lang\en" /i "..\built\retail\ServerApp" /d "NDEBUG" /d "_REG" /d "_AFXDLL"
LINK32=link.exe
# ADD LINK32 server.lib winmm.lib /nologo /subsystem:windows /machine:I386 /out:"..\built\Final\ServerApp\ContractJackServer.exe" /libpath:"..\..\Engine\built\Final\server"

!ENDIF 

# Begin Target

# Name "ServerApp - Win32 Final"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;rc;lib"
# Begin Source File

SOURCE=..\Shared\CommonUtilities.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\DebugNew.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\Shared\GameAlloc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\sdk\inc\ltmodule.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\Shared\NetDefs.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\ParsedMsg.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\ProfileUtils.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\ScmdConsole.cpp
# End Source File
# Begin Source File

SOURCE=.\ScmdConsoleDriver_ServerApp.cpp
# End Source File
# Begin Source File

SOURCE=.\SelectConfigDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ServerApp.cpp
# End Source File
# Begin Source File

SOURCE=.\ServerDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Splash.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=..\Shared\VersionMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\res\Lang\EN\ServerApp.rc
# End Source File
# Begin Source File

SOURCE=..\..\Engine\built\Final\server\server.lib
# End Source File
# Begin Source File

SOURCE=..\..\libs\built\Final\ButeMgrMfcDll\ButeMgrMfcDll.lib
# End Source File
# Begin Source File

SOURCE=..\..\libs\built\Final\CryptMgrMfcDll\CryptMgrMfcDll.lib
# End Source File
# Begin Source File

SOURCE=..\..\libs\built\Final\Lib_Lith\Lib_Lith.lib
# End Source File
# Begin Source File

SOURCE=..\..\libs\built\Final\Lib_StdLith\Lib_StdLith.lib
# End Source File
# Begin Source File

SOURCE=..\..\libs\built\Final\RegMgr\regmgr.lib
# End Source File
# Begin Source File

SOURCE=..\..\libs\built\Final\RegMgr32\regmgr32.lib
# End Source File
# End Group
# End Target
# End Project
