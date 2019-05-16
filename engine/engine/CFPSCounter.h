#pragma once

#include "base.h"
#include "function.h"
#include "CSysChrono.h"

class CFPSCounter
{
public:
	CFPSCounter()
	{
		FPS = 60;
		LastTime = CSysChrono::getTimePointNow();
		FrameCounted = 0;
	}

	//
	float getFPS() const { return FPS; }
	void registerFrame(TIME_POINT now);

private:
	float FPS;
	TIME_POINT LastTime;
	uint32_t FrameCounted;
};

inline void CFPSCounter::registerFrame(TIME_POINT now)
{
	++FrameCounted;

	const uint32_t milli = CSysChrono::getDurationMilliseconds(now, LastTime);

	if (milli >= 1500)
	{
		const float invMilli = reciprocal_((float)milli);
		FPS = (1000 * (float)FrameCounted) * invMilli;
		FrameCounted = 0;
		LastTime = now;
	}
}

