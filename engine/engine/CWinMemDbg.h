#pragma once

#include <crtdbg.h>
#include "CSysChrono.h"

class CWinMemDbg
{
public:
	CWinMemDbg() 
	{
		MPS = 60;
		LastTime = CSysChrono::getTimePointNow();
	}

	void beginCheckPoint();

	bool endCheckPoint();

	void outputDifference(const char* funcname);

	void setAllocHook(bool enable, int nMaxAlloc = 1000);

	void outputMaxMemoryUsed();

	const char* getBlockAllocInfo(const void* address);

	void registerFrame(TIME_POINT now);
	float getMPS() const { return MPS; }

private:
	_CrtMemState		OldState;
	_CrtMemState		NewState;
	_CrtMemState		DiffState;

	TIME_POINT LastTime;
	float MPS;

	char	m_BlockInfo[1024];
};
