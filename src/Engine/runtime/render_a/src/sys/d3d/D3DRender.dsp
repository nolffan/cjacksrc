# Microsoft Developer Studio Project File - Name="D3DRender" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=D3DRender - Win32 Final
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "D3DRender.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "D3DRender.mak" CFG="D3DRender - Win32 Final"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "D3DRender - Win32 Final" (based on "Win32 (x86) Static Library")
!MESSAGE 

# cjacksrc: the renderer, render_a (LIB_D3DRender in the LithTech Jupiter
# tree, ported there to Direct3D 9; nolf2src's back-port to NOLF2's
# Direct3D 8), a static library of Lithtech.exe (Engine.dsw).
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "D3DRender - Win32 Final"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Final"
# PROP BASE Intermediate_Dir "Final"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\..\..\built\Final\D3DRender"
# PROP Intermediate_Dir "..\..\..\..\..\built\Final\D3DRender"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /Ox /GF /Zm200 /I "." /I "..\shadows" /I "..\..\..\..\client\src" /I "..\..\..\..\shared\src" /I "..\..\..\..\kernel\src" /I "..\..\..\..\kernel\mem\src" /I "..\..\..\..\world\src" /I "..\..\..\..\kernel\io\src" /I "..\..\..\..\kernel\net\src" /I "..\..\..\..\model\src" /I "..\..\..\..\sound\src" /I "..\..\..\..\shared\src\sys\win" /I "..\..\..\..\lithtemplate" /I "..\..\..\..\physics\src" /I "..\..\..\..\render_b\src" /I "..\..\..\..\ui\src" /I "..\..\..\..\controlfilemgr" /I "..\..\..\..\server\src" /I "..\..\..\..\kernel\src\sys\win" /I "..\..\..\..\kernel\io\src\sys\win" /I "..\..\..\..\kernel\mem\src\sys\win" /I "..\..\..\..\kernel\net\src\sys\win" /I "..\..\..\..\client\src\sys\win" /I "." /I "..\..\..\..\..\sdk\inc" /I "..\..\..\..\..\sdk\inc\physics" /I "..\..\..\..\..\..\libs\stdlith" /I "..\..\..\..\..\..\libs\lith" /I "..\..\..\..\..\libs\rezmgr" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_FINAL" /D "__D3D" /D "__D3DREND" /D "_LIB" /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\..\..\built\Final\D3DRender\D3DRender.lib"
!ENDIF 

# Begin Target

# Name "D3DRender - Win32 Final"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\d3d_device.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\common_init.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\d3d_renderstyleinterface.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\d3d_texture.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\d3d_init.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\d3d_device_wrapper.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\d3d_renderworld.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\d3d_draw.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\d3d_utils.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\d3d_shell.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\pixelshadermgr.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\screenglowmgr.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\d3dmeshrendobj_rigid.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\rendershadowlist.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\drawobjects.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\tagnodes.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\d3d_surface.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\memstats_world.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\d3d_renderblock.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\modelshadowshader.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\d3d_rendershader_dynamiclight.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\d3d_renderworld_occluder.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\texturescriptmgr.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\drawpolygrid.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\3d_ops.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\aabb.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\common_draw.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\common_stuff.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\d3d_drawsky.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\d3d_optimizedsurface.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\d3d_rendershader_glow.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\d3d_rendershader_gouraud.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\d3d_rendershader_gouraud_fullbright.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\d3d_rendershader_lightmap.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\d3d_renderstatemgr.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\d3d_renderstyle.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\d3d_viewparams.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\d3dmeshrendobj_skel.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\d3dmeshrendobj_vertanim.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=..\shadows\d3dshadowtexture.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\devicelightlist.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\dirtyrect.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\draw_canvas.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\drawlight.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\drawlinesystem.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\drawmodel.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\drawparticles.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\drawparticles_A.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\drawsky.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\drawsprite.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\drawvolumeeffects.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\drawworldmodel.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\modeldebug.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\objectgroupmgr.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\relevantlightlist.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\rendererconsolevars.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\renderlight.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\rendermodelinfolist.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\rendermodelpiecelist.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\renderstylelookuptables.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\renderstylemap.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\setupmodel.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\setuptouchinglights.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\texturescriptinstance.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\precompile.cpp
# ADD CPP /Yc"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\texturescriptinterpreter.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\texturescriptvarmgr.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\VertexBufferController.cpp
# ADD CPP /Yu"precompile.h"
# End Source File
# End Group
# End Target
# End Project
