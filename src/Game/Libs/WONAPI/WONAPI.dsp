# Microsoft Developer Studio Project File - Name="WONAPI" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=WONAPI - Win32 Final
!MESSAGE "WONAPI - Win32 Final" (based on "Win32 (x86) Static Library")

# cjacksrc: WONAPI.lib, the part of WON's client library (as the LithTech
# Jupiter SDK ships it in Game\Libs\WONAPI) that GameSpy.dll links: its
# IsCDKeyValid checks a key with WONAPI::CDKey (the retail image's RTTI name
# .?AVCDKey@WONAPI@@), whose code follows the GameSpy SDK's at 0x10014740.
# Written for this project after the retail image. The library was compiled
# with /GX (new-expressions guarded) and /GR (the vtables have RTTI), which
# the rest of GameSpy.dll is not.

!IF  "$(CFG)" == "WONAPI - Win32 Final"

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\built\Final\libs\WONAPI"
# PROP Intermediate_Dir "..\..\built\Final\libs\WONAPI"
# PROP Target_Dir ""
# ADD CPP /nologo /MD /W3 /GX /GR /O2 /I "." /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
LIB32=link.exe -lib
# ADD LIB32 /nologo /out:"..\..\built\Final\libs\WONAPI\WONAPI.lib"
!ENDIF 

# Begin Target

# Name "WONAPI - Win32 Final"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\WONAuth\CDKey.cpp
# End Source File
# Begin Source File

SOURCE=.\WONCommon\CRC.cpp
# End Source File
# Begin Source File

SOURCE=.\WONCommon\ByteBuffer.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\WONAuth\CDKey.h
# End Source File
# Begin Source File

SOURCE=.\WONCommon\ByteBuffer.h
# End Source File
# Begin Source File

SOURCE=.\WONCommon\CRC.h
# End Source File
# Begin Source File

SOURCE=.\WONCommon\SmartPtr.h
# End Source File
# Begin Source File

SOURCE=.\WONShared.h
# End Source File
# End Group
# End Target
# End Project
