//------------------------------------------------------------------
//
//	File	  : ServerEvent.cpp
//
//	Purpose	  : 
//
//	Created	  : February 14 1997
//
//	Copyright : MONOLITH Inc 1997 All Rights Reserved
//
//------------------------------------------------------------------

// Includes....
#include "bdefs.h"
#include "serverevent.h"
#include "servermgr.h"


void CServerEvent::DecrementRefCount()
{
	LTLink *pCur, *pNext;
	ClientStructNode *pNode;

	--m_RefCount;
	if( m_RefCount == 0 )
	{
		pCur = m_ClientStructNodeList.m_Head.m_pNext;
		while( pCur != &m_ClientStructNodeList.m_Head )
		{
			pNext = pCur->m_pNext;
			pNode = ( ClientStructNode * )pCur->m_pData;
			// RETAIL: each link is taken off the list first (Lithtech.exe 0x46c690)
			dl_RemoveAt( &m_ClientStructNodeList, pCur );
			sb_Free( &g_pServerMgr->m_ClientStructNodeBank, pNode );
			pCur = pNext;
		}
		dl_InitList( &m_ClientStructNodeList );

		sb_Free(&g_pServerMgr->m_ServerEventBank, this);
	}
}




