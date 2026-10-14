// cjacksrc: WONAPI's ByteBuffer (WONCommon/ByteBuffer.h), written for this
// project after the retail GameSpy.dll (0x10014d70..0x10014e5b).
#ifndef __WON_BYTEBUFFER_H__
#define __WON_BYTEBUFFER_H__
#include "WONShared.h"
#include "SmartPtr.h"

namespace WONAPI
{

class ByteBuffer : public WONAPI::RefCount
{
private:
	char *mData;
	unsigned long mDataLen;

public:
	ByteBuffer(const void *theData, unsigned long theLen, bool takeOwnership=false);

	const char* data() const { return this?mData:NULL; }
	unsigned long length() const { return this?mDataLen:0; }

protected:
	virtual ~ByteBuffer();
};

typedef ConstSmartPtr<ByteBuffer> ByteBufferPtr;

}; // namespace WONAPI

#endif
