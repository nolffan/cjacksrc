// ----------------------------------------------------------------------- //
//
// MODULE  : SharedMovement.cpp
//
// PURPOSE : Shared movement implementation.
//
// CREATED : 8/27/99
//
// (c)	1999 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#include "stdafx.h"
#include "SharedMovement.h"

// RETAIL: CShell.dll has these too (0x100ecc10, 0x100ecc30).

char* aPPMStrings[] =
{
	"Normal",
	"Vehicle",		// RETAIL: CShell .data 0x1014f8c0, Object 0x10230878
	"Lure"
	// RETAIL: Contract J.A.C.K. has no light cycle.
};

char* GetPropertyNameFromPlayerPhysicsModel(PlayerPhysicsModel ePPModel)
{
	if (PPM_FIRST <= ePPModel && ePPModel < PPM_NUM_MODELS)
	{
		return aPPMStrings[ePPModel];
	}

    return LTNULL;
}


PlayerPhysicsModel GetPlayerPhysicsModelFromPropertyName(char* pName)
{
	if (!pName) return PPM_NORMAL;

	for (int i = 0; i < PPM_NUM_MODELS; i++)
	{
		if (_stricmp(aPPMStrings[i], pName) == 0)
		{
			return (PlayerPhysicsModel)i;
		}
	}

	return PPM_NORMAL;
}

