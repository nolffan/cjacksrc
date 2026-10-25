// g++ -std=c++98 -Wall -Wextra -pedantic -DCJACK_MODERN
//     -Isrc/Engine/sdk/inc build/tests/ltreference.cpp -o /tmp/ltreference
#include <cassert>
typedef unsigned int uint32;
#define ASSERT assert
#include "iltrefcount.h"

class Counted : public ILTRefCount
{
public:
	Counted(int &freed) : m_Freed(freed) {}
private:
	virtual void Free() { ++m_Freed; delete this; }
	int &m_Freed;
};

int main()
{
	int freed = 0;
	CLTReference<Counted> saved;
	{
		CLTReference<Counted> local = new Counted(freed);
		assert(local->GetRefCount() == 1);
		{
			CLTReference<Counted> copied(local);
			assert(local->GetRefCount() == 2);
			saved = copied;
			assert(local->GetRefCount() == 3);
			saved = saved;
			saved = local;
			assert(local->GetRefCount() == 3);
		}
		assert(local->GetRefCount() == 2);
	}
	assert(freed == 0);
	assert(saved->GetRefCount() == 1);
	{
		CLTReference<Counted> replacement = new Counted(freed);
		saved = replacement;
		assert(freed == 1);
		assert(replacement->GetRefCount() == 2);
	}
	saved = 0;
	assert(freed == 2);
	CLTReference<Counted> empty(saved);
	saved = empty;
	assert(saved == 0);
}
