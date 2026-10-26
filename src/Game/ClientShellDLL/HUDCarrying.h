// ----------------------------------------------------------------------- //
//
// MODULE  : HUDCarrying.h
//
// PURPOSE : HUDItem to display an icon while carrying a body
//
// (c) 2002 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#ifndef __HUD_CARRYING_H
#define __HUD_CARRYING_H

#include "HUDItem.h"



//******************************************************************************************
//** HUD Carry Icon display
//******************************************************************************************
class CHUDCarrying : public CHUDItem
{
public:
	CHUDCarrying();

    LTBOOL      Init();
	void		Term();

    void        Render();
    void        Update();

    void        UpdateLayout();

	// cjacksrc: Contract J.A.C.K. shows the keys the player holds beside
	// the carrying icon (CShell.dll 0x10052570, 0x100522d0, 0x10052340).
	bool		AddKey( uint32 nKeyId );
	bool		RemoveKey( uint32 nKeyId );
	void		ClearKeys( );

private:
    LTIntPt		m_BasePos;
	uint16		m_nSize;

	LTPoly_GT4	m_Poly;

	HTEXTURE	m_hBodyCanDrop;
	HTEXTURE	m_hBodyNoDrop;
	HTEXTURE	m_hBodyCanCarry;

	// The carrying and pickup icons of each chassis piece, by its id.
	struct ChassisPieceIcons
	{
		// RETAIL: CShell 0x1005244b clears them.
		ChassisPieceIcons( ) : hCarrying( LTNULL ), hPickup( LTNULL ) { }

		HTEXTURE	hCarrying;
		HTEXTURE	hPickup;
	};
	typedef std::vector< ChassisPieceIcons > ChassisPieceIconList;
	ChassisPieceIconList	m_ChassisPieceIcons;

	// The keys held, with their icons' sizes.
	struct KeyIcon
	{
		KeyIcon( ) : hIcon( LTNULL ), nKeyId( 0 ), nWidth( 0 ), nHeight( 0 ) { }

		HTEXTURE	hIcon;
		uint32		nKeyId;
		uint32		nWidth;
		uint32		nHeight;
	};
	typedef std::vector< KeyIcon > KeyIconList;
	KeyIconList		m_KeyIcons;
};

#endif