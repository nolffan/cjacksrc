// counter.cpp: the engine's performance-counter module, linked into the
// sound driver. The driver calls none of the counter functions, but
// compiled /Ox (no /Gy, so no COMDATs to drop) they are all in the retail
// image, at its start (0x10001000..0x100011ab), in the order the compiler
// finished them -- the constructor, which calls functions defined after
// it, last -- with the global divisor's initializer among the others
// (0x10008db0). They pull in the 64-bit division and multiplication
// helpers (msvcrt.lib's ulldiv, lldiv and llmul).

#include <windows.h>
#include "ltbasedefs.h"
#include "counter.h"

static unsigned __int64 g_CountDiv = 1;

// The high-resolution counter's divisor: always 1 (counts are not scaled to
// microseconds).
class CountDivSetter
{
public:
	CountDivSetter()
	{
		LARGE_INTEGER perSec;

		QueryPerformanceFrequency(&perSec);
		g_CountDiv = 1;
	}
};
static CountDivSetter __g_CountDivSetter;

unsigned long cnt_NumTicksPerSecond()
{
	LARGE_INTEGER perSec;

	QueryPerformanceFrequency(&perSec);
	return (unsigned long)(perSec.QuadPart / g_CountDiv);
}

CounterFinal::CounterFinal(unsigned long startMode)
{
	if(startMode == CSTART_MICRO)
		StartMicro();
	else if(startMode == CSTART_MILLI)
		StartMS();
}

void CounterFinal::StartMS()
{
	LARGE_INTEGER *pInt;

	pInt = (LARGE_INTEGER*)m_Data;
	QueryPerformanceCounter(pInt);
}

unsigned long CounterFinal::EndMS()
{
	LARGE_INTEGER curCount, *pInCount, perSec;
	LONGLONG timeElapsed;

	pInCount = (LARGE_INTEGER*)m_Data;
	QueryPerformanceCounter(&curCount);

	QueryPerformanceFrequency(&perSec);
	timeElapsed = curCount.QuadPart - pInCount->QuadPart;
	return (unsigned long)((timeElapsed*(LONGLONG)1000) / perSec.QuadPart);
}

unsigned long CounterFinal::CountMS()
{
	return EndMS();
}

void CounterFinal::StartMicro()
{
	LARGE_INTEGER *pInt;

	pInt = (LARGE_INTEGER*)m_Data;
	QueryPerformanceCounter(pInt);
}

unsigned long CounterFinal::EndMicro()
{
	LARGE_INTEGER curCount, *pInCount;

	pInCount = (LARGE_INTEGER*)m_Data;
	QueryPerformanceCounter(&curCount);

	return (unsigned long)((curCount.QuadPart - pInCount->QuadPart) / g_CountDiv);
}

unsigned long CounterFinal::CountMicro()
{
	return EndMicro();
}

void cnt_StartCounterFinal(CounterFinal &cCounter)
{
	cCounter.StartMicro();
}

unsigned long cnt_EndCounterFinal(CounterFinal &cCounter)
{
	return cCounter.EndMicro();
}
