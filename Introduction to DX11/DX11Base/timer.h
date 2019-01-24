#pragma once
#include <stdint.h>
#include <Windows.h>


class CTimer
{
public:
	CTimer();
	virtual ~CTimer();

	void reset();
	void start();
	void stop();
	float get_delta();
	float get_total();

protected:
	double _seconds_per_count;
	int64_t _start_time;
	int64_t _stop_time;
};
