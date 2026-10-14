// stdafx.h: ltmem's precompiled header (the 2006 Jupiter tree's
// libs/ltmem/stdafx.h, Developer Studio's static library template). Every
// source file of the library includes it first and compiles /Yu through it,
// which, beyond the time it saves, decides the order of the header
// constants each object keeps in .bss: the retail sound driver has ltmem's
// in the order they are defined (0x1002d4e8..0x1002d4fc), as a compile
// through a precompiled header lays them out.

#if !defined(AFX_STDAFX_H__FD67504D_D265_4C7D_9845_56DC0914F5A5__INCLUDED_)
#define AFX_STDAFX_H__FD67504D_D265_4C7D_9845_56DC0914F5A5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#endif // !defined(AFX_STDAFX_H__FD67504D_D265_4C7D_9845_56DC0914F5A5__INCLUDED_)
