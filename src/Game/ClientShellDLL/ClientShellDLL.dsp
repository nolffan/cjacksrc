# Microsoft Developer Studio Project File - Name="ClientShellDLL" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ClientShellDLL - Win32 Final

# cjacksrc: CShell.dll, the game's client. Contract J.A.C.K.'s CShell.dll
# was linked from one project, not NOLF2's two (the TO2 DLL and the
# ClientShellShared library): its code is in the alphabetical order of all
# their sources together (ActivateObjectFX first, WinUtil last; HUDAir, a
# TO2 file, between GibFX and HUDBar, shared ones), then the libraries in
# the alphabetical order of their projects, as Object.lto's is. So the two
# projects' sources are in this directory and listed here in that order,
# compiled with the TO2 project's flags through one precompiled header
# (stdafx.h, both projects' includes). Monolith's VS.NET 2003 project is
# lost; this is it in the VC6 format tools/dsp2mk.py reads, with its Final
# configuration only.

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ClientShellDLL - Win32 Final"

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\built\Final\ClientShellDLL"
# PROP Intermediate_Dir "..\built\Final\ClientShellDLL"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD CPP /nologo /MD /W3 /GR /O2 /I "..\Shared\TO2" /I ".\\" /I "..\ClientRes\Shared" /I "..\ClientRes\TO2" /I "..\Shared" /I "..\..\Engine\sdk\inc" /I "..\..\libs\stdlith" /I "..\..\libs\butemgr" /I "..\..\libs\cryptmgr" /I "..\..\libs\lith" /I "..\..\libs\mfcstub" /I "..\Libs\LTGUIMgr" /I "..\..\libs\regmgr" /I "..\Libs\ServerDir" /I "..\Libs\GameSpy" /D "NDEBUG" /D "_CLIENTBUILD" /D "WIN32" /D "_WINDOWS" /D "_NOLFBUILD" /D "NO_PRAGMA_LIBS" /D "NOPS2" /D "_FINAL" /D "MEMTRACK_CLIENT" /Yu"stdafx.h" /FD /Zm1000 /c
# ADD RSC /l 0x409 /d "NDEBUG"
LINK32=link.exe
# ADD LINK32 winmm.lib ole32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /nodefaultlib:"LIBCMTD" /out:"..\built\Final\ClientShellDLL\CShell.dll"

!ENDIF 

# Begin Target

# Name "ClientShellDLL - Win32 Final"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AimMagnetFX.cpp
# End Source File
# Begin Source File

SOURCE=.\ActivateObjectFX.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\ActivateTypeMgr.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\Animator.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\AssertMgr.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\AttachButeMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\AutoTargetMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\BaseLineSystemFX.cpp
# End Source File
# Begin Source File

SOURCE=.\BaseMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\BaseParticleSystemFX.cpp
# End Source File
# Begin Source File

SOURCE=.\BasePolyDrawFX.cpp
# End Source File
# Begin Source File

SOURCE=.\BaseScaleFX.cpp
# End Source File
# Begin Source File

SOURCE=.\BaseScreen.cpp
# End Source File
# Begin Source File

SOURCE=.\BeamFX.cpp
# End Source File
# Begin Source File

SOURCE=.\BodyFX.cpp
# End Source File
# Begin Source File

SOURCE=.\BulletTrailFX.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\ButeListReader.cpp
# End Source File
# Begin Source File

SOURCE=.\CameraOffsetMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\ChainedFX.cpp
# End Source File
# Begin Source File

SOURCE=.\CharacterFX.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\ChassisButeMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\ChassisFX.cpp
# End Source File
# Begin Source File

SOURCE=.\ChassisPieceFX.cpp
# End Source File
# Begin Source File

SOURCE=.\CheatMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\client_physics.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientButeMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientFXDB.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientFXMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientInfoMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientMultiplayerMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientSaveLoadMgr.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\ClientServerShared.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientSoundMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientTrackedNodeContext.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientTrackedNodeMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientUtilities.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientWeapon.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientWeaponMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientWeaponUtils.cpp
# End Source File
# Begin Source File

SOURCE=.\CMoveMgr.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\CommonUtilities.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\CRC32.cpp
# End Source File
# Begin Source File

SOURCE=.\Credits.cpp
# End Source File
# Begin Source File

SOURCE=.\CursorMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\DamageFXMgr.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\DamageTypes.cpp
# End Source File
# Begin Source File

SOURCE=.\DeathFX.cpp
# End Source File
# Begin Source File

SOURCE=.\DebrisFX.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\DebrisMgr.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\DebugLine.cpp
# End Source File
# Begin Source File

SOURCE=.\DebugLineFX.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\DebugNew.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\DynamicLightFX.cpp
# End Source File
# Begin Source File

SOURCE=.\DynamicOccluderVolumeFX.cpp
# End Source File
# Begin Source File

SOURCE=.\ExplosionFX.cpp
# End Source File
# Begin Source File

SOURCE=.\FireFX.cpp
# End Source File
# Begin Source File

SOURCE=.\FlashLight.cpp
# End Source File
# Begin Source File

SOURCE=.\FullScreenTint.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\FXButeMgr.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\FXStructs.cpp
# End Source File
# Begin Source File

SOURCE=.\GadgetDisabler.cpp
# End Source File
# Begin Source File

SOURCE=.\GadgetTargetFX.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\GameAlloc.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\GameButeMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\GameClientShell.cpp
# End Source File
# Begin Source File

SOURCE=.\GameSettings.cpp
# End Source File
# Begin Source File

SOURCE=.\GibFX.cpp
# End Source File
# Begin Source File

SOURCE=.\GlobalClientMgr.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\GlobalMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\GlobalsInit.cpp
# End Source File
# Begin Source File

SOURCE=.\HeadBobMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\HitBox.cpp
# End Source File
# Begin Source File

SOURCE=.\HUDAir.cpp
# End Source File
# Begin Source File

SOURCE=.\HUDAmmo.cpp
# End Source File
# Begin Source File

SOURCE=.\HUDBar.cpp
# End Source File
# Begin Source File

SOURCE=.\HUDBombTarget.cpp
# End Source File
# Begin Source File

SOURCE=.\HUDCarrying.cpp
# End Source File
# Begin Source File

SOURCE=.\HUDChatInput.cpp
# End Source File
# Begin Source File

SOURCE=.\HUDChooser.cpp
# End Source File
# Begin Source File

SOURCE=.\HUDCompass.cpp
# End Source File
# Begin Source File

SOURCE=.\HUDCrosshair.cpp
# End Source File
# Begin Source File

SOURCE=.\HUDDamage.cpp
# End Source File
# Begin Source File

SOURCE=.\HUDDamageDir.cpp
# End Source File
# Begin Source File

SOURCE=.\HUDDecision.cpp
# End Source File
# Begin Source File

SOURCE=.\HUDDisplayMeter.cpp
# End Source File
# Begin Source File

SOURCE=.\HUDDistance.cpp
# End Source File
# Begin Source File

SOURCE=.\HUDDoomsday.cpp
# End Source File
# Begin Source File

SOURCE=.\HUDHealth.cpp
# End Source File
# Begin Source File

SOURCE=.\HUDHiding.cpp
# End Source File
# Begin Source File

SOURCE=.\HUDHidingBar.cpp
# End Source File
# Begin Source File

SOURCE=.\HUDMessage.cpp
# End Source File
# Begin Source File

SOURCE=.\HUDMessageQueue.cpp
# End Source File
# Begin Source File

SOURCE=.\HUDMeter.cpp
# End Source File
# Begin Source File

SOURCE=.\HUDMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\HUDMissionText.cpp
# End Source File
# Begin Source File

SOURCE=.\HUDObjectives.cpp
# End Source File
# Begin Source File

SOURCE=.\HUDPaused.cpp
# End Source File
# Begin Source File

SOURCE=.\HUDPopup.cpp
# End Source File
# Begin Source File

SOURCE=.\HUDProgressBar.cpp
# End Source File
# Begin Source File

SOURCE=.\HUDRadar.cpp
# End Source File
# Begin Source File

SOURCE=.\HUDRadio.cpp
# End Source File
# Begin Source File

SOURCE=.\HUDRespawn.cpp
# End Source File
# Begin Source File

SOURCE=.\HUDScores.cpp
# End Source File
# Begin Source File

SOURCE=.\HUDSubtitles.cpp
# End Source File
# Begin Source File

SOURCE=.\HUDTransmission.cpp
# End Source File
# Begin Source File

SOURCE=.\HUDWeaponHeat.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\sdk\inc\iltbaseclass.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\IntelItemList.cpp
# End Source File
# Begin Source File

SOURCE=.\InterfaceMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\InterfaceResMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\InterfaceSurfMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\InterfaceTimer.cpp
# End Source File
# Begin Source File

SOURCE=.\IpMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\JumpVolumeFX.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\KeyMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\LaserBeam.cpp
# End Source File
# Begin Source File

SOURCE=.\LaserTriggerFX.cpp
# End Source File
# Begin Source File

SOURCE=.\LayoutMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\LeanMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\LensFlareFX.cpp
# End Source File
# Begin Source File

SOURCE=.\LightFX.cpp
# End Source File
# Begin Source File

SOURCE=.\LightGroupFX.cpp
# End Source File
# Begin Source File

SOURCE=.\LightningFX.cpp
# End Source File
# Begin Source File

SOURCE=.\LightScaleMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\LineSystemFX.cpp
# End Source File
# Begin Source File

SOURCE=.\LoadingScreen.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\sdk\inc\LTEulerAngles.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\Engine\sdk\inc\ltmodule.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\engine\sdk\inc\ltobjref.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\Engine\sdk\inc\ltquatbase.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\MarkSFX.cpp
# End Source File
# Begin Source File

SOURCE=.\MenuMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\MenuMission.cpp
# End Source File
# Begin Source File

SOURCE=.\MenuSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\MessageBox.cpp
# End Source File
# Begin Source File

SOURCE=.\MineFX.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\MissionButeMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\MissionMgr.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\ModelButeMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\Music.cpp
# End Source File
# Begin Source File

SOURCE=.\MuzzleFlashFX.cpp
# End Source File
# Begin Source File

SOURCE=.\MuzzleFlashParticleFX.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\NetDefs.cpp
# End Source File
# Begin Source File

SOURCE=.\NodeController.cpp
# End Source File
# Begin Source File

SOURCE=.\NodeLinesFX.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectiveControl.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjSpriteFX.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\ParsedMsg.cpp
# End Source File
# Begin Source File

SOURCE=.\ParticleExplosionFX.cpp
# End Source File
# Begin Source File

SOURCE=.\ParticleShowerFX.cpp
# End Source File
# Begin Source File

SOURCE=.\ParticleSystemFX.cpp
# End Source File
# Begin Source File

SOURCE=.\ParticleTrailFX.cpp
# End Source File
# Begin Source File

SOURCE=.\ParticleTrailSegmentFX.cpp
# End Source File
# Begin Source File

SOURCE=.\PerformanceMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\PerformanceTest.cpp
# End Source File
# Begin Source File

SOURCE=.\PickupItemFX.cpp
# End Source File
# Begin Source File

SOURCE=.\PlayerCamera.cpp
# End Source File
# Begin Source File

SOURCE=.\PlayerLureFX.cpp
# End Source File
# Begin Source File

SOURCE=.\PlayerMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\PlayerSoundFX.cpp
# End Source File
# Begin Source File

SOURCE=.\PlayerStats.cpp
# End Source File
# Begin Source File

SOURCE=.\PlayerVehicleFX.cpp
# End Source File
# Begin Source File

SOURCE=.\PlayerViewAttachmentMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\PolyDebrisFX.cpp
# End Source File
# Begin Source File

SOURCE=.\PolyGridFX.cpp
# End Source File
# Begin Source File

SOURCE=.\PolyLineFX.cpp
# End Source File
# Begin Source File

SOURCE=.\PopupMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\PopupText.cpp
# End Source File
# Begin Source File

SOURCE=.\ProfileMgr.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\ProfileUtils.cpp
# End Source File
# Begin Source File

SOURCE=.\ProjectileFX.cpp
# End Source File
# Begin Source File

SOURCE=.\PVFXMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\RadarObjectFX.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\RadarTypeMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\RagDoll.cpp
# End Source File
# Begin Source File

SOURCE=.\RagDollConstraint.cpp
# End Source File
# Begin Source File

SOURCE=.\RandomSparksFX.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\SaveLoadMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\ScatterFX.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\ScmdConsole.cpp
# End Source File
# Begin Source File

SOURCE=.\ScmdConsoleDriver_CShell.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenAudio.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenConfigure.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenControls.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenCrosshair.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenDisplay.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenEndCoopMission.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenEndDMMission.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenEndMission.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenFailure.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenGame.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenHost.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenHostDDOptions.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenHostDemolitionOptions.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenHostDMOptions.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenHostLevels.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenHostMission.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenHostOptions.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenHostTDMOptions.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenHostWeapons.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenJoin.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenJoystick.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenKeyboard.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenLoad.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenMain.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenMouse.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenMulti.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenOptions.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenPerformance.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenPlayer.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenPlayerTeam.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenPostload.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenPreload.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenProfile.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenSave.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenSingle.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenSpriteMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenTeam.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenTintMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\Searcher.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\SearchItemMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\SearchLightFX.cpp
# End Source File
# Begin Source File

SOURCE=.\SFXMgr.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\SharedFXStructs.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\SharedMission.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\SharedScoring.cpp
# End Source File
# Begin Source File

SOURCE=.\ShellCasingFX.cpp
# End Source File
# Begin Source File

SOURCE=.\SkillControl.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\SkillsButeMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\SmokeFX.cpp
# End Source File
# Begin Source File

SOURCE=.\SnowFX.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\SoundButeMgr.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\SoundFilterMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\SoundFX.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\SoundMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\Sparam.cpp
# End Source File
# Begin Source File

SOURCE=.\SpecialFXList.cpp
# End Source File
# Begin Source File

SOURCE=.\sprinklesfx.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\stacktrace.cpp
# End Source File
# Begin Source File

SOURCE=.\Stdafx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\SteamFX.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\SurfaceFunctions.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\SurfaceMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\TargetMgr.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\TeamMgr.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\TemplateList.cpp
# End Source File
# Begin Source File

SOURCE=.\TextureFXMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\TimedText.cpp
# End Source File
# Begin Source File

SOURCE=.\TO2ClientWeaponAllocator.cpp
# End Source File
# Begin Source File

SOURCE=.\TO2GameClientShell.cpp
# End Source File
# Begin Source File

SOURCE=.\TO2HUDMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\TO2InterfaceMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\TO2LayoutMgr.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\TO2\TO2MissionButeMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\TO2PlayerMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\TO2PlayerStats.cpp
# End Source File
# Begin Source File

SOURCE=.\TO2ScreenMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\TracerFX.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\TrackedNode.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\TrackedNodeContext.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\TrackedNodeControlFn.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\TrackedNodeMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\TransitionFXMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\TriggerFX.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\TriggerTypeMgr.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\UberAssert.cpp
# End Source File
# Begin Source File

SOURCE=.\VehicleMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\VehicleWeapon.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\VehicleButeMgr.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\VersionMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\VisionModeMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\VKDefs.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\WaveFn.cpp
# End Source File
# Begin Source File

SOURCE=.\WeaponChooser.cpp
# End Source File
# Begin Source File

SOURCE=.\WeaponFX.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\WeaponMgr.cpp
# ADD CPP /Ox /GF
# SUBTRACT CPP /O2
# End Source File
# Begin Source File

SOURCE=.\WeatherFX.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\WinUtil.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AimMagnetFX.h
# End Source File
# Begin Source File

SOURCE=.\ActivateObjectFX.h
# End Source File
# Begin Source File

SOURCE=..\Shared\ActivateTypeMgr.h
# End Source File
# Begin Source File

SOURCE=..\Shared\ActivationData.h
# End Source File
# Begin Source File

SOURCE=.\AutoTargetMgr.h
# End Source File
# Begin Source File

SOURCE=.\BaseLineSystemFX.h
# End Source File
# Begin Source File

SOURCE=.\BaseMenu.h
# End Source File
# Begin Source File

SOURCE=.\BaseParticleSystemFX.h
# End Source File
# Begin Source File

SOURCE=.\BasePolyDrawFX.h
# End Source File
# Begin Source File

SOURCE=.\BaseScaleFX.h
# End Source File
# Begin Source File

SOURCE=.\BaseScreen.h
# End Source File
# Begin Source File

SOURCE=.\BeamFX.h
# End Source File
# Begin Source File

SOURCE=.\BodyFX.h
# End Source File
# Begin Source File

SOURCE=.\BulletTrailFX.h
# End Source File
# Begin Source File

SOURCE=.\CameraFX.h
# End Source File
# Begin Source File

SOURCE=.\CameraOffsetMgr.h
# End Source File
# Begin Source File

SOURCE=.\ChainedFX.h
# End Source File
# Begin Source File

SOURCE=.\CharacterFX.h
# End Source File
# Begin Source File

SOURCE=..\Shared\ChassisButeMgr.h
# End Source File
# Begin Source File

SOURCE=.\ChassisFX.h
# End Source File
# Begin Source File

SOURCE=.\ChassisPieceFX.h
# End Source File
# Begin Source File

SOURCE=.\CheatMgr.h
# End Source File
# Begin Source File

SOURCE=.\client_physics.h
# End Source File
# Begin Source File

SOURCE=.\ClientButeMgr.h
# End Source File
# Begin Source File

SOURCE=.\ClientFXDB.h
# End Source File
# Begin Source File

SOURCE=.\ClientFXMgr.h
# End Source File
# Begin Source File

SOURCE=.\ClientInfoMgr.h
# End Source File
# Begin Source File

SOURCE=.\ClientMultiplayerMgr.h
# End Source File
# Begin Source File

SOURCE=.\ClientSaveLoadMgr.h
# End Source File
# Begin Source File

SOURCE=..\Shared\ClientServerShared.h
# End Source File
# Begin Source File

SOURCE=.\ClientSoundMgr.h
# End Source File
# Begin Source File

SOURCE=.\ClientTrackedNodeContext.h
# End Source File
# Begin Source File

SOURCE=.\ClientTrackedNodeMgr.h
# End Source File
# Begin Source File

SOURCE=.\ClientUtilities.h
# End Source File
# Begin Source File

SOURCE=.\ClientWeapon.h
# End Source File
# Begin Source File

SOURCE=.\ClientWeaponAllocator.h
# End Source File
# Begin Source File

SOURCE=.\ClientWeaponBase.h
# End Source File
# Begin Source File

SOURCE=.\ClientWeaponMgr.h
# End Source File
# Begin Source File

SOURCE=.\ClientWeaponNone.h
# End Source File
# Begin Source File

SOURCE=.\ClientWeaponUtils.h
# End Source File
# Begin Source File

SOURCE=.\CMoveMgr.h
# End Source File
# Begin Source File

SOURCE=..\Shared\CRC32.h
# End Source File
# Begin Source File

SOURCE=.\Credits.h
# End Source File
# Begin Source File

SOURCE=.\CursorMgr.h
# End Source File
# Begin Source File

SOURCE=.\DamageFXMgr.h
# End Source File
# Begin Source File

SOURCE=.\DeathFX.h
# End Source File
# Begin Source File

SOURCE=.\DebrisFX.h
# End Source File
# Begin Source File

SOURCE=.\DebugLineFX.h
# End Source File
# Begin Source File

SOURCE=.\DynamicLightFX.h
# End Source File
# Begin Source File

SOURCE=.\ExplosionFX.h
# End Source File
# Begin Source File

SOURCE=.\FastList.h
# End Source File
# Begin Source File

SOURCE=.\FireFX.h
# End Source File
# Begin Source File

SOURCE=.\FlashLight.h
# End Source File
# Begin Source File

SOURCE=.\FullScreenTint.h
# End Source File
# Begin Source File

SOURCE=.\GadgetDisabler.h
# End Source File
# Begin Source File

SOURCE=.\GadgetTargetFX.h
# End Source File
# Begin Source File

SOURCE=.\GameButes.h
# End Source File
# Begin Source File

SOURCE=.\GameClientShell.h
# End Source File
# Begin Source File

SOURCE=.\GameSettings.h
# End Source File
# Begin Source File

SOURCE=.\GibFX.h
# End Source File
# Begin Source File

SOURCE=.\GlobalClientMgr.h
# End Source File
# Begin Source File

SOURCE=.\HeadBobMgr.h
# End Source File
# Begin Source File

SOURCE=.\HitBox.h
# End Source File
# Begin Source File

SOURCE=.\HUDAir.h
# End Source File
# Begin Source File

SOURCE=.\HUDAmmo.h
# End Source File
# Begin Source File

SOURCE=.\HUDBar.h
# End Source File
# Begin Source File

SOURCE=.\HUDBombTarget.h
# End Source File
# Begin Source File

SOURCE=.\HUDCarrying.h
# End Source File
# Begin Source File

SOURCE=.\HUDChatInput.h
# End Source File
# Begin Source File

SOURCE=.\HUDChooser.h
# End Source File
# Begin Source File

SOURCE=.\HUDCompass.h
# End Source File
# Begin Source File

SOURCE=.\HUDCrosshair.h
# End Source File
# Begin Source File

SOURCE=.\HUDDamage.h
# End Source File
# Begin Source File

SOURCE=.\HUDDamageDir.h
# End Source File
# Begin Source File

SOURCE=.\HUDDecision.h
# End Source File
# Begin Source File

SOURCE=.\HUDDisplayMeter.h
# End Source File
# Begin Source File

SOURCE=.\HUDDistance.h
# End Source File
# Begin Source File

SOURCE=.\HUDDoomsday.h
# End Source File
# Begin Source File

SOURCE=.\HUDHealth.h
# End Source File
# Begin Source File

SOURCE=.\HUDHiding.h
# End Source File
# Begin Source File

SOURCE=.\HUDHidingBar.h
# End Source File
# Begin Source File

SOURCE=.\HUDItem.h
# End Source File
# Begin Source File

SOURCE=.\HUDMessage.h
# End Source File
# Begin Source File

SOURCE=.\HUDMessageQueue.h
# End Source File
# Begin Source File

SOURCE=.\HUDMeter.h
# End Source File
# Begin Source File

SOURCE=.\HUDMgr.h
# End Source File
# Begin Source File

SOURCE=.\HUDMissionText.h
# End Source File
# Begin Source File

SOURCE=.\HUDObjectives.h
# End Source File
# Begin Source File

SOURCE=.\HUDPaused.h
# End Source File
# Begin Source File

SOURCE=.\HUDPopup.h
# End Source File
# Begin Source File

SOURCE=.\HUDProgressBar.h
# End Source File
# Begin Source File

SOURCE=.\HUDRadar.h
# End Source File
# Begin Source File

SOURCE=.\HUDRadio.h
# End Source File
# Begin Source File

SOURCE=.\HUDRespawn.h
# End Source File
# Begin Source File

SOURCE=.\HUDScores.h
# End Source File
# Begin Source File

SOURCE=.\HUDSubtitles.h
# End Source File
# Begin Source File

SOURCE=.\HUDTransmission.h
# End Source File
# Begin Source File

SOURCE=.\HUDWeaponHeat.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\sdk\inc\iltbaseclass.h
# End Source File
# Begin Source File

SOURCE=..\Shared\ImpactType.h
# End Source File
# Begin Source File

SOURCE=.\IntelItemList.h
# End Source File
# Begin Source File

SOURCE=.\InterfaceMgr.h
# End Source File
# Begin Source File

SOURCE=.\InterfaceResMgr.h
# End Source File
# Begin Source File

SOURCE=.\InterfaceSurfMgr.h
# End Source File
# Begin Source File

SOURCE=.\InterfaceTimer.h
# End Source File
# Begin Source File

SOURCE=.\IpMgr.h
# End Source File
# Begin Source File

SOURCE=..\Libs\ServerDir\IServerDir.h
# End Source File
# Begin Source File

SOURCE=.\JumpVolumeFX.h
# End Source File
# Begin Source File

SOURCE=..\Shared\KeyMgr.h
# End Source File
# Begin Source File

SOURCE=.\LaserBeam.h
# End Source File
# Begin Source File

SOURCE=.\LaserTriggerFX.h
# End Source File
# Begin Source File

SOURCE=.\LayoutMgr.h
# End Source File
# Begin Source File

SOURCE=.\LeanMgr.h
# End Source File
# Begin Source File

SOURCE=.\LensFlareFX.h
# End Source File
# Begin Source File

SOURCE=.\LightFX.h
# End Source File
# Begin Source File

SOURCE=.\LightGroupFX.h
# End Source File
# Begin Source File

SOURCE=.\LightningFX.h
# End Source File
# Begin Source File

SOURCE=.\LightScaleMgr.h
# End Source File
# Begin Source File

SOURCE=.\LineSystemFX.h
# End Source File
# Begin Source File

SOURCE=.\LoadingScreen.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\sdk\inc\LTEulerAngles.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\sdk\inc\ltmodule.h
# End Source File
# Begin Source File

SOURCE=..\..\engine\sdk\inc\ltobjref.h
# End Source File
# Begin Source File

SOURCE=..\Shared\LTPoly.h
# End Source File
# Begin Source File

SOURCE=.\MarkSFX.h
# End Source File
# Begin Source File

SOURCE=.\MenuEnum.h
# End Source File
# Begin Source File

SOURCE=.\MenuMgr.h
# End Source File
# Begin Source File

SOURCE=.\MenuMission.h
# End Source File
# Begin Source File

SOURCE=.\MenuSystem.h
# End Source File
# Begin Source File

SOURCE=.\MessageBox.h
# End Source File
# Begin Source File

SOURCE=.\MineFX.h
# End Source File
# Begin Source File

SOURCE=..\Shared\MissionButeMgr.h
# End Source File
# Begin Source File

SOURCE=.\MissionMgr.h
# End Source File
# Begin Source File

SOURCE=.\Music.h
# End Source File
# Begin Source File

SOURCE=.\MuzzleFlashFX.h
# End Source File
# Begin Source File

SOURCE=.\MuzzleFlashParticleFX.h
# End Source File
# Begin Source File

SOURCE=.\NodeController.h
# End Source File
# Begin Source File

SOURCE=.\NodeLinesFX.h
# End Source File
# Begin Source File

SOURCE=.\ObjectiveControl.h
# End Source File
# Begin Source File

SOURCE=.\ObjSpriteFX.h
# End Source File
# Begin Source File

SOURCE=.\Overlays.h
# End Source File
# Begin Source File

SOURCE=.\ParticleExplosionFX.h
# End Source File
# Begin Source File

SOURCE=.\ParticleShowerFX.h
# End Source File
# Begin Source File

SOURCE=.\ParticleSystemFX.h
# End Source File
# Begin Source File

SOURCE=.\ParticleTrailFX.h
# End Source File
# Begin Source File

SOURCE=.\ParticleTrailSegmentFX.h
# End Source File
# Begin Source File

SOURCE=.\PerformanceEnum.h
# End Source File
# Begin Source File

SOURCE=.\PerformanceMgr.h
# End Source File
# Begin Source File

SOURCE=.\PerformanceTest.h
# End Source File
# Begin Source File

SOURCE=.\PickupItemFX.h
# End Source File
# Begin Source File

SOURCE=.\plasma.h
# End Source File
# Begin Source File

SOURCE=.\PlayerCamera.h
# End Source File
# Begin Source File

SOURCE=.\PlayerLureFX.h
# End Source File
# Begin Source File

SOURCE=.\PlayerMgr.h
# End Source File
# Begin Source File

SOURCE=.\PlayerSoundFX.h
# End Source File
# Begin Source File

SOURCE=.\PlayerStats.h
# End Source File
# Begin Source File

SOURCE=.\PlayerVehicleFX.h
# End Source File
# Begin Source File

SOURCE=.\PlayerViewAttachmentMgr.h
# End Source File
# Begin Source File

SOURCE=.\PolyDebrisFX.h
# End Source File
# Begin Source File

SOURCE=.\PolyGridFX.h
# End Source File
# Begin Source File

SOURCE=.\PolyLineFX.h
# End Source File
# Begin Source File

SOURCE=.\PopupMgr.h
# End Source File
# Begin Source File

SOURCE=.\PopupText.h
# End Source File
# Begin Source File

SOURCE=.\ProfileMgr.h
# End Source File
# Begin Source File

SOURCE=..\Shared\ProfileUtils.h
# End Source File
# Begin Source File

SOURCE=.\ProjectileFX.h
# End Source File
# Begin Source File

SOURCE=.\PVFXMgr.h
# End Source File
# Begin Source File

SOURCE=.\RadarObjectFX.h
# End Source File
# Begin Source File

SOURCE=..\Shared\RadarTypeMgr.h
# End Source File
# Begin Source File

SOURCE=.\RagDoll.h
# End Source File
# Begin Source File

SOURCE=.\RagDollConstraint.h
# End Source File
# Begin Source File

SOURCE=.\RagDollNode.h
# End Source File
# Begin Source File

SOURCE=.\RandomSparksFX.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=..\Shared\SaveLoadMgr.h
# End Source File
# Begin Source File

SOURCE=.\ScatterFX.h
# End Source File
# Begin Source File

SOURCE=..\Shared\ScmdConsole.h
# End Source File
# Begin Source File

SOURCE=.\ScmdConsoleDriver_CShell.h
# End Source File
# Begin Source File

SOURCE=..\Shared\ScmdShared.h
# End Source File
# Begin Source File

SOURCE=.\ScreenAudio.h
# End Source File
# Begin Source File

SOURCE=.\ScreenCommands.h
# End Source File
# Begin Source File

SOURCE=.\ScreenConfigure.h
# End Source File
# Begin Source File

SOURCE=.\ScreenControls.h
# End Source File
# Begin Source File

SOURCE=.\ScreenCrosshair.h
# End Source File
# Begin Source File

SOURCE=.\ScreenDisplay.h
# End Source File
# Begin Source File

SOURCE=.\ScreenEndCoopMission.h
# End Source File
# Begin Source File

SOURCE=.\ScreenEndDMMission.h
# End Source File
# Begin Source File

SOURCE=.\ScreenEndMission.h
# End Source File
# Begin Source File

SOURCE=.\ScreenEnum.h
# End Source File
# Begin Source File

SOURCE=.\ScreenFailure.h
# End Source File
# Begin Source File

SOURCE=.\ScreenGame.h
# End Source File
# Begin Source File

SOURCE=.\ScreenHost.h
# End Source File
# Begin Source File

SOURCE=.\ScreenHostDDOptions.h
# End Source File
# Begin Source File

SOURCE=.\ScreenHostDemolitionOptions.h
# End Source File
# Begin Source File

SOURCE=.\ScreenHostDMOptions.h
# End Source File
# Begin Source File

SOURCE=.\ScreenHostLevels.h
# End Source File
# Begin Source File

SOURCE=.\ScreenHostMission.h
# End Source File
# Begin Source File

SOURCE=.\ScreenHostOptions.h
# End Source File
# Begin Source File

SOURCE=.\ScreenHostTDMOptions.h
# End Source File
# Begin Source File

SOURCE=.\ScreenHostWeapons.h
# End Source File
# Begin Source File

SOURCE=.\ScreenJoin.h
# End Source File
# Begin Source File

SOURCE=.\ScreenJoystick.h
# End Source File
# Begin Source File

SOURCE=.\ScreenKeyboard.h
# End Source File
# Begin Source File

SOURCE=.\ScreenLoad.h
# End Source File
# Begin Source File

SOURCE=.\ScreenMain.h
# End Source File
# Begin Source File

SOURCE=.\ScreenMgr.h
# End Source File
# Begin Source File

SOURCE=.\ScreenMouse.h
# End Source File
# Begin Source File

SOURCE=.\ScreenMulti.h
# End Source File
# Begin Source File

SOURCE=.\ScreenOptions.h
# End Source File
# Begin Source File

SOURCE=.\ScreenPerformance.h
# End Source File
# Begin Source File

SOURCE=.\ScreenPlayer.h
# End Source File
# Begin Source File

SOURCE=.\ScreenPlayerTeam.h
# End Source File
# Begin Source File

SOURCE=.\ScreenPostload.h
# End Source File
# Begin Source File

SOURCE=.\ScreenPreload.h
# End Source File
# Begin Source File

SOURCE=.\ScreenProfile.h
# End Source File
# Begin Source File

SOURCE=.\ScreenSave.h
# End Source File
# Begin Source File

SOURCE=.\ScreenSingle.h
# End Source File
# Begin Source File

SOURCE=.\ScreenSpriteMgr.h
# End Source File
# Begin Source File

SOURCE=.\ScreenTeam.h
# End Source File
# Begin Source File

SOURCE=.\ScreenTintMgr.h
# End Source File
# Begin Source File

SOURCE=.\Searcher.h
# End Source File
# Begin Source File

SOURCE=..\Shared\SearchItemMgr.h
# End Source File
# Begin Source File

SOURCE=.\SearchLightFX.h
# End Source File
# Begin Source File

SOURCE=.\SFXMgr.h
# End Source File
# Begin Source File

SOURCE=.\SharedDefs.h
# End Source File
# Begin Source File

SOURCE=..\Shared\SharedMission.h
# End Source File
# Begin Source File

SOURCE=..\Shared\SharedScoring.h
# End Source File
# Begin Source File

SOURCE=.\ShellCasingFX.h
# End Source File
# Begin Source File

SOURCE=.\SkillControl.h
# End Source File
# Begin Source File

SOURCE=.\SmokeFX.h
# End Source File
# Begin Source File

SOURCE=.\SnowFX.h
# End Source File
# Begin Source File

SOURCE=.\SoundFX.h
# End Source File
# Begin Source File

SOURCE=.\Sparam.h
# End Source File
# Begin Source File

SOURCE=.\SpecialFX.h
# End Source File
# Begin Source File

SOURCE=.\SpecialFXList.h
# End Source File
# Begin Source File

SOURCE=.\sprinklesfx.h
# End Source File
# Begin Source File

SOURCE=.\stack.h
# End Source File
# Begin Source File

SOURCE=.\StandardCreator.h
# End Source File
# Begin Source File

SOURCE=.\Stdafx.h
# End Source File
# Begin Source File

SOURCE=.\SteamFX.h
# End Source File
# Begin Source File

SOURCE=.\SystemDependant.h
# End Source File
# Begin Source File

SOURCE=.\TargetMgr.h
# End Source File
# Begin Source File

SOURCE=..\Shared\TeamMgr.h
# End Source File
# Begin Source File

SOURCE=.\TextureFXMgr.h
# End Source File
# Begin Source File

SOURCE=.\TimedText.h
# End Source File
# Begin Source File

SOURCE=.\TO2ClientWeaponAllocator.h
# End Source File
# Begin Source File

SOURCE=.\TO2GameClientShell.h
# End Source File
# Begin Source File

SOURCE=.\TO2HUDMgr.h
# End Source File
# Begin Source File

SOURCE=.\TO2InterfaceMgr.h
# End Source File
# Begin Source File

SOURCE=.\TO2LayoutMgr.h
# End Source File
# Begin Source File

SOURCE=..\Shared\TO2\TO2MissionButeMgr.h
# End Source File
# Begin Source File

SOURCE=.\TO2PlayerMgr.h
# End Source File
# Begin Source File

SOURCE=.\TO2PlayerStats.h
# End Source File
# Begin Source File

SOURCE=.\TO2ScreenMgr.h
# End Source File
# Begin Source File

SOURCE=.\TracerFX.h
# End Source File
# Begin Source File

SOURCE=..\Shared\TrackedNodeContext.h
# End Source File
# Begin Source File

SOURCE=.\TriggerFX.h
# End Source File
# Begin Source File

SOURCE=..\Shared\TriggerTypeMgr.h
# End Source File
# Begin Source File

SOURCE=..\Shared\UberAssert.h
# End Source File
# Begin Source File

SOURCE=.\VarTrack.h
# End Source File
# Begin Source File

SOURCE=.\VehicleMgr.h
# End Source File
# Begin Source File

SOURCE=.\VehicleWeapon.h
# End Source File
# Begin Source File

SOURCE=.\VisionModeMgr.h
# End Source File
# Begin Source File

SOURCE=.\VKDefs.h
# End Source File
# Begin Source File

SOURCE=.\VolumeBrushFX.h
# End Source File
# Begin Source File

SOURCE=..\Shared\WaveFn.h
# End Source File
# Begin Source File

SOURCE=.\WeaponChooser.h
# End Source File
# Begin Source File

SOURCE=.\WeaponFX.h
# End Source File
# Begin Source File

SOURCE=.\WeaponStringDefs.h
# End Source File
# Begin Source File

SOURCE=.\WeatherFX.h
# End Source File
# Begin Source File

SOURCE=..\Shared\WinUtil.h
# End Source File
# End Group
# Begin Group "Libraries"

# PROP Default_Filter "lib"
# Begin Source File

SOURCE=..\built\Final\libs\GameSpy\GameSpy.lib
# End Source File
# Begin Source File

SOURCE=..\..\libs\built\Final\ButeMgr\ButeMgr.lib
# End Source File
# Begin Source File

SOURCE=..\..\libs\built\Final\CryptMgr\cryptmgr.lib
# End Source File
# Begin Source File

SOURCE=..\..\libs\built\Final\Lib_Lith\Lib_Lith.lib
# End Source File
# Begin Source File

SOURCE=..\..\libs\built\Final\Lib_StdLith\Lib_StdLith.lib
# End Source File
# Begin Source File

SOURCE=..\built\Final\libs\LTGuiMgr\ltguimgr.lib
# End Source File
# Begin Source File

SOURCE=..\..\libs\built\Final\MFCStub\MFCStub.lib
# End Source File
# Begin Source File

SOURCE=..\..\libs\built\Final\RegMgr\regmgr.lib
# End Source File
# End Group
# End Target
# End Project
