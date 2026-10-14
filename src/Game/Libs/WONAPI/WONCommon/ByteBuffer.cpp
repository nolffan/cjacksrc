// cjacksrc: WONAPI's ByteBuffer, written for this project after the retail
// GameSpy.dll (0x10014d70..0x10014e5b).
#include <string.h>
#include "ByteBuffer.h"
using namespace WONAPI;

ByteBuffer::ByteBuffer(const void *theData, unsigned long theLen, bool takeOwnership)
{
	if(takeOwnership)
	{
		mData = (char*)theData;
		mDataLen = theLen;
	}
	else
	{
		mData = new char[theLen];
		mDataLen = theLen;

		if(theLen>0)
			memcpy(mData,theData,theLen);
	}
}

ByteBuffer::~ByteBuffer()
{
	delete[] mData;
}
