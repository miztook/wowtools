#pragma once

#include <stdint.h>
#include <chrono>
#include <ctime>
#include <cassert>

using namespace std::chrono;

struct SDateTime
{
	int32_t	year;		//	year since 1900
	int32_t	month;		//	[0, 11]
	int32_t	day;		//	[1, 31]
	int32_t	hour;		//	[0, 23]
	int32_t	minute;		//	[0, 59]
	int32_t	second;		//	[0, 59]
	int32_t	wday;		//	day of week, [0, 6]
};

using TIME_POINT = high_resolution_clock::time_point;

class CSysChrono
{
public:
	static TIME_POINT getTimePointNow()
	{
		return high_resolution_clock::now();
	}

	static uint32_t getDurationMilliseconds(const TIME_POINT& now, const TIME_POINT& last)
	{
		auto delta = duration_cast<milliseconds>(now - last).count();
		assert(delta >= 0);
		return (uint32_t)delta;
	}

	static uint32_t getDurationMilliseconds(const TIME_POINT& last)
	{
		auto delta = duration_cast<milliseconds>(getTimePointNow() - last).count();
		assert(delta >= 0);
		return (uint32_t)delta;
	}

	static uint32_t getDurationMicroseconds(const TIME_POINT& now, const TIME_POINT& last)
	{
		auto delta = duration_cast<microseconds>(now - last).count();
		assert(delta >= 0);
		return (uint32_t)delta;
	}

	static uint32_t getDurationMicroseconds(const TIME_POINT& last)
	{
		auto delta = duration_cast<microseconds>(getTimePointNow() - last).count();
		assert(delta >= 0);
		return (uint32_t)delta;
	}

	static time_t GetTimeSince1970()
	{
		return std::time(nullptr);
	}

	static void GMTime(time_t _time, SDateTime& atm)
	{
		time_t t = (time_t)_time;
		tm* ptm = ::gmtime(&t);

		atm.year = ptm->tm_year;
		atm.month = ptm->tm_mon;
		atm.day = ptm->tm_mday;
		atm.hour = ptm->tm_hour;
		atm.minute = ptm->tm_min;
		atm.second = ptm->tm_sec;
		atm.wday = ptm->tm_wday;
	}

	static void LocalTime(time_t _time, SDateTime& atm)
	{
		tm* ptm = std::localtime(&_time);

		atm.year = ptm->tm_year;
		atm.month = ptm->tm_mon;
		atm.day = ptm->tm_mday;
		atm.hour = ptm->tm_hour;
		atm.minute = ptm->tm_min;
		atm.second = ptm->tm_sec;
		atm.wday = ptm->tm_wday;
	}

	static time_t TimeLocal(const SDateTime& atm)
	{
		tm _tm;
		_tm.tm_year = atm.year;
		_tm.tm_mon = atm.month;
		_tm.tm_mday = atm.day;
		_tm.tm_hour = atm.hour;
		_tm.tm_min = atm.minute;
		_tm.tm_sec = atm.second;
		_tm.tm_isdst = 0;
		_tm.tm_wday = 0;
		_tm.tm_yday = 0;

		return std::mktime(&_tm);
	}

	static void GetCurGMTime(SDateTime& atm)
	{
		time_t t = ::time(nullptr);
		GMTime(t, atm);
	}

	static void GetCurLocalTime(SDateTime& atm)
	{
		time_t t = ::time(nullptr);
		LocalTime(t, atm);
	}
};