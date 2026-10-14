// cjacksrc: WONAPI::CDKey, the part GameSpy.dll links, written for this
// project from the retail GameSpy.dll (0x10014740..0x10014cd6).
//
// A key is 20 characters in the pattern CVCN CVCN CVCN CVCN NNNN --
// consonants (4 bits), vowels (2 bits), digits (3 bits) -- with dashes,
// spaces and tabs ignored. The 48 bits are packed, least significant first,
// into an 8-byte buffer; byte 3 must equal the middle 8 bits of a CRC-16 of
// the product name and bytes 0-2 and 4-7.
#include <string.h>
#include <ctype.h>
#include "CDKey.h"
#include "WONCommon/CRC.h"

using namespace WONAPI;

namespace
{
	const char*         IGNORED_CHARS  = "- \t";
	const char*         KEY_PATTERN    = "CVCNCVCNCVCNCVCNNNNN";
	const unsigned int  KEY_LENGTH     = strlen(KEY_PATTERN);
}

// RETAIL: 0x10014740. Ors nNumBits of nValue into pBuffer at bit nBitPos.
static void AddBits(char* pBuffer, int& nBitPos, int nValue, int nNumBits)
{
	int nByte = nBitPos/8;
	int nShift = nBitPos%8;

	pBuffer[nByte] |= (nValue<<nShift)&0xff;

	int nSpill = nShift + nNumBits - 8;
	if(nSpill>0)
		pBuffer[nByte+1] |= (nValue>>(nNumBits - nSpill))&0xff;

	nBitPos+=nNumBits;
}

// RETAIL: 0x10014790. 16 consonants: no H, K, Q, V or Y.
static bool AddConsonant(char* pBuffer, int& nBitPos, char cChar)
{
	int nValue;

	switch (toupper(cChar))
	{
		case 'B': nValue = 0;  break;
		case 'C': nValue = 1;  break;
		case 'D': nValue = 2;  break;
		case 'F': nValue = 3;  break;
		case 'G': nValue = 4;  break;
		case 'J': nValue = 5;  break;
		case 'L': nValue = 6;  break;
		case 'M': nValue = 7;  break;
		case 'N': nValue = 8;  break;
		case 'P': nValue = 9;  break;
		case 'R': nValue = 10; break;
		case 'S': nValue = 11; break;
		case 'T': nValue = 12; break;
		case 'W': nValue = 13; break;
		case 'X': nValue = 14; break;
		case 'Z': nValue = 15; break;
		default:
			return false;
	}

	AddBits(pBuffer,nBitPos,nValue,4);
	return true;
}

// RETAIL: 0x100148a0. 4 vowels: no I or O.
static bool AddVowel(char* pBuffer, int& nBitPos, char cChar)
{
	int nValue;

	switch (toupper(cChar))
	{
		case 'A': nValue = 0; break;
		case 'E': nValue = 1; break;
		case 'U': nValue = 2; break;
		case 'Y': nValue = 3; break;
		default:
			return false;
	}

	AddBits(pBuffer,nBitPos,nValue,2);
	return true;
}

// RETAIL: 0x10014930. 8 digits: '2' to '9'.
static bool AddDigit(char* pBuffer, int& nBitPos, char cChar)
{
	if(!isdigit(cChar) || cChar<'2')
		return false;

	AddBits(pBuffer,nBitPos,cChar - '2',3);
	return true;
}

// RETAIL: 0x10014970
void CDKey::Invalidate()
{
	mIsValid = false;
}

// RETAIL: 0x10014980
void CDKey::SetProductString(const std::string& sProduct)
{
	Invalidate();
	mProduct = sProduct;
}

// RETAIL: 0x100149f0
CDKey::CDKey()
{
	mIsValid = false;
}

CDKey::CDKey(const std::string& sProduct) : mProduct(sProduct)
{
	mIsValid = false;
}

// RETAIL: 0x10014a30
bool CDKey::CalcKeyFromBuffer(const char* pBuffer)
{
	mKey = new ByteBuffer(pBuffer,8);

	CRC16 crc;
	crc.Put(mProduct.data(),mProduct.length());
	crc.Put(pBuffer,3);
	crc.Put(pBuffer+4,4);

	char cCheck = (crc.Get() & 0x0fff) >> 4;
	mIsValid = (cCheck == pBuffer[3]);
	return mIsValid;
}

// RETAIL: 0x10014b00
bool CDKey::Init(const std::string& sKey)
{
	mIsValid = false;
	mKey = NULL;

	std::string sChars;
	unsigned int nPos = 0;
	while(nPos < sKey.length())
	{
		unsigned char cChar = sKey[nPos];
		if(strchr(IGNORED_CHARS,cChar)==NULL)
			sChars+=cChar;
		nPos++;
	}

	if (sChars.size() != KEY_LENGTH)
	{
		mIsValid = false;
		return false;
	}

	char aBuffer[8];
	memset(aBuffer,0,8);

	int nBitPos = 0;
	for (unsigned int i=0; i < KEY_LENGTH; i++)
	{
		bool bOk = true;
		switch (KEY_PATTERN[i])
		{
			case 'C': bOk = AddConsonant(aBuffer, nBitPos, sChars[i]); break;
			case 'V': bOk = AddVowel(aBuffer, nBitPos, sChars[i]);     break;
			case 'N': bOk = AddDigit(aBuffer, nBitPos, sChars[i]);     break;
		}

		if (!bOk)
		{
			mIsValid = false;
			return false;
		}
	}

	return CalcKeyFromBuffer(aBuffer);
}

// Where WONAPI keeps keys in the registry (its registry functions are not
// linked; the path is, as in the retail image)
const char* CDKey::mCDKeyPath = "SOFTWARE\\Sierra\\CDKeys";
