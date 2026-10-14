// ----------------------------------------------------------------------- //
//
// MODULE  : GameSpyPatchInfo.cpp
//
// PURPOSE : IGameSpyPatchInfo on GameSpy's patching and tracking SDK.
//
// cjacksrc: Monolith's source is lost; reconstructed from the retail
// GameSpy.dll (1.1), 0x10003430..0x1000388b.
//
// ----------------------------------------------------------------------- //

#include "GameSpyPatchInfo.h"
#include "GameSpySDK/ghttp/ghttp.h"

// The request thread's beat.
#define THREAD_THINK_PERIOD		10

// How long a dying thread is waited for.
#define THREAD_END_TIMEOUT		1000

// RETAIL: 0x10003430
void IGameSpyPatchInfo::Delete( IGameSpyPatchInfo* pGameSpyPatchInfo )
{
	if( pGameSpyPatchInfo )
		delete pGameSpyPatchInfo;
}

// RETAIL: 0x10003440
IGameSpyPatchInfo::EBrowserStatus CGameSpyPatchInfo::GetPatchInfoStatus( )
{
	CWinSync_CSAuto lockData( m_csData );
	return m_eStatus;
}

// RETAIL: 0x10003460
DWORD CGameSpyPatchInfo::ThreadMain( )
{
	m_ThreadStartedEvent.Set( );

	ghttpThink( );
	while( m_eStatus == eBrowserStatus_Processing &&
		WaitForSingleObject( m_ThreadEndEvent.GetEvent( ), THREAD_THINK_PERIOD ) == WAIT_TIMEOUT )
	{
		ghttpThink( );
	}

	return 0;
}

// RETAIL: 0x100034a0
bool CGameSpyPatchInfo::StopThread( )
{
	if( m_hThread )
	{
		m_ThreadEndEvent.Set( );
		if( WaitForSingleObject( m_hThread, THREAD_END_TIMEOUT ) == WAIT_TIMEOUT )
			TerminateThread( m_hThread, 0 );
		CloseHandle( m_hThread );
		m_hThread = NULL;
	}

	return true;
}

// RETAIL: 0x100034f0
void CGameSpyPatchInfo::PatchCallback( PTBool bAvailable, PTBool bMandatory, const char* pszVersionName,
	int nFileId, const char* pszDownloadURL, void* pParam )
{
	CGameSpyPatchInfo* pGameSpyPatchInfo = ( CGameSpyPatchInfo* )pParam;

	CWinSync_CSAuto lockData( pGameSpyPatchInfo->m_csData );

	pGameSpyPatchInfo->m_PatchInfoResults.m_bNewVersion = ( bAvailable == PTTrue );
	pGameSpyPatchInfo->m_PatchInfoResults.m_bMandatory = ( bMandatory == PTTrue );
	pGameSpyPatchInfo->m_PatchInfoResults.m_sVersionName = pszVersionName;
	pGameSpyPatchInfo->m_PatchInfoResults.m_sURL = pszDownloadURL;
	pGameSpyPatchInfo->m_eStatus = eBrowserStatus_Complete;
}

// RETAIL: 0x10003550
CGameSpyPatchInfo::CGameSpyPatchInfo( StartupInfo const& startupInfo )
{
	m_hThread = NULL;
	m_StartupInfo = startupInfo;
	m_eStatus = eBrowserStatus_Idle;
}

// RETAIL: 0x100035e0
CGameSpyPatchInfo::~CGameSpyPatchInfo( )
{
	if( m_hThread )
	{
		m_ThreadEndEvent.Set( );
		if( WaitForSingleObject( m_hThread, THREAD_END_TIMEOUT ) == WAIT_TIMEOUT )
			TerminateThread( m_hThread, 0 );
		CloseHandle( m_hThread );
		m_hThread = NULL;
	}
}

// RETAIL: 0x10003670
bool CGameSpyPatchInfo::HaltRequest( )
{
	StopThread( );
	m_eStatus = eBrowserStatus_Idle;
	return true;
}

// RETAIL: 0x100036d0
bool CGameSpyPatchInfo::GetPatchInfoResults( PatchInfoResults& patchInfoResults )
{
	CWinSync_CSAuto lockData( m_csData );

	if( m_eStatus == eBrowserStatus_Processing )
		return false;

	patchInfoResults = m_PatchInfoResults;
	return true;
}

// RETAIL: 0x10003730
DWORD WINAPI CGameSpyPatchInfo::ThreadProc( void* pParam )
{
	return (( CGameSpyPatchInfo* )pParam )->ThreadMain( );
}

// RETAIL: 0x10003740
IGameSpyPatchInfo* IGameSpyPatchInfo::Create( StartupInfo const& startupInfo )
{
	return new CGameSpyPatchInfo( startupInfo );
}

// RETAIL: 0x10003770
bool CGameSpyPatchInfo::RequestPatchInfo( )
{
	if( !HaltRequest( ))
		return false;

	m_eStatus = eBrowserStatus_Error;

	int nProductId;
	if( !CGameSpyCommon::Instance( ).GetProductId( m_StartupInfo.m_nGameType, &nProductId ))
		return false;

	m_eStatus = eBrowserStatus_Processing;

	DWORD dwThreadId;
	m_hThread = CreateThread( NULL, 0, ThreadProc, this, 0, &dwThreadId );
	WaitForSingleObject( m_ThreadStartedEvent.GetEvent( ), INFINITE );

	CWinSync_CSAuto lockData( m_csData );

	if( !ptCheckForPatchAndTrackUsage( 0, nProductId, m_StartupInfo.m_sVersion.c_str( ),
		m_StartupInfo.m_nDistributionId, PatchCallback, PTFalse, this ))
	{
		m_eStatus = eBrowserStatus_Error;
		StopThread( );
		return false;
	}

	return true;
}
