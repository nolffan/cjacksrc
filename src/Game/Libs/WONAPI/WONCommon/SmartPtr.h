// cjacksrc: WONAPI's reference counting (WONCommon/SmartPtr.h), the part
// the CD key check uses, written for this project after the retail
// GameSpy.dll (RefCount's vtable 0x100176ac, ConstSmartPtr's assignment
// 0x10014990).
#ifndef __WON_SMARTPTR_H__
#define __WON_SMARTPTR_H__
#include "WONShared.h"

#ifdef  _MSC_VER
#pragma pack(push,8) // ensure proper alignment on the mRefCount member
#endif  /* _MSC_VER */

namespace WONAPI
{

class RefCount
{
private:
	mutable long mRefCount;

protected:
	// deleted only through Release
	virtual ~RefCount() {}

public:
	RefCount() : mRefCount(0) {}

	const RefCount* CreateRef() const
	{
		InterlockedIncrement(&mRefCount);
		return this;
	}

	void Release()
	{
		if(InterlockedDecrement(&mRefCount)<=0)
			delete this;
	}

	RefCount(const RefCount &) : mRefCount(0) {}
	RefCount& operator=(const RefCount &) { return *this; }
};

// A reference counted pointer with const access to its object.
template <class T> class ConstSmartPtr
{
protected:
	T *mObject;

public:
	ConstSmartPtr() : mObject(NULL) {}
	ConstSmartPtr(const T* theObject) : mObject((T*)(theObject?theObject->CreateRef():NULL)) {}
	ConstSmartPtr(const ConstSmartPtr& theCopy) : mObject((T*)(theCopy.mObject?theCopy.mObject->CreateRef():NULL)) {}
	~ConstSmartPtr() { if(mObject!=NULL) mObject->Release(); }

	const T* operator->() const { return mObject; }
	operator const T*() const { return mObject; }

	const T* operator=(const T* thePtr)
	{
		if(mObject!=thePtr) // prevent self-assignment
		{
			if(mObject!=NULL) mObject->Release();
			mObject = (T*)(thePtr?thePtr->CreateRef():NULL);
		}
		return thePtr;
	}

	const ConstSmartPtr& operator=(const ConstSmartPtr& theCopy)
	{
		operator=(theCopy.get());
		return *this;
	}

	const T* get() const { return mObject; }
};

}; // namespace WONAPI

#ifdef  _MSC_VER
#pragma pack(pop)
#endif  /* _MSC_VER */

#endif
