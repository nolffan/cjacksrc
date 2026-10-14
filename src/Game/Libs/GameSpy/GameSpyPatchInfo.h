// ----------------------------------------------------------------------- //
//
// MODULE  : GameSpyPatchInfo.h
//
// PURPOSE : IGameSpyPatchInfo on GameSpy's patching and tracking SDK.
//
// cjacksrc: reconstructed from the retail GameSpy.dll (1.1); the layout is
// the retail object's (0x8c bytes), the vtable 0x10016490.
//
// ----------------------------------------------------------------------- //

#ifndef __GAMESPYPATCHINFO_H__
#define __GAMESPYPATCHINFO_H__

#include <string>
#include "IGameSpy.h"
#include "GameSpyCommon.h"
#include "WinSync.h"
#include "GameSpySDK/pt/pt.h"

class CGameSpyPatchInfo : public IGameSpyPatchInfo
{
	public:

		CGameSpyPatchInfo( StartupInfo const& startupInfo );
		virtual ~CGameSpyPatchInfo( );

		virtual bool RequestPatchInfo( );
		virtual bool HaltRequest( );
		virtual EBrowserStatus GetPatchInfoStatus( );
		virtual bool GetPatchInfoResults( PatchInfoResults& patchInfoResults );

	private:

		bool StopThread( );

		// The request's thread: thinks the HTTP SDK every 10 ms.
		static DWORD WINAPI ThreadProc( void* pParam );
		DWORD ThreadMain( );

		static void PatchCallback( PTBool bAvailable, PTBool bMandatory, const char* pszVersionName,
			int nFileId, const char* pszDownloadURL, void* pParam );

	private:

		StartupInfo				m_StartupInfo;			// 0x04
		HANDLE					m_hThread;				// 0x28
		CWinSync_Event			m_ThreadEndEvent;		// 0x2c
		CWinSync_PulseEvent		m_ThreadStartedEvent;	// 0x30
		CWinSync_CS				m_csData;				// 0x34
		EBrowserStatus			m_eStatus;				// 0x4c
		PatchInfoResults		m_PatchInfoResults;		// 0x50
};

#endif // __GAMESPYPATCHINFO_H__
