// ----------------------------------------------------------------------- //
//
// MODULE  : ChassisButeMgr.h
//
// PURPOSE : The chassis pieces of attributes\chassis.txt: what a piece
//           looks like as a prop, where it goes on its chassis, and how
//           it shows on the radar and the HUD.
//
// cjacksrc: Contract J.A.C.K.'s own; reconstructed from CShell.dll
// (0x10017640..0x10017c60) and Object.lto, which both have it.
//
// ----------------------------------------------------------------------- //

#ifndef __CHASSIS_BUTE_MGR_H__
#define __CHASSIS_BUTE_MGR_H__

#include "GameButeMgr.h"

#define CHASSISBMGR_DEFAULT_FILE	"attributes\\chassis.txt"
#define CHASSISBMGR_INVALID_ID		0xFF

// One ChassisPiece<n> record (0x138 bytes).
struct CHASSISPIECE
{
	CHASSISPIECE();
	~CHASSISPIECE();

	bool		Read( CButeMgr &buteMgr, char const* pszTag, uint8 nId );

	std::string	m_sName;
	std::string	m_sPropType;
	std::string	m_sPropTypeTarget;
	std::string	m_sPropTypeReplaced;
	std::string	m_sChassisSocket;
	uint8		m_nId;
	bool		m_bHeavy;
	std::string	m_sRadarType;
	std::string	m_sCarryingIcon;
	std::string	m_sPickupIcon;
	std::string	m_sFadeFX;
	std::string	m_sSpawnFX;
	std::string	m_sUnspawnFX;
};

class ChassisButeMgr : public CGameButeMgr
{
	private :

		// Not allowed to create directly.  Use Instance().
		ChassisButeMgr();

		// Copy ctor and assignment operator not implemented and should never be used.
		ChassisButeMgr( ChassisButeMgr const& other );
		ChassisButeMgr& operator=( ChassisButeMgr const& other );

	public :

		~ChassisButeMgr();

		// Call this to get the singleton instance of the chassis mgr.
		static ChassisButeMgr& Instance( );

		LTBOOL		Init( const char* szAttributeFile = "" );
		bool		IsInitialized( ) const { return m_bInitialized; }
		void		Term( );

		uint32				GetNumPieces( ) const { return m_Pieces.size( ); }
		CHASSISPIECE const*	GetPiece( uint8 nId ) const { return m_Pieces[nId]; }
		CHASSISPIECE const*	GetPiece( char const* pszName );
		uint8				GetPieceId( char const* pszName );

		typedef std::vector< CHASSISPIECE* > PieceList;
		PieceList const&	GetPieceList( ) const { return m_Pieces; }

	private :

		bool		m_bInitialized;
		PieceList	m_Pieces;
};

#endif // __CHASSIS_BUTE_MGR_H__
