#include "timer.h"


CTimer::CTimer()
{
	int64_t frequency;
	QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
	_seconds_per_count = 1.0 / (double)frequency;
}

CTimer::~CTimer()
{
}

void CTimer::reset()
{
	int64_t frequency;
	QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
	_seconds_per_count = 1.0 / (double)frequency;

	_start_time = 0;
	_stop_time = 0;
}

void CTimer::start()
{
	QueryPerformanceCounter((LARGE_INTEGER*)&_start_time);

	_stop_time = 0;
}

void CTimer::stop()
{
	QueryPerformanceCounter((LARGE_INTEGER*)&_stop_time);
}

float CTimer::get_delta()
{
	int64_t delta;
	QueryPerformanceCounter((LARGE_INTEGER*)&delta);

	return (float)((delta - _start_time) * _seconds_per_count);
}

float CTimer::get_total()
{
	return (float)((_stop_time - _start_time) * _seconds_per_count);
}
