# Microsoft Developer Studio Project File - Name="CryptMgrMfcDll" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=CryptMgrMfcDll - Win32 Final

# cjacksrc: CryptMgr built against MFC in a DLL, for the dedicated server.
# NOLF2's CryptMgrMfcDll.dsp was an external target that built
# cryptmgr.dsp's "Final MFC DLL" configuration; this is that configuration
# as a project of its own, with VC .NET 2003's flags, in the VC6 format
# tools/dsp2mk.py reads.

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CryptMgrMfcDll - Win32 Final"

# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\built\Final\CryptMgrMfcDll"
# PROP Intermediate_Dir "..\built\Final\CryptMgrMfcDll"
# PROP Target_Dir ""
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /D "NO_PRAGMA_LIBS" /YX /FD /c
# ADD RSC /l 0x409 /d "_AFXDLL"
LIB32=link.exe -lib
# ADD LIB32 /nologo /out:"..\built\Final\CryptMgrMfcDll\CryptMgrMfcDll.lib"

!ENDIF 

# Begin Target

# Name "CryptMgrMfcDll - Win32 Final"
# Begin Group "Source Files"

# PROP Default_Filter ".cpp"
# Begin Source File

SOURCE=.\blowfish.cpp
# End Source File
# Begin Source File

SOURCE=.\CryptMgr.cpp
# End Source File
# End Group
# End Target
# End Project
