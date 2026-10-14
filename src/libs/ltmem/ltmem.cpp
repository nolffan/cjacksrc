// ltmem.cpp: the LithTech memory interface (Engine/sdk/inc/ltmem.h) in its
// plain form, without the engine's own heap (the 2006 Jupiter tree's
// libs/ltmem/ltmem.cpp without USELTMEM): allocation and release go
// straight to the C runtime.
//
// Contract J.A.C.K.'s sound driver links it before msacm32.lib, ahead of the
// MSACM32 import thunks (retail 0x100085f0..0x10008632). Compiled /Ox, which
// unlike /O2 does not make each function a COMDAT, its code is one section
// the link keeps whole: LTMemAlloc, LTMemFree and LTMemReAlloc (each a jump
// to the runtime's through its import, /MD), then the initializer object's
// constructor (`mov eax,ecx; ret`) and destructor (`ret`), as Visual C++
// .NET 2003 orders them, 16 bytes apart. LTMemInit and LTMemTerm are not
// there: without USELTMEM this file has neither them nor the calls. (The
// 2006 file defines the initializer first; here it follows the functions,
// which puts its constructor and destructor after them as retail has
// them.)

#include <stdlib.h>
#include "ltmem.h"

// true if lt mem system is initialized (never, in this form; ltmemstats.cpp
// reads it)
bool g_bLTMemInitialized = false;

///////////////////////////////////////////////////////////////////////////////////////////
// LTMem functions
///////////////////////////////////////////////////////////////////////////////////////////

// LTMem Allocation function
void* LTMemAlloc(uint32 nSize)
{
	return malloc(nSize);
}

// LTMem Free function
void LTMemFree(void* pMem)
{
	free(pMem);
}

// LTMem ReAllocation function
void* LTMemReAlloc(void* pOldMem, uint32 nNewSize)
{
	return realloc(pOldMem, nNewSize);
}


///////////////////////////////////////////////////////////////////////////////////////////
// Initializer class for ltmem
///////////////////////////////////////////////////////////////////////////////////////////
class CLTMemInitialize
{
public:
	CLTMemInitialize();
	~CLTMemInitialize();
} g_LTMemInitialize;

CLTMemInitialize::CLTMemInitialize()
{
#ifdef USELTMEM
	LTMemInit();
#endif
}

CLTMemInitialize::~CLTMemInitialize()
{
#ifdef USELTMEM
	LTMemTerm();
#endif
}
