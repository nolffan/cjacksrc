//////////////////////////////////////////////////////////////////////////////
// CRC-32 calculation routines

#include "stdafx.h"

#include "CRC32.h"
#include "CommonUtilities.h"
#include <stdio.h>

using namespace CRC32;

// Table management class
class CRC32Table
{
public:
	CRC32Table();
	enum { k_Polynomial = 0xedb88320L };
	enum { k_CRCTableSize = 256 };
	uint32 m_Table[k_CRCTableSize];
	inline void Calc(uint32 &nCurCRC, uint8 nData) { 
		nCurCRC = m_Table[(nCurCRC ^ nData) & 0xFF] ^ (nCurCRC >> 8);
	}
};

CRC32Table::CRC32Table()
{
	for (uint32 nCurEntry = 0; nCurEntry < k_CRCTableSize; ++nCurEntry)
	{
		uint32 nAccumulator = nCurEntry;
		for (uint32 nModEntry = 0; nModEntry < 8; ++nModEntry)
		{
			if (nAccumulator & 1)
				nAccumulator = k_Polynomial ^ (nAccumulator >> 1);
			else
				nAccumulator = nAccumulator >> 1;
		}
		m_Table[nCurEntry] = nAccumulator;
	}
}

// The global, static table
CRC32Table g_CRCTable;


// RETAIL: CShell 0x1002ffd0, Object 0x100bb8e0: Contract J.A.C.K. goes on
// to CRC the upper-cased file title of pName after the file's bytes, so a
// file the client found by its module path CRCs as the server's copy does.
uint32 CRC32::CalcFileCRC(const char *pFilename, const char *pName)
{
	FILE *pFile = fopen(pFilename, "rb");
	if (!pFile)
		return 0;

	uint32 nAccumulator = 0xFFFFFFFF;
	while (!feof(pFile))
	{
		int nNextChar = fgetc(pFile);
		if (nNextChar < 0)
			break;
		g_CRCTable.Calc(nAccumulator, (uint8)nNextChar);
	}

	// cjacksrc: Contract J.A.C.K. continues over the upper-cased file title
	char szTitle[_MAX_PATH * 2];
	_splitpath(pName, NULL, NULL, szTitle, NULL);
	_strupr(szTitle);
	for (const char *pCur = szTitle; *pCur; ++pCur)
		g_CRCTable.Calc(nAccumulator, (uint8)*pCur);

	return nAccumulator ^ 0xFFFFFFFF;
}

// RETAIL: CShell 0x100300a0, Object 0x100bb9b0.
uint32 CRC32::CalcFileCRC(const char *pFilename)
{
	return CalcFileCRC(pFilename, pFilename);
}

uint32 CRC32::CalcDataCRC(const void *pData, uint32 nDataSize)
{
	if (!nDataSize)
		return 0;

	uint32 nAccumulator = 0xFFFFFFFF;
	const char *pCharData = (const char *)pData;
	uint32 nIndex = 0;
	// For those unfamiliar with the technique, this is an unrolled loop that
	// handles the remaining loop iterations without needing an extra loop at the end
	// Not that this was really necessary from testing, but I've always wanted to do 
	// this somewhere.. ;D
	switch (8 - (nDataSize & 7))
	{
		case 8 : 
		while (nIndex < nDataSize) {
			g_CRCTable.Calc(nAccumulator, pCharData[nIndex++]);
		case 1 : 			
			g_CRCTable.Calc(nAccumulator, pCharData[nIndex++]);
		case 2 : 			
			g_CRCTable.Calc(nAccumulator, pCharData[nIndex++]);
		case 3 : 			
			g_CRCTable.Calc(nAccumulator, pCharData[nIndex++]);
		case 4 : 			
			g_CRCTable.Calc(nAccumulator, pCharData[nIndex++]);
		case 5 : 			
			g_CRCTable.Calc(nAccumulator, pCharData[nIndex++]);
		case 6 : 			
			g_CRCTable.Calc(nAccumulator, pCharData[nIndex++]);
		case 7 : 			
			g_CRCTable.Calc(nAccumulator, pCharData[nIndex++]);
		}
	}
	return nAccumulator ^ 0xFFFFFFFF;
}

uint32 CRC32::CalcRezFileCRC(const char *pFilename)
{
	if (!g_pLTBase)
		return 0;
	
	ILTStream *pFileStream;
	LTRESULT nOpenResult = g_pLTBase->OpenFile(const_cast<char*>(pFilename), &pFileStream);
	if (nOpenResult != LT_OK)
		return 0;

	ASSERT(pFileStream);

	uint32 nAccumulator = 0xFFFFFFFF;

	for (uint32 nFileSize = pFileStream->GetLen(); nFileSize; --nFileSize)
	{
		uint8 nNextChar;
		(*pFileStream) >> nNextChar;
		g_CRCTable.Calc(nAccumulator, nNextChar);
	}

	pFileStream->Release();

	// RETAIL: CShell 0x10030271, Object 0x100bbb81: and the file's title.
	// cjacksrc: Contract J.A.C.K. continues over the upper-cased file title
	char szTitle[_MAX_PATH * 2];
	_splitpath(pFilename, NULL, NULL, szTitle, NULL);
	_strupr(szTitle);
	for (const char *pCur = szTitle; *pCur; ++pCur)
		g_CRCTable.Calc(nAccumulator, (uint8)*pCur);

	return nAccumulator ^ 0xFFFFFFFF;
}

