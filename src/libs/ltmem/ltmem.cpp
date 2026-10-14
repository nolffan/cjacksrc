// ltmem.cpp: the LithTech memory interface (Engine/sdk/inc/ltmem.h) in its
// plain form, without the engine's own heap: the 2006 Jupiter tree's
// libs/ltmem/ltmem.cpp without USELTMEM, so allocation and release go
// straight to the C runtime.
//
// Contract J.A.C.K.'s sound driver links it before msacm32.lib, ahead of the
// MSACM32 import thunks (retail 0x100085d0..0x10008632). Compiled /Ox, which
// unlike /O2 does not make each function a COMDAT, its code is one section
// the link keeps whole, in the order Visual C++ .NET 2003 finishes the
// functions: the initializer's constructor and destructor call LTMemInit
// and LTMemTerm, defined after them, so they wait for the end of the file;
// LTMemInit and LTMemTerm, empty here, are a `ret` each (0x100085d0,
// 0x100085e0); LTMemAlloc, LTMemFree and LTMemReAlloc a jump to the
// runtime's through its import (/MD); then the constructor (`mov eax,ecx;
// ret`, LTMemInit inlined) and the destructor (`ret`), all 16 bytes apart.
// Nothing calls any of them but the initializer's own.

#include "stdafx.h"
#ifdef _WIN32
#include "windows.h"
#endif
#include <stdlib.h>
#include "ltmem.h"

///////////////////////////////////////////////////////////////////////////////////////////
// ltheap global variables
///////////////////////////////////////////////////////////////////////////////////////////

// true if lt mem system is initialized (never, in this form; ltmemstats.cpp
// reads it)
bool g_bLTMemInitialized = false;

// critical section to make heap thread safe. RETAIL: 0x1002d4d0, 0x18
// bytes of .bss between g_bLTMemInitialized and the header constants
// (0x1002d4e8): defined although nothing without USELTMEM uses it (the 2006
// file has it under USELTMEM).
CRITICAL_SECTION g_LTMemCriticalSection;

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
	LTMemInit();
}

CLTMemInitialize::~CLTMemInitialize()
{
	LTMemTerm();
}


///////////////////////////////////////////////////////////////////////////////////////////
// LTMem functions
///////////////////////////////////////////////////////////////////////////////////////////

// Initialize: nothing to do without the engine's heap (RETAIL: 0x100085d0)
void LTMemInit()
{
}


// Terminate (RETAIL: 0x100085e0)
void LTMemTerm()
{
}


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


// LTMem system memory re-size function
void* LTMemReAlloc(void* pOldMem, uint32 nNewSize)
{
	return realloc(pOldMem, nNewSize);
}
