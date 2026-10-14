# Microsoft Developer Studio Project File - Name="GameSpy" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=GameSpy - Win32 Final
!MESSAGE "GameSpy - Win32 Final" (based on "Win32 (x86) Dynamic-Link Library")

# cjacksrc: GameSpy.dll, Monolith's Game\Libs\GameSpy (C:\proj\to2\source\
# Game\Libs\GameSpy in the retail image's strings) around GameSpy's C SDK,
# which the image names file by file (GameSpySDK\darray.c, ...). Monolith's
# own project file is lost; this one is written for this project. The DLL
# exports the three interfaces of IGameSpy.h and IsCDKeyValid; CShell.dll and
# Object.lto import them (GameSpy.lib).
#
# The flags are what the retail code shows: /Ox /GF -- /O2's optimizations
# without its function packaging (/Gy, below) -- (loop tops aligned, frame
# pointer omitted, string literals pooled across objects), no /GS (no security cookie anywhere), no NDEBUG (the
# retail image's asserts are live, the SDK's and Monolith's -- Ping.cpp's
# assert(false) -- alike), and no /GX for Monolith's wrapper: its functions
# keep std::string locals without any exception frame, and only the explicit
# try blocks of the STL's templates have one (WONAPI.lib, compiled with /GX,
# guards its new-expressions). No /Gy: functions nothing calls are in
# the image (Monolith's string conversions, nonport.c's spare socket
# helpers), while WONAPI.lib's are not (the linker's /OPT:REF drops a
# library member's unreferenced functions only when they are packaged). The
# sources are in the retail link order: Monolith's C++, then the SDK's C
# files (29 C objects in the Rich header with the runtime's); WONAPI.lib
# (its CD key check) comes after them, as in the image.

!IF  "$(CFG)" == "GameSpy - Win32 Final"

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\built\Final\libs\GameSpy"
# PROP Intermediate_Dir "..\..\built\Final\libs\GameSpy"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /MD /W3 /Ox /GF /I "." /I ".." /I "..\WONAPI" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "GAMESPY_EXPORTS" /D "_FINAL" /FD /c
# ADD RSC /l 0x409
LINK32=link.exe
# ADD LINK32 kernel32.lib advapi32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"..\..\built\Final\libs\GameSpy\GameSpy.dll"

!ENDIF 

# Begin Target

# Name "GameSpy - Win32 Final"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\GameSpyBrowser.cpp
# End Source File
# Begin Source File

SOURCE=.\GameSpyCommon.cpp
# End Source File
# Begin Source File

SOURCE=.\GameSpyPatchInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\GameSpyServer.cpp
# End Source File
# Begin Source File

SOURCE=.\Ping.cpp
# End Source File
# Begin Group "GameSpySDK"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GameSpySDK\darray.c
# End Source File
# Begin Source File

SOURCE=.\GameSpySDK\gcdkey\gcdkeyc.c
# End Source File
# Begin Source File

SOURCE=.\GameSpySDK\gcdkey\gcdkeys.c
# End Source File
# Begin Source File

SOURCE=.\GameSpySDK\ghttp\ghttpBuffer.c
# End Source File
# Begin Source File

SOURCE=.\GameSpySDK\ghttp\ghttpCallbacks.c
# End Source File
# Begin Source File

SOURCE=.\GameSpySDK\ghttp\ghttpCommon.c
# End Source File
# Begin Source File

SOURCE=.\GameSpySDK\ghttp\ghttpConnection.c
# End Source File
# Begin Source File

SOURCE=.\GameSpySDK\ghttp\ghttpMain.c
# End Source File
# Begin Source File

SOURCE=.\GameSpySDK\ghttp\ghttpPost.c
# End Source File
# Begin Source File

SOURCE=.\GameSpySDK\ghttp\ghttpProcess.c
# End Source File
# Begin Source File

SOURCE=.\GameSpySDK\hashtable.c
# End Source File
# Begin Source File

SOURCE=.\GameSpySDK\md5c.c
# End Source File
# Begin Source File

SOURCE=.\GameSpySDK\natneg\natneg.c
# End Source File
# Begin Source File

SOURCE=.\GameSpySDK\nonport.c
# End Source File
# Begin Source File

SOURCE=.\GameSpySDK\pt\ptMain.c
# End Source File
# Begin Source File

SOURCE=.\GameSpySDK\qr2\qr2.c
# End Source File
# Begin Source File

SOURCE=.\GameSpySDK\qr2\qr2regkeys.c
# End Source File
# Begin Source File

SOURCE=.\GameSpySDK\serverbrowsing\sb_crypt.c
# End Source File
# Begin Source File

SOURCE=.\GameSpySDK\serverbrowsing\sb_queryengine.c
# End Source File
# Begin Source File

SOURCE=.\GameSpySDK\serverbrowsing\sb_server.c
# End Source File
# Begin Source File

SOURCE=.\GameSpySDK\serverbrowsing\sb_serverbrowsing.c
# End Source File
# Begin Source File

SOURCE=.\GameSpySDK\serverbrowsing\sb_serverlist.c
# End Source File
# End Group
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\GameSpyBrowser.h
# End Source File
# Begin Source File

SOURCE=.\GameSpyCommon.h
# End Source File
# Begin Source File

SOURCE=.\GameSpyPatchInfo.h
# End Source File
# Begin Source File

SOURCE=.\GameSpyServer.h
# End Source File
# Begin Source File

SOURCE=.\IGameSpy.h
# End Source File
# Begin Source File

SOURCE=.\Ping.h
# End Source File
# Begin Source File

SOURCE=.\WinSync.h
# End Source File
# End Group
# End Target
# End Project
