// cjacksrc: WONAPI's CDKey (WONAuth/CDKey.h), the part GameSpy.dll links:
// a CD key's form and checksum, checked offline. Written for this project
// after the retail GameSpy.dll (0x10014740..0x10014cd6; RTTI names the class
// WONAPI::CDKey, its vtable is 0x100176a0).
#ifndef __WON_CDKEY_H__
#define __WON_CDKEY_H__
#include "WONShared.h"

#include "WONCommon/ByteBuffer.h"
#include <string>

namespace WONAPI
{

class CDKey
{
protected:
	std::string				mProduct;     // the product the key is for
	ByteBufferPtr			mKey;         // the key's 8 bytes, once parsed
	bool					mIsValid;

	static const char*		mCDKeyPath;

public:
	CDKey();
	explicit CDKey(const std::string& sProduct);

	// Parses and checks a key; true if it is valid for the product
	bool Init(const std::string& sKey);

	const ByteBuffer* GetRaw() const { return mKey; }

	// Sets the product (and forgets the key's validity)
	void SetProductString(const std::string& sProduct);
	const std::string& GetProductString() const { return mProduct; }

	bool IsValid() const { return mIsValid; }
	void Invalidate();

protected:
	bool CalcKeyFromBuffer(const char* pBuffer);
	// for WONAPI's encrypted storage of keys (not linked); the retail
	// vtable has them (0x10014a10, 0x10014a20)
	virtual void NotifyUsedOldMethodToDecrypt() { }
	virtual bool GetUseNewEncryptMethod() const { return true; }
};

}; // namespace WONAPI

#endif
